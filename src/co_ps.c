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
#include "co_ps.h"

struct p_callid_type{
	unsigned int cid;
	GSList *contexts;
};

struct private_object_data {
	struct tcore_ps_operations *ops;

	gboolean online;

	/* 1 ~ UMTS_PS_MAX_CID */
	struct p_callid_type cid[PS_MAX_CID + 1];
	//CoreObject *cid[PS_MAX_CID + 1];

	GSList *context_list;
};

static void _clone_ps_operations(struct private_object_data *po, struct tcore_ps_operations *ps_ops)
{
	if(ps_ops->activate_context) {
		po->ops->activate_context = ps_ops->activate_context;
	}
	if(ps_ops->deactivate_context) {
		po->ops->deactivate_context = ps_ops->deactivate_context;
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

	free(po);
	tcore_object_link_object(o, NULL);
}

static gboolean _ps_is_active_context(CoreObject *o, CoreObject *ps_context)
{
	GSList *contexts = NULL;
	CoreObject *s_context = NULL;
	int idx_cid = 0;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);

	for (idx_cid = 0; idx_cid < PS_MAX_CID; idx_cid++) {
		if (po->cid[idx_cid].cid != 0) {
			contexts = po->cid[idx_cid].contexts;
			if (contexts == NULL)
				continue;

			for (; contexts != NULL; contexts = g_slist_next(contexts)) {
				s_context = contexts->data;
				if (s_context == NULL)
					continue;

				if (ps_context == s_context) {
					dbg("find contexts(%p) in cid(%d)",
							ps_context, idx_cid);
					return TRUE;
				}
			}
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

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	t_apn = tcore_context_get_apn(ps_context);

	for (idx_cid = 0; idx_cid < PS_MAX_CID; idx_cid++) {
		if (po->cid[idx_cid].cid != 0) {
			contexts = po->cid[idx_cid].contexts;
			if (contexts == NULL)
				continue;

			for (; contexts != NULL; contexts = g_slist_next(contexts)) {
				s_context = contexts->data;
				if (s_context == NULL)
					continue;

				if (ps_context == s_context)
					continue;

				s_apn = tcore_context_get_apn(s_context);

				if (g_strcmp0(t_apn, s_apn) == 0) {
					dbg("target and source have same APN");
					tcore_context_cp_service_info(
							ps_context, s_context);
					g_free(t_apn);
					g_free(s_apn);
					return TRUE;
				}

				g_free(s_apn);
			}
		}
	}

	g_free(t_apn);
	return FALSE;
}

void tcore_ps_override_ops(CoreObject *o, struct tcore_ps_operations *ps_ops)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_PS);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		return;
	}

	if(ps_ops) {
		_clone_ps_operations(po, ps_ops);
	}

	return;
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

	po = calloc(1, sizeof(struct private_object_data));
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	po->ops = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_PS);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_clone_hook(o, _clone_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

CoreObject *tcore_ps_clone(TcorePlugin *p, const char *name, TcoreHal *hal)
{
	CoreObject *o = NULL;

	if (!p)
		return NULL;

	o = tcore_object_clone_template_object(p, name, CORE_OBJECT_TYPE_PS);
	if (!o)
		return NULL;

	tcore_object_set_hal(o, hal);

	return o;
}

void tcore_ps_free(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_PS);

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	g_free(po);
	tcore_object_link_object(o, po);
	tcore_object_free(o);
}

TReturn tcore_ps_add_context(CoreObject *o, CoreObject *ctx_o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);
	CORE_OBJECT_CHECK_RETURN(ctx_o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->context_list = g_slist_insert(po->context_list, ctx_o, 0);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_ps_remove_context(CoreObject *o, CoreObject *ctx_o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);
	CORE_OBJECT_CHECK_RETURN(ctx_o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	tcore_ps_clear_context_id(o, ctx_o);
	po->context_list = g_slist_remove(po->context_list, ctx_o);

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
	dbg("ps status = %d", po->online);

	return TCORE_RETURN_SUCCESS;
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

	if (id == 0 || id > PS_MAX_CID)
		return NULL;

	if (po->cid[id].cid != id)
		return NULL;

	return po->cid[id].contexts;
}

TReturn tcore_ps_assign_context_id(CoreObject *o, CoreObject *context, unsigned int cid)
{
	struct private_object_data *po = NULL;
	int idx;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);
	CORE_OBJECT_CHECK_RETURN(context, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (cid == 0) {
		/* Automatic assign */
		for (idx = 1; idx <= PS_MAX_CID; idx++) {
			if (po->cid[idx].cid == 0) {
				po->cid[idx].cid = idx;
				po->cid[idx].contexts = g_slist_append(po->cid[idx].contexts, context);
				dbg("assign contexts(%p) in cid(%d)", context, idx);
				return tcore_context_set_id(context, idx);
			}
			else {
				dbg("cid[%d] is not null", idx);
			}
		}

		dbg("can't find empty cid");
	}
	else {
		/* Manual assign */
		if (po->cid[cid].cid == cid) {
			po->cid[cid].contexts = g_slist_append(po->cid[cid].contexts, context);
			return tcore_context_set_id(context, cid);
		}
		else {
			dbg("cid[%d] is not null", cid);
		}
	}

	return TCORE_RETURN_PS_CID_ERROR;
}

TReturn tcore_ps_clear_context_id(CoreObject *o, CoreObject *context)
{
	struct private_object_data *po = NULL;
	int i = 0, cnt = 0;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);
	CORE_OBJECT_CHECK_RETURN(context, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	i = tcore_context_get_id(context);
	if (i == 0) {
		return TCORE_RETURN_PS_CID_ERROR;
	}

	if (i > PS_MAX_CID)
		return TCORE_RETURN_PS_CID_ERROR;

	po->cid[i].contexts = g_slist_remove(po->cid[i].contexts, context);
	cnt = g_slist_length(po->cid[i].contexts);
	if (cnt <= 0)
		po->cid[i].cid = 0;

	return tcore_context_set_id(context, 0);
}

TReturn tcore_ps_define_context(CoreObject *o, CoreObject *ps_context, void *user_data)
{
	int rv;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (!ps_context)
		return TCORE_RETURN_EINVAL;

	rv = _ps_is_active_context(o, ps_context);
	if (rv)
		return TCORE_RETURN_SUCCESS;

	rv = _ps_is_duplicated_apn(o, ps_context);
	if (rv) {
		int cid = 0;
		cid = tcore_context_get_id(ps_context);
		po->cid[cid].contexts = g_slist_append(po->cid[cid].contexts, ps_context);
		return TCORE_RETURN_SUCCESS;
	}

	if (tcore_context_get_id(ps_context) == 0) {
		if (tcore_ps_assign_context_id(o, ps_context, 0) != TCORE_RETURN_SUCCESS)
			return TCORE_RETURN_PS_CID_ERROR;
	}

	dbg("contexts(%p), cid = %d", ps_context, tcore_context_get_id(ps_context));

	return po->ops->define_context(o, ps_context, user_data);
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
	if (!rv)
	{
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

	if (context_state == CONTEXT_STATE_ACTIVATED)
		return TCORE_RETURN_SUCCESS;
	else if (context_state == CONTEXT_STATE_ACTIVATING)
		return TCORE_RETURN_SUCCESS;
	else if (context_state == CONTEXT_STATE_DEACTIVATING)
		return TCORE_RETURN_PS_DEACTIVATING;


	dbg("cid = %d", tcore_context_get_id(ps_context));

	tcore_context_set_state(ps_context, CONTEXT_STATE_ACTIVATING);
	return po->ops->activate_context(o, ps_context, user_data);
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
		int cid = 0;
		cid = tcore_context_get_id(ps_context);
		po->cid[cid].contexts = g_slist_remove(po->cid[cid].contexts, ps_context);
		tcore_context_set_state(ps_context, CONTEXT_STATE_DEACTIVATED);
		return TCORE_RETURN_SUCCESS;
	}

	context_state = tcore_context_get_state(ps_context);
	if (context_state == CONTEXT_STATE_DEACTIVATED)
		return TCORE_RETURN_SUCCESS;
	else if (context_state == CONTEXT_STATE_DEACTIVATING)
		return TCORE_RETURN_SUCCESS;
	else if (context_state == CONTEXT_STATE_ACTIVATING)
		return TCORE_RETURN_PS_ACTIVATING;

	tcore_context_set_state(ps_context, CONTEXT_STATE_DEACTIVATING);
	return po->ops->deactivate_context(o, ps_context, user_data);
}

TReturn tcore_ps_deactivate_contexts(CoreObject *o)
{
	GSList *contexts = NULL;
	CoreObject *context = NULL;
	int index = 0;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (!po->online) {
		dbg("ps network is not online !");
		return TCORE_RETURN_PS_NETWORK_NOT_READY;
	}

	for (index = 0; index < PS_MAX_CID; index++) {
		if (po->cid[index].cid != 0) {
			contexts = po->cid[index].contexts;
			if (contexts == NULL)
				continue;

			for (; contexts != NULL; contexts = g_slist_next(contexts)) {
				context = contexts->data;
				if (context == NULL)
					continue;

				tcore_ps_deactivate_context(o, context, NULL);
			}
		}
	}

	return TCORE_RETURN_SUCCESS;
}
