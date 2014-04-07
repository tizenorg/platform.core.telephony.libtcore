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
#include <dlfcn.h>

#include <glib.h>

#include "tcore.h"
#include "core_object.h"
#include "plugin.h"
#include "server.h"

struct tcore_plugin_type {
	gchar *filename;
	const struct tcore_plugin_define_desc *desc;
	void *handle;

	void *user_data;
	Communicator *comm;
	GSList *list_co;
	GHashTable *property;

	Server *parent_server;
};

typedef struct {
	TcoreResponseCallback resp_cb;
	void *resp_cbdata;
	void *user_data;
} TcorePluginRespData;

TcorePlugin *tcore_plugin_new(Server *server,
	const struct tcore_plugin_define_desc *desc,
	const gchar *filename, void *handle)
{
	TcorePlugin *p;

	p = g_try_new0(struct tcore_plugin_type, 1);
	if (p == NULL) {
		err("plugin is NULL");
		return NULL;
	}

	/*
	 * Assign 'filename' name irrespective of if it is NULL,
	 * g_strdup would take care of this scenario.
	 */
	p->filename = tcore_strdup(filename);

	p->desc = desc;
	p->property = g_hash_table_new(g_str_hash, g_str_equal);
	p->handle = handle;
	p->parent_server = server;

	dbg("Plug-in '%s' created", p->filename);

	return p;
}

void tcore_plugin_free(TcorePlugin *plugin)
{
	if (plugin == NULL) {
		err("plugin is NULL");
		return;
	}

	dbg("Plug-in '%s' freeing", plugin->filename);

	/* Free resources */
	g_slist_free_full(plugin->list_co, (GDestroyNotify)tcore_object_free);
	tcore_free(plugin->filename);
	g_hash_table_destroy(plugin->property);

	if (plugin->handle)
		dlclose(plugin->handle);

	plugin->desc = NULL;

	tcore_free(plugin);
}

const struct tcore_plugin_define_desc *tcore_plugin_get_description(TcorePlugin *plugin)
{
	if (plugin == NULL) {
		err("plugin is NULL");
		return NULL;
	}

	return plugin->desc;
}

gchar *tcore_plugin_get_filename(TcorePlugin *plugin)
{
	if (plugin == NULL) {
		err("plugin is NULL");
		return NULL;
	}

	/*
	 * Return copy of 'filename',
	 *
	 * it CAN even be NULL if plugin->filename is NULL,
	 * g_strdup will take care of this scenario.
	 */
	return tcore_strdup(plugin->filename);
}

const gchar *tcore_plugin_ref_plugin_name(TcorePlugin *plugin)
{
	if (plugin == NULL) {
		err("plugin is NULL");
		return NULL;
	}

	if (plugin->desc == NULL) {
		err("desc is NULL");
		return NULL;
	}

	return plugin->desc->name;
}

Server *tcore_plugin_ref_server(TcorePlugin *plugin)
{
	if (plugin == NULL) {
		err("plugin is NULL");
		return NULL;
	}

	return plugin->parent_server;
}

TelReturn tcore_plugin_link_user_data(TcorePlugin *plugin, void *user_data)
{
	if (plugin == NULL) {
		err("plugin is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	plugin->user_data = user_data;

	return TEL_RETURN_SUCCESS;
}

void *tcore_plugin_ref_user_data(TcorePlugin *plugin)
{
	if (plugin == NULL) {
		err("plugin is NULL");
		return FALSE;
	};

	return plugin->user_data;
}

TelReturn tcore_plugin_add_core_object(TcorePlugin *plugin, CoreObject *co)
{
	if ((plugin == NULL) || (co == NULL)) {
		err("plugin: [%p] co: [%p]", plugin, co);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	/* Core Object is prepended to the Core Object list */
	plugin->list_co = g_slist_insert(plugin->list_co, co, 0);
	dbg("Core Object (Type: [0x%x]) added!", tcore_object_get_type(co));

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_plugin_remove_core_object(TcorePlugin *plugin, CoreObject *co)
{
	if ((plugin == NULL) || (co == NULL)) {
		err("plugin: [%p] co: [%p]", plugin, co);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	plugin->list_co = g_slist_remove(plugin->list_co, co);
	dbg("Core Object (Type: [0x%x]) added!", tcore_object_get_type(co));

	return TEL_RETURN_SUCCESS;
}

GSList *tcore_plugin_ref_core_objects(TcorePlugin *plugin)
{
	if (plugin == NULL) {
		err("plugin is NULL");
		return NULL;
	}

	return plugin->list_co;
}

CoreObject *tcore_plugin_ref_core_object(TcorePlugin *plugin, unsigned int type)
{
	GSList *list;
	CoreObject *co;

	if (plugin == NULL) {
		err("plugin is NULL");
		return NULL;
	}

	for (list = plugin->list_co; list; list = list->next) {
		co = list->data;
		if (co != NULL)
			if (tcore_object_get_type(co) == type)
				return co;
	}

	return NULL;
}

GSList *tcore_plugin_get_core_objects_bytype(TcorePlugin *plugin, unsigned int type)
{
	GSList *list, *rlist = NULL;
	CoreObject *co;

	if (plugin == NULL) {
		err("plugin is NULL");
		return NULL;
	}

	for (list = plugin->list_co; list; list = list->next) {
		co = list->data;
		if (co != NULL) {
			/* Generate list of Core Object of similar type */
			if (tcore_object_get_type(co) == type)
				rlist = g_slist_append(rlist, co);
		}
	}

	return rlist;
}

TelReturn tcore_plugin_core_object_event_emit(TcorePlugin *plugin,
		const gchar *event, const void *event_info)
{
	GSList *list;
	CoreObject *co;

	if (plugin == NULL) {
		err("plugin is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	dbg("Emitting '%s' event!!!", event);
	for (list = plugin->list_co; list; list = list->next) {
		co = list->data;
		if (co != NULL)
			tcore_object_emit_callback(co, event, event_info);
	}

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_plugin_link_property(TcorePlugin *plugin, const gchar *key, void *data)
{
	void *prev;

	if (plugin == NULL) {
		err("plugin is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (plugin->property == NULL) {
		err("property is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	prev = g_hash_table_lookup(plugin->property, key);
	if (prev != NULL) {
		tcore_free(prev);
		g_hash_table_replace(plugin->property, (gpointer)key, data);
	}
	else {
		g_hash_table_insert(plugin->property, tcore_strdup(key), data);
	}

	return TEL_RETURN_SUCCESS;
}

void *tcore_plugin_ref_property(TcorePlugin *plugin, const gchar *key)
{
	if (plugin == NULL) {
		err("plugin is NULL");
		return NULL;
	};

	if (plugin->property == NULL) {
		err("property is NULL");
		return NULL;
	}

	return g_hash_table_lookup(plugin->property, key);
}

TelReturn tcore_plugin_add_request_hook(TcorePlugin *plugin,
	TcoreCommand command, TcoreRequestHook func, void *user_data)
{
	CoreObject *co;

	if ((plugin == NULL) || (func == NULL)) {
		err("plugin: [%p] func: [%p]", plugin, func);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	co = tcore_plugin_ref_core_object(plugin, GET_OBJECT_TYPE(command));
	if (co == NULL) {
		err("co is NULL");
		return TEL_RETURN_FAILURE;
	}

	return tcore_object_add_request_hook(co, command, func, user_data);
}

void tcore_plugin_remove_request_hook(TcorePlugin *plugin,
	TcoreCommand command, TcoreRequestHook func)
{
	CoreObject *co;

	if ((plugin == NULL) || (func == NULL)) {
		err("plugin: [%p] func: [%p]", plugin, func);
		return;
	}

	co = tcore_plugin_ref_core_object(plugin, GET_OBJECT_TYPE(command));
	if (co == NULL) {
		err("co is NULL");
		return;
	}

	return tcore_object_remove_request_hook(co, command, func);
}

TelReturn tcore_plugin_add_notification_hook(TcorePlugin *plugin,
	TcoreNotification command, TcoreNotificationHook func, void *user_data)
{
	CoreObject *co;

	if ((plugin == NULL) || (func == NULL)) {
		err("plugin: [%p] func: [%p]", plugin, func);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	co = tcore_plugin_ref_core_object(plugin, GET_OBJECT_TYPE(command));
	if (co == NULL) {
		err("co [0x%x] is NULL", GET_OBJECT_TYPE(command));
		return TEL_RETURN_FAILURE;
	}

	return tcore_object_add_notification_hook(co, command, func, user_data);
}

void tcore_plugin_remove_notification_hook(TcorePlugin *plugin,
	TcoreNotification command, TcoreNotificationHook func)
{
	CoreObject *co;

	if ((plugin == NULL) || (func == NULL)) {
		err("plugin: [%p] func: [%p]", plugin, func);
		return;
	}

	co = tcore_plugin_ref_core_object(plugin, GET_OBJECT_TYPE(command));
	if (co == NULL) {
		err("co [0x%x] is NULL", GET_OBJECT_TYPE(command));
		return;
	}

	return tcore_object_remove_notification_hook(co, command, func);
}


/* TODO - Need to move the static function and rename accordingly */
static void tcore_plugin_response_callback(TcorePlugin *plugin,
	gint result, const void *response, void *user_data)
{
	TcorePluginRespData *resp_cb_data = (TcorePluginRespData *)user_data;

	if (resp_cb_data == NULL) {
		err("Response Callback data is NULL");
		return;
	}

	/* Invoke response callback */
	if (resp_cb_data->resp_cb)
		resp_cb_data->resp_cb(result, response, resp_cb_data->resp_cbdata);

	/* Free resource */
	tcore_free(resp_cb_data);
}

TelReturn tcore_plugin_dispatch_request(TcorePlugin *plugin,
		gboolean exec_hooks, TcoreCommand command,
		const void *request, guint request_len,
		TcoreResponseCallback cb, const void *user_data)
{
	CoreObject *co;
	TcorePluginRespData *resp_cb_data;
	TelReturn ret;

	if (plugin == NULL) {
		err("plugin is NULL");
		return TEL_RETURN_FAILURE;
	}

	co = tcore_plugin_ref_core_object(plugin, GET_OBJECT_TYPE(command));
	if (co == NULL) {
		err("co is NULL");
		return TEL_RETURN_FAILURE;
	}

	resp_cb_data = tcore_try_malloc0(sizeof(TcorePluginRespData));
	if (resp_cb_data == NULL) {
		err("Failed to allocate memory");
		return TEL_RETURN_FAILURE;
	}

	resp_cb_data->resp_cb = cb;
	resp_cb_data->resp_cbdata = (void *)user_data;

	/* Dispatch Request to Core Object */
	ret = tcore_object_dispatch_request(co, exec_hooks, command,
		request, request_len, tcore_plugin_response_callback, (void *)resp_cb_data);
	if (ret != TEL_RETURN_SUCCESS) {
		err("Failed to dispatch request!!!");

		/* Free resource */
		tcore_free(resp_cb_data);
	}

	return ret;
}

TelReturn tcore_plugin_send_notification(TcorePlugin *plugin,
	TcoreNotification command, guint data_len, void *data)
{
	Server *s;

	if (plugin == NULL) {
		err("plugin is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	s = tcore_plugin_ref_server(plugin);

	/* Dispatch the Notification to Server */
	return tcore_server_send_notification(s, plugin, command, data_len, data);
}
