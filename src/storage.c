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
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "server.h"
#include "storage.h"

struct tcore_storage_type {
	char *name;
	TcorePlugin *parent_plugin;

	TcoreStorageOperations *ops;
	GHashTable *callback;
};

typedef struct {
	TcoreStorageKeyCallback cb_fn;
	void *user_data;
} TcoreStorageCb;

TcoreStorage *tcore_storage_new(TcorePlugin *plugin,
	const char *name, TcoreStorageOperations *ops)
{
	TcoreStorage *strg;

	strg = tcore_malloc0(sizeof(TcoreStorage));

	if (name)
		strg->name = tcore_strdup(name);

	strg->parent_plugin = plugin;
	strg->ops = ops;
	strg->callback =
		g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

	tcore_server_add_storage(tcore_plugin_ref_server(plugin), strg);

	return strg;
}

void tcore_storage_free(TcoreStorage *strg)
{
	if (strg == NULL) {
		err("Storage is NULL");
		return;
	}

	tcore_free(strg->name);
	tcore_free(strg);
}

const char *tcore_storage_ref_name(TcoreStorage *strg)
{
	if (strg == NULL) {
		err("Storage is NULL");
		return NULL;
	}

	return (const char *)strg->name;
}

void *tcore_storage_create_handle(TcoreStorage *strg, const char *path)
{
	if (path == NULL) {
		err("path is NULL");
		return NULL;
	}

	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->create_handle == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->create_handle: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->create_handle : NULL) : NULL));
		return NULL;
	}

	return strg->ops->create_handle(strg, path);
}

gboolean tcore_storage_remove_handle(TcoreStorage *strg, void *handle)
{
	if (handle == NULL) {
		err("handle is NULL");
		return FALSE;
	}

	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->remove_handle == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->remove_handle: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->remove_handle : NULL) : NULL));
		return FALSE;
	}

	return strg->ops->remove_handle(strg, handle);
}

gboolean tcore_storage_set_int(TcoreStorage *strg,
	TcoreStorageKey key, int value)
{
	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->set_int == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->set_int: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->set_int : NULL) : NULL));
		return FALSE;
	}

	return strg->ops->set_int(strg, key, value);
}

gboolean tcore_storage_set_string(TcoreStorage *strg,
	TcoreStorageKey key, const char *value)
{
	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->set_string == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->set_string: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->set_string : NULL) : NULL));
		return FALSE;
	}

	return strg->ops->set_string(strg, key, value);
}

gboolean tcore_storage_set_bool(TcoreStorage *strg,
	TcoreStorageKey key, gboolean value)
{
	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->set_bool == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->set_bool: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->set_bool : NULL) : NULL));
		return FALSE;
	}

	return strg->ops->set_bool(strg, key, value);
}

int tcore_storage_get_int(TcoreStorage *strg, TcoreStorageKey key)
{
	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->get_int == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->get_int: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->get_int : NULL) : NULL));
		return -1;
	}

	return strg->ops->get_int(strg, key);
}

char *tcore_storage_get_string(TcoreStorage *strg, TcoreStorageKey key)
{
	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->get_string == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->get_string: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->get_string : NULL) : NULL));
		return NULL;
	}

	return strg->ops->get_string(strg, key);
}

gboolean tcore_storage_get_bool(TcoreStorage *strg, TcoreStorageKey key)
{
	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->get_bool == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->get_bool: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->get_bool : NULL) : NULL));
		return FALSE;
	}

	return strg->ops->get_bool(strg, key);
}

static void tcore_storage_vkey_callback_dispatcher(TcoreStorage *strg,
		TcoreStorageKey key, void *value)
{
	gchar *key_gen = NULL;
	gpointer hash_node = NULL;

	key_gen = g_strdup_printf("%d", key);

	/* Lookup for 'key' callback list node in Hash table */
	hash_node = g_hash_table_lookup(strg->callback, key_gen);
	if (hash_node != NULL) {
		GSList *cb_list = (GSList *)hash_node;
		TcoreStorageCb *cb_node;

		do {
			cb_node = cb_list->data;
			if (cb_node && cb_node->cb_fn)
				cb_node->cb_fn(key, value, cb_node->user_data);

			cb_list = g_slist_next(cb_list);
		} while (cb_list != NULL);
	}

	tcore_free(key_gen);
}

gboolean tcore_storage_set_key_callback(TcoreStorage *strg,
	TcoreStorageKey key, TcoreStorageKeyCallback cb, void *user_data)
{
	gpointer hash_node = NULL;
	gchar *key_gen = NULL;
	TcoreStorageCb *strg_cb_data = NULL;

	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->set_key_callback == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->set_key_callback: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->set_key_callback : NULL) : NULL));
		return FALSE;
	}

	/* Create callback node */
	strg_cb_data = tcore_malloc0(sizeof(TcoreStorageCb));
	strg_cb_data->cb_fn = cb;
	strg_cb_data->user_data = user_data;

	key_gen = g_strdup_printf("%d", key);

	/* Lookup for 'key' callback list node in Hash table */
	hash_node = g_hash_table_lookup(strg->callback, key_gen);
	if (hash_node != NULL) {
		/*
		 * There is a 'value' (hash_node) previously added
		 * for the 'key'.
		 *
		 * 'callback' is appended to the list of callbacks if it doesn;t
		 * match any of the previously added 'callbacks' for the 'key'.
		 */
		GSList *cb_list = (GSList *)hash_node;
		TcoreStorageCb *cb_node = NULL;

		do {
			cb_node = cb_list->data;
			if (cb_node && (cb_node->cb_fn == cb)) {
				tcore_free(key_gen);
				tcore_free(strg_cb_data);
				return FALSE;
			}
		} while ((cb_list = g_slist_next(cb_list)));

		/* Append additional callback to same 'key' in Hash table */
		hash_node = g_slist_append((GSList *)hash_node, strg_cb_data);
	}
	else {
		/*
		 * There is no 'value' (hash_node) previously added
		 * for the 'key'.
		 */
		GSList *data = NULL;
		data = g_slist_append(data, strg_cb_data);

		/* Add first entry to Hash table for 'key' */
		g_hash_table_insert(strg->callback, tcore_strdup(key_gen), data);

		/* Set Key callback dispatcher */
		strg->ops->set_key_callback(strg,
			key, tcore_storage_vkey_callback_dispatcher);
	}

	tcore_free(key_gen);
	return TRUE;
}

gboolean tcore_storage_remove_key_callback(TcoreStorage *strg,
		TcoreStorageKey key, TcoreStorageKeyCallback cb)
{
	gpointer hash_node = NULL;
	gchar *key_gen = NULL;
	GSList *cb_list = NULL;
	int cb_cnt = 0;
	TcoreStorageCb *cb_node = NULL;

	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->remove_key_callback == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->remove_key_callback: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->remove_key_callback : NULL) : NULL));
		return FALSE;
	}

	key_gen = g_strdup_printf("%d", key);

	/* Lookup for 'key' callback list node in Hash table */
	hash_node = g_hash_table_lookup(strg->callback, key_gen);
	if (hash_node == NULL) {
		tcore_free(key_gen);
		return FALSE;
	}

	cb_list = (GSList *)hash_node;
	do {
		cb_node = cb_list->data;
		if (cb_node && (cb_node->cb_fn == cb)) {
			hash_node = g_slist_remove((GSList *) hash_node, cb_node);
			tcore_free(cb_node);
			break;
		}
	} while ((cb_list = g_slist_next(cb_list)));

	cb_cnt = g_slist_length((GSList *) hash_node);
	dbg("Callback list count: [%d]", cb_cnt);
	if (cb_cnt == 0) {
		/*
		 * No more callbacks registered for 'key',
		 * remove callback list node from Hash table.
		 */
		g_hash_table_remove(strg->callback, key_gen);
		strg->ops->remove_key_callback(strg, key);
	}

	tcore_free(key_gen);
	return TRUE;
}

gboolean tcore_storage_update_query_database(TcoreStorage *strg,
	void *handle, const char *query, GHashTable *in_param)
{
	if ((strg == NULL) || (handle == NULL) || (query == NULL)) {
		err("Storage: [%p] handle: [%p] query: [%p]", strg, handle, query);
		return FALSE;
	}

	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->update_query_database == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->update_query_database: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->update_query_database : NULL) : NULL));
		return FALSE;
	}

	return strg->ops->update_query_database(strg, handle, query, in_param);
}

gboolean tcore_storage_read_query_database(TcoreStorage *strg,
	void *handle, const char *query, GHashTable *in_param,
	GHashTable *out_param, int out_param_cnt)
{
	if ((strg == NULL) || (handle == NULL) || (query == NULL)) {
		err("Storage: [%p] handle: [%p] query: [%p]", strg, handle, query);
		return FALSE;
	}

	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->read_query_database == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->read_query_database: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->read_query_database : NULL) : NULL));
		return FALSE;
	}

	return strg->ops->read_query_database(strg, handle, query,
			in_param, out_param, out_param_cnt);
}

gboolean tcore_storage_insert_query_database(TcoreStorage *strg,
	void *handle, const char *query, GHashTable *in_param)
{
	if ((strg == NULL) || (handle == NULL) || (query == NULL)) {
		err("Storage: [%p] handle: [%p] query: [%p]", strg, handle, query);
		return FALSE;
	}

	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->insert_query_database == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->insert_query_database: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->insert_query_database : NULL) : NULL));
		return FALSE;
	}

	return strg->ops->insert_query_database(strg, handle, query, in_param);
}

gboolean tcore_storage_remove_query_database(TcoreStorage *strg,
	void *handle, const char *query, GHashTable *in_param)
{
	if ((strg == NULL) || (handle == NULL) || (query == NULL)) {
		err("Storage: [%p] handle: [%p] query: [%p]", strg, handle, query);
		return FALSE;
	}

	if ((strg == NULL) || (strg->ops == NULL)
			|| (strg->ops->remove_query_database == NULL)) {
		err("strg: [%p] strg->ops: [%p] strg->ops->remove_query_database: [%p]",
			strg, (strg ? strg->ops : NULL), (strg ? (strg->ops ?
			strg->ops->remove_query_database : NULL) : NULL));
		return FALSE;
	}

	return strg->ops->remove_query_database(strg, handle, query, in_param);
}
