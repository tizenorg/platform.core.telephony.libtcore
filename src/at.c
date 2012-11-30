/*
 * libtcore
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Ja-young Gu <jygu@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <glib.h>

#include "tcore.h"
#include "hal.h"
#include "queue.h"
#include "user_request.h"
#include "at.h"
#include "core_object.h"

#define NUM_ELEMS(x) (sizeof(x)/sizeof(x[0]))
#define MODE_HEX 	0
#define MODE_BIN	1

#define CR '\r'
#define LF '\n'

#define MAX_AT_RESPONSE    255

typedef gboolean (*rfs_hook_cb) (const char *data);

struct tcore_at_type {
	TcoreHal *hal;

	enum tcore_at_command_type cmd_type;

	GHashTable *unsolicited_table;

	struct tcore_at_request *req;
	struct tcore_at_response *resp;

	unsigned int buf_size;
	char *buf;
	char *buf_read_pos;
	char *buf_write_pos;

	gboolean pdu_status;
	struct _notification *pdu_noti;
	GSList *pdu_lines;

	rfs_hook_cb rfs_hook;
	gboolean data_mode;
};

struct _notification_callback {
	TcoreATNotificationCallback callback;
	void *user_data;
};

struct _notification {
	gboolean type_pdu;
	GSList *callbacks;
};

/**
 * returns 1 if line is a final response indicating success
 * See 27.007 annex B
 */
static const char *list_final_responses_success[] = {
	"OK",
};

/**
 * returns 1 if line is a final response indicating error
 * See 27.007 annex B
 */
static const char *list_final_responses_error[] = {
	"ERROR",
	"+CMS ERROR:",
	"+CME ERROR:",
	"NO ANSWER",
	"NO DIALTONE",
};

static int _check_final_response(const char *line)
{
	unsigned int i;

	for (i = 0; i < NUM_ELEMS(list_final_responses_success); i++) {
		if (g_str_has_prefix(line, list_final_responses_success[i])) {
			return 1;
		}
	}

	for (i = 0; i < NUM_ELEMS(list_final_responses_error); i++) {
		if (g_str_has_prefix(line, list_final_responses_error[i])) {
			return 2;
		}
	}

	return 0;
}


static char* _find_next_EOL(char *cur)
{
	if (cur[0] == '>' && cur[1] == ' ' && cur[2] == '\0') {
		/* SMS prompt character...not \r terminated */
		return cur + 2;
	}

	// Find next newline
	while (*cur != '\0' && !((*cur == CR) && (*(cur + 1) == LF)))      // avoid issue when AT response is having <CR> in between before end of line( <CR> <LF>) or '/0'
		cur++;

	return *cur == '\0' ? NULL : cur;
}

static struct tcore_at_response* _response_new()
{
	struct tcore_at_response *resp;

	resp = calloc(sizeof(struct tcore_at_response), 1);
	if (!resp)
		return NULL;

	return resp;
}

static void _response_free(struct tcore_at_response *resp)
{
	if (!resp)
		return;

	if (resp->lines) {
		g_slist_free_full(resp->lines, g_free);
	}

	if (resp->final_response)
		free(resp->final_response);

	free(resp);
}


static void _response_add(struct tcore_at_response *resp,
						  const char *line)
{
	if (!resp || !line)
		return;

	dbg("current lines = %d", g_slist_length(resp->lines));

	resp->lines = g_slist_append(resp->lines, strdup(line));
}

static void _emit_pending_response(TcoreAT *at)
{
	TcorePending *p;

	if (!at)
		return;

	tcore_at_request_free(at->req);
	at->req = NULL;

	p = tcore_queue_pop(tcore_hal_ref_queue(at->hal));
	if (!p) {
		dbg("no pending");
	}

	tcore_pending_emit_response_callback(p, sizeof(TcoreATResponse *), at->resp);
	tcore_user_request_unref(tcore_pending_ref_user_request(p));
	tcore_pending_free(p);

	_response_free(at->resp);
	at->resp = NULL;
}

static void _emit_unsolicited_message(TcoreAT *at, const char *line)
{
	struct _notification *noti = NULL;
	struct _notification_callback *item = NULL;
	GSList *p;
	GHashTableIter iter;
	gpointer key = NULL, value;
	gboolean ret;
	GSList *data = NULL;

	if (!at || !line)
		return;

	dbg("at->pdu_status  %d line 0x%x at->data_mode %d", at->pdu_status, line, at->data_mode);
	if (at->pdu_status == FALSE) {
		g_hash_table_iter_init(&iter, at->unsolicited_table);

		while (g_hash_table_iter_next(&iter, &key, &value)) {
			if (!g_str_has_prefix(line, key))
				continue;

			noti = value;
			break;
		}

		if (!noti)
			return;

		if (noti->type_pdu == TRUE) {
			at->pdu_status = TRUE;
			at->pdu_noti = noti;
			at->pdu_lines = g_slist_append(NULL, g_strdup(line));
			dbg("PDU mode");
			return;
		}

		if (at->data_mode == MODE_BIN) {
			at->pdu_lines = g_slist_append(at->pdu_lines, (gpointer)line);
			data = at->pdu_lines;
		} else {
			data = g_slist_append(NULL, g_strdup(line));
		}
	}
	else {
		noti = at->pdu_noti;
		at->pdu_status = FALSE;
		at->pdu_noti = NULL;

		if (at->data_mode == MODE_BIN) {
			dbg("Binary mode");
			at->pdu_lines = g_slist_append(at->pdu_lines, (gpointer)line);
			dbg("at->pdu_lines: 0x%x", at->pdu_lines);
		} else {
			at->pdu_lines = g_slist_append(at->pdu_lines, g_strdup(line));
		}
		data = at->pdu_lines;
	}

	p = noti->callbacks;
	while (p) {
		item = p->data;
		if (!item) {
			p = p->next;
			continue;
		}

		ret = item->callback(at, data, item->user_data);
		if (ret == FALSE) {
			p = p->next;
			noti->callbacks = g_slist_remove(noti->callbacks, item);
			continue;
		}

		p = p->next;
	}
	dbg(" Free the list");
	if (at->data_mode != MODE_BIN) {
		g_slist_free_full(data, g_free);
	}
	at->pdu_lines = NULL;
	if (g_slist_length(noti->callbacks) == 0) {
		g_hash_table_remove(at->unsolicited_table, key);
	}
	dbg("exit");
}

static void _free_noti_list(void *data)
{
	struct _notification *noti = data;

	if (!data)
		return;

	g_slist_free_full(noti->callbacks, g_free);
}

#if 0
static void _msgat(const char *prefix, const char *str)
{
	unsigned int i;
	char buf[8192] = {0, };
	char *pos;

	if (!str) {
		msg("str is NULL");
		return;
	}

	if (strlen(str) > 4096) {
		msg("%s[%s]", prefix, str);
		return;
	}

	pos = buf;
	for (i = 0; i < strlen(str); i++) {
		if (str[i] == '\r') {
			strncpy(pos, "<CR>", 4);
			pos += 4;
		} else if (str[i] == '\n') {
			strncpy(pos, "<LF>", 4);
			pos += 4;
		} else {
			*pos = str[i];
			pos++;
		}
	}

	msg("%s[%s]", prefix, buf);
}
#endif

TcoreAT* tcore_at_new(TcoreHal *hal)
{
	TcoreAT *at;

	at = calloc(sizeof(struct tcore_at_type), 1);
	if (!at)
		return NULL;

	at->hal = hal;
	at->buf_size = MAX_AT_RESPONSE;
	at->buf = calloc(at->buf_size + 1, 1);
	at->buf_read_pos = at->buf;
	at->buf_write_pos = at->buf;
	at->data_mode = MODE_HEX;
	at->unsolicited_table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, _free_noti_list);
	return at;
}

void tcore_at_free(TcoreAT *at)
{
	if (!at)
		return;

	if (at->buf)
		free(at->buf);

	if (at->unsolicited_table)
		g_hash_table_destroy(at->unsolicited_table);

	free(at);
}

TReturn tcore_at_remove_notification_full(TcoreAT *at, const char *prefix, TcoreATNotificationCallback callback, void *user_data)
{
	struct _notification *noti;
	struct _notification_callback *item;
	GSList *p;

	if (!at || !prefix)
		return TCORE_RETURN_EINVAL;

	if (!callback) {
		/* remove all callbacks for prefix */
		g_hash_table_remove(at->unsolicited_table, prefix);
		return TCORE_RETURN_SUCCESS;
	}

	noti = g_hash_table_lookup(at->unsolicited_table, prefix);
	if (!noti)
		return TCORE_RETURN_SUCCESS;

	p = noti->callbacks;
	for (; p; p = p->next) {
		item = p->data;
		if (!item)
			continue;

		if (callback == item->callback) {
			if (!user_data) {
				noti->callbacks = g_slist_remove(noti->callbacks, item);
				continue;
			}

			if (user_data == item->user_data) {
				noti->callbacks = g_slist_remove(noti->callbacks, item);
				continue;
			}
		}
	}

	return TCORE_RETURN_SUCCESS;
}


TReturn tcore_at_remove_notification(TcoreAT *at, const char *prefix, TcoreATNotificationCallback callback)
{
	return tcore_at_remove_notification_full(at, prefix, callback, NULL);
}

TReturn tcore_at_add_notification(TcoreAT *at, const char *prefix,
								  gboolean pdu, TcoreATNotificationCallback callback,
								  void *user_data)
{
	struct _notification *noti;
	struct _notification_callback *item;

	if (!at || !prefix || !callback)
		return TCORE_RETURN_EINVAL;

	noti = g_hash_table_lookup(at->unsolicited_table, prefix);
	if (!noti) {
		noti = calloc(sizeof(struct _notification), 1);
		if (!noti)
			return TCORE_RETURN_ENOMEM;

		noti->type_pdu = pdu;
		noti->callbacks = NULL;

		g_hash_table_insert(at->unsolicited_table, g_strdup(prefix), noti);
	}

	if (noti->type_pdu != pdu)
		return TCORE_RETURN_EINVAL;

	item = calloc(sizeof(struct _notification_callback), 1);
	if (!item)
		return TCORE_RETURN_ENOMEM;

	item->callback = callback;
	item->user_data = user_data;

	noti->callbacks = g_slist_append(noti->callbacks, item);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_at_set_request(TcoreAT *at, TcoreATRequest *req, gboolean send)
{
	TReturn ret;
	char *end;
	char next;

	if (!at)
		return TCORE_RETURN_EINVAL;

	at->req = req;

	if (req) {
		dbg("req->cmd = [%s]", at->req->cmd);
		dbg("req->prefix = [%s]", at->req->prefix);
		dbg("req->type = %d", at->req->type);
	}

	if (send == FALSE)
		return TCORE_RETURN_SUCCESS;

	end = strchr(at->req->cmd, CR);
	next = *(end + 1);
	if (next == '\0') {
		return tcore_hal_send_data(at->hal, strlen(req->cmd), req->cmd);
	}

	dbg("backup data = [%c]", next);
	at->req->next_send_pos = end + 1;
	dbg("next data = [%s]", at->req->next_send_pos);

	*(end + 1) = '\0';
	ret = tcore_hal_send_data(at->hal, strlen(req->cmd), req->cmd);
	*(end + 1) = next;

	return ret;
}

TcoreATRequest* tcore_at_get_request(TcoreAT *at)
{
	if (!at)
		return NULL;

	return at->req;
}


TcoreATResponse* tcore_at_get_response(TcoreAT *at)
{
	if (!at)
		return NULL;

	return at->resp;
}

TReturn tcore_at_buf_write(TcoreAT *at, unsigned int data_len, const char *data)
{
	unsigned int read_pos;
	unsigned int write_pos;

	if (!at)
		return TCORE_RETURN_EINVAL;

	read_pos = at->buf_read_pos - at->buf;
	write_pos = at->buf_write_pos - at->buf;

	if (write_pos + data_len >= at->buf_size) {
		/* shift left (trim completed data) */
		dbg("shift left buffer (request data_len = %d)", data_len);
		dbg("before read_pos=buf+%d, write_pos=buf+%d", read_pos, write_pos);
		memmove(at->buf, at->buf_read_pos, at->buf_size - read_pos);
		at->buf_read_pos = at->buf;
		at->buf_write_pos = at->buf + write_pos - read_pos;
		dbg("after  read_pos=buf+%d, write_pos=buf+%d",
			at->buf_read_pos - at->buf,
			at->buf_write_pos - at->buf);
		memset(at->buf_write_pos, 0, at->buf_size - (at->buf_write_pos - at->buf));
	}

	write_pos = at->buf_write_pos - at->buf;
	if (write_pos + data_len >= at->buf_size) {
		while (1) {
			at->buf_size = at->buf_size << 1;
			if (at->buf_size > write_pos + data_len)
				break;
		}

		at->buf = realloc(at->buf, at->buf_size);
		at->buf_read_pos = at->buf;
		at->buf_write_pos = at->buf + write_pos;
		memset(at->buf_write_pos, 0, at->buf_size - (at->buf_write_pos - at->buf));
		dbg("resize buffer to %d", at->buf_size);
	}

	memcpy(at->buf_write_pos, data, data_len);

	at->buf_write_pos += data_len;

	return TCORE_RETURN_SUCCESS;
}

TcoreATRequest* tcore_at_request_new(const char *cmd, const char *prefix, enum tcore_at_command_type type)
{
	TcoreATRequest *req;

	if (!cmd)
		return NULL;

	if (strlen(cmd) < 1)
		return NULL;

	req = calloc(sizeof(struct tcore_at_request), 1);
	if (!req)
		return NULL;

	if (!strchr(cmd, CR))
		req->cmd = g_strdup_printf("%s%c", cmd, CR);
	else
		req->cmd = g_strdup_printf("%s%c", cmd, 26);

	if (prefix)
		req->prefix = strdup(prefix);

	req->type = type;

	return req;
}

void tcore_at_request_free(TcoreATRequest *req)
{
	if (!req)
		return;

	if (req->cmd)
		free(req->cmd);

	if (req->prefix)
		free(req->prefix);

	free(req);
}

/* To get the length value from little-endian bytes */
static int __sum_4_bytes(const char *posn)
{
	int sum = 0;
	sum = sum | (*(posn+3)) << 24;
	sum = sum | (*(posn+2)) << 16;
	sum = sum | (*(posn+1)) << 8;
	sum = sum | *posn ;
	return sum;
}

/* Function to process binary data received as part of XDRV Indication */
void tcore_at_process_binary_data(TcoreAT *at, char *position, int data_len)
{

	#define NVM_PAYLOAD_LENGTH_0			52
	#define NVM_PAYLOAD_LENGTH_1			68

	int m_length_0 = ZERO , m_length_1 = ZERO;
	static int data_len_final = ZERO, actual_buffer_size = ZERO;
	dbg("Entered");

	m_length_0 = __sum_4_bytes(&position[NVM_PAYLOAD_LENGTH_0]);
	m_length_1 = __sum_4_bytes(&position[NVM_PAYLOAD_LENGTH_1]);
	data_len_final = data_len_final + data_len;

	dbg("m_length_0 = %d , m_length_1 = %d, data_len_final = %d actual_buffer_size: %d", m_length_0, m_length_1, data_len_final, actual_buffer_size);
	if (actual_buffer_size == ZERO) {
		actual_buffer_size = data_len + m_length_0 + m_length_1;
		dbg("Actual buffer size is %d", actual_buffer_size);
	}

	if (data_len_final == actual_buffer_size) {
		_emit_unsolicited_message(at, position);
		at->data_mode = MODE_HEX;
		at->buf_read_pos = at->buf_read_pos + (actual_buffer_size + 1);
		data_len_final = ZERO;
		actual_buffer_size = ZERO;
	}
	dbg("Exit");
}

gboolean tcore_at_process(TcoreAT *at, unsigned int data_len, const char *data)
{
	char *pos;
	char *next_pos;
	int ret;

	if (!at || !data)
		return FALSE;

	tcore_at_buf_write(at, data_len, data);

	pos = at->buf_read_pos;
	dbg("On entry at->buf_read_pos: 0x%x", at->buf_read_pos);

	while (1) {
		while (*pos == CR || *pos == LF)
			pos++;

		next_pos = _find_next_EOL(pos);
		if (!next_pos)
		{
			dbg("Data could be in Binary mode !!");
			if (at->rfs_hook) {
				if (TRUE == at->rfs_hook(pos)){
					at->data_mode = MODE_BIN;
					tcore_at_process_binary_data(at, pos, data_len);
				}
				dbg("Not Binary data");
			}
			dbg("Rfs hook is not set !!");
			break;
		}
		if (pos != next_pos)
			*next_pos = '\0';

		// dbg("complete line found.");
		dbg("line = [%s]", pos);
		// check request
		if (!at->req) {
			dbg(" Not At request " );
			_emit_unsolicited_message(at, pos);
		}
		else {

			if (g_strcmp0(pos, "> ") == 0) {
				if (at->req->next_send_pos) {
					dbg("send next: [%s]", at->req->next_send_pos);
					tcore_hal_send_data(at->hal, strlen(at->req->next_send_pos), at->req->next_send_pos);
					pos += 2;
					at->buf_read_pos = pos;
					break;
				}
			}

			if (!at->resp) {
				at->resp = _response_new();
			}

			ret = _check_final_response(pos);
			if (ret) {
				if (ret == 1)
					at->resp->success = TRUE;
				else
					at->resp->success = FALSE;

				at->resp->final_response = strdup(pos);

				_emit_pending_response(at);
				at->buf_read_pos = next_pos + 1;
				return TRUE;
			} else {
				switch (at->req->type) {
				case TCORE_AT_NO_RESULT:
					_emit_unsolicited_message(at, pos);
					break;

				case TCORE_AT_NUMERIC:
					if (at->resp->lines == NULL && isdigit(pos[0])) {
						_response_add(at->resp, pos);
					} else {
						_emit_unsolicited_message(at, pos);
					}

					break;

					case TCORE_AT_SINGLELINE:
						dbg("Type is SINGLELINE");
						if (at->resp->lines == NULL) {
							if (at->req->prefix) {
								if (g_str_has_prefix(pos, at->req->prefix)) {
										_response_add(at->resp, pos);
								}
								else {
									_emit_unsolicited_message(at, pos);
								}
							} else {
								_response_add(at->resp, pos);
							}
						}
						else {
							_emit_unsolicited_message(at, pos);
						}
						break;

				case TCORE_AT_MULTILINE:
					if (at->req->prefix) {
						if (g_str_has_prefix(pos, at->req->prefix)) {
							_response_add(at->resp, pos);
						} else {
							_emit_unsolicited_message(at, pos);
						}
					} else {
						_response_add(at->resp, pos);
					}
					break;

				case TCORE_AT_PDU:
					if (at->req->prefix) {
						if (g_str_has_prefix(pos, at->req->prefix)) {
							_response_add(at->resp, pos);
						} else {
							if (at->resp->lines != NULL) {
								_response_add(at->resp, pos);
							} else {
								_emit_unsolicited_message(at, pos);
							}
						}
					} else {
						_response_add(at->resp, pos);
					}
					break;

				default:
					dbg("unknown");
					_emit_unsolicited_message(at, pos);
					break;
				}
			}
		}

		//
		pos = next_pos + 1;
		at->buf_read_pos = pos;
	}
	dbg("On exit at->buf_read_pos: 0x%x", at->buf_read_pos);
	return FALSE;
}

TcorePending* tcore_at_pending_new(CoreObject *co, const char *cmd, const char *prefix, enum tcore_at_command_type type, TcorePendingResponseCallback func, void *user_data)
{
	TcorePending *p;
	TcoreATRequest *req;

	if (!cmd)
		return NULL;

	req = tcore_at_request_new(cmd, prefix, type);
	if (!req)
		return NULL;

	p = tcore_pending_new(co, 0);
	if (!p) {
		tcore_at_request_free(req);
		return NULL;
	}

	tcore_pending_set_request_data(p, 0, req);
	tcore_pending_set_response_callback(p, func, user_data);

	return p;
}

#define TYPE_NONE       0
#define TYPE_RAW        1
#define TYPE_STR        2
#define TYPE_STR_FIN    3
#define TYPE_PAREN      4
#define TYPE_PAREN_FIN  5

GSList* tcore_at_tok_new(const char *line)
{
	char *begin;
	char *pos;
	char *buf = NULL;
	char *mark_end = NULL;
	int type = TYPE_NONE;
	GSList *tokens = NULL;

	if (!line)
		return NULL;

	if (strlen(line) == 0)
		return NULL;

	if (line[0] == '(') {
		/* list token container */
		pos = (char *) line;
		if (line[strlen(line) - 1] == ')')
			mark_end = (char *) line + strlen(line) - 1;
	} else {
		/* normal at message */
		pos = strchr(line, ':');
		if (!pos) {
			tokens = g_slist_append(tokens, strdup(line));
			return tokens;
		}
	}

	pos++;

	/* skip whitespace */
	while (*pos != '\0' && isspace(*pos)) {
		pos++;
	}

	begin = pos;

	do {
		switch (type) {
		case TYPE_NONE:
			if (*pos == '"') {
				type = TYPE_STR;
			} else if (*pos == ',') {
				tokens = g_slist_append(tokens, strdup(""));
			} else if (*pos == ' ') {
				// skip
			} else if (*pos == '(') {
				type = TYPE_PAREN;
			} else {
				type = TYPE_RAW;
			}
			begin = pos;
			break;

		case TYPE_STR:
			if (*pos == '"') {
				type = TYPE_STR_FIN;
				buf = calloc(pos - begin + 2, 1);
				memcpy(buf, begin, pos - begin + 1);
				tokens = g_slist_append(tokens, buf);
			}
			break;

		case TYPE_PAREN:
			if (*pos == ')') {
				type = TYPE_PAREN_FIN;
				buf = calloc(pos - begin + 2, 1);
				memcpy(buf, begin, pos - begin + 1);
				tokens = g_slist_append(tokens, buf);
			}
			break;

		case TYPE_RAW:
			if (*pos == ',' || *pos == '\0') {
				type = TYPE_NONE;
				buf = calloc(pos - begin + 1, 1);
				memcpy(buf, begin, pos - begin);
				tokens = g_slist_append(tokens, buf);
			}
			break;

		case TYPE_STR_FIN:
		case TYPE_PAREN_FIN:
			if (*pos == ',') {
				type = TYPE_NONE;
			}
			break;
		}

		if (*pos == '\0' || pos == mark_end)
			break;

		pos++;
	} while (1);

	if (type == TYPE_RAW) {
		buf = calloc(pos - begin + 1, 1);
		memcpy(buf, begin, pos - begin);
		tokens = g_slist_append(tokens, buf);
	}

	return tokens;
}

void tcore_at_tok_free(GSList *tokens)
{
	if (!tokens)
		return;

	g_slist_free_full(tokens, g_free);
}

char* tcore_at_tok_extract(const char *src)
{
	char *dest = NULL;
	char *last = NULL;

	if (!src)
		return NULL;

	if (strlen(src) < 2)
		return g_strdup(src);

	last = (char *) src + strlen(src) - 1;

	switch (*src) {
	case '(':
		if (*last == ')') {
			dest = g_strdup(src + 1);
			dest[strlen(dest) - 1] = '\0';
		}
		break;

	case '"':
		if (*last == '"') {
			dest = g_strdup(src + 1);
			dest[strlen(dest) - 1] = '\0';
		}
		break;

	default:
		return g_strdup(src);
		break;
	}

	return dest;
}

char* tcore_at_tok_nth(GSList *tokens, unsigned int token_index)
{
	if (!tokens)
		return NULL;

	if (token_index > g_slist_length(tokens))
		return NULL;

	return (char *) g_slist_nth_data(tokens, token_index);
}

gboolean tcore_at_add_hook(TcoreHal *hal, void *hook_func)
{
	TcoreAT *at;
	at = tcore_hal_get_at(hal);

	if (at != NULL) {
		dbg("Setting the rfs hook callback function");
		at->rfs_hook = (rfs_hook_cb) hook_func;
		return TRUE;
	}
	dbg("AT is NULL !!!");
	return FALSE;
}

TReturn tcore_prepare_and_send_at_request(CoreObject *co,
												const char *at_cmd,
												const char *at_cmd_prefix,
												enum tcore_at_command_type at_cmd_type,
												UserRequest *ur,
												TcorePendingResponseCallback resp_cb,
												void *resp_cb_data,
												TcorePendingSendCallback send_cb,
												void *send_cb_data)
{
	TcorePending *pending = NULL;
	TcoreATRequest *req = NULL;
	TcoreHal *hal = NULL;
	TReturn ret = TCORE_RETURN_FAILURE;

	hal = tcore_object_get_hal(co);
	if (!hal) {
		dbg("HAL is NULL");
		return ret;
	}

	/* Create Pending Request */
	pending = tcore_pending_new(co, 0);
	if (!pending) {
		dbg("Pending is NULL");
		return ret;
	}

	/* Create AT-Command Request */
	req = tcore_at_request_new(at_cmd, at_cmd_prefix, at_cmd_type);
	if (!req) {
		dbg("Request is NULL");
		tcore_pending_free(pending);
		return ret;
	}
	dbg("AT Command: %s, Prefix(if any):%s, AT-Command length: %d", req->cmd, req->prefix, strlen(req->cmd));

	tcore_pending_set_request_data(pending, 0, req);
	tcore_pending_set_response_callback(pending, resp_cb, resp_cb_data);
	tcore_pending_set_send_callback(pending, send_cb, send_cb_data);
	tcore_pending_link_user_request(pending, ur);

	ret = tcore_hal_send_request(hal, pending);
	return ret;
}
