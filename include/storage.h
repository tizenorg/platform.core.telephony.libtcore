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

#ifndef __TCORE_STORAGE_H__
#define __TCORE_STORAGE_H__

__BEGIN_DECLS

#define STORAGE_KEY_INT		0x01000000
#define STORAGE_KEY_BOOL	0x02000000
#define STORAGE_KEY_STRING	0x03000000
#define STORAGE_KEY_DOUBLE	0x04000000

enum tcore_storage_key {
	STORAGE_KEY_TELEPHONY_INT = STORAGE_KEY_INT,
	STORAGE_KEY_TELEPHONY_PLMN,
	STORAGE_KEY_TELEPHONY_LAC,
	STORAGE_KEY_TELEPHONY_CELLID,
	STORAGE_KEY_TELEPHONY_SVCTYPE,
	STORAGE_KEY_TELEPHONY_SVC_CS,

	STORAGE_KEY_TELEPHONY_SVC_PS,
	STORAGE_KEY_TELEPHONY_SVC_ROAM,
	STORAGE_KEY_TELEPHONY_SIM_PB_INIT,
	STORAGE_KEY_TELEPHONY_CALL_STATE,
	STORAGE_KEY_TELEPHONY_CALL_FORWARD_STATE,

	STORAGE_KEY_TELEPHONY_TAPI_STATE,
	STORAGE_KEY_TELEPHONY_SPN_DISP_CONDITION,
	STORAGE_KEY_TELEPHONY_ZONE_ZUHAUSE,
	STORAGE_KEY_TELEPHONY_RSSI,
	STORAGE_KEY_TELEPHONY_SIM_SLOT,

	STORAGE_KEY_PM_STATE,
	STORAGE_KEY_PACKET_SERVICE_STATE,
	STORAGE_KEY_MESSAGE_NETWORK_MODE,
	STORAGE_KEY_CELLULAR_STATE,
	STORAGE_KEY_CELLULAR_PKT_TOTAL_RCV,

	STORAGE_KEY_CELLULAR_PKT_TOTAL_SNT,
	STORAGE_KEY_CELLULAR_PKT_LAST_RCV,
	STORAGE_KEY_CELLULAR_PKT_LAST_SNT,
	STORAGE_KEY_TELEPHONY_BOOL = STORAGE_KEY_BOOL,
	STORAGE_KEY_3G_ENABLE,

	STORAGE_KEY_TELEPHONY_READY,
	STORAGE_KEY_SETAPPL_STATE_DATA_ROAMING_BOOL,
	STORAGE_KEY_SETAPPL_STATE_AUTOMATIC_TIME_UPDATE_BOOL,
	STORAGE_KEY_FLIGHT_MODE_BOOL,
	STORAGE_KEY_TELEPHONY_STRING = STORAGE_KEY_STRING,

	STORAGE_KEY_TELEPHONY_NWNAME,
	STORAGE_KEY_TELEPHONY_SPN_NAME,
	STORAGE_KEY_TELEPHONY_IMEI,
	STORAGE_KEY_TELEPHONY_SUBSCRIBER_NUMBER,
	STORAGE_KEY_TELEPHONY_SWVERSION,

	STORAGE_KEY_TELEPHONY_HWVERSION,
	STORAGE_KEY_TELEPHONY_IMEI_FACTORY_REBOOT,
	STORAGE_KEY_TELEPHONY_SIM_FACTORY_MODE,
	STORAGE_KEY_TELEPHONY_FACTORY_KSTRINGB,
	STORAGE_KEY_TELEPHONY_IMSI,

	STORAGE_KEY_LANGUAGE_SET,
	STORAGE_KEY_IDLE_SCREEN_LAUNCHED_BOOL,
};

enum storage_value {
	STORAGE_VALUE_UNKNOWN = 0,
	STORAGE_VALUE_OFF = 0,
	STORAGE_VALUE_ON  = 1,
	STORAGE_VALUE_STATE_0 = 0,
	STORAGE_VALUE_STATE_1 = 1,
	STORAGE_VALUE_STATE_2 = 2,
	STORAGE_VALUE_STATE_3 = 3,
	STORAGE_VALUE_STATE_4 = 4,
	STORAGE_VALUE_STATE_5 = 5,
	STORAGE_VALUE_STATE_6 = 6,
	STORAGE_VALUE_STATE_7 = 7,
	STORAGE_VALUE_STATE_8 = 8,
	STORAGE_VALUE_STATE_9 = 9,
};

typedef void (*TcoreStorageKeyCallback) (enum tcore_storage_key key,
    void *value, void *user_data);
typedef void (*TcoreStorageDispatchCallback) (Storage *strg,
    enum tcore_storage_key key, void *value);

struct storage_operations {
	void* (*create_handle)(Storage *strg, const char *path);
	gboolean (*remove_handle)(Storage *strg, void *handle);

	gboolean (*set_int)(Storage *strg, enum tcore_storage_key key, int value);
	gboolean (*set_string)(Storage *strg, enum tcore_storage_key key, const char *value);
	gboolean (*set_bool)(Storage *strg, enum tcore_storage_key key, gboolean value);
	int (*get_int)(Storage *strg, enum tcore_storage_key key);
	char* (*get_string)(Storage *strg, enum tcore_storage_key key);
	gboolean (*get_bool)(Storage *strg, enum tcore_storage_key key);
	gboolean (*set_key_callback)(Storage *strg, enum tcore_storage_key key,
			TcoreStorageDispatchCallback cb);
	gboolean (*remove_key_callback)(Storage *strg, enum tcore_storage_key key);

	gboolean (*update_query_database)(Storage *strg, void *handle,
			const char *query, GHashTable *in_param);
	gboolean (*read_query_database)(Storage *strg, void *handle,
			const char *query, GHashTable *in_param,
			GHashTable *out_param, int out_param_cnt);
	gboolean (*insert_query_database)(Storage *strg, void *handle,
			const char *query, GHashTable *in_param);
	gboolean (*remove_query_database)(Storage *strg, void *handle,
			const char *query, GHashTable *in_param);
};

Storage*     tcore_storage_new(TcorePlugin *plugin, const char *name,
                 struct storage_operations *ops);
void         tcore_storage_free(Storage *strg);
const char*  tcore_storage_ref_name(Storage *strg);

void*        tcore_storage_create_handle(Storage *strg, const char *path);
gboolean     tcore_storage_remove_handle(Storage *strg, void *handle);

//storage vconf
gboolean     tcore_storage_set_int(Storage *strg, enum tcore_storage_key key,
                 int value);
int          tcore_storage_get_int(Storage *strg, enum tcore_storage_key key);

gboolean     tcore_storage_set_string(Storage *strg, enum tcore_storage_key key,
                 const char *value);
char*        tcore_storage_get_string(Storage *strg, enum tcore_storage_key key);

gboolean     tcore_storage_set_bool(Storage *strg, enum tcore_storage_key key,
                 gboolean value);
gboolean     tcore_storage_get_bool(Storage *strg, enum tcore_storage_key key);

gboolean     tcore_storage_set_key_callback(Storage *strg,
                 enum tcore_storage_key key, TcoreStorageKeyCallback cb,
                 void *user_data);
gboolean     tcore_storage_remove_key_callback(Storage *strg,
                 enum tcore_storage_key key, TcoreStorageKeyCallback cb);

//storage database
gboolean     tcore_storage_update_query_database(Storage *strg, void *handle,
                 const char *query, GHashTable *in_param);
gboolean     tcore_storage_read_query_database(Storage *strg, void *handle,
                 const char *query, GHashTable *in_param, GHashTable *out_param,
                 int out_param_cnt);
gboolean     tcore_storage_insert_query_database(Storage *strg, void *handle,
                 const char *query, GHashTable *in_param);
gboolean     tcore_storage_remove_query_database(Storage *strg, void *handle,
                 const char *query, GHashTable *in_param);

__END_DECLS

#endif
