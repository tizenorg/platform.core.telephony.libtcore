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
#include "user_request.h"
#include "co_ps.h"
#include "co_network.h"

struct p_callid_type {
	unsigned int cid;
	gboolean active;
	gboolean connected;
	gchar *apn;
	GSList *contexts;
};

struct private_object_data {
	struct tcore_ps_operations *ops[TCORE_OPS_TYPE_MAX];

	gboolean online;
	gint num_of_pdn;

	/* 1 ~ UMTS_PS_MAX_CID */
	struct p_callid_type cid[PS_MAX_CID + 1];

	GSList *context_list;
};

static TReturn _dispatcher(CoreObject *o, UserRequest *ur, enum tcore_ops_type ops_type)
{
	enum tcore_request_command command;
	struct private_object_data *po = tcore_object_ref_object(o);
	struct tcore_ps_operations *ops = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_MODEM, TCORE_RETURN_EINVAL);
	CORE_OBJECT_VALIDATE_OPS_RETURN_VAL(ops_type, TCORE_RETURN_EINVAL);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_EINVAL);
	tcore_check_null_ret_err("ur", ur, TCORE_RETURN_EINVAL);

	ops = po->ops[ops_type];
	tcore_check_null_ret_err("ops", ops, TCORE_RETURN_FAILURE);

	command = tcore_user_request_get_command(ur);
	switch (command) {
	default:
		break;
	}

	return TCORE_RETURN_SUCCESS;
}

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;
	GSList *list;

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	if (po->context_list) {
		for (list = po->context_list; list; list = list->next) {
			if (list->data)
				free(list->data);

			list->data = NULL;
		}

		g_slist_free(po->context_list);
		po->context_list = NULL;
	}

	g_free(po);
	tcore_object_link_object(o, NULL);
}

static gboolean _ps_is_active_context(CoreObject *o, CoreObject *ps_context)
{
	GSList *contexts = NULL;
	CoreObject *s_context = NULL;

	int idx_cid = 0;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, FALSE);

	po = tcore_object_ref_object(o);

	for (idx_cid = 1; idx_cid <= po->num_of_pdn; idx_cid++) {
		if (po->cid[idx_cid].cid == 0)
			continue;

		contexts = po->cid[idx_cid].contexts;
		if (!contexts)
			continue;

		while (contexts) {
			s_context = contexts->data;
			if (!s_context) {
				contexts = contexts->next;
				continue;
			}

			if (ps_context == s_context) {
				dbg("find contexts(%p) in cid(%d)", ps_context, idx_cid);
				return TRUE;
			}

			contexts = contexts->next;
		}
	}

	dbg("cannot find contexts(%p) ", ps_context);

	return FALSE;
}

static gboolean _ps_is_duplicated_apn(CoreObject *o, CoreObject *ps_context)
{
	GSList *contexts = NULL;
	CoreObject *s_context = NULL;
	gchar *t_apn = NULL, *s_apn = NULL;

	int idx_cid = 0;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, FALSE);

	po = tcore_object_ref_object(o);
	t_apn = tcore_context_get_apn(ps_context);

	for (idx_cid = 1; idx_cid <= po->num_of_pdn; idx_cid++) {
		if (po->cid[idx_cid].cid == 0)
			continue;

		contexts = po->cid[idx_cid].contexts;
		if (!contexts)
			continue;

		while (contexts) {
			s_context = contexts->data;
			if (!s_context) {
				contexts = contexts->next;
				continue;
			}

			if (ps_context == s_context) {
				contexts = contexts->next;
				continue;
			}

			s_apn = tcore_context_get_apn(s_context);
			if (g_strcmp0(t_apn, s_apn) == 0) {
				dbg("target and source context have same APN");
				tcore_context_cp_service_info(ps_context, s_context);
				g_free(t_apn);
				g_free(s_apn);

				return TRUE;
			}
			g_free(s_apn);

			contexts = contexts->next;
		}
	}

	g_free(t_apn);
	return FALSE;
}

static void _deactivate_context(gpointer context, gpointer user_data)
{
	if (!context || !user_data)
		return;

	tcore_ps_deactivate_context(user_data, context, NULL);
}

CoreObject *tcore_ps_new(TcorePlugin *p, const char *name,
	struct tcore_ps_operations *ops, TcoreHal *hal)
{
	CoreObject *o = NULL;
	struct private_object_data *po = NULL;

	if (!p)
		return NULL;

	o = tcore_object_new(p, name, hal);
	if (!o)
		return NULL;

	po = g_try_malloc0(sizeof(struct private_object_data));
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	/* set ops to default type when core object is created. */
	po->ops[TCORE_OPS_TYPE_CP] = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_PS);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_ps_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_PS);
	tcore_object_free(o);
}

void tcore_ps_set_ops(CoreObject *o,
	struct tcore_ps_operations *ops, enum tcore_ops_type ops_type)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_PS);
	CORE_OBJECT_VALIDATE_OPS_RETURN(ops_type);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		err("po is NULL");
		return;
	}

	po->ops[ops_type] = ops;
}

TReturn tcore_ps_add_context(CoreObject *o, CoreObject *ctx_o)
{
	struct private_object_data *po = NULL;
	unsigned int count;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);
	CORE_OBJECT_CHECK_RETURN(ctx_o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	count = g_slist_length(po->context_list);
	po->context_list = g_slist_insert(po->context_list, ctx_o, 0);
	dbg("num. of contexts: %d -> %d", count, g_slist_length(po->context_list));

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_ps_remove_context(CoreObject *o, CoreObject *ctx_o)
{
	struct private_object_data *po = NULL;
	unsigned int count;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);
	CORE_OBJECT_CHECK_RETURN(ctx_o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	tcore_ps_clear_context_id(o, ctx_o);
	count = g_slist_length(po->context_list);
	po->context_list = g_slist_remove(po->context_list, ctx_o);
	dbg("num. of contexts: %d -> %d", count, g_slist_length(po->context_list));

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_ps_set_online(CoreObject *o, gboolean state)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->online = state;
	dbg("PS Status: [%s]", (po->online ? "ONLINE" : "OFFLINE"));

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_ps_set_num_of_pdn(CoreObject *o, gint numbers)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (numbers <= 0 || numbers > PS_MAX_CID)
		po->num_of_pdn = PS_MAX_CID;
	else
		po->num_of_pdn = numbers;

	dbg("ps num of pdn = %d, numbers = %d", po->num_of_pdn, numbers);

	return TCORE_RETURN_SUCCESS;
}

unsigned int tcore_ps_get_num_of_pdn(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, 0);

	po = tcore_object_ref_object(o);
	if (!po)
		return 0;

	return po->num_of_pdn;
}

unsigned int tcore_ps_set_cid_active(CoreObject *o, unsigned int cid, unsigned int enable)
{
	int idx_cid = 0;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, 0);
	if (cid == 0)
		return 0;

	po = tcore_object_ref_object(o);
	for (idx_cid = 1; idx_cid <= po->num_of_pdn; idx_cid++) {
		if (po->cid[idx_cid].cid == cid) {
			po->cid[idx_cid].active = enable;
			return 1;
		}
	}

	return 0;
}

unsigned int tcore_ps_get_cid_active(CoreObject *o, unsigned int cid)
{
	int idx_cid = 0;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, 0);
	if (cid == 0)
		return 0;

	po = tcore_object_ref_object(o);
	for (idx_cid = 1; idx_cid <= po->num_of_pdn; idx_cid++)
		if (po->cid[idx_cid].cid == cid)
			return po->cid[idx_cid].active;

	return 0;
}

GSList *tcore_ps_get_active_cids(CoreObject *o)
{
	int idx_cid = 0;
	GSList *active_list = NULL;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, NULL);

	po = tcore_object_ref_object(o);

	for (idx_cid = 1; idx_cid <= po->num_of_pdn; idx_cid++)
		if (po->cid[idx_cid].active)
			active_list = g_slist_append(active_list, &po->cid[idx_cid].cid);

	return active_list;
}

unsigned int tcore_ps_set_cid_connected(CoreObject *o, unsigned int cid, unsigned int connected)
{
	int idx_cid = 0;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, 0);
	if (cid == 0)
		return 0;

	po = tcore_object_ref_object(o);
	for (idx_cid = 1; idx_cid <= po->num_of_pdn; idx_cid++) {
		if (po->cid[idx_cid].cid == cid) {
			po->cid[idx_cid].connected = connected;
			return 1;
		}
	}

	return 0;
}

unsigned int tcore_ps_get_cid_connected(CoreObject *o, unsigned int cid)
{
	int idx_cid = 0;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, 0);
	if (cid == 0)
		return 0;

	po = tcore_object_ref_object(o);
	for (idx_cid = 1; idx_cid <= po->num_of_pdn; idx_cid++)
		if (po->cid[idx_cid].cid == cid)
			return po->cid[idx_cid].connected;

	return 0;
}

GSList *tcore_ps_get_connected_cids(CoreObject *o)
{
	int idx_cid = 0;
	GSList *active_list = NULL;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, NULL);

	po = tcore_object_ref_object(o);

	for (idx_cid = 1; idx_cid <= po->num_of_pdn; idx_cid++)
		if (po->cid[idx_cid].connected)
			active_list = g_slist_append(active_list, &po->cid[idx_cid].cid);

	return active_list;
}

unsigned int tcore_ps_is_active_apn(CoreObject *o, const char *apn)
{
	int idx_cid = 0;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, 0);

	po = tcore_object_ref_object(o);

	for (idx_cid = 1; idx_cid <= po->num_of_pdn; idx_cid++) {
		if (po->cid[idx_cid].cid == 0)
			continue;

		if (g_strcmp0((const char *)po->cid[idx_cid].apn, apn) == 0 && po->cid[idx_cid].active)
			return 1;
	}

	return 0;
}

CoreObject *tcore_ps_ref_context_by_role(CoreObject *o, enum co_context_role role)
{
	struct private_object_data *po = NULL;
	GSList *list;
	CoreObject *pdp_o;
	TcorePlugin *p;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, NULL);

	po = tcore_object_ref_object(o);
	if (!po)
		return NULL;

	p = tcore_object_ref_plugin(o);
	if (!p)
		return NULL;

	if (po->context_list) {
		for (list = po->context_list; list; list = list->next) {
			if (!list->data)
				continue;

			pdp_o = list->data;
			if (!pdp_o)
				continue;

			if (tcore_object_get_type(pdp_o) != CORE_OBJECT_TYPE_PS_CONTEXT)
				continue;

			if (tcore_context_get_role(pdp_o) == role)
				return pdp_o;
		}
	}

	return NULL;
}

GSList *tcore_ps_ref_context_by_id(CoreObject *o, unsigned int id)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, NULL);

	po = tcore_object_ref_object(o);
	if (!po)
		return NULL;

	if (id == 0 || id > (unsigned int)po->num_of_pdn)
		return NULL;

	if (po->cid[id].cid != id)
		return NULL;

	return po->cid[id].contexts;
}

gboolean tcore_ps_any_context_activating_activated(CoreObject *o, int * state)
{
	struct private_object_data *po = NULL;
	CoreObject *pdp_o;
	gboolean ret = FALSE;
	GSList *list = NULL;
	enum co_context_state context_state = CONTEXT_STATE_DEACTIVATED;

	po = tcore_object_ref_object(o);
	if (!po)
		return ret;

	if (po->context_list) {
		for (list = po->context_list; list; list = list->next) {
			if (!list->data)
				continue;

			pdp_o = list->data;

			if (tcore_object_get_type(pdp_o) != CORE_OBJECT_TYPE_PS_CONTEXT)
				continue;

			context_state = tcore_context_get_state(pdp_o);

			if (CONTEXT_STATE_ACTIVATED == context_state) {
				*state = CONTEXT_STATE_ACTIVATED;
				return  TRUE;
			} else if (CONTEXT_STATE_ACTIVATING == context_state) {
				*state = CONTEXT_STATE_ACTIVATING;
				ret = TRUE;
				continue;
			} else if (CONTEXT_STATE_DEACTIVATING == context_state) {
				*state = CONTEXT_STATE_DEACTIVATING;
				ret = TRUE;
				continue;
			}
		}
	}

	return ret;
}


TReturn tcore_ps_assign_context_id(CoreObject *o, CoreObject *context, unsigned char cid)
{
	struct private_object_data *po = NULL;
	unsigned char idx;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);
	CORE_OBJECT_CHECK_RETURN(context, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (cid > (unsigned int)po->num_of_pdn)
		return TCORE_RETURN_PS_CID_ERROR;

	if (cid == 0) {
		/* Automatic assign */
		for (idx = 1; idx <= po->num_of_pdn; idx++) {
			if (po->cid[idx].cid == 0) {
				po->cid[idx].cid = idx;
				po->cid[idx].contexts = g_slist_append(po->cid[idx].contexts, context);
				po->cid[idx].apn = tcore_context_get_apn(context);

				dbg("assign contexts(%p) in cid(%d), apn(%s)", context, idx, po->cid[idx].apn);

				return tcore_context_set_id(context, idx);
			} else
				dbg("cid[%d] is not null", idx);
		}

		dbg("can't find empty cid");
	} else {
		/* Manual assign */
		if (po->cid[cid].cid == cid) {
			po->cid[cid].contexts = g_slist_append(po->cid[cid].contexts, context);
			return tcore_context_set_id(context, cid);
		} else
			dbg("cid[%d] is not null", cid);
	}

	return TCORE_RETURN_PS_CID_ERROR;
}

TReturn tcore_ps_send_dormant_request(CoreObject *o, void *user_data)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (!po->online) {
		dbg("ps network is not online !");
		return TCORE_RETURN_PS_NETWORK_NOT_READY;
	}

	/* By 'default' considering Modem/CP opearations */
	return po->ops[TCORE_OPS_TYPE_CP]->send_dormant_request(o, user_data);
}

TReturn tcore_ps_clear_context_id(CoreObject *o, CoreObject *context)
{
	struct private_object_data *po = NULL;
	unsigned char i = 0, cnt = 0;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);
	CORE_OBJECT_CHECK_RETURN(context, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	i = tcore_context_get_id(context);
	if (i == 0)
		return TCORE_RETURN_PS_CID_ERROR;

	if (i > po->num_of_pdn)
		return TCORE_RETURN_PS_CID_ERROR;

	po->cid[i].contexts = g_slist_remove(po->cid[i].contexts, context);
	cnt = g_slist_length(po->cid[i].contexts);
	if (cnt <= 0) {
		po->cid[i].cid = 0;
		po->cid[i].active = FALSE;
		po->cid[i].connected = FALSE;
		g_free(po->cid[i].apn);
		po->cid[i].apn = NULL;
	}

	return tcore_context_set_id(context, 0);
}

TReturn tcore_ps_define_context(CoreObject *o, CoreObject *ps_context, void *user_data)
{
	int rv;
	struct private_object_data *po = NULL;
	CoreObject *co_nw = NULL;
	enum telephony_network_access_technology act = NETWORK_ACT_UNKNOWN;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (!ps_context)
		return TCORE_RETURN_EINVAL;

	rv = _ps_is_active_context(o, ps_context);
	if (rv)
		return TCORE_RETURN_SUCCESS;

	co_nw = tcore_plugin_ref_core_object(tcore_object_ref_plugin(o), CORE_OBJECT_TYPE_NETWORK);
	tcore_network_get_access_technology(co_nw, &act);
	if (act >= NETWORK_ACT_IS95A && act <= NETWORK_ACT_EHRPD)
		tcore_context_set_tech_preference(ps_context, CONTEXT_TECH_3GPP2);
	else {
		tcore_context_set_tech_preference(ps_context, CONTEXT_TECH_3GPP);
		dbg("3GPP preference by default");
	}

	rv = _ps_is_duplicated_apn(o, ps_context);
	if (rv) {
		unsigned char cid = 0;
		cid = tcore_context_get_id(ps_context);
		po->cid[cid].contexts = g_slist_append(po->cid[cid].contexts, ps_context);
		return TCORE_RETURN_SUCCESS;
	}

	if (tcore_context_get_id(ps_context) == 0)
		if (tcore_ps_assign_context_id(o, ps_context, 0) != TCORE_RETURN_SUCCESS)
			return TCORE_RETURN_PS_CID_ERROR;

	dbg("contexts(%p), cid = %d", ps_context, tcore_context_get_id(ps_context));

	/* By 'default' considering Modem/CP opearations */
	return po->ops[TCORE_OPS_TYPE_CP]->define_context(o, ps_context, user_data);
}

TReturn tcore_ps_activate_context(CoreObject *o, CoreObject *ps_context, void *user_data)
{
	int rv;
	struct private_object_data *po = NULL;
	enum co_context_state context_state = CONTEXT_STATE_DEACTIVATED;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (!po->online) {
		dbg("ps network is not online !");
		return TCORE_RETURN_PS_NETWORK_NOT_READY;
	}

	if (!ps_context)
		return TCORE_RETURN_EINVAL;

	rv = _ps_is_active_context(o, ps_context);
	if (!rv) {
		dbg("it is not defined context");
		return TCORE_RETURN_EINVAL;
	}

	rv = _ps_is_duplicated_apn(o, ps_context);
	if (rv) {
		dbg("context activation is already requested for the same apn(%s)",
			tcore_context_get_apn(ps_context));
		return TCORE_RETURN_SUCCESS;
	}

	context_state = tcore_context_get_state(ps_context);

	if (context_state == CONTEXT_STATE_ACTIVATED) {
		dbg("Context state : CONTEXT_STATE_ACTIVATED");
		return TCORE_RETURN_SUCCESS;
	} else if (context_state == CONTEXT_STATE_ACTIVATING) {
		dbg("Context state : CONTEXT_STATE_ACTIVATING");
		return TCORE_RETURN_SUCCESS;
	} else if (context_state == CONTEXT_STATE_DEACTIVATING) {
		dbg("Context state : CONTEXT_STATE_DEACTIVATING");
		return TCORE_RETURN_PS_DEACTIVATING;
	}

	dbg("cid = %d", tcore_context_get_id(ps_context));

	tcore_context_set_state(ps_context, CONTEXT_STATE_ACTIVATING);

	/* By 'default' considering Modem/CP opearations */
	rv = po->ops[TCORE_OPS_TYPE_CP]->activate_context(o, ps_context, user_data);
	if (rv != TCORE_RETURN_SUCCESS)
		tcore_context_set_state(ps_context, CONTEXT_STATE_DEACTIVATED);

	return rv;
}

TReturn tcore_ps_deactivate_context(CoreObject *o, CoreObject *ps_context, void *user_data)
{
	int rv;
	struct private_object_data *po = NULL;
	enum co_context_state context_state = CONTEXT_STATE_DEACTIVATED;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (!po->online) {
		dbg("ps network is not online !");
		return TCORE_RETURN_PS_NETWORK_NOT_READY;
	}

	if (!ps_context)
		return TCORE_RETURN_EINVAL;

	rv = _ps_is_active_context(o, ps_context);
	if (!rv)
		return TCORE_RETURN_EINVAL;

	rv = _ps_is_duplicated_apn(o, ps_context);
	if (rv) {
		unsigned char cid = 0;
		cid = tcore_context_get_id(ps_context);
		po->cid[cid].contexts = g_slist_remove(po->cid[cid].contexts, ps_context);
		tcore_context_set_state(ps_context, CONTEXT_STATE_DEACTIVATED);
		return TCORE_RETURN_SUCCESS;
	}

	context_state = tcore_context_get_state(ps_context);
	if (context_state == CONTEXT_STATE_DEACTIVATED) {
		warn("Context State : CONTEXT_STATE_DEACTIVATED");
		tcore_ps_clear_context_id(o, ps_context);
		return TCORE_RETURN_SUCCESS;
	} else if (context_state == CONTEXT_STATE_DEACTIVATING) {
		dbg("Context State : CONTEXT_STATE_DEACTIVATING");
		return TCORE_RETURN_SUCCESS;
	} else if (context_state == CONTEXT_STATE_ACTIVATING) {
		dbg("Context State :CONTEXT_STATE_ACTIVATING");
		return TCORE_RETURN_PS_ACTIVATING;
	}

	tcore_context_set_state(ps_context, CONTEXT_STATE_DEACTIVATING);

	/* By 'default' considering Modem/CP opearations */
	rv = po->ops[TCORE_OPS_TYPE_CP]->deactivate_context(o, ps_context, user_data);
	if (rv != TCORE_RETURN_SUCCESS)
		tcore_context_set_state(ps_context, CONTEXT_STATE_ACTIVATED);

	return rv;
}

TReturn tcore_ps_deactivate_contexts(CoreObject *o)
{
	int temp_index = 0;
	struct private_object_data *po = NULL;
	GSList *contexts = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (!po->online) {
		dbg("ps network is not online !");
		return TCORE_RETURN_PS_NETWORK_NOT_READY;
	}

	for (temp_index = 1; temp_index <= po->num_of_pdn; temp_index++) {
		if (po->cid[temp_index].cid != 0) {
			contexts = po->cid[temp_index].contexts;
			if (!contexts)
				continue;

			g_slist_foreach(contexts, _deactivate_context, o);
		}
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_ps_deactivate_cid(CoreObject *o, unsigned int cid)
{
	int temp_index = 0;
	struct private_object_data *po = NULL;
	GSList *contexts = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (!po->online) {
		dbg("ps network is not online !");
		return TCORE_RETURN_PS_NETWORK_NOT_READY;
	}

	for (temp_index = 1; temp_index <= po->num_of_pdn; temp_index++) {
		if (po->cid[temp_index].cid != 0 && po->cid[temp_index].cid == cid) {
			contexts = po->cid[temp_index].contexts;
			if (!contexts)
				continue;

			g_slist_foreach(contexts, _deactivate_context, o);
		}
	}

	return TCORE_RETURN_SUCCESS;
}
