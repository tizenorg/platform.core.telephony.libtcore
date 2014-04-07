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

#ifndef __STORAGE_H__
#define __STORAGE_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
	STORAGE_KEY_INT = 0x01000000,
	STORAGE_KEY_PLMN,
	STORAGE_KEY_LAC,
	STORAGE_KEY_CELLID,
	STORAGE_KEY_SVCTYPE,
	STORAGE_KEY_SVC_CS,

	STORAGE_KEY_SVC_PS,
	STORAGE_KEY_SPN_DISP_CONDITION,
	STORAGE_KEY_RSSI,
	STORAGE_KEY_SIM_SLOT,
	STORAGE_KEY_PM_STATE,

	STORAGE_KEY_PACKET_SERVICE_STATE,
	STORAGE_KEY_PACKET_INDICATOR_STATE,
	STORAGE_KEY_CELLULAR_STATE,
	STORAGE_KEY_CELLULAR_PKT_TOTAL_RCV,
	STORAGE_KEY_CELLULAR_PKT_TOTAL_SNT,

	STORAGE_KEY_CELLULAR_PKT_LAST_RCV,
	STORAGE_KEY_CELLULAR_PKT_LAST_SNT,
	STORAGE_KEY_SIM_PB_INIT,
	STORAGE_KEY_SVC_ROAM,
	STORAGE_KEY_TAPI_STATE,

	STORAGE_KEY_BOOL = 0x02000000,
	STORAGE_KEY_CALL_FORWARD_STATE,
	STORAGE_KEY_DATA_ENABLE,
	STORAGE_KEY_READY,
	STORAGE_KEY_SETAPPL_STATE_DATA_ROAMING,
	STORAGE_KEY_SETAPPL_STATE_AUTOMATIC_TIME_UPDATE,

	STORAGE_KEY_FLIGHT_MODE,
	STORAGE_KEY_IDLE_SCREEN_LAUNCHED,

	STORAGE_KEY_STRING = 0x03000000,
	STORAGE_KEY_NWNAME,
	STORAGE_KEY_SPN_NAME,
	STORAGE_KEY_LANGUAGE_SET,
} TcoreStorageKey;

typedef void (*TcoreStorageKeyCallback) (TcoreStorageKey key,
	void *value, void *user_data);
typedef void (*TcoreStorageDispatchCallback) (TcoreStorage *strg,
	TcoreStorageKey key, void *value);

typedef struct {
	void *(*create_handle)(TcoreStorage *strg, const char *path);
	gboolean (*remove_handle)(TcoreStorage *strg, void *handle);

	gboolean (*set_int)(TcoreStorage *strg,
		TcoreStorageKey key, int value);
	gboolean (*set_string)(TcoreStorage *strg,
		TcoreStorageKey key, const char *value);
	gboolean (*set_bool)(TcoreStorage *strg,
		TcoreStorageKey key, gboolean value);

	int (*get_int)(TcoreStorage *strg, TcoreStorageKey key);
	char *(*get_string)(TcoreStorage *strg, TcoreStorageKey key);
	gboolean (*get_bool)(TcoreStorage *strg, TcoreStorageKey key);

	gboolean (*set_key_callback)(TcoreStorage *strg,
		TcoreStorageKey key, TcoreStorageDispatchCallback cb);
	gboolean (*remove_key_callback)(TcoreStorage *strg, TcoreStorageKey key);

	gboolean (*update_query_database)(TcoreStorage *strg,
		void *handle, const char *query, GHashTable *in_param);
	gboolean (*read_query_database)(TcoreStorage *strg,
		void *handle, const char *query, GHashTable *in_param,
		GHashTable *out_param, int out_param_cnt);
	gboolean (*insert_query_database)(TcoreStorage *strg,
		void *handle, const char *query, GHashTable *in_param);
	gboolean (*remove_query_database)(TcoreStorage *strg,
		void *handle, const char *query, GHashTable *in_param);
}TcoreStorageOperations;

TcoreStorage *tcore_storage_new(TcorePlugin *plugin,
	const char *name, TcoreStorageOperations *ops);
void tcore_storage_free(TcoreStorage *strg);

const char *tcore_storage_ref_name(TcoreStorage *strg);

void *tcore_storage_create_handle(TcoreStorage *strg, const char *path);
gboolean tcore_storage_remove_handle(TcoreStorage *strg, void *handle);

/* 'vconf' TcoreStorage */
gboolean tcore_storage_set_int(TcoreStorage *strg,
	TcoreStorageKey key, int value);
int  tcore_storage_get_int(TcoreStorage *strg, TcoreStorageKey key);

gboolean tcore_storage_set_string(TcoreStorage *strg,
	TcoreStorageKey key, const char *value);
char *tcore_storage_get_string(TcoreStorage *strg, TcoreStorageKey key);

gboolean tcore_storage_set_bool(TcoreStorage *strg,
	TcoreStorageKey key, gboolean value);
gboolean tcore_storage_get_bool(TcoreStorage *strg, TcoreStorageKey key);

gboolean tcore_storage_set_key_callback(TcoreStorage *strg,
	TcoreStorageKey key, TcoreStorageKeyCallback cb, void *user_data);
gboolean tcore_storage_remove_key_callback(TcoreStorage *strg,
	TcoreStorageKey key, TcoreStorageKeyCallback cb);

/* 'database' TcoreStorage */
gboolean tcore_storage_update_query_database(TcoreStorage *strg,
	void *handle, const char *query, GHashTable *in_param);
gboolean tcore_storage_read_query_database(TcoreStorage *strg,
	void *handle, const char *query, GHashTable *in_param,
	GHashTable *out_param, int out_param_cnt);
gboolean tcore_storage_insert_query_database(TcoreStorage *strg,
	void *handle, const char *query, GHashTable *in_param);
gboolean tcore_storage_remove_query_database(TcoreStorage *strg,
	void *handle, const char *query, GHashTable *in_param);

#ifdef __cplusplus
}
#endif

#endif	/* __STORAGE_H__ */
