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
#include "internal/tcore_types.h"
#include "plugin.h"
#include "queue.h"
#include "user_request.h"
#include "core_object.h"
#include "co_phonebook.h"

struct private_object_data {
	struct tcore_phonebook_operations *ops[TCORE_OPS_TYPE_MAX];

	gboolean b_init;
	struct tel_phonebook_support_list support_list;
	struct tel_phonebook_field_support_list field_support_list;
	enum tel_phonebook_type selected;
};

static TReturn _dispatcher(CoreObject *o, UserRequest *ur, enum tcore_ops_type ops_type)
{
	enum tcore_request_command command;
	struct private_object_data *po = tcore_object_ref_object(o);
	struct tcore_phonebook_operations *ops = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, TCORE_RETURN_EINVAL);
	CORE_OBJECT_VALIDATE_OPS_RETURN_VAL(ops_type, TCORE_RETURN_EINVAL);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_EINVAL);
	tcore_check_null_ret_err("ur", ur, TCORE_RETURN_EINVAL);

	ops = po->ops[ops_type];
	tcore_check_null_ret_err("ops", ops, TCORE_RETURN_FAILURE);

	command = tcore_user_request_get_command(ur);
	switch (command) {
	case TREQ_PHONEBOOK_GETCOUNT:
		tcore_check_null_ret_err("ops->get_count",
			ops->get_count, TCORE_RETURN_ENOSYS);

		return ops->get_count(o, ur);

	case TREQ_PHONEBOOK_GETMETAINFO:
		tcore_check_null_ret_err("ops->get_info",
			ops->get_info, TCORE_RETURN_ENOSYS);

		return ops->get_info(o, ur);

	case TREQ_PHONEBOOK_GETUSIMINFO:
		tcore_check_null_ret_err("ops->get_usim_info",
			ops->get_usim_info, TCORE_RETURN_ENOSYS);

		return ops->get_usim_info(o, ur);

	case TREQ_PHONEBOOK_READRECORD:
		tcore_check_null_ret_err("ops->read_record",
			ops->read_record, TCORE_RETURN_ENOSYS);

		return ops->read_record(o, ur);

	case TREQ_PHONEBOOK_UPDATERECORD:
		tcore_check_null_ret_err("ops->update_record",
			ops->update_record, TCORE_RETURN_ENOSYS);

		return ops->update_record(o, ur);

	case TREQ_PHONEBOOK_DELETERECORD:
		tcore_check_null_ret_err("ops->delete_record",
			ops->delete_record, TCORE_RETURN_ENOSYS);

		return ops->delete_record(o, ur);

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
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, FALSE);
	po = tcore_object_ref_object(o);

	return po->b_init;
}

gboolean tcore_phonebook_set_status(CoreObject *o, gboolean b_init)
{
	struct private_object_data *po = NULL;
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, FALSE);
	po = tcore_object_ref_object(o);

	po->b_init = b_init;

	return TRUE;
}

struct tel_phonebook_support_list *tcore_phonebook_get_support_list(CoreObject *o)
{
	struct private_object_data *po = NULL;
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, NULL);
	po = tcore_object_ref_object(o);

	if (po == NULL)
		return NULL;

	return g_memdup(&po->support_list, sizeof(struct tel_phonebook_support_list));
}

gboolean tcore_phonebook_set_support_list(CoreObject *o, struct tel_phonebook_support_list *list)
{
	struct private_object_data *po = NULL;
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, FALSE);
	po = tcore_object_ref_object(o);

	memcpy(&po->support_list, list, sizeof(struct tel_phonebook_support_list));

	return TRUE;
}

struct tel_phonebook_field_support_list *tcore_phonebook_get_field_support_list(CoreObject *o)
{
	struct private_object_data *po = NULL;
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, NULL);
	po = tcore_object_ref_object(o);

	if (po == NULL)
		return NULL;

	return g_memdup(&po->field_support_list, sizeof(struct tel_phonebook_field_support_list));
}

gboolean tcore_phonebook_set_field_support_list(CoreObject *o, struct tel_phonebook_field_support_list *list)
{
	struct private_object_data *po = NULL;
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, FALSE);
	po = tcore_object_ref_object(o);

	memcpy(&po->field_support_list, list, sizeof(struct tel_phonebook_field_support_list));

	return TRUE;
}

enum tel_phonebook_type tcore_phonebook_get_selected_type(CoreObject *o)
{
	struct private_object_data *po = NULL;
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, PB_TYPE_UNKNOWNN);
	po = tcore_object_ref_object(o);

	return po->selected;
}

gboolean tcore_phonebook_set_selected_type(CoreObject *o, enum tel_phonebook_type t)
{
	struct private_object_data *po = NULL;
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PHONEBOOK, FALSE);
	po = tcore_object_ref_object(o);

	po->selected = t;

	return TRUE;
}

CoreObject *tcore_phonebook_new(TcorePlugin *p, const char *name,
	struct tcore_phonebook_operations *ops, TcoreHal *hal)
{
	CoreObject *o = NULL;
	struct private_object_data *po = NULL;

	if (!p)
		return NULL;

	o = tcore_object_new(p, name, hal);
	if (!o)
		return NULL;

	po = calloc(1, sizeof(struct private_object_data));
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	/* set ops to default type when core object is created. */
	po->ops[TCORE_OPS_TYPE_CP] = ops;

	po->selected = PB_TYPE_UNKNOWNN;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_PHONEBOOK);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_phonebook_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_PHONEBOOK);
	tcore_object_free(o);
}

void tcore_phonebook_set_ops(CoreObject *o, struct tcore_phonebook_operations *ops, enum tcore_ops_type ops_type)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_PHONEBOOK);
	CORE_OBJECT_VALIDATE_OPS_RETURN(ops_type);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		err("po is NULL");
		return;
	}

	po->ops[ops_type] = ops;
}
