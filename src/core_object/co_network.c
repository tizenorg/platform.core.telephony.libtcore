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

#include <tzplatform_config.h>

#include <glib.h>

#include "tcore.h"
#include "internal/tcore_types.h"
#include "plugin.h"
#include "queue.h"
#include "server.h"
#include "user_request.h"
#include "co_network.h"
#include "storage.h"

#define NETWORK_MCC_MNC_OPER_LIST_GSMA_DB tzplatform_mkpath(TZ_SYS_DB, ".mcc_mnc_oper_list.db")

struct private_object_data {
	struct tcore_network_operations *ops[TCORE_OPS_TYPE_MAX];

	enum telephony_network_service_type service_type;
	enum telephony_network_access_technology act;
	enum telephony_network_service_domain_status cs_domain_status;
	enum telephony_network_service_domain_status ps_domain_status;
	char *plmn;
	gboolean roaming_state;
	int restricted_state;
	unsigned int lac; /* represent LAC or TAC(in case of LTE) */
	unsigned int rac;
	unsigned int cell_id;
	gboolean gsm_dtm_support; /* DTM (Dual Transfer Mode) */

	char *network_name_short;
	char *network_name_full;
	char *network_name_spn;
	enum tcore_network_name_priority network_name_priority;
	GHashTable *operator_info_hash;
	struct tel_network_ims_registration_info ims_reg_info; /* IMS specific */
	gboolean ims_voice_status;
	enum telephony_network_ecc_rat_search_status rat_search_status;
	enum telephony_network_access_technology ecc_rat;
};

static TReturn _dispatcher(CoreObject *co, UserRequest *ur, enum tcore_ops_type ops_type)
{
	enum tcore_request_command command;
	struct private_object_data *po = tcore_object_ref_object(co);
	struct tcore_network_operations *ops = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);
	CORE_OBJECT_VALIDATE_OPS_RETURN_VAL(ops_type, TCORE_RETURN_EINVAL);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_EINVAL);
	tcore_check_null_ret_err("ur", ur, TCORE_RETURN_EINVAL);

	ops = po->ops[ops_type];
	tcore_check_null_ret_err("ops", ops, TCORE_RETURN_FAILURE);

	command = tcore_user_request_get_command(ur);
	switch (command) {
	case TREQ_NETWORK_SEARCH:
		tcore_check_null_ret_err("ops->search",
			ops->search, TCORE_RETURN_ENOSYS);

		return ops->search(co, ur);

	case TREQ_NETWORK_SET_PLMN_SELECTION_MODE:
		tcore_check_null_ret_err("ops->set_plmn_selection_mode",
			ops->set_plmn_selection_mode, TCORE_RETURN_ENOSYS);

		return ops->set_plmn_selection_mode(co, ur);

	case TREQ_NETWORK_GET_PLMN_SELECTION_MODE:
		tcore_check_null_ret_err("ops->get_plmn_selection_mode",
			ops->get_plmn_selection_mode, TCORE_RETURN_ENOSYS);

		return ops->get_plmn_selection_mode(co, ur);

	case TREQ_NETWORK_SET_SERVICE_DOMAIN:
		tcore_check_null_ret_err("ops->set_service_domain",
			ops->set_service_domain, TCORE_RETURN_ENOSYS);

		return ops->set_service_domain(co, ur);

	case TREQ_NETWORK_GET_SERVICE_DOMAIN:
		tcore_check_null_ret_err("ops->get_service_domain",
			ops->get_service_domain, TCORE_RETURN_ENOSYS);

		return ops->get_service_domain(co, ur);

	case TREQ_NETWORK_SET_BAND:
		tcore_check_null_ret_err("ops->set_band",
			ops->set_band, TCORE_RETURN_ENOSYS);

		return ops->set_band(co, ur);

	case TREQ_NETWORK_GET_BAND:
		tcore_check_null_ret_err("ops->get_band",
			ops->get_band, TCORE_RETURN_ENOSYS);

		return ops->get_band(co, ur);

	case TREQ_NETWORK_SET_PREFERRED_PLMN:
		tcore_check_null_ret_err("ops->set_preferred_plmn",
			ops->set_preferred_plmn, TCORE_RETURN_ENOSYS);

		return ops->set_preferred_plmn(co, ur);

	case TREQ_NETWORK_GET_PREFERRED_PLMN:
		tcore_check_null_ret_err("ops->get_preferred_plmn",
			ops->get_preferred_plmn, TCORE_RETURN_ENOSYS);

		return ops->get_preferred_plmn(co, ur);

	case TREQ_NETWORK_SET_ORDER:
		tcore_check_null_ret_err("ops->set_order",
			ops->set_order, TCORE_RETURN_ENOSYS);

		return ops->set_order(co, ur);

	case TREQ_NETWORK_GET_ORDER:
		tcore_check_null_ret_err("ops->get_order",
			ops->get_order, TCORE_RETURN_ENOSYS);

		return ops->get_order(co, ur);

	case TREQ_NETWORK_SET_POWER_ON_ATTACH:
		tcore_check_null_ret_err("ops->set_power_on_attach",
			ops->set_power_on_attach, TCORE_RETURN_ENOSYS);

		return ops->set_power_on_attach(co, ur);

	case TREQ_NETWORK_GET_POWER_ON_ATTACH:
		tcore_check_null_ret_err("ops->get_power_on_attach",
			ops->get_power_on_attach, TCORE_RETURN_ENOSYS);

		return ops->get_power_on_attach(co, ur);

	case TREQ_NETWORK_SET_CANCEL_MANUAL_SEARCH:
		tcore_check_null_ret_err("ops->set_cancel_manual_search",
			ops->set_cancel_manual_search, TCORE_RETURN_ENOSYS);

		return ops->set_cancel_manual_search(co, ur);

	case TREQ_NETWORK_GET_SERVING_NETWORK:
		tcore_check_null_ret_err("ops->get_serving_network",
			ops->get_serving_network, TCORE_RETURN_ENOSYS);

		return ops->get_serving_network(co, ur);

	case TREQ_NETWORK_SET_MODE:
		tcore_check_null_ret_err("ops->set_mode",
			ops->set_mode, TCORE_RETURN_ENOSYS);

		return ops->set_mode(co, ur);

	case TREQ_NETWORK_GET_MODE:
		tcore_check_null_ret_err("ops->get_mode",
			ops->get_mode, TCORE_RETURN_ENOSYS);

		return ops->get_mode(co, ur);

	case TREQ_NETWORK_SET_NEIGHBORING_CELL_INFO:
		tcore_check_null_ret_err("ops->set_neighboring_cell_info",
			ops->set_neighboring_cell_info, TCORE_RETURN_ENOSYS);

		return ops->set_neighboring_cell_info(co, ur);

	case TREQ_NETWORK_GET_NEIGHBORING_CELL_INFO:
		tcore_check_null_ret_err("ops->get_neighboring_cell_info",
			ops->get_neighboring_cell_info, TCORE_RETURN_ENOSYS);

		return ops->get_neighboring_cell_info(co, ur);

	case TREQ_NETWORK_SET_DEFAULT_DATA_SUBSCRIPTION:
		tcore_check_null_ret_err("ops->set_default_data_subscription",
			ops->set_default_data_subscription, TCORE_RETURN_ENOSYS);

		return ops->set_default_data_subscription(co, ur);

	case TREQ_NETWORK_GET_DEFAULT_DATA_SUBSCRIPTION:
		tcore_check_null_ret_err("ops->get_default_data_subscription",
			ops->get_default_data_subscription, TCORE_RETURN_ENOSYS);

		return ops->get_default_data_subscription(co, ur);

	case TREQ_NETWORK_SET_DEFAULT_SUBSCRIPTION:
		tcore_check_null_ret_err("ops->set_default_subscription",
			ops->set_default_subscription, TCORE_RETURN_ENOSYS);

		return ops->set_default_subscription(co, ur);

	case TREQ_NETWORK_GET_DEFAULT_SUBSCRIPTION:
		tcore_check_null_ret_err("ops->get_default_subscription",
			ops->get_default_subscription, TCORE_RETURN_ENOSYS);

		return ops->get_default_subscription(co, ur);

	case TREQ_NETWORK_SET_EMERGENCY_CALLBACK_MODE:
		tcore_check_null_ret_err("ops->set_emergency_callback_mode",
			ops->set_emergency_callback_mode, TCORE_RETURN_ENOSYS);

		return ops->set_emergency_callback_mode(co, ur);

	case TREQ_NETWORK_SET_ROAMING_PREFERENCE:
		tcore_check_null_ret_err("ops->set_roaming_preference",
			ops->set_roaming_preference, TCORE_RETURN_ENOSYS);

		return ops->set_roaming_preference(co, ur);

	case TREQ_NETWORK_GET_ROAMING_PREFERENCE:
		tcore_check_null_ret_err("ops->get_roaming_preference",
			ops->get_roaming_preference, TCORE_RETURN_ENOSYS);

		return ops->get_roaming_preference(co, ur);

	case TREQ_NETWORK_GET_SUBSCRIPTION_INFO:
		tcore_check_null_ret_err("ops->get_subscription_info",
			ops->get_subscription_info, TCORE_RETURN_ENOSYS);

		return ops->get_subscription_info(co, ur);

	case TREQ_NETWORK_SEARCH_ECC_RAT:
		tcore_check_null_ret_err("ops->search_ecc_rat",
			ops->search_ecc_rat, TCORE_RETURN_ENOSYS);

		return ops->search_ecc_rat(co, ur);

	case TREQ_NETWORK_IMS_DEREGISTER:
		tcore_check_null_ret_err("ops->ims_deregister",
			ops->ims_deregister, TCORE_RETURN_ENOSYS);

		return ops->ims_deregister(co, ur);

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
		g_free(po);
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

	po = g_try_malloc0(sizeof(struct private_object_data));
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	/* set ops to default type when core object is created. */
	po->ops[TCORE_OPS_TYPE_CP] = ops;

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

void tcore_network_set_ops(CoreObject *o,
	struct tcore_network_operations *ops, enum tcore_ops_type ops_type)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_NETWORK);
	CORE_OBJECT_VALIDATE_OPS_RETURN(ops_type);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		err("po is NULL");
		return;
	}

	po->ops[ops_type] = ops;
}

char *tcore_network_get_plmn(CoreObject *co)
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


char *tcore_network_get_network_name(CoreObject *co,
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
			free(po->network_name_short);
			po->network_name_short = NULL;
		}

		if (network_name)
			po->network_name_short = g_strdup(network_name);

	break;

	case TCORE_NETWORK_NAME_TYPE_FULL:
		if (po->network_name_full) {
			free(po->network_name_full);
			po->network_name_full = NULL;
		}

		if (network_name)
			po->network_name_full = g_strdup(network_name);

	break;

	case TCORE_NETWORK_NAME_TYPE_SPN:
		if (po->network_name_spn) {
			free(po->network_name_spn);
			po->network_name_spn = NULL;
		}

		if (network_name)
			po->network_name_spn = g_strdup(network_name);

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

TReturn tcore_network_set_ims_reg_info(CoreObject *co,
	struct tel_network_ims_registration_info ims_reg_info)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->ims_reg_info.is_registered = ims_reg_info.is_registered;
	po->ims_reg_info.feature_mask = ims_reg_info.feature_mask;
	po->ims_reg_info.network_type = ims_reg_info.network_type;
	po->ims_reg_info.ecmp_mode = ims_reg_info.ecmp_mode;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_get_ims_reg_info(CoreObject *co,
	struct tel_network_ims_registration_info *ims_reg_info)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	if (!ims_reg_info)
		return TCORE_RETURN_EINVAL;

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	ims_reg_info->is_registered = po->ims_reg_info.is_registered;
	ims_reg_info->feature_mask = po->ims_reg_info.feature_mask;
	ims_reg_info->network_type = po->ims_reg_info.network_type;
	ims_reg_info->ecmp_mode = po->ims_reg_info.ecmp_mode;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_set_ims_voice_status(CoreObject *co, gboolean status)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->ims_voice_status = status;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_get_ims_voice_status(CoreObject *co, gboolean *status)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	if (!status)
		return TCORE_RETURN_EINVAL;

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	*status = po->ims_voice_status;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_set_ecc_rat_search_status(CoreObject *co,
	enum telephony_network_ecc_rat_search_status status)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->rat_search_status = status;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_get_ecc_rat_search_status(CoreObject *co,
	enum telephony_network_ecc_rat_search_status *status)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	if (!status)
		return TCORE_RETURN_EINVAL;

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	*status = po->rat_search_status;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_set_ecc_rat(CoreObject *co,
	enum telephony_network_access_technology ecc_rat)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->ecc_rat = ecc_rat;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_network_get_ecc_rat(CoreObject *co,
	enum telephony_network_access_technology *ecc_rat)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_NETWORK, TCORE_RETURN_EINVAL);

	if (!ecc_rat)
		return TCORE_RETURN_EINVAL;

	po = tcore_object_ref_object(co);
	if (!po)
		return TCORE_RETURN_EINVAL;

	*ecc_rat = po->ecc_rat;

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

	g_strlcpy(plmn, noi->mcc, NETWORK_MAX_MCC_LEN + 1);
	g_strlcpy(plmn + NETWORK_MAX_MCC_LEN, noi->mnc, NETWORK_MAX_MNC_LEN + 1);

	if (g_hash_table_lookup(po->operator_info_hash, plmn))
		g_hash_table_remove(po->operator_info_hash, plmn);

	g_hash_table_insert(po->operator_info_hash,
		g_strdup(plmn),
		g_memdup(noi, sizeof(struct tcore_network_operator_info)));

	return TCORE_RETURN_SUCCESS;
}

struct tcore_network_operator_info *
tcore_network_operator_info_find(CoreObject *co, const char *mcc, const char *mnc)
{
	struct private_object_data *po = NULL;
	struct tcore_network_operator_info *data;
	char plmn[NETWORK_MAX_PLMN_LEN+1];
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

	g_strlcpy(plmn, mcc, NETWORK_MAX_MCC_LEN + 1);
	g_strlcpy(plmn + NETWORK_MAX_MCC_LEN, mnc, NETWORK_MAX_MNC_LEN + 1);

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

void tcore_network_update_mcc_mnc_oper_list(CoreObject *co,
	const char *plmn, const char *name, enum tcore_network_operator_info_type type)
{
	char mcc[NETWORK_MAX_MCC_LEN+1] = { 0, };
	const char *mnc;
	struct tcore_network_operator_info *noi = NULL;

	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_NETWORK);

	if (!plmn) {
		err("plmn is NULL");
		return;
	}

	if (strlen(plmn) < NETWORK_MAX_MCC_LEN+1) {
		err("strlen of plmn is %d", strlen(plmn));
		return;
	}

	memcpy(mcc, plmn, NETWORK_MAX_MCC_LEN);
	mnc = plmn + NETWORK_MAX_MCC_LEN;

	if (atoi(mcc) == 0) {
		err("atoi(mcc) is Zero. mcc[%s]", mcc);
		return;
	}

	noi = g_try_malloc0(sizeof(struct tcore_network_operator_info));
	if (noi == NULL) {
		err("Memory alloc failed");
		return;
	}

	g_snprintf(noi->mcc, NETWORK_MAX_MCC_LEN+1, "%s", mcc);
	g_snprintf(noi->mnc, NETWORK_MAX_MNC_LEN+1, "%s", mnc);
	g_snprintf(noi->name, NETWORK_MAX_NETWORK_NAME_LEN+1, "%s", name);
	noi->type = type;

	info("[NETINFO] Update mcc[%s] mnc[%s] name[%s] type[%d] oper_list", noi->mcc, noi->mnc, noi->name, noi->type);
	tcore_network_operator_info_add(co, noi);

	g_free(noi);
}

/* Loads operator info from database */
gboolean tcore_network_load_mcc_mnc_oper_list_from_db(TcorePlugin *p,
	CoreObject *co, const char *mcc, const char *mnc, const char *db_path)
{
	Server *s;
	Storage *strg;
	void *handle;
	char query[256] = {0, };
	char mcc_str[NETWORK_MAX_MCC_LEN+1] = { 0, };
	GHashTableIter iter;
	gpointer key, value;
	GHashTable *result = NULL, *row = NULL;
	struct tcore_network_operator_info *noi = NULL;
	int count = 0;

	s = tcore_plugin_ref_server(p);
	strg = tcore_server_find_storage(s, "database");

	handle = tcore_storage_create_handle(strg, db_path);
	if (!handle) {
		err("fail to create database handle");
		return FALSE;
	}

	g_snprintf(query, 255,
		"select country, mcc, mnc, oper from mcc_mnc_oper_list where mcc='%s' and mnc='%s'",
		mcc, mnc);
	dbg("query = [%s]", query);

	result = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
		(GDestroyNotify) g_hash_table_destroy);
	if (!result) {
		err("fail to create new hash table");
		tcore_storage_remove_handle(strg, handle);
		return FALSE;
	}

	if (tcore_storage_read_query_database(strg, handle, query, NULL, result, 4) == FALSE) {
		tcore_storage_remove_handle(strg, handle);
		g_hash_table_destroy(result);
		return FALSE;
	}

	g_hash_table_iter_init(&iter, result);
	noi = g_try_malloc0(sizeof(struct tcore_network_operator_info));
	if (noi == NULL) {
		err("Memory alloc failed");
		tcore_storage_remove_handle(strg, handle);
		g_hash_table_destroy(result);
		return FALSE;
	}

	while (g_hash_table_iter_next(&iter, &key, &value) == TRUE) {
		row = value;

		memset(noi, 0x0, sizeof(struct tcore_network_operator_info));
		memset(mcc_str, 0, sizeof(mcc_str));

		g_snprintf(mcc_str, NETWORK_MAX_MCC_LEN+1, "%s", (char *)g_hash_table_lookup(row, "1"));
		if (strlen(mcc_str) == 1)
			g_snprintf(noi->mcc, NETWORK_MAX_MCC_LEN+1, "00%s", mcc_str);
		else if (strlen(mcc_str) == 2)
			g_snprintf(noi->mcc, NETWORK_MAX_MCC_LEN+1, "0%s", mcc_str);
		else
			g_snprintf(noi->mcc, NETWORK_MAX_MCC_LEN+1, "%s", mcc_str);
		g_snprintf(noi->mnc, NETWORK_MAX_MNC_LEN+1, "%s", (char *)g_hash_table_lookup(row, "2"));
		g_snprintf(noi->name, NETWORK_MAX_NETWORK_NAME_LEN+1, "%s", (char *)g_hash_table_lookup(row, "3"));
		g_snprintf(noi->country, NETWORK_MAX_COUNTRY_CODE_LEN+1, "%s", (char *)g_hash_table_lookup(row, "0"));
		if (g_strcmp0(NETWORK_MCC_MNC_OPER_LIST_GSMA_DB, db_path) == 0)
			noi->type = TCORE_NETWORK_OPERATOR_INFO_TYPE_TS25_DB;
		else
			noi->type = TCORE_NETWORK_OPERATOR_INFO_TYPE_DELTA_DB;
		info("[NETINFO] load name from DB : mcc[%s] mnc[%s] name[%s] type:[%d]", noi->mcc, noi->mnc, noi->name, noi->type);
		tcore_network_operator_info_add(co, noi);
		count++;
	}
	g_free(noi);

	g_hash_table_destroy(result);

	tcore_storage_remove_handle(strg, handle);

	if (count == 0)
		return FALSE;

	return TRUE;
}

/* Updates Operator info in the database */
void tcore_network_update_mcc_mnc_oper_list_db(TcorePlugin *p, const char *mcc, const char *mnc, const char *oper_name)
{
	Server *s;
	Storage *strg;
	void *handle;
	char query[256] = {0, };
	struct tcore_network_operator_info *noi = NULL;
	GHashTable *result, *row = NULL;
	GHashTableIter iter;
	gpointer key, value;
	gboolean field_exist = FALSE;

	if (G_UNLIKELY(!mcc || !mnc || !oper_name || !p)) {
		err("Input Parameter Addresses : mcc[%p] mnc[%p] oper_name[%p] plugin[%p]", mcc, mnc, oper_name, p);
		return;
	}

	if (G_UNLIKELY(strlen(oper_name) == 0)) {
		err("strlen of oper_name is Zero");
		return;
	}

	dbg("mcc[%s] mnc[%s] oper_name[%s]", mcc, mnc, oper_name);

	/* 1. Get DB handle */
	s = tcore_plugin_ref_server(p);
	strg = tcore_server_find_storage(s, "database");

	handle = tcore_storage_create_handle(strg, NETWORK_MCC_MNC_OPER_LIST_GSMA_DB);
	if (!handle) {
		err("fail to create database handle");
		return;
	}

	/* 2. Check duplicated network operator name in DB */
	g_snprintf(query, 255,
		"select country, mcc, mnc, oper from mcc_mnc_oper_list where mcc='%s' and mnc='%s'",
		mcc, mnc);
	dbg("query = [%s]", query);

	result = g_hash_table_new_full(g_str_hash, g_str_equal, NULL,
		(GDestroyNotify) g_hash_table_destroy);
	if (!result) {
		err("fail to create new hash table");
		tcore_storage_remove_handle(strg, handle);
		return;
	}

	tcore_storage_read_query_database(strg, handle, query, NULL, result, 4);

	g_hash_table_iter_init(&iter, result);
	noi = g_try_malloc0(sizeof(struct tcore_network_operator_info));
	if (noi == NULL) {
		err("Memory alloc failed");
		tcore_storage_remove_handle(strg, handle);
		g_hash_table_destroy(result);
		return;
	}
	while (g_hash_table_iter_next(&iter, &key, &value) == TRUE) {
		row = value;

		memset(noi, 0x0, sizeof(struct tcore_network_operator_info));

		g_snprintf(noi->mcc, NETWORK_MAX_MCC_LEN+1, "%s", (char *)g_hash_table_lookup(row, "1"));
		g_snprintf(noi->mnc, NETWORK_MAX_MNC_LEN+1, "%s", (char *)g_hash_table_lookup(row, "2"));
		g_snprintf(noi->name, NETWORK_MAX_NETWORK_NAME_LEN+1, "%s", (char *)g_hash_table_lookup(row, "3"));
		g_snprintf(noi->country, NETWORK_MAX_COUNTRY_CODE_LEN+1, "%s", (char *)g_hash_table_lookup(row, "0"));

		info("This field (mcc[%s] mnc[%s] name[%s]) will be updated ", noi->mcc, noi->mnc, noi->name);
		field_exist = TRUE;
	}
	g_free(noi);
	g_hash_table_destroy(result);

	/* 3. Update Network Operator name DB */
	result = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, g_free);
	if (!result) {
		err("fail to create new hash table");
		tcore_storage_remove_handle(strg, handle);
		return;
	}

	if (field_exist) {
		/* Update DB */
		snprintf(query, 255, "update mcc_mnc_oper_list set oper = ? where mcc = ? and mnc = ?");
		dbg("query[%s]", query);

		g_hash_table_insert(result, (gpointer)"1", g_strdup(oper_name));
		g_hash_table_insert(result, (gpointer)"2", g_strdup(mcc));
		g_hash_table_insert(result, (gpointer)"3", g_strdup(mnc));

		if (tcore_storage_update_query_database(strg, handle, query, result) == FALSE)
			err("update query failed");
	} else {
		/* Insert DB */
		snprintf(query, 255, "insert into mcc_mnc_oper_list (country, mcc, mnc, oper) values (?, ?, ?, ?)");
		dbg("query[%s]", query);

		g_hash_table_insert(result, (gpointer)"1", g_strdup("__"));
		g_hash_table_insert(result, (gpointer)"2", g_strdup(mcc));
		g_hash_table_insert(result, (gpointer)"3", g_strdup(mnc));
		g_hash_table_insert(result, (gpointer)"4", g_strdup(oper_name));

		if (tcore_storage_insert_query_database(strg, handle, query, result) == FALSE)
			err("insert query failed");
	}

	g_hash_table_destroy(result);
	tcore_storage_remove_handle(strg, handle);
}
