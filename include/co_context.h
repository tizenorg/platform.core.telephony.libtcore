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

#ifndef __CO_CONTEXT_H__
#define __CO_CONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <core_object.h>
#include <tel_return.h>
#include "type/ps.h"

typedef enum {
	TCORE_CONTEXT_STATE_DEACTIVATED,
	TCORE_CONTEXT_STATE_DEFINING,
	TCORE_CONTEXT_STATE_DEFINED,
	TCORE_CONTEXT_STATE_ACTIVATING,
	TCORE_CONTEXT_STATE_ACTIVATED,
	TCORE_CONTEXT_STATE_DEACTIVATING,
} TcoreContextState;

typedef enum {
	TCORE_CONTEXT_TYPE_UNKNOWN,
	TCORE_CONTEXT_TYPE_X25,
	TCORE_CONTEXT_TYPE_IP,
	TCORE_CONTEXT_TYPE_IHOST,
	TCORE_CONTEXT_TYPE_PPP,
	TCORE_CONTEXT_TYPE_IPV6,
} TcoreContextType;

typedef enum {
	TCORE_CONTEXT_ROLE_UNKNOWN,
	TCORE_CONTEXT_ROLE_INTERNET,
	TCORE_CONTEXT_ROLE_MMS,
	TCORE_CONTEXT_ROLE_PREPAID_INTERNET,
	TCORE_CONTEXT_ROLE_PREPAID_MMS,
	TCORE_CONTEXT_ROLE_TETHERING,
	TCORE_CONTEXT_ROLE_USER_DEFINED,
} TcoreContextRole;

typedef enum {
	TCORE_CONTEXT_D_COMP_OFF,
	TCORE_CONTEXT_D_COMP_ON,
	TCORE_CONTEXT_D_COMP_V42BIS,
	TCORE_CONTEXT_D_COMP_V44,
} TcoreContextDComp;

typedef enum {
	TCORE_CONTEXT_H_COMP_OFF,
	TCORE_CONTEXT_H_COMP_ON,
	TCORE_CONTEXT_H_COMP_RFC1144,
	TCORE_CONTEXT_H_COMP_RFC2507,
	TCORE_CONTEXT_H_COMP_RFC3095
} TcoreContextHComp;

typedef enum {
	TCORE_CONTEXT_AUTH_NONE,
	TCORE_CONTEXT_AUTH_PAP,
	TCORE_CONTEXT_AUTH_CHAP
} TcoreContextAuth;

CoreObject *tcore_context_new(TcorePlugin *p, TcoreHal *hal);
void tcore_context_free(CoreObject *co);

gboolean tcore_context_get_state(CoreObject *co,  TcoreContextState *state);
gboolean tcore_context_set_state(CoreObject *co, TcoreContextState state);

gboolean tcore_context_get_id(CoreObject *co, guint *id);
gboolean tcore_context_set_id(CoreObject *co, guint id);

gboolean tcore_context_get_role(CoreObject *co, TcoreContextRole *role);
gboolean tcore_context_set_role(CoreObject *co, TcoreContextRole type);

gboolean tcore_context_get_apn(CoreObject *co, gchar **apn);
gboolean tcore_context_set_apn(CoreObject *co, const gchar *apn);

gboolean tcore_context_get_address(CoreObject *co, gchar **addr);
gboolean tcore_context_set_address(CoreObject *co, const gchar *addr);

gboolean tcore_context_get_dns1(CoreObject *co, gchar **dns);
gboolean tcore_context_set_dns1(CoreObject *co, const gchar *dns);

gboolean tcore_context_get_dns2(CoreObject *co, gchar **dns);
gboolean tcore_context_set_dns2(CoreObject *co, const gchar *dns);

gboolean tcore_context_get_type(CoreObject *co, TcoreContextType *type);
gboolean tcore_context_set_type(CoreObject *co, TcoreContextType type);

gboolean tcore_context_get_data_compression(CoreObject *co, TcoreContextDComp *comp);
gboolean tcore_context_set_data_compression(CoreObject *co, TcoreContextDComp comp);

gboolean tcore_context_get_header_compression(CoreObject *co, TcoreContextHComp *comp);
gboolean tcore_context_set_header_compression(CoreObject *co, TcoreContextHComp comp);

gboolean tcore_context_get_username(CoreObject *co, gchar **username);
gboolean tcore_context_set_username(CoreObject *co, const gchar *username);

gboolean tcore_context_get_password(CoreObject *co, gchar **password);
gboolean tcore_context_set_password(CoreObject *co, const gchar *password);

gboolean tcore_context_get_auth(CoreObject *co, TcoreContextAuth *auth);
gboolean tcore_context_set_auth(CoreObject *co, TcoreContextAuth auth);

gboolean tcore_context_get_proxy(CoreObject *co, gchar **proxy);
gboolean tcore_context_set_proxy(CoreObject *co, const gchar *proxy);

gboolean tcore_context_get_mmsurl(CoreObject *co, gchar **mmsurl);
gboolean tcore_context_set_mmsurl(CoreObject *co, const gchar *mmsurl);

gboolean tcore_context_get_profile_name(CoreObject *co, gchar **profile_name);
gboolean tcore_context_set_profile_name(CoreObject *co, const gchar *profile_name);

gboolean tcore_context_set_devinfo(CoreObject *co, TcorePsPdpIpConf *devinfo);
gboolean tcore_context_reset_devinfo(CoreObject *co);

gboolean tcore_context_get_ipv4_addr(CoreObject *co, gchar **ipv4);
gboolean tcore_context_set_ipv4_addr(CoreObject *co, const gchar *addr);

gboolean tcore_context_get_ipv4_dns1(CoreObject *co, gchar **ipv4_dns);
gboolean tcore_context_get_ipv4_dns2(CoreObject *co, gchar **ipv4_dns2);
gboolean tcore_context_set_ipv4_dns(CoreObject *co, const gchar *dns1, const gchar *dns2);

gboolean tcore_context_get_ipv4_gw(CoreObject *co, gchar **ipv4_gw);

gboolean tcore_context_get_ipv4_devname(CoreObject *co, gchar **dev_name);
gboolean tcore_context_set_ipv4_devname(CoreObject *co, const gchar *name);

void tcore_context_cp_service_info(CoreObject *dest, CoreObject *src);

#ifdef __cplusplus
}
#endif

#endif	/* __CO_CONTEXT_H__ */
