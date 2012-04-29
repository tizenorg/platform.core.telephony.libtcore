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
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "queue.h"
#include "user_request.h"
#include "core_object.h"


struct tcore_queue_type {
	TcorePlugin *plugin;
	GQueue *gq;
};

struct tcore_pending_type {
	unsigned int id;

	TcorePendingSendCallback on_send;
	void *on_send_user_data;
	TcorePendingTimeoutCallback on_timeout;
	void *on_timeout_user_data;
	TcorePendingResponseCallback on_response;
	void *on_response_user_data;

	enum tcore_pending_priority priority;
	void *data;
	unsigned int data_len;

	gboolean enable;
	unsigned int timeout;
	time_t timestamp;
	gboolean flag_sent;
	gboolean flag_received_response;
	gboolean flag_auto_free_after_sent;

	UserRequest *ur;
	TcorePlugin *plugin;
	CoreObject *co;
};

TcorePending *tcore_pending_new(CoreObject *co, unsigned int id)
{
	TcorePending *p;

	p = calloc(sizeof(struct tcore_pending_type), 1);
	if (!p)
		return NULL;

	p->id = id;
	time(&p->timestamp);

	p->on_send = NULL;
	p->on_send_user_data = NULL;
	p->on_response = NULL;
	p->on_response_user_data = NULL;
	p->on_timeout = NULL;
	p->on_timeout_user_data = NULL;
	p->data = NULL;
	p->data_len = 0;
	p->timeout = 0;
	p->priority = TCORE_PENDING_PRIORITY_DEFAULT;
	p->flag_sent = FALSE;
	p->co = co;
	p->plugin = tcore_object_ref_plugin(co);

	return p;
}

void tcore_pending_free(TcorePending *pending)
{
	if (!pending)
		return;

	dbg("pending(0x%x) free, id=0x%x", pending, pending->id);

	if (pending->data)
		free(pending->data);

	free(pending);
}

unsigned int tcore_pending_get_id(TcorePending *pending)
{
	if (!pending)
		return 0;

	return pending->id;
}

TReturn tcore_pending_set_auto_free_status_after_sent(TcorePending *pending,
		gboolean flag)
{
	if (!pending)
		return TCORE_RETURN_EINVAL;

	pending->flag_auto_free_after_sent = flag;

	return TCORE_RETURN_SUCCESS;
}

gboolean tcore_pending_get_auto_free_status_after_sent(TcorePending *pending)
{
	if (!pending)
		return FALSE;

	return pending->flag_auto_free_after_sent;
}

TReturn tcore_pending_set_request_data(TcorePending *pending,
		unsigned int data_len, void *data)
{
	if (!pending)
		return TCORE_RETURN_EINVAL;

	if (pending->data) {
		free(pending->data);
		pending->data = NULL;
	}

	pending->data_len = data_len;
	if (pending->data_len > 0) {
		pending->data = calloc(data_len, 1);
		if (!pending->data)
			return TCORE_RETURN_ENOMEM;

		memcpy(pending->data, data, data_len);
	}

	return TCORE_RETURN_SUCCESS;
}

void *tcore_pending_ref_request_data(TcorePending *pending, unsigned int *data_len)
{
	if (!pending)
		return NULL;

	if (data_len)
		*data_len = pending->data_len;

	return pending->data;
}

TReturn tcore_pending_set_priority(TcorePending *pending,
		enum tcore_pending_priority priority)
{
	if (!pending)
		return TCORE_RETURN_EINVAL;

	pending->priority = priority;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_pending_get_priority(TcorePending *pending,
		enum tcore_pending_priority *result_priority)
{
	if (!pending || !result_priority)
		return TCORE_RETURN_EINVAL;

	*result_priority = pending->priority;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_pending_set_timeout(TcorePending *pending, unsigned int timeout)
{
	if (!pending)
		return TCORE_RETURN_EINVAL;

	pending->timeout = timeout;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_pending_get_send_status(TcorePending *pending,
		gboolean *result_status)
{
	if (!pending || !result_status)
		return TCORE_RETURN_EINVAL;

	*result_status = pending->flag_sent;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_pending_set_send_callback(TcorePending *pending,
		TcorePendingSendCallback func, void *user_data)
{
	if (!pending)
		return TCORE_RETURN_EINVAL;

	pending->on_send = func;
	pending->on_send_user_data = user_data;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_pending_set_timeout_callback(TcorePending *pending,
		TcorePendingTimeoutCallback func, void *user_data)
{
	if (!pending)
		return TCORE_RETURN_EINVAL;

	pending->on_timeout = func;
	pending->on_timeout_user_data = user_data;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_pending_set_response_callback(TcorePending *pending,
		TcorePendingResponseCallback func, void *user_data)
{
	if (!pending)
		return TCORE_RETURN_EINVAL;

	pending->on_response = func;
	pending->on_response_user_data = user_data;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_pending_emit_send_callback(TcorePending *pending, gboolean result)
{
	if (!pending)
		return TCORE_RETURN_EINVAL;

	pending->flag_sent = TRUE;

	if (pending->on_send)
		pending->on_send(pending, result, pending->on_send_user_data);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_pending_emit_timeout_callback(TcorePending *pending)
{
	if (!pending)
		return TCORE_RETURN_EINVAL;

	if (pending->on_timeout)
		pending->on_timeout(pending, pending->on_timeout_user_data);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_pending_emit_response_callback(TcorePending *pending,
		int data_len, const void *data)
{
	if (!pending)
		return TCORE_RETURN_EINVAL;

	if (pending->on_response)
		pending->on_response(pending, data_len, data,
				pending->on_response_user_data);

	return TCORE_RETURN_SUCCESS;
}

CoreObject *tcore_pending_ref_core_object(TcorePending *pending)
{
	if (!pending)
		return NULL;

	return pending->co;
}

TcorePlugin *tcore_pending_ref_plugin(TcorePending *pending)
{
	if (!pending)
		return NULL;

	return pending->plugin;
}

TReturn tcore_pending_link_user_request(TcorePending *pending, UserRequest *ur)
{
	if (!pending)
		return TCORE_RETURN_EINVAL;

	pending->ur = ur;

	return TCORE_RETURN_SUCCESS;
}

UserRequest *tcore_pending_ref_user_request(TcorePending *pending)
{
	if (!pending)
		return NULL;

	return pending->ur;
}

TcoreQueue *tcore_queue_new(TcorePlugin *plugin)
{
	TcoreQueue *queue;

	queue = calloc(sizeof(struct tcore_queue_type), 1);
	if (!queue)
		return FALSE;

	queue->plugin = plugin;

	queue->gq = g_queue_new();
	if (!queue->gq) {
		free(queue);
		return FALSE;
	}

	g_queue_init(queue->gq);

	return queue;
}

void tcore_queue_free(TcoreQueue *queue)
{
	if (!queue)
		return;

	if (queue->gq)
		g_queue_free(queue->gq);

	free(queue);
}

TReturn tcore_queue_push(TcoreQueue *queue, TcorePending *pending)
{
	enum tcore_pending_priority priority;

	if (!queue || !pending)
		return TCORE_RETURN_EINVAL;

	tcore_pending_get_priority(pending, &priority);
	switch (priority) {
		case TCORE_PENDING_PRIORITY_IMMEDIATELY:
		case TCORE_PENDING_PRIORITY_HIGH:
			g_queue_push_head(queue->gq, pending);
			break;

		case TCORE_PENDING_PRIORITY_DEFAULT:
		case TCORE_PENDING_PRIORITY_LOW:
			g_queue_push_tail(queue->gq, pending);
			break;

		default:
			return TCORE_RETURN_EINVAL;
			break;
	}

	dbg("pending(0x%x) push to queue. queue len=%d",
			pending, g_queue_get_length(queue->gq));

	return TCORE_RETURN_SUCCESS;
}

TcorePending *tcore_queue_pop(TcoreQueue *queue)
{
	if (!queue)
		return NULL;

	return g_queue_pop_head(queue->gq);
}

TcorePending *tcore_queue_pop_timeout_pending(TcoreQueue *queue)
{
	int i = 0;
	TcorePending *pending = NULL;
	time_t cur_time = 0;

	time(&cur_time);

	do {
		pending = g_queue_peek_nth(queue->gq, i);
		if (!pending)
			return NULL;

		if (cur_time - pending->timestamp >= (int)pending->timeout) {
			pending = g_queue_pop_nth(queue->gq, i);
			break;
		}

		i++;
	} while (pending != NULL);

	return pending;
}

TcorePending *tcore_queue_ref_head(TcoreQueue *queue)
{
	if (!queue)
		return NULL;

	return g_queue_peek_head(queue->gq);
}

TcorePending *tcore_queue_ref_tail(TcoreQueue *queue)
{
	if (!queue)
		return NULL;

	return g_queue_peek_tail(queue->gq);
}


static TcorePending *_tcore_queue_search(TcoreQueue *queue, unsigned int id,
		gboolean flag_pop)
{
	TcorePending *pending = NULL;
	int i = 0;

	if (!queue)
		return NULL;

	do {
		pending = g_queue_peek_nth(queue->gq, i);
		if (!pending)
			return NULL;

		if (pending->id == id) {
			if (flag_pop == TRUE) {
				pending = g_queue_pop_nth(queue->gq, i);
			}
			break;
		}

		i++;
	} while (pending != NULL);

	return pending;
}

TcorePending *tcore_queue_pop_by_id(TcoreQueue *queue, unsigned int id)
{
	if (!queue)
		return NULL;

	return _tcore_queue_search(queue, id, TRUE);
}

TcorePending *tcore_queue_ref_pending_by_id(TcoreQueue *queue, unsigned int id)
{
	if (!queue)
		return NULL;

	return _tcore_queue_search(queue, id, FALSE);
}

TcorePending *tcore_queue_ref_next_pending(TcoreQueue *queue)
{
	TcorePending *pending = NULL;
	int i = 0;

	if (!queue)
		return NULL;

	do {
		pending = g_queue_peek_nth(queue->gq, i);
		if (!pending) {
			return NULL;
		}

		/* skip already sent immediately pending */
		if (pending->priority == TCORE_PENDING_PRIORITY_IMMEDIATELY) {
			if (pending->flag_sent == FALSE) {
				break;
			}

			i++;
			continue;
		}
		else {
			break;
		}

		i++;
	} while (pending != NULL);

	if (pending->flag_sent == TRUE) {
		dbg("pending(0x%x) is waiting state.", pending);
		return NULL;
	}

	return pending;
}

unsigned int tcore_queue_get_length(TcoreQueue *queue)
{
	if (!queue)
		return 0;

	return g_queue_get_length(queue->gq);
}

TcorePlugin *tcore_queue_ref_plugin(TcoreQueue *queue)
{
	if (!queue)
		return NULL;

	return queue->plugin;
}
