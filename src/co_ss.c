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
#include "co_ss.h"

#define _check_null( name, value, err ) { \
	if ( !value ) { \
		dbg("[error] %s : NULL", name ); \
		return err; \
	} \
}

struct ussd_session {
	gboolean session;
	enum tcore_ss_ussd_type type;
	void* data;
	int data_len;
};

struct private_object_data {
	struct ussd_session ussd_s;
	struct tcore_ss_operations *ops;
};

static TReturn _dispatcher(CoreObject *o, UserRequest *ur)
{
	enum tcore_request_command command;
	TReturn ret = 0;

	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->ops", po->ops, TCORE_RETURN_FAILURE);
	_check_null( "ur", ur, TCORE_RETURN_FAILURE);

	command = tcore_user_request_get_command(ur);
	switch (command) {
		case TREQ_SS_BARRING_ACTIVATE:
			ret = po->ops->barring_activate(o, ur);
			break;

		case TREQ_SS_BARRING_DEACTIVATE:
			ret = po->ops->barring_deactivate(o, ur);
			break;

		case TREQ_SS_BARRING_CHANGE_PASSWORD:
			ret = po->ops->barring_change_password(o, ur);
			break;

		case TREQ_SS_BARRING_GET_STATUS:
			ret = po->ops->barring_get_status(o, ur);
			break;

		case TREQ_SS_FORWARDING_ACTIVATE:
			ret = po->ops->forwarding_activate(o, ur);
			break;

		case TREQ_SS_FORWARDING_DEACTIVATE:
			ret = po->ops->forwarding_deactivate(o, ur);
			break;

		case TREQ_SS_FORWARDING_REGISTER:
			ret = po->ops->forwarding_register(o, ur);
			break;

		case TREQ_SS_FORWARDING_DEREGISTER:
			ret = po->ops->forwarding_deregister(o, ur);
			break;

		case TREQ_SS_FORWARDING_GET_STATUS:
			ret = po->ops->forwarding_get_status(o, ur);
			break;

		case TREQ_SS_WAITING_ACTIVATE:
			ret = po->ops->waiting_activate(o, ur);
			break;

		case TREQ_SS_WAITING_DEACTIVATE:
			ret = po->ops->waiting_deactivate(o, ur);
			break;

		case TREQ_SS_WAITING_GET_STATUS:
			ret = po->ops->waiting_get_status(o, ur);
			break;

		case TREQ_SS_CLI_ACTIVATE:
			ret = po->ops->cli_activate(o, ur);
			break;

		case TREQ_SS_CLI_DEACTIVATE:
			ret = po->ops->cli_deactivate(o, ur);
			break;

		case TREQ_SS_CLI_GET_STATUS:
			ret = po->ops->cli_get_status(o, ur);
			break;

		case TREQ_SS_SEND_USSD:
			ret = po->ops->send_ussd(o, ur);
			break;

		case TREQ_SS_SET_AOC:
			ret = po->ops->set_aoc(o, ur);
			break;

		case TREQ_SS_GET_AOC:
			ret = po->ops->get_aoc(o, ur);
			break;

		default:
			break;
	}

	return ret;
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

struct ussd_session* tcore_ss_ussd_create_session(CoreObject *o,
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

	}
	else {
		dbg("[ error ] already exist ussd session, type : (0x%x)", po->ussd_s.type);
		return 0;
	}
}

void tcore_ss_ussd_destroy_session(struct ussd_session *ussd_s)
{
	if (!ussd_s || !ussd_s->session) {
		return;
	}
	else {

		_ussd_session_init(ussd_s);
	}
}

struct ussd_session* tcore_ss_ussd_get_session(CoreObject *o)
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
	else {
		return ussd_s->type;
	}
}

void tcore_ss_ussd_set_session_type(struct ussd_session *ussd_s,
		enum tcore_ss_ussd_type type)
{
	if (!ussd_s || !ussd_s->session) {
		dbg("[ error ] there is no session");
		return;

	}
	else {
		ussd_s->type = type;
	}
}

int tcore_ss_ussd_get_session_data(struct ussd_session* ussd_s, void **data)
{
	if (!ussd_s || !ussd_s->session) {
		dbg("[ error ] there is no session");
		return -1;

	}
	else {

		*data = ussd_s->data;
		return ussd_s->data_len;
	}
}

void tcore_ss_ussd_set_session_data(struct ussd_session* ussd_s, void *data, int data_len)
{
	if (!ussd_s || !ussd_s->session) {
		dbg("[ error ] there is no session");
		return ;

	}
	else {

		ussd_s->data = data;
		ussd_s->data_len = data_len;
	}
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

	po = calloc(sizeof(struct private_object_data), 1);
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	po->ops = ops;

	_ussd_session_init(&po->ussd_s);

	tcore_object_set_type(o, CORE_OBJECT_TYPE_SS);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_clone_hook(o, _clone_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_ss_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SS);

	tcore_object_free(o);
}
