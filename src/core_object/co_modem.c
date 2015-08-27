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
#include "user_request.h"
#include "co_modem.h"
#include "hal.h"

struct private_object_data {
	struct tcore_modem_operations *ops[TCORE_OPS_TYPE_MAX];

	gboolean flight_mode;
	gboolean powered;
};

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (po) {
		free(po);
		tcore_object_link_object(o, NULL);
	}
}

static TReturn _dispatcher(CoreObject *o, UserRequest *ur, enum tcore_ops_type ops_type)
{
	enum tcore_request_command command;
	struct private_object_data *po = tcore_object_ref_object(o);
	struct tcore_modem_operations *ops = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_MODEM, TCORE_RETURN_EINVAL);
	CORE_OBJECT_VALIDATE_OPS_RETURN_VAL(ops_type, TCORE_RETURN_EINVAL);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_EINVAL);
	tcore_check_null_ret_err("ur", ur, TCORE_RETURN_EINVAL);

	ops = po->ops[ops_type];
	tcore_check_null_ret_err("ops", ops, TCORE_RETURN_FAILURE);

	command = tcore_user_request_get_command(ur);
	switch (command) {
	case TREQ_MODEM_POWER_ON:
		tcore_check_null_ret_err("ops->power_on",
			ops->power_on, TCORE_RETURN_ENOSYS);

		return ops->power_on(o, ur);

	case TREQ_MODEM_POWER_OFF:
		tcore_check_null_ret_err("ops->power_off",
			ops->power_off, TCORE_RETURN_ENOSYS);

		return ops->power_off(o, ur);

	case TREQ_MODEM_POWER_RESET:
		tcore_check_null_ret_err("ops->power_reset",
			ops->power_reset, TCORE_RETURN_ENOSYS);

		return ops->power_reset(o, ur);

	case TREQ_MODEM_POWER_LOW:
		tcore_check_null_ret_err("ops->power_low",
			ops->power_low, TCORE_RETURN_ENOSYS);

		return ops->power_low(o, ur);

	case TREQ_MODEM_SET_FLIGHTMODE:
		tcore_check_null_ret_err("ops->set_flight_mode",
			ops->set_flight_mode, TCORE_RETURN_ENOSYS);

		return ops->set_flight_mode(o, ur);

	case TREQ_MODEM_GET_IMEI:
		tcore_check_null_ret_err("ops->get_imei",
			ops->get_imei, TCORE_RETURN_ENOSYS);

		return ops->get_imei(o, ur);

	case TREQ_MODEM_GET_VERSION:
		tcore_check_null_ret_err("ops->get_version",
			ops->get_version, TCORE_RETURN_ENOSYS);

		return ops->get_version(o, ur);

	case TREQ_MODEM_GET_SN:
		tcore_check_null_ret_err("ops->get_sn",
			ops->get_sn, TCORE_RETURN_ENOSYS);

		return ops->get_sn(o, ur);

	case TREQ_MODEM_SET_DUN_PIN_CONTROL:
		tcore_check_null_ret_err("ops->dun_pin_ctrl",
			ops->dun_pin_ctrl, TCORE_RETURN_ENOSYS);

		return ops->dun_pin_ctrl(o, ur);

	case TREQ_MODEM_GET_FLIGHTMODE:
		tcore_check_null_ret_err("ops->get_flight_mode",
			ops->get_flight_mode, TCORE_RETURN_ENOSYS);

		return ops->get_flight_mode(o, ur);

	case TREQ_MODEM_GET_DEVICE_INFO:
		tcore_check_null_ret_err("ops->get_device_info",
			ops->get_device_info, TCORE_RETURN_ENOSYS);

		return ops->get_device_info(o, ur);

	default:
		return TCORE_RETURN_EINVAL;
	}

	return TCORE_RETURN_SUCCESS;
}

CoreObject *tcore_modem_new(TcorePlugin *p, const char *name,
		struct tcore_modem_operations *ops, TcoreHal *hal)
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

	tcore_object_set_type(o, CORE_OBJECT_TYPE_MODEM);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_modem_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_MODEM);
	tcore_object_free(o);
}

void tcore_modem_set_ops(CoreObject *o, struct tcore_modem_operations *ops, enum tcore_ops_type ops_type)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_MODEM);
	CORE_OBJECT_VALIDATE_OPS_RETURN(ops_type);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		err("po is NULL");
		return;
	}

	po->ops[ops_type] = ops;
}

TReturn tcore_modem_set_flight_mode_state(CoreObject *o, gboolean flag)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_MODEM, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->flight_mode = flag;

	return TCORE_RETURN_SUCCESS;
}

gboolean tcore_modem_get_flight_mode_state(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_MODEM, FALSE);

	po = tcore_object_ref_object(o);
	if (!po)
		return FALSE;

	return po->flight_mode;
}

TReturn tcore_modem_set_powered(CoreObject *o, gboolean pwr)
{
	TcoreHal *h;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_MODEM, TCORE_RETURN_EINVAL);

	h = tcore_object_get_hal(o);
	if (!h)
		return TCORE_RETURN_FAILURE;

	tcore_hal_set_power_state(h, pwr);

	return TCORE_RETURN_SUCCESS;
}

gboolean tcore_modem_get_powered(CoreObject *o)
{
	TcoreHal *h;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_MODEM, FALSE);

	h = tcore_object_get_hal(o);
	if (!h)
		return FALSE;

	return tcore_hal_get_power_state(h);
}
