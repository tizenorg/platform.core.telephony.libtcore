/*
 * libtcore
 *
 * Copyright (c) 2013 Samsung Electronics Co. Ltd. All rights reserved.
 * Copyright (c) 2013 Intel Corporation. All rights reserved.
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
#include "at.h"
#include "core_object.h"

#define NUM_ELEMS(x) (sizeof(x)/sizeof(x[0]))
#define MODE_HEX 	0
#define MODE_BIN	1

#define CR '\r'
#define LF '\n'

#define MAX_AT_RESPONSE    255

typedef gboolean (*rfs_hook_cb) (const gchar *data);

struct tcore_at_type {
	TcoreHal *hal;

	TcoreAtCommandType cmd_type;

	GHashTable *unsolicited_table;

	TcoreAtRequest *req;
	TcoreAtResponse *resp;

	guint buf_size;
	gchar *buf;
	gchar *buf_read_pos;
	gchar *buf_write_pos;

	gboolean pdu_status;
	struct _notification *pdu_noti;
	GSList *pdu_lines;

	rfs_hook_cb rfs_hook;
	gboolean data_mode;
};

struct _notification_callback {
	TcoreAtNotificationCallback callback;
	void *user_data;
};

struct _notification {
	gboolean type_pdu;
	GSList *callbacks;
};

typedef enum {
	TCORE_AT_TOKEN_TYPE_NONE = 0,
	TCORE_AT_TOKEN_TYPE_RAW,
	TCORE_AT_TOKEN_TYPE_STR,
	TCORE_AT_TOKEN_TYPE_STR_FIN,
	TCORE_AT_TOKEN_TYPE_PAREN,
	TCORE_AT_TOKEN_TYPE_PAREN_FIN
} TcoreAtTokenType;

typedef enum {
	TCORE_AT_RECV_MSG_TYPE_NOTI,
	TCORE_AT_RECV_MSG_TYPE_RESP_SUCCESS,
	TCORE_AT_RECV_MSG_TYPE_RESP_FAILURE
} TcoreAtRcvMsgType;

/**
 * Returns TCORE_AT_RECV_MSG_TYPE_RESP_SUCCESS
 * if line is a final response indicating success
 *
 * See 27.007 annex B
 */
static const gchar *at_final_responses_success[] = {
	"OK",
	"CONNECT",
	"ABORTED"
};

/**
 * Returns TCORE_AT_RECV_MSG_TYPE_RESP_FAILURE
 * if line is a final response indicating error
 *
 * See 27.007 annex B
 */
static const gchar *at_final_responses_error[] = {
	"+CMS ERROR:",
	"+CME ERROR:",
	"ERROR",
	"NO ANSWER",
	"NO CARRIER",
	"BUSY",
	"NO DIALTONE"
};

static TcoreAtRcvMsgType __check_final_response(const gchar *line)
{
	guint i;

	for (i = 0; i < NUM_ELEMS(at_final_responses_success); i++)
		if (g_str_has_prefix(line, at_final_responses_success[i]))
			return TCORE_AT_RECV_MSG_TYPE_RESP_SUCCESS;

	for (i = 0; i < NUM_ELEMS(at_final_responses_error); i++)
		if (g_str_has_prefix(line, at_final_responses_error[i]))
			return TCORE_AT_RECV_MSG_TYPE_RESP_FAILURE;

	return TCORE_AT_RECV_MSG_TYPE_NOTI;
}


static gchar *_find_next_EOL(gchar *cur)
{
	if (cur[0] == '>' && cur[1] == ' ' && cur[2] == '\0')
		return cur + 2;	/* SMS prompt character...not \r terminated */

	/*
	 * Find next newline
	 *
	 * Avoid issue when AT response is having <CR> in between,
	 * before end of line ( <CR> <LF>) or '/0'
	 */
	while (*cur != '\0' && !((*cur == CR) && (*(cur + 1) == LF)))
		cur++;

	return *cur == '\0' ? NULL : cur;
}

static TcoreAtResponse *__at_response_new()
{
	TcoreAtResponse *at_resp;

	at_resp = tcore_try_malloc0(sizeof(TcoreAtResponse));
	if (at_resp == NULL) {
		err("Failed to allocate memory");
		return NULL;
	}

	return at_resp;
}

static void __at_response_free(TcoreAtResponse *at_resp)
{
	if (at_resp == NULL) {
		err("Response is NULL");
		return;
	}

	g_slist_free_full(at_resp->lines, g_free);
	tcore_free(at_resp->final_response);
	tcore_free(at_resp);
}

static gboolean __at_is_abort(TcoreAtResponse *at_resp)
{
	gboolean aborted = FALSE;
	if (at_resp == NULL) {
		err("at_resp is NULL");
		return FALSE;
	}

	/* Request is ABORTED */
	if (g_strcmp0(at_resp->final_response, "ABORTED") == 0)
		aborted = TRUE;
	dbg("Aborted: [%s]", (aborted ? "YES" : "NO"));

	return aborted;
}

static void __at_response_add(TcoreAtResponse *at_resp, const gchar *line)
{
	if ((at_resp == NULL) || (line == NULL)) {
		err("at_resp: [%p] line: [%p]", at_resp, line);
		return;
	}

	dbg("Current number of lines: [%d]", g_slist_length(at_resp->lines));

	at_resp->lines = g_slist_append(at_resp->lines, tcore_strdup(line));
}

static void __at_emit_pending_response(TcoreAT *at, gboolean abort)
{
	TcorePending *p;
	TcoreQueue *at_queue;

	if (at == NULL) {
		err("at is NULL");
		return;
	}

	tcore_at_request_free(at->req);
	at->req = NULL;

	at_queue = tcore_hal_ref_queue(at->hal);

	/*
	 * If 'abort' is TRUE,
	 * then Pop the first Abortable Request from Queue
	 */
	if (abort) {
		p = tcore_queue_pop_abortable_pending(at_queue);
		if (p == NULL) {
			warn("No Abortable requests!!!");
			goto out;
		}
	}
	else {
		p = tcore_queue_pop(at_queue);
		if (p == NULL) {
			warn("NO pending request!!!");
			goto out;
		}
	}

	tcore_pending_emit_response_callback(p, sizeof(TcoreAtResponse), at->resp);
	tcore_pending_free(p);

out:
	__at_response_free(at->resp);
	at->resp = NULL;
}

static void __at_emit_unsolicited_message(TcoreAT *at, const gchar *line)
{
	struct _notification *noti = NULL;
	struct _notification_callback *item = NULL;
	GSList *p;
	GHashTableIter iter;
	gpointer key = NULL, value;
	gboolean ret;
	GSList *data = NULL;

	if ((at == NULL) || (line == NULL)) {
		err("at: [%p] line: [%p]", at, line);
		return;
	}

	dbg("PDU Status: [%s] Line: [%s] Data mode: [%s]",
		(at->pdu_status ? "TRUE" : "FALSE"), line,
		(at->data_mode ? "TRUE" : "FALSE"));

	if (at->pdu_status == FALSE) {
		g_hash_table_iter_init(&iter, at->unsolicited_table);
		while (g_hash_table_iter_next(&iter, &key, &value)) {
			if (g_str_has_prefix(line, key) == 0)
				continue;

			noti = value;
			break;
		}

		if (noti == NULL)
			return;

		if (noti->type_pdu == TRUE) {
			dbg("PDU mode");

			at->pdu_status = TRUE;
			at->pdu_noti = noti;
			at->pdu_lines =
				g_slist_append(NULL, tcore_strdup(line));

			return;
		}

		if (at->data_mode == MODE_BIN) {
			at->pdu_lines =
				g_slist_append(at->pdu_lines, (gpointer)line);

			data = at->pdu_lines;
		}
		else
			data = g_slist_append(NULL, tcore_strdup(line));
	}
	else {
		noti = at->pdu_noti;
		at->pdu_status = FALSE;
		at->pdu_noti = NULL;

		if (at->data_mode == MODE_BIN) {
			dbg("Binary mode");
			at->pdu_lines = g_slist_append(at->pdu_lines, (gpointer)line);
			dbg("at->pdu_lines: [%p]", at->pdu_lines);
		}
		else
			at->pdu_lines = g_slist_append(at->pdu_lines, tcore_strdup(line));

		data = at->pdu_lines;
	}

	p = noti->callbacks;
	while (p) {
		GSList *tmp_list = p;
		item = p->data;
		if (item != NULL) {
			if (item->callback != NULL) {
				ret = item->callback(at, data, item->user_data);
				if (ret == FALSE) {
					noti->callbacks =
						g_slist_remove(noti->callbacks, item);

					tcore_free(item);
				}
			}
		}

		p = tmp_list->next;
	}

	if (at->data_mode != MODE_BIN) {
		dbg("Free the list");
		g_slist_free_full(data, g_free);
	}

	at->pdu_lines = NULL;
	if (g_slist_length(noti->callbacks) == 0)
		g_hash_table_remove(at->unsolicited_table, key);

	dbg("exit");
}

static void __free_noti_list(void *data)
{
	struct _notification *noti = data;

	if (noti != NULL)
		g_slist_free_full(noti->callbacks, g_free);
}

TcoreAT *tcore_at_new(TcoreHal *hal)
{
	TcoreAT *at;

	at = g_try_new0(struct tcore_at_type, 1);
	if (at == NULL) {
		err("Failed to allocate memory");
		return NULL;
	}

	at->hal = hal;
	at->buf_size = MAX_AT_RESPONSE;
	at->buf = tcore_try_malloc0(at->buf_size + 1);
	if (at->buf == NULL) {
		err("Failed to allocate memory");
		tcore_free(at);
		return NULL;
	}

	at->buf_read_pos = at->buf;
	at->buf_write_pos = at->buf;
	at->data_mode = MODE_HEX;
	at->unsolicited_table = g_hash_table_new_full(g_str_hash,
		g_str_equal, g_free, __free_noti_list);

	return at;
}

void tcore_at_free(TcoreAT *at)
{
	if (at == NULL) {
		err("AT-Command is NULL");
		return;
	}

	g_hash_table_destroy(at->unsolicited_table);
	tcore_free(at->buf);
	tcore_free(at);
}

TelReturn tcore_at_remove_notification_full(TcoreAT *at, const gchar *prefix,
	TcoreAtNotificationCallback callback, void *user_data)
{
	struct _notification *noti;
	struct _notification_callback *item;
	GSList *p;

	if ((at == NULL) || !prefix)
		return TEL_RETURN_INVALID_PARAMETER;

	if (!callback) {
		/* remove all callbacks for prefix */
		g_hash_table_remove(at->unsolicited_table, prefix);
		return TEL_RETURN_SUCCESS;
	}

	noti = g_hash_table_lookup(at->unsolicited_table, prefix);
	if (!noti)
		return TEL_RETURN_SUCCESS;

	p = noti->callbacks;
	for (; p; p = p->next) {
		item = p->data;
		if (!item)
			continue;

		if (callback == item->callback) {
			if (!user_data) {
				noti->callbacks =
					g_slist_remove(noti->callbacks, item);
				continue;
			}

			if (user_data == item->user_data) {
				noti->callbacks =
					g_slist_remove(noti->callbacks, item);
				continue;
			}
		}
	}

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_at_remove_notification(TcoreAT *at,
	const gchar *prefix, TcoreAtNotificationCallback callback)
{
	return tcore_at_remove_notification_full(at, prefix, callback, NULL);
}

TelReturn tcore_at_add_notification(TcoreAT *at, const gchar *prefix,
	gboolean pdu, TcoreAtNotificationCallback callback, void *user_data)
{
	struct _notification *noti;
	struct _notification_callback *item;

	if ((at == NULL) || !prefix || !callback)
		return TEL_RETURN_INVALID_PARAMETER;

	noti = g_hash_table_lookup(at->unsolicited_table, prefix);
	if (noti == NULL) {
		noti = g_try_new0(struct _notification, 1);
		if (!noti)
			return TEL_RETURN_MEMORY_FAILURE;

		noti->type_pdu = pdu;
		noti->callbacks = NULL;

		g_hash_table_insert(at->unsolicited_table, tcore_strdup(prefix), noti);
	}

	if (noti->type_pdu != pdu)
		return TEL_RETURN_INVALID_PARAMETER;

	item = g_try_new0(struct _notification_callback, 1);
	if (item == NULL)
		return TEL_RETURN_MEMORY_FAILURE;

	item->callback = callback;
	item->user_data = user_data;

	noti->callbacks = g_slist_append(noti->callbacks, item);

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_at_set_request(TcoreAT *at, TcoreAtRequest *req)
{
	if (at == NULL) {
		err("AT-Command is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	at->req = req;

	if (req) {
		dbg("AT Request - Command: [%s] Pre-fix: [%s] Type: [%d])",
			at->req->cmd, at->req->prefix, at->req->type);
	}
	else {
		err("AT request is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_at_send_data(TcoreAT *at, TcoreAtRequest *req, gboolean send)
{
	TelReturn ret;
	gchar *end;
	gchar next;

	if (at == NULL) {
		err("AT-Command is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (req == NULL) {
		err("AT request is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if ((ret = tcore_at_set_request(at, req)) != TEL_RETURN_SUCCESS) {
		err("Failed to set AT request");
		return ret;
	}

	if (send == FALSE)
		return TEL_RETURN_SUCCESS;

	end = strchr(at->req->cmd, CR);
	next = *(end + 1);
	if (next == '\0')
		return tcore_hal_send_data(at->hal, strlen(req->cmd), req->cmd);

	dbg("backup data = [%c]", next);
	at->req->next_send_pos = end + 1;
	dbg("next data = [%s]", at->req->next_send_pos);

	*(end + 1) = '\0';
	ret = tcore_hal_send_data(at->hal, strlen(req->cmd), req->cmd);
	*(end + 1) = next;

	return ret;
}

TcoreAtRequest *tcore_at_get_request(TcoreAT *at)
{
	if (at == NULL) {
		err("AT-Command is NULL");
		return NULL;
	}

	return at->req;
}


TcoreAtResponse *tcore_at_get_response(TcoreAT *at)
{
	if (at == NULL) {
		err("AT-Command is NULL");
		return NULL;
	}

	return at->resp;
}

TelReturn tcore_at_buf_write(TcoreAT *at, guint data_len, const gchar *data)
{
	guint read_pos;
	guint write_pos;

	if (at == NULL) {
		err("AT-Command is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

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
			at->buf_read_pos - at->buf, at->buf_write_pos - at->buf);
		memset(at->buf_write_pos, 0x0,
			at->buf_size - (at->buf_write_pos - at->buf));
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
		memset(at->buf_write_pos, 0x0,
			at->buf_size - (at->buf_write_pos - at->buf));
		dbg("resize buffer to %d", at->buf_size);
	}

	memcpy(at->buf_write_pos, data, data_len);

	at->buf_write_pos += data_len;

	return TEL_RETURN_SUCCESS;
}

TcoreAtRequest *tcore_at_request_new(const gchar *cmd,
	const gchar *prefix, TcoreAtCommandType type)
{
	TcoreAtRequest *at_req;

	if (cmd == NULL) {
		err("AT-Command is NULL");
		return NULL;
	}

	if (strlen(cmd) < 1) {
		err("Invalid AT-Command length: [%d]", strlen(cmd));
		return NULL;
	}

	at_req = g_try_new0(TcoreAtRequest, 1);
	if (at_req == NULL) {
		err("Failed to allocate memory");
		return NULL;
	}

	if (strchr(cmd, CR) == 0)
		at_req->cmd = g_strdup_printf("%s%c", cmd, CR);
	else
		at_req->cmd = g_strdup_printf("%s%c", cmd, 26);

	if (prefix)
		at_req->prefix = tcore_memdup(prefix, strlen(prefix) + 1);

	at_req->type = type;
	dbg("AT-Command: [%s] Prefix(if any): [%s] AT-Command length: [%d]",
		at_req->cmd, at_req->prefix, strlen(at_req->cmd));

	return at_req;
}

void tcore_at_request_free(TcoreAtRequest *at_req)
{
	if (at_req == NULL) {
		err("AT-Command is NULL");
		return;
	}

	tcore_free(at_req->cmd);
	tcore_free(at_req->prefix);
	tcore_free(at_req);
}

/* To get the length value from little-endian bytes */
static gint __sum_4_bytes(const gchar *posn)
{
	gint sum = 0;
	sum = sum | (*(posn+3)) << 24;
	sum = sum | (*(posn+2)) << 16;
	sum = sum | (*(posn+1)) << 8;
	sum = sum | *posn ;
	return sum;
}

/* Function to process binary data received as part of XDRV Indication */
void tcore_at_process_binary_data(TcoreAT *at, gchar *position, guint data_len)
{

	#define NVM_PAYLOAD_LENGTH_0			52
	#define NVM_PAYLOAD_LENGTH_1			68

	gint m_length_0 = ZERO , m_length_1 = ZERO;
	static gint data_len_final = ZERO, actual_buffer_size = ZERO;
	dbg("Entered");

	m_length_0 = __sum_4_bytes(&position[NVM_PAYLOAD_LENGTH_0]);
	m_length_1 = __sum_4_bytes(&position[NVM_PAYLOAD_LENGTH_1]);
	data_len_final = data_len_final + data_len;

	dbg("m_length_0 = %d , m_length_1 = %d, data_len_final = %d actual_buffer_size: %d",
		m_length_0, m_length_1, data_len_final, actual_buffer_size);
	if (actual_buffer_size == ZERO) {
		actual_buffer_size = data_len + m_length_0 + m_length_1;
		dbg("Actual buffer size is %d", actual_buffer_size);
	}

	if (data_len_final == actual_buffer_size) {
		__at_emit_unsolicited_message(at, position);
		at->data_mode = MODE_HEX;
		at->buf_read_pos = at->buf_read_pos + (actual_buffer_size + 1);
		data_len_final = ZERO;
		actual_buffer_size = ZERO;
	}
	dbg("Exit");
}

gboolean tcore_at_process(TcoreAT *at, guint data_len, const gchar *data)
{
	gchar *pos;
	gchar *next_pos;
	TcoreAtRcvMsgType ret;

	if ((at == NULL) || (data == NULL))
		return FALSE;

	tcore_at_buf_write(at, data_len, data);

	pos = at->buf_read_pos;
	dbg("On entry at->buf_read_pos: 0x%x", at->buf_read_pos);

	while (1) {
		while (*pos == CR || *pos == LF)
			pos++;

		next_pos = _find_next_EOL(pos);
		if (!next_pos) {
			dbg("Data could be in Binary mode !!");
			if (at->rfs_hook) {
				if (TRUE == at->rfs_hook(pos)){
					at->data_mode = MODE_BIN;
					tcore_at_process_binary_data(at, pos, data_len);
				}
				dbg("Not Binary data");
			}else
				dbg("Rfs hook is not set !!");
			break;
		}
		if (pos != next_pos)
			*next_pos = '\0';

		dbg("line = [%s]", pos);

		/* Check request */
		if (at->req == NULL) {
			dbg("Not AT request" );
			__at_emit_unsolicited_message(at, pos);
		}
		else {
			dbg("AT request" );
			if (g_strcmp0(pos, "> ") == 0) {
				if (at->req->next_send_pos) {
					dbg("send next: [%s]", at->req->next_send_pos);
					tcore_hal_send_data(at->hal,
						strlen(at->req->next_send_pos),
						at->req->next_send_pos);

					pos += 2;
					at->buf_read_pos = pos;
					break;
				}
			}

			if (at->resp == NULL)
				at->resp = __at_response_new();

			ret = __check_final_response(pos);
			switch (ret) {
			case TCORE_AT_RECV_MSG_TYPE_RESP_SUCCESS:
			case TCORE_AT_RECV_MSG_TYPE_RESP_FAILURE: {
				at->resp->success =
					(ret == TCORE_AT_RECV_MSG_TYPE_RESP_SUCCESS
					? TRUE : FALSE);

				at->resp->final_response = tcore_strdup(pos);

				__at_emit_pending_response(at, __at_is_abort(at->resp));
				at->buf_read_pos = next_pos + 1;

				return TRUE;
			}

			case TCORE_AT_RECV_MSG_TYPE_NOTI: {
				switch (at->req->type) {
				case TCORE_AT_COMMAND_TYPE_NO_RESULT:
					__at_emit_unsolicited_message(at, pos);
					break;

				case TCORE_AT_COMMAND_TYPE_NUMERIC:
					if (at->resp->lines == NULL && isdigit(pos[0]))
						__at_response_add(at->resp, pos);
					else
						__at_emit_unsolicited_message(at, pos);
				break;

				case TCORE_AT_COMMAND_TYPE_SINGLELINE:
					dbg("SINGLELINE");
					if (at->resp->lines == NULL)
						if (at->req->prefix)
							if (g_str_has_prefix(pos, at->req->prefix))
								__at_response_add(at->resp, pos);
							else
								__at_emit_unsolicited_message(at, pos);
						else
							__at_response_add(at->resp, pos);
					else
						__at_emit_unsolicited_message(at, pos);
				break;

				case TCORE_AT_COMMAND_TYPE_MULTILINE:
					dbg("MULTILINE");
					if (at->req->prefix)
						if (g_str_has_prefix(pos, at->req->prefix))
							__at_response_add(at->resp, pos);
						else
							__at_emit_unsolicited_message(at, pos);
					else
						__at_response_add(at->resp, pos);
				break;

				case TCORE_AT_COMMAND_TYPE_PDU:
					dbg("PDU");
					if (at->req->prefix)
						if (g_str_has_prefix(pos, at->req->prefix))
							__at_response_add(at->resp, pos);
						else
							if (at->resp->lines != NULL)
								__at_response_add(at->resp, pos);
							else
								__at_emit_unsolicited_message(at, pos);
					else
						__at_response_add(at->resp, pos);
				break;

				default:
					warn("UNKNOWN");
					__at_emit_unsolicited_message(at, pos);
				break;
				}	/* switch (at->req->type) */
			} break;
			}	/* switch (ret) */
		}

		//
		pos = next_pos + 1;
		at->buf_read_pos = pos;
	}

	dbg("On exit at->buf_read_pos: 0x%x", at->buf_read_pos);
	return FALSE;
}

TcorePending *tcore_at_pending_new(CoreObject *co,
	const gchar *cmd, const gchar *prefix, TcoreAtCommandType type,
	TcorePendingResponseCallback func, void *user_data)
{
	TcorePending *p;
	TcoreAtRequest *req;

	if (cmd == NULL)
		return NULL;

	req = tcore_at_request_new(cmd, prefix, type);
	if (req == NULL)
		return NULL;

	p = tcore_pending_new(co, 0);
	if (p == NULL) {
		tcore_at_request_free(req);
		return NULL;
	}

	tcore_pending_set_request_data(p, 0, req);
	tcore_pending_set_response_callback(p, func, user_data);

	return p;
}

GSList *tcore_at_tok_new(const gchar *line)
{
	gchar *begin;
	gchar *pos;
	gchar *buf = NULL;
	gchar *mark_end = NULL;
	TcoreAtTokenType type = TCORE_AT_TOKEN_TYPE_NONE;
	GSList *tokens = NULL;

	if (line == NULL)
		return NULL;

	if (strlen(line) == 0)
		return NULL;

	if (line[0] == '(') {
		/* list token container */
		pos = (gchar *) line;
		if (line[strlen(line) - 1] == ')')
			mark_end = (gchar *) line + strlen(line) - 1;
	} else {
		/* normal AT message */
		pos = strchr(line, ':');
		if (pos == NULL) {
			tokens = g_slist_append(tokens, tcore_strdup(line));
			return tokens;
		}
	}

	pos++;

	/* skip whitespace(s) */
	while (*pos != '\0' && isspace(*pos))
		pos++;

	begin = pos;

	do {
		switch (type) {
		case TCORE_AT_TOKEN_TYPE_NONE: {
			if (*pos == '"')
				type = TCORE_AT_TOKEN_TYPE_STR;
			else if (*pos == ',')
				tokens = g_slist_append(tokens, tcore_strdup(""));
			else if (*pos == '(')
				type = TCORE_AT_TOKEN_TYPE_PAREN;
			else if (*pos == ' ')
				;		/* skip */
			else
				type = TCORE_AT_TOKEN_TYPE_RAW;

			begin = pos;
		} break;

		case TCORE_AT_TOKEN_TYPE_STR: {
			if (*pos == '"') {
				type = TCORE_AT_TOKEN_TYPE_STR_FIN;

				buf = tcore_try_malloc0(pos - begin + 2);
				memcpy(buf, begin, pos - begin + 1);
				tokens = g_slist_append(tokens, buf);
			}
		} break;

		case TCORE_AT_TOKEN_TYPE_PAREN: {
			if (*pos == ')') {
				type = TCORE_AT_TOKEN_TYPE_PAREN_FIN;

				buf = tcore_try_malloc0(pos - begin + 2);
				memcpy(buf, begin, pos - begin + 1);
				tokens = g_slist_append(tokens, buf);
			}
		} break;

		case TCORE_AT_TOKEN_TYPE_RAW: {
			if (*pos == ',' || *pos == '\0') {
				type = TCORE_AT_TOKEN_TYPE_NONE;
				buf = tcore_try_malloc0(pos - begin + 1);
				memcpy(buf, begin, pos - begin);
				tokens = g_slist_append(tokens, buf);
			}
		} break;

		case TCORE_AT_TOKEN_TYPE_STR_FIN:
		case TCORE_AT_TOKEN_TYPE_PAREN_FIN: {
			if (*pos == ',')
				type = TCORE_AT_TOKEN_TYPE_NONE;
		} break;
		}

		if (*pos == '\0' || pos == mark_end)
			break;

		pos++;
	} while (1);

	if (type == TCORE_AT_TOKEN_TYPE_RAW) {
		buf = tcore_try_malloc0(pos - begin + 1);
		memcpy(buf, begin, pos - begin);
		tokens = g_slist_append(tokens, buf);
	}

	return tokens;
}

void tcore_at_tok_free(GSList *tokens)
{
	if (tokens != NULL)
		g_slist_free_full(tokens, g_free);
}

gchar *tcore_at_tok_extract(const gchar *src)
{
	gchar *dest = NULL;
	gchar *last = NULL;

	if (src == NULL)
		return NULL;

	if (strlen(src) < 2)
		return tcore_strdup(src);

	last = (gchar *) src + strlen(src) - 1;

	switch (*src) {
	case '(':
		if (*last == ')') {
			dest = tcore_strdup(src + 1);
			dest[strlen(dest) - 1] = '\0';
		}
		break;

	case '"':
		if (*last == '"') {
			dest = tcore_strdup(src + 1);
			dest[strlen(dest) - 1] = '\0';
		}
		break;

	default:
		return tcore_strdup(src);
	}

	return dest;
}

gchar *tcore_at_tok_nth(GSList *tokens, guint index)
{
	if (tokens == NULL)
		return NULL;

	if (index > g_slist_length(tokens))
		return NULL;

	return (gchar *) g_slist_nth_data(tokens, index);
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

TelReturn tcore_at_prepare_and_send_request(CoreObject *co,
	const gchar *cmd, const gchar *prefix,
	TcoreAtCommandType type, void *request,
	TcorePendingResponseCallback resp_cb, void *resp_cb_data,
	TcorePendingSendCallback send_cb, void *send_cb_data)
{
	TcorePending *pending;
	TcoreAtRequest *at_req;
	TcoreHal *hal;
	TelReturn ret = TEL_RETURN_FAILURE;

	hal = tcore_object_get_hal(co);
	if (hal == NULL) {
		err("HAL is NULL");
		return ret;
	}
	dbg("hal: [%p]", hal);

	/* Create Pending Request */
	pending = tcore_pending_new(co, 0);
	if (pending == NULL) {
		err("Pending is NULL");
		return ret;
	}

	/* Create AT-Command Request */
	at_req = tcore_at_request_new(cmd, prefix, type);
	if (at_req == NULL) {
		err("Request is NULL");

		tcore_pending_free(pending);
		return ret;
	}

	tcore_pending_set_request_data(pending, 0, at_req);
	tcore_pending_link_request(pending, request);

	tcore_pending_set_priority(pending, TCORE_PENDING_PRIORITY_DEFAULT);

	tcore_pending_set_response_callback(pending, resp_cb, resp_cb_data);
	tcore_pending_set_send_callback(pending, send_cb, send_cb_data);

	ret = tcore_hal_send_request(hal, pending);
	dbg("ret: [0x%x]", ret);

	return ret;
}

/*
 * Extended API
 *	Provides more control on the AT-Command request being sent
 */
TelReturn tcore_at_prepare_and_send_request_ex(CoreObject *co,
	const gchar *cmd, const gchar *prefix, TcoreAtCommandType type,
	TcorePendingPriority priority, void *request,
	TcorePendingResponseCallback resp_cb, void *resp_cb_data,
	TcorePendingSendCallback send_cb, void *send_cb_data,
	guint timeout, TcorePendingTimeoutCallback timeout_cb, void *timeout_cb_data,
	gboolean auto_free, gboolean abortable)
{
	TcorePending *pending;
	TcoreAtRequest *at_req;
	TcoreHal *hal;
	TelReturn ret = TEL_RETURN_FAILURE;

	hal = tcore_object_get_hal(co);
	if (hal == NULL) {
		err("HAL is NULL");
		return ret;
	}
	dbg("hal: [%p]", hal);

	/* Create Pending Request */
	pending = tcore_pending_new(co, 0);
	if (pending == NULL) {
		err("Pending is NULL");
		return ret;
	}

	/* Create AT-Command Request */
	at_req = tcore_at_request_new(cmd, prefix, type);
	if (at_req == NULL) {
		err("Request is NULL");

		tcore_pending_free(pending);
		return ret;
	}

	tcore_pending_set_request_data(pending, 0, at_req);
	tcore_pending_link_request(pending, request);

	tcore_pending_set_priority(pending, priority);

	tcore_pending_set_response_callback(pending, resp_cb, resp_cb_data);
	tcore_pending_set_send_callback(pending, send_cb, send_cb_data);

	if (timeout > 0)
		tcore_pending_set_timeout(pending, timeout);
	tcore_pending_set_timeout_callback(pending, timeout_cb, timeout_cb_data);

	if (auto_free)
		tcore_pending_set_auto_free_status_after_sent(pending, auto_free);

	if (abortable)
		tcore_pending_set_abortable(pending, abortable);

	ret = tcore_hal_send_request(hal, pending);
	dbg("ret: [0x%x]", ret);

	return ret;
}

