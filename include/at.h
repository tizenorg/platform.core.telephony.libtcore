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

#ifndef __TCORE_AT_H__
#define __TCORE_AT_H__

__BEGIN_DECLS

#include <tcore.h>
#include <queue.h>

enum tcore_at_command_type {
	TCORE_AT_NO_RESULT, /* no intermediate response expected */
	TCORE_AT_NUMERIC, /* a single intermediate response starting with a 0-9 */
	TCORE_AT_SINGLELINE, /* a single intermediate response starting with a prefix */
	TCORE_AT_MULTILINE, /* multiple line intermediate response starting with a prefix */
	TCORE_AT_PDU
};

enum ATCMEError {
	AT_ERROR_MOBILE_FAILRURE = 0,
	AT_ERROR_NOT_CONNECTED_TO_PHONE = 1,
	AT_ERROR_LINK_RESERVED = 2,
	AT_ERROR_OPER_NOT_ALLOWED = 3,
	AT_ERROR_OPER_NOT_SUPPORTED = 4,
	AT_ERROR_PH_SIM_PIN_REQUIRED = 5,
	AT_ERROR_PH_FSIM_PIN_REQUIRED = 6,
	AT_ERROR_PH_FSIM_PUK_REQUIRED = 7,

	AT_ERROR_SIM_NOT_INSERTED = 10,
	AT_ERROR_SIM_PIN_REQUIRED = 11,
	AT_ERROR_SIM_PUK_REQUIRED = 12,
	AT_ERROR_SIM_FAILURE = 13,
	AT_ERROR_SIM_BUSY = 14,
	AT_ERROR_SIM_WRONG = 15,
	AT_ERROR_INCORRECT_PWD = 16,
	AT_ERROR_SIM_PIN2_REQUIRED = 17,
	AT_ERROR_SIM_PUK2_REQUIRED = 18,

	AT_ERROR_MEMORY_FULL = 20,
	AT_ERROR_INVALID_INDEX = 21,
	AT_ERROR_NOT_FOUND = 22,
	AT_ERROR_MEMORY_FAILURE = 23,
	AT_ERROR_TEXT_TOO_LONG = 24,
	AT_ERROR_INVALID_CHAR_IN_STR = 25,
	AT_ERROR_DIAL_TOO_LONG = 26,
	AT_ERROR_INVALID_CHAR_IN_DIAL = 27,

	AT_ERROR_NO_NETWORK_SVC = 30,
	AT_ERROR_NETWORK_TIMEOUT = 31,
	AT_ERROR_EMERGENCY_CALL_ONLY = 32,

	AT_ERROR_NET_PERSONAL_PIN_REQ = 40,
	AT_ERROR_NET_PERSONAL_PUN_REQ = 41,
	AT_ERROR_NET_SUB_PERSONAL_PIN_REQ = 42,
	AT_ERROR_NET_SUB_PERSONAL_PUK_REQ = 43,
	AT_ERROR_PROVIDER_PERSONAL_PIN_REQ = 44,
	AT_ERROR_PROVIDER_PERSONAL_PUK_REQ = 45,
	AT_ERROR_CORP_PERSONAL_PIN_REQ = 46,
	AT_ERROR_CORP_PERSONAL_PUK_REQ = 47,
	AT_ERROR_HIDDEN_KEY_REQUIRED = 48,
	AT_ERROR_EAP_METHOD_NOT_SUPPORTED = 49,
	AT_ERROR_INCORRECT_PARAM = 50,

	AT_ERROR_UNKNOWN = 100
};


struct tcore_at_request {
	char *cmd;
	char *next_send_pos;
	char *prefix;
	enum tcore_at_command_type type;
};

struct tcore_at_response {
	int success; /* true if final response indicates success (eg "OK") */
	char *final_response; /* eg OK, ERROR */
	GSList *lines; /* any intermediate responses */
};

typedef gboolean (* TcoreATNotificationCallback)(TcoreAT *at,
	const GSList *lines, void *user_data);

typedef struct tcore_at_response TcoreATResponse;
typedef struct tcore_at_request TcoreATRequest;

TcoreAT *tcore_at_new(TcoreHal *hal);
void tcore_at_free(TcoreAT *at);

TReturn tcore_at_buf_write(TcoreAT *at,
	unsigned int data_len, const char *data);

TReturn tcore_at_set_request(TcoreAT *at, TcoreATRequest *req, gboolean send);
TcoreATRequest *tcore_at_get_request(TcoreAT *at);
TcoreATResponse *tcore_at_get_response(TcoreAT *at);

void tcore_at_process_binary_data(TcoreAT *at, char *position, int data_len);

/* RFS Hook callback function proto-type */
typedef gboolean (*RfsHookCb)(const gchar *data);
gboolean tcore_at_add_hook(TcoreHal *hal, void *hook_func);

TReturn tcore_at_add_notification(TcoreAT *at, const char *prefix,
	gboolean pdu, TcoreATNotificationCallback callback, void *user_data);
TReturn tcore_at_remove_notification(TcoreAT *at, const char *prefix,
	TcoreATNotificationCallback callback);
TReturn tcore_at_remove_notification_full(TcoreAT *at, const char *prefix,
	TcoreATNotificationCallback callback, void *user_data);

TcoreATRequest *tcore_at_request_new(const char *cmd, const char *prefix,
	enum tcore_at_command_type type);
void tcore_at_request_free(TcoreATRequest *req);
void tcore_at_response_free(TcoreATResponse* rsp);

gboolean tcore_at_process(TcoreAT *at,
	unsigned int data_len, const char *data);

TcorePending *tcore_at_pending_new(CoreObject *co, const char *cmd,
	const char *prefix, enum tcore_at_command_type type,
	TcorePendingResponseCallback func, void *user_data);

GSList *tcore_at_tok_new(const char *line);
void tcore_at_tok_free(GSList *tokens);
char *tcore_at_tok_extract(const char *src);
char *tcore_at_tok_nth(GSList *tokens, unsigned int token_index);
void tcore_free_pending_timeout_at_request(TcoreAT *at);

TReturn tcore_prepare_and_send_at_request(CoreObject *co,
	const char *at_cmd, const char *at_cmd_prefix, enum tcore_at_command_type at_cmd_type,
	UserRequest *ur,
	TcorePendingResponseCallback resp_cb, void *resp_cb_data,
	TcorePendingSendCallback send_cb, void *send_cb_data,
	unsigned int timeout, TcorePendingTimeoutCallback timeout_cb, void *timeout_cb_data);

__END_DECLS

#endif /* __TCORE_AT_H__ */
