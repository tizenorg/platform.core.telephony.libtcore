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
#include <stdlib.h>

#include <glib.h>

#include "tcore.h"
#include "udev.h"

struct tcore_udev_type {
	Server *server;

	GUdevClient *client;
	GUdevEnumerator *enumer;
	GSList *enumer_callbacks;
	GSList *callbacks;
};

struct udev_enumer_callback_type {
	TcoreUdevEnumerCallback func;
	void *user_data;
};

struct udev_callback_type {
	TcoreUdevCallback func;
	char *subsystem;
	char *action;
	void *user_data;
};

static const gchar *_default_subsystems[] = { NULL, };

static void _on_uevent(GUdevClient *client, gchar *action, GUdevDevice *device, gpointer user_data)
{
	TcoreUdev *udev = user_data;
	GSList *cb = NULL;
	struct udev_callback_type *node;

	if (!udev)
		return;

	dbg("action = '%s'", action);

	for (cb = udev->callbacks; cb; cb = cb->next) {
		node = cb->data;
		if (!node)
			continue;

		if (node->action)
			if (g_strcmp0(node->action, action) != 0)
				continue;

		if (node->subsystem)
			if (g_strcmp0(node->subsystem, g_udev_device_get_subsystem(device)) != 0)
				continue;

		if (!node->func)
			continue;

		node->func(udev, device, node->user_data);
	}
}

TcoreUdev *tcore_udev_new(Server *s, const gchar **subsystems)
{
	TcoreUdev *udev;

	udev = calloc(sizeof(struct tcore_udev_type), 1);
	if (!udev)
		return NULL;

	if (!subsystems) {
		subsystems = _default_subsystems;
	}

	udev->server = s;
	udev->client = g_udev_client_new(subsystems);
	if (!udev->client) {
		free(udev);
		return NULL;
	}

	g_signal_connect(udev->client, "uevent", G_CALLBACK(_on_uevent), udev);

	udev->enumer = g_udev_enumerator_new(udev->client);

	return udev;
}

void tcore_udev_free(TcoreUdev *udev)
{
	if (!udev)
		return;

	if (udev->client)
		g_object_unref(udev->client);

	if (udev->enumer)
		g_object_unref(udev->enumer);

	free(udev);
}

Server *tcore_udev_ref_server(TcoreUdev *udev)
{
	if (!udev)
		return NULL;

	return udev->server;
}

GUdevClient *tcore_udev_ref_client(TcoreUdev *udev)
{
	if (!udev)
		return NULL;

	return udev->client;
}

GUdevEnumerator *tcore_udev_ref_enumerator(TcoreUdev *udev)
{
	if (!udev)
		return NULL;

	return udev->enumer;
}

TReturn tcore_udev_add_enumerator_callback(TcoreUdev *udev, TcoreUdevEnumerCallback func, void *user_data)
{
	struct udev_enumer_callback_type *node;

	if (!udev || !func)
		return TCORE_RETURN_FAILURE;

	node = calloc(sizeof(struct udev_enumer_callback_type), 1);
	if (!node)
		return TCORE_RETURN_ENOMEM;

	node->func = func;
	node->user_data = user_data;

	udev->enumer_callbacks = g_slist_append(udev->enumer_callbacks, node);

	return TCORE_RETURN_SUCCESS;
}

GList *tcore_udev_exec_enumerator(TcoreUdev *udev, gboolean event_emit_flag)
{
	GList *list = NULL;
	GSList *cb = NULL;
	struct udev_enumer_callback_type *node;

	if (!udev) {
		return NULL;
	}

	list = g_udev_enumerator_execute(udev->enumer);
	if (!list) {
		return NULL;
	}

	if (event_emit_flag == FALSE)
		return list;

	for (cb = udev->enumer_callbacks; cb; cb = cb->next) {
		node = cb->data;
		if (!node) {
			continue;
		}

		if (!node->func)
			continue;

		node->func(udev, list, node->user_data);
	}

	return list;
}

TReturn tcore_udev_add_callback(TcoreUdev *udev, const char *subsystem, const char *action, TcoreUdevCallback func, void *user_data)
{
	struct udev_callback_type *node;

	if (!udev || !func)
		return TCORE_RETURN_FAILURE;

	node = calloc(sizeof(struct udev_callback_type), 1);
	if (!node)
		return TCORE_RETURN_ENOMEM;

	node->func = func;
	node->user_data = user_data;

	if (subsystem)
		node->subsystem = strdup(subsystem);

	if (action)
		node->action = strdup(action);

	udev->callbacks = g_slist_append(udev->callbacks, node);

	dbg("subsystem = [%s]", node->subsystem);
	dbg("action = [%s]", node->action);
	dbg("callbacks length = %d", g_slist_length(udev->callbacks));

	return TCORE_RETURN_SUCCESS;
}

