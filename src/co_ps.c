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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "co_ps.h"

typedef struct {
	guint cid;
	GSList *contexts;
} TcorePsContextInfo;

typedef struct {
	TcorePsOps *ops;

	gboolean online;

	TcorePsContextInfo context_id[TCORE_PS_MAX_CID + 1];
	GSList *context_list;
} PrivateObject;

static TelReturn __ps_define_context(CoreObject *o, CoreObject *ps_context,
	TcoreObjectResponseCallback cb, void *user_data);
static TelReturn __ps_activate_context(CoreObject *o, CoreObject *ps_context,
	TcoreObjectResponseCallback cb, void *user_data);
static TelReturn __ps_deactivate_context(CoreObject *o, CoreObject *ps_context,
	TcoreObjectResponseCallback cb, void *user_data);

static TelReturn __dispatcher(CoreObject *co,
	TcoreCommand command, const void *request,
	TcoreObjectResponseCallback cb, const void *user_data)
{
	TcorePsOps *ps = NULL;
	CoreObject **co_context;
	PrivateObject *po = tcore_object_ref_object(co);

	tcore_check_return_value_assert(po != NULL, TEL_RETURN_INVALID_PARAMETER);
	tcore_check_return_value_assert(po->ops != NULL, TEL_RETURN_INVALID_PARAMETER);

	ps = po->ops;
	co_context = (CoreObject **)request;

	switch (command) {
	case TCORE_COMMAND_PS_DEFINE_CONTEXT:
		if (ps->define_context)
			return __ps_define_context(co, *co_context, cb, (void *)user_data);

	case TCORE_COMMAND_PS_ACTIVATE_CONTEXT:
		if (ps->activate_context)
			return __ps_activate_context (co, *co_context, cb, (void *)user_data);

	case TCORE_COMMAND_PS_DEACTIVATE_CONTEXT:
		if (ps->deactivate_context)
			return __ps_deactivate_context (co, *co_context, cb, (void *)user_data);

	default:
		err("Unsupported Command: [0x%x]", command);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	err("Unsupported Operation");
	return TEL_RETURN_OPERATION_NOT_SUPPORTED;
}

static void __po_free_hook(CoreObject *co)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return(po != NULL);

	g_slist_free_full(po->context_list, g_free);
	tcore_free(po->ops);
	tcore_free(po);

	tcore_object_link_object(co, NULL);
}

static void __po_clone_hook(CoreObject *src, CoreObject *dest)
{
	PrivateObject *src_po = tcore_object_ref_object(src);
	PrivateObject *dest_po = NULL;

	tcore_check_return_assert(src_po != NULL);
	tcore_check_return_assert(src_po->ops != NULL);

	dest_po = tcore_malloc0(sizeof(PrivateObject));
	dest_po->ops = tcore_memdup(src_po->ops, sizeof(TcorePsOps));
	tcore_object_link_object(dest, dest_po);
}

void tcore_ps_override_ops(CoreObject *co, TcorePsOps *ops)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_assert(po != NULL);
	tcore_check_return_assert(po->ops != NULL);
	tcore_check_return_assert(ops != NULL);

	if (ops->define_context)
		po->ops->define_context = ops->define_context;

	if (ops->activate_context)
		po->ops->activate_context = ops->activate_context;

	if (ops->deactivate_context)
		po->ops->deactivate_context = ops->deactivate_context;
}

gboolean tcore_ps_set_ops(CoreObject *co, TcorePsOps *ops)
{
	PrivateObject *po;
	tcore_check_return_value (co != NULL, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	if (po->ops != NULL) {
		tcore_free(po->ops);
		po->ops = NULL;
	}

	if (ops != NULL)
		po->ops = tcore_memdup((gconstpointer)ops, sizeof(TcorePsOps));

	return TRUE;
}

static gboolean  __ps_is_context_active(CoreObject *o, CoreObject *ps_context)
{
	guint idx_cid = 0;
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, FALSE);

	dbg("Context [%p] to be checked for Activeness", ps_context);

	po = tcore_object_ref_object(o);
	tcore_check_return_value_assert (po != NULL, FALSE);

	for (idx_cid = 1; idx_cid <= TCORE_PS_MAX_CID; idx_cid++) {
		if (po->context_id[idx_cid].cid) {
			CoreObject *s_context;
			GSList *contexts = po->context_id[idx_cid].contexts;
			while (contexts) {
	    			s_context = contexts->data;
	      			if (s_context) {
		         			if (ps_context == s_context) {
						dbg("Context: [%p] found with context ID: [%d] - [ACTIVE]",
							s_context, idx_cid);
						return TRUE;
					}
		      		}
		      		contexts = g_slist_next(contexts);
   			}
		}
	}

	dbg("Cannot find context: [%p] - [INACTIVE]", ps_context);
	return FALSE;
}

static gboolean __ps_is_apn_duplicated(CoreObject *o, CoreObject *ps_context, guint *cid)
{
	PrivateObject *po = NULL;
	gchar *t_apn = NULL;
	guint idx_cid = 0;

	dbg("Entered");
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, FALSE);

	po = tcore_object_ref_object(o);
	tcore_check_return_value_assert (po != NULL, FALSE);
	if (FALSE == tcore_context_get_apn(ps_context, &t_apn)) {
		dbg("No APN present for the given context");
		return FALSE;
	}

	for (idx_cid = 1; idx_cid <= TCORE_PS_MAX_CID; idx_cid++) {
		CoreObject *s_context;
		gchar *s_apn = NULL;
		GSList *contexts;

		if (po->context_id[idx_cid].cid == 0)
			continue;

		contexts = po->context_id[idx_cid].contexts;
		for (; contexts; contexts = contexts->next) {
			s_context = contexts->data;
			if ((s_context == NULL) || (ps_context == s_context))
				continue;

			if (FALSE == tcore_context_get_apn(s_context, &s_apn)) {
				tcore_free(s_apn);
				continue;
			}

			if (g_strcmp0(t_apn, s_apn) == 0) {
				dbg("Target and Source have same APN");

				tcore_context_cp_service_info(ps_context, s_context);
				*cid = idx_cid;

				tcore_free(t_apn);
				tcore_free(s_apn);
				return TRUE;
			}

			tcore_free(s_apn);
			s_apn = NULL;
		}
	}

	dbg("No Duplicate APN present for the given context: [%p]", ps_context);
	tcore_free(t_apn);
	return FALSE;
}

CoreObject *tcore_ps_new(TcorePlugin *plugin,
	TcorePsOps *ops, TcoreHal *hal)
{
	CoreObject *co = NULL;
	PrivateObject *po = NULL;
	tcore_check_return_value_assert(plugin != NULL, NULL);

	co = tcore_object_new(plugin, hal);
	tcore_check_return_value_assert(co != NULL, NULL);

	po = tcore_malloc0(sizeof(PrivateObject));
	if (ops != NULL)
		po->ops = tcore_memdup(ops, sizeof(TcorePsOps));

	tcore_object_set_type(co, CORE_OBJECT_TYPE_PS);
	tcore_object_link_object(co, po);
	tcore_object_set_free_hook(co, __po_free_hook);
	tcore_object_set_clone_hook(co, __po_clone_hook);
	tcore_object_set_dispatcher(co, __dispatcher);

	return co;
}

void tcore_ps_free(CoreObject *co)
{
	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_PS);
	tcore_object_free(co);
}

gboolean tcore_ps_add_context(CoreObject *o, CoreObject *ctx_o)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, FALSE);
	CORE_OBJECT_CHECK_RETURN(ctx_o, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(o);
	tcore_check_return_value_assert (po != NULL, FALSE);

	po->context_list = g_slist_insert(po->context_list, ctx_o, 0);

	return TRUE;
}

gboolean tcore_ps_remove_context(CoreObject *o, CoreObject *ctx_o)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, FALSE);
	CORE_OBJECT_CHECK_RETURN(ctx_o, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(o);
	tcore_check_return_value_assert (po != NULL, FALSE);

	(void)tcore_ps_clear_context_id(o, ctx_o);
	po->context_list = g_slist_remove(po->context_list, ctx_o);

	return TRUE;
}

gboolean tcore_ps_set_online(CoreObject *o, gboolean state)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, FALSE);

	po = tcore_object_ref_object(o);
	tcore_check_return_value_assert (po != NULL, FALSE);

	po->online = state;
	dbg("PS Status: [%s]", (po->online ? "ONLINE" : "OFFLINE"));

	return TRUE;
}

gboolean tcore_ps_assign_context_id(CoreObject *o, CoreObject *context, guint cid)
{
	PrivateObject *po = NULL;
	guint idx;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, FALSE);
	CORE_OBJECT_CHECK_RETURN(context, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	dbg("Entered");
	po = tcore_object_ref_object(o);
	tcore_check_return_value_assert (po != NULL, FALSE);

	if (cid == 0) {
		/* Automatic assign */
		dbg("Automatic assigning of CID");
		for (idx = 1; idx <= TCORE_PS_MAX_CID; idx++) {
			if (po->context_id[idx].cid == 0) {
				po->context_id[idx].cid = idx;

				po->context_id[idx].contexts =
					g_slist_append(po->context_id[idx].contexts, context);

				dbg("Assign CID [%d] to context: [%p]", idx, context);
				return tcore_context_set_id(context, idx);
			}
			else {
				dbg("CID: [%d] is not available", idx);
			}
		}

		dbg("Can't find free CID");
	}
	else {
		/* Manual assign */
		dbg("Manual assigning of CID: [%d]", cid);
		if (po->context_id[cid].cid == cid) {
			po->context_id[cid].contexts =
				g_slist_append(po->context_id[cid].contexts, context);

			dbg("Assign CID [%d] to context: [%p]", cid, context);
			return tcore_context_set_id(context, cid);
		}
		else {
			dbg("CID: [%d] is not available", cid);
		}
	}

	dbg("Exiting");
	return FALSE;
}

gboolean tcore_ps_clear_context_id(CoreObject *o, CoreObject *context)
{
	PrivateObject *po = NULL;
	guint cid = 0, cnt = 0;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, FALSE);
	CORE_OBJECT_CHECK_RETURN(context, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(o);
	tcore_check_return_value_assert (po != NULL, FALSE);

	if (FALSE == tcore_context_get_id(context, &cid))
		return FALSE;

	if ((cid == 0) || (cid > TCORE_PS_MAX_CID))
		return FALSE;

	/* Remove context */
	po->context_id[cid].contexts = g_slist_remove(po->context_id[cid].contexts, context);
	cnt = g_slist_length(po->context_id[cid].contexts);
	if (cnt == 0)
		po->context_id[cid].cid = 0;

	return tcore_context_set_id(context, 0);
}

CoreObject *tcore_ps_ref_context_by_role(CoreObject *o, TcoreContextRole role)
{
	PrivateObject *po = NULL;
	GSList *list;
	CoreObject *pdp_o;
	TcoreContextRole context_role;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, NULL);

	po = tcore_object_ref_object(o);
	tcore_check_return_value_assert (po != NULL, NULL);

	if (po->context_list) {
		for (list = po->context_list; list; list = list->next) {
			pdp_o = list->data;
			if (pdp_o && (tcore_object_get_type(pdp_o) == CORE_OBJECT_TYPE_PS_CONTEXT))
				if (tcore_context_get_role(pdp_o, &context_role))
					if (context_role == role)
						return pdp_o;
		}
	}

	return NULL;
}

gboolean tcore_ps_ref_context_by_id(CoreObject *o, guint id, GSList **list)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, FALSE);

	po = tcore_object_ref_object(o);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(list != NULL, FALSE);

	if ((id == 0) || (id > TCORE_PS_MAX_CID))
		return FALSE;

	if (po->context_id[id].cid != id)
		return FALSE;

	*list = po->context_id[id].contexts;

	return TRUE;
}

static TelReturn __ps_define_context(CoreObject *o, CoreObject *ps_context,
	TcoreObjectResponseCallback cb, void *user_data)
{
	PrivateObject *po = NULL;
	guint cid = 0;

	dbg(" Entered ps_context: [%p]", ps_context);
	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TEL_RETURN_INVALID_PARAMETER);

	po = tcore_object_ref_object(o);
	tcore_check_return_value_assert(po != NULL, TEL_RETURN_INVALID_PARAMETER);
	tcore_check_return_value_assert(ps_context != NULL, TEL_RETURN_INVALID_PARAMETER);

	if (TRUE == __ps_is_context_active(o, ps_context)) {
		dbg("Return, requested context is already in active state");
		return TEL_RETURN_SUCCESS;
	}

	/*
	 * Check if APN of requested context is same as any active context
	 * if yes simply append context to list of that Context ID
	 */
	if (TRUE == __ps_is_apn_duplicated(o, ps_context, &cid)) {
		/* Append to Context ID list */
		po->context_id[cid].contexts =
			g_slist_append(po->context_id[cid].contexts, ps_context);

		/* Set Context ID */
		tcore_context_set_id(ps_context, cid);
		dbg("Duplicate APN present, added context[%p] to list of CID: [%d]",
			ps_context, cid);

		return TEL_RETURN_SUCCESS;
	}

	/* New context to be defined assgin Context ID */
	if (TRUE == tcore_context_get_id(ps_context, &cid)) {
		dbg("Current context [%p] has context ID: [%d]", ps_context, cid);
		if (cid == 0) {
			if (tcore_ps_assign_context_id(o, ps_context, 0) != TRUE) {
				err("Failed to assign context id to context");
				return TEL_RETURN_FAILURE;
			}
		}
	} else {
		err("Failed to get Context ID for context: [%p]", ps_context);
		return TEL_RETURN_FAILURE;
	}

	if (FALSE == tcore_context_get_id(ps_context, &cid))
		return TEL_RETURN_FAILURE;
	dbg("Context: [%p] CID: [%d]", ps_context, cid);

	dbg("Sending 'Define Context' request to modem");
	return po->ops->define_context(o, ps_context, cb, user_data);
}

static TelReturn __ps_activate_context(CoreObject *o, CoreObject *ps_context,
	TcoreObjectResponseCallback cb, void *user_data)
{
	PrivateObject *po = NULL;
	TcoreContextState context_state = TCORE_CONTEXT_STATE_DEACTIVATED;

	dbg("Entered");

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TEL_RETURN_INVALID_PARAMETER);

	po = tcore_object_ref_object(o);
	tcore_check_return_value_assert(po != NULL, TEL_RETURN_INVALID_PARAMETER);
	tcore_check_return_value_assert(ps_context != NULL, TEL_RETURN_INVALID_PARAMETER);

 	if (!po->online) {
		err("PS service is NOT Online");
		return TEL_RETURN_OPERATION_NOT_SUPPORTED;
	}

	if (FALSE ==  __ps_is_context_active(o, ps_context)) {
		err("Context: [%p] is NOT yet Defined!!!", ps_context);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	(void)tcore_context_get_state(ps_context, &context_state);
	dbg("Context: [%p] Current state: [%d] ", ps_context, context_state);

	if (context_state == TCORE_CONTEXT_STATE_ACTIVATED)
		return TEL_RETURN_SUCCESS;
	else if (context_state == TCORE_CONTEXT_STATE_ACTIVATING)
		return TEL_RETURN_SUCCESS;
	else if (context_state == TCORE_CONTEXT_STATE_DEACTIVATING)
		return TEL_RETURN_FAILURE;

	/* Set state to ACTIVATING */
	(void)tcore_context_set_state(ps_context, TCORE_CONTEXT_STATE_ACTIVATING);

	dbg("Sending 'Activate Context' request for context: [%p]", ps_context);
	return po->ops->activate_context(o, ps_context, cb, user_data);
}

static TelReturn __ps_deactivate_context(CoreObject *o, CoreObject *ps_context,
	TcoreObjectResponseCallback cb, void *user_data)
{
	PrivateObject *po = NULL;
	TcoreContextState context_state = TCORE_CONTEXT_STATE_DEACTIVATED;
	guint cid = 0;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, TEL_RETURN_INVALID_PARAMETER);

	po = tcore_object_ref_object(o);
	tcore_check_return_value_assert(po != NULL, TEL_RETURN_INVALID_PARAMETER);
	tcore_check_return_value_assert(ps_context != NULL, TEL_RETURN_INVALID_PARAMETER);
	dbg("Sending deactivation request for context[%p]", ps_context);
	if (!po->online) {
		dbg("PS service is not online");
		return TEL_RETURN_OPERATION_NOT_SUPPORTED;
	}

	if (FALSE ==  __ps_is_context_active(o, ps_context)) {
		dbg("Context is not in active state");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	(void)tcore_context_get_id(ps_context, &cid);
	if (g_slist_length(po->context_id[cid].contexts) > 1) {
		/* Remove context from list and return Success */
		po->context_id[cid].contexts =
			g_slist_remove(po->context_id[cid].contexts, ps_context);

		/* Set duplicated apn profile state to deactivated */
		tcore_context_set_state(ps_context, TCORE_CONTEXT_STATE_DEACTIVATED);

		return TEL_RETURN_SUCCESS;
	}

	(void)tcore_context_get_state(ps_context, &context_state);
	dbg("Context: [%p] State: [%d]", ps_context, context_state);
	if (context_state == TCORE_CONTEXT_STATE_DEACTIVATED)
		return TEL_RETURN_SUCCESS;
	else if (context_state == TCORE_CONTEXT_STATE_DEACTIVATING)
		return TEL_RETURN_SUCCESS;
	else if (context_state == TCORE_CONTEXT_STATE_ACTIVATING)
		return TEL_RETURN_FAILURE;

	tcore_context_set_state(ps_context, TCORE_CONTEXT_STATE_DEACTIVATING);

	dbg("Sending 'Deactivate Context' request for context: [%p]", ps_context);
	return po->ops->deactivate_context(o, ps_context, cb , user_data);
}

gboolean tcore_ps_is_active_apn(CoreObject *o, const char *apn)
{
	guint idx_cid = 0;
	PrivateObject *po = NULL;
	CoreObject *s_context  = NULL;
	gchar *s_apn = NULL;
	GSList *contexts;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS, FALSE);

	po = tcore_object_ref_object(o);

	for (idx_cid = 1 ; idx_cid <= TCORE_PS_MAX_CID ; idx_cid++) {
		contexts = po->context_id[idx_cid].contexts;
		if (g_slist_length(contexts) == 0)
			continue;

		if (contexts->data == NULL) {
			contexts = g_slist_next(contexts);
			continue;
		}

		s_context = (CoreObject *)(contexts->data);
		tcore_context_get_apn(s_context, &s_apn);
		if (0 == g_strcmp0(apn, s_apn)) {
			dbg("'%s' APN is - [ACTIVE]", apn);
			tcore_free(s_apn);
			return TRUE;
		}

		tcore_free(s_apn);
		s_apn = NULL;
		contexts = g_slist_next(contexts);
	}

	dbg("'%s' APN - [INACTIVE]", apn);
	return FALSE;
}
