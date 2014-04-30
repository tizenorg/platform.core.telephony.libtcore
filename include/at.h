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

#ifndef __AT_H__
#define __AT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <tcore.h>
#include <queue.h>

#define ZERO		0

typedef enum {
    TCORE_AT_COMMAND_TYPE_NO_RESULT,   /* no intermediate response expected */
    TCORE_AT_COMMAND_TYPE_NUMERIC,     /* a single intermediate response starting with a 0-9 */
    TCORE_AT_COMMAND_TYPE_SINGLELINE,  /* a single intermediate response starting with a prefix */
    TCORE_AT_COMMAND_TYPE_MULTILINE,   /* multiple line intermediate response starting with a prefix */
    TCORE_AT_COMMAND_TYPE_PDU
} TcoreAtCommandType;

typedef struct {
	gchar *cmd;
	gchar *prefix;
	TcoreAtCommandType type;

	gchar *next_send_pos;
} TcoreAtRequest;

typedef struct {
	gboolean success; /* true if final response indicates success (eg "OK") */
	gchar *final_response; /* eg OK, ERROR */
	GSList *lines; /* any intermediate responses */
} TcoreAtResponse;

typedef gboolean (*TcoreAtNotificationCallback)(TcoreAT *at, const GSList *lines,
		void *user_data);

void tcore_at_process_binary_data(TcoreAT *at, gchar *position, guint data_len);
gboolean tcore_at_add_hook(TcoreHal *hal, void *hook_func);

TcoreAT *tcore_at_new(TcoreHal *hal);
void tcore_at_free(TcoreAT *at);

TelReturn tcore_at_buf_write(TcoreAT *at, guint data_len, const gchar *data);

TelReturn tcore_at_set_request(TcoreAT *at, TcoreAtRequest *req);
TelReturn tcore_at_send_data(TcoreAT *at, TcoreAtRequest *req, gboolean send);

TcoreAtRequest *tcore_at_get_request(TcoreAT *at);
TcoreAtResponse *tcore_at_get_response(TcoreAT *at);

TelReturn tcore_at_add_notification(TcoreAT *at, const gchar *prefix, gboolean pdu,
	TcoreAtNotificationCallback callback, void *user_data);
TelReturn tcore_at_remove_notification(TcoreAT *at, const gchar *prefix, TcoreAtNotificationCallback callback);
TelReturn tcore_at_remove_notification_full(TcoreAT *at, const gchar *prefix,
	TcoreAtNotificationCallback callback, void *user_data);

TcoreAtRequest *tcore_at_request_new(const gchar *cmd, const gchar *prefix, TcoreAtCommandType type);
void tcore_at_request_free(TcoreAtRequest *at_req);

gboolean tcore_at_process(TcoreAT *at, guint data_len, const gchar *data);

TcorePending *tcore_at_pending_new(CoreObject *co, const gchar *cmd,
	const gchar *prefix, TcoreAtCommandType type,
	TcorePendingResponseCallback func, void *user_data);

GSList *tcore_at_tok_new(const gchar *line);
void tcore_at_tok_free(GSList *tokens);
gchar *tcore_at_tok_extract(const gchar *src);
gchar *tcore_at_tok_nth(GSList *tokens, guint index);

TelReturn tcore_at_prepare_and_send_request(CoreObject *co,
	const gchar *cmd, const gchar *prefix,
	TcoreAtCommandType type, void *request,
	TcorePendingResponseCallback resp_cb, void *resp_cb_data,
	TcorePendingSendCallback send_cb, void *send_cb_data);
TelReturn tcore_at_prepare_and_send_request_ex(CoreObject *co,
	const gchar *cmd, const gchar *prefix, TcoreAtCommandType type,
	TcorePendingPriority priority, void *request,
	TcorePendingResponseCallback resp_cb, void *resp_cb_data,
	TcorePendingSendCallback send_cb, void *send_cb_data,
	guint timeout, TcorePendingTimeoutCallback timeout_cb, void *timeout_cb_data,
	gboolean auto_free, gboolean abortable);

#ifdef __cplusplus
}
#endif

#endif	/* __AT_H__ */
