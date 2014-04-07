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

#ifndef __CORE_OBJECT_H__
#define __CORE_OBJECT_H__

#ifdef __cplusplus
extern "C" {
#endif

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
#define CORE_OBJECT_TYPE_CUSTOM	(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_CUSTOM)
#define CORE_OBJECT_TYPE_GPS		(CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_GPS)

#define CORE_OBJECT_CHECK(co,t) \
	if (co == NULL) { warn("core_object is NULL"); return; } \
	if (tcore_object_get_type(co) != t) { \
		warn("type(0x%x != 0x%x) mismatch", tcore_object_get_type(co), t); \
		return; \
	}

#define CORE_OBJECT_CHECK_RETURN(co,t,r) \
	if (co == NULL) { warn("core_object is NULL"); return r; } \
	if (tcore_object_get_type(co) != t) { \
		warn("type(0x%x != 0x%x) mismatch", tcore_object_get_type(co), t); \
		return r; \
	}

#define GET_OBJECT_TYPE(command)	((command & 0x0FF00000) | CORE_OBJECT_TYPE_DEFAULT)

typedef struct tcore_object_mapping_tbl TcoreObjectMappingTable;

typedef void (*TcoreObjectFreeHook)(CoreObject *co);
typedef void (*TcoreObjectCloneHook)(CoreObject *src, CoreObject *dest);
typedef gboolean (*TcoreObjectCallback)(CoreObject *co,
	const void *event_info, void *user_data);

typedef gboolean (*TcoreObjectInit)(TcorePlugin *plugin, CoreObject *co);
typedef void (*TcoreObjectDeinit)(TcorePlugin *plugin, CoreObject *co);

/* Core Object Initializers */
typedef struct {
	TcoreObjectInit modem_init;
	TcoreObjectInit sim_init;
	TcoreObjectInit sat_init;
	TcoreObjectInit sap_init;
	TcoreObjectInit network_init;
	TcoreObjectInit ps_init;
	TcoreObjectInit call_init;
	TcoreObjectInit ss_init;
	TcoreObjectInit sms_init;
	TcoreObjectInit phonebook_init;
	TcoreObjectInit gps_init;
	/* To be updated based on New modules */
} TcoreObjectInitializer;

/* Core Object De-initializers */
typedef struct {
	TcoreObjectDeinit modem_deinit;
	TcoreObjectDeinit sim_deinit;
	TcoreObjectDeinit sat_deinit;
	TcoreObjectDeinit sap_deinit;
	TcoreObjectDeinit network_deinit;
	TcoreObjectDeinit ps_deinit;
	TcoreObjectDeinit call_deinit;
	TcoreObjectDeinit ss_deinit;
	TcoreObjectDeinit sms_deinit;
	TcoreObjectDeinit phonebook_deinit;
	TcoreObjectDeinit gps_deinit;
	/* To be updated based on New modules */
} TcoreObjectDeinitializer;

CoreObject *tcore_object_new(TcorePlugin *plugin, TcoreHal *hal);
void tcore_object_free(CoreObject *co);

TelReturn tcore_object_set_free_hook(CoreObject *co,
		TcoreObjectFreeHook free_hook);
TelReturn tcore_object_set_clone_hook(CoreObject *co,
		TcoreObjectCloneHook clone_hook);

CoreObject *tcore_object_clone(CoreObject *src, TcorePlugin *new_parent);
CoreObject *tcore_object_clone_template_object(TcorePlugin *p,
		guint co_type);

TelReturn tcore_object_set_plugin(CoreObject *co, TcorePlugin *plugin);
TcorePlugin *tcore_object_ref_plugin(CoreObject *co);

TelReturn tcore_object_link_object(CoreObject *co, void *object);
void *tcore_object_ref_object(CoreObject *co);

TelReturn tcore_object_set_type(CoreObject *co, guint type);
guint tcore_object_get_type(CoreObject *co);

TelReturn tcore_object_set_hal(CoreObject *co, TcoreHal *hal);
TcoreHal *tcore_object_get_hal(CoreObject *co);

TelReturn tcore_object_link_user_data(CoreObject *co, void *user_data);
void *tcore_object_ref_user_data(CoreObject *co);

TelReturn tcore_object_set_dispatcher(CoreObject *co, TcoreObjectDispatcher func);
TelReturn tcore_object_dispatch_request(CoreObject *co,
		gboolean exec_hooks, TcoreCommand command,
		const void *request, guint request_len,
		TcorePluginResponseCallback cb, const void *user_data);

TelReturn tcore_object_add_callback(CoreObject *co, const gchar *event,
		TcoreObjectCallback callback, void *user_data);
TelReturn tcore_object_del_callback(CoreObject *co, const gchar *event,
		TcoreObjectCallback callback);
TelReturn tcore_object_override_callback(CoreObject *co, const gchar *event,
		TcoreObjectCallback callback, void *user_data);
TelReturn tcore_object_emit_callback(CoreObject *co, const gchar *event,
		const void *event_info);

TelReturn tcore_object_add_request_hook(CoreObject *co,
		TcoreCommand command, TcoreRequestHook func, void *user_data);
void tcore_object_remove_request_hook(CoreObject *co,
		TcoreCommand command, TcoreRequestHook func);

TelReturn tcore_object_add_response_hook(CoreObject *co,
		TcoreCommand command, const void *request,
		TcoreResponseHook func, void *user_data);
void tcore_object_remove_response_hook(CoreObject *co,
		TcoreCommand command, TcoreResponseHook func);

TelReturn tcore_object_add_notification_hook(CoreObject *co,
	TcoreNotification command, TcoreNotificationHook func, void *user_data);
void tcore_object_remove_notification_hook(CoreObject *co,
	TcoreNotification command, TcoreNotificationHook func);
TelReturn tcore_object_send_notification(CoreObject *co,
	TcoreNotification command, guint data_len, void *data);

void *tcore_object_add_mapping_tbl_entry(void *mapping_tbl,
		guint object_type, TcoreHal *hal);
void tcore_object_remove_mapping_tbl(void *mapping_tbl);
void *tcore_object_remove_mapping_tbl_entry(void *mapping_tbl, TcoreHal *hal);
void tcore_object_remove_mapping_tbl_entry_by_type(void *mapping_tbl,
		guint co_type);

void tcore_object_print_mapping_tbl(void *mapping_tbl);

TelReturn tcore_object_init_objects(TcorePlugin *plugin,
		TcoreObjectInitializer *initializer_list);
void tcore_object_deinit_objects(TcorePlugin *plugin,
		TcoreObjectDeinitializer *deinitializer_list);

#ifdef __cplusplus
}
#endif

#endif	/* __CORE_OBJECT_H__ */
