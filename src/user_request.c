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
#include "user_request.h"
#include "communicator.h"

struct tcore_user_request_type {
	struct tcore_user_info ui;

	Communicator *comm;

	char *modem_name;

	enum tcore_request_command command;

	void *data;
	unsigned int data_len;

	void *metainfo;
	unsigned int metainfo_len;

	UserRequestFreeHook free_hook;
	UserRequestResponseHook response_hook;
	void *response_hook_user_data;
};

UserRequest *tcore_user_request_new(Communicator *comm, const char *modem_name)
{
	UserRequest *ur;

	ur = calloc(sizeof(struct tcore_user_request_type), 1);
	if (!ur)
		return NULL;

	ur->comm = comm;

	if (modem_name)
		ur->modem_name = strdup(modem_name);
	else
		ur->modem_name = NULL;

	return ur;
}

void tcore_user_request_free(UserRequest *ur)
{
	if (!ur)
		return;

	if (ur->free_hook)
		ur->free_hook(ur);

	if (ur->modem_name)
		free(ur->modem_name);

	if (ur->data)
		free(ur->data);

	if(ur->metainfo)
		free(ur->metainfo);

	dbg("user_request(0x%x) free.", ur);

	free(ur);
}

UserRequest *tcore_user_request_dup(UserRequest *ur)
{
	UserRequest *dest;

	if (!ur)
		return NULL;

	dest = tcore_user_request_new(ur->comm, ur->modem_name);
	if (!dest)
		return NULL;

	tcore_user_request_set_user_info(dest, &(ur->ui));
	dest->command = ur->command;

	if (ur->data_len > 0) {
		dest->data = calloc(ur->data_len, 1);
		dest->data_len = ur->data_len;
		memcpy(dest->data, ur->data, ur->data_len);
	}

	if (ur->metainfo_len > 0) {
		dest->metainfo = calloc(ur->metainfo_len,1);
		dest->metainfo_len = ur->metainfo_len;
		memcpy(dest->metainfo, ur->metainfo, ur->metainfo_len);
	}

	if (ur->free_hook)
		dest->free_hook = ur->free_hook;

	if (ur->response_hook)
		dest->response_hook = ur->response_hook;

	if (ur->response_hook_user_data)
		dest->response_hook_user_data = ur->response_hook_user_data;

	return dest;
}

TReturn tcore_user_request_set_free_hook(UserRequest *ur,
		UserRequestFreeHook free_hook)
{
	if (!ur)
		return TCORE_RETURN_EINVAL;

	ur->free_hook = free_hook;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_user_request_set_response_hook(UserRequest *ur,
		UserRequestResponseHook resp_hook, void *user_data)
{
	if (!ur)
		return TCORE_RETURN_EINVAL;

	ur->response_hook = resp_hook;
	ur->response_hook_user_data = user_data;

	return TCORE_RETURN_SUCCESS;
}

Communicator *tcore_user_request_ref_communicator(UserRequest *ur)
{
	if (!ur)
		return NULL;

	return ur->comm;
}

char *tcore_user_request_get_modem_name(UserRequest *ur)
{
	if (!ur)
		return NULL;

	if (!ur->modem_name)
		return NULL;

	return strdup(ur->modem_name);
}

TReturn tcore_user_request_set_user_info(UserRequest *ur,
		const struct tcore_user_info *ui)
{
	if (!ur || !ui)
		return TCORE_RETURN_EINVAL;

	ur->ui.uid = ui->uid;
	ur->ui.gid = ui->gid;
	ur->ui.pid = ui->pid;
	ur->ui.channel_id = ui->channel_id;
	ur->ui.client_cmd = ui->client_cmd;

	if (ur->ui.appname) {
		dbg("free old appname (%s)", ur->ui.appname);
		free(ur->ui.appname);
	}

	if (ui->appname) {
		ur->ui.appname = strdup(ui->appname);
		dbg("alloc appname(%s, %s)", ur->ui.appname, ui->appname);
	}

	return TCORE_RETURN_SUCCESS;
}

const struct tcore_user_info *tcore_user_request_ref_user_info(UserRequest *ur)
{
	if (!ur)
		return NULL;

	return &(ur->ui);
}

TReturn tcore_user_request_send_response(UserRequest *ur,
		enum tcore_response_command command,
		unsigned int data_len, const void *data)
{
	if (!ur) {
		dbg("ur is NULL");
		return TCORE_RETURN_EINVAL;
	}

	if (ur->response_hook) {
		ur->response_hook(ur, command, data_len, data,
				ur->response_hook_user_data);
	}

	if (ur->comm) {
		return tcore_communicator_send_response(ur->comm, ur,
				command, data_len, data);
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_user_request_set_command(UserRequest *ur,
		enum tcore_request_command command)
{
	if (!ur)
		return TCORE_RETURN_EINVAL;

	ur->command = command;

	return TCORE_RETURN_SUCCESS;
}

enum tcore_request_command tcore_user_request_get_command(UserRequest *ur)
{
	if (!ur)
		return 0;

	return ur->command;
}

TReturn tcore_user_request_set_data(UserRequest *ur,
		unsigned int data_len, const void *data)
{
	if (!ur)
		return TCORE_RETURN_EINVAL;

	ur->data_len = data_len;

	if (data_len > 0 && data != NULL) {
		ur->data = calloc(data_len, 1);
		if (!ur->data)
			return TCORE_RETURN_ENOMEM;

		memcpy(ur->data, data, data_len);
	}
	else {
		ur->data = NULL;
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_user_request_set_metainfo(UserRequest *ur,
		unsigned int metainfo_len, const void *metainfo)
{
	if (!ur)
		return TCORE_RETURN_EINVAL;

	ur->metainfo_len = metainfo_len;

	if (metainfo_len > 0 && metainfo != NULL) {
		ur->metainfo = calloc(metainfo_len, 1);
		if (!ur->metainfo)
			return TCORE_RETURN_ENOMEM;

		memcpy(ur->metainfo, metainfo, metainfo_len);
	}
	else {
		ur->metainfo = NULL;
	}

	return TCORE_RETURN_SUCCESS;
}

const void *tcore_user_request_ref_data(UserRequest *ur,
		unsigned int *data_len)
{
	if (!ur)
		return NULL;

	if (data_len)
		*data_len = ur->data_len;

	return ur->data;
}

const void *tcore_user_request_ref_metainfo(UserRequest *ur,
		unsigned int *metainfo_len)
{
	if (!ur)
		return NULL;

	if (metainfo_len)
		*metainfo_len = ur->metainfo_len;

	return ur->metainfo;
}
