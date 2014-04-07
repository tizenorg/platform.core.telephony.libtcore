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

#ifndef __QUEUE_H__
#define __QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*TcorePendingSendCallback)(TcorePending *p,
	TelReturn send_status, void *user_data);

typedef void(*TcorePendingTimeoutCallback)(TcorePending *p, void *user_data);

typedef void(*TcorePendingResponseCallback)(TcorePending *p,
	guint data_len, const void *data, void *user_data);

typedef enum {
	TCORE_PENDING_PRIORITY_IMMEDIATELY = 0,
	TCORE_PENDING_PRIORITY_HIGH = 100,
	TCORE_PENDING_PRIORITY_DEFAULT = 200,
	TCORE_PENDING_PRIORITY_LOW = 300,
} TcorePendingPriority;

TcorePending *tcore_pending_new(CoreObject *co, guint id);
void tcore_pending_free(TcorePending *pending);

guint  tcore_pending_get_id(TcorePending *pending);

TelReturn tcore_pending_set_auto_free_status_after_sent(TcorePending *pending,
	gboolean flag);
gboolean tcore_pending_get_auto_free_status_after_sent(TcorePending *pending);

TelReturn tcore_pending_set_request_data(TcorePending *pending,
	guint data_len, void *data);
void *tcore_pending_ref_request_data(TcorePending *pending, guint *data_len);

TelReturn tcore_pending_set_timeout(TcorePending *pending, guint timeout);

TcorePlugin *tcore_pending_ref_plugin(TcorePending *pending);
CoreObject *tcore_pending_ref_core_object(TcorePending *pending);

TelReturn tcore_pending_set_priority(TcorePending *pending,
	TcorePendingPriority priority);
TelReturn tcore_pending_get_priority(TcorePending *pending,
	TcorePendingPriority *priority);

TelReturn tcore_pending_get_send_status(TcorePending *pending, gboolean *status);

TelReturn tcore_pending_link_request(TcorePending *pending, void *request);
void *tcore_pending_ref_request(TcorePending *pending);

TelReturn tcore_pending_set_send_callback(TcorePending *pending,
	TcorePendingSendCallback func, void *user_data);
TelReturn tcore_pending_emit_send_callback(TcorePending *pending,
	TelReturn send_status);

TelReturn tcore_pending_set_timeout_callback(TcorePending *pending,
	TcorePendingTimeoutCallback func, void *user_data);
TelReturn tcore_pending_emit_timeout_callback(TcorePending *pending);

TelReturn tcore_pending_set_response_callback(TcorePending *pending,
	TcorePendingResponseCallback func, void *user_data);
TelReturn tcore_pending_emit_response_callback(TcorePending *pending,
	guint data_len, const void *data);


TcoreQueue *tcore_queue_new(TcoreHal *hal);
void tcore_queue_free(TcoreQueue *queue);

TelReturn tcore_queue_push(TcoreQueue *queue, TcorePending *pending);
TcorePending *tcore_queue_pop(TcoreQueue *queue);
TcorePending *tcore_queue_pop_by_id(TcoreQueue *queue, guint id);
TcorePending *tcore_queue_pop_by_pending(TcoreQueue *queue,
	TcorePending *pending);
TcorePending *tcore_queue_pop_timeout_pending(TcoreQueue *queue);

TcorePending *tcore_queue_ref_head(TcoreQueue *queue);
TcorePending *tcore_queue_ref_tail(TcoreQueue *queue);
TcorePending *tcore_queue_ref_pending_by_id(TcoreQueue *queue, guint id);
TcorePending *tcore_queue_ref_next_pending(TcoreQueue *queue);

guint tcore_queue_get_length(TcoreQueue *queue);

TcoreHal *tcore_queue_ref_hal(TcoreQueue *queue);
TelReturn tcore_queue_cancel_pending_by_command(TcoreQueue *queue,
	TcoreCommand command);

TcorePending *tcore_queue_search_by_command(TcoreQueue *queue,
	TcoreCommand command, gboolean flag_sent);

#ifdef __cplusplus
}
#endif

#endif	/* __QUEUE_H__ */
