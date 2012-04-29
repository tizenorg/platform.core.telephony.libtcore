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
#include "hal.h"
#include "queue.h"
#include "plugin.h"
#include "user_request.h"

//#define IDLE_SEND_PRIORITY G_PRIORITY_DEFAULT
#define IDLE_SEND_PRIORITY G_PRIORITY_HIGH

struct hook_send_type {
	TcoreHalSendHook func;
	void *user_data;
};

struct recv_callback_item_type {
	TcoreHalReceiveCallback func;
	void *user_data;
};

struct tcore_hal_type {
	TcorePlugin *parent_plugin;
	TcoreQueue *queue;
	char *name;
	struct tcore_hal_operations *ops;
	void *user_data;
	GSList *callbacks;
	gboolean power_state;
	GSList *hook_list_send;
};

static gboolean _hal_idle_send(void *user_data)
{
	TcoreHal *h = user_data;
	TcorePending *p = NULL;
	TcoreQueue *q;
	int ret = 0;
	void *data = NULL;
	unsigned int data_len = 0;
	gboolean renew = FALSE;

	if (!h)
		return FALSE;

	msg("--[Queue SEND]-------------------");

	p = tcore_queue_ref_next_pending(h->queue);
	if (!p) {
		dbg("next pending is NULL. no send, queue len=%d", tcore_queue_get_length(h->queue));
		goto out;
	}

	data = tcore_pending_ref_request_data(p, &data_len);
	dbg("queue len=%d, pending=0x%x, id=0x%x, data_len=%d",
			tcore_queue_get_length(h->queue), p, tcore_pending_get_id(p), data_len);

	ret = tcore_hal_send_data(h, data_len, data);
	if (ret != TCORE_RETURN_HOOK_STOP) {
		tcore_pending_emit_send_callback(p, ret);

		if (tcore_pending_get_auto_free_status_after_sent(p)) {
			q = tcore_hal_ref_queue(h);
			p = tcore_queue_pop(q);
			tcore_pending_free(p);

			/* renew idler */
			renew = TRUE;
		}
		else {
			/* Send fail */
			if (ret != TCORE_RETURN_SUCCESS) {
				dbg("send fail.");
				q = tcore_hal_ref_queue(h);
				p = tcore_queue_pop(q);
				tcore_pending_free(p);
			}
		}
	}

out:
	msg("--[Queue SEND FINISH]------------\n");
	return renew;
}

TcoreHal *tcore_hal_new(TcorePlugin *plugin, const char *name,
		struct tcore_hal_operations *hops)
{
	TcoreHal *h;

	if (!plugin || !name)
		return NULL;

	h = calloc(sizeof(struct tcore_hal_type), 1);
	if (!h)
		return NULL;

	h->parent_plugin = plugin;
	h->ops = hops;
	h->name = strdup(name);
	h->queue = tcore_queue_new(plugin);

	tcore_plugin_set_hal(plugin, h);

	return h;
}

void tcore_hal_free(TcoreHal *hal)
{
	if (!hal)
		return;

	dbg("hal=%s", hal->name);

	if (hal->name)
		free(hal->name);

	if (hal->callbacks)
		g_slist_free(hal->callbacks);

	if (hal->queue)
		tcore_queue_free(hal->queue);

	free(hal);
}

TReturn tcore_hal_set_name(TcoreHal *hal, const char *name)
{
	if (!hal)
		return TCORE_RETURN_EINVAL;

	if (hal->name) {
		free(hal->name);
		hal->name = NULL;
	}

	if (name)
		hal->name = strdup(name);

	return TCORE_RETURN_SUCCESS;
}

char *tcore_hal_get_name(TcoreHal *hal)
{
	if (!hal)
		return NULL;

	if (hal->name)
		return strdup(hal->name);

	return NULL;
}

TReturn tcore_hal_link_user_data(TcoreHal *hal, void *user_data)
{
	if (!hal)
		return TCORE_RETURN_EINVAL;

	hal->user_data = user_data;

	return TCORE_RETURN_SUCCESS;
}

void *tcore_hal_ref_user_data(TcoreHal *hal)
{
	if (!hal)
		return NULL;

	return hal->user_data;
}

/* Send data without Queue */
TReturn tcore_hal_send_data(TcoreHal *hal, unsigned int data_len, void *data)
{
	struct hook_send_type *hook;
	GSList *list;

	if (!hal || !hal->ops || !hal->ops->send)
		return TCORE_RETURN_EINVAL;

	for (list = hal->hook_list_send; list; list = list->next) {
		hook = list->data;
		if (!hook) {
			continue;
		}

		if (hook->func(hal, data_len, data, hook->user_data) == TCORE_HOOK_RETURN_STOP_PROPAGATION) {
			return TCORE_RETURN_HOOK_STOP;
		}
	}

	return hal->ops->send(hal, data_len, data);
}

/* Send data by Queue */
TReturn tcore_hal_send_request(TcoreHal *hal, TcorePending *plugin)
{
	int qlen = 0;
	enum tcore_pending_priority priority;

	if (!hal || !plugin)
		return TCORE_RETURN_EINVAL;

	qlen = tcore_queue_get_length(hal->queue);
	tcore_queue_push(hal->queue, plugin);

	tcore_pending_get_priority(plugin, &priority);
	if (priority == TCORE_PENDING_PRIORITY_IMMEDIATELY) {
		dbg("IMMEDIATELY pending !!");
		_hal_idle_send(hal);
	}
	else {
		if (tcore_queue_get_length(hal->queue) == 1) {
			g_idle_add_full(IDLE_SEND_PRIORITY, _hal_idle_send, hal, NULL);
		}
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_hal_dispatch_notification_data(TcoreHal *hal, const char *event,
		const void *data)
{
	TcorePlugin *p = NULL;

	if (!hal || !event)
		return TCORE_RETURN_EINVAL;

	p = tcore_hal_ref_plugin(hal);
	if (!p)
		return TCORE_RETURN_SERVER_WRONG_PLUGIN;

	tcore_plugin_core_object_event_emit(p, event, data);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_hal_dispatch_response_data(TcoreHal *hal, int id,
		unsigned int data_len, const void *data)
{
	TcorePending *p = NULL;

	if (!hal)
		return TCORE_RETURN_EINVAL;

	if (data_len > 0 && data == NULL)
		return TCORE_RETURN_EINVAL;

	p = tcore_queue_pop_by_id(hal->queue, id);
	if (!p) {
		dbg("unknown pending (id=0x%x)", id);
		return TCORE_RETURN_PENDING_WRONG_ID;
	}

	tcore_pending_emit_response_callback(p, data_len, data);
	tcore_user_request_free(tcore_pending_ref_user_request(p));
	tcore_pending_free(p);

	/* Send next request in queue */
	g_idle_add_full(IDLE_SEND_PRIORITY, _hal_idle_send, hal, NULL);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_hal_add_recv_callback(TcoreHal *hal, TcoreHalReceiveCallback func,
		void *user_data)
{
	struct recv_callback_item_type *item;

	if (!hal)
		return TCORE_RETURN_EINVAL;

	item = calloc(sizeof(struct recv_callback_item_type), 1);
	if (!item)
		return TCORE_RETURN_ENOMEM;

	item->func = func;
	item->user_data = user_data;

	hal->callbacks = g_slist_append(hal->callbacks, item);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_hal_remove_recv_callback(TcoreHal *hal, TcoreHalReceiveCallback func)
{
	struct recv_callback_item_type *item;
	GSList *list;

	if (!hal)
		return TCORE_RETURN_EINVAL;

	for (list = hal->callbacks; list; list = list->next) {
		item = list->data;
		if (!item) {
			continue;
		}

		if (item->func == func) {
			hal->callbacks = g_slist_remove(hal->callbacks, item);
			free(item);
			list = hal->callbacks;
		}
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_hal_emit_recv_callback(TcoreHal *hal, unsigned int data_len,
		const void *data)
{
	GSList *list;
	struct recv_callback_item_type *item;

	if (!hal)
		return TCORE_RETURN_EINVAL;

	for (list = hal->callbacks; list; list = list->next) {
		item = list->data;

		if (item) {
			item->func(hal, data_len, data, item->user_data);
		}
	}

	return TCORE_RETURN_SUCCESS;
}


TReturn tcore_hal_add_send_hook(TcoreHal *hal, TcoreHalSendHook func, void *user_data)
{
	struct hook_send_type *hook;

	if (!hal || !func)
		return TCORE_RETURN_EINVAL;

	hook = calloc(sizeof(struct hook_send_type), 1);
	if (!hook)
		return TCORE_RETURN_ENOMEM;

	hook->func = func;
	hook->user_data = user_data;

	hal->hook_list_send = g_slist_append(hal->hook_list_send, hook);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_hal_remove_send_hook(TcoreHal *hal, TcoreHalSendHook func)
{
	struct hook_send_type *hook;
	GSList *list;

	if (!hal)
		return TCORE_RETURN_EINVAL;

	for (list = hal->hook_list_send; list; list = list->next) {
		hook = list->data;
		if (!hook) {
			continue;
		}

		if (hook->func == func) {
			hal->hook_list_send = g_slist_remove(hal->hook_list_send, hook);
			free(hook);
			list = hal->hook_list_send;
		}
	}

	return TCORE_RETURN_SUCCESS;
}

TcoreQueue *tcore_hal_ref_queue(TcoreHal *hal)
{
	if (!hal)
		return NULL;

	return hal->queue;
}

TcorePlugin *tcore_hal_ref_plugin(TcoreHal *hal)
{
	if (!hal)
		return NULL;

	return hal->parent_plugin;
}

TReturn tcore_hal_set_power_state(TcoreHal *hal, gboolean flag)
{
	if (!hal)
		return TCORE_RETURN_EINVAL;

	hal->power_state = flag;

	return TCORE_RETURN_SUCCESS;
}

gboolean tcore_hal_get_power_state(TcoreHal *hal)
{
	if (!hal)
		return FALSE;

	return hal->power_state;
}

TReturn tcore_hal_set_power(TcoreHal *hal, gboolean flag)
{
	if (!hal || !hal->ops || !hal->ops->power)
		return TCORE_RETURN_EINVAL;

	return hal->ops->power(hal, flag);
}
