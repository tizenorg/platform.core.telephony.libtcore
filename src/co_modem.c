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
#include "user_request.h"
#include "co_modem.h"
#include "hal.h"

struct private_object_data {
	struct tcore_modem_operations *ops;

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

static void _clone_hook(CoreObject *src, CoreObject *dest)
{
	struct private_object_data *src_po = NULL;
	struct private_object_data *dest_po = NULL;

	if (!src || !dest)
		return;

	dest_po = calloc(sizeof(struct private_object_data), 1);
	if (!dest_po) {
		tcore_object_link_object(dest, NULL);
		return;
	}

	src_po = tcore_object_ref_object(src);
	dest_po->ops = src_po->ops;

	tcore_object_link_object(dest, dest_po);
}

static TReturn _dispatcher(CoreObject *o, UserRequest *ur)
{
	enum tcore_request_command command;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_MODEM, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po || !po->ops)
		return TCORE_RETURN_ENOSYS;

	command = tcore_user_request_get_command(ur);
	switch (command) {
		case TREQ_MODEM_POWER_ON:
			if (!po->ops->power_on)
				return TCORE_RETURN_ENOSYS;

			return po->ops->power_on(o, ur);
			break;

		case TREQ_MODEM_POWER_OFF:
			if (!po->ops->power_off)
				return TCORE_RETURN_ENOSYS;

			return po->ops->power_off(o, ur);
			break;

		case TREQ_MODEM_POWER_RESET:
			if (!po->ops->power_reset)
				return TCORE_RETURN_ENOSYS;

			return po->ops->power_reset(o, ur);
			break;

		case TREQ_MODEM_SET_FLIGHTMODE:
			if (!po->ops->set_flight_mode)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_flight_mode(o, ur);
			break;

		case TREQ_MODEM_GET_IMEI:
			if (!po->ops->get_imei)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_imei(o, ur);
			break;

		case TREQ_MODEM_GET_VERSION:
			if (!po->ops->get_version)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_version(o, ur);
			break;

		case TREQ_MODEM_GET_SN:
			if (!po->ops->get_sn)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_sn(o, ur);
			break;

		case TREQ_MODEM_SET_DUN_PIN_CONTROL:
			if (!po->ops->dun_pin_ctrl)
				return TCORE_RETURN_ENOSYS;

			return po->ops->dun_pin_ctrl(o, ur);
			break;

		case TREQ_MODEM_GET_FLIGHTMODE:
			if (!po->ops->get_flight_mode)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_flight_mode(o, ur);
			break;

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

	po = calloc(sizeof(struct private_object_data), 1);
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	po->ops = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_MODEM);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_clone_hook(o, _clone_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_modem_free(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_MODEM);

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	free(po);
	tcore_object_free(o);
}

TReturn tcore_modem_set_flight_mode_state(CoreObject *o, gboolean flag)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_MODEM, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->flight_mode = flag;

	return TRUE;
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

	return TRUE;
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
