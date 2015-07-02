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
#include "co_ss.h"

struct ussd_session {
	gboolean session;
	enum tcore_ss_ussd_type type;
	void *data;
	int data_len;
};

struct ss_routing_policy {
	enum tcore_ss_routing_policy ss_routing_policy;
	enum tcore_ussd_routing_policy ussd_routing_policy;
};

struct private_object_data {
	struct tcore_ss_operations *ops[TCORE_OPS_TYPE_MAX];

	struct ussd_session ussd_s;
	struct ss_routing_policy routing_policy;
};

static TReturn _dispatcher(CoreObject *o, UserRequest *ur, enum tcore_ops_type ops_type)
{
	enum tcore_request_command command;
	struct private_object_data *po = tcore_object_ref_object(o);
	struct tcore_ss_operations *ops = NULL;
	TReturn ret = 0;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_SS, TCORE_RETURN_EINVAL);
	CORE_OBJECT_VALIDATE_OPS_RETURN_VAL(ops_type, TCORE_RETURN_EINVAL);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_EINVAL);
	tcore_check_null_ret_err("ur", ur, TCORE_RETURN_EINVAL);

	ops = po->ops[ops_type];
	tcore_check_null_ret_err("ops", ops, TCORE_RETURN_FAILURE);

	command = tcore_user_request_get_command(ur);
	switch (command) {
	case TREQ_SS_BARRING_ACTIVATE:
		tcore_check_null_ret_err("ops->barring_activate",
			ops->barring_activate, TCORE_RETURN_ENOSYS);

		ret = ops->barring_activate(o, ur);
	break;

	case TREQ_SS_BARRING_DEACTIVATE:
		tcore_check_null_ret_err("ops->barring_deactivate",
			ops->barring_deactivate, TCORE_RETURN_ENOSYS);

		ret = ops->barring_deactivate(o, ur);
	break;

	case TREQ_SS_BARRING_CHANGE_PASSWORD:
		tcore_check_null_ret_err("ops->barring_change_password",
			ops->barring_change_password, TCORE_RETURN_ENOSYS);

		ret = ops->barring_change_password(o, ur);
	break;

	case TREQ_SS_BARRING_GET_STATUS:
		tcore_check_null_ret_err("ops->barring_get_status",
			ops->barring_get_status, TCORE_RETURN_ENOSYS);

		ret = ops->barring_get_status(o, ur);
	break;

	case TREQ_SS_FORWARDING_ACTIVATE:
		tcore_check_null_ret_err("ops->forwarding_activate",
			ops->forwarding_activate, TCORE_RETURN_ENOSYS);

		ret = ops->forwarding_activate(o, ur);
	break;

	case TREQ_SS_FORWARDING_DEACTIVATE:
		tcore_check_null_ret_err("ops->forwarding_deactivate",
			ops->forwarding_deactivate, TCORE_RETURN_ENOSYS);

		ret = ops->forwarding_deactivate(o, ur);
	break;

	case TREQ_SS_FORWARDING_REGISTER:
		tcore_check_null_ret_err("ops->forwarding_register",
			ops->forwarding_register, TCORE_RETURN_ENOSYS);

		ret = ops->forwarding_register(o, ur);
	break;

	case TREQ_SS_FORWARDING_DEREGISTER:
		tcore_check_null_ret_err("ops->forwarding_deregister",
			ops->forwarding_deregister, TCORE_RETURN_ENOSYS);

		ret = ops->forwarding_deregister(o, ur);
	break;

	case TREQ_SS_FORWARDING_GET_STATUS:
		tcore_check_null_ret_err("ops->forwarding_get_status",
			ops->forwarding_get_status, TCORE_RETURN_ENOSYS);

		ret = ops->forwarding_get_status(o, ur);
	break;

	case TREQ_SS_WAITING_ACTIVATE:
		tcore_check_null_ret_err("ops->waiting_activate",
			ops->waiting_activate, TCORE_RETURN_ENOSYS);

		ret = ops->waiting_activate(o, ur);
	break;

	case TREQ_SS_WAITING_DEACTIVATE:
		tcore_check_null_ret_err("ops->waiting_deactivate",
			ops->waiting_deactivate, TCORE_RETURN_ENOSYS);

		ret = ops->waiting_deactivate(o, ur);
	break;

	case TREQ_SS_WAITING_GET_STATUS:
		tcore_check_null_ret_err("ops->waiting_get_status",
			ops->waiting_get_status, TCORE_RETURN_ENOSYS);

		ret = ops->waiting_get_status(o, ur);
	break;

	case TREQ_SS_CLI_ACTIVATE:
		tcore_check_null_ret_err("ops->cli_activate",
			ops->cli_activate, TCORE_RETURN_ENOSYS);

		ret = ops->cli_activate(o, ur);
	break;

	case TREQ_SS_CLI_DEACTIVATE:
		tcore_check_null_ret_err("ops->cli_deactivate",
			ops->cli_deactivate, TCORE_RETURN_ENOSYS);

		ret = ops->cli_deactivate(o, ur);
	break;

	case TREQ_SS_CLI_SET_STATUS:
		tcore_check_null_ret_err("ops->cli_set_status",
			ops->cli_set_status, TCORE_RETURN_ENOSYS);

		ret = ops->cli_set_status(o, ur);
	break;

	case TREQ_SS_CLI_GET_STATUS:
		tcore_check_null_ret_err("ops->cli_get_status",
			ops->cli_get_status, TCORE_RETURN_ENOSYS);

		ret = ops->cli_get_status(o, ur);
	break;

	case TREQ_SS_SEND_USSD:
		tcore_check_null_ret_err("ops->send_ussd",
			ops->send_ussd, TCORE_RETURN_ENOSYS);

		ret = ops->send_ussd(o, ur);
	break;

	case TREQ_SS_SET_AOC:
		tcore_check_null_ret_err("ops->set_aoc",
			ops->set_aoc, TCORE_RETURN_ENOSYS);

		ret = ops->set_aoc(o, ur);
	break;

	case TREQ_SS_GET_AOC:
		tcore_check_null_ret_err("ops->get_aoc",
			ops->get_aoc, TCORE_RETURN_ENOSYS);

		ret = ops->get_aoc(o, ur);
	break;

	default:
	break;
	}

	return ret;
}

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	free(po);
	tcore_object_link_object(o, NULL);
}

static void _ussd_session_init(struct ussd_session *ussd_s)
{
	ussd_s->session = FALSE;
	ussd_s->type = 0;
	ussd_s->data = 0;
	ussd_s->data_len = 0;
}

struct ussd_session *tcore_ss_ussd_create_session(CoreObject *o,
		enum tcore_ss_ussd_type type, void *data, int data_len)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po)
		return 0;

	if (type < TCORE_SS_USSD_TYPE_USER_INITIATED
			|| type > TCORE_SS_USSD_TYPE_NETWORK_INITIATED) {
		dbg("[ error ] wrong ussd type : (0x%x)", type);
		return 0;
	}

	if (!po->ussd_s.session) {
		po->ussd_s.session = TRUE;
		po->ussd_s.type = type;
		po->ussd_s.data = data;

		if (data_len < 0)
			po->ussd_s.data_len = 0;
		else
			po->ussd_s.data_len = data_len;

		return &po->ussd_s;

	} else {
		dbg("[ error ] already exist ussd session, type : (0x%x)", po->ussd_s.type);
		return 0;
	}
}

void tcore_ss_ussd_destroy_session(struct ussd_session *ussd_s)
{
	if (!ussd_s || !ussd_s->session)
		return;

	_ussd_session_init(ussd_s);
}

struct ussd_session *tcore_ss_ussd_get_session(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po)
		return 0;

	if (!po->ussd_s.session)
		return 0;
	else
		return &po->ussd_s;
}

enum tcore_ss_ussd_type tcore_ss_ussd_get_session_type(struct ussd_session *ussd_s)
{
	if (!ussd_s || !ussd_s->session) {
		dbg("[ error ] there is no session");
		return 0;
	}

	return ussd_s->type;
}

void tcore_ss_ussd_set_session_type(struct ussd_session *ussd_s,
		enum tcore_ss_ussd_type type)
{
	if (!ussd_s || !ussd_s->session) {
		dbg("[ error ] there is no session");
		return;
	}

	ussd_s->type = type;
}

int tcore_ss_ussd_get_session_data(struct ussd_session *ussd_s, void **data)
{
	if (!ussd_s || !ussd_s->session) {
		dbg("[ error ] there is no session");
		return -1;
	}

	*data = ussd_s->data;
	return ussd_s->data_len;
}

void tcore_ss_ussd_set_session_data(struct ussd_session *ussd_s, void *data, int data_len)
{
	if (!ussd_s || !ussd_s->session) {
		dbg("[ error ] there is no session");
		return;
	}

	ussd_s->data = data;
	ussd_s->data_len = data_len;
}

void tcore_ss_set_ussd_routing(CoreObject *o, enum tcore_ussd_routing_policy ussd_routing_policy)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	po->routing_policy.ussd_routing_policy = ussd_routing_policy;
}

enum tcore_ussd_routing_policy tcore_ss_get_ussd_routing(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_SS_USSD_ROUTING_POLICY_CS_ALWAYS; /* 'default' to CS */

	return po->routing_policy.ussd_routing_policy;
}

void tcore_ss_set_ss_routing(CoreObject *o, enum tcore_ss_routing_policy ss_routing_policy)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	po->routing_policy.ss_routing_policy = ss_routing_policy;
}

enum tcore_ss_routing_policy tcore_ss_get_ss_routing(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_SS_ROUTING_POLICY_CS_ALWAYS; /* 'default' to CS */

	return po->routing_policy.ss_routing_policy;
}

CoreObject *tcore_ss_new(TcorePlugin *p, const char *name,
		struct tcore_ss_operations *ops, TcoreHal *hal)
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
	po->routing_policy.ss_routing_policy = TCORE_SS_ROUTING_POLICY_CS_ALWAYS; /* 'default' to CS */
	po->routing_policy.ussd_routing_policy = TCORE_SS_USSD_ROUTING_POLICY_CS_ALWAYS; /* 'default' to CS */

	_ussd_session_init(&po->ussd_s);

	tcore_object_set_type(o, CORE_OBJECT_TYPE_SS);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_ss_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SS);

	tcore_object_free(o);
}

void tcore_ss_set_ops(CoreObject *o, struct tcore_ss_operations *ops, enum tcore_ops_type ops_type)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SS);
	CORE_OBJECT_VALIDATE_OPS_RETURN(ops_type);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		err("po is NULL");
		return;
	}

	po->ops[ops_type] = ops;
}

