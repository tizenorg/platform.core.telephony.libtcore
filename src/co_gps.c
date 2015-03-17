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

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (po) {
		free(po);
		tcore_object_link_object(o, NULL);
	}
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

		case TREQ_GPS_SET_FREQUENCY_AIDING:
			dbg("TREQ_GPS_SET_FREQUENCY_AIDING");
			if (!po->ops->set_frequency_aiding)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_frequency_aiding(o, ur);

		case TREQ_ENABLE_SMART_ASSISTANT:
			dbg("TREQ_ENABLE_SMART_ASSISTANT");
			if (!po->ops->enable_smart_assistant)
				return TCORE_RETURN_ENOSYS;

			return po->ops->enable_smart_assistant(o, ur);

		case TREQ_DISABLE_SMART_ASSISTANT:
			dbg("TREQ_DISABLE_SMART_ASSISTANT");
			if (!po->ops->disable_smart_assistant)
				return TCORE_RETURN_ENOSYS;

			return po->ops->disable_smart_assistant(o, ur);

		case TREQ_SYNC_SMART_ASSISTANT_AREA_LIST:
			dbg("TREQ_SYNC_SMART_ASSISTANT_AREA_LIST");
			if (!po->ops->sync_smart_assistant_area_list)
				return TCORE_RETURN_ENOSYS;

			return po->ops->sync_smart_assistant_area_list(o, ur);

		case TREQ_DEL_SMART_ASSISTANT_AREA_LIST:
			dbg("TREQ_DEL_SMART_ASSISTANT_AREA_LIST");
			if (!po->ops->del_smart_assistant_area_list)
				return TCORE_RETURN_ENOSYS;

			return po->ops->del_smart_assistant_area_list(o, ur);

		case TREQ_ADD_SMART_ASSISTANT_AREA:
			dbg("TREQ_ADD_SMART_ASSISTANT_AREA");
			if (!po->ops->add_smart_assistant_area)
				return TCORE_RETURN_ENOSYS;

			return po->ops->add_smart_assistant_area(o, ur);

		case TREQ_MODIFY_SMART_ASSISTANT_AREA:
			dbg("TREQ_MODIFY_SMART_ASSISTANT_AREA");
			if (!po->ops->modify_smart_assistant_area)
				return TCORE_RETURN_ENOSYS;

			return po->ops->modify_smart_assistant_area(o, ur);

		case TREQ_SET_SMART_ASSISTANT_INFO:
			dbg("TREQ_SET_SMART_ASSISTANT_INFO");
			if (!po->ops->set_smart_assistant_info)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_smart_assistant_info(o, ur);

		default:
			dbg("not supported cmd");
			break;
	}
	return TCORE_RETURN_SUCCESS;
}

CoreObject *tcore_gps_new(TcorePlugin *p, const char *name,
		struct tcore_gps_operations *ops, TcoreHal *hal)
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

	po->ops = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_GPS);
	tcore_object_link_object(o, po);
	tcore_object_set_dispatcher(o, _dispatcher);
	tcore_object_set_free_hook(o, _free_hook);

	return o;
}

void tcore_gps_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_GPS);

	tcore_object_free(o);
}

void tcore_gps_set_ops(CoreObject *o, struct tcore_gps_operations *ops)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_GPS);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		return;
	}

	po->ops = ops;
}
