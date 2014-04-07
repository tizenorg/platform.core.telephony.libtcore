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

#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "server.h"
#include "communicator.h"

struct tcore_communicator_type {
	gchar *name;
	TcoreCommunicatorOps *ops;

	void *user_data;

	TcorePlugin *parent_plugin;
};


Communicator* tcore_communicator_new(TcorePlugin *plugin,
	const gchar *name, TcoreCommunicatorOps *ops)
{
	Communicator *comm;

	comm = g_try_new0(struct tcore_communicator_type, 1);
	if (comm == NULL) {
		err("Failed to allocate memory");
		return NULL;
	}

	/* Update parameters */
	comm->name = tcore_strdup(name);
	comm->parent_plugin = plugin;
	comm->ops = ops;

	dbg("Comunicator '%s' created", name);

	/* Add communicator to Server */
	tcore_server_add_communicator(tcore_plugin_ref_server(plugin), comm);

	return comm;
}

void tcore_communicator_free(Communicator *comm)
{
	if (comm == NULL) {
		err("Communicator is NULL");
		return;
	}

	dbg("Comunicator '%s' free", comm->name);

	/* Free resources */
	tcore_free((void *)comm->name);
	tcore_free(comm);
}

TcorePlugin *tcore_communicator_ref_plugin(Communicator *comm)
{
	if (comm == NULL) {
		err("Communicator is NULL");
		return NULL;
	}

	return comm->parent_plugin;
}

const gchar *tcore_communicator_ref_name(Communicator *comm)
{
	if (comm == NULL) {
		err("Communicator is NULL");
		return NULL;
	}

	return comm->name;
}

TelReturn tcore_communicator_link_user_data(Communicator *comm, void *data)
{
	if (comm == NULL) {
		err("Communicator is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	comm->user_data = data;

	return TEL_RETURN_SUCCESS;
}

void *tcore_communicator_ref_user_data(Communicator *comm)
{
	if (comm == NULL) {
		err("Communicator is NULL");
		return NULL;
	}

	return comm->user_data;
}

TelReturn tcore_communicator_send_notification(Communicator *comm,
	TcorePlugin *source, gint command, guint data_len, const void *data)
{
	if ((comm == NULL) || (comm->ops == NULL)
			|| (comm->ops->send_notification == NULL)) {
		err("comm: [%p] ops: [%p] send_notiifcation: [%p]",
				comm, (comm ? comm->ops : NULL),
				((comm && comm->ops) ? comm->ops->send_notification : NULL));
		return TEL_RETURN_INVALID_PARAMETER;
	}

	return comm->ops->send_notification(comm, source, command, data_len, data);
}
