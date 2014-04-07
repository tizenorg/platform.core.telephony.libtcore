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

#ifndef __CO_NETWORK_H__
#define __CO_NETWORK_H__

#include "core_object.h"
#include <tel_network.h>
#include <tel_return.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	TelReturn (*get_identity_info)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*search)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*cancel_search)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*select_automatic)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*select_manual)(CoreObject *co, const TelNetworkSelectManualInfo *sel_manual, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_selection_mode)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_preferred_plmn)(CoreObject *co, const TelNetworkPreferredPlmnInfo *pref_plmn, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_preferred_plmn)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_mode)(CoreObject *co, TelNetworkMode mode, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_mode)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_neighboring_cell_info)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
}TcoreNetworkOps;

CoreObject* tcore_network_new(TcorePlugin *plugin, TcoreNetworkOps *ops, TcoreHal *hal);
void tcore_network_free(CoreObject *co);

gboolean tcore_network_set_ops(CoreObject *co, TcoreNetworkOps *ops);
void tcore_network_override_ops(CoreObject *co, TcoreNetworkOps *ops);


gboolean tcore_network_get_cs_reg_status(CoreObject *co,
		TelNetworkRegStatus *cs_status);
gboolean tcore_network_set_cs_reg_status(CoreObject *co,
		TelNetworkRegStatus cs_status);

gboolean tcore_network_get_ps_reg_status(CoreObject *co,
		TelNetworkRegStatus *ps_status);
gboolean tcore_network_set_ps_reg_status(CoreObject *co,
		TelNetworkRegStatus ps_status);


gboolean tcore_network_get_access_technology(CoreObject *co,
		TelNetworkAct *act);
gboolean tcore_network_set_access_technology(CoreObject *co,
		TelNetworkAct act);

gboolean tcore_network_get_roam_state(CoreObject *co, gboolean *state);
gboolean tcore_network_set_roam_state(CoreObject *co, gboolean state);


gboolean tcore_network_get_lac(CoreObject *co, unsigned int *lac);
gboolean tcore_network_set_lac(CoreObject *co, unsigned int lac);

gboolean tcore_network_get_rac(CoreObject *co, unsigned int *rac);
gboolean tcore_network_set_rac(CoreObject *co, unsigned int rac);

gboolean tcore_network_get_cell_id(CoreObject *co, unsigned int *cell_id);
gboolean tcore_network_set_cell_id(CoreObject *co, unsigned int cell_id);

/* The calling function needs to free memory allocated at output parameter */
gboolean tcore_network_get_plmn(CoreObject *co, char **plmn);
gboolean tcore_network_set_plmn(CoreObject *co, const char *plmn);

gboolean tcore_network_get_short_name(CoreObject *co, char **short_name);
gboolean tcore_network_set_short_name(CoreObject *co, const char *short_name);

gboolean tcore_network_get_long_name(CoreObject *co, char **long_name);
gboolean tcore_network_set_long_name(CoreObject *co, const char *long_name);

gboolean tcore_network_get_operator_name(CoreObject *co,
		const char *plmn, char **name);
gboolean tcore_network_set_operator_name(CoreObject *co,
		const char *plmn, const char *name);

#ifdef __cplusplus
}
#endif

#endif
