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
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <dlfcn.h>

#include <glib.h>
#include <glib/gprintf.h>

#include "tcore.h"
#include "server.h"
#include "plugin.h"
#include "core_object.h"
#include "communicator.h"
#include "storage.h"
#include "udev.h"
#include "util.h"

#define MODEMS_PATH "/usr/lib/telephony/plugins/modems"

struct tcore_server_type {
	GMainLoop *mainloop;

	GSList *plugins;
	GSList *communicators;
	GSList *storages;
	GSList *modems;

	GSList *template_co;

	GSList *hook_list_notification;

	TcorePlugin *default_plugin;
	TcoreUdev *udev;
};

struct tcore_modem_type {
	gchar *cp_name;
	TcorePlugin *modem_iface_plugin;

	TcorePlugin *modem_plugin;

	void *mapping_tbl;
};

struct hook_notification_type {
	TcoreServerNotification command;
	TcoreServerNotificationHook func;
	void *user_data;
};

static gint __compare_priority(gconstpointer a, gconstpointer b)
{
	TcorePlugin *plugin1 = (TcorePlugin *)a;
	TcorePlugin *plugin2 = (TcorePlugin *)b;

	if (plugin2 == NULL)
		return 1;

	return tcore_plugin_get_description(plugin1)->priority -
		tcore_plugin_get_description(plugin2)->priority;
}

static gchar *_server_enumerate_modem(TcorePlugin *plugin)
{
	static guint cp_counter = 0;
	const gchar *filename;

	if (plugin == NULL)
		return NULL;

	/*
	 * Presently enumeration is based on Modem Interface Plug-in descriptor name
	 * followed by an incremental Positive integer 'cp_count'.
	 *
	 * For example, if Modem Interface Plug-in descriptor name is 'imcmodem' then,
	 *	'name' would be enumerated as "imcmode_N", where N >= 0
	 */
	filename = tcore_plugin_ref_plugin_name(plugin);
	if (filename == NULL)
		return NULL;

	dbg("%s", filename);

	return g_strdup_printf("%s%d", filename, cp_counter++);
}

static TcoreModem *__server_find_modem(Server *s,
	TcorePlugin *modem_iface_plugin, TcorePlugin *modem_plugin)
{
	GSList *list;
	TcoreModem *modem;

	dbg("Modem Plug-in [0x%x][%s] Modem Interface Plug-in: [0x%x][%s]",
		modem_plugin, tcore_plugin_ref_plugin_name(modem_plugin),
		modem_iface_plugin, tcore_plugin_ref_plugin_name(modem_iface_plugin));

	for (list = s->modems; list; list = list->next) {
		modem = list->data;
		if (modem == NULL)
			continue;

		/*
		 * Specifically for Unregister, Add Modem Plug-in
		 * and Add/Remove Mapping Table -
		 *
		 * MUST match Modem Interface Plug-in
		 *			AND
		 * If passed Modem Interface Plug-in is NULL, then
		 * Modem Interface Plug-in of 'modems' MUST be NULL
		 */
		if ((modem_iface_plugin == modem->modem_iface_plugin)
			&& ((modem_plugin == NULL) && (modem->modem_plugin == NULL))) {
			dbg("'modem' found!!!");
			return modem;
		}

		/*
		 * Specifically for get Mapping Table -
		 *
		 * Modem Interface Plug-in MUST be NULL
		 *			AND
		 * Passed Modem Plug-in MUST match Modem Plug-of 'modems'
		 */
		if ((modem_iface_plugin == NULL)
			&& (modem_plugin == modem->modem_plugin)) {
			dbg("'modem' found!!!");
			return modem;
		}

		/*
		 * Specifically for get CP name -
		 *
		 * Passed Modem OR Modem Interface Plug-in MUST match
		 *			AND
		 * MUST match either Modem OR Modem Interface Plug-in of 'modems'
		 */
		if ((modem_iface_plugin == modem_plugin)
				&& ((modem_iface_plugin == modem->modem_iface_plugin)
				|| (modem_plugin == modem->modem_plugin))) {
			dbg("'modem' found!!!");
			return modem;
		}
	}

	err("Modem not found");
	return NULL;
}

Server *tcore_server_new()
{
	Server *s;

	/* Server structure */
	s = g_try_new0(struct tcore_server_type, 1);
	if (s == NULL) {
		err("Failed to allocate memory");
		return NULL;
	}

	/* g-main loop */
	s->mainloop = g_main_loop_new (NULL, FALSE);
	if (s->mainloop == NULL) {
		err("Failed to allocate memory");

		/* Free resources */
		tcore_free(s);
		return NULL;
	}

	/* Initialize Server structure */
	s->plugins = NULL;
	s->communicators = NULL;
	s->storages = NULL;
	s->template_co = NULL;
	s->hook_list_notification = NULL;
	s->default_plugin = NULL;

	return s;
}

void tcore_server_free(Server *s)
{
	GSList *list;
	TcorePlugin *p;
	const struct tcore_plugin_define_desc *desc;

	if (s == NULL) {
		err("Server is NULL");
		return;
	}

	/* Unref g-main loop */
	if (s->mainloop)
		g_main_loop_unref(s->mainloop);

	/* Unload Plug-ins */
	for (list = s->plugins; list; list = list->next) {
		p = list->data;
		if (p != NULL) {
			desc = (struct tcore_plugin_define_desc *)tcore_plugin_get_description(p);

			/* Unload Plug-in */
			if ((desc != NULL) && (desc->unload != NULL))
				desc->unload(p);

			/* Free resource */
			tcore_plugin_free(p);
			list->data = NULL;
		}
	}

	/* Free resources */
	g_slist_free_full(s->hook_list_notification, g_free);
	g_slist_free(s->template_co);
	g_slist_free(s->storages);
	g_slist_free(s->communicators);
	g_slist_free(s->plugins);

	/* Freeing Server */
	tcore_free(s);
}

TelReturn tcore_server_run(Server *s)
{
	gchar *version;

	if (s == NULL) {
		err("Server is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (s->mainloop == NULL) {
		err("Server mainloop is NULL");
		return TEL_RETURN_FAILURE;
	}

	version = tcore_util_get_version();
	if (version) {
		dbg("libtcore version: %s", version);
		tcore_free(version);
	}

	/* Send 'Server running' notification */
	tcore_server_send_server_notification(s,
		TCORE_SERVER_NOTIFICATION_SERVER_RUN,
		0, NULL);

	/* Enter g-main loop */
	g_main_loop_run(s->mainloop);

	dbg("~*~*~* Server Exit *~*~*~");

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_server_exit(Server *s)
{
	if (s == NULL) {
		err("Server is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (s->mainloop == NULL) {
		err("Server mainloop is NULL");
		return TEL_RETURN_FAILURE;
	}

	/* Quit g-main loop */
	g_main_loop_quit(s->mainloop);

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_server_add_plugin(Server *s, TcorePlugin *plugin)
{
	if ((s == NULL) || (plugin == NULL)) {
		err("s: [%p] plugin: [%p]", s, plugin);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	/*
	 * Plug-in addition to Server list
	 *
	 * Plug-ins are added in a sorted list based on 'priority' of Plug-in
	 */
	s->plugins = g_slist_insert_sorted(s->plugins, plugin, __compare_priority);

	/* Send 'Plug-in added' notification */
	tcore_server_send_server_notification(s,
		TCORE_SERVER_NOTIFICATION_ADDED_PLUGIN,
		0, NULL);

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_server_remove_plugin(Server *s, TcorePlugin *plugin)
{
	if ((s == NULL) || (plugin == NULL)) {
		err("s: [%p] plugin: [%p]", s, plugin);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	s->plugins = g_slist_remove(s->plugins, plugin);

	/* Send 'Plug-in removed' notification */
	tcore_server_send_server_notification(s,
		TCORE_SERVER_NOTIFICATION_REMOVED_PLUGIN,
		0, NULL);

	return TEL_RETURN_SUCCESS;
}

TcorePlugin *tcore_server_find_plugin(Server *s, const gchar *name)
{
	GSList *list;
	TcoreModem *modem;

	dbg("Plug-in Name: [%s]", name);

	for (list = s->modems; list; list = list->next) {
		modem = list->data;
		if (modem != NULL)
			if (g_strcmp0(modem->cp_name, name) == 0)
				return modem->modem_plugin;
	}

	err("Modem plugin '%s' not found", name);
	return NULL;
}

GSList *tcore_server_ref_plugins(Server *s)
{
	if (s == NULL) {
		err("server is NULL");
		return NULL;
	}

	return s->plugins;
}

TelReturn tcore_server_add_communicator(Server *s, Communicator *comm)
{
	if ((s == NULL) || (comm == NULL)) {
		err("s: [%p] comm: [%p]", s, comm);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	/*
	 * Communication addition to Server list
	 *
	 * Communicators are added to the beginning of the list (prepended)
	 */
	s->communicators = g_slist_insert(s->communicators, comm, 0);

	/* Send 'Communicator added' notification */
	tcore_server_send_server_notification(s,
		TCORE_SERVER_NOTIFICATION_ADDED_COMMUNICATOR,
		0, NULL);

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_server_remove_communicator(Server *s, Communicator *comm)
{
	if ((s == NULL) || (comm == NULL)) {
		err("s: [%p] comm: [%p]", s, comm);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	s->communicators = g_slist_remove(s->communicators, comm);

	/* Send 'Communicator removed' notification */
	tcore_server_send_server_notification(s,
		TCORE_SERVER_NOTIFICATION_REMOVED_COMMUNICATOR,
		0, NULL);

	return TEL_RETURN_SUCCESS;
}

GSList *tcore_server_ref_communicators(Server *s)
{
	if (s == NULL) {
		err("server is NULL");
		return NULL;
	}

	return s->communicators;
}

Communicator *tcore_server_find_communicator(Server *s, const gchar *name)
{
	GSList *list;
	Communicator *comm;

	dbg("Communicator Name: [%s]", name);

	for (list = s->communicators; list; list = list->next) {
		comm = list->data;
		if (comm != NULL)
			if (g_strcmp0(tcore_communicator_ref_name(comm), name) == 0)
				return comm;
	}

	return NULL;
}

TelReturn tcore_server_add_storage(Server *s, TcoreStorage *strg)
{
	if ((s == NULL) || (strg == NULL)) {
		err("s: [%p] strg: [%p]", s, strg);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	/*
	 * Storage addition to Server list
	 *
	 * Storages are added to the beginning of the list (prepended)
	 */
	s->storages = g_slist_insert(s->storages, strg, 0);

	/* Send 'Storage added' notification */
	tcore_server_send_server_notification(s,
		TCORE_SERVER_NOTIFICATION_ADDED_STORAGE,
		0, NULL);

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_server_remove_storage(Server *s, TcoreStorage *strg)
{
	if ((s == NULL) || (strg == NULL)) {
		err("s: [%p] strg: [%p]", s, strg);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	s->storages = g_slist_remove(s->storages, strg);

	/* Send 'Storage remove' notification */
	tcore_server_send_server_notification(s,
		TCORE_SERVER_NOTIFICATION_REMOVED_STORAGE,
		0, NULL);

	return TEL_RETURN_SUCCESS;
}

GSList *tcore_server_ref_storages(Server *s)
{
	if (s == NULL) {
		err("server is NULL");
		return NULL;
	}

	return s->storages;
}

TcoreStorage *tcore_server_find_storage(Server *s, const gchar *name)
{
	GSList *list;
	TcoreStorage *strg;

	for (list = s->storages; list; list = list->next) {
		strg = list->data;
		if (strg != NULL)
			if (g_strcmp0(tcore_storage_ref_name(strg), name) == 0)
				return strg;
	}

	return NULL;
}

TelReturn tcore_server_add_template_object(Server *s, CoreObject *template_co)
{
	GSList *list;
	CoreObject *temp;

	if ((s == NULL) || (template_co == NULL)) {
		err("s: [%p] template_co: [%p]", s, template_co);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	/*
	* Template Core Object addition to Server list
	*
	* Only one Template Core Object of a specific type is added to the
	* list on Template Core Objects
	*/
	for (list = s->template_co; list; list = list->next) {
		temp = list->data;
		if (temp != NULL)
			if (tcore_object_get_type(temp) ==
					tcore_object_get_type(template_co))
				return TEL_RETURN_FAILURE;
	}

	/* Template Core Objects are added to the beginning of the list (prepended) */
	s->template_co = g_slist_insert(s->template_co, template_co, 0);

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_server_remove_template_object(Server *s, CoreObject *template_co)
{
	if ((s == NULL) || (template_co == NULL)) {
		err("s: [%p] template_co: [%p]", s, template_co);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	s->template_co = g_slist_remove(s->template_co, template_co);

	return TEL_RETURN_SUCCESS;
}

GSList *tcore_server_ref_template_object(Server *s)
{
	if (s == NULL) {
		err("server is NULL");
		return NULL;
	}

	return s->template_co;
}

CoreObject *tcore_server_find_template_object(Server *s, guint type)
{
	GSList *list;
	CoreObject *template_co;

	dbg("Template Core Object type: [0x%x]", type);

	for (list = s->template_co; list; list = list->next) {
		template_co = list->data;
		if (template_co != NULL)
			if (type == tcore_object_get_type(template_co))
				return template_co;
	}

	return NULL;
}

TelReturn tcore_server_link_udev(Server *s, TcoreUdev *udev)
{
	if ((s == NULL) || (udev == NULL)) {
		err("s: [%p] udev: [%p]", s, udev);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	s->udev = udev;

	return TEL_RETURN_SUCCESS;
}

TcoreUdev *tcore_server_ref_udev(Server *s)
{
	if (s == NULL) {
		err("server is NULL");
		return NULL;
	}

	return s->udev;
}

TelReturn tcore_server_send_server_notification(Server *s,
	TcoreServerNotification command, guint data_len, void *data)
{
	GSList *list;
	Communicator *comm;
	struct hook_notification_type *hook;

	dbg("Send Server Notification!!! Command: [0x%x]", command);

	if (s == NULL) {
		err("server is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	for (list = s->hook_list_notification; list; list = list->next) {
		hook = list->data;
		if (hook != NULL) {
			if (hook->command == command) {
				dbg("Invoking hook_func() for Command: [0x%x]", command);
				if (hook->func(s, command, data_len, data, hook->user_data)
						== TCORE_HOOK_RETURN_STOP_PROPAGATION)
					return TEL_RETURN_SUCCESS;
			}
		}
	}

	for (list = s->communicators; list; list = list->next) {
		comm = list->data;
		if (comm != NULL)
			tcore_communicator_send_notification(comm,
				NULL, command, data_len, data);
	}

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_server_send_notification(Server *s,
	TcorePlugin *source, TcoreNotification command, guint data_len, void *data)
{
	GSList *list;
	Communicator *comm;

	dbg("Send Notification!!! Command: [0x%x]", command);

	if (s == NULL) {
		err("server is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	for (list = s->communicators; list; list = list->next) {
		comm = list->data;
		if (comm != NULL)
			tcore_communicator_send_notification(comm,
				source, command, data_len, data);
	}

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_server_add_notification_hook(Server *s,
	TcoreServerNotification command, TcoreServerNotificationHook func, void *user_data)
{
	struct hook_notification_type *hook;

	if ((s == NULL) || (func == NULL)) {
		err("server: [0x%x] func: [0x%x]", s, func);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (command < TCORE_SERVER_NOTIFICATION_SERVER_RUN
			|| command > TCORE_SERVER_NOTIFICATION_MODEM_ERR) {
		err("Invalid command: [0x%x]", command);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	/* Create 'hook' node */
	hook = g_try_new0(struct hook_notification_type, 1);
	if (hook == NULL) {
		err("Failed to allocate memory");
		return TEL_RETURN_MEMORY_FAILURE;
	}
	hook->command = command;
	hook->func = func;
	hook->user_data = user_data;

	/* Notification Hooks are appended to the list */
	s->hook_list_notification = g_slist_append(s->hook_list_notification, hook);
	dbg("Server Notification hook added for Command: [0x%x]", command);

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_server_remove_notification_hook(Server *s,
		TcoreServerNotificationHook func)
{
	struct hook_notification_type *hook;
	GSList *list;

	if (s == NULL) {
		err("Server is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	list = s->hook_list_notification;
	while (list) {
		GSList *tmp_list = list->next;

		hook = list->data;
		if (hook != NULL) {
			if (hook->func == func) {
				/* Remove 'hook' node */
				s->hook_list_notification =
					g_slist_remove(s->hook_list_notification, hook);
				dbg("Server Notification hook removed for Command: [0x%x]",
					hook->command);

				/* Free resource */
				tcore_free(hook);
			}
		}

		/* Proceed to next node */
		list = tmp_list;
	}

	return TEL_RETURN_SUCCESS;
}

gboolean tcore_server_register_modem(Server *s, TcorePlugin *modem_iface_plugin)
{
	TcoreModem *modem;

	if ((s == NULL) || (modem_iface_plugin == NULL)) {
		err("server [0x%x] Modem Interface Plug-in: [0x%x]", s, modem_iface_plugin);
		return FALSE;
	}

	modem = g_try_new0(TcoreModem, 1);
	if (modem == NULL) {
		err("Failed to allocate memory");
		return FALSE;
	}

	/* Enumerate CP Name */
	modem->cp_name = _server_enumerate_modem(modem_iface_plugin);
	modem->modem_iface_plugin = modem_iface_plugin;

	/*
	 * 'modem' addition to modems list
	 *
	 * 'modem' is appended to modems list
	 */
	s->modems = g_slist_append(s->modems, modem);
	dbg("Added to 'modems' entry - CP Name: [%s] Modem Interface Plug-in: [%s]",
		modem->cp_name, tcore_plugin_ref_plugin_name(modem_iface_plugin));

	return TRUE;
}

void tcore_server_unregister_modem(Server *s, TcorePlugin *modem_iface_plugin)
{
	TcoreModem *modem;

	if ((s == NULL) || (modem_iface_plugin == NULL)) {
		err("server [0x%x] Modem Interface Plug-in: [0x%x]", s, modem_iface_plugin);
		return;
	}

	/* Find 'modem' */
	modem = __server_find_modem(s, modem_iface_plugin, NULL);
	if (modem == NULL) {
		err("Failed to find 'modem' for Modem Interface Plug-in: [%s]",
			tcore_plugin_ref_plugin_name(modem_iface_plugin));
		return;
	}

	/* Remove 'modem' from modems list */
	s->modems = g_slist_remove(s->modems, modem);

	/* Free resources */
	tcore_free(modem->cp_name);
	tcore_free(modem);
}

gboolean tcore_server_update_modem_plugin(TcorePlugin *modem_iface_plugin,
	TcorePlugin *modem_plugin)
{
	Server *s;
	TcoreModem *modem;

	if (modem_iface_plugin == NULL) {
		err("Modem Interface Plug-in: [0x%x]", modem_iface_plugin);
		return FALSE;
	}

	s = tcore_plugin_ref_server(modem_iface_plugin);
	if (s == NULL) {
		err("server is NULL");
		return FALSE;
	}

	/* Find 'modem' */
	modem = __server_find_modem(s, modem_iface_plugin, NULL);
	if (modem == NULL) {
		err("Failed to find 'modem' for Modem Interface Plug-in: [%s]",
			tcore_plugin_ref_plugin_name(modem_iface_plugin));
		return FALSE;
	}

	/* Update Modem Plug-in */
	modem->modem_plugin = modem_plugin;
	dbg("Added to 'modems' pair - Modem Plug-in [%s] <---> Modem Interface Plug-in: [%s]",
		tcore_plugin_ref_plugin_name(modem_plugin),
		tcore_plugin_ref_plugin_name(modem_iface_plugin));

	return TRUE;
}

const gchar *tcore_server_get_cp_name_by_plugin(TcorePlugin *plugin)
{
	Server *s;
	TcoreModem *modem;

	if (plugin == NULL) {
		err("plugin is NULL");
		return NULL;
	}

	s = tcore_plugin_ref_server(plugin);
	if (s == NULL) {
		err("server is NULL");
		return NULL;
	}

	/* Find 'modem' */
	modem = __server_find_modem(s, plugin, plugin);
	if (modem == NULL) {
		err("Failed to find 'modem' for Plug-in: [%s]",
			tcore_plugin_ref_plugin_name(plugin));
		return NULL;
	}

	return (const gchar *)modem->cp_name;
}

gboolean tcore_server_add_cp_mapping_tbl_entry(TcorePlugin *modem_iface_plugin,
	guint co_type, TcoreHal *hal)
{
	Server *s;
	TcoreModem *modem;

	if (modem_iface_plugin == NULL) {
		err("Modem Interface is NULL");
		return FALSE;
	}

	s = tcore_plugin_ref_server(modem_iface_plugin);
	if (s == NULL) {
		err("server is NULL");
		return FALSE;
	}

	modem = __server_find_modem(s, modem_iface_plugin, NULL);
	if (modem == NULL) {
		err("Failed to find 'modem' for Modem Interface Plug-in: [%s]",
			tcore_plugin_ref_plugin_name(modem_iface_plugin));
		return FALSE;
	}

	/*
	 * Set the Mapping Table to the Modems list
	 */
	modem->mapping_tbl =
			tcore_object_add_mapping_tbl_entry(modem->mapping_tbl,
								co_type, hal);

	return TRUE;
}

void tcore_server_remove_cp_mapping_tbl(TcorePlugin *modem_iface_plugin)
{
	Server *s;
	TcoreModem *modem;

	if (modem_iface_plugin == NULL) {
		err("Modem Interface is NULL");
		return;
	}

	s = tcore_plugin_ref_server(modem_iface_plugin);
	if (s == NULL) {
		err("server is NULL");
		return;
	}

	modem = __server_find_modem(s, modem_iface_plugin, NULL);
	if (modem == NULL) {
		err("Failed to find 'modem' for Modem Interface Plug-in: [%s]",
			tcore_plugin_ref_plugin_name(modem_iface_plugin));
		return;
	}

	/* Removing the Mapping Table from the Modems list */
	tcore_object_remove_mapping_tbl(modem->mapping_tbl);
}

void tcore_server_remove_cp_mapping_tbl_entry(TcorePlugin *modem_iface_plugin,
	TcoreHal *hal)
{
	Server *s;
	TcoreModem *modem;

	if (modem_iface_plugin == NULL) {
		err("Modem Interface is NULL");
		return;
	}

	s = tcore_plugin_ref_server(modem_iface_plugin);
	if (s == NULL) {
		err("server is NULL");
		return;
	}

	modem = __server_find_modem(s, modem_iface_plugin, NULL);
	if (modem == NULL) {
		err("Failed to find 'modem' for Modem Interface Plug-in: [%s]",
			tcore_plugin_ref_plugin_name(modem_iface_plugin));
		return;
	}

	/* Removing the Mapping Table from the Modems list */
	modem->mapping_tbl =
		tcore_object_remove_mapping_tbl_entry(modem->mapping_tbl, hal);
}

void *tcore_server_get_cp_mapping_tbl(TcorePlugin *modem_plugin)
{
	Server *s;
	TcoreModem *modem;

	if (modem_plugin == NULL) {
		err("Modem Interface is NULL");
		return NULL;
	}

	s = tcore_plugin_ref_server(modem_plugin);
	if (s == NULL) {
		err("server is NULL");
		return NULL;
	}

	modem = __server_find_modem(s, NULL, modem_plugin);
	if (modem == NULL) {
		err("Failed to find 'modem' for Modem Plug-in: [%s]",
			tcore_plugin_ref_plugin_name(modem_plugin));
		return NULL;
	}

	return modem->mapping_tbl;
}

void tcore_server_print_modems(TcorePlugin *plugin)
{
	Server *s;
	TcoreModem *modem;

	if (plugin == NULL) {
		err("Modem Interface is NULL");
		return;
	}

	s = tcore_plugin_ref_server(plugin);
	if (s == NULL) {
		err("server is NULL");
		return;
	}

	modem = __server_find_modem(s, plugin, plugin);
	if (modem == NULL) {
		err("Failed to find 'modem' for Plug-in: [%s]",
			tcore_plugin_ref_plugin_name(plugin));
		return;
	}

	dbg("Modem Plug-in: [%s] Modem Interface Plug-in: [%s] - CP Name: [%s]",
		tcore_plugin_ref_plugin_name(modem->modem_plugin),
		tcore_plugin_ref_plugin_name(modem->modem_iface_plugin),
		modem->cp_name);

	tcore_object_print_mapping_tbl(modem->mapping_tbl);
}

TelReturn tcore_server_load_modem_plugin(Server *s,
	TcorePlugin *modem_if_plugin, const gchar *name)
{
	struct tcore_plugin_define_desc *desc;
	TcorePlugin *modem_plugin;
	gchar *filename;
	void *handle;
	TelReturn ret = TEL_RETURN_FAILURE;

	if ((s == NULL) || (name == NULL) || (modem_if_plugin == NULL)) {
		err("s: [%p] name: [%s] modem_if_plugin: [%p]", s, name, modem_if_plugin);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	filename = g_build_filename(MODEMS_PATH, name, NULL);

	/* Open .so */
	handle = dlopen(filename, RTLD_NOW);
	if (handle == NULL) {
		err("Failed to load '%s': %s", filename, dlerror());

		tcore_free(filename);
		return ret;
	}

	/* Symbol 'plugin_define_desc' search */
	desc = dlsym(handle, "plugin_define_desc");
	if (desc == NULL) {
		err("Failed to load symbol: %s", dlerror());

		/* Close handle */
		dlclose(handle);

		tcore_free(filename);
		return ret;
	}
	dbg("Plugin '%s' found", desc->name);

	if (desc->load != NULL) {
		/* Load Modem Plug-in */
		if (desc->load() == FALSE) {
			err("Failed to load '%s' plugin", desc->name);

			/* Close handle */
			dlclose(handle);

			tcore_free(filename);
			return ret;
		}
	}

	/* Create new Modem Plug-in and update to corresponding 'modem's structure */
	modem_plugin = tcore_plugin_new(s, desc, filename, handle);
	tcore_server_update_modem_plugin(modem_if_plugin, modem_plugin);
	dbg("Plugin %s loaded successfully", desc->name);

	if (desc->init == NULL) {
		err("Plugin '%s' has no initializer", desc->name);
		goto OUT;
	}

	/* Initialize Modem Plug-in */
	if (desc->init(modem_plugin) == FALSE) {
		err("Plugin %s initialization failed", desc->name);
		goto OUT;
	}
	dbg("Plugin '%s' successfully initialized", desc->name);

	/* Notify addition of Plug-in to Upper Layers */
	tcore_server_send_server_notification(s,
		TCORE_SERVER_NOTIFICATION_ADDED_MODEM_PLUGIN,
		0, modem_plugin);

	ret = TEL_RETURN_SUCCESS;

OUT:
	tcore_free(filename);

	if (ret != TEL_RETURN_SUCCESS) {
		/* Unload Modem Plug-in */
		if (desc->unload != NULL) {
			dbg("Unloading Modem Plug-in: [%s]",
				tcore_plugin_ref_plugin_name(modem_plugin));
			desc->unload(modem_plugin);
		}

		/* Free Modem Plug-in and update the 'modem's structure */
		tcore_server_update_modem_plugin(modem_if_plugin, NULL);
		tcore_plugin_free(modem_plugin);

		/* Close handle */
		dlclose(handle);
	}

	return ret;
}

void tcore_server_unload_modem_plugin(Server *s, TcorePlugin *modem_if_plugin)
{
	TcoreModem *modem;
	TcorePlugin *modem_plugin;
	const struct tcore_plugin_define_desc *desc;

	if ((s == NULL) || (modem_if_plugin == NULL)) {
		err("s: [%p] modem_if_plugin: [%p]", s, modem_if_plugin);
		return;
	}

	/* Find modem from Server's Modem's list */
	modem = __server_find_modem(s, modem_if_plugin, modem_if_plugin);
	if (modem == NULL) {
		err("Failed to find 'modem' for Plug-in: [%s]",
			tcore_plugin_ref_plugin_name(modem_if_plugin));
		return;
	}

	dbg("Modem Plug-in: [%s] Modem Interface Plug-in: [%s] - CP Name: [%s]",
		tcore_plugin_ref_plugin_name(modem->modem_plugin),
		tcore_plugin_ref_plugin_name(modem->modem_iface_plugin),
		modem->cp_name);

	/* Extract Modem Plug-in */
	modem_plugin = modem->modem_plugin;
	if (modem_plugin == NULL) {
		err("Modem Plug-in is NULL");
		return;
	}

	/* Notify deletion of Plug-in to Upper Layers */
	tcore_server_send_server_notification(s,
		TCORE_SERVER_NOTIFICATION_REMOVED_MODEM_PLUGIN,
		0, modem_plugin);

	/* Extract descriptor of Modem Plug-in */
	desc = tcore_plugin_get_description(modem_plugin);
	if (desc != NULL) {
		/* Unload Modem Plug-in */
		if (desc->unload != NULL) {
			dbg("Unloading Modem Plug-in: [%s]",
				tcore_plugin_ref_plugin_name(modem_plugin));
			desc->unload(modem_plugin);
		}
	}

	/* Free Modem Plug-in and update the 'modem's structure */
	tcore_server_update_modem_plugin(modem_if_plugin, NULL);
	tcore_plugin_free(modem_plugin);

	dbg("Unloaded Modem Plug-in");
}

GSList *tcore_server_get_modem_plugin_list(Server *s)
{
	GSList *list;
	GSList *modem_plugin_list = NULL;
	TcoreModem *modem;

	for (list = s->modems; list; list = list->next) {
		modem = list->data;
		if (modem != NULL)
			if (NULL != modem->modem_plugin)
				modem_plugin_list =
					g_slist_append(modem_plugin_list, modem->modem_plugin);
	}

	return modem_plugin_list;
}
