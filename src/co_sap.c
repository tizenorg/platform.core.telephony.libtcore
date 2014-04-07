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
#include "co_sap.h"

typedef struct {
	TcoreSapOps *ops;
} PrivateObject;

static TelReturn _dispatcher(CoreObject *co,
	TcoreCommand command, const void *request,
	TcoreObjectResponseCallback cb, const void *user_data)
{
	TcoreSapOps *sap = NULL;
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, TEL_RETURN_INVALID_PARAMETER);
	tcore_check_return_value_assert(po->ops != NULL, TEL_RETURN_INVALID_PARAMETER);

	sap = po->ops;

	switch (command) {
	case TCORE_COMMAND_SAP_REQ_CONNECT:
		if (sap->req_connect)
			return sap->req_connect(co,
				*(unsigned int *)request,
				cb, (void*)user_data);
		break;

	case TCORE_COMMAND_SAP_REQ_DISCONNECT:
		if (sap->req_disconnect)
			return sap->req_disconnect(co, cb, (void*)user_data);
		break;

	case TCORE_COMMAND_SAP_GET_ATR:
		if (sap->get_atr)
			return sap->get_atr(co, cb, (void*)user_data);
		break;

	case TCORE_COMMAND_SAP_REQ_TRANSFER_APDU:
		if (sap->req_transfer_apdu)
			return sap->req_transfer_apdu(co,
				(const TelSapApdu *)request,
				cb, (void*)user_data);
		break;

	case TCORE_COMMAND_SAP_REQ_TRANSPORT_PROTOCOL:
		if (sap->req_transport_protocol)
			return sap->req_transport_protocol(co,
				*(TelSimSapProtocol *)request,
				cb, (void*)user_data);
		break;

	case TCORE_COMMAND_SAP_REQ_POWER_OPERATION:
		if (sap->req_power_operation)
			return sap->req_power_operation(co,
				*(TelSapPowerMode *)request,
				cb, (void*)user_data);
		break;

	case TCORE_COMMAND_SAP_GET_CARDREADER_STATUS:
		if (sap->get_cardreader_status)
			return sap->get_cardreader_status(co, cb, (void*)user_data);
		break;

	default:
		err("Unsupported command [0x%x]", command);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	err("Operation not Supported");
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
	dest_po->ops = tcore_memdup(src_po->ops, sizeof(TcoreSapOps));

	tcore_object_link_object(dest, dest_po);
}

static void _po_free_hook(CoreObject *co)
{
	PrivateObject *po = tcore_object_ref_object(co);
 	tcore_check_return(po != NULL);

	tcore_free(po->ops);
	tcore_free(po);
	tcore_object_link_object(co, NULL);
}

void tcore_sap_override_ops(CoreObject *co, TcoreSapOps *ops)
{
	PrivateObject *po = tcore_object_ref_object(co);

	tcore_check_return_assert(po != NULL);
	tcore_check_return_assert(po->ops != NULL);
	tcore_check_return_assert(ops != NULL);

	if (ops->req_connect)
		po->ops->req_connect = ops->req_connect;

	if (ops->req_disconnect)
		po->ops->req_disconnect = ops->req_disconnect;

	if (ops->get_atr)
		po->ops->get_atr = ops->get_atr;

	if (ops->req_transfer_apdu)
		po->ops->req_transfer_apdu = ops->req_transfer_apdu;

	if (ops->req_transport_protocol)
		po->ops->req_transport_protocol = ops->req_transport_protocol;

	if (ops->req_power_operation)
		po->ops->req_power_operation = ops->req_power_operation;

	if (ops->get_cardreader_status)
		po->ops->get_cardreader_status = ops->get_cardreader_status;
}

gboolean tcore_sap_set_ops(CoreObject *co, TcoreSapOps *ops)
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
		po->ops = tcore_memdup((gconstpointer)ops, sizeof(TcoreSapOps));

	return TRUE;
}

CoreObject *tcore_sap_new(TcorePlugin *plugin, TcoreSapOps *ops, TcoreHal *hal)
{
	CoreObject *co = NULL;
	PrivateObject *po = NULL;
	tcore_check_return_value_assert(plugin != NULL, NULL);

	co = tcore_object_new(plugin, hal);
	tcore_check_return_value_assert(co != NULL, NULL);

	po = tcore_malloc0(sizeof(PrivateObject));

	if (ops != NULL)
		po->ops = tcore_memdup(ops, sizeof(TcoreSapOps));

	tcore_object_set_type(co, CORE_OBJECT_TYPE_SAP);
	tcore_object_link_object(co, po);
	tcore_object_set_free_hook(co, _po_free_hook);
	tcore_object_set_clone_hook(co, _po_clone_hook);
	tcore_object_set_dispatcher(co, _dispatcher);

	return co;
}

void tcore_sap_free(CoreObject *co)
{
	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_SAP);
	tcore_object_free(co);
}
