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

#define NUM_ELEMS(x)		(sizeof(x)/sizeof(x[0]))
#define MODE_HEX		0
#define MODE_BIN		1

#define CR			'\r'
#define LF			'\n'

/* Response buffer would grow on requirement */
#define MAX_AT_RESPONSE	255

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

	RfsHookCb rfs_hook;
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
	"CONNECT",
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
	"NO CARRIER",
	"COMMAND NOT SUPPORT",	/* For Huawei Cellular dongle */
};

/**
 * 'typedef' to define final response received
 */
typedef enum {
	TCORE_AT_FINAL_RESPONSE_UNKNOWN = 0,
	TCORE_AT_FINAL_RESPONSE_SUCCESS,
	TCORE_AT_FINAL_RESPONSE_FAILURE,
} tcore_at_final_response;

static tcore_at_final_response _check_final_response(const char *line)
{
	unsigned int i;

	for (i = 0; i < NUM_ELEMS(list_final_responses_success); i++)
		if (g_str_has_prefix(line, list_final_responses_success[i]))
			return TCORE_AT_FINAL_RESPONSE_SUCCESS;

	for (i = 0; i < NUM_ELEMS(list_final_responses_error); i++)
		if (g_str_has_prefix(line, list_final_responses_error[i]))
			return TCORE_AT_FINAL_RESPONSE_FAILURE;

	return TCORE_AT_FINAL_RESPONSE_UNKNOWN;
}


static char *_find_next_EOL(char *cur)
{
	if ((cur[0] == '>' && cur[1] == ' ')
			&& (cur[2] == '\0' || cur[2] == CR)) {
		/* SMS prompt character...not \r terminated */
		dbg("SMS prompt character: [%c]", cur[0]);
		return cur + 2;
	}

	/*
	 * Find next newline
	 *
	 * Line should either end with -
	 *  - '\0' termination
	 * or
	 *  - Carriage Return '\r' and Line Feed  '\n'.
	 */
	while (*cur != '\0' && !((*cur == CR) && (*(cur + 1) == LF)))
		cur++;

	return *cur == '\0' ? NULL : cur;
}

static struct tcore_at_response *_response_new()
{
	struct tcore_at_response *resp;

	resp = g_try_malloc0(sizeof(struct tcore_at_response));
	if (!resp)
		return NULL;

	return resp;
}

static void _response_free(struct tcore_at_response *resp)
{
	if (!resp)
		return;

	if (resp->lines)
		g_slist_free_full(resp->lines, g_free);

	if (resp->final_response)
		g_free(resp->final_response);

	g_free(resp);
}


static void _response_add(struct tcore_at_response *resp,
		const char *line)
{
	if (!resp || !line)
		return;

	dbg("current lines = %d", g_slist_length(resp->lines));

	resp->lines = g_slist_append(resp->lines, g_strdup(line));
}

static void _emit_pending_response(TcoreAT *at)
{
	TcorePending *p;

	if (!at)
		return;

	p = tcore_queue_pop(tcore_hal_ref_queue(at->hal));
	if (!p)
		info("NO pending request!");

	tcore_pending_emit_response_callback(p, sizeof(TcoreATResponse), at->resp);
	tcore_user_request_unref(tcore_pending_ref_user_request(p));

	tcore_at_request_free(at->req);
	at->req = NULL;
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

	dbg("pdu_status:  [%s] data_mode: [%s]",
		(at->pdu_status ? "TRUE" : "FALSE"),
		(at->data_mode == MODE_BIN ? "Binary" : "Hex"));
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
		} else
			data = g_slist_append(NULL, g_strdup(line));
	} else {
		noti = at->pdu_noti;
		at->pdu_status = FALSE;
		at->pdu_noti = NULL;

		if (at->data_mode == MODE_BIN) {
			dbg("Binary mode");

			at->pdu_lines = g_slist_append(at->pdu_lines, (gpointer)line);
			dbg("at->pdu_lines: 0x%x", at->pdu_lines);
		} else
			at->pdu_lines = g_slist_append(at->pdu_lines, g_strdup(line));

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

	if (at->data_mode != MODE_BIN)
		g_slist_free_full(data, g_free);

	at->pdu_lines = NULL;

	if (g_slist_length(noti->callbacks) == 0)
		g_hash_table_remove(at->unsolicited_table, key);
}

static void _free_noti_list(void *data)
{
	struct _notification *noti = data;

	if (!data)
		return;

	g_slist_free_full(noti->callbacks, g_free);
}

TcoreAT *tcore_at_new(TcoreHal *hal)
{
	TcoreAT *at;

	at = g_try_malloc0(sizeof(struct tcore_at_type));
	if (!at)
		return NULL;

	at->hal = hal;
	at->buf_size = MAX_AT_RESPONSE;
	at->buf = g_try_malloc0(at->buf_size + 1);
	if (at->buf == NULL) {
		g_free(at);
		return NULL;
	}

	at->buf_read_pos = at->buf;
	at->buf_write_pos = at->buf;
	at->data_mode = MODE_HEX;
	at->unsolicited_table = g_hash_table_new_full(g_str_hash, g_str_equal,
			g_free, _free_noti_list);

	return at;
}

void tcore_at_free(TcoreAT *at)
{
	if (!at)
		return;

	if (at->buf)
		g_free(at->buf);

	if (at->unsolicited_table)
		g_hash_table_destroy(at->unsolicited_table);

	g_free(at);
}

TReturn tcore_at_remove_notification_full(TcoreAT *at, const char *prefix,
		TcoreATNotificationCallback callback, void *user_data)
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


TReturn tcore_at_remove_notification(TcoreAT *at, const char *prefix,
		TcoreATNotificationCallback callback)
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
		noti = calloc(1, sizeof(struct _notification));
		if (!noti)
			return TCORE_RETURN_ENOMEM;

		noti->type_pdu = pdu;
		noti->callbacks = NULL;

		g_hash_table_insert(at->unsolicited_table, g_strdup(prefix), noti);
	}

	if (noti->type_pdu != pdu)
		return TCORE_RETURN_EINVAL;

	item = calloc(1, sizeof(struct _notification_callback));
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

	if (!at || !req) {
		err("Invalid data");
		return TCORE_RETURN_EINVAL;
	}

	if (!req->cmd) {
		err("Invalid cmd");
		return TCORE_RETURN_EINVAL;
	}

	at->req = req;

	if (send == FALSE)
		return TCORE_RETURN_SUCCESS;

#ifdef EMULATOR
	if (strstr(at->req->cmd, "CMGS")) {
		dbg("In case of emulator, do not check CR for CMGS");
		return tcore_hal_send_data(at->hal, strlen(req->cmd), req->cmd);
	}
#endif

	end = strchr(at->req->cmd, CR);
	if (end) {
		next = *(end + 1);
		if (next == '\0')
			return tcore_hal_send_data(at->hal, strlen(req->cmd), req->cmd);

		at->req->next_send_pos = end + 1;
		dbg("backup data = [%c] next data = [%s]", next, at->req->next_send_pos);

		*(end+1) = '\0';
		ret = tcore_hal_send_data(at->hal, strlen(req->cmd), req->cmd);
		*(end+1) = next;

		return ret;
	}

	return TCORE_RETURN_EINVAL;
}

TcoreATRequest *tcore_at_get_request(TcoreAT *at)
{
	if (!at)
		return NULL;

	return at->req;
}


TcoreATResponse *tcore_at_get_response(TcoreAT *at)
{
	if (!at)
		return NULL;

	return at->resp;
}

TReturn tcore_at_buf_write(TcoreAT *at, unsigned int data_len, const char *data)
{
	unsigned int read_pos;
	unsigned int write_pos;

	if (!at) {
		err("at is NULL");
		return TCORE_RETURN_EINVAL;
	}

	read_pos = at->buf_read_pos - at->buf;
	write_pos = at->buf_write_pos - at->buf;

	if (write_pos + data_len >= at->buf_size) {
		/* shift left (trim completed data) */
		memmove(at->buf, at->buf_read_pos, at->buf_size - read_pos);
		at->buf_read_pos = at->buf;
		at->buf_write_pos = at->buf + write_pos - read_pos;

		dbg("Shift [Left] - Requested len: %d bytes - " \
			"before (read_pos: %d write_pos: %d) " \
			"after (read_pos: %d write_pos: %d)",
			data_len, read_pos, write_pos,
			(at->buf_read_pos - at->buf),
			(at->buf_write_pos - at->buf));

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

		dbg("Resize buffer to '%d' bytes", at->buf_size);
	}

	/*
	 * Copy Data to write buffer and adjust write buffer pointer
	 */
	memcpy(at->buf_write_pos, data, data_len);
	at->buf_write_pos += data_len;

	return TCORE_RETURN_SUCCESS;
}

TcoreATRequest *tcore_at_request_new(const char *cmd,
	const char *prefix, enum tcore_at_command_type type)
{
	TcoreATRequest *req;

	if (!cmd)
		return NULL;

	if (strlen(cmd) < 1)
		return NULL;

	req = g_try_malloc0(sizeof(struct tcore_at_request));
	if (!req)
		return NULL;

	if (!strchr(cmd, CR))
		req->cmd = g_strdup_printf("%s%c", cmd, CR);
	else
		req->cmd = g_strdup_printf("%s%c", cmd, 26);

	if (prefix)
		req->prefix = g_strdup(prefix);

	req->type = type;

	return req;
}

void tcore_at_request_free(TcoreATRequest *req)
{
	if (!req)
		return;

	if (req->cmd)
		g_free(req->cmd);

	if (req->prefix)
		g_free(req->prefix);

	g_free(req);
}

void tcore_at_response_free(TcoreATResponse *rsp)
{
	_response_free(rsp);
}

/* To get the length value from little-endian bytes */
static int __sum_4_bytes(const char *posn)
{
	int sum = 0;

	sum = sum | (*(posn+3)) << 24;
	sum = sum | (*(posn+2)) << 16;
	sum = sum | (*(posn+1)) << 8;
	sum = sum | *posn;

	return sum;
}

/*
 * Function to process binary data received as part of XDRV Indication
 */
void tcore_at_process_binary_data(TcoreAT *at,
	char *position, int data_len)
{

	#define NVM_PAYLOAD_LENGTH_0			52
	#define NVM_PAYLOAD_LENGTH_1			68

	int m_length_0 = 0 , m_length_1 = 0;
	static int data_len_final = 0, actual_buffer_size = 0;
	dbg("Entered");

	m_length_0 = __sum_4_bytes(&position[NVM_PAYLOAD_LENGTH_0]);
	m_length_1 = __sum_4_bytes(&position[NVM_PAYLOAD_LENGTH_1]);
	data_len_final = data_len_final + data_len;

	dbg("m_length_0 = %d , m_length_1 = %d, " \
		"data_len_final = %d actual_buffer_size: %d",
		m_length_0, m_length_1, data_len_final, actual_buffer_size);
	if (actual_buffer_size == 0) {
		actual_buffer_size = data_len + m_length_0 + m_length_1;
		dbg("Actual buffer size is %d", actual_buffer_size);
	}

	if (data_len_final == actual_buffer_size) {
		_emit_unsolicited_message(at, position);
		at->data_mode = MODE_HEX;
		at->buf_read_pos = at->buf_read_pos + (actual_buffer_size + 1);
		data_len_final = 0;
		actual_buffer_size = 0;
	}
	dbg("Exit");
}

gboolean tcore_at_process(TcoreAT *at, unsigned int data_len, const char *data)
{
	char *pos;
	char *next_pos;
	gboolean is_resp_processed = FALSE;

	if (!at || !data)
		return FALSE;

	tcore_at_buf_write(at, data_len, data);

	pos = at->buf_read_pos;

	while (1) {

		while (*pos == CR || *pos == LF)
			pos++;

		next_pos = _find_next_EOL(pos);
		if (!next_pos) {
			if (at->rfs_hook) {
				dbg("RFS hook set - Not Binary data!");
				if (TRUE == at->rfs_hook(pos)) {
					at->data_mode = MODE_BIN;
					tcore_at_process_binary_data(at, pos, data_len);
				}

			}

			break;
		}

		if (pos != next_pos)
			*next_pos = '\0';

		dbg("Received 'AT' data: [%s]", pos);

		/* Validate Request */
		if (!at->req) {
			err("at->req is NULL");
			_emit_unsolicited_message(at, pos);
		} else {
			tcore_at_final_response ret;
			if (g_strcmp0(pos, "> ") == 0) {
				if (at->req->next_send_pos) {
					dbg("Send next: [%s]", at->req->next_send_pos);
					tcore_hal_send_data(at->hal,
						strlen(at->req->next_send_pos), at->req->next_send_pos);

					/* Including NULL character */
					pos += 3;
					at->buf_read_pos = pos;

					break;
				}
			}

			if (!at->resp) {
				at->resp = _response_new();
				if (!at->resp) {
					err("Failed to create response!");
					return FALSE;
				}
			}

			ret = _check_final_response(pos);
			if (ret != TCORE_AT_FINAL_RESPONSE_UNKNOWN) {
				/*
				 * Valid Response
				 *
				 * Success OR Failure
				 */
				if (ret == TCORE_AT_FINAL_RESPONSE_SUCCESS) {
					at->resp->success = TRUE;
				} else {
					gchar *str_escape = NULL;
					gchar str_secure[12] = {'\0'};

					char *hal_name = tcore_hal_get_name(at->hal);
					msg("=========== [%s] ERROR ===========", hal_name);

					str_escape = g_strescape(at->req->cmd, NULL);
					(void)g_strlcpy(str_secure, str_escape, G_N_ELEMENTS(str_secure));
					msg("Request: [%s]", str_secure);
					g_free(str_escape); str_escape = NULL;

					str_escape = g_strescape(pos, NULL);
					msg("Response: [%s]", str_escape);
					g_free(str_escape); str_escape = NULL;
					msg("===============================================");

					free(hal_name);

					at->resp->success = FALSE;
				}

				at->resp->final_response = g_strdup(pos);

				_emit_pending_response(at);

				is_resp_processed = TRUE;
			} else {
				/*
				 * Specific Response type OR Possible notification
				 */
				switch (at->req->type) {
				case TCORE_AT_NO_RESULT:
					_emit_unsolicited_message(at, pos);
				break;

				case TCORE_AT_NUMERIC:
					if (at->resp->lines == NULL && isdigit(pos[0]))
						_response_add(at->resp, pos);
					else
						_emit_unsolicited_message(at, pos);
				break;

				case TCORE_AT_SINGLELINE:
					if (at->resp->lines == NULL) {
						if (at->req->prefix) {
							if (g_str_has_prefix(pos, at->req->prefix))
								_response_add(at->resp, pos);
							else
								_emit_unsolicited_message(at, pos);
						} else {
							_response_add(at->resp, pos);
						}
					} else {
						_emit_unsolicited_message(at, pos);
					}
				break;

				case TCORE_AT_MULTILINE:
					if (at->req->prefix) {
						if (g_str_has_prefix(pos, at->req->prefix))
							_response_add(at->resp, pos);
						else
							_emit_unsolicited_message(at, pos);
					} else {
						_response_add(at->resp, pos);
					}
				break;

				case TCORE_AT_PDU:
					if (at->req->prefix) {
						if (g_str_has_prefix(pos, at->req->prefix)) {
							_response_add(at->resp, pos);
						} else {
							if (at->resp->lines != NULL)
								_response_add(at->resp, pos);
							else
								_emit_unsolicited_message(at, pos);
						}
					} else {
						_response_add(at->resp, pos);
					}
				break;

				default:
					warn("Unknown Request type: %d", (at->req->type));
					_emit_unsolicited_message(at, pos);
				break;
				}
			}
		}

		/* Progress 'Read' buffer */
		pos = next_pos + 1;
		at->buf_read_pos = pos;
	}

	if (is_resp_processed == FALSE)
		return FALSE;

	return TRUE;
}

TcorePending *tcore_at_pending_new(CoreObject *co, const char *cmd,
	const char *prefix, enum tcore_at_command_type type,
	TcorePendingResponseCallback func, void *user_data)
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

#define TYPE_NONE		0
#define TYPE_RAW		1
#define TYPE_STR		2
#define TYPE_STR_FIN	3
#define TYPE_PAREN		4
#define TYPE_PAREN_FIN	5

GSList *tcore_at_tok_new(const char *line)
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
		pos = (char *)line;
		if (line[strlen(line)-1] == ')')
			mark_end = (char *)line + strlen(line) - 1;
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
	while (*pos != '\0' && isspace(*pos))
		pos++;

	begin = pos;

	do {
		switch (type) {
		case TYPE_NONE:
			if (*pos == '"')
				type = TYPE_STR;
			else if (*pos == ',')
				tokens = g_slist_append(tokens, strdup(""));
			else if (*pos == ' ') {
				/*
				 * Skip, no processing for blank space
				 */
			} else if (*pos == '(')
				type = TYPE_PAREN;
			else
				type = TYPE_RAW;

			begin = pos;
		break;

		case TYPE_STR:
			if (*pos == '"') {
				type = TYPE_STR_FIN;
				buf = g_try_malloc0((pos - begin) + 2);
				if (!buf) {
					g_slist_free_full(tokens, g_free);
					return NULL;
				}

				memcpy(buf, begin, pos - begin + 1);
				tokens = g_slist_append(tokens, buf);
			}
		break;

		case TYPE_PAREN:
			if (*pos == ')') {
				type = TYPE_PAREN_FIN;
				buf = g_try_malloc0((pos - begin) + 2);
				if (!buf) {
					g_slist_free_full(tokens, g_free);
					return NULL;
				}
				memcpy(buf, begin, pos - begin + 1);
				tokens = g_slist_append(tokens, buf);
			}
		break;

		case TYPE_RAW:
			if (*pos == ',' || *pos == '\0') {
				type = TYPE_NONE;
				buf = g_try_malloc0((pos - begin) + 1);
				if (!buf) {
					g_slist_free_full(tokens, g_free);
					return NULL;
				}

				memcpy(buf, begin, pos - begin);
				tokens = g_slist_append(tokens, buf);
			}
		break;

		case TYPE_STR_FIN:
		case TYPE_PAREN_FIN:
			if (*pos == ',')
				type = TYPE_NONE;
		break;

		default:
			err("invalid string type");
		break;
		}

		if (*pos == '\0' || pos == mark_end)
			break;

		pos++;
	} while (1);

	if (type == TYPE_RAW) {
		buf = g_try_malloc0((pos - begin) + 1);
		if (!buf) {
			g_slist_free_full(tokens, g_free);
			return NULL;
		}

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

char *tcore_at_tok_extract(const char *src)
{
	char *dest = NULL;
	char *last = NULL;

	if (!src)
		return NULL;

	if (strlen(src) < 2)
		return g_strdup(src);

	last = (char *)src + strlen(src) - 1;

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
	}

	return dest;
}

char *tcore_at_tok_nth(GSList *tokens, unsigned int token_index)
{
	if (!tokens)
		return NULL;

	if (token_index > g_slist_length(tokens))
		return NULL;

	return (char *)g_slist_nth_data(tokens, token_index);
}

gboolean tcore_at_add_hook(TcoreHal *hal, void *hook_func)
{
	TcoreAT *at;
	at = tcore_hal_get_at(hal);

	if (at != NULL) {
		dbg("Setting the rfs hook callback function");
		at->rfs_hook = (RfsHookCb)hook_func;
		return TRUE;
	}

	dbg("AT is NULL !!!");
	return FALSE;
}

void tcore_free_pending_timeout_at_request(TcoreAT *at)
{
	if (!at)
		return;

	tcore_at_request_free(at->req);
	at->req = NULL;

	tcore_at_response_free(at->resp);
	at->resp = NULL;
}

TReturn tcore_prepare_and_send_at_request(CoreObject *co,
	const char *at_cmd, const char *at_cmd_prefix,
	enum tcore_at_command_type at_cmd_type,
	UserRequest *ur,
	TcorePendingResponseCallback resp_cb, void *resp_cb_data,
	TcorePendingSendCallback send_cb, void *send_cb_data,
	unsigned int timeout, TcorePendingTimeoutCallback timeout_cb, void *timeout_cb_data)
{
	TcorePending *pending = NULL;
	TcoreATRequest *req = NULL;
	TcoreHal *hal = NULL;
	TReturn ret = TCORE_RETURN_FAILURE;

	hal = tcore_object_get_hal(co);
	if (!hal) {
		err("HAL is NULL");
		return ret;
	}
	dbg("hal: [0x%x]", hal);

	/* Create Pending Request */
	pending = tcore_pending_new(co, 0);
	if (!pending) {
		err("Pending is NULL");
		return ret;
	}

	/* Create AT-Command Request */
	req = tcore_at_request_new(at_cmd, at_cmd_prefix, at_cmd_type);
	if (req == NULL) {
		err("Request is NULL");

		tcore_pending_free(pending);
		return ret;
	}

	dbg("AT Command: [%s], Prefix(if any): [%s], AT-Command length: [%d]",
		req->cmd, req->prefix, strlen(req->cmd));

	tcore_pending_set_request_data(pending, 0, req);
	tcore_pending_set_response_callback(pending, resp_cb, resp_cb_data);
	tcore_pending_set_send_callback(pending, send_cb, send_cb_data);
	tcore_pending_set_timeout(pending, timeout);
	tcore_pending_set_timeout_callback(pending, timeout_cb, timeout_cb_data);
	tcore_pending_link_user_request(pending, ur);

	ret = tcore_hal_send_request(hal, pending);
	if (ret != TCORE_RETURN_SUCCESS) {
		err("Failed to send AT-Request!");

		tcore_at_request_free(req);
		tcore_pending_free(pending);
	}

	return ret;
}
