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
#include <string.h>

#include "tcore.h"
#include "plugin.h"
#include "co_ss.h"

struct ussd_session {
	gboolean session;
	TelSsUssdType type;
	void *data;
	guint data_len;
};

typedef struct {
	UssdSession ussd_s;
	TcoreSsOps *ops;
} PrivateObject;

static TelReturn _dispatcher(CoreObject *co,
	TcoreCommand command, const void *request,
	TcoreObjectResponseCallback cb, const void *user_data)
{
	TcoreSsOps *ss = NULL;
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, TEL_RETURN_INVALID_PARAMETER);
	tcore_check_return_value_assert(po->ops != NULL, TEL_RETURN_INVALID_PARAMETER);

	ss = po->ops;

	switch (command) {
	case TCORE_COMMAND_SS_SET_BARRING:
		if (ss->set_barring)
			return ss->set_barring(co,
				(TelSsBarringInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SS_GET_BARRING_STATUS:
		if (ss->get_barring_status)
			return ss->get_barring_status(co,
				(TelSsBarringGetInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SS_CHANGE_BARRING_PASSWORD:
		if (ss->change_barring_password)
			return ss->change_barring_password(co,
				(TelSsBarringPwdInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SS_SET_FORWARDING:
		if (ss->set_forwarding)
			return ss->set_forwarding(co,
				(TelSsForwardInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SS_GET_FORWARDING_STATUS:
		if (ss->get_forwarding_status)
			return ss->get_forwarding_status(co,
				(TelSsForwardGetInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SS_SET_WAITING:
		if (ss->set_waiting)
			return ss->set_waiting(co,
				(TelSsWaitingInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SS_GET_WAITING_STATUS:
		if (ss->get_waiting_status)
			return ss->get_waiting_status(co,
				*(TelSsClass *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SS_SET_CLI:
		if (ss->set_cli)
			return ss->set_cli(co,
				(TelSsCliInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SS_GET_CLI_STATUS:
		if (ss->get_cli_status)
			return ss->get_cli_status(co,
				*(TelSsCliType *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_SS_SEND_USSD_REQUEST:
		if (ss->send_ussd_request)
			return ss->send_ussd_request(co,
				(TelSsUssdInfo *)request,
				cb, (void *)user_data);
		break;

	default:
		err("Unsupported Command [0x%x]", command);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	err("Operation NOT Supported, Command: [0x%x]", command);
	return TEL_RETURN_OPERATION_NOT_SUPPORTED;
}

static void _po_clone_hook(CoreObject *src, CoreObject *dest)
{
	PrivateObject *dest_po = NULL;
	PrivateObject *src_po = tcore_object_ref_object(src);

	tcore_check_return_assert(src_po != NULL);
	tcore_check_return_assert(src_po->ops != NULL);

	dest_po = tcore_malloc0(sizeof(PrivateObject));
	dest_po->ops = tcore_memdup(src_po->ops, sizeof(TcoreSsOps));
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

UssdSession *tcore_ss_ussd_create_session(CoreObject *co,
	TelSsUssdType type, void *data, guint data_len)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, NULL);

	if (type > TEL_SS_USSD_TYPE_USER_REL) {
		err("USSD type is Invalid");
		return NULL;
	}

	if (!po->ussd_s.session) {
		po->ussd_s.session = TRUE;
		po->ussd_s.type = type;
		po->ussd_s.data_len = data_len;
		po->ussd_s.data = data;

		return &po->ussd_s;
	}

	warn("USSD session already exists, type: [%d]", po->ussd_s.type);
	return NULL;
}

void tcore_ss_ussd_destroy_session(UssdSession *ussd_s)
{
	tcore_check_return_assert(ussd_s != NULL);

	memset(ussd_s, 0x00, sizeof(UssdSession));
}

UssdSession *tcore_ss_ussd_get_session(CoreObject *co)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, NULL);

	if (po->ussd_s.session)
		return &po->ussd_s;

	err("ussd session does not exist");
	return NULL;
}

gboolean tcore_ss_ussd_get_session_type(UssdSession *ussd_s,
	TelSsUssdType *ussd_type)
{
	tcore_check_return_value_assert(ussd_s != NULL, FALSE);
	tcore_check_return_value_assert(ussd_type != NULL, FALSE);

	if (ussd_s->session) {
		*ussd_type = ussd_s->type;
		return TRUE;
	}

	err("ussd session does not exist");
	return FALSE;
}

gboolean tcore_ss_ussd_set_session_type(UssdSession *ussd_s,
				TelSsUssdType type)
{
	tcore_check_return_value_assert(ussd_s != NULL, FALSE);

	if (ussd_s->session) {
		ussd_s->type = type;
		return TRUE;
	}

	err("ussd session does not exist");
	return FALSE;
}

gboolean tcore_ss_ussd_get_session_data(UssdSession *ussd_s,
	void **data, guint *data_len)
{
	tcore_check_return_value_assert(ussd_s != NULL, FALSE);
	tcore_check_return_value_assert(data != NULL, FALSE);
	tcore_check_return_value_assert(data_len != NULL, FALSE);

	if (ussd_s->session) {
		*data = ussd_s->data;
		*data_len = ussd_s->data_len;
		return TRUE;
	}

	err("ussd session does not exist");
	return FALSE;
}

gboolean tcore_ss_ussd_set_session_data(UssdSession *ussd_s,
				void *data, guint data_len)
{
	tcore_check_return_value_assert(ussd_s != NULL, FALSE);

	if (ussd_s->session) {
		ussd_s->data = data;
		ussd_s->data_len = data_len;
		return TRUE;
	}

	err("Session does not exist");
	return FALSE;
}

void tcore_ss_override_ops(CoreObject *co, TcoreSsOps *ops)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_assert(po != NULL);
	tcore_check_return_assert(ops != NULL);
	tcore_check_return_assert(po -> ops != NULL);

	if (ops->set_barring)
		po->ops->set_barring = ops->set_barring;
	if (ops->get_barring_status)
		po->ops->get_barring_status = ops->get_barring_status;
	if (ops->change_barring_password)
		po->ops->change_barring_password = ops->change_barring_password;
	if (ops->set_forwarding)
		po->ops->set_forwarding = ops->set_forwarding;
	if (ops->get_forwarding_status)
		po->ops->get_forwarding_status = ops->get_forwarding_status;
	if (ops->set_waiting)
		po->ops->set_waiting = ops->set_waiting;
	if (ops->get_waiting_status)
		po->ops->get_waiting_status = ops->get_waiting_status;
	if (ops->set_cli)
		po->ops->set_cli = ops->set_cli;
	if (ops->get_cli_status)
		po->ops->get_cli_status = ops->get_cli_status;
	if (ops->send_ussd_request)
		po->ops->send_ussd_request = ops->send_ussd_request;
}

gboolean tcore_ss_set_ops(CoreObject *co, TcoreSsOps *ops)
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
		po->ops = tcore_memdup((gconstpointer)ops, sizeof(TcoreSsOps));

	return TRUE;
}

CoreObject *tcore_ss_new(TcorePlugin *p, TcoreSsOps *ops, TcoreHal *hal)
{
	CoreObject *co = NULL;
	PrivateObject *po = NULL;
	tcore_check_return_value_assert(p != NULL, NULL);

	co = tcore_object_new(p, hal);
	tcore_check_return_value_assert(co != NULL, NULL);

	po = tcore_malloc0(sizeof(PrivateObject));

	if (ops != NULL)
		po->ops = tcore_memdup(ops, sizeof(TcoreSsOps));

	tcore_object_set_type(co, CORE_OBJECT_TYPE_SS);
	tcore_object_link_object(co, po);
	tcore_object_set_free_hook(co, _po_free_hook);
	tcore_object_set_clone_hook(co, _po_clone_hook);
	tcore_object_set_dispatcher(co, _dispatcher);
	return co;
}

void tcore_ss_free(CoreObject *co)
{
	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_SS);
	tcore_object_free(co);
}
