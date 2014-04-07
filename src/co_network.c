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
#include "co_network.h"

typedef struct {
	TcoreNetworkOps *ops;

	TelNetworkRegStatus cs_status;
	TelNetworkRegStatus ps_status;
	TelNetworkAct act;
	gboolean roam_state;

	unsigned int lac;
	unsigned int rac;
	unsigned int cell_id;

	char *plmn;
	char *short_name;
	char *long_name;

	GHashTable *operator_name_hash;
} PrivateObject;

static TelReturn _dispatcher(CoreObject *co,
	TcoreCommand command, const void *request,
	TcoreObjectResponseCallback cb, const void *user_data)
{
	TcoreNetworkOps *network = NULL;
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, TEL_RETURN_INVALID_PARAMETER);
	tcore_check_return_value_assert(po->ops != NULL, TEL_RETURN_INVALID_PARAMETER);

	network = po->ops;

	switch (command) {
	case TCORE_COMMAND_NETWORK_GET_IDENTITY_INFO:
		if (network->get_identity_info)
			return network->get_identity_info(co, cb, (void *)user_data);
		break;

	case TCORE_COMMAND_NETWORK_SEARCH:
		if (network->search)
			return network->search(co, cb, (void *)user_data);
		break;

	case TCORE_COMMAND_NETWORK_CANCEL_SEARCH:
		if (network->cancel_search)
			return network->cancel_search(co, cb, (void *)user_data);
		break;

	case TCORE_COMMAND_NETWORK_SELECT_AUTOMATIC:
		if (network->select_automatic)
			return network->select_automatic(co, cb, (void *)user_data);
		break;

	case TCORE_COMMAND_NETWORK_SELECT_MANUAL:
		if (network->select_manual)
			return network->select_manual(co,
				(TelNetworkSelectManualInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_NETWORK_GET_SELECTION_MODE:
		if (network->get_selection_mode)
			return network->get_selection_mode(co, cb, (void *)user_data);
		break;

	case TCORE_COMMAND_NETWORK_SET_PREFERRED_PLMN:
		if (network->set_preferred_plmn)
			return network->set_preferred_plmn(co,
				(const TelNetworkPreferredPlmnInfo *)request,
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_NETWORK_GET_PREFERRED_PLMN:
		if (network->get_preferred_plmn)
			return network->get_preferred_plmn(co, cb, (void *)user_data);
		break;

	case TCORE_COMMAND_NETWORK_SET_MODE:
		if (network->set_mode)
			return network->set_mode(co,
				*((TelNetworkMode *)request),
				cb, (void *)user_data);
		break;

	case TCORE_COMMAND_NETWORK_GET_MODE:
		if (network->get_mode)
			return network->get_mode(co, cb, (void *)user_data);
		break;

	case TCORE_COMMAND_NETWORK_GET_NEIGHBORING_CELL_INFO:
		if (network->get_neighboring_cell_info)
			return network->get_neighboring_cell_info(co,
				cb, (void *)user_data);
		break;

	default:
		err("Unsupported Command: [0x%x]",command);
		return TEL_NETWORK_RESULT_INVALID_PARAMETER;

	}
	err("Unsupported Operation");
	return TEL_RETURN_OPERATION_NOT_SUPPORTED;
}

static void _po_free_hook(CoreObject *co)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return(po != NULL);

	g_hash_table_destroy(po->operator_name_hash);
	tcore_free(po->plmn);
	tcore_free(po->short_name);
	tcore_free(po->long_name);
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

	dest_po = tcore_malloc0(sizeof(PrivateObject));
	dest_po->ops = tcore_memdup(src_po->ops, sizeof(TcoreNetworkOps));
	tcore_object_link_object(dest, dest_po);
}

void tcore_network_override_ops(CoreObject *co, TcoreNetworkOps *ops)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_assert(po != NULL);
	tcore_check_return_assert(po->ops != NULL);
	tcore_check_return_assert(ops != NULL);

	if (ops->get_identity_info)
		po->ops->get_identity_info = ops->get_identity_info;
	if (ops->search)
		po->ops->search = ops->search;
	if (ops->cancel_search)
		po->ops->cancel_search = ops->cancel_search;
	if (ops->select_automatic)
		po->ops->select_automatic = ops->select_automatic;
	if (ops->select_manual)
		po->ops->select_manual = ops->select_manual;
	if (ops->get_selection_mode)
		po->ops->get_selection_mode = ops->get_selection_mode;
	if (ops->set_preferred_plmn)
		po->ops->set_preferred_plmn = ops->set_preferred_plmn;
	if (ops->get_preferred_plmn)
		po->ops->get_preferred_plmn = ops->get_preferred_plmn;
	if (ops->set_mode)
		po->ops->set_mode = ops->set_mode;
	if (ops->get_mode)
		po->ops->get_mode = ops->get_mode;
	if (ops->get_neighboring_cell_info)
		po->ops->get_neighboring_cell_info = ops->get_neighboring_cell_info;
}

gboolean tcore_network_set_ops(CoreObject *co, TcoreNetworkOps *ops)
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
		po->ops = tcore_memdup((gconstpointer)ops, sizeof(TcoreNetworkOps));

	return TRUE;
}

CoreObject *tcore_network_new(TcorePlugin *plugin,
			TcoreNetworkOps *ops, TcoreHal *hal)
{
	CoreObject *co = NULL;
	PrivateObject *po = NULL;
	tcore_check_return_value_assert(plugin != NULL, NULL);

	co = tcore_object_new(plugin, hal);
	tcore_check_return_value_assert(co != NULL, NULL);

	po = tcore_malloc0(sizeof(PrivateObject));

	if (ops != NULL)
		po->ops = tcore_memdup(ops, sizeof(TcoreNetworkOps));

	po->operator_name_hash = g_hash_table_new_full(g_str_hash,
			g_str_equal, g_free, g_free);

	tcore_object_set_type(co, CORE_OBJECT_TYPE_NETWORK);
	tcore_object_link_object(co, po);
	tcore_object_set_free_hook(co, _po_free_hook);
	tcore_object_set_clone_hook(co, _po_clone_hook);
	tcore_object_set_dispatcher(co, _dispatcher);
	return co;
}

void tcore_network_free(CoreObject *co)
{
	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_NETWORK);
	tcore_object_free(co);
}

gboolean tcore_network_get_cs_reg_status(CoreObject *co,
		TelNetworkRegStatus *cs_status)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(cs_status != NULL, FALSE);

	*cs_status = po->cs_status;
	return TRUE;
}

gboolean tcore_network_set_cs_reg_status(CoreObject *co,
		TelNetworkRegStatus cs_status)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->cs_status = cs_status;
	return TRUE;
}

gboolean tcore_network_get_ps_reg_status(CoreObject *co,
		TelNetworkRegStatus *ps_status)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(ps_status != NULL, FALSE);

	*ps_status = po->ps_status;
	return TRUE;
}

gboolean tcore_network_set_ps_reg_status(CoreObject *co,
		TelNetworkRegStatus ps_status)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->ps_status = ps_status;
	return TRUE;
}

gboolean tcore_network_set_access_technology(CoreObject *co,
		TelNetworkAct act)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->act = act;
	return TRUE;

}

gboolean tcore_network_get_access_technology(CoreObject *co,
		TelNetworkAct *act)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(act != NULL, FALSE);

	*act = po->act;
	return TRUE;
}

gboolean tcore_network_get_roam_state(CoreObject *co, gboolean *state)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(state != NULL, FALSE);

	*state = po->roam_state;
	return TRUE;
}

gboolean tcore_network_set_roam_state(CoreObject *co, gboolean state)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->roam_state = state;
	return TRUE;
}

gboolean tcore_network_get_lac(CoreObject *co, unsigned int *lac)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(lac != NULL, FALSE);

	*lac = po->lac;
	return TRUE;
}

gboolean tcore_network_set_lac(CoreObject *co, unsigned int lac)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->lac = lac;
	return TRUE;
}

gboolean tcore_network_get_rac(CoreObject *co, unsigned int *rac)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(rac != NULL, FALSE);

	*rac = po->rac;
	return TRUE;
}

gboolean tcore_network_set_rac(CoreObject *co, unsigned int rac)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->rac = rac;
	return TRUE;
}

gboolean tcore_network_get_cell_id(CoreObject *co, unsigned int *cell_id)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(cell_id != NULL, FALSE);

	*cell_id = po->cell_id;
	return TRUE;
}

gboolean tcore_network_set_cell_id(CoreObject *co, unsigned int cell_id)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->cell_id = cell_id;
	return TRUE;
}

gboolean tcore_network_get_plmn(CoreObject *co, char **plmn)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(plmn != NULL, FALSE);

	*plmn = tcore_strdup(po->plmn);
	return TRUE;
}

gboolean tcore_network_set_plmn(CoreObject *co, const char *plmn)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(plmn != NULL, FALSE);

	tcore_free(po->plmn);
	po->plmn = tcore_strdup(plmn);
	return TRUE;
}

gboolean tcore_network_get_short_name(CoreObject *co, char **short_name)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(short_name != NULL, FALSE);

	*short_name = tcore_strdup(po->short_name);
	return TRUE;
}

gboolean tcore_network_set_short_name(CoreObject *co, const char *short_name)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(short_name != NULL, FALSE);

	tcore_free(po->short_name);
	po->short_name = tcore_strdup(short_name);
	return TRUE;
}

gboolean tcore_network_get_long_name(CoreObject *co, char **long_name)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(long_name != NULL, FALSE);

	*long_name = tcore_strdup(po->long_name);
	return TRUE;
}

gboolean tcore_network_set_long_name(CoreObject *co, const char *long_name)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(long_name != NULL, FALSE);

	tcore_free(po->long_name);
	po->long_name = tcore_strdup(long_name);
	return TRUE;
}

gboolean tcore_network_get_operator_name(CoreObject *co,
		const char *plmn, char **name)
{
	PrivateObject *po = tcore_object_ref_object(co);
	char *network_name;
	tcore_check_return_value_assert(plmn != NULL, FALSE);
	tcore_check_return_value_assert(name != NULL, FALSE);

	network_name = (char *)g_hash_table_lookup(po->operator_name_hash, plmn);
	*name = tcore_strdup(network_name);
	return TRUE;
}


gboolean tcore_network_set_operator_name(CoreObject *co,
		const char *plmn, const char *name)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(plmn != NULL, FALSE);
	tcore_check_return_value_assert(name != NULL, FALSE);

	dbg("Set Operator Name, PLMN: [%s], Name: [%s]", plmn, name);

	g_hash_table_insert(po->operator_name_hash,
			tcore_strdup(plmn), tcore_strdup(name));

	return TRUE;
}
