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

#ifndef __TCORE_CO_NETWORK_H__
#define __TCORE_CO_NETWORK_H__

#include <core_object.h>

__BEGIN_DECLS

/*
 * Manual PLMN property
 *
 * Value: MCC+MNC or Empty(Unknown mode)
 */
#define PROP_NET_MANUAL_PLMN  "manual_plmn"

/*
 * Manual Selection status property
 *
 * Value: 'waiting' / 'success' / 'failure' / Empty(Unknown state)
 */
#define PROP_NET_MANUAL_SELECTION_STATUS  "manual_selection_status"

/*
 * Network Registration Reject Cause Property
 *
 * Value: LU registration reject cause strings
 */
#define PROP_NET_REG_LU_REJECT_CAUSE  "reg_reject_cause"

/*
 * Read mdn Property
 *
 * Value: mdn strings
 */
#define PROP_NET_READ_MDN_FOR_ACTIVATION  "mdn_for_activation"

/*
 * Read pESN Property
 *
 * Value: pESN strings
 */
#define PROP_MODEM_READ_ESN_FOR_ACTIVATION  "esn_for_activation"

#define NETWORK_MCC_MNC_OPER_LIST_GSMA_DB "/opt/dbspace/.mcc_mnc_oper_list.db"

enum tcore_network_service_domain_type {
	TCORE_NETWORK_SERVICE_DOMAIN_TYPE_CIRCUIT,
	TCORE_NETWORK_SERVICE_DOMAIN_TYPE_PACKET
};

enum tcore_network_name_type {
	TCORE_NETWORK_NAME_TYPE_SHORT,
	TCORE_NETWORK_NAME_TYPE_FULL,
	TCORE_NETWORK_NAME_TYPE_SPN,
};

enum tcore_network_name_priority {
	TCORE_NETWORK_NAME_PRIORITY_UNKNOWN,
	TCORE_NETWORK_NAME_PRIORITY_NETWORK,
	TCORE_NETWORK_NAME_PRIORITY_SPN,
	TCORE_NETWORK_NAME_PRIORITY_ANY,
};

enum tcore_network_operator_info_type {
	TCORE_NETWORK_OPERATOR_INFO_TYPE_TS25_DB = 0,
	TCORE_NETWORK_OPERATOR_INFO_TYPE_DELTA_DB,
	TCORE_NETWORK_OPERATOR_INFO_TYPE_NITZ,
	TCORE_NETWORK_OPERATOR_INFO_TYPE_EONS,
};

struct tcore_network_operator_info {
	enum tcore_network_operator_info_type type;
	char mcc[NETWORK_MAX_MCC_LEN+1];
	char mnc[NETWORK_MAX_MNC_LEN+1];
	char name[NETWORK_MAX_NETWORK_NAME_LEN+1];
	char country[NETWORK_MAX_COUNTRY_CODE_LEN+1];
};

struct tcore_network_operations {
	TReturn (*search)(CoreObject *o, UserRequest *ur);
	TReturn (*set_plmn_selection_mode)(CoreObject *o, UserRequest *ur);
	TReturn (*get_plmn_selection_mode)(CoreObject *o, UserRequest *ur);
	TReturn (*set_service_domain)(CoreObject *o, UserRequest *ur);
	TReturn (*get_service_domain)(CoreObject *o, UserRequest *ur);
	TReturn (*set_band)(CoreObject *o, UserRequest *ur);
	TReturn (*get_band)(CoreObject *o, UserRequest *ur);
	TReturn (*set_preferred_plmn)(CoreObject *o, UserRequest *ur);
	TReturn (*get_preferred_plmn)(CoreObject *o, UserRequest *ur);
	TReturn (*set_order)(CoreObject *o, UserRequest *ur);
	TReturn (*get_order)(CoreObject *o, UserRequest *ur);
	TReturn (*set_power_on_attach)(CoreObject *o, UserRequest *ur);
	TReturn (*get_power_on_attach)(CoreObject *o, UserRequest *ur);
	TReturn (*set_cancel_manual_search)(CoreObject *o, UserRequest *ur);
	TReturn (*get_serving_network)(CoreObject *o, UserRequest *ur);
	TReturn (*set_mode)(CoreObject *o, UserRequest *ur);
	TReturn (*get_mode)(CoreObject *o, UserRequest *ur);
	TReturn (*set_neighboring_cell_info)(CoreObject *o, UserRequest *ur);
	TReturn (*get_neighboring_cell_info)(CoreObject *o, UserRequest *ur);
	TReturn (*set_default_data_subscription)(CoreObject *o, UserRequest *ur);
	TReturn (*get_default_data_subscription)(CoreObject *o, UserRequest *ur);
	TReturn (*set_default_subscription)(CoreObject *o, UserRequest *ur);
	TReturn (*get_default_subscription)(CoreObject *o, UserRequest *ur);
	TReturn (*set_emergency_callback_mode)(CoreObject *o, UserRequest *ur); /* 3GPP2 spcefic */
	TReturn (*set_roaming_preference)(CoreObject *o, UserRequest *ur); /* 3GPP2 spcefic */
	TReturn (*get_roaming_preference)(CoreObject *o, UserRequest *ur); /* 3GPP2 spcefic */
	TReturn (*get_subscription_info)(CoreObject *o, UserRequest *ur); /* 3GPP2 spcefic */
	TReturn (*search_ecc_rat)(CoreObject *o, UserRequest *ur);
	TReturn (*ims_deregister)(CoreObject *o, UserRequest *ur);
};


CoreObject *tcore_network_new(TcorePlugin *plugin, const char *name,
	struct tcore_network_operations *ops, TcoreHal *hal);
void tcore_network_free(CoreObject *co);

void tcore_network_set_ops(CoreObject *o,
	struct tcore_network_operations *ops, enum tcore_ops_type ops_type);

TReturn tcore_network_set_plmn(CoreObject *co, const char *plmn);
char *tcore_network_get_plmn(CoreObject *co);

TReturn tcore_network_set_network_name(CoreObject *co,
	enum tcore_network_name_type type, const char *network_name);
char *tcore_network_get_network_name(CoreObject *co,
	enum tcore_network_name_type type);

TReturn tcore_network_set_network_name_priority(CoreObject *co,
	enum tcore_network_name_priority priority);
TReturn tcore_network_get_network_name_priority(CoreObject *co,
	enum tcore_network_name_priority *priority);

TReturn tcore_network_set_roaming_state(CoreObject *co, gboolean state);
gboolean tcore_network_get_roaming_state(CoreObject *co);

TReturn tcore_network_set_restricted_state(CoreObject *co, int state);
int tcore_network_get_restricted_state(CoreObject *co);

TReturn tcore_network_set_service_status(CoreObject *co,
	enum tcore_network_service_domain_type type,
	enum telephony_network_service_domain_status status);
TReturn tcore_network_get_service_status(CoreObject *co,
	enum tcore_network_service_domain_type type,
	enum telephony_network_service_domain_status *result);

TReturn tcore_network_set_access_technology(CoreObject *co,
	enum telephony_network_access_technology act);
TReturn tcore_network_get_access_technology(CoreObject *co,
	enum telephony_network_access_technology *result);

TReturn tcore_network_set_lac(CoreObject *co, unsigned int lac);
TReturn tcore_network_get_lac(CoreObject *co, unsigned int *result);

TReturn tcore_network_set_rac(CoreObject *co, unsigned int rac);
TReturn tcore_network_get_rac(CoreObject *co, unsigned int *result);

TReturn tcore_network_set_cell_id(CoreObject *co, unsigned int cell_id);
TReturn tcore_network_get_cell_id(CoreObject *co, unsigned int *result);

TReturn tcore_network_set_gsm_dtm_support(CoreObject *co, gboolean state);
gboolean tcore_network_get_gsm_dtm_support(CoreObject *co);

TReturn tcore_network_set_service_type(CoreObject *co,
	enum telephony_network_service_type service_type);
TReturn tcore_network_get_service_type(CoreObject *co,
	enum telephony_network_service_type *result);
TReturn tcore_network_set_ims_reg_info(CoreObject *co,
        struct tel_network_ims_registration_info ims_reg_info);
TReturn tcore_network_get_ims_reg_info(CoreObject *co,
        struct tel_network_ims_registration_info *ims_reg_info);
TReturn tcore_network_set_ims_voice_status(CoreObject *co, gboolean status);
TReturn tcore_network_get_ims_voice_status(CoreObject *co, gboolean *status);
TReturn tcore_network_set_ecc_rat_search_status(CoreObject *co,
        enum telephony_network_ecc_rat_search_status status);
TReturn tcore_network_get_ecc_rat_search_status(CoreObject *co,
        enum telephony_network_ecc_rat_search_status *status);
TReturn tcore_network_set_ecc_rat(CoreObject *co,
        enum telephony_network_access_technology ecc_rat);
TReturn tcore_network_get_ecc_rat(CoreObject *co,
        enum telephony_network_access_technology *ecc_rat);
TReturn tcore_network_operator_info_add(CoreObject *co,
	const struct tcore_network_operator_info *noi);

struct tcore_network_operator_info *tcore_network_operator_info_find(CoreObject *co,
	const char *mcc, const char *mnc);
void tcore_network_update_mcc_mnc_oper_list(CoreObject *co,
	const char *plmn, const char *name, enum tcore_network_operator_info_type type);
gboolean tcore_network_load_mcc_mnc_oper_list_from_db(TcorePlugin *p,
	CoreObject *co, const char *mcc, const char *mnc, const char *db_path);
void tcore_network_update_mcc_mnc_oper_list_db(TcorePlugin *p, const char *mcc, const char *mnc, const char *oper_name);

__END_DECLS

#endif /* __TCORE_CO_NETWORK_H__ */
