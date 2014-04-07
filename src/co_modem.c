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
#include "co_modem.h"

typedef struct {
	TcoreModemOps *ops;

	gboolean flight_mode;
	gboolean powered;
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

	dest_po = tcore_malloc0(sizeof(PrivateObject));
	dest_po->ops = tcore_memdup(src_po->ops, sizeof(TcoreModemOps));
	tcore_object_link_object(dest, dest_po);
}

static TelReturn _dispatcher(CoreObject *co,
	TcoreCommand command, const void *request,
	TcoreObjectResponseCallback cb, const void *user_data)
{
	TcoreModemOps *modem = NULL;
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, TEL_RETURN_INVALID_PARAMETER);
	tcore_check_return_value_assert(po->ops != NULL, TEL_RETURN_INVALID_PARAMETER);
	modem = po->ops;

	switch (command) {
	case TCORE_COMMAND_MODEM_SET_POWER_STATUS:
		if (modem->set_power_status)
			return modem->set_power_status(co,
					*((TelModemPowerStatus *)request),
					cb, (void *)user_data);
		break;

	case TCORE_COMMAND_MODEM_SET_FLIGHTMODE:
		if (modem->set_flight_mode)
			return modem->set_flight_mode(co,
					*(gboolean *)request,
					cb, (void *)user_data);
		break;

	case TCORE_COMMAND_MODEM_GET_IMEI:
		if (modem->get_imei)
			return modem->get_imei(co, cb, (void *)user_data);
		break;

	case TCORE_COMMAND_MODEM_GET_VERSION:
		if (modem->get_version)
			return modem->get_version(co, cb, (void *)user_data);
		break;

	case TCORE_COMMAND_MODEM_GET_FLIGHTMODE:
		if (modem->get_flight_mode)
			return modem->get_flight_mode(co, cb, (void *)user_data);
		break;

	default:
		err("Unsupported Command [0x%x]", command);
		break;
	}
	err("Operation NOT Supported");
	return TEL_RETURN_OPERATION_NOT_SUPPORTED;
}

void tcore_modem_override_ops(CoreObject *co, TcoreModemOps *ops)
{
	PrivateObject *po = tcore_object_ref_object(co);

	tcore_check_return_assert(po != NULL);
	tcore_check_return_assert(po->ops != NULL);
	tcore_check_return_assert(ops != NULL);

	if (ops->set_power_status)
		po->ops->set_power_status = ops->set_power_status;
	if (ops->set_flight_mode)
		po->ops->set_flight_mode = ops->set_flight_mode;
	if (ops->get_imei)
		po->ops->get_imei = ops->get_imei;
	if (ops->get_version)
		po->ops->get_version = ops->get_version;
	if (ops->get_flight_mode)
		po->ops->get_flight_mode = ops->get_flight_mode;
}

gboolean tcore_modem_set_ops(CoreObject *co, TcoreModemOps *ops)
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
		po->ops = tcore_memdup((gconstpointer)ops, sizeof(TcoreModemOps));

	return TRUE;
}

CoreObject *tcore_modem_new(TcorePlugin *p,
			TcoreModemOps *ops, TcoreHal *hal)
{
	CoreObject *co = NULL;
	PrivateObject *po = NULL;
	tcore_check_return_value_assert(p != NULL, NULL);

	co = tcore_object_new(p, hal);
	tcore_check_return_value_assert(co != NULL, NULL);

	po = tcore_malloc0(sizeof(PrivateObject));

	if (ops != NULL)
		po->ops = tcore_memdup(ops, sizeof(TcoreModemOps));

	tcore_object_set_type(co, CORE_OBJECT_TYPE_MODEM);
	tcore_object_link_object(co, po);
	tcore_object_set_free_hook(co, _po_free_hook);
	tcore_object_set_clone_hook(co, _po_clone_hook);
	tcore_object_set_dispatcher(co, _dispatcher);
	return co;
}

void tcore_modem_free(CoreObject *co)
{
	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_MODEM);
	tcore_object_free(co);
}

gboolean tcore_modem_set_flight_mode_state(CoreObject *co, gboolean state)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->flight_mode = state;
	return TRUE;
}

gboolean tcore_modem_get_flight_mode_state(CoreObject *co, gboolean *state)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*state = po->flight_mode;
	return TRUE;
}

gboolean tcore_modem_set_powered(CoreObject *co, gboolean pwr)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->powered = pwr;
	return TRUE;
}

gboolean tcore_modem_get_powered(CoreObject *co, gboolean *pwr)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*pwr = po->powered;
	return TRUE;
}
