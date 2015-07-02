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

#ifndef __TCORE_CORE_OBJECT_H__
#define __TCORE_CORE_OBJECT_H__

__BEGIN_DECLS

#define CORE_OBJECT_TYPE_DEFAULT	0xB0000000
#define CORE_OBJECT_TYPE_MODEM	(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_MODEM)
#define CORE_OBJECT_TYPE_CALL		(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_CALL)
#define CORE_OBJECT_TYPE_SS		(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_SS)
#define CORE_OBJECT_TYPE_NETWORK	(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_NETWORK)
#define CORE_OBJECT_TYPE_PS		(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_PS)
#define CORE_OBJECT_TYPE_PS_CONTEXT	(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_PDP)
#define CORE_OBJECT_TYPE_SIM		(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_SIM)
#define CORE_OBJECT_TYPE_SAT		(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_SAT)
#define CORE_OBJECT_TYPE_SAP		(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_SAP)
#define CORE_OBJECT_TYPE_SMS		(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_SMS)
#define CORE_OBJECT_TYPE_PHONEBOOK	(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_PHONEBOOK)
#define CORE_OBJECT_TYPE_SOUND	(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_SOUND)
#define CORE_OBJECT_TYPE_CUSTOM	(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_CUSTOM)
#define CORE_OBJECT_TYPE_GPS		(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_GPS)

#define CORE_OBJECT_CHECK(o,t) \
	if (!o) { warn("core_object is NULL"); return; } \
	if (tcore_object_get_type(o) != t) { warn("type(0x%x != 0x%x) mismatch", tcore_object_get_type(o), t); return; }
#define CORE_OBJECT_CHECK_RETURN(o,t,r) \
	if (!o) { warn("core_object is NULL"); return r; } \
	if (tcore_object_get_type(o) != t) { warn("type(0x%x != 0x%x) mismatch", tcore_object_get_type(o), t); return r; }

enum tcore_ops_type {
	TCORE_OPS_TYPE_CP = 0,
	TCORE_OPS_TYPE_IMS,
	TCORE_OPS_TYPE_MAX
};

#define CORE_OBJECT_VALIDATE_OPS_RETURN(ops_type)  \
do { \
	switch (ops_type) { \
	case TCORE_OPS_TYPE_CP: \
	case TCORE_OPS_TYPE_IMS: \
	break; \
	case TCORE_OPS_TYPE_MAX: \
	default: \
		err("Invalid/Unsupported ops_type:[%d]", ops_type); \
		return; \
	} \
} while (FALSE)

#define CORE_OBJECT_VALIDATE_OPS_RETURN_VAL(ops_type,ret_val)  \
do { \
	switch (ops_type) { \
	case TCORE_OPS_TYPE_CP: \
	case TCORE_OPS_TYPE_IMS: \
	break; \
	case TCORE_OPS_TYPE_MAX: \
	default: \
		err("Invalid/Unsupported ops_type:[%d]", ops_type); \
		return ret_val; \
	} \
} while (FALSE)

typedef struct tcore_object_mapping_tbl object_mapping_table_t;

#define CORE_OBJECT_EVENT_PROPERTY_CHANGED "core_object_property_changed"
#define CORE_OBJECT_KEY_FIND(keys, k) g_slist_find_custom((keys), (k), (GCompareFunc)g_strcmp0)

typedef void (*CoreObjectFreeHook)(CoreObject *co);
typedef gboolean (*CoreObjectCallback)(CoreObject *co, const void *event_info, void *user_data);
typedef gboolean (*tcore_object_callback)(CoreObject *co,
				const void *event_info, void *user_data);
typedef TReturn (*CoreObjectDispatcher)(CoreObject *co, UserRequest *ur, enum tcore_ops_type ops_type);

typedef gboolean (*tcore_object_init)(TcorePlugin *plugin, CoreObject *co);
typedef void (*tcore_object_deinit)(TcorePlugin *plugin, CoreObject *co);

/* Core Object Initializers */
struct object_initializer {
	tcore_object_init modem_init;
	tcore_object_init sim_init;
	tcore_object_init sat_init;
	tcore_object_init sap_init;
	tcore_object_init network_init;
	tcore_object_init ps_init;
	tcore_object_init call_init;
	tcore_object_init ss_init;
	tcore_object_init sms_init;
	tcore_object_init phonebook_init;
	tcore_object_init gps_init;
	/* To be updated based on New modules */
};

/* Core Object De-initializers */
struct object_deinitializer {
	tcore_object_deinit modem_deinit;
	tcore_object_deinit sim_deinit;
	tcore_object_deinit sat_deinit;
	tcore_object_deinit sap_deinit;
	tcore_object_deinit network_deinit;
	tcore_object_deinit ps_deinit;
	tcore_object_deinit call_deinit;
	tcore_object_deinit ss_deinit;
	tcore_object_deinit sms_deinit;
	tcore_object_deinit phonebook_deinit;
	tcore_object_deinit gps_deinit;
	/* To be updated based on New modules */
};


CoreObject *tcore_object_new(TcorePlugin *plugin, const char *name, TcoreHal *hal);
void tcore_object_free(CoreObject *co);

TReturn tcore_object_set_free_hook(CoreObject *co, CoreObjectFreeHook free_hook);

TReturn tcore_object_set_name(CoreObject *co, const char *name);
const char *tcore_object_ref_name(CoreObject *co);

TReturn tcore_object_set_plugin(CoreObject *co, TcorePlugin *plugin);
TcorePlugin *tcore_object_ref_plugin(CoreObject *co);

TReturn tcore_object_link_object(CoreObject *co, void *object);
void *tcore_object_ref_object(CoreObject *co);

TReturn tcore_object_set_type(CoreObject *co, unsigned int type);
unsigned int tcore_object_get_type(CoreObject *co);

TReturn tcore_object_set_hal(CoreObject *co, TcoreHal *hal);
TcoreHal* tcore_object_get_hal(CoreObject *co);

TReturn tcore_object_link_user_data(CoreObject *co, void *user_data);
void *tcore_object_ref_user_data(CoreObject *co);

TReturn tcore_object_set_dispatcher(CoreObject *co, CoreObjectDispatcher func);
TReturn tcore_object_dispatch_request(CoreObject *co, UserRequest *ur);
TReturn tcore_object_dispatch_request_with_type(CoreObject *co, UserRequest *ur, enum tcore_ops_type ops_type);

TReturn tcore_object_add_callback(CoreObject *co, const char *event, CoreObjectCallback callback, void *user_data);
TReturn tcore_object_del_callback(CoreObject *co, const char *event, CoreObjectCallback callback);
TReturn tcore_object_override_callback(CoreObject *co, const char *event, CoreObjectCallback callback, void *user_data);
TReturn tcore_object_emit_callback(CoreObject *co, const char *event, const void *event_info);

#define tcore_object_set_property(co, ...) tcore_object_set_property_full(co, __VA_ARGS__, NULL, NULL)
TReturn tcore_object_set_property_full(CoreObject *co, const char *first_property, ...);
const char *tcore_object_ref_property(CoreObject *co, const char *key);
GHashTable* tcore_object_ref_property_hash(CoreObject *co);
void *tcore_object_add_mapping_tbl_entry(void *mapping_tbl,
	unsigned int object_type, TcoreHal *hal);
void tcore_object_remove_mapping_tbl(void *mapping_tbl);
void *tcore_object_remove_mapping_tbl_entry(void *mapping_tbl, TcoreHal *hal);
void tcore_object_remove_mapping_tbl_entry_by_type(void *mapping_tbl,
	unsigned int co_type);

void tcore_object_print_mapping_tbl(void *mapping_tbl);

TReturn tcore_object_init_objects(TcorePlugin *plugin,
	struct object_initializer *initializer_list);
void tcore_object_deinit_objects(TcorePlugin *plugin,
	struct object_deinitializer *deinitializer_list);

__END_DECLS

#endif /* __TCORE_CORE_OBJECT_H__ */
