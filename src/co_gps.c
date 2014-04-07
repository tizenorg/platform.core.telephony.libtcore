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

#include "tcore.h"
#include "plugin.h"
#include "co_gps.h"

typedef struct {
	TcoreGpsOps *ops;
} PrivateObject;

static void _po_free_hook(CoreObject *co)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return(po != NULL);

	tcore_free(po->ops);
	tcore_free(po);
	tcore_object_link_object(co, NULL);
}

static void _po_clone_hook(CoreObject *src, CoreObject *dest)
{
	PrivateObject *dest_po = NULL;
	PrivateObject *src_po = tcore_object_ref_object(src);
	tcore_check_return_assert(src_po != NULL);
	tcore_check_return_assert(src_po->ops != NULL);
	tcore_check_return_assert(dest != NULL);

	dest_po = tcore_malloc0(sizeof(PrivateObject));
	dest_po->ops = tcore_memdup(src_po->ops, sizeof(TcoreGpsOps));

	tcore_object_link_object(dest, dest_po);
}

static TelReturn _dispatcher(CoreObject *co,
	TcoreCommand command, const void *request,
	TcoreObjectResponseCallback cb, const void *user_data)
{
	TcoreGpsOps *gps = NULL;
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, TEL_RETURN_INVALID_PARAMETER);
	tcore_check_return_value_assert(po->ops != NULL, TEL_RETURN_INVALID_PARAMETER);

	gps = po->ops;

	switch (command) {
	case TCORE_COMMAND_GPS_CONFIRM_MEASURE_POS:
		if (gps->confirm_measure_pos)
			return gps->confirm_measure_pos(co,
				(TelGpsDataInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_GPS_SET_FREQUENCY_AIDING:
		if (gps->set_frequency_aiding)
			return gps->set_frequency_aiding(co,
				*(gboolean *)request,
				cb, (void *)user_data);
		break;

	default:
		err("Unsupported Command [0x%x]", command);
		return TEL_RETURN_INVALID_PARAMETER;
	}
	err("Operation NOT Supported");
	return TEL_RETURN_OPERATION_NOT_SUPPORTED;
}

void tcore_gps_override_ops(CoreObject *co, TcoreGpsOps *ops)
{
	PrivateObject *po = tcore_object_ref_object(co);

	tcore_check_return_assert(po != NULL);
	tcore_check_return_assert(po->ops != NULL);
	tcore_check_return_assert(ops != NULL);

	if (ops->confirm_measure_pos)
		po->ops->confirm_measure_pos = ops->confirm_measure_pos;
	if (ops->set_frequency_aiding)
		po->ops->set_frequency_aiding = ops->set_frequency_aiding;
}

gboolean tcore_gps_set_ops(CoreObject *co, TcoreGpsOps *ops)
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
		po->ops = tcore_memdup((gconstpointer)ops, sizeof(TcoreGpsOps));

	return TRUE;
}

CoreObject *tcore_gps_new(TcorePlugin *p,
			TcoreGpsOps *ops, TcoreHal *hal)
{
	CoreObject *co = NULL;
	PrivateObject *po = NULL;
	tcore_check_return_value_assert(p != NULL, NULL);

	co = tcore_object_new(p, hal);
	tcore_check_return_value_assert(co != NULL, NULL);

	po = tcore_malloc0(sizeof(PrivateObject));

	if (ops != NULL)
		po->ops = tcore_memdup(ops, sizeof(TcoreGpsOps));

	tcore_object_set_type(co, CORE_OBJECT_TYPE_GPS);
	tcore_object_link_object(co, po);
	tcore_object_set_free_hook(co, _po_free_hook);
	tcore_object_set_clone_hook(co, _po_clone_hook);
	tcore_object_set_dispatcher(co, _dispatcher);
	return co;
}

void tcore_gps_free(CoreObject *co)
{
	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_GPS);
	tcore_object_free(co);
}
