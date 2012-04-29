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

struct callback_type {
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
	CoreObjectDispatcher dispatcher;
	GSList *callbacks;
};

CoreObject *tcore_object_new(TcorePlugin *plugin, const char *name)
{
	CoreObject *co;

	if (!plugin)
		return NULL;

	co = calloc(sizeof(struct tcore_object_type), 1);
	if (!co)
		return NULL;

	co->parent_plugin = plugin;
	co->name = strdup(name);
	co->type = CORE_OBJECT_TYPE_DEFAULT;
	co->object = NULL;
	co->user_data = NULL;
	co->free_hook = NULL;
	co->callbacks = NULL;

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
			if (!l)
				continue;

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

const char *tcore_object_ref_name(CoreObject *co)
{
	if (!co)
		return NULL;

	return co->name;
}

TReturn tcore_object_set_free_hook(CoreObject *co, CoreObjectFreeHook free_hook)
{
	if (!co)
		return TCORE_RETURN_EINVAL;

	co->free_hook = free_hook;

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

TReturn tcore_object_link_user_data(CoreObject *co, void *user_data)
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

TReturn tcore_object_dispatch_request(CoreObject *co, UserRequest *ur)
{
	if (!co || !ur)
		return TCORE_RETURN_EINVAL;

	if (!co->dispatcher)
		return TCORE_RETURN_ENOSYS;

	return co->dispatcher(co, ur);
}

TReturn tcore_object_set_dispatcher(CoreObject *co, CoreObjectDispatcher func)
{
	if (!co || !func)
		return TCORE_RETURN_EINVAL;

	co->dispatcher = func;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_object_add_callback(CoreObject *co, const char *event, CoreObjectCallback callback, void *user_data)
{
	struct callback_type *cb = NULL;

	if (!co || !event || !callback)
		return TCORE_RETURN_EINVAL;

	if (strlen(event) < 1)
		return TCORE_RETURN_EINVAL;

	cb = calloc(sizeof(struct callback_type), 1);
	if (!cb)
		return TCORE_RETURN_ENOMEM;

	cb->event = strdup(event);
	cb->callback = callback;
	cb->user_data = user_data;

	co->callbacks = g_slist_append(co->callbacks, cb);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_object_del_callback(CoreObject *co, const char *event, CoreObjectCallback callback)
{
	struct callback_type *cb = NULL;
	GSList *l = NULL;

	if (!co || !event || !callback || !co->callbacks)
		return TCORE_RETURN_EINVAL;

	if (strlen(event) < 1)
		return TCORE_RETURN_EINVAL;

	for (l = co->callbacks; l; l = l->next) {
		if (!l)
			continue;

		cb = l->data;
		if (!cb)
			continue;

		if (cb->callback != callback)
			continue;

		if (g_strcmp0(cb->event, event) != 0)
			continue;

		free(cb->event);
		co->callbacks = g_slist_remove(co->callbacks, cb);
		free(cb);
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_object_emit_callback(CoreObject *co, const char *event, const void *event_info)
{
	struct callback_type *cb = NULL;
	GSList *l = NULL;

	if (!co || !event)
		return TCORE_RETURN_EINVAL;

	for (l = co->callbacks; l; l = l->next) {
		if (!l)
			continue;

		cb = l->data;
		if (!cb)
			continue;

		if (g_strcmp0(cb->event, event) != 0)
			continue;

		if (cb->callback)
			cb->callback(co, event_info, cb->user_data);
	}

	return TCORE_RETURN_SUCCESS;
}
