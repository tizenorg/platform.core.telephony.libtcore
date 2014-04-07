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

#ifndef __SERVER_H__
#define __SERVER_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef TcoreHookReturn (*TcoreServerNotificationHook)(Server *s,
		TcoreServerNotification command, guint data_len, void *data, void *user_data);

Server *tcore_server_new();
void tcore_server_free(Server *s);

TelReturn tcore_server_run(Server *s);
TelReturn tcore_server_exit(Server *s);

TelReturn tcore_server_add_plugin(Server *s, TcorePlugin *plugin);
TelReturn tcore_server_remove_plugin(Server *s, TcorePlugin *plugin);
GSList *tcore_server_ref_plugins(Server *s);
TcorePlugin *tcore_server_find_plugin(Server *s, const gchar *name);

TelReturn tcore_server_add_communicator(Server *s, Communicator *comm);
TelReturn tcore_server_remove_communicator(Server *s, Communicator *comm);
GSList *tcore_server_ref_communicators(Server *s);
Communicator *tcore_server_find_communicator(Server *s, const gchar *name);

TelReturn tcore_server_add_storage(Server *s, TcoreStorage *strg);
TelReturn tcore_server_remove_storage(Server *s, TcoreStorage *strg);
GSList *tcore_server_ref_storages(Server *s);
TcoreStorage * tcore_server_find_storage(Server *s, const gchar *name);

TelReturn tcore_server_add_template_object(Server *s, CoreObject *template_co);
TelReturn tcore_server_remove_template_object(Server *s, CoreObject *template_co);
GSList *tcore_server_ref_template_object(Server *s);
CoreObject *tcore_server_find_template_object(Server *s, guint type);

TelReturn tcore_server_link_udev(Server *s, TcoreUdev *udev);
TcoreUdev *tcore_server_ref_udev(Server *s);

TelReturn tcore_server_send_server_notification(Server *s,
	TcoreServerNotification command, guint data_len, void *data);
TelReturn tcore_server_send_notification(Server *s,
	TcorePlugin *source, TcoreNotification command, guint data_len, void *data);

TelReturn tcore_server_add_notification_hook(Server *s,
	TcoreServerNotification command, TcoreServerNotificationHook func, void *user_data);
TelReturn tcore_server_remove_notification_hook(Server *s,
	TcoreServerNotificationHook func);

gboolean tcore_server_register_modem(Server *s, TcorePlugin *modem_iface_plugin);
void tcore_server_unregister_modem(Server *s, TcorePlugin *modem_iface_plugin);
gboolean tcore_server_update_modem_plugin(TcorePlugin *modem_iface_plugin,
	TcorePlugin *modem_plugin);

const gchar *tcore_server_get_cp_name_by_plugin(TcorePlugin *plugin);

gboolean tcore_server_add_cp_mapping_tbl_entry(TcorePlugin *modem_iface_plugin,
	guint co_type, TcoreHal *hal);
void tcore_server_remove_cp_mapping_tbl(TcorePlugin *modem_iface_plugin);
void tcore_server_remove_cp_mapping_tbl_entry(TcorePlugin *modem_iface_plugin,
	TcoreHal *hal);
void *tcore_server_get_cp_mapping_tbl(TcorePlugin *modem_plugin);

void tcore_server_print_modems(TcorePlugin *plugin);

TelReturn tcore_server_load_modem_plugin(Server *s,
	TcorePlugin *modem_if_plugin, const gchar *name);
void tcore_server_unload_modem_plugin(Server *s,
	TcorePlugin *modem_if_plugin);

GSList *tcore_server_get_modem_plugin_list(Server *s);

#ifdef __cplusplus
}
#endif

#endif	/* __SERVER_H__ */
