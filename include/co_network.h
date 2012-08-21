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

struct tcore_network_operator_info {
	char mcc[4];
	char mnc[4];
	char name[41];
	char country[4];
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
};


CoreObject* tcore_network_new(TcorePlugin *plugin, const char *name,
                struct tcore_network_operations *ops, TcoreHal *hal);
void        tcore_network_free(CoreObject *co);

TReturn     tcore_network_set_plmn(CoreObject *co, const char *plmn);
char*       tcore_network_get_plmn(CoreObject *co);

TReturn     tcore_network_set_network_name(CoreObject *co,
                enum tcore_network_name_type type, const char *network_name);
char*       tcore_network_get_network_name(CoreObject *co,
                enum tcore_network_name_type type);

TReturn     tcore_network_set_network_name_priority(CoreObject *co,
                enum tcore_network_name_priority priority);
TReturn     tcore_network_get_network_name_priority(CoreObject *co,
                enum tcore_network_name_priority *priority);

TReturn     tcore_network_set_roaming_state(CoreObject *co, gboolean state);
gboolean    tcore_network_get_roaming_state(CoreObject *co);

TReturn     tcore_network_set_service_status(CoreObject *co,
                enum tcore_network_service_domain_type type,
                enum telephony_network_service_domain_status status);
TReturn     tcore_network_get_service_status(CoreObject *co,
                enum tcore_network_service_domain_type type,
                enum telephony_network_service_domain_status *result);

TReturn     tcore_network_set_access_technology(CoreObject *co,
                enum telephony_network_access_technology act);
TReturn     tcore_network_get_access_technology(CoreObject *co,
                enum telephony_network_access_technology *result);

TReturn     tcore_network_set_lac(CoreObject *co, unsigned int lac);
TReturn     tcore_network_get_lac(CoreObject *co, unsigned int *result);

TReturn     tcore_network_set_rac(CoreObject *co, unsigned int rac);
TReturn     tcore_network_get_rac(CoreObject *co, unsigned int *result);

TReturn     tcore_network_set_cell_id(CoreObject *co, unsigned int cell_id);
TReturn     tcore_network_get_cell_id(CoreObject *co, unsigned int *result);

TReturn     tcore_network_set_service_type(CoreObject *co,
                enum telephony_network_service_type service_type);
TReturn     tcore_network_get_service_type(CoreObject *co,
                enum telephony_network_service_type *result);

TReturn     tcore_network_operator_info_add(CoreObject *co,
                struct tcore_network_operator_info *noi);

struct tcore_network_operator_info*
            tcore_network_operator_info_find(CoreObject *co,
                const char *mcc, const char *mnc);

__END_DECLS

#endif
