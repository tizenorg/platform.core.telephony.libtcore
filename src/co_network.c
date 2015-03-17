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
#include "co_network.h"

struct private_object_data {
	struct tcore_network_operations *ops;

	enum telephony_network_service_type service_type;
	enum telephony_network_access_technology act;
	enum telephony_network_service_domain_status cs_domain_status;
	enum telephony_network_service_domain_status ps_domain_status;
	char *plmn;
	gboolean roaming_state;
	int restricted_state;
	unsigned int lac;
	unsigned int rac;
	unsigned int cell_id;
	gboolean gsm_dtm_support; // DTM (Dual Transfer Mode)

	char *network_name_short;
	char *network_name_full;
	char *network_name_spn;
	enum tcore_network_name_priority network_name_priority;
	GHashTable *operator_info_hash;
};

static TReturn _dispatcher(CoreObject *co, UserRequest *ur)
{
	enum tcore_request_command command;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po || !po->ops)
		return TCORE_RETURN_ENOSYS;

	command = tcore_user_request_get_command(ur);
	switch (command) {
		case TREQ_NETWORK_SEARCH:
			if (!po->ops->search)
				return TCORE_RETURN_ENOSYS;

			return po->ops->search(co, ur);

		case TREQ_NETWORK_SET_PLMN_SELECTION_MODE:
			if (!po->ops->set_plmn_selection_mode)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_plmn_selection_mode(co, ur);

		case TREQ_NETWORK_GET_PLMN_SELECTION_MODE:
			if (!po->ops->get_plmn_selection_mode)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_plmn_selection_mode(co, ur);

		case TREQ_NETWORK_SET_SERVICE_DOMAIN:
			if (!po->ops->set_service_domain)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_service_domain(co, ur);

		case TREQ_NETWORK_GET_SERVICE_DOMAIN:
			if (!po->ops->get_service_domain)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_service_domain(co, ur);

		case TREQ_NETWORK_SET_BAND:
			if (!po->ops->set_band)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_band(co, ur);

		case TREQ_NETWORK_GET_BAND:
			if (!po->ops->get_band)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_band(co, ur);

		case TREQ_NETWORK_SET_PREFERRED_PLMN:
			if (!po->ops->set_preferred_plmn)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_preferred_plmn(co, ur);

		case TREQ_NETWORK_GET_PREFERRED_PLMN:
			if (!po->ops->get_preferred_plmn)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_preferred_plmn(co, ur);

		case TREQ_NETWORK_SET_ORDER:
			if (!po->ops->set_order)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_order(co, ur);

		case TREQ_NETWORK_GET_ORDER:
			if (!po->ops->get_order)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_order(co, ur);

		case TREQ_NETWORK_SET_POWER_ON_ATTACH:
			if (!po->ops->set_power_on_attach)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_power_on_attach(co, ur);

		case TREQ_NETWORK_GET_POWER_ON_ATTACH:
			if (!po->ops->get_power_on_attach)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_power_on_attach(co, ur);

		case TREQ_NETWORK_SET_CANCEL_MANUAL_SEARCH:
			if (!po->ops->set_cancel_manual_search)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_cancel_manual_search(co, ur);

		case TREQ_NETWORK_GET_SERVING_NETWORK:
			if (!po->ops->get_serving_network)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_serving_network(co, ur);

		case TREQ_NETWORK_SET_MODE:
			if (!po->ops->set_mode)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_mode(co, ur);

		case TREQ_NETWORK_GET_MODE:
			if (!po->ops->get_mode)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_mode(co, ur);

		case TREQ_NETWORK_SET_NEIGHBORING_CELL_INFO:
			if (!po->ops->set_neighboring_cell_info)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_neighboring_cell_info(co, ur);

		case TREQ_NETWORK_GET_NEIGHBORING_CELL_INFO:
			if (!po->ops->get_neighboring_cell_info)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_neighboring_cell_info(co, ur);

		case TREQ_NETWORK_SET_DEFAULT_DATA_SUBSCRIPTION:
			if (!po->ops->set_default_data_subscription)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_default_data_subscription(co, ur);

		case TREQ_NETWORK_GET_DEFAULT_DATA_SUBSCRIPTION:
			if (!po->ops->get_default_data_subscription)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_default_data_subscription(co, ur);

		case TREQ_NETWORK_SET_DEFAULT_SUBSCRIPTION:
			if (!po->ops->set_default_subscription)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_default_subscription(co, ur);

		case TREQ_NETWORK_GET_DEFAULT_SUBSCRIPTION:
			if (!po->ops->get_default_subscription)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_default_subscription(co, ur);

		case TREQ_NETWORK_SET_EMERGENCY_CALLBACK_MODE:
			if (!po->ops->set_emergency_callback_mode)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_emergency_callback_mode(co, ur);

		case TREQ_NETWORK_SET_ROAMING_PREFERENCE:
			if (!po->ops->set_roaming_preference)
				return TCORE_RETURN_ENOSYS;

			return po->ops->set_roaming_preference(co, ur);

		case TREQ_NETWORK_GET_ROAMING_PREFERENCE:
			if (!po->ops->get_roaming_preference)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_roaming_preference(co, ur);

		case TREQ_NETWORK_GET_SUBSCRIPTION_INFO:
			if (!po->ops->get_subscription_info)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_subscription_info(co, ur);

		default:
			break;
	}

	return TCORE_RETURN_SUCCESS;
}

static void _free_hook(CoreObject *co)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_NETWORK);

	po = tcore_object_ref_object(co);
	if (po) {
		free(po);
		tcore_object_link_object(co, NULL);
	}
}

CoreObject *tcore_network_new(TcorePlugin *plugin, const char *name,
		struct tcore_network_operations *ops, TcoreHal *hal)
{
	CoreObject *o = NULL;
	struct private_object_data *po = NULL;

	if (!plugin)
		return NULL;

	o = tcore_object_new(plugin, name, hal);
	if (!o)
		return NULL;

	po = calloc(1, sizeof(struct private_object_data));
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	po->ops = ops;
	po->operator_info_hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	tcore_object_set_type(o, CORE_OBJECT_TYPE_NETWORK);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_network_free(CoreObject *co)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_NETWORK);

	po = tcore_object_ref_object(co);
	if (!po)
		return;

	if (po->network_name_short)
		free(po->network_name_short);
	if (po->network_name_full)
		free(po->network_name_full);
	if (po->network_name_spn)
		free(po->network_name_spn);
	if (po->plmn)
		free(po->plmn);

	if (po->operator_info_hash)
		g_hash_table_destroy(po->operator_info_hash);

	tcore_object_free(co);
}

void tcore_network_set_ops(CoreObject *o, struct tcore_network_operations *ops)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_NETWORK);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		return;
	}

	po->ops = ops;
}

char* tcore_network_get_plmn(CoreObject *co)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, NULL);

	po = tcore_object_ref_object(co);
	if (!po)
		return NULL;

	return g_strdup(po->plmn);
}

TReturn tcore_network_set_plmn(CoreObject *co, const char *plmn)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (po->plmn)
		free(po->plmn);

	po->plmn = g_strdup(plmn);

	return TCORE_RETURN_SUCCESS;
}


char* tcore_network_get_network_name(CoreObject *co,
		enum tcore_network_name_type type)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, NULL);

	po = tcore_object_ref_object(co);
	if (!po)
		return NULL;

	if (type == TCORE_NETWORK_NAME_TYPE_SHORT)
		return g_strdup(po->network_name_short);
	else if (type == TCORE_NETWORK_NAME_TYPE_FULL)
		return g_strdup(po->network_name_full);
	else if (type == TCORE_NETWORK_NAME_TYPE_SPN)
		return g_strdup(po->network_name_spn);
	else
		return NULL;
}

TReturn tcore_network_set_network_name(CoreObject *co,
		enum tcore_network_name_type type, const char *network_name)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	switch (type) {
		case TCORE_NETWORK_NAME_TYPE_SHORT:
			if (po->network_name_short) {
				free (po->network_name_short);
				po->network_name_short = NULL;
			}

			if (network_name)
				po->network_name_short = g_strdup (network_name);

			break;

		case TCORE_NETWORK_NAME_TYPE_FULL:
			if (po->network_name_full) {
				free (po->network_name_full);
				po->network_name_full = NULL;
			}

			if (network_name)
				po->network_name_full = g_strdup (network_name);

			break;

		case TCORE_NETWORK_NAME_TYPE_SPN:
			if (po->network_name_spn) {
				free (po->network_name_spn);
				po->network_name_spn = NULL;
			}

			if (network_name)
				po->network_name_spn = g_strdup (network_name);

			break;

		default:
			return TCORE_RETURN_EINVAL;
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_get_network_name_priority(CoreObject *co,
		enum tcore_network_name_priority *priority)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	if (!priority)
		return TCORE_RETURN_EINVAL;

	po = tcore_object_ref_object(co);
	if (!po)
		return FALSE;

	*priority = po->network_name_priority;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_set_network_name_priority(CoreObject *co,
		enum tcore_network_name_priority priority)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->network_name_priority  = priority;

	return TCORE_RETURN_SUCCESS;
}

gboolean tcore_network_get_roaming_state(CoreObject *co)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, FALSE);

	po = tcore_object_ref_object(co);
	if (!po)
		return FALSE;

	return po->roaming_state;
}

TReturn tcore_network_set_roaming_state(CoreObject *co, gboolean state)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->roaming_state = state;
	dbg("roaming_state = 0x%x", state);

	return TCORE_RETURN_SUCCESS;
}

int tcore_network_get_restricted_state(CoreObject *co)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, FALSE);

	po = tcore_object_ref_object(co);
	if (!po)
		return FALSE;

	return po->restricted_state;
}

TReturn tcore_network_set_restricted_state(CoreObject *co, int state)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->restricted_state = state;
	dbg("restricted_state = 0x%x", state);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_get_service_status(CoreObject *co,
		enum tcore_network_service_domain_type type,
		enum telephony_network_service_domain_status *result)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	if (!result)
		return TCORE_RETURN_EINVAL;

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	switch (type) {
		case TCORE_NETWORK_SERVICE_DOMAIN_TYPE_CIRCUIT:
			*result = po->cs_domain_status;
			break;

		case TCORE_NETWORK_SERVICE_DOMAIN_TYPE_PACKET:
			*result = po->ps_domain_status;
			break;
		default:
			err("invalid network type");
			break;
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_set_service_status(CoreObject *co,
		enum tcore_network_service_domain_type type,
		enum telephony_network_service_domain_status status)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	switch (type) {
		case TCORE_NETWORK_SERVICE_DOMAIN_TYPE_CIRCUIT:
			po->cs_domain_status = status;
			dbg("cs.status = 0x%x", status);
			break;

		case TCORE_NETWORK_SERVICE_DOMAIN_TYPE_PACKET:
			po->ps_domain_status = status;
			dbg("ps.status = 0x%x", status);
			break;
		default:
			err("invalid network type");
			break;

	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_set_access_technology(CoreObject *co,
		enum telephony_network_access_technology act)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->act = act;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_get_access_technology(CoreObject *co,
		enum telephony_network_access_technology *result)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	if (!result)
		return TCORE_RETURN_EINVAL;

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	*result = po->act;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_set_lac(CoreObject *co, unsigned int lac)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->lac = lac;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_get_lac(CoreObject *co, unsigned int *result)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	if (!result)
		return TCORE_RETURN_EINVAL;

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	*result = po->lac;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_set_rac(CoreObject *co, unsigned int rac)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->rac = rac;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_get_rac(CoreObject *co, unsigned int *result)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	if (!result)
		return TCORE_RETURN_EINVAL;

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	*result = po->rac;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_set_cell_id(CoreObject *co, unsigned int cell_id)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->cell_id = cell_id;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_get_cell_id(CoreObject *co, unsigned int *result)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	if (!result)
		return TCORE_RETURN_EINVAL;

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	*result = po->cell_id;

	return TCORE_RETURN_SUCCESS;
}

gboolean tcore_network_get_gsm_dtm_support(CoreObject *co)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, FALSE);

	po = tcore_object_ref_object(co);
	if (!po)
		return FALSE;

	return po->gsm_dtm_support;
}

TReturn tcore_network_set_gsm_dtm_support(CoreObject *co, gboolean state)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->gsm_dtm_support = state;
	dbg("gsm_dtm_support = %d", state);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_set_service_type(CoreObject *co,
		enum telephony_network_service_type service_type)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->service_type = service_type;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_get_service_type(CoreObject *co,
		enum telephony_network_service_type *result)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	if (!result)
		return TCORE_RETURN_EINVAL;

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	*result = po->service_type;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_operator_info_add(CoreObject *co,
		const struct tcore_network_operator_info *noi)
{
	struct private_object_data *po = NULL;
	char plmn[7];
	int mcc_index, mnc_index;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	if (!noi)
		return TCORE_RETURN_EINVAL;

	mcc_index = atoi(noi->mcc);
	mnc_index = atoi(noi->mnc);

	if (mcc_index > 999 || mnc_index > 999) {
		err("mcc_index %d mnc_index %d", mcc_index, mnc_index);
		return TCORE_RETURN_EINVAL;
	}

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	g_strlcpy(plmn, noi->mcc, 4);
	g_strlcpy(plmn+3, noi->mnc, 3);

	if (g_hash_table_lookup(po->operator_info_hash, plmn)) {
		//dbg("Remove existed (key:%s)", plmn);
		g_hash_table_remove(po->operator_info_hash, plmn);
	}

	//dbg("Adding mcc[%s] mnc[%s] name[%s] type[%d] in operator info table", noi->mcc, noi->mnc, noi->name, noi->type);
	g_hash_table_insert(po->operator_info_hash, g_strdup(plmn), g_memdup(noi, sizeof(struct tcore_network_operator_info)));

	return TCORE_RETURN_SUCCESS;
}

struct tcore_network_operator_info *
tcore_network_operator_info_find(CoreObject *co, const char *mcc, const char *mnc)
{
	struct private_object_data *po = NULL;
	struct tcore_network_operator_info *data;
	char plmn[7];
	int mcc_index, mnc_index;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, NULL);

	if (!mcc || !mnc)
		return NULL;

	mcc_index = atoi(mcc);
	mnc_index = atoi(mnc);

	if (mcc_index > 999 || mnc_index > 999) {
		err("mcc_index %d mnc_index %d", mcc_index, mnc_index);
		return NULL;
	}

	po = tcore_object_ref_object(co);
	if (!po)
		return NULL;

	g_strlcpy(plmn, mcc, 4);
	g_strlcpy(plmn+3, mnc, 3);

	data =  g_hash_table_lookup(po->operator_info_hash, plmn);

	if (data) {
		dbg("found mcc[%s], mnc[%s] name[%s] type[%d] in operator info table (%p)",
			data->mcc, data->mnc, data->name, data->type, po->operator_info_hash);
	} else {
		dbg("mcc[%s] mnc[%s] not present in operator table (%p)",
			mcc, mnc, po->operator_info_hash);
	}

	return data;
}
