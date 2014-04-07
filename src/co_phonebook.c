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

#include <glib.h>
#include <string.h>

#include "tcore.h"
#include "plugin.h"
#include "co_phonebook.h"

typedef struct {
	TcorePbOps *ops;
	gboolean init;
	TelPbList support_list;
	TelPbType pb_type;
} PrivateObject;

static TelReturn _dispatcher(CoreObject *co,
	TcoreCommand command, const void *request,
	TcoreObjectResponseCallback cb, const void *user_data)
{
	TcorePbOps *phonebook = NULL;
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, TEL_RETURN_INVALID_PARAMETER);
	tcore_check_return_value_assert(po->ops != NULL, TEL_RETURN_INVALID_PARAMETER);
	phonebook = po->ops;

	switch (command) {
	case TCORE_COMMAND_PHONEBOOK_GET_INFO:
		if (phonebook->get_info)
			return phonebook->get_info(co,
				*(TelPbType *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_PHONEBOOK_READ_RECORD:
		if (phonebook->read_record)
			return phonebook->read_record(co,
				(TelPbRecordInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_PHONEBOOK_UPDATE_RECORD:
		if (phonebook->update_record)
			return phonebook->update_record(co,
				(TelPbUpdateRecord *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_PHONEBOOK_DELETE_RECORD:
		if (phonebook->delete_record)
			return phonebook->delete_record(co,
				(TelPbRecordInfo *)request,
				cb, (void *)user_data);
		break;

	default:
		err("Unsupported Command:[%d]",command);
		return TEL_RETURN_INVALID_PARAMETER;
	}
	err("Operation NOT supported");
	return TEL_RETURN_OPERATION_NOT_SUPPORTED;
}

static void _po_clone_hook(CoreObject *src, CoreObject *dest)
{
	PrivateObject *dest_po = NULL;
	PrivateObject *src_po = tcore_object_ref_object(src);

	tcore_check_return_assert(src_po != NULL);
	tcore_check_return_assert(src_po->ops != NULL);
	tcore_check_return_assert(dest != NULL);

	dest_po = tcore_malloc0(sizeof(PrivateObject));
	dest_po->ops = tcore_memdup(src_po->ops, sizeof(TcorePbOps));
	tcore_object_link_object(dest, dest_po);
}

static void _po_free_hook(CoreObject *co)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_assert(po != NULL);

	tcore_free(po->ops);
	tcore_free(po);
	tcore_object_link_object(co, NULL);
}

void tcore_phonebook_override_ops(CoreObject *co, TcorePbOps *ops)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_assert(po != NULL);
	tcore_check_return_assert(po->ops != NULL);
	tcore_check_return_assert(ops != NULL);

	if (ops->get_info)
		po->ops->get_info = ops->get_info;
	if (ops->read_record)
		po->ops->read_record = ops->read_record;
	if (ops->update_record)
		po->ops->update_record = ops->update_record;
	if (ops->delete_record)
		po->ops->delete_record = ops->delete_record;
}

gboolean tcore_phonebook_get_status(CoreObject *co, gboolean *init_status)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(init_status != NULL, FALSE);

	 *init_status = po->init;
	 return TRUE;
}

gboolean tcore_phonebook_set_status(CoreObject *co, gboolean init_status)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->init = init_status;
	return TRUE;
}

gboolean tcore_phonebook_get_support_list(CoreObject *co, TelPbList **pb_list)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(pb_list != NULL, FALSE);

	*pb_list = tcore_memdup(&po->support_list, sizeof(TelPbList));
	return TRUE;
}

gboolean tcore_phonebook_set_support_list(CoreObject *co, TelPbList *pb_list)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	memcpy(&po->support_list, pb_list, sizeof(TelPbList));
	return TRUE;
}

gboolean tcore_phonebook_get_selected_type(CoreObject *co, TelPbType *pb_type)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(pb_type != NULL, FALSE);

	*pb_type = po->pb_type;
	return TRUE;
}

gboolean tcore_phonebook_set_selected_type(CoreObject *co, TelPbType pb_type)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->pb_type = pb_type;
	return TRUE;
}

gboolean tcore_phonebook_set_ops(CoreObject *co, TcorePbOps *ops)
{
	PrivateObject *po;
	tcore_check_return_value(co != NULL, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	if (po->ops != NULL) {
		tcore_free(po->ops);
		po->ops = NULL;
	}

	if (ops != NULL)
		po->ops = tcore_memdup((gconstpointer)ops, sizeof(TcorePbOps));

	return TRUE;
}

CoreObject *tcore_phonebook_new(TcorePlugin *p, TcorePbOps *ops, TcoreHal *hal)
{
	CoreObject *co = NULL;
	PrivateObject *po = NULL;
	tcore_check_return_value_assert(p != NULL, NULL);

	co = tcore_object_new(p, hal);
	tcore_check_return_value_assert(co != NULL, NULL);

	po = tcore_malloc0(sizeof(PrivateObject));

	if (ops != NULL)
		po->ops = tcore_memdup(ops, sizeof(TcorePbOps));

	tcore_object_set_type(co, CORE_OBJECT_TYPE_PHONEBOOK);
	tcore_object_link_object(co, po);
	tcore_object_set_free_hook(co, _po_free_hook);
	tcore_object_set_clone_hook(co, _po_clone_hook);
	tcore_object_set_dispatcher(co, _dispatcher);
	return co;
}

void tcore_phonebook_free(CoreObject *co)
{
	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_PHONEBOOK);
	tcore_object_free(co);
}
