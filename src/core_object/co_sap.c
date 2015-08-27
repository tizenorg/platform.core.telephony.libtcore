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
#include "co_sap.h"

struct private_object_data {
	struct tcore_sap_operations *ops[TCORE_OPS_TYPE_MAX];
};

static TReturn _dispatcher(CoreObject *o, UserRequest *ur, enum tcore_ops_type ops_type)
{
	enum tcore_request_command command;
	struct private_object_data *po = tcore_object_ref_object(o);
	struct tcore_sap_operations *ops = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_SAP, TCORE_RETURN_EINVAL);
	CORE_OBJECT_VALIDATE_OPS_RETURN_VAL(ops_type, TCORE_RETURN_EINVAL);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_EINVAL);
	tcore_check_null_ret_err("ur", ur, TCORE_RETURN_EINVAL);

	ops = po->ops[ops_type];
	tcore_check_null_ret_err("ops", ops, TCORE_RETURN_FAILURE);

	command = tcore_user_request_get_command(ur);
	switch (command) {
	case TREQ_SAP_REQ_CONNECT:
		tcore_check_null_ret_err("ops->connect",
			ops->connect, TCORE_RETURN_ENOSYS);

		return ops->connect(o, ur);

	case TREQ_SAP_REQ_DISCONNECT:
		tcore_check_null_ret_err("ops->disconnect",
			ops->disconnect, TCORE_RETURN_ENOSYS);

		return ops->disconnect(o, ur);

	case TREQ_SAP_REQ_STATUS:
		tcore_check_null_ret_err("ops->req_status",
			ops->req_status, TCORE_RETURN_ENOSYS);

		return ops->req_status(o, ur);

	case TREQ_SAP_REQ_ATR:
		tcore_check_null_ret_err("ops->get_atr",
			ops->get_atr, TCORE_RETURN_ENOSYS);

		return ops->get_atr(o, ur);

	case TREQ_SAP_TRANSFER_APDU:
		tcore_check_null_ret_err("ops->transfer_apdu",
			ops->transfer_apdu, TCORE_RETURN_ENOSYS);

		return ops->transfer_apdu(o, ur);

	case TREQ_SAP_SET_PROTOCOL:
		tcore_check_null_ret_err("ops->set_transport_protocol",
			ops->set_transport_protocol, TCORE_RETURN_ENOSYS);

		return ops->set_transport_protocol(o, ur);

	case TREQ_SAP_SET_POWER:
		tcore_check_null_ret_err("ops->set_power",
			ops->set_power, TCORE_RETURN_ENOSYS);

		return ops->set_power(o, ur);

	case TREQ_SAP_REQ_CARDREADERSTATUS:
		tcore_check_null_ret_err("ops->get_cardreader_status",
			ops->get_cardreader_status, TCORE_RETURN_ENOSYS);

		return ops->get_cardreader_status(o, ur);

	default:
		break;
	}

	return TCORE_RETURN_SUCCESS;
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

	/* set ops to default type when core object is created. */
	po->ops[TCORE_OPS_TYPE_CP] = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_SAP);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_sap_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SAP);
	tcore_object_free(o);
}

void tcore_sap_set_ops(CoreObject *o, struct tcore_sap_operations *ops, enum tcore_ops_type ops_type)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SAP);
	CORE_OBJECT_VALIDATE_OPS_RETURN(ops_type);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		err("po is NULL");
		return;
	}

	po->ops[ops_type] = ops;
}
