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
	GSList *hals;
	GSList *hook_list_request;
	GSList *hook_list_notification;
	TcorePlugin *default_plugin;
	TcoreUdev *udev;
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
		if (!p)
			continue;

		co_list = tcore_plugin_get_core_objects_bytype(p, CORE_OBJECT_TYPE_MODEM);
		if (!co_list)
			continue;

		g_slist_free(co_list);
		s->default_plugin = p;
		return p;
	}

	return NULL;
}

Server *tcore_server_new()
{
	Server *s;

	s = calloc(sizeof(struct tcore_server_type), 1);
	if (!s)
		return NULL;

	s->mainloop = g_main_loop_new (NULL, FALSE);
	if (!s->mainloop) {
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

	if (!s)
		return;

	if (s->mainloop)
		g_main_loop_unref(s->mainloop);

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

    if (s->plugins) {
        g_slist_free(s->plugins);
        s->plugins = NULL;
    }
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
	TcorePlugin *p;

	if (g_strcmp0(name, TCORE_PLUGIN_DEFAULT) == 0) {
		return _find_default_plugin(s);
	}

	for (list = s->plugins; list; list = list->next) {
		p = list->data;
		if (!p) {
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
		if (!comm) {
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
		if (!strg) {
			continue;
		}

		if (g_strcmp0(tcore_storage_ref_name(strg), name) == 0) {
			return strg;
		}
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
		if (!temp) {
			continue;
		}

		if (temp == hal)
			return TCORE_RETURN_EALREADY;
	}

	s->hals = g_slist_insert(s->hals, hal, 0);

	tcore_server_send_notification(s, NULL, TNOTI_SERVER_ADDED_HAL, 0, NULL);

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
		if (!hal) {
			continue;
		}

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

TReturn tcore_server_link_udev(Server *s, TcoreUdev *udev)
{
	if (!s || !udev)
		return TCORE_RETURN_EINVAL;

	s->udev = udev;

	return TCORE_RETURN_SUCCESS;
}

TcoreUdev *tcore_server_ref_udev(Server *s)
{
	if (!s)
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
	CoreObject *o;
	TReturn ret = TCORE_RETURN_ENOSYS;

	if (!s || !ur)
		return TCORE_RETURN_EINVAL;

	for (list = s->hook_list_request; list; list = list->next) {
		hook = list->data;
		if (!hook) {
			continue;
		}

		if (hook->command == tcore_user_request_get_command(ur)) {
			if (hook->func(s, ur, hook->user_data) == TCORE_HOOK_RETURN_STOP_PROPAGATION) {
				return TCORE_RETURN_SUCCESS;
			}
		}
	}

	modem = tcore_user_request_get_modem_name(ur);
	if (!modem)
		return TCORE_RETURN_EINVAL;

	p = tcore_server_find_plugin(s, modem);
	if (!p) {
		free(modem);
		return TCORE_RETURN_SERVER_WRONG_PLUGIN;
	}
	free(modem);

	command = tcore_user_request_get_command(ur);

	category = CORE_OBJECT_TYPE_DEFAULT | (command & 0x0FF00000);

	co_list = tcore_plugin_get_core_objects_bytype(p, category);
	if (!co_list) {
		warn("can't find 0x%x core_object", category);
		return TCORE_RETURN_ENOSYS;
	}

	for (list = co_list; list; list = list->next) {
		o = (CoreObject *) list->data;
		if (!o) {
			warn("can't find 0x%x core_object", category);
			continue;
		}

		if (tcore_object_dispatch_request(o, ur) == TCORE_RETURN_SUCCESS)
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

	if (!s)
		return TCORE_RETURN_EINVAL;

	for (list = s->hook_list_notification; list; list = list->next) {
		hook = list->data;
		if (!hook) {
			continue;
		}

		if (hook->command == command) {
			if (hook->func(s, source, command, data_len, data, hook->user_data) == TCORE_HOOK_RETURN_STOP_PROPAGATION) {
				return TCORE_RETURN_SUCCESS;
			}
		}
	}

	for (list = s->communicators; list; list = list->next) {
		comm = list->data;
		if (!comm) {
			continue;
		}

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

	hook = calloc(sizeof(struct hook_request_type), 1);
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
		if (!hook) {
			continue;
		}

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

	hook = calloc(sizeof(struct hook_notification_type), 1);
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
		if (!hook) {
			continue;
		}

		if (hook->func == func) {
			s->hook_list_notification = g_slist_remove(s->hook_list_notification, hook);
			free(hook);
			list = s->hook_list_notification;
		}
	}

	return TCORE_RETURN_SUCCESS;
}
