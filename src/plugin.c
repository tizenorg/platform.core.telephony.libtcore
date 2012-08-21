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
#include <dlfcn.h>

#include <glib.h>

#include "tcore.h"
#include "core_object.h"
#include "plugin.h"
#include "server.h"

struct tcore_plugin_type {
	char *filename;
	const struct tcore_plugin_define_desc *desc;
	void *handle;

	void *user_data;
	Communicator *comm;
	GSList *list_co;
	GHashTable *property;

	Server *parent_server;
};

TcorePlugin *tcore_plugin_new(Server *server,
		const struct tcore_plugin_define_desc *desc,
		const char *filename, void *handle)
{
	TcorePlugin *p;

	p = calloc(sizeof(struct tcore_plugin_type), 1);
	if (!p)
		return NULL;

	if (filename)
		p->filename = strdup(filename);

	p->desc = desc;
	p->property = g_hash_table_new(g_str_hash, g_str_equal);
	p->handle = handle;
	p->parent_server = server;

	return p;
}

void tcore_plugin_free(TcorePlugin *plugin)
{
	GSList *list;
	CoreObject *o;

	if (!plugin)
		return;

	dbg("");

	if (plugin->list_co) {
		for (list = plugin->list_co; list; list = list->next) {
			if (!list)
				continue;

			o = list->data;
			if (!o)
				continue;

			tcore_object_free(o);
			list->data = NULL;
		}

		g_slist_free(plugin->list_co);
		plugin->list_co = NULL;
	}

	if (plugin->filename) {
		free(plugin->filename);
		plugin->filename = NULL;
	}

	if (plugin->property) {
		g_hash_table_destroy(plugin->property);
		plugin->property = NULL;
	}

	plugin->desc = NULL;

	if (plugin->handle) {
		dlclose(plugin->handle);
		plugin->handle = NULL;
	}

	free(plugin);
}

const struct tcore_plugin_define_desc *tcore_plugin_get_description(TcorePlugin *plugin)
{
	if (!plugin)
		return NULL;

	return plugin->desc;
}

char *tcore_plugin_get_filename(TcorePlugin *plugin)
{
	if (!plugin)
		return NULL;

	if (!plugin->filename)
		return NULL;

	return strdup(plugin->filename);
}

char* tcore_plugin_ref_plugin_name(TcorePlugin *plugin)
{
	if (!plugin)
		return NULL;

	if (!plugin->desc->name)
		return NULL;

	return plugin->desc->name;
}

Server *tcore_plugin_ref_server(TcorePlugin *plugin)
{
	if (!plugin)
		return NULL;

	return plugin->parent_server;
}

TReturn tcore_plugin_link_user_data(TcorePlugin *plugin, void *user_data)
{
	if (!plugin)
		return TCORE_RETURN_EINVAL;

	plugin->user_data = user_data;

	return TCORE_RETURN_SUCCESS;
}

void *tcore_plugin_ref_user_data(TcorePlugin *plugin)
{
	if (!plugin)
		return FALSE;

	return plugin->user_data;
}

TReturn tcore_plugin_add_core_object(TcorePlugin *plugin, CoreObject *co)
{
	if (!plugin || !co)
		return TCORE_RETURN_EINVAL;

	dbg("add core_object! (name=%s)", tcore_object_ref_name(co));

	plugin->list_co = g_slist_insert(plugin->list_co, co, 0);

	return TCORE_RETURN_SUCCESS;
}

CoreObject *tcore_plugin_ref_core_object(TcorePlugin *plugin, const char *name)
{
	GSList *list;
	CoreObject *co;

	if (!plugin)
		return NULL;

	for (list = plugin->list_co; list; list = list->next) {
		if (!list)
			continue;

		co = list->data;
		if (!co)
			continue;

		if (strcmp(tcore_object_ref_name(co), name) == 0) {
			return co;
		}
	}

	return NULL;
}


GSList *tcore_plugin_get_core_objects_bytype(TcorePlugin *plugin, unsigned int type)
{
	GSList *list, *rlist = NULL;
	CoreObject *co;

	if (!plugin)
		return NULL;

	for (list = plugin->list_co; list; list = list->next) {
		if (!list)
			continue;

		co = list->data;
		if (!co)
			continue;

		if (tcore_object_get_type(co) == type) {
			rlist = g_slist_append(rlist, co);
		}
	}

	return rlist;
}

TReturn tcore_plugin_core_object_event_emit(TcorePlugin *plugin, const char *event, const void *event_info)
{
	GSList *list;
	CoreObject *co;

	if (!plugin)
		return TCORE_RETURN_EINVAL;

	dbg("event(%s) emit", event);

	for (list = plugin->list_co; list; list = list->next) {
		if (!list)
			continue;

		co = list->data;
		if (!co)
			continue;

		tcore_object_emit_callback(co, event, event_info);
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_plugin_link_property(TcorePlugin *plugin, const char *key, void *data)
{
	void *prev;

	if (!plugin)
		return TCORE_RETURN_EINVAL;

	if (!plugin->property)
		return TCORE_RETURN_EINVAL;

	prev = g_hash_table_lookup(plugin->property, key);
	if (prev != NULL) {
		free(prev);
		g_hash_table_replace(plugin->property, (gpointer)key, data);
	}
	else {
		g_hash_table_insert(plugin->property, strdup(key), data);
	}

	return TCORE_RETURN_SUCCESS;
}

void *tcore_plugin_ref_property(TcorePlugin *plugin, const char *key)
{
	if (!plugin)
		return NULL;

	if (!plugin->property)
		return NULL;

	return g_hash_table_lookup(plugin->property, key);
}
