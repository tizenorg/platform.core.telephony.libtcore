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
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "core_object.h"
#include "hal.h"
#include "at.h"

struct callback_type {
	CoreObject *co;
	char *event;
	CoreObjectCallback callback;
	void *user_data;
};

struct tcore_object_type {
	unsigned int type;
	char *name;

	TcorePlugin *parent_plugin;

	void *object;
	void *user_data;

	CoreObjectFreeHook free_hook;
	CoreObjectCloneHook clone_hook;
	CoreObjectDispatcher dispatcher;
	GSList *callbacks;

	TcoreHal *hal;
};

static CoreObject *_object_new(TcorePlugin *plugin, const char *name, unsigned int type)
{
	CoreObject *co;

	co = calloc(sizeof(struct tcore_object_type), 1);
	if (!co)
		return NULL;

	co->parent_plugin = plugin;

	if (name)
		co->name = strdup(name);

	co->type = type;

	return co;
}

static gboolean _on_at_event(TcoreAT *at, const GSList *lines, void *user_data)
{
	gboolean ret;

	struct callback_type *cb = user_data;

	ret = cb->callback(cb->co, lines, cb->user_data);
	if (ret == FALSE) {

	}

	return ret;
}

static void _remove_at_callback(TcoreAT *at, struct callback_type *cb)
{
	tcore_at_remove_notification_full(at, cb->event, _on_at_event, cb);
}

CoreObject *tcore_object_new(TcorePlugin *plugin,
		const char *name, TcoreHal *hal)
{
	CoreObject *co;

	co = _object_new(plugin, name, CORE_OBJECT_TYPE_DEFAULT);
	if (!co)
		return NULL;

	tcore_object_set_hal(co, hal);

	if (plugin)
		tcore_plugin_add_core_object(plugin, co);

	return co;
}

void tcore_object_free(CoreObject *co)
{
	GSList *l = NULL;
	struct callback_type *cb = NULL;

	if (!co)
		return;

	dbg("co_name=%s", co->name);

	if (co->free_hook)
		co->free_hook(co);

	if (co->callbacks) {
		for (l = co->callbacks; l; l = l->next) {
			cb = l->data;
			if (!cb)
				continue;

			if (cb->event)
				g_free(cb->event);

			g_free(cb);
		}

		g_slist_free(co->callbacks);
		co->callbacks = NULL;
	}

	if (co->name)
		g_free(co->name);

	g_free(co);
}

CoreObject *tcore_object_clone(CoreObject *src, TcorePlugin *new_parent, const char *new_name)
{
	CoreObject *dest;
	TcorePlugin *p;
	const char *name;
	GSList *l = NULL;
	struct callback_type *cb = NULL;

	if (!src)
		return NULL;

	if (new_parent)
		p = new_parent;
	else
		p = src->parent_plugin;

	if (new_name)
		name = new_name;
	else
		name = src->name;

	dest = _object_new(p, name, src->type);
	if (!dest)
		return NULL;

	dest->object = src->object;
	dest->user_data = src->user_data;
	dest->free_hook = src->free_hook;
	dest->clone_hook = src->clone_hook;
	dest->dispatcher = src->dispatcher;
	dest->hal = src->hal;

	for (l = src->callbacks; l; l = l->next) {
		cb = l->data;
		if (!cb)
			continue;

		tcore_object_add_callback(dest, cb->event, cb->callback, cb->user_data);
	}

	tcore_plugin_add_core_object(p, dest);

	if (src->clone_hook)
		src->clone_hook(src, dest);

	return dest;
}

const char *tcore_object_ref_name(CoreObject *co)
{
	if (!co)
		return NULL;

	return co->name;
}

TReturn tcore_object_set_free_hook(CoreObject *co,
		CoreObjectFreeHook free_hook)
{
	if (!co)
		return TCORE_RETURN_EINVAL;

	co->free_hook = free_hook;

	return TCORE_RETURN_SUCCESS;
}


TReturn tcore_object_set_clone_hook(CoreObject *co,
		CoreObjectCloneHook clone_hook)
{
	if (!co)
		return TCORE_RETURN_EINVAL;

	co->clone_hook = clone_hook;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_object_set_name(CoreObject *co, const char *name)
{
	if (!co)
		return TCORE_RETURN_EINVAL;

	if (co->name) {
		free(co->name);
		co->name = NULL;
	}

	if (name)
		co->name = strdup(name);

	return TCORE_RETURN_SUCCESS;
}

TcorePlugin *tcore_object_ref_plugin(CoreObject *co)
{
	if (!co)
		return NULL;

	return co->parent_plugin;
}

TReturn tcore_object_link_object(CoreObject *co, void *object)
{
	if (!co)
		return TCORE_RETURN_EINVAL;

	co->object = object;

	return TCORE_RETURN_SUCCESS;
}

void *tcore_object_ref_object(CoreObject *co)
{
	if (!co)
		return NULL;

	return co->object;
}

TReturn tcore_object_set_type(CoreObject *co, unsigned int type)
{
	if (!co)
		return TCORE_RETURN_EINVAL;

	co->type = type;

	return TCORE_RETURN_SUCCESS;
}

unsigned int tcore_object_get_type(CoreObject *co)
{
	if (!co)
		return 0;

	return co->type;
}

TReturn tcore_object_set_hal(CoreObject *co, TcoreHal *hal)
{
	TcoreAT *at;
	struct callback_type *cb = NULL;
	GSList *l = NULL;

	if (!co)
		return TCORE_RETURN_EINVAL;

	if (co->hal) {
		// remove unsolicited callbacks
		if (tcore_hal_get_mode(co->hal) == TCORE_HAL_MODE_AT) {
			at = tcore_hal_get_at(co->hal);
			for (l = co->callbacks; l != NULL; l = l->next) {
				cb = l->data;
				if (cb == NULL)
					continue;

				tcore_at_remove_notification_full(
					at, cb->event, _on_at_event, cb);
			}
		}
	}

	co->hal = hal;
	if (!hal)
		return TCORE_RETURN_SUCCESS;

	// register unsolicited callbacks
	if (tcore_hal_get_mode(hal) == TCORE_HAL_MODE_AT) {
		at = tcore_hal_get_at(hal);
		for (l = co->callbacks; l != NULL; l = l->next) {
			cb = l->data;
			if (cb == NULL)
				continue;

			if (cb->event[0] == 27)
				tcore_at_add_notification(at, cb->event + 1,
						TRUE, _on_at_event, cb);
			else
				tcore_at_add_notification(at, cb->event,
						FALSE, _on_at_event, cb);
		}
	}

	return TCORE_RETURN_SUCCESS;
}

TcoreHal *tcore_object_get_hal(CoreObject *co)
{
	if (!co)
		return NULL;

	return co->hal;
}

TReturn tcore_object_link_user_data(CoreObject *co,
		void *user_data)
{
	if (!co)
		return TCORE_RETURN_EINVAL;

	co->user_data = user_data;

	return TCORE_RETURN_SUCCESS;
}

void *tcore_object_ref_user_data(CoreObject *co)
{
	if (!co)
		return NULL;

	return co->user_data;
}

TReturn tcore_object_dispatch_request(CoreObject *co,
		UserRequest *ur)
{
	if (!co || !ur)
		return TCORE_RETURN_EINVAL;

	if (!co->dispatcher)
		return TCORE_RETURN_ENOSYS;

	return co->dispatcher(co, ur);
}

TReturn tcore_object_set_dispatcher(CoreObject *co,
		CoreObjectDispatcher func)
{
	if (!co || !func)
		return TCORE_RETURN_EINVAL;

	co->dispatcher = func;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_object_add_callback(CoreObject *co,
		const char *event,
		CoreObjectCallback callback, void *user_data)
{
	struct callback_type *cb = NULL;
	TcoreAT *at = NULL;

	if (!co || !event || !callback)
		return TCORE_RETURN_EINVAL;

	if (strlen(event) < 1)
		return TCORE_RETURN_EINVAL;

	cb = calloc(sizeof(struct callback_type), 1);
	if (!cb)
		return TCORE_RETURN_ENOMEM;

	cb->co = co;
	cb->event = strdup(event);
	cb->callback = callback;
	cb->user_data = user_data;

	co->callbacks = g_slist_append(co->callbacks, cb);

	if (co->hal) {
		if (tcore_hal_get_mode(co->hal) == TCORE_HAL_MODE_AT) {
			at = tcore_hal_get_at(co->hal);
			if (event[0] == 27)
				tcore_at_add_notification(at, cb->event + 1, TRUE, _on_at_event, cb);
			else
				tcore_at_add_notification(at, cb->event, FALSE, _on_at_event, cb);

		}
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_object_del_callback(CoreObject *co,
		const char *event, CoreObjectCallback callback)
{
	struct callback_type *cb = NULL;
	GSList *l = NULL;
	TcoreAT *at = NULL;

	if (!co || !event || !callback || !co->callbacks)
		return TCORE_RETURN_EINVAL;

	if (strlen(event) < 1)
		return TCORE_RETURN_EINVAL;

	if (co->hal) {
		if (tcore_hal_get_mode(co->hal) == TCORE_HAL_MODE_AT)
			at = tcore_hal_get_at(co->hal);
	}

	for (l = co->callbacks; l; l = l->next) {
		cb = l->data;
		if (!cb)
			continue;

		if (cb->callback != callback)
			continue;

		if (g_strcmp0(cb->event, event) != 0)
			continue;

		if (at)
			_remove_at_callback(at, cb);

		free(cb->event);
		co->callbacks = g_slist_remove(co->callbacks, cb);
		free(cb);
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_object_emit_callback(CoreObject *co,
		const char *event, const void *event_info)
{
	struct callback_type *cb = NULL;
	GSList *l = NULL;
	TReturn ret;

	if (!co || !event)
		return TCORE_RETURN_EINVAL;

	l = co->callbacks;
	while (l) {
		cb = l->data;
		if (!cb) {
			l = l->next;
			continue;
		}

		if (g_strcmp0(cb->event, event) != 0) {
			l = l->next;
			continue;
		}

		if (cb->callback) {
			ret = cb->callback(co, event_info, cb->user_data);
			if (ret == FALSE) {
				l = l->next;
				co->callbacks = g_slist_remove(co->callbacks, cb);
				continue;
			}
		}

		l = l->next;
	}

	return TCORE_RETURN_SUCCESS;
}
