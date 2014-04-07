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

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TcoreResponseCallback)(gint result, const void *response, void *user_data);

enum tcore_plugin_priority {
	TCORE_PLUGIN_PRIORITY_HIGH = -100,
	TCORE_PLUGIN_PRIORITY_MID = 0,
	TCORE_PLUGIN_PRIORITY_LOW = +100
};

struct tcore_plugin_define_desc {
	gchar *name;
	enum tcore_plugin_priority priority;
	int version;
	gboolean (*load)();
	gboolean (*init)(TcorePlugin *);
	void (*unload)(TcorePlugin *);
};


TcorePlugin *tcore_plugin_new(Server *server,
		const struct tcore_plugin_define_desc *desc,
		const gchar *filename, void *handle);
void tcore_plugin_free(TcorePlugin *plugin);

const struct tcore_plugin_define_desc *tcore_plugin_get_description(TcorePlugin *plugin);

gchar *tcore_plugin_get_filename(TcorePlugin *plugin);
const gchar *tcore_plugin_ref_plugin_name(TcorePlugin *plugin);
Server *tcore_plugin_ref_server(TcorePlugin *plugin);

TelReturn tcore_plugin_link_user_data(TcorePlugin *plugin, void *user_data);
void *tcore_plugin_ref_user_data(TcorePlugin *plugin);

TelReturn tcore_plugin_add_core_object(TcorePlugin *plugin, CoreObject *co);
TelReturn tcore_plugin_remove_core_object(TcorePlugin *plugin, CoreObject *co);

GSList *tcore_plugin_ref_core_objects(TcorePlugin *plugin);
CoreObject *tcore_plugin_ref_core_object(TcorePlugin *plugin, unsigned int type);
GSList *tcore_plugin_get_core_objects_bytype(TcorePlugin *plugin,
		unsigned int type);

TelReturn tcore_plugin_core_object_event_emit(TcorePlugin *plugin,
		const gchar *event, const void *event_info);

TelReturn tcore_plugin_link_property(TcorePlugin *plugin, const gchar *key,
		void *data);
void *tcore_plugin_ref_property(TcorePlugin *plugin, const gchar *key);

TelReturn tcore_plugin_add_request_hook(TcorePlugin *plugin,
	TcoreCommand command, TcoreRequestHook func, void *user_data);
void tcore_plugin_remove_request_hook(TcorePlugin *plugin,
	TcoreCommand command, TcoreRequestHook func);
TelReturn tcore_plugin_add_notification_hook(TcorePlugin *plugin,
	TcoreNotification command, TcoreNotificationHook func, void *user_data);
void tcore_plugin_remove_notification_hook(TcorePlugin *plugin,
	TcoreNotification command, TcoreNotificationHook func);

TelReturn tcore_plugin_dispatch_request(TcorePlugin *plugin,
		gboolean exec_hooks, TcoreCommand command,
		const void *request, guint request_len,
		TcoreResponseCallback cb, const void *user_data);
TelReturn tcore_plugin_send_notification(TcorePlugin *plugin,
	TcoreNotification command, guint data_len, void *data);

#ifdef __cplusplus
}
#endif

#endif	/* __PLUGIN_H__ */
