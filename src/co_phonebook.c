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
#include "queue.h"
#include "user_request.h"
#include "core_object.h"
#include "co_phonebook.h"

struct private_object_data {
	struct tcore_phonebook_operations *ops;
	gboolean b_init;
	struct tel_phonebook_support_list support_list;
};

static TReturn _dispatcher(CoreObject *o, UserRequest *ur)
{
	enum tcore_request_command command;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po || !po->ops)
		return TCORE_RETURN_ENOSYS;

	command = tcore_user_request_get_command(ur);
	switch (command) {
		case TREQ_PHONEBOOK_GETCOUNT:
			if (!po->ops->get_count)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_count(o, ur);
			break;

		case TREQ_PHONEBOOK_GETMETAINFO:
			if (!po->ops->get_info)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_info(o, ur);
			break;

		case TREQ_PHONEBOOK_GETUSIMINFO:
			if (!po->ops->get_usim_info)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_usim_info(o, ur);
			break;

		case TREQ_PHONEBOOK_READRECORD:
			if (!po->ops->read_record)
				return TCORE_RETURN_ENOSYS;

			return po->ops->read_record(o, ur);
			break;

		case TREQ_PHONEBOOK_UPDATERECORD:
			if (!po->ops->update_record)
				return TCORE_RETURN_ENOSYS;

			return po->ops->update_record(o, ur);
			break;

		case TREQ_PHONEBOOK_DELETERECORD:
			if (!po->ops->delete_record)
				return TCORE_RETURN_ENOSYS;

			return po->ops->delete_record(o, ur);
			break;

		default:
			break;
	}

	return TCORE_RETURN_SUCCESS;
}

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_PHONEBOOK);

	po = tcore_object_ref_object(o);
	if (po) {
		free(po);
		tcore_object_link_object(o, NULL);
	}
}

gboolean tcore_phonebook_get_status(CoreObject *o)
{
	struct private_object_data *po = NULL;
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, TCORE_RETURN_EINVAL);
	po = tcore_object_ref_object(o);
	return po->b_init;
}

gboolean tcore_phonebook_set_status(CoreObject *o, gboolean b_init)
{
	struct private_object_data *po = NULL;
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, TCORE_RETURN_EINVAL);
	po = tcore_object_ref_object(o);
	po->b_init = b_init;
	return TRUE;
}

struct tel_phonebook_support_list* tcore_phonebook_get_support_list(CoreObject *o)
{
	struct tel_phonebook_support_list *tmp;
	struct private_object_data *po = NULL;
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, NULL);
	po = tcore_object_ref_object(o);
	tmp = calloc(sizeof(struct tel_phonebook_support_list), 1);
	memcpy(tmp, &po->support_list, sizeof(struct tel_phonebook_support_list));
	return tmp;
}

gboolean tcore_phonebook_set_support_list(CoreObject *o, struct tel_phonebook_support_list *list)
{
	struct private_object_data *po = NULL;
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, TCORE_RETURN_EINVAL);
	po = tcore_object_ref_object(o);
	memcpy(&po->support_list, list, sizeof(struct tel_phonebook_support_list));
	return TRUE;
}

CoreObject *tcore_phonebook_new(TcorePlugin *p, const char *name, struct tcore_phonebook_operations *ops)
{
	CoreObject *o = NULL;
	struct private_object_data *po = NULL;

	if (!p)
		return NULL;

	o = tcore_object_new(p, name);
	if (!o)
		return NULL;

	po = calloc(sizeof(struct private_object_data), 1);
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	po->ops = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_PHONEBOOK);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_phonebook_free(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_PHONEBOOK);

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	free(po);
	tcore_object_free(o);
}
