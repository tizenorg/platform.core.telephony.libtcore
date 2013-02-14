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

#ifndef __TCORE_CO_CONTEXT_H__
#define __TCORE_CO_CONTEXT_H__

#include <core_object.h>

__BEGIN_DECLS

enum co_context_state {
	CONTEXT_STATE_DEACTIVATED,
	CONTEXT_STATE_ACTIVATING,
	CONTEXT_STATE_ACTIVATED,
	CONTEXT_STATE_DEACTIVATING,
};

enum co_context_type {
	CONTEXT_TYPE_UNKNOWN,
	CONTEXT_TYPE_X25,
	CONTEXT_TYPE_IP,
	CONTEXT_TYPE_IHOST,
	CONTEXT_TYPE_PPP,
	CONTEXT_TYPE_IPV6,
};

enum co_context_role {
	CONTEXT_ROLE_UNKNOWN,
	CONTEXT_ROLE_INTERNET,
	CONTEXT_ROLE_MMS,
	CONTEXT_ROLE_PREPAID_INTERNET,
	CONTEXT_ROLE_PREPAID_MMS,
	CONTEXT_ROLE_TETHERING,
	CONTEXT_ROLE_USER_DEFINED,
};

enum co_context_d_comp {
	CONTEXT_D_COMP_OFF,
	CONTEXT_D_COMP_ON,
	CONTEXT_D_COMP_V42BIS,
	CONTEXT_D_COMP_V44,
};

enum co_context_h_comp {
	CONTEXT_H_COMP_OFF,
	CONTEXT_H_COMP_ON,
	CONTEXT_H_COMP_RFC1144,
	CONTEXT_H_COMP_RFC2507,
	CONTEXT_H_COMP_RFC3095
};

enum co_context_auth {
	CONTEXT_AUTH_NONE,
	CONTEXT_AUTH_PAP,
	CONTEXT_AUTH_CHAP
};

CoreObject*              tcore_context_new(TcorePlugin *p, const char *name, TcoreHal *hal);
void                     tcore_context_free(CoreObject *o);

TReturn                  tcore_context_set_state(CoreObject *o, enum co_context_state state);
enum co_context_state    tcore_context_get_state(CoreObject *o);
TReturn                  tcore_context_set_id(CoreObject *o, unsigned int id);
unsigned int             tcore_context_get_id(CoreObject *o);
TReturn                  tcore_context_set_role(CoreObject *o, enum co_context_role type);
enum co_context_role     tcore_context_get_role(CoreObject *o);

TReturn                  tcore_context_set_apn(CoreObject *o, const char *apn);
char*                    tcore_context_get_apn(CoreObject *o);
TReturn                  tcore_context_set_address(CoreObject *o, const char *addr);
char*                    tcore_context_get_address(CoreObject *o);
TReturn                  tcore_context_set_type(CoreObject *o, enum co_context_type type);
enum co_context_type     tcore_context_get_type(CoreObject *o);
TReturn                  tcore_context_set_data_compression(CoreObject *o, enum co_context_d_comp comp);
enum co_context_d_comp   tcore_context_get_data_compression(CoreObject *o);
TReturn                  tcore_context_set_header_compression(CoreObject *o, enum co_context_h_comp comp);
enum co_context_h_comp   tcore_context_get_header_compression(CoreObject *o);

TReturn                  tcore_context_set_username(CoreObject *o, const char *username);
char*                    tcore_context_get_username(CoreObject *o);
TReturn                  tcore_context_set_password(CoreObject *o, const char *password);
char*                    tcore_context_get_password(CoreObject *o);
TReturn                  tcore_context_set_dns1(CoreObject *o, const char *dns);
char*                    tcore_context_get_dns1(CoreObject *o);
TReturn                  tcore_context_set_dns2(CoreObject *o, const char *dns);
char*                    tcore_context_get_dns2(CoreObject *o);
TReturn                  tcore_context_set_auth(CoreObject *o, enum co_context_auth auth);
enum co_context_auth     tcore_context_get_auth(CoreObject *o);
TReturn                  tcore_context_set_proxy(CoreObject *o, const char *proxy);
char*                    tcore_context_get_proxy(CoreObject *o);
TReturn                  tcore_context_set_mmsurl(CoreObject *o, const char *mmsurl);
char*                    tcore_context_get_mmsurl(CoreObject *o);
TReturn                  tcore_context_set_profile_name(CoreObject *o, const char *profile_name);
char*                    tcore_context_get_profile_name(CoreObject *o);

TReturn                  tcore_context_set_devinfo(CoreObject *o, struct tnoti_ps_pdp_ipconfiguration *devinfo);
TReturn                  tcore_context_reset_devinfo(CoreObject *o);
void                     tcore_context_cp_service_info(CoreObject *dest, CoreObject *src);

char*                    tcore_context_get_ipv4_addr(CoreObject *o);
char*                    tcore_context_get_ipv4_dns1(CoreObject *o);
char*                    tcore_context_get_ipv4_dns2(CoreObject *o);
char*                    tcore_context_get_ipv4_gw(CoreObject *o);
char*                    tcore_context_get_ipv4_devname(CoreObject *o);

__END_DECLS

#endif
