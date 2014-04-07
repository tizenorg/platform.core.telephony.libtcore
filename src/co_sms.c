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
#include "co_sms.h"

typedef struct {
	TcoreSmsOps *ops;
	gboolean ready_status;
} PrivateObject;

static TelReturn _dispatcher(CoreObject *co,
	TcoreCommand command, const void *request,
	TcoreObjectResponseCallback cb, const void *user_data)
{
	TcoreSmsOps *sms = NULL;
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, TEL_RETURN_INVALID_PARAMETER);
	tcore_check_return_value_assert(po->ops != NULL, TEL_RETURN_INVALID_PARAMETER);

	if (!po->ready_status) {
		err("DEVICE_NOT_READY");
		return TEL_RETURN_OPERATION_NOT_SUPPORTED;
	}
	sms = po->ops;

	switch (command) {
	case TCORE_COMMAND_SMS_SEND_SMS:
		if (sms->send_sms)
			return sms->send_sms(co,
				(const TelSmsSendInfo *)request,
				cb,(void *)user_data);
		break;

	case TCORE_COMMAND_SMS_READ_IN_SIM:
		if (sms->read_in_sim)
			return sms->read_in_sim(co,
				*((unsigned int *)request),
				cb,(void *)user_data);
		break;

	case TCORE_COMMAND_SMS_WRITE_IN_SIM:
		if (sms->write_in_sim)
			return sms->write_in_sim(co,
				(const TelSmsSimDataInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SMS_DELETE_IN_SIM:
		if (sms->delete_in_sim)
			return sms->delete_in_sim(co,
				*((unsigned int *)request),
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SMS_GET_COUNT:
		if (sms->get_count)
			return sms->get_count(co, cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SMS_SET_CB_CONFIG:
		if (sms->set_cb_config)
			return sms->set_cb_config(co,
				(const TelSmsCbConfigInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SMS_GET_CB_CONFIG:
		if (sms->get_cb_config)
			return sms->get_cb_config(co, cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SMS_GET_PARAMETERS:
		if (sms->get_parameters)
			return sms->get_parameters(co, cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SMS_SET_PARAMETERS:
		if (sms->set_parameters)
			return sms->set_parameters(co,
				(const TelSmsParamsInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SMS_SEND_DELIVER_REPORT:
		if (sms->send_deliver_report)
			return sms->send_deliver_report(co,
				(const TelSmsDeliverReportInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SMS_SET_SCA:
		if (sms->set_sca)
			return sms->set_sca(co,
				(const TelSmsSca *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SMS_GET_SCA:
		if (sms->get_sca)
			return sms->get_sca(co, cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SMS_SET_MEMORY_STATUS:
		if (sms->set_memory_status)
			return sms->set_memory_status(co,
				*(gboolean *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SMS_SET_MESSAGE_STATUS:
		if (sms->set_message_status)
			return sms->set_message_status(co,
				(const TelSmsStatusInfo *)request,
				cb, (void *)user_data);
		break;

	default:
		err("Unsupported command:[%d]",command);
		return TEL_RETURN_INVALID_PARAMETER;
	}
	err("Operation NOT Supported");
	return TEL_RETURN_OPERATION_NOT_SUPPORTED;
}

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
	dest_po->ops = tcore_memdup(src_po->ops, sizeof(TcoreSmsOps));
	tcore_object_link_object(dest, dest_po);
}

gboolean tcore_sms_get_ready_status(CoreObject *co, gboolean *status)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(status != NULL, FALSE);

	*status = po->ready_status;
	return TRUE;
}

gboolean tcore_sms_set_ready_status(CoreObject *co, gboolean status)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->ready_status = status;
	return TRUE;
}

void tcore_sms_override_ops(CoreObject *co, TcoreSmsOps *ops)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_assert(po != NULL);
	tcore_check_return_assert(po->ops != NULL);
	tcore_check_return_assert(ops != NULL);

	if (ops->send_sms)
		po->ops->send_sms = ops->send_sms;
	if (ops->read_in_sim)
		po->ops->read_in_sim = ops->read_in_sim;
	if (ops->write_in_sim)
		po->ops->write_in_sim = ops->write_in_sim;
	if (ops->delete_in_sim)
		po->ops->delete_in_sim = ops->delete_in_sim;
	if (ops->get_count)
		po->ops->get_count = ops->get_count;
	if (ops->set_cb_config)
		po->ops->set_cb_config = ops->set_cb_config;
	if (ops->get_cb_config)
		po->ops->get_cb_config = ops->get_cb_config;
	if (ops->get_parameters)
		po->ops->get_parameters = ops->get_parameters;
	if (ops->set_parameters)
		po->ops->set_parameters = ops->set_parameters;
	if (ops->send_deliver_report)
		po->ops->send_deliver_report = ops->send_deliver_report;
	if (ops->set_sca)
		po->ops->set_sca = ops->set_sca;
	if (ops->get_sca)
		po->ops->get_sca = ops->get_sca;
	if (ops->set_memory_status)
		po->ops->set_memory_status = ops->set_memory_status;
	if (ops->set_message_status)
		po->ops->set_message_status = ops->set_message_status;
}

gboolean tcore_sms_set_ops(CoreObject *co, TcoreSmsOps *ops)
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
		po->ops = tcore_memdup((gconstpointer)ops, sizeof(TcoreSmsOps));

	return TRUE;
}

CoreObject *tcore_sms_new(TcorePlugin *p,
			TcoreSmsOps *ops, TcoreHal *hal)
{
	CoreObject *co = NULL;
	PrivateObject *po = NULL;
	tcore_check_return_value_assert(p != NULL, NULL);

	co = tcore_object_new(p, hal);
	tcore_check_return_value_assert(co != NULL, NULL);

	po = tcore_malloc0(sizeof(PrivateObject));

	if (ops != NULL)
		po->ops = tcore_memdup(ops, sizeof(TcoreSmsOps));

	tcore_object_set_type(co, CORE_OBJECT_TYPE_SMS);
	tcore_object_link_object(co, po);
	tcore_object_set_free_hook(co, _po_free_hook);
	tcore_object_set_clone_hook(co, _po_clone_hook);
	tcore_object_set_dispatcher(co, _dispatcher);
	return co;
}

void tcore_sms_free(CoreObject *co)
{
	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_SMS);
	tcore_object_free(co);
}
