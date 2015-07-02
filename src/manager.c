/*
 * libtcore
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd. All rights reserved.
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
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "tcore.h"
#include "server.h"
#include "manager.h"
#include "plugin.h"
#include "core_object.h"

struct tcore_manager_type {
	TcorePlugin *parent_plugin;

	ManagerRequestDispatcher request_dispatcher;
	ManagerNotificationHandler notification_handler;
};

Manager *tcore_manager_new(TcorePlugin *manager_plugin)
{
	Server *server;
	Manager *manager;

	/* Allocate Manager */
	manager = g_try_malloc0(sizeof(Manager));
	if (manager == NULL)
		return NULL;

	manager->parent_plugin = manager_plugin;

	/* Set Manager to Server */
	server = tcore_plugin_ref_server(manager_plugin);
	tcore_server_set_manager(server, manager);

	return manager;
}

void tcore_manager_free(TcorePlugin *manager_plugin, Manager *manager)
{
	Server *server;

	/* Free Manager */
	if (manager)
		g_free(manager);

	/* Reset Manager at Server */
	server = tcore_plugin_ref_server(manager_plugin);
	tcore_server_set_manager(server, NULL);
}

TReturn tcore_manager_set_request_dispatcher(Manager *manager,
	ManagerRequestDispatcher request_dispatcher)
{
	if (!manager)
		return TCORE_RETURN_FAILURE;

	/* Update Request dispatcher at Manager */
	manager->request_dispatcher = request_dispatcher;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_manager_set_notification_handler(Manager *manager,
	ManagerNotificationHandler notification_handler)
{
	if (!manager)
		return TCORE_RETURN_FAILURE;

	/* Update Notification handler at Manager */
	manager->notification_handler = notification_handler;

	return TCORE_RETURN_SUCCESS;
}

enum tcore_manager_return tcore_manager_dispatch_request(Manager *manager, UserRequest *ur)
{
	if (!manager || !manager->request_dispatcher)
		return TCORE_MANAGER_RETURN_CONTINUE;

	return manager->request_dispatcher(manager, ur);
}

enum tcore_manager_return tcore_manager_send_notification(Manager *manager, CoreObject *source,
	enum tcore_notification_command command,
	unsigned int data_len, void *data)
{
	if (!manager || !manager->notification_handler)
		return TCORE_MANAGER_RETURN_CONTINUE;

	return manager->notification_handler(manager,
			source, command, data_len, data);
}

TcorePlugin *tcore_manager_get_plugin(Manager *manager)
{
	if (!manager)
		return NULL;

	/* Provide 'parent plug-in' */
	return manager->parent_plugin;
}
