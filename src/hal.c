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
//#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <glib.h>

#include "tcore.h"
#include "hal.h"
#include "at.h"
#include "queue.h"
#include "plugin.h"
#include "server.h"
#include "mux.h"

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

	gchar *name;
	TcoreHalOperations *ops;
	gboolean power_state;

	TcoreQueue *queue;
	void *user_data;

	GSList *callbacks;
	GSList *hook_list_send;

	TcoreHalMode mode;
	TcoreAT *at;
};

typedef enum {
	TCORE_HAL_SEND_DATA_SUCCESS,
	TCORE_HAL_SEND_DATA_FAILURE,
	TCORE_HAL_SEND_DATA_HOOK_STOP
} TcoreHalSendData;

static gboolean __hal_idle_send(void *user_data)
{
	TcoreHal *hal = user_data;
	TcorePending *pending;
	void *data;
	guint data_len = 0;
	TelReturn ret;
	gboolean renew = FALSE;

	if (hal == NULL) {
		err("hal is NULL");
		return FALSE;
	}

	msg("--[Queue SEND]-------------------");

	pending = tcore_queue_ref_next_pending(hal->queue);
	if (pending == NULL) {
		dbg("Queue is empty!!! (queue length: %d)",
			tcore_queue_get_length(hal->queue));
		goto out;
	}

	data = tcore_pending_ref_request_data(pending, &data_len);
	dbg("Queue length: [%d] pending: [%p] id: [0x%x] data_len: [%d]",
	tcore_queue_get_length(hal->queue),
		(guint)pending, tcore_pending_get_id(pending), data_len);

	if (hal->mode == TCORE_HAL_MODE_AT)
		ret = tcore_at_send_data(hal->at, data, TRUE);
	else
		ret = tcore_hal_send_data(hal, data_len, data);

	tcore_pending_emit_send_callback(pending, ret);

	if (ret != (TelReturn)TCORE_HAL_SEND_DATA_HOOK_STOP) {
		TcoreQueue *queue;

		if (tcore_pending_get_auto_free_status_after_sent(pending)) {
			queue = tcore_hal_ref_queue(hal);
			tcore_queue_pop_by_pending(queue, pending);
			tcore_pending_free(pending);

			/* renew idler */
			renew = TRUE;
		}
		else {
			/* Send fail */
			if (ret != TEL_RETURN_SUCCESS) {
				err("HAL Send failed");

				queue = tcore_hal_ref_queue(hal);
				pending = tcore_queue_pop(queue);
				tcore_pending_free(pending);
			}
		}
	}

out:
	msg("--[Queue SEND FINISH]------------\n");
	return renew;
}

TcoreHal *tcore_hal_new(TcorePlugin *plugin, const gchar *name,
	TcoreHalOperations *hops, TcoreHalMode mode)
{
	TcoreHal *hal;

	if (name == NULL) {
		err("HAL name is NULL");
		return NULL;
	}

	hal = g_try_new0(struct tcore_hal_type, 1);
	if (hal == NULL) {
		err("Failed to allocate memory");
		return NULL;
	}

	hal->parent_plugin = plugin;
	hal->name = tcore_strdup(name);
	hal->ops = hops;

	hal->queue = tcore_queue_new(hal);
	hal->mode = mode;

	/* Create and link AT parser if HAL mode is AT */
	if (mode == TCORE_HAL_MODE_AT)
		hal->at = tcore_at_new(hal);

	return hal;
}

void tcore_hal_free(TcoreHal *hal)
{
	if (hal == NULL) {
		err("HAL is NULL");
		return;
	}

	dbg("hal: [%s]", hal->name);

	/* Freeing HAL name */
	tcore_free(hal->name);

	/* Freeing HAL callbacks list */
	g_slist_free_full(hal->callbacks, g_free);

	/* Freeing HAL Queue */
	tcore_queue_free(hal->queue);

	/* Freeing AT parser linked to HAL */
	if (hal->mode == TCORE_HAL_MODE_AT)
		tcore_at_free(hal->at);

	/* Freeing HAL */
	tcore_free(hal);
}

TelReturn tcore_hal_set_name(TcoreHal *hal, const gchar *name)
{
	if (hal == NULL) {
		err("HAL is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	/* Freeing the previously assigned HAL name */
	tcore_free(hal->name);
	hal->name = NULL;

	/*
	 * Assign the new HAL name irrespective of if 'name' is NULL,
	 * g_strdup would take care of this scenario.
	 */
	hal->name = tcore_strdup(name);

	return TEL_RETURN_SUCCESS;
}

gchar *tcore_hal_get_name(TcoreHal *hal)
{
	if (hal == NULL) {
		err("HAL is NULL");
		return NULL;
	}

	/*
	 * Return copy of HAL name,
	 *
	 * it CAN even be NULL if hal->name is NULL,
	 * g_strdup will take care of this scenario.
	 */
	return tcore_strdup(hal->name);
}

TcoreAT *tcore_hal_get_at(TcoreHal *hal)
{
	if (hal == NULL) {
		err("HAL is NULL");
		return NULL;
	}

	return hal->at;
}

TcoreHalMode tcore_hal_get_mode(TcoreHal *hal)
{
	if (hal == NULL) {
		err("HAL is NULL");
		return TCORE_HAL_MODE_UNKNOWN;
	}

	return hal->mode;
}

TelReturn tcore_hal_set_mode(TcoreHal *hal, TcoreHalMode mode)
{
	if (hal == NULL) {
		err("HAL is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	hal->mode = mode;

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_hal_link_user_data(TcoreHal *hal, void *user_data)
{
	if (hal == NULL) {
		err("HAL is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	hal->user_data = user_data;

	return TEL_RETURN_SUCCESS;
}

void *tcore_hal_ref_user_data(TcoreHal *hal)
{
	if (hal == NULL) {
		err("HAL is NULL");
		return NULL;
	}

	return hal->user_data;
}

/* Send data without Queue */
TelReturn tcore_hal_send_data(TcoreHal *hal, guint data_len, void *data)
{
	struct hook_send_type *hook;
	GSList *list;

	if ((hal == NULL) || (hal->ops == NULL) || (hal->ops->send == NULL)) {
		err("hal: [%p] hal->ops: [%p] hal->ops->send: [%p]",
			hal, (hal ? hal->ops : NULL),
			(hal ? (hal->ops ? hal->ops->send : NULL) : NULL));

		return TEL_RETURN_INVALID_PARAMETER;
	}

	for (list = hal->hook_list_send; list; list = list->next) {
		hook = list->data;
		if (hook != NULL)
			if (hook->func != NULL)
				if (hook->func(hal, data_len, data, hook->user_data)
					== TCORE_HOOK_RETURN_STOP_PROPAGATION)
					return TCORE_HAL_SEND_DATA_HOOK_STOP;
	}

	return hal->ops->send(hal, data_len, data);
}

/* Send data by Queue */
TelReturn tcore_hal_send_request(TcoreHal *hal, TcorePending *pending)
{
	TcorePendingPriority priority;
	TelReturn ret;
	dbg("HAL: [%p]", hal);

	if ((hal == NULL) || (pending == NULL)){
		err("hal: [%p] pending: [%p]", hal, pending);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (hal->power_state == FALSE) {
		err("HAL Power: [OFF]");
		return TEL_RETURN_FAILURE;
	}

	ret = tcore_queue_push(hal->queue, pending);
	if (ret != TEL_RETURN_SUCCESS) {
		err("Failed to PUSH request to Queue");
		return ret;
	}

	tcore_pending_get_priority(pending, &priority);
	if (priority == TCORE_PENDING_PRIORITY_IMMEDIATELY) {
		dbg("IMMEDIATELY pending !!!");
		__hal_idle_send(hal);
	}
	else {
		/* If it is the ONLY entry then add to g_idle_add_full */
		if (tcore_queue_get_length(hal->queue) == 1) {
			dbg("Single entry in pending  Queue!!!");
			g_idle_add_full(IDLE_SEND_PRIORITY,
				__hal_idle_send, hal, NULL);
		}
	}

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_hal_send_force(TcoreHal *hal)
{
	if (hal == NULL) {
		err("HAL is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	__hal_idle_send(hal);

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_hal_dispatch_response_data(TcoreHal *hal, guint id,
		guint data_len, const void *data)
{
	TcorePending *pending = NULL;

	if (hal == NULL) {
		err("HAL is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if ((data_len == 0) || (data == NULL)) {
		err("data_len: [%d] data: [%p]", data_len, data);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (hal->mode == TCORE_HAL_MODE_AT) {
		gboolean ret;
		ret = tcore_at_process(hal->at, data_len, data);
		if (ret) {
			/* Send next request in queue */
			g_idle_add_full(IDLE_SEND_PRIORITY,
				__hal_idle_send, hal, NULL);
		}
	}
	else {
		if(hal->mode == TCORE_HAL_MODE_CUSTOM) {
			dbg("TCORE_HAL_MODE_CUSTOM");
			pending = tcore_queue_pop_by_id(hal->queue, id);
			if (pending == NULL) {
				err("unknown pending (id=0x%x)", id);
				return TEL_RETURN_INVALID_PARAMETER;
			}

			/* Emit response callback */
			tcore_pending_emit_response_callback(pending,
				data_len, data);

			/* Free pending request */
			tcore_pending_free(pending);
		}
		else if(hal->mode == TCORE_HAL_MODE_TRANSPARENT) {
			dbg("TCORE_HAL_MODE_TRANSPARENT");

			/* Invoke CMUX receive API for decoding */
			tcore_cmux_rcv_from_hal(hal, (guchar *)data, data_len);
		}

		/* Send next request in queue */
		g_idle_add_full(IDLE_SEND_PRIORITY, __hal_idle_send, hal, NULL);
	}

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_hal_add_recv_callback(TcoreHal *hal, TcoreHalReceiveCallback func,
		void *user_data)
{
	struct recv_callback_item_type *item;

	if (hal == NULL) {
		err("HAL is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	item = g_try_new0(struct recv_callback_item_type, 1);
	if (item == NULL) {
		err("Failed to allocate memory");
		return TEL_RETURN_MEMORY_FAILURE;
	}

	item->func = func;
	item->user_data = user_data;

	hal->callbacks = g_slist_append(hal->callbacks, item);

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_hal_remove_recv_callback(TcoreHal *hal, TcoreHalReceiveCallback func)
{
	struct recv_callback_item_type *item;
	GSList *list;

	if (hal == NULL) {
		err("HAL is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	for (list = hal->callbacks; list; list = list->next) {
		item = list->data;
		if (item != NULL) {
			if (item->func == func) {
				hal->callbacks =
					g_slist_remove(hal->callbacks, item);
				tcore_free(item);
				if (hal->callbacks == NULL)
					break;

				list = hal->callbacks;
			}
		}
	}

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_hal_emit_recv_callback(TcoreHal *hal, guint data_len,
		const void *data)
{
	GSList *list;
	struct recv_callback_item_type *item;

	if (hal == NULL) {
		err("HAL is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	for (list = hal->callbacks; list; list = list->next) {
		item = list->data;
		if (item != NULL)
			if (item->func)
				item->func(hal, data_len, data, item->user_data);
	}

	return TEL_RETURN_SUCCESS;
}


TelReturn tcore_hal_add_send_hook(TcoreHal *hal,
	TcoreHalSendHook func, void *user_data)
{
	struct hook_send_type *hook;

	if ((hal == NULL) || (func == NULL)) {
		err("hal: [%p] func: [%p]", hal, func);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	hook = g_try_new0(struct hook_send_type, 1);
	if (hook == NULL) {
		err("Failed to allocate memory");
		return TEL_RETURN_MEMORY_FAILURE;
	}

	hook->func = func;
	hook->user_data = user_data;

	hal->hook_list_send = g_slist_append(hal->hook_list_send, hook);

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_hal_remove_send_hook(TcoreHal *hal, TcoreHalSendHook func)
{
	struct hook_send_type *hook;
	GSList *list;

	if (hal == NULL) {
		err("HAL is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	for (list = hal->hook_list_send; list; list = list->next) {
		hook = list->data;
		if (hook != NULL) {
			if (hook->func == func) {
				hal->hook_list_send =
					g_slist_remove(hal->hook_list_send, hook);

				tcore_free(hook);
				list = hal->hook_list_send;
			}
		}
	}

	return TEL_RETURN_SUCCESS;
}

TcoreQueue *tcore_hal_ref_queue(TcoreHal *hal)
{
	if (hal == NULL) {
		err("HAL is NULL");
		return NULL;
	}

	return hal->queue;
}

TcorePlugin *tcore_hal_ref_plugin(TcoreHal *hal)
{
	if (hal == NULL) {
		err("HAL is NULL");
		return NULL;
	}

	return hal->parent_plugin;
}

TelReturn tcore_hal_set_power_state(TcoreHal *hal, gboolean flag)
{
	if (hal == NULL) {
		err("HAL is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	hal->power_state = flag;

	return TEL_RETURN_SUCCESS;
}

gboolean tcore_hal_get_power_state(TcoreHal *hal)
{
	if (hal == NULL) {
		err("HAL is NULL");
		return FALSE;
	}

	return hal->power_state;
}

TelReturn tcore_hal_set_power(TcoreHal *hal, gboolean flag)
{
	if ((hal == NULL) || (hal->ops == NULL) || (hal->ops->power == NULL)) {
		err("hal: [%p] hal->ops: [%p] hal->ops->power: [%p]",
			hal, (hal ? hal->ops : NULL),
			(hal ? (hal->ops ? hal->ops->power : NULL) : NULL));
		return TEL_RETURN_INVALID_PARAMETER;
	}

	return hal->ops->power(hal, flag);
}

TelReturn tcore_hal_setup_netif(TcoreHal *hal, CoreObject *co,
	TcoreHalSetupNetifCallback func, void *user_data,
	guint cid, gboolean enable)
{
	if ((hal == NULL) || (hal->ops == NULL) || (hal->ops->setup_netif == NULL)) {
		err("hal: [%p] hal->ops: [%p] hal->ops->setup_netif: [%p]",
			hal, (hal ? hal->ops : NULL),
			(hal ? (hal->ops ? hal->ops->setup_netif : NULL) : NULL));
		return TEL_RETURN_INVALID_PARAMETER;
	}

	return hal->ops->setup_netif(co, func, user_data, cid, enable);
}
