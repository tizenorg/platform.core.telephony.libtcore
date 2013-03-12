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
#include "co_gps.h"

struct private_object_data {
	struct tcore_gps_operations *ops;
};

static void _clone_hook(CoreObject *src, CoreObject *dest)
{
	struct private_object_data *src_po = NULL;
	struct private_object_data *dest_po = NULL;

	if (!src || !dest)
		return;

	dest_po = calloc(1, sizeof(struct private_object_data));
	if (!dest_po) {
		tcore_object_link_object(dest, NULL);
		return;
	}

	src_po = tcore_object_ref_object(src);
	dest_po->ops = src_po->ops;

	tcore_object_link_object(dest, dest_po);
}

static void _clone_gps_operations(struct private_object_data *po, struct tcore_gps_operations *gps_ops)
{
	if(gps_ops->confirm_measure_pos) {
		po->ops->confirm_measure_pos = gps_ops->confirm_measure_pos;
	}

	return;
}

static TReturn _dispatcher(CoreObject *o, UserRequest *ur)
{
	enum tcore_request_command command;
	struct private_object_data *po = NULL;

	if (!o || !ur)
		return TCORE_RETURN_EINVAL;

	po = tcore_object_ref_object(o);
	if (!po || !po->ops)
		return TCORE_RETURN_ENOSYS;

	command = tcore_user_request_get_command(ur);
	switch (command) {
		case TREQ_GPS_CONFIRM_MEASURE_POS:
			dbg("TREQ_GPS_CONFIRM_MEASURE_POS");
			if (!po->ops->confirm_measure_pos)
				return TCORE_RETURN_ENOSYS;

			return po->ops->confirm_measure_pos(o, ur);
			break;

		case TREQ_GPS_SET_FREQUENCY_AIDING:
			dbg("TREQ_GPS_SET_FREQUENCY_AIDING");
			if (!po->ops->set_frequency_aiding)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_frequency_aiding(o, ur);
			break;

		default:
			dbg("not supported cmd");
			break;
	}
	return TCORE_RETURN_SUCCESS;
}

void tcore_gps_override_ops(CoreObject *o, struct tcore_gps_operations *gps_ops)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_GPS);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		return;
	}

	if(gps_ops) {
		_clone_gps_operations(po, gps_ops);
	}

	return;
}

CoreObject *tcore_gps_new(TcorePlugin *p,
			struct tcore_gps_operations *ops, TcoreHal *hal)
{
	CoreObject *o = NULL;
	struct private_object_data *po = NULL;

	if (!p)
		return NULL;

	o = tcore_object_new(p, hal);
	if (!o)
		return NULL;

	po = calloc(1, sizeof(struct private_object_data));
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	po->ops = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_GPS);
	tcore_object_link_object(o, po);
	tcore_object_set_dispatcher(o, _dispatcher);
	tcore_object_set_clone_hook(o, _clone_hook);

	return o;
}

void tcore_gps_free(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_GPS);

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	g_free(po);
	tcore_object_link_object(o, po);
	tcore_object_free(o);
}

