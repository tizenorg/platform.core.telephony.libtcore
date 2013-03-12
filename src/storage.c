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
#include "server.h"
#include "storage.h"

struct tcore_storage_type {
	const char *name;
	struct storage_operations *ops;
	GHashTable *callback;

	TcorePlugin *parent_plugin;
};

struct storage_callback_type{
	TcoreStorageKeyCallback cb_fn;
	void *user_data;
};

Storage *tcore_storage_new(TcorePlugin *plugin, const char *name,
		struct storage_operations *ops)
{
	Storage *strg;

	strg = calloc(sizeof(struct tcore_storage_type), 1);
	if (!strg)
		return NULL;

	if (name)
		strg->name = strdup(name);

	strg->parent_plugin = plugin;
	strg->ops = ops;
	strg->callback = g_hash_table_new_full(g_str_hash,g_str_equal, g_free, NULL);

	tcore_server_add_storage(tcore_plugin_ref_server(plugin), strg);

	return strg;
}

void tcore_storage_free(Storage *strg)
{
	if (!strg)
		return;

	if (strg->name)
		free((void *)strg->name);

	free(strg);
}

const char *tcore_storage_ref_name(Storage *strg)
{
	if (!strg)
		return NULL;

	return strg->name;
}

void *tcore_storage_create_handle(Storage *strg, const char *path)
{
	if (!path)
		return NULL;

	if (!strg || !strg->ops || !strg->ops->create_handle) {
		return NULL;
	}

	return strg->ops->create_handle(strg, path);
}

gboolean tcore_storage_remove_handle(Storage *strg, void *handle)
{
	if (!handle)
		return FALSE;

	if (!strg || !strg->ops || !strg->ops->remove_handle) {
		return FALSE;
	}

	return strg->ops->remove_handle(strg, handle);
}

gboolean tcore_storage_set_int(Storage *strg, enum tcore_storage_key key,
		int value)
{
	if (!strg || !strg->ops || !strg->ops->set_int) {
		return FALSE;
	}

	return strg->ops->set_int(strg, key, value);
}

gboolean tcore_storage_set_string(Storage *strg, enum tcore_storage_key key,
		const char *value)
{
	if (!strg || !strg->ops || !strg->ops->set_string) {
		return FALSE;
	}

	return strg->ops->set_string(strg, key, value);
}

gboolean tcore_storage_set_bool(Storage *strg, enum tcore_storage_key key,
		gboolean value)
{
	if (!strg || !strg->ops || !strg->ops->set_bool) {
		return FALSE;
	}

	return strg->ops->set_bool(strg, key, value);
}

int tcore_storage_get_int(Storage *strg, enum tcore_storage_key key)
{
	if (!strg || !strg->ops || !strg->ops->get_int) {
		return -1;
	}

	return strg->ops->get_int(strg, key);
}

char *tcore_storage_get_string(Storage *strg, enum tcore_storage_key key)
{
	if (!strg || !strg->ops || !strg->ops->get_string) {
		return NULL;
	}

	return strg->ops->get_string(strg, key);
}

gboolean tcore_storage_get_bool(Storage *strg, enum tcore_storage_key key)
{
	if (!strg || !strg->ops || !strg->ops->get_bool) {
		return FALSE;
	}

	return strg->ops->get_bool(strg, key);
}

static void tcore_storage_vkey_callback_dispatcher(Storage *strg,
		enum tcore_storage_key key, void *value)
{
	gpointer tmp = NULL;
	gchar *key_gen = NULL;
	struct storage_callback_type *tmp_cb = NULL;

	key_gen = g_strdup_printf("%d", key);
	tmp = g_hash_table_lookup(strg->callback, key_gen);

	if (tmp != NULL) {
		GSList *cb_data = (GSList *) tmp;

		for (; cb_data != NULL; cb_data = g_slist_next(cb_data)) {
			tmp_cb = cb_data->data;
			tmp_cb->cb_fn(key, value, tmp_cb->user_data);
			cb_data = g_slist_next(cb_data);
		}
	}

	g_free(key_gen);
	return;
}

gboolean tcore_storage_set_key_callback(Storage *strg,
		enum tcore_storage_key key, TcoreStorageKeyCallback cb, void *user_data)
{
	gpointer tmp = NULL;
	gchar *key_gen = NULL;
	struct storage_callback_type *strg_cb_data = NULL;
	struct storage_callback_type *tmp_cb = NULL;

	if (!strg || !strg->ops || !strg->ops->set_key_callback)
	{
		return FALSE;
	}

	strg_cb_data = g_new0(struct storage_callback_type, 1);
	strg_cb_data->cb_fn = cb;
	strg_cb_data->user_data = user_data;

	key_gen = g_strdup_printf("%d", key);
	tmp = g_hash_table_lookup(strg->callback, key_gen);
	if (tmp != NULL) {
		GSList *cb_data = (GSList *) tmp;

		do {
			tmp_cb = cb_data->data;
			if (tmp_cb->cb_fn == cb) {
				g_free(key_gen);
				g_free(strg_cb_data);
				return FALSE;
			}

		} while ((cb_data = g_slist_next(cb_data)));

		tmp = g_slist_append( (GSList *)tmp, strg_cb_data);
	}
	else {
		GSList *data = NULL;
		data = g_slist_append(data, strg_cb_data);
		g_hash_table_insert(strg->callback, g_strdup(key_gen), data);
		strg->ops->set_key_callback(strg, key, tcore_storage_vkey_callback_dispatcher);
	}

	g_free(key_gen);
	return TRUE;
}

gboolean tcore_storage_remove_key_callback(Storage *strg,
		enum tcore_storage_key key, TcoreStorageKeyCallback cb)
{
	gpointer tmp = NULL;
	gchar *key_gen = NULL;
	GSList *cb_data = NULL;
	int cb_cnt = 0;
	struct storage_callback_type *tmp_cb = NULL;

	if (!strg || !strg->ops || !strg->ops->remove_key_callback) {
		return FALSE;
	}

	key_gen = g_strdup_printf("%d", key);
	tmp = g_hash_table_lookup(strg->callback, key_gen);

	if (tmp == NULL){
		g_free(key_gen);
		return FALSE;
	}

	cb_data = (GSList *) tmp;

	do {
		tmp_cb = cb_data->data;
		if (tmp_cb->cb_fn == cb) {
			tmp = g_slist_remove((GSList *) tmp, cb_data->data);
			g_free(cb_data->data);
			break;
		}

	} while ((cb_data = g_slist_next(cb_data)));

	cb_cnt = g_slist_length( (GSList *) tmp );
	dbg("glist cnt (%d)", cb_cnt);

	if(cb_cnt == 0){
		g_hash_table_remove(strg->callback, key_gen);
		strg->ops->remove_key_callback(strg, key);
	}

	g_free(key_gen);
	return TRUE;
}

gboolean tcore_storage_update_query_database(Storage *strg, void *handle,
		const char *query, GHashTable *in_param)
{
	if (!strg || !handle || !query)
		return FALSE;

	if (!strg->ops || !strg->ops->update_query_database) {
		return FALSE;
	}

	return strg->ops->update_query_database(strg, handle, query, in_param);
}

gboolean tcore_storage_read_query_database(Storage *strg, void *handle,
		const char *query, GHashTable *in_param,
		GHashTable *out_param, int out_param_cnt)
{
	if (!strg || !handle || !query)
		return FALSE;

	if (!strg->ops || !strg->ops->read_query_database) {
		return FALSE;
	}

	return strg->ops->read_query_database(strg, handle, query,
			in_param, out_param, out_param_cnt);
}

gboolean tcore_storage_insert_query_database(Storage *strg, void *handle,
		const char *query, GHashTable *in_param)
{
	if (!strg || !handle || !query)
		return FALSE;

	if (!strg->ops || !strg->ops->insert_query_database) {
		return FALSE;
	}

	return strg->ops->insert_query_database(strg, handle, query, in_param);
}

gboolean tcore_storage_remove_query_database(Storage *strg, void *handle,
		const char *query, GHashTable *in_param)
{
	if (!strg || !handle || !query)
		return FALSE;

	if (!strg->ops || !strg->ops->remove_query_database) {
		return FALSE;
	}

	return strg->ops->remove_query_database(strg, handle, query, in_param);
}
