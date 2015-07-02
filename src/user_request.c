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
	int ref;

	/* Communicator information */
	void *user_info;
	Communicator *comm;

	char *modem_name;

	enum tcore_request_command command;

	void *data;
	unsigned int data_len;

	void *metainfo;
	unsigned int metainfo_len;

	UserRequestFreeHook free_hook;
	GSList *response_hook;
};

struct hook_response_type {
	UserRequestResponseHook func;
	void *user_data;
};

UserRequest *tcore_user_request_new(Communicator *comm, const char *modem_name)
{
	UserRequest *ur;

	ur = calloc(1, sizeof(struct tcore_user_request_type));
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

	if (ur->ref > 0) {
		ur->ref--;
		return;
	}

	g_slist_free_full(ur->response_hook, free);

	if (ur->free_hook)
		ur->free_hook(ur);

	if (ur->modem_name)
		free(ur->modem_name);

	if (ur->data)
		free(ur->data);

	if (ur->metainfo)
		free(ur->metainfo);

	dbg("user_request(%p) free.", ur);

	free(ur);
}

UserRequest *tcore_user_request_ref(UserRequest *ur)
{
	if (!ur)
		return NULL;

	ur->ref++;

	return ur;
}

void tcore_user_request_unref(UserRequest *ur)
{
	if (!ur)
		return;

	if (ur->ref > 0)
		ur->ref--;
	else
		tcore_user_request_free(ur);

	return;
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
	struct hook_response_type *hook;

	if (!ur)
		return TCORE_RETURN_EINVAL;

	hook = calloc(1, sizeof(struct hook_response_type));
	if (!hook)
		return TCORE_RETURN_ENOMEM;

	hook->func = resp_hook;
	hook->user_data = user_data;

	ur->response_hook = g_slist_append(ur->response_hook, hook);

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

TReturn tcore_user_request_set_user_info(UserRequest *ur, void *user_info)
{
	if (!ur)
		return TCORE_RETURN_EINVAL;

	ur->user_info = user_info;

	return TCORE_RETURN_SUCCESS;
}

void *tcore_user_request_ref_user_info(UserRequest *ur)
{
	if (!ur)
		return NULL;

	return ur->user_info;
}

TReturn tcore_user_request_send_response(UserRequest *ur,
		enum tcore_response_command command,
		unsigned int data_len, const void *data)
{
	GSList *list;
	struct hook_response_type *hook;

	if (!ur) {
		dbg("ur is NULL");
		return TCORE_RETURN_EINVAL;
	}

	for (list = ur->response_hook; list;) {
		hook = list->data;
		list = list->next;

		if (!hook)
			continue;

		if (hook->func)
			hook->func(ur, command, data_len, data, hook->user_data);
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

	if (ur->data != NULL) {
		free(ur->data);
		ur->data = NULL;
	}

	ur->data_len = data_len;

	if (data_len > 0) {
		if (data != NULL) {
			ur->data = calloc(data_len, 1);
			if (!ur->data) {
				ur->data_len = 0;
				return TCORE_RETURN_ENOMEM;
			}

			memcpy(ur->data, data, data_len);
		} else
			ur->data_len = 0;
	} else
		ur->data = NULL;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_user_request_set_metainfo(UserRequest *ur,
		unsigned int metainfo_len, const void *metainfo)
{
	if (!ur)
		return TCORE_RETURN_EINVAL;

	if (ur->metainfo != NULL) {
		free(ur->metainfo);
		ur->metainfo = NULL;
	}

	ur->metainfo_len = metainfo_len;

	if (metainfo_len > 0) {
		if (metainfo != NULL) {
			ur->metainfo = calloc(metainfo_len, 1);
			if (!ur->metainfo) {
				ur->metainfo_len = 0;
				return TCORE_RETURN_ENOMEM;
			}

			memcpy(ur->metainfo, metainfo, metainfo_len);
		} else
			ur->metainfo_len = 0;
	} else
		ur->metainfo = NULL;

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
