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
#include "co_sap.h"

struct private_object_data {
	struct tcore_sap_operations *ops;
};

static TReturn _dispatcher(CoreObject *o, UserRequest *ur)
{
	enum tcore_request_command command;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_SAP, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po || !po->ops)
		return TCORE_RETURN_ENOSYS;

	command = tcore_user_request_get_command(ur);
	switch (command) {
		case TREQ_SAP_REQ_CONNECT:
			po = tcore_object_ref_object(o);
			if (!po->ops->connect)
				return TCORE_RETURN_ENOSYS;

			return po->ops->connect(o, ur);
			break;

		case TREQ_SAP_REQ_DISCONNECT:
			if (!po->ops->disconnect)
				return TCORE_RETURN_ENOSYS;

			return po->ops->disconnect(o, ur);
			break;

		case TREQ_SAP_REQ_STATUS:
			if (!po->ops->req_status)
				return TCORE_RETURN_ENOSYS;

			return po->ops->req_status(o, ur);
			break;

		case TREQ_SAP_REQ_ATR:
			if (!po->ops->get_atr)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_atr(o, ur);
			break;

		case TREQ_SAP_TRANSFER_APDU:
			if (!po->ops->transfer_apdu)
				return TCORE_RETURN_ENOSYS;

			return po->ops->transfer_apdu(o, ur);
			break;

		case TREQ_SAP_SET_PROTOCOL:
			if (!po->ops->set_transport_protocol)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_transport_protocol(o, ur);
			break;

		case TREQ_SAP_SET_POWER:
			if (!po->ops->set_power)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_power(o, ur);
			break;

		case TREQ_SAP_REQ_CARDREADERSTATUS:
			if (!po->ops->get_cardreader_status)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_cardreader_status(o, ur);
			break;

		default:
			break;
	}
	return TCORE_RETURN_SUCCESS;
}

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

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SAP);

	po = tcore_object_ref_object(o);
	if (po) {
		free(po);
		tcore_object_link_object(o, NULL);
	}
}

CoreObject *tcore_sap_new(TcorePlugin *p, const char *name,
		struct tcore_sap_operations *ops, TcoreHal *hal)
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

	tcore_object_set_type(o, CORE_OBJECT_TYPE_SAP);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_clone_hook(o, _clone_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_sap_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SAP);
	tcore_object_free(o);
}

void tcore_sap_set_ops(CoreObject *o, struct tcore_sap_operations *ops)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SAP);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		return;
	}

	po->ops = ops;
}