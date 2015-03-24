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
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "server.h"
#include "communicator.h"

struct tcore_communicator_type {
	const char *name;
	struct tcore_communitor_operations *ops;

	void *user_data;

	TcorePlugin *parent_plugin;
};


Communicator* tcore_communicator_new(TcorePlugin *plugin, const char *name,
		struct tcore_communitor_operations *ops)
{
	Communicator *comm;

	comm = calloc(1, sizeof(struct tcore_communicator_type));
	if (!comm)
		return NULL;

	if (name)
		comm->name = strdup(name);

	comm->parent_plugin = plugin;
	comm->ops = ops;

	tcore_server_add_communicator(tcore_plugin_ref_server(plugin), comm);

	return comm;
}

void tcore_communicator_free(Communicator *comm)
{
	if (!comm)
		return;

	if (comm->name)
		free((void *)comm->name);

	free(comm);
}

TcorePlugin *tcore_communicator_ref_plugin(Communicator *comm)
{
	if (!comm)
		return NULL;

	return comm->parent_plugin;
}

const char *tcore_communicator_ref_name(Communicator *comm)
{
	if (!comm)
		return NULL;

	return comm->name;
}

TReturn tcore_communicator_link_user_data(Communicator *comm, void *data)
{
	if (!comm)
		return TCORE_RETURN_EINVAL;

	comm->user_data = data;

	return TCORE_RETURN_SUCCESS;
}

void *tcore_communicator_ref_user_data(Communicator *comm)
{
	if (!comm)
		return NULL;

	return comm->user_data;
}

TReturn tcore_communicator_dispatch_request(Communicator *comm, UserRequest *ur)
{
	Server *s;

	if (!comm || !ur)
		return TCORE_RETURN_EINVAL;

	s = tcore_plugin_ref_server(comm->parent_plugin);

	return tcore_server_dispatch_request(s, ur);
}

TReturn tcore_communicator_send_response(Communicator *comm, UserRequest *ur,
		enum tcore_response_command command,
		unsigned int data_len, const void *data)
{
	if (!comm || !comm->ops || !comm->ops->send_response)
		return TCORE_RETURN_EINVAL;

	dbg("ur = %p", ur);

	return comm->ops->send_response(comm, ur, command, data_len, data);
}

TReturn tcore_communicator_send_notification(Communicator *comm,
		CoreObject *source, enum tcore_notification_command command,
		unsigned int data_len, const void *data)
{
	if (!comm || !comm->ops || !comm->ops->send_notification)
		return TCORE_RETURN_EINVAL;

	return comm->ops->send_notification(comm, source, command, data_len, data);
}
