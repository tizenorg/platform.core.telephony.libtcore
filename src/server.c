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
#include <glib/gprintf.h>

#include "tcore.h"
#include "plugin.h"
#include "server.h"
#include "user_request.h"
#include "core_object.h"
#include "co_ps.h"
#include "communicator.h"
#include "storage.h"
#include "udev.h"
#include "util.h"

struct tcore_server_type {
	GMainLoop *mainloop;
	GSList *plugins;
	GSList *communicators;
	GSList *storages;
	GSList *modems;

	GSList *template_co;

	GSList *hook_list_request;
	GSList *hook_list_notification;

	TcorePlugin *default_plugin;
	TcoreUdev *udev;
};

struct tcore_modem_type {
	char *cp_name;
	TcorePlugin *modem_iface_plugin;

	TcorePlugin *modem_plugin;

	void *mapping_tbl;
};

struct hook_request_type {
	enum tcore_request_command command;
	tcore_server_request_hook func;
	void *user_data;
};

struct hook_notification_type {
	enum tcore_notification_command command;
	tcore_server_notification_hook func;
	void *user_data;
};

static gint _compare_priority(gconstpointer a, gconstpointer b)
{
	TcorePlugin *plugin1 = (TcorePlugin *)a;
	TcorePlugin *plugin2 = (TcorePlugin *)b;

	if (plugin2 == NULL)
		return 1;

	return tcore_plugin_get_description(plugin1)->priority -
			tcore_plugin_get_description(plugin2)->priority;
}

static TcorePlugin *_find_default_plugin(Server *s)
{
	GSList *list;
	TcorePlugin *p;
	GSList *co_list;

	if (s->default_plugin != NULL) {
		return s->default_plugin;
	}

	for (list = s->plugins; list; list = list->next) {
		p = list->data;
		if (p == NULL)
			continue;

		co_list = tcore_plugin_get_core_objects_bytype(p, CORE_OBJECT_TYPE_MODEM);
		if (co_list == NULL)
			continue;

		g_slist_free(co_list);
		s->default_plugin = p;
		return p;
	}

	return NULL;
}

static char *_server_enumerate_modem(TcorePlugin *plugin)
{
	static unsigned int cp_counter = 0;
	char *cp_name;
	char *filename;
	char *name;

	if (plugin == NULL)
		return NULL;

	/*
	 * Presently enumeration is based on Modem Interface Plug-in descriptor name
	 * followed by an incremental Positive integer 'cp_count'.
	 *
	 * For example, if Modem Interface Plug-in descriptor name is 'imcmodem' then,
	 *	'name' would be enumerated as "imcmode_N", where N >= 0
	 */
	filename = tcore_plugin_get_filename(plugin);
	if (filename == NULL)
		return NULL;

	/* Stripping '.so' from 'filename' */
	name = strtok(filename, ".so");

	cp_name = g_strdup_printf("%s_%d", name, cp_counter++);
	g_free(filename);

	return cp_name;
}

static TcoreModem *_server_find_modem(Server *s,
				TcorePlugin *modem_iface_plugin, TcorePlugin *modem_plugin)
{
	GSList *list;
	TcoreModem *modem;

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
			&& ((modem_plugin == NULL) && (modem->modem_plugin == NULL)))
			return modem;

		/*
		 * Specifically for get Mapping Table -
		 *
		 * Modem Interface Plug-in MUST be NULL
		 *			AND
		 * Passed Modem Plug-in MUST match Modem Plug-of 'modems'
		 */
		if ((modem_iface_plugin == NULL)
			&& (modem_plugin == modem->modem_plugin))
			return modem;

		/*
		 * Specifically for get CP name -
		 *
		 * Passed Modem OR Modem Interface Plug-in MUST match
		 *			AND
		 * MUST match either Modem OR Modem Interface Plug-in of 'modems'
		 */
		if ((modem_iface_plugin == modem_plugin)
				&& ((modem_iface_plugin == modem->modem_iface_plugin)
					|| (modem_plugin == modem->modem_plugin)))
			return modem;
	}

	return NULL;
}
Server *tcore_server_new()
{
	Server *s;

	s = g_try_new0(struct tcore_server_type, 1);
	if (s == NULL)
		return NULL;

	s->mainloop = g_main_loop_new (NULL, FALSE);
	if (s->mainloop == NULL) {
		free(s);
		return NULL;
	}

	s->plugins = NULL;
	s->communicators = NULL;
	s->storages = NULL;
	s->template_co = NULL;
	s->hook_list_request = NULL;
	s->hook_list_notification = NULL;
	s->default_plugin = NULL;

	return s;
}

void tcore_server_free(Server *s)
{
	GSList *list;
	TcorePlugin *p;
	const struct tcore_plugin_define_desc *desc;

	if (s == NULL)
		return;

	if (s->mainloop)
		g_main_loop_unref(s->mainloop);

    for (list = s->plugins; list; list = list->next) {
        p = list->data;
        if (p == NULL)
            continue;

        desc = (struct tcore_plugin_define_desc *)tcore_plugin_get_description(p);
		if ((desc == NULL) || (desc->unload == NULL))
			continue;

		desc->unload(p);

		tcore_plugin_free(p);

		list->data = NULL;
	}

	if (s->hook_list_notification)
		g_slist_free_full(s->hook_list_notification, g_free);

	if (s->hook_list_request)
		g_slist_free_full(s->hook_list_request, g_free);

	if (s->template_co)
		g_slist_free(s->template_co);

	if (s->storages)
		g_slist_free(s->storages);

	if (s->communicators)
		g_slist_free(s->communicators);

	if (s->plugins) {
		g_slist_free(s->plugins);
		s->plugins = NULL;
	}

	/* Freeing Server */
	g_free(s);
}

TReturn tcore_server_run(Server *s)
{
	char *version;

	if ((s == NULL)|| (s->mainloop == NULL))
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
	if ((s == NULL)|| (s->mainloop == NULL))
		return TCORE_RETURN_EINVAL;

	g_main_loop_quit(s->mainloop);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_server_add_plugin(Server *s, TcorePlugin *plugin)
{
	if ((s == NULL)|| (plugin == NULL))
		return TCORE_RETURN_EINVAL;

	s->plugins = g_slist_insert_sorted(s->plugins, plugin, _compare_priority);

	tcore_server_send_notification(s, NULL, TNOTI_SERVER_ADDED_PLUGIN, 0, NULL);

	return TCORE_RETURN_SUCCESS;
}

TcorePlugin *tcore_server_find_plugin(Server *s, const char *name)
{
	GSList *list;
	TcorePlugin *p;

	if (g_strcmp0(name, TCORE_PLUGIN_DEFAULT) == 0) {
		return _find_default_plugin(s);
	}

	for (list = s->plugins; list; list = list->next) {
		p = list->data;
		if (p == NULL) {
			continue;
		}

		if (g_strcmp0(tcore_plugin_get_description(p)->name, name) == 0) {
			return p;
		}
	}

	return NULL;
}

GSList *tcore_server_ref_plugins(Server *s)
{
	if (s == NULL)
		return NULL;

	return s->plugins;
}

TReturn tcore_server_add_communicator(Server *s, Communicator *comm)
{
	if ((s == NULL)|| (comm == NULL))
		return TCORE_RETURN_EINVAL;

	s->communicators = g_slist_insert(s->communicators, comm, 0);

	tcore_server_send_notification(s, NULL, TNOTI_SERVER_ADDED_COMMUNICATOR, 0, NULL);

	return TCORE_RETURN_SUCCESS;
}

GSList *tcore_server_ref_communicators(Server *s)
{
	if (s == NULL)
		return NULL;

	return s->communicators;
}

Communicator *tcore_server_find_communicator(Server *s, const char *name)
{
	GSList *list;
	Communicator *comm;

	for (list = s->communicators; list; list = list->next) {
		comm = list->data;
		if (comm == NULL) {
			continue;
		}

		if (g_strcmp0(tcore_communicator_ref_name(comm), name) == 0) {
			return comm;
		}
	}

	return NULL;
}

TReturn tcore_server_add_storage(Server *s, Storage *strg)
{
	if ((s == NULL)|| (strg == NULL))
		return TCORE_RETURN_EINVAL;

	s->storages = g_slist_insert(s->storages, strg, 0);

	tcore_server_send_notification(s, NULL, TNOTI_SERVER_ADDED_STORAGE, 0, NULL);

	return TCORE_RETURN_SUCCESS;
}

GSList *tcore_server_ref_storages(Server *s)
{
	if (s == NULL)
		return NULL;

	return s->storages;
}

Storage *tcore_server_find_storage(Server *s, const char *name)
{
	GSList *list;
	Storage *strg;

	for (list = s->storages; list; list = list->next) {
		strg = list->data;
		if (strg == NULL) {
			continue;
		}

		if (g_strcmp0(tcore_storage_ref_name(strg), name) == 0) {
			return strg;
		}
	}

	return NULL;
}

TReturn tcore_server_add_template_object(Server *s, CoreObject *template_co)
{
	GSList *list;
	CoreObject *temp;

	if ((s == NULL)|| (template_co == NULL))
		return TCORE_RETURN_EINVAL;

	for (list = s->template_co; list; list = list->next) {
		temp = list->data;
		if (temp == NULL) {
			continue;
		}

		if (tcore_object_get_type(temp) == tcore_object_get_type(template_co)) {
			return TCORE_RETURN_EALREADY;
		}
	}

	s->template_co = g_slist_insert(s->template_co, template_co, 0);

	return TCORE_RETURN_SUCCESS;
}

GSList *tcore_server_ref_template_object(Server *s)
{
	if (s == NULL)
		return NULL;

	return s->template_co;
}

CoreObject *tcore_server_find_template_object(Server *s, unsigned int type)
{
	GSList *list;
	CoreObject *template_co;

	for (list = s->template_co; list; list = list->next) {
		template_co = list->data;
		if (template_co == NULL)
			continue;

		if (type == tcore_object_get_type(template_co))
			return template_co;
	}

	return NULL;
}

TReturn tcore_server_link_udev(Server *s, TcoreUdev *udev)
{
	if ((s == NULL)|| (udev == NULL))
		return TCORE_RETURN_EINVAL;

	s->udev = udev;

	return TCORE_RETURN_SUCCESS;
}

TcoreUdev *tcore_server_ref_udev(Server *s)
{
	if (s == NULL)
		return NULL;

	return s->udev;
}

TReturn tcore_server_dispatch_request(Server *s, UserRequest *ur)
{
	char *modem = NULL;
	TcorePlugin *p;
	enum tcore_request_command command = 0;
	GSList *list, *co_list=NULL;
	struct hook_request_type *hook;
	int category;
	CoreObject *co;
	TReturn ret = TCORE_RETURN_ENOSYS;

	if ((s == NULL)|| (ur == NULL))
		return TCORE_RETURN_EINVAL;

	for (list = s->hook_list_request; list; list = list->next) {
		hook = list->data;
		if (hook == NULL)
			continue;

		if (hook->command == tcore_user_request_get_command(ur))
			if (hook->func(s, ur, hook->user_data)
					== TCORE_HOOK_RETURN_STOP_PROPAGATION)
				return TCORE_RETURN_SUCCESS;
	}

	modem = tcore_user_request_get_modem_name(ur);
	if (modem == NULL)
		return TCORE_RETURN_EINVAL;

	p = tcore_server_find_plugin(s, modem);
	if (p == NULL) {
		free(modem);
		return TCORE_RETURN_SERVER_WRONG_PLUGIN;
	}
	free(modem);

	command = tcore_user_request_get_command(ur);

	category = CORE_OBJECT_TYPE_DEFAULT | (command & 0x0FF00000);

	co_list = tcore_plugin_get_core_objects_bytype(p, category);
	if (co_list == NULL) {
		warn("can't find 0x%x core_object", category);
		return TCORE_RETURN_ENOSYS;
	}

	for (list = co_list; list; list = list->next) {
		co = (CoreObject *) list->data;
		if (co == NULL) {
			warn("can't find 0x%x core_object", category);
			continue;
		}

		if (tcore_object_dispatch_request(co, ur) == TCORE_RETURN_SUCCESS)
			ret = TCORE_RETURN_SUCCESS;
		else
			dbg("failed...");
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

	if (s == NULL)
		return TCORE_RETURN_EINVAL;

	for (list = s->hook_list_notification; list; list = list->next) {
		hook = list->data;
		if (hook == NULL) {
			continue;
		}

		if (hook->command == command) {
			if (hook->func(s, source, command, data_len, data, hook->user_data)
					== TCORE_HOOK_RETURN_STOP_PROPAGATION)
				return TCORE_RETURN_SUCCESS;
		}
	}

	for (list = s->communicators; list; list = list->next) {
		comm = list->data;
		if (comm == NULL)
			continue;

		tcore_communicator_send_notification(comm, source, command, data_len, data);
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_server_add_request_hook(Server *s,
		enum tcore_request_command command,
		tcore_server_request_hook func, void *user_data)
{
	struct hook_request_type *hook;

	if ((s == NULL)|| (func == NULL))
		return TCORE_RETURN_EINVAL;

	hook = g_try_new0(struct hook_request_type, 1);
	if (hook == NULL)
		return TCORE_RETURN_ENOMEM;

	hook->command = command;
	hook->func = func;
	hook->user_data = user_data;

	s->hook_list_request = g_slist_append(s->hook_list_request, hook);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_server_remove_request_hook(Server *s, tcore_server_request_hook func)
{
	struct hook_request_type *hook;
	GSList *list;

	if (s == NULL)
		return TCORE_RETURN_EINVAL;

	for (list = s->hook_list_request; list; list = list->next) {
		hook = list->data;
		if (hook == NULL) {
			continue;
		}

		if (hook->func == func) {
			s->hook_list_request = g_slist_remove(s->hook_list_request, hook);
			list = s->hook_list_request;
			g_free(hook);
		}
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_server_add_notification_hook(Server *s,
		enum tcore_notification_command command,
		tcore_server_notification_hook func, void *user_data)
{
	struct hook_notification_type *hook;

	if ((s == NULL)|| (func == NULL))
		return TCORE_RETURN_EINVAL;

	hook = g_try_new0(struct hook_notification_type, 1);
	if (hook == NULL)
		return TCORE_RETURN_ENOMEM;

	hook->command = command;
	hook->func = func;
	hook->user_data = user_data;

	s->hook_list_notification = g_slist_append(s->hook_list_notification, hook);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_server_remove_notification_hook(Server *s,
		tcore_server_notification_hook func)
{
	struct hook_notification_type *hook;
	GSList *list;

	if (s == NULL)
		return TCORE_RETURN_EINVAL;

	for (list = s->hook_list_notification; list; list = list->next) {
		hook = list->data;
		if (hook == NULL) {
			continue;
		}

		if (hook->func == func) {
			s->hook_list_notification = g_slist_remove(s->hook_list_notification, hook);
			list = s->hook_list_notification;
			g_free(hook);
		}
	}

	return TCORE_RETURN_SUCCESS;
}

gboolean tcore_server_register_modem(Server *s, TcorePlugin *modem_iface_plugin)
{
	TcoreModem *modem;

	if ((s == NULL) || (modem_iface_plugin == NULL))
		return FALSE;

	modem = g_try_new0(TcoreModem, 1);
	if (modem == NULL)
		return FALSE;

	modem->cp_name = _server_enumerate_modem(modem_iface_plugin);
	modem->modem_iface_plugin = modem_iface_plugin;

	s->modems = g_slist_append(s->modems, modem);

	return TRUE;
}

void tcore_server_unregister_modem(Server *s, TcorePlugin *modem_iface_plugin)
{
	TcoreModem *modem;

	if ((s == NULL) || (modem_iface_plugin == NULL))
		return;

	modem = _server_find_modem(s, modem_iface_plugin, NULL);
	if (modem == NULL)
		return;

	s->modems = g_slist_remove(s->modems, modem);

	g_free(modem->cp_name);
	g_free(modem);
}

gboolean tcore_server_update_modem_plugin(TcorePlugin *modem_iface_plugin,
							TcorePlugin *modem_plugin)
{
	Server *s;
	TcoreModem *modem;

	if ((modem_iface_plugin == NULL) || (modem_plugin == NULL))
		return FALSE;

	s = tcore_plugin_ref_server(modem_iface_plugin);
	if (s == NULL)
		return FALSE;

	modem = _server_find_modem(s, modem_iface_plugin, NULL);
	if (modem == NULL)
		return FALSE;

	modem->modem_plugin = modem_plugin;

	return TRUE;
}

char **tcore_server_get_cp_name_list(Server *s)
{
	char **cp_name_list = NULL;
	GSList *list;
	unsigned int list_count;
	TcoreModem *modem;

	int i = 0;

	if (s == NULL)
		return NULL;

	list_count = g_slist_length(s->modems);
	if (list_count == 0)
		return NULL;

	/* (+1) is considered for NULL string to define the last string */
	cp_name_list = g_try_new0(char *, list_count + 1);
	if (cp_name_list == NULL)
		return NULL;

	for (list = s->modems; list; list = list->next) {
		modem = list->data;
		if (modem == NULL)
			continue;

		cp_name_list[i] = g_strdup(modem->cp_name);
		dbg("CP Name[%d] = %s", i, cp_name_list[i]);

		i++;
	}

	/* 'cp_name_list' would be freed by the calling function */
	return cp_name_list;
}

const char *tcore_server_get_cp_name_by_plugin(TcorePlugin *plugin)
{
	Server *s;
	TcoreModem *modem;

	if (plugin == NULL)
		return NULL;

	s = tcore_plugin_ref_server(plugin);
	if (s == NULL)
		return NULL;

	modem = _server_find_modem(s, plugin, plugin);
	if (modem == NULL)
		return NULL;

	return (const char *)modem->cp_name;
}

gboolean tcore_server_add_cp_mapping_tbl_entry(TcorePlugin *modem_iface_plugin,
					unsigned int co_type, TcoreHal *hal)
{
	Server *s;
	TcoreModem *modem;

	if (modem_iface_plugin == NULL)
		return FALSE;

	s = tcore_plugin_ref_server(modem_iface_plugin);
	if (s == NULL)
		return FALSE;

	modem = _server_find_modem(s, modem_iface_plugin, NULL);
	if (modem == NULL)
		return FALSE;

	/*
	 * Set the Mapping Table to the Modems list
	 */
	return tcore_object_add_mapping_tbl_entry(modem->mapping_tbl, co_type, hal);
}

void tcore_server_remove_cp_mapping_tbl_entry(TcorePlugin *modem_iface_plugin,
					TcoreHal *hal)
{
	Server *s;
	TcoreModem *modem;

	if (modem_iface_plugin == NULL)
		return;

	s = tcore_plugin_ref_server(modem_iface_plugin);
	if (s == NULL)
		return;

	modem = _server_find_modem(s, modem_iface_plugin, NULL);
	if (modem == NULL)
		return;

	/* Removing the Mapping Table from the Modems list */
	tcore_object_remove_mapping_tbl_entry(modem->mapping_tbl, hal);
	modem->mapping_tbl = NULL;
}

void *tcore_server_get_cp_mapping_tbl(TcorePlugin *modem_plugin)
{
	Server *s;
	TcoreModem *modem;

	if (modem_plugin == NULL)
		return NULL;

	s = tcore_plugin_ref_server(modem_plugin);
	if (s == NULL)
		return NULL;

	modem = _server_find_modem(s, NULL, modem_plugin);
	if (modem == NULL)
		return NULL;

	return modem->mapping_tbl;
}

TReturn tcore_server_load_modem_plugin(Server *s,
					TcorePlugin *modem_if_plugin,
					const char *name)
{
	struct tcore_plugin_define_desc *desc;
	TcorePlugin *modem_plugin;
	char *filename = NULL;
	void *handle;
	TReturn ret = TCORE_RETURN_FAILURE;

	dbg("Enter");

	if (s == NULL || name == NULL) {
		ret = TCORE_RETURN_EINVAL;
		goto out;
	}

	filename = g_build_filename("/usr/lib/telephny/plugins/modems",
					name, NULL);

	handle = dlopen(filename, RTLD_NOW);
	if (handle == NULL) {
		dbg("Failed to load '%s': %s", filename, dlerror());
		goto out;
	}

	desc = dlsym(handle, "plugin_define_desc");
	if (desc == NULL) {
		dbg("Failed to load symbol: %s", dlerror());
		dlclose(handle);
		goto out;
	}

	dbg("Plugin %s found", desc->name);

	if (desc->load != NULL) {
		if (desc->load() == FALSE) {
			err("Failed to load %s plugin", desc->name);
			dlclose(handle);
			goto out;
		}
	}

	modem_plugin = tcore_plugin_new(s, desc, filename, handle);
	tcore_server_add_plugin(s, modem_plugin);

	if (modem_if_plugin != NULL)
		tcore_server_update_modem_plugin(modem_if_plugin,
							modem_plugin);

	dbg("Plugin %s loaded successfully", desc->name);

	if (desc->init == NULL) {
		err("Plugin %s has not initializer", desc->name);
		dlclose(handle);
		goto out;
	}

	if (desc->init(modem_plugin) == FALSE) {
		err("Plugin %s initialization failed", desc->name);
		dlclose(handle);
		goto out;
	}

	dbg("Plugin %s initialization success", desc->name);
	ret = TCORE_RETURN_SUCCESS;

out:
	g_free(filename);

	dbg("Exit");

	return ret;
}
