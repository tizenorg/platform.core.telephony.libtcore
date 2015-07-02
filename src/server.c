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
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <dlfcn.h>

#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "hal.h"
#include "server.h"
#include "manager.h"
#include "user_request.h"
#include "core_object.h"
#include "co_ps.h"
#include "communicator.h"
#include "storage.h"
#include "util.h"

#define MODEMS_PATH "/usr/lib/telephony/plugins/modems/"

struct tcore_server_type {
	GMainLoop *mainloop;

	Manager *manager;

	GSList *plugins;
	GSList *communicators;
	GSList *storages;
	GSList *hals;
	GSList *modems;

	GSList *hook_list_request;
	GSList *hook_list_notification;
	TcorePlugin *default_plugin;
};

struct tcore_modem_type {
	char *cp_name;
	TcorePlugin *modem_iface_plugin;

	TcorePlugin *modem_plugin;

	void *mapping_tbl;
};

struct hook_request_type {
	enum tcore_request_command command;
	TcoreServerRequestHook func;
	void *user_data;
};

struct hook_notification_type {
	enum tcore_notification_command command;
	TcoreServerNotificationHook func;
	void *user_data;
};

static gint _compare_priority(gconstpointer a, gconstpointer b)
{
	TcorePlugin *plugin1 = (TcorePlugin *)a;
	TcorePlugin *plugin2 = (TcorePlugin *)b;

	if (!plugin2)
		return 1;

	return tcore_plugin_get_description(plugin1)->priority -
			tcore_plugin_get_description(plugin2)->priority;
}

static char *_server_enumerate_modem(TcorePlugin *plugin)
{
	static unsigned int cp_counter = 0;
	const char *filename;

	if (plugin == NULL)
		return NULL;

	/*
	 * Presently enumeration is based on Modem Interface Plug-in descriptor name
	 * followed by an incremental Positive integer 'cp_count'.
	 *
	 * For example, if Modem Interface Plug-in descriptor name is 'qmimodem' then,
	 *	'name' would be enumerated as "qmimodem_N", where N >= 0
	 */
	filename = tcore_plugin_ref_plugin_name(plugin);
	if (filename == NULL)
		return NULL;

	dbg("%s", filename);

	return g_strdup_printf("%s%d", filename, cp_counter++);
}

static TcoreModem *__get_modem(TcorePlugin *modem_plugin)
{
	GSList *list;
	TcoreModem *modem;
	Server *s;

	s = tcore_plugin_ref_server(modem_plugin);
	if (s == NULL) {
		err("server is NULL");
		return NULL;
	}

	for (list = s->modems; list; list = list->next) {
		modem = list->data;
		if (modem == NULL) {
			dbg("Modem is NULL");
			continue;
		}

		if (modem->modem_plugin == modem_plugin)
			return modem;
	}

	err("Modem not found");

	return NULL;
}

static TcorePlugin *_find_default_plugin(Server *s)
{
	GSList *list;
	TcoreModem *modem;

	if (s->default_plugin != NULL)
		return s->default_plugin;

	for (list = s->modems; list; list = list->next) {
		modem = list->data;
		if (modem == NULL)
			continue;

		s->default_plugin = modem->modem_plugin;

		return modem->modem_plugin;
	}

	return NULL;
}

Server *tcore_server_new()
{
	Server *s;

	s = calloc(1, sizeof(struct tcore_server_type));
	if (!s) {
		err("Server allocation failed!!!");
		return NULL;
	}

	s->mainloop = g_main_loop_new(NULL, FALSE);
	if (!s->mainloop) {
		err("mainloop creation failed!!!");
		free(s);
		return NULL;
	}

	s->plugins = NULL;
	s->communicators = NULL;
	s->storages = NULL;
	s->hals = NULL;
	s->hook_list_request = NULL;
	s->hook_list_notification = NULL;
	s->default_plugin = NULL;

	return s;
}

void tcore_server_free(Server *s)
{
	GSList *list = NULL;
	TcorePlugin *p = NULL;
	struct tcore_plugin_define_desc *desc = NULL;

	if (!s) {
		err("Server is NULL");
		return;
	}
	s->plugins = g_slist_reverse(s->plugins);

	/* Unload all plug-ins */
	for (list = s->plugins; list; list = list->next) {
		p = list->data;
		if (!p)
			continue;

		desc = (struct tcore_plugin_define_desc *)tcore_plugin_get_description(p);
		if (!desc || !desc->unload)
			continue;

		desc->unload(p);

		tcore_plugin_free(p);

		list->data = NULL;
	}

	/* Free plug-ins */
	if (s->plugins) {
		g_slist_free(s->plugins);
		s->plugins = NULL;
	}

	/* Unref 'mainloop' */
	if (s->mainloop)
		g_main_loop_unref(s->mainloop);

	/* Free server */
	free(s);
	dbg("Freeing Server");
}

TReturn tcore_server_run(Server *s)
{
	char *version;

	if (!s || !s->mainloop)
		return TCORE_RETURN_EINVAL;

	version = tcore_util_get_version();
	if (version) {
		dbg("libtcore version: %s", version);
		free(version);
	}

	tcore_server_send_notification(s, NULL, TNOTI_SERVER_RUN, 0, NULL);

	g_main_loop_run(s->mainloop);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_server_exit(Server *s)
{
	if (!s || !s->mainloop)
		return TCORE_RETURN_EINVAL;

	g_main_loop_quit(s->mainloop);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_server_set_manager(Server *s, Manager *manager)
{
	if (!s)
		return TCORE_RETURN_EINVAL;

	s->manager = manager;

	return TCORE_RETURN_SUCCESS;
}

Manager *tcore_server_ref_manager(Server *s)
{
	if (!s)
		return NULL;

	return s->manager;
}

TReturn tcore_server_add_plugin(Server *s, TcorePlugin *plugin)
{
	if (!s || !plugin)
		return TCORE_RETURN_EINVAL;

	s->plugins = g_slist_insert_sorted(s->plugins, plugin, _compare_priority);

	tcore_server_send_notification(s, NULL, TNOTI_SERVER_ADDED_PLUGIN, 0, NULL);

	return TCORE_RETURN_SUCCESS;
}

TcorePlugin *tcore_server_find_plugin(Server *s, const char *name)
{
	GSList *list;
	TcoreModem *modem;

	dbg("Name: [%s]", name);
	if (g_strcmp0(TCORE_PLUGIN_DEFAULT, name) == 0)
		return _find_default_plugin(s);

	for (list = s->modems; list; list = list->next) {
		modem = list->data;
		if (modem == NULL)
			continue;

		if (g_strcmp0(modem->cp_name, name) == 0)
			return modem->modem_plugin;
	}

	err("Modem plugin not found");

	return NULL;
}

GSList *tcore_server_ref_plugins(Server *s)
{
	if (!s)
		return NULL;

	return s->plugins;
}

TReturn tcore_server_add_communicator(Server *s, Communicator *comm)
{
	if (!s || !comm)
		return TCORE_RETURN_EINVAL;

	s->communicators = g_slist_insert(s->communicators, comm, 0);

	tcore_server_send_notification(s, NULL, TNOTI_SERVER_ADDED_COMMUNICATOR, 0, NULL);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_server_remove_communicator(Server *s, Communicator *comm)
{
	if (!s || !comm)
		return TCORE_RETURN_EINVAL;

	s->communicators = g_slist_remove(s->communicators, comm);

	return TCORE_RETURN_SUCCESS;
}


GSList *tcore_server_ref_communicators(Server *s)
{
	if (!s)
		return NULL;

	return s->communicators;
}

Communicator *tcore_server_find_communicator(Server *s, const char *name)
{
	GSList *list;
	Communicator *comm;

	for (list = s->communicators; list; list = list->next) {
		comm = list->data;
		if (!comm)
			continue;

		if (g_strcmp0(tcore_communicator_ref_name(comm), name) == 0)
			return comm;
	}

	return NULL;
}

TReturn tcore_server_add_storage(Server *s, Storage *strg)
{
	if (!s || !strg)
		return TCORE_RETURN_EINVAL;

	s->storages = g_slist_insert(s->storages, strg, 0);

	tcore_server_send_notification(s, NULL, TNOTI_SERVER_ADDED_STORAGE, 0, NULL);

	return TCORE_RETURN_SUCCESS;
}

GSList *tcore_server_ref_storages(Server *s)
{
	if (!s)
		return NULL;

	return s->storages;
}

Storage *tcore_server_find_storage(Server *s, const char *name)
{
	GSList *list;
	Storage *strg;

	for (list = s->storages; list; list = list->next) {
		strg = list->data;
		if (!strg)
			continue;

		if (g_strcmp0(tcore_storage_ref_name(strg), name) == 0)
			return strg;
	}

	return NULL;
}

TReturn tcore_server_add_hal(Server *s, TcoreHal *hal)
{
	GSList *list;
	TcoreHal *temp;

	if (!s || !hal)
		return TCORE_RETURN_EINVAL;

	for (list = s->hals; list; list = list->next) {
		temp = list->data;
		if (!temp)
			continue;

		if (temp == hal)
			return TCORE_RETURN_EALREADY;
	}

	s->hals = g_slist_insert(s->hals, hal, 0);

	tcore_server_send_notification(s, NULL, TNOTI_SERVER_ADDED_HAL, 0, NULL);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_server_remove_hal(Server *s, TcoreHal *hal)
{
	if (!s || !hal)
		return TCORE_RETURN_EINVAL;

	/* Remove HAL from list */
	s->hals = g_slist_remove(s->hals, hal);

	/* Send Notification */
	tcore_server_send_notification(s, NULL, TNOTI_SERVER_REMOVED_HAL, 0, NULL);

	return TCORE_RETURN_SUCCESS;
}

GSList *tcore_server_ref_hals(Server *s)
{
	if (!s)
		return NULL;

	return s->hals;
}

TcoreHal *tcore_server_find_hal(Server *s, const char *name)
{
	GSList *list;
	TcoreHal *hal;
	char *buf;

	for (list = s->hals; list; list = list->next) {
		hal = list->data;
		if (!hal)
			continue;

		buf = tcore_hal_get_name(hal);
		if (!buf)
			continue;

		if (g_strcmp0(buf, name) == 0) {
			free(buf);
			return hal;
		}

		free(buf);
	}

	return NULL;
}

TReturn tcore_server_dispatch_request(Server *s, UserRequest *ur)
{
	char *modem_name = NULL;
	TcorePlugin *p;
	enum tcore_request_command command = 0;
	GSList *list, *co_list = NULL;
	struct hook_request_type *hook;
	int category;
	CoreObject *o;
	TReturn ret = TCORE_RETURN_ENOSYS;
	TReturn prev_ret = TCORE_RETURN_FAILURE;
	enum tcore_ops_type ops_type = TCORE_OPS_TYPE_CP;

	if (!s || !ur)
		return TCORE_RETURN_EINVAL;

	for (list = s->hook_list_request; list; list = list->next) {
		hook = list->data;
		if (!hook)
			continue;

		if (hook->command == tcore_user_request_get_command(ur)) {
			if (hook->func) {
				enum tcore_hook_return h_ret;

				h_ret = hook->func(s, ur, hook->user_data);
				if (h_ret == TCORE_HOOK_RETURN_STOP_PROPAGATION)
					return TCORE_RETURN_SUCCESS;
			}
		}
	}

	/* In case Manager is available, process Request in Manager */
	if (s->manager) {
		enum tcore_manager_return mgr_ret;

		mgr_ret = tcore_manager_dispatch_request(s->manager, ur);

		if (mgr_ret == TCORE_MANAGER_RETURN_FAILURE)
			return TCORE_RETURN_FAILURE;
		else if (mgr_ret == TCORE_MANAGER_RETURN_STOP)
			return TCORE_RETURN_SUCCESS;
		else if (mgr_ret == TCORE_MANAGER_RETURN_CONTINUE_IMS)
			ops_type = TCORE_OPS_TYPE_IMS;
		else
			ops_type = TCORE_OPS_TYPE_CP;
	}

	modem_name = tcore_user_request_get_modem_name(ur);
	if (!modem_name)
		return TCORE_RETURN_EINVAL;

	p = tcore_server_find_plugin(s, (const char *)modem_name);
	if (!p) {
		free(modem_name);
		return TCORE_RETURN_SERVER_WRONG_PLUGIN;
	}
	free(modem_name);

	command = tcore_user_request_get_command(ur);

	category = CORE_OBJECT_TYPE_DEFAULT | (command & 0x0FF00000);

	co_list = tcore_plugin_get_core_objects_bytype(p, category);
	if (!co_list) {
		warn("can't find 0x%x core_object. co_list is null.", category);
		return TCORE_RETURN_ENOSYS;
	}

	for (list = co_list; list; list = list->next) {
		o = (CoreObject *) list->data;
		if (!o) {
			warn("can't find 0x%x core_object", category);
			continue;
		}

		/*
		 * SUCCESS would be returned if even one of the Core Object
		 * in the list 'co_list' Successfully processes the request.
		 *
		 * The concept is to consider that the Request is being
		 * processed atleast by 1 entity.
		 */
		ret = tcore_object_dispatch_request_with_type(o, ur, ops_type);
		if (ret != TCORE_RETURN_SUCCESS)
			dbg("failed... ret=[%d]", ret);
		else
			prev_ret = ret;

		ret = prev_ret;
	}

	g_slist_free(co_list);
	return ret;
}

TReturn tcore_server_send_notification(Server *s, CoreObject *source,
		enum tcore_notification_command command,
		unsigned int data_len, void *data)
{
	GSList *list;
	Communicator *comm;
	struct hook_notification_type *hook;
	if (!s)
		return TCORE_RETURN_EINVAL;

	/* In case Manager is available, process Notification in Manager */
	if (s->manager) {
		enum tcore_manager_return mgr_ret;

		/* Send notification to 'manager' */
		mgr_ret = tcore_manager_send_notification(s->manager,
			source, command, data_len, data);

		if (mgr_ret == TCORE_MANAGER_RETURN_FAILURE)
			return TCORE_RETURN_FAILURE;
		else if (mgr_ret == TCORE_MANAGER_RETURN_STOP)
			return TCORE_RETURN_SUCCESS;
		/* in other cases, send notification to communicator. */
	}

	for (list = s->hook_list_notification; list;) {
		hook = list->data;
		list = list->next;
		if (!hook)
			continue;

		if (hook->command == command)
			if (hook->func(s, source,
					command, data_len, data,
					hook->user_data) == TCORE_HOOK_RETURN_STOP_PROPAGATION)
				return TCORE_RETURN_SUCCESS;
	}

	for (list = s->communicators; list;) {
		comm = list->data;
		list = list->next;
		if (!comm)
			continue;

		tcore_communicator_send_notification(comm, source, command, data_len, data);
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_server_add_request_hook(Server *s,
	enum tcore_request_command command,
	TcoreServerRequestHook func, void *user_data)
{
	struct hook_request_type *hook;

	if (!s || !func)
		return TCORE_RETURN_EINVAL;

	hook = calloc(1, sizeof(struct hook_request_type));
	if (!hook)
		return TCORE_RETURN_ENOMEM;

	hook->command = command;
	hook->func = func;
	hook->user_data = user_data;

	s->hook_list_request = g_slist_append(s->hook_list_request, hook);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_server_remove_request_hook(Server *s, TcoreServerRequestHook func)
{
	struct hook_request_type *hook;
	GSList *list;

	if (!s)
		return TCORE_RETURN_EINVAL;

	for (list = s->hook_list_request; list; list = list->next) {
		hook = list->data;
		if (!hook)
			continue;

		if (hook->func == func) {
			s->hook_list_request = g_slist_remove(s->hook_list_request, hook);
			free(hook);
			list = s->hook_list_request;
		}
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_server_add_notification_hook(Server *s,
	enum tcore_notification_command command,
	TcoreServerNotificationHook func, void *user_data)
{
	struct hook_notification_type *hook;

	if (!s || !func)
		return TCORE_RETURN_EINVAL;

	hook = calloc(1, sizeof(struct hook_notification_type));
	if (!hook)
		return TCORE_RETURN_ENOMEM;

	hook->command = command;
	hook->func = func;
	hook->user_data = user_data;

	s->hook_list_notification = g_slist_append(s->hook_list_notification, hook);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_server_remove_notification_hook(Server *s,
	TcoreServerNotificationHook func)
{
	struct hook_notification_type *hook;
	GSList *list;

	if (!s)
		return TCORE_RETURN_EINVAL;

	for (list = s->hook_list_notification; list; list = list->next) {
		hook = list->data;
		if (!hook)
			continue;

		if (hook->func == func) {
			s->hook_list_notification = g_slist_remove(s->hook_list_notification, hook);
			free(hook);
			list = s->hook_list_notification;
		}
	}

	return TCORE_RETURN_SUCCESS;
}

TcoreModem *tcore_server_register_modem(Server *s, TcorePlugin *modem_iface_plugin)
{
	TcoreModem *modem;

	if ((s == NULL) || (modem_iface_plugin == NULL)) {
		err("Server [%p] Modem Interface Plug-in: [%p]", s, modem_iface_plugin);
		return NULL;
	}

	/* Allocate 'modem' */
	modem = g_try_new0(TcoreModem, 1);
	if (modem == NULL) {
		err("Failed to allocate memory");
		return NULL;
	}

	/* Enumerate CP Name to 'modem' */
	modem->cp_name = _server_enumerate_modem(modem_iface_plugin);

	/* Update Modem Interface Plug-in to 'modem' */
	modem->modem_iface_plugin = modem_iface_plugin;

	/* Add 'modem' to 'modems' list */
	s->modems = g_slist_append(s->modems, modem);
	dbg("Added to 'modems' entry - CP Name: [%s] Modem Interface Plug-in: [%s]",
		modem->cp_name, tcore_plugin_ref_plugin_name(modem_iface_plugin));

	return modem;
}

void tcore_server_unregister_modem(Server *s, TcoreModem *modem)
{
	if ((s == NULL) || (modem == NULL)) {
		err("server [%p] Modem: [%p]", s, modem);
		return;
	}

	/* Remove 'modem' from 'modems' list */
	s->modems = g_slist_remove(s->modems, modem);
	dbg("Removed from 'modems' entry - CP Name: [%s] Modem Interface Plug-in: [%s]",
		modem->cp_name, tcore_plugin_ref_plugin_name(modem->modem_iface_plugin));

	/* Free memory */
	g_free(modem->cp_name);
	g_free(modem);
}

GSList *tcore_server_get_cp_name_list(Server *s)
{
	GSList *cp_name_list = NULL;
	GSList *list;
	TcoreModem *modem;

	int i = 0;

	if (s == NULL) {
		err("Server is NULL");
		return NULL;
	}

	for (list = s->modems; list; list = list->next) {
		modem = list->data;
		if (modem == NULL) {
			dbg("No modem - continue");
			continue;
		}

		/* Add CP name to 'cp_name_list' */
		dbg("[%d] CP Name: [%s]", i++, modem->cp_name);
		cp_name_list = g_slist_append(cp_name_list, g_strdup(modem->cp_name));
	}

	/* 'cp_name_list' would be freed by the calling function */
	return cp_name_list;
}

const char *tcore_server_get_cp_name_by_plugin(TcorePlugin *modem_plugin)
{
	TcoreModem *modem;

	if (modem_plugin == NULL) {
		err("Modem Plug-in is NULL");
		return NULL;
	}

	modem = __get_modem(modem_plugin);
	if (modem == NULL) {
		err("Failed to find 'modem' for Plug-in: [%s]",
			tcore_plugin_ref_plugin_name(modem_plugin));

		return NULL;
	}

	return (const char *)modem->cp_name;
}

gboolean tcore_server_add_cp_mapping_tbl_entry(TcoreModem *modem,
	unsigned int co_type, TcoreHal *hal)
{
	if (modem == NULL) {
		err("Modem is NULL");
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

void tcore_server_remove_cp_mapping_tbl(TcoreModem *modem)
{
	if (modem == NULL) {
		err("Modem is NULL");
		return;
	}

	/* Removing the Mapping Table from the Modems list */
	tcore_object_remove_mapping_tbl(modem->mapping_tbl);
}

void tcore_server_remove_cp_mapping_tbl_entry(TcoreModem *modem,
	TcoreHal *hal)
{
	if (modem == NULL) {
		err("Modem is NULL");
		return;
	}

	/* Removing the Mapping Table Entry from the Modems list */
	modem->mapping_tbl =
		tcore_object_remove_mapping_tbl_entry(modem->mapping_tbl, hal);
}

void *tcore_server_get_cp_mapping_tbl(TcorePlugin *modem_plugin)
{
	TcoreModem *modem;

	if (modem_plugin == NULL) {
		err("Modem Interface is NULL");
		return NULL;
	}

	modem = __get_modem(modem_plugin);
	if (modem == NULL) {
		err("Failed to find 'modem' for Modem Plug-in: [%s]",
			tcore_plugin_ref_plugin_name(modem_plugin));
		return NULL;
	}

	return modem->mapping_tbl;
}

void tcore_server_print_modems(Server *s)
{
	TcoreModem *modem;
	GSList *list;

	if (s == NULL) {
		err("Server is NULL");
		return;
	}

	for (list = s->modems ; list ; list = list->next) {
		modem = list->data;
		if (modem == NULL) {
			dbg("Modem is NULL");
			continue;
		}

		msg("Modem: [0x%x] CP Name: [%s]", modem, modem->cp_name);
		msg("Modem Plug-in: [%s] <---> Modem Interface Plug-in: [%s]",
			tcore_plugin_ref_plugin_name(modem->modem_plugin),
			tcore_plugin_ref_plugin_name(modem->modem_iface_plugin));
	}
}

TReturn tcore_server_load_modem_plugin(Server *s,
	TcoreModem *modem, const char *name)
{
	struct tcore_plugin_define_desc *desc;
	TcorePlugin *modem_plugin;
	char *filename = NULL;
	void *handle;
	TReturn ret = TCORE_RETURN_FAILURE;

	if ((s == NULL) || (modem == NULL) || (name == NULL)) {
		err("Server: [%p] modem: [%p] Plug-in Name (.so): [%s] ",
			s, modem, name);
		ret = TCORE_RETURN_EINVAL;
		goto out;
	}

	filename = g_build_filename(MODEMS_PATH, name, NULL);

	/* Open '.so' */
	handle = dlopen(filename, RTLD_LAZY);
	if (handle == NULL) {
		err("Failed to load '%s': %s", filename, dlerror());
		goto out;
	}

	desc = dlsym(handle, "plugin_define_desc");
	if (desc == NULL) {
		err("Failed to obtain the address of plugin_define_desc: %s", dlerror());
		dlclose(handle);
		goto out;
	}

	dbg("Plugin %s found", desc->name);

	if (desc->load != NULL) {
		/* Load Modem Plug-in */
		if (desc->load() == FALSE) {
			err("Failed to load %s plugin", desc->name);
			dlclose(handle);
			goto out;
		}
	}

	/* Create new Plug-in */
	modem_plugin = tcore_plugin_new(s, desc, filename, handle);
	if (modem_plugin == NULL) {
		err("Modem Plug-in is NULL");
		dlclose(handle);
		goto out;
	}

	/* Update Modem Plug-in to 'modem' */
	modem->modem_plugin = modem_plugin;

	dbg("Plugin %s loaded successfully", desc->name);

	if (desc->init == NULL) {
		err("Plugin %s has no initializer", desc->name);
		dlclose(handle);
		goto out;
	}

	/* Initialize Modem Plug-in */
	if (desc->init(modem_plugin) == FALSE) {
		err("Plugin %s initialization failed", desc->name);
		dlclose(handle);
		goto out;
	}

	dbg("Plugin %s initialization success", desc->name);

	/* Notify addition of Plug-in to Upper Layers */
	tcore_server_send_notification(s, NULL,
			TNOTI_SERVER_ADDED_MODEM_PLUGIN,
			0, modem_plugin);

	ret = TCORE_RETURN_SUCCESS;

out:
	g_free(filename);

	return ret;
}

void tcore_server_unload_modem_plugin(Server *s, TcoreModem *modem)
{
	TcorePlugin *modem_plugin;
	const struct tcore_plugin_define_desc *desc;

	if ((s == NULL) || (modem == NULL)) {
		err("Server: [%p] modem: [%p]", s, modem);
		return;
	}

	/* Extract Modem Plug-in */
	modem_plugin = modem->modem_plugin;
	if (modem_plugin == NULL) {
		err("Modem Plug-in is NULL");
		return;
	}

	msg("Modem Plug-in: [%s] <---> Modem Interface Plug-in: [%s] - CP Name: [%s]",
		tcore_plugin_ref_plugin_name(modem->modem_plugin),
		tcore_plugin_ref_plugin_name(modem->modem_iface_plugin), modem->cp_name);

	/* Notify removal of Plug-in to Upper Layers */
	tcore_server_send_notification(s, NULL,
		TNOTI_SERVER_REMOVED_MODEM_PLUGIN,
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

	/* Free Modem Plug-in */
	tcore_plugin_free(modem_plugin);
	modem->modem_plugin = NULL;

	dbg("Unloaded Modem Plug-in");
}

GSList *tcore_server_get_modem_plugin_list(Server *s)
{
	GSList *list;
	GSList *modem_plugin_list = NULL;
	TcoreModem *modem;

	for (list = s->modems; list; list = list->next) {
		modem = list->data;
		if (modem == NULL) {
			dbg("Modem is NULL");
			continue;
		}

		if (NULL != modem->modem_plugin)
			modem_plugin_list = g_slist_append(modem_plugin_list, modem->modem_plugin);
	}

	return modem_plugin_list;
}

gint tcore_server_get_modems_count(Server *s)
{
	return g_slist_length(s->modems);
}
