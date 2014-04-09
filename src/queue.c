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
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "queue.h"
#include "hal.h"
#include "core_object.h"


struct tcore_queue_type {
	TcoreHal *hal;
	GQueue *gq;
	guint next_id;
};

struct tcore_pending_type {
	guint id;		/**< Unique ID: to Identify the Pending Request */

	TcorePendingPriority priority;	/**< Prority */
	gboolean enable;

	/**< Request data */
	void *data;
	guint data_len;

	/**< Flags */
	gboolean flag_sent;	/**< Sent: Request sent to modem */
	gboolean flag_received_response;	/**< Received response: Response received from modem */
	gboolean flag_auto_free_after_sent;	/**< Auto free: Not expecting Response */

	/**< Abortable Request */
	gboolean abortable;

	/**< Essential references */
	void *request;
	TcorePlugin *plugin;
	CoreObject *co;
	TcoreQueue *queue;

	/**< Timeout parameters */
	guint timer_src;	/**< Timer */
	time_t timestamp;	/**< Time stamp of request*/
	guint timeout;	/**< Timeout value*/

	/* Callbacks */
	TcorePendingSendCallback on_send;	/**< On send */
	void *on_send_user_data;
	TcorePendingResponseCallback on_response;	/**< On response */
	void *on_response_user_data;
	TcorePendingTimeoutCallback on_timeout;	/**< On timeout */
	void *on_timeout_user_data;
};

typedef enum {
	SEARCH_FIELD_ID_ALL = 0x01,
	SEARCH_FIELD_ID_WAIT = 0x11,
	SEARCH_FIELD_ID_SENT = 0x21,
	SEARCH_FIELD_COMMAND_ALL = 0x02,
	SEARCH_FIELD_COMMAND_WAIT = 0x12,
	SEARCH_FIELD_COMMAND_SENT = 0x22
} TcoreSearchField;

static gboolean __on_pending_timeout(gpointer user_data)
{
	TcorePending *pending = user_data;

	dbg("'pending' timeout!!!");

	if (pending == NULL) {
		err("pending is NULL");
		return FALSE;
	}

	/* Emit timeout callback */
	tcore_pending_emit_timeout_callback(pending);

	pending->on_response = NULL;
	tcore_hal_dispatch_response_data(pending->queue->hal, pending->id, 0, NULL);

	return FALSE;
}

static void __queue_push_head(TcoreQueue *queue, TcorePending *pending)
{
	guint i = 0;
	TcorePending *tmp;

	do {
		tmp = g_queue_peek_nth(queue->gq, i);

		/*
		 * Either NO nodes in the list or all prior nodes were of
		 * IMMEDIATE priority
		 */
		if (tmp == NULL)
			break;

		/*
		 * Push to head unless HEAD is not of IMMEDIATE priority
		 */
		if (tmp->priority != TCORE_PENDING_PRIORITY_IMMEDIATELY)
			break;

		i++;
	} while (1);

	g_queue_push_nth(queue->gq, pending, i);
}

static TcorePending *__queue_search_full(TcoreQueue *queue, guint id,
	TcoreCommand command, TcoreSearchField field, gboolean flag_pop)
{
	TcorePending *pending = NULL;
	guint i = 0;

	if (queue == NULL) {
		err("queue is NULL");
		return NULL;
	}

	do {
		pending = g_queue_peek_nth(queue->gq, i);
		if (pending == NULL) {
			err("pending is NULL");
			return NULL;
		}

		if ((field & 0xF0) == 0x10) {
			/* search option is wait pending */
			if (pending->flag_sent) {
				i++;
				continue;
			}
		}
		else if ((field & 0xF0) == 0x20) {
			/* search option is sent pending */
			if (pending->flag_sent == FALSE) {
				i++;
				continue;
			}
		}

		if ((field & 0x0F) == SEARCH_FIELD_ID_ALL) {
			if (pending->id == id) {
				if (flag_pop == TRUE)
					pending = g_queue_pop_nth(queue->gq, i);

				break;
			}
		}
#if 0
		else if ((field & 0x0F) == SEARCH_FIELD_COMMAND_ALL) {
			ur = tcore_pending_ref_request(pending);
			if (tcore_user_request_get_command(ur) == command) {
				if (flag_pop == TRUE)
					pending = g_queue_pop_nth(queue->gq, i);

				break;
			}
		}
#endif
		i++;
	} while (pending != NULL);

	return pending;
}

TcorePending *tcore_pending_new(CoreObject *co, guint id)
{
	TcorePending *pending;

	/* Create 'pending' */
	pending = g_try_new0(struct tcore_pending_type, 1);
	if (pending == NULL) {
		err("pending is NULL");
		return NULL;
	}

	/*
	 * Initialize parameters
	 *
	 * By default others are initialized to '0' or NULL.
	 */
	pending->id = id;
	pending->priority = TCORE_PENDING_PRIORITY_DEFAULT;

	time(&pending->timestamp);

	pending->co = co;
	pending->plugin = tcore_object_ref_plugin(co);

	return pending;
}

void tcore_pending_free(TcorePending *pending)
{
	TcoreHalMode hal_mode;

	if (pending == NULL) {
		err("pending is NULL");
		return;
	}

	dbg("Freeing - pending: [%p] ID: [0x%x]", (guint)pending, pending->id);

	hal_mode = tcore_hal_get_mode(pending->queue->hal);
	if ((hal_mode != TCORE_HAL_MODE_AT)
			&& (hal_mode != TCORE_HAL_MODE_TRANSPARENT)) {
		if (pending->data)
			tcore_free(pending->data);
	}

	if (pending->timer_src)
		g_source_remove(pending->timer_src);

	tcore_free(pending);
}

guint tcore_pending_get_id(TcorePending *pending)
{
	if (pending == NULL) {
		err("pending is NULL");
		return 0;
	}

	return pending->id;
}

TelReturn tcore_pending_set_auto_free_status_after_sent(TcorePending *pending,
		gboolean flag)
{
	if (pending == NULL) {
		err("pending is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	pending->flag_auto_free_after_sent = flag;

	return TEL_RETURN_SUCCESS;
}

gboolean tcore_pending_get_auto_free_status_after_sent(TcorePending *pending)
{
	if (pending == NULL) {
		err("pending is NULL");
		return FALSE;
	}

	return pending->flag_auto_free_after_sent;
}

TelReturn tcore_pending_set_abortable(TcorePending *pending,
	gboolean abortable)
{
	if (pending == NULL) {
		err("pending is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	pending->abortable = abortable;

	return TEL_RETURN_SUCCESS;
}

gboolean tcore_pending_get_abortable(TcorePending *pending)
{
	if (pending == NULL) {
		err("pending is NULL");
		return FALSE;
	}

	return pending->abortable;
}

TelReturn tcore_pending_set_request_data(TcorePending *pending,
		guint data_len, void *data)
{
	if (pending == NULL) {
		err("pending is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (pending->data) {
		if (pending->data_len != 0) {
			tcore_free(pending->data);
			pending->data = NULL;
			pending->data_len = 0;
		}
	}

	pending->data_len = data_len;
	if (data_len > 0)
		pending->data = tcore_memdup((gconstpointer)data, data_len);
	else
		pending->data = data;

	return TEL_RETURN_SUCCESS;
}

void *tcore_pending_ref_request_data(TcorePending *pending, guint *data_len)
{
	if (pending == NULL) {
		err("pending is NULL");
		return NULL;
	}

	if (data_len)
		*data_len = pending->data_len;

	return pending->data;
}

TelReturn tcore_pending_set_priority(TcorePending *pending, TcorePendingPriority priority)
{
	if (pending == NULL) {
		err("pending is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	pending->priority = priority;

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_pending_get_priority(TcorePending *pending, TcorePendingPriority *priority)
{
	if ((pending == NULL) || (priority == NULL)) {
		err("pending: [%p] priority: [%p]", pending, priority);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	*priority = pending->priority;

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_pending_set_timeout(TcorePending *pending, guint timeout)
{
	if (pending == NULL) {
		err("pending is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	pending->timeout = timeout;

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_pending_get_send_status(TcorePending *pending, gboolean *status)
{
	if ((pending == NULL) || (status == NULL)) {
		err("pending: [%p] status: [%p]", pending, status);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	*status = pending->flag_sent;

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_pending_set_send_callback(TcorePending *pending,
		TcorePendingSendCallback func, void *user_data)
{
	if (pending == NULL) {
		err("pending is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	pending->on_send = func;
	pending->on_send_user_data = user_data;

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_pending_set_timeout_callback(TcorePending *pending,
		TcorePendingTimeoutCallback func, void *user_data)
{
	if (pending == NULL) {
		err("pending is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	pending->on_timeout = func;
	pending->on_timeout_user_data = user_data;

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_pending_set_response_callback(TcorePending *pending,
		TcorePendingResponseCallback func, void *user_data)
{
	if (pending == NULL) {
		err("pending is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	pending->on_response = func;
	pending->on_response_user_data = user_data;

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_pending_emit_send_callback(TcorePending *pending,
	TelReturn send_status)
{
	if (pending == NULL) {
		err("pending is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	pending->flag_sent = TRUE;

	if (pending->on_send) {
		pending->on_send(pending,
			send_status, pending->on_send_user_data);
	}

	if (send_status == TEL_RETURN_SUCCESS) {
		if (pending->flag_auto_free_after_sent == FALSE
				&& pending->timeout > 0) {
			/* timer */
			dbg("Start pending timer!!! [Timeout: %d secs]",
				pending->timeout);
			pending->timer_src =
				g_timeout_add_seconds(pending->timeout,
					__on_pending_timeout, pending);
		}
	}

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_pending_emit_timeout_callback(TcorePending *pending)
{
	if (pending == NULL) {
		err("pending is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (pending->on_timeout)
		pending->on_timeout(pending, pending->on_timeout_user_data);

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_pending_emit_response_callback(TcorePending *pending,
		guint data_len, const void *data)
{
	if (pending == NULL) {
		err("pending is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (pending->on_response)
		pending->on_response(pending,
			data_len, data, pending->on_response_user_data);

	return TEL_RETURN_SUCCESS;
}

CoreObject *tcore_pending_ref_core_object(TcorePending *pending)
{
	if (pending == NULL) {
		err("pending is NULL");
		return NULL;
	}

	return pending->co;
}

TcorePlugin *tcore_pending_ref_plugin(TcorePending *pending)
{
	if (pending == NULL) {
		err("pending is NULL");
		return NULL;
	}

	return pending->plugin;
}

TelReturn tcore_pending_link_request(TcorePending *pending, void *request)
{
	if (pending == NULL) {
		err("pending is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	pending->request = request;

	return TEL_RETURN_SUCCESS;
}

void *tcore_pending_ref_request(TcorePending *pending)
{
	if (pending == NULL) {
		err("pending is NULL");
		return NULL;
	}

	return pending->request;
}

TcoreQueue *tcore_queue_new(TcoreHal *h)
{
	TcoreQueue *queue;

	queue = g_try_new0(struct tcore_queue_type, 1);
	if (queue == NULL) {
		err("Failed to allocate memory");
		return FALSE;
	}

	queue->hal = h;

	queue->gq = g_queue_new();
	if (queue->gq == NULL) {
		err("Failed to create Queue");

		tcore_free(queue);
		return FALSE;
	}

	g_queue_init(queue->gq);

	return queue;
}

void tcore_queue_free(TcoreQueue *queue)
{
	if (queue == NULL) {
		err("pending is NULL");
		return;
	}

	/* Free resources */
	g_queue_free(queue->gq);
	tcore_free(queue);
}

TelReturn tcore_queue_push(TcoreQueue *queue, TcorePending *pending)
{
	if ((queue == NULL) || (pending == NULL)) {
		err("queue: [%p] pending: [%p]", queue, pending);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (pending->id == 0) {
		pending->id = queue->next_id;
		queue->next_id++;
	}

	switch (pending->priority) {
	case TCORE_PENDING_PRIORITY_IMMEDIATELY:
	case TCORE_PENDING_PRIORITY_HIGH:
		__queue_push_head(queue, pending);
	break;

	case TCORE_PENDING_PRIORITY_DEFAULT:
	case TCORE_PENDING_PRIORITY_LOW:
		g_queue_push_tail(queue->gq, pending);
	break;

	default:
		err("Invalid priority: [%d]", pending->priority);
		return TEL_RETURN_INVALID_PARAMETER;
	}
	pending->queue = queue;

	dbg("Push to Queue - pending: [%p] queue length: [%d]",
		(guint)pending, g_queue_get_length(queue->gq));

	return TEL_RETURN_SUCCESS;
}

TcorePending *tcore_queue_pop(TcoreQueue *queue)
{
	if (queue == NULL) {
		err("pending is NULL");
		return NULL;
	}

	return g_queue_pop_head(queue->gq);
}

TcorePending *tcore_queue_pop_by_pending(TcoreQueue *queue,
	TcorePending *pending)
{
	TcorePending *tmp;
	guint i = 0;

	if (queue == NULL) {
		err("queue is NULL");
		return NULL;
	}

	do {
		tmp = g_queue_peek_nth(queue->gq, i);
		if (tmp == pending) {
			g_queue_pop_nth(queue->gq, i);
			return tmp;
		}

		i++;
	} while(tmp != NULL);

	return NULL;
}

TcorePending *tcore_queue_pop_abortable_pending(TcoreQueue *queue)
{
	TcorePending *pending = NULL;
	gint i = 0;

	if (queue == NULL) {
		err("queue is NULL");
		return NULL;
	}

	/*
	 * Scan through the list of 'pending' Requests and
	 * get the first Abortable Request
	 */
	do {
		pending = g_queue_peek_nth(queue->gq, i);
		if (pending == NULL) {
			err("pending is NULL");
			return NULL;
		}
		else if (tcore_pending_get_abortable(pending) == TRUE) {
			dbg("Found Abrotable 'pending' request: [%p]", pending);

			/* Pop the Abortable 'pending' Request */
			g_queue_pop_nth(queue->gq, i);
			break;
		}

		i++;
	} while (pending != NULL);

	return pending;
}
TcorePending *tcore_queue_pop_timeout_pending(TcoreQueue *queue)
{
	guint i = 0;
	TcorePending *pending = NULL;
	time_t cur_time = 0;

	time(&cur_time);

	do {
		pending = g_queue_peek_nth(queue->gq, i);
		if (pending == NULL)
			return NULL;

		if (cur_time - pending->timestamp >= (gint)pending->timeout) {
			pending = g_queue_pop_nth(queue->gq, i);
			break;
		}

		i++;
	} while (pending != NULL);

	return pending;
}

TcorePending *tcore_queue_ref_head(TcoreQueue *queue)
{
	if (queue == NULL) {
		err("queue is NULL");
		return NULL;
	}

	return g_queue_peek_head(queue->gq);
}

TcorePending *tcore_queue_ref_tail(TcoreQueue *queue)
{
	if (queue == NULL) {
		err("queue is NULL");
		return NULL;
	}

	return g_queue_peek_tail(queue->gq);
}

TcorePending *tcore_queue_search_by_command(TcoreQueue *queue,
		TcoreCommand command, gboolean flag_sent)
{
	if (flag_sent)
		return __queue_search_full(queue, 0,
			command, SEARCH_FIELD_COMMAND_SENT, FALSE);

	return __queue_search_full(queue, 0,
		command, SEARCH_FIELD_COMMAND_WAIT, FALSE);
}

TcorePending *tcore_queue_pop_by_id(TcoreQueue *queue, guint id)
{
	if (queue == NULL) {
		err("queue is NULL");
		return NULL;
	}

	return __queue_search_full(queue, id, 0, SEARCH_FIELD_ID_ALL, TRUE);
}

TcorePending *tcore_queue_ref_next_pending(TcoreQueue *queue)
{
	TcorePending *pending = NULL;
	gint i = 0;

	if (queue == NULL) {
		err("queue is NULL");
		return NULL;
	}

	do {
		pending = g_queue_peek_nth(queue->gq, i);
		if (pending == NULL) {
			err("pending is NULL");
			return NULL;
		}

		/* skip already sent immediately pending */
		if (pending->priority == TCORE_PENDING_PRIORITY_IMMEDIATELY) {
			if (pending->flag_sent == FALSE)
				break;

			i++;
			continue;
		}
		else
			break;
	} while (pending != NULL);

	if (pending->flag_sent == TRUE) {
		dbg("pending: [%p] is waiting state", (guint)pending);
		return NULL;
	}

	return pending;
}

TcorePending *tcore_queue_ref_pending_by_id(TcoreQueue *queue, guint id)
{
	if (queue == NULL) {
		err("queue is NULL");
		return NULL;
	}

	return __queue_search_full(queue, id, 0, SEARCH_FIELD_ID_ALL, FALSE);
}

guint tcore_queue_get_length(TcoreQueue *queue)
{
	if (queue == NULL) {
		err("queue is NULL");
		return 0;
	}

	return g_queue_get_length(queue->gq);
}

TcoreHal *tcore_queue_ref_hal(TcoreQueue *queue)
{
	if (queue == NULL) {
		err("queue is NULL");
		return NULL;
	}

	return queue->hal;
}

TelReturn tcore_queue_cancel_pending_by_command(TcoreQueue *queue,
	TcoreCommand command)
{
	TcorePending *pending;

	if (queue == NULL) {
		err("queue is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	while (1) {
		pending = __queue_search_full(queue, 0,
			command, SEARCH_FIELD_COMMAND_ALL, FALSE);
		if (pending == NULL)
			break;

		dbg("pending: [%p] cancel", (guint)pending);

		if (queue->hal) {
			tcore_hal_dispatch_response_data(queue->hal,
				pending->id, 0, NULL);
		}
		else {
			pending = tcore_queue_pop_by_pending(queue, pending);
			tcore_pending_emit_response_callback(pending, 0, NULL);
			tcore_pending_free(pending);
		}
	}

	return TEL_RETURN_SUCCESS;
}
