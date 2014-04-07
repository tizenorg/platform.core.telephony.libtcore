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
#include "util.h"
#include "plugin.h"
#include "co_context.h"

#define DEVNAME_LEN_MAX 16

typedef struct {
	TcoreContextState state;
	guint id;
	TcoreContextRole role;

	gchar *apn;
	gchar *addr;
	TcoreContextType type;
	TcoreContextDComp d_comp;
	TcoreContextHComp h_comp;

	gchar *username;
	gchar *password;
	gchar *dns1;
	gchar *dns2;
	TcoreContextAuth auth;

	TcoreIp4Type ip_v4;
	TcoreIp4Type gateway_v4;
	TcoreIp4Type dns_primary_v4;
	TcoreIp4Type dns_secondary_v4;

	/* IPv6 will be supported */

	gchar *proxy;
	gchar *mmsurl;
	gchar *profile_name;
	gchar devname[DEVNAME_LEN_MAX];
} PrivateObject;

static void __context_set_ipv4_atoi(guchar *ip4, const gchar *str)
{
	gchar *token = NULL;
	gchar *temp = NULL;
	guint index = 0;

	temp = tcore_strdup(str);
	token = strtok(temp, ".");
	while (token != NULL) {
		ip4[index++] = atoi(token);
		msg("	[%c]", ip4[index-1]);
		token = strtok(NULL, ".");
	}
	tcore_free(temp);
}

static void __po_free_hook(CoreObject *co)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return(po != NULL);

	tcore_free(po->apn);
	tcore_free(po->addr);
	tcore_free(po->username);
	tcore_free(po->password);
	tcore_free(po->dns1);
	tcore_free(po->dns2);
	tcore_free(po->proxy);
	tcore_free(po->mmsurl);
	tcore_free(po->profile_name);
	tcore_free(po);

	tcore_object_link_object(co, NULL);
}

CoreObject *tcore_context_new(TcorePlugin *p, TcoreHal *hal)
{
	CoreObject *co = NULL;
	PrivateObject *po = NULL;

	tcore_check_return_value_assert(p != NULL, NULL);

	co = tcore_object_new(p, hal);
	tcore_check_return_value_assert(co != NULL, NULL);

	po = tcore_malloc0(sizeof(PrivateObject));
	po->type = TCORE_CONTEXT_TYPE_IP;
	po->d_comp = TCORE_CONTEXT_D_COMP_OFF;
	po->h_comp = TCORE_CONTEXT_H_COMP_OFF;
	po->role = TCORE_CONTEXT_ROLE_UNKNOWN;
	po->auth = TCORE_CONTEXT_AUTH_NONE;

	tcore_object_set_type(co, CORE_OBJECT_TYPE_PS_CONTEXT);
	tcore_object_link_object(co, po);
	tcore_object_set_free_hook(co, __po_free_hook);

	return co;
}

void tcore_context_free(CoreObject *co)
{
	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_PS_CONTEXT);
	tcore_object_free(co);
}

gboolean tcore_context_set_state(CoreObject *co, TcoreContextState state)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	dbg("Set State: [%s]",
		((state == TCORE_CONTEXT_STATE_ACTIVATED) ? "ACTIVATED"
		: (state == TCORE_CONTEXT_STATE_ACTIVATING) ? "ACTIVATING"
		: (state == TCORE_CONTEXT_STATE_DEACTIVATED) ? "DEACTIVATED"
		: "DEACTIVATING"));

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->state = state;
	return TRUE;
}

gboolean tcore_context_get_state(CoreObject *co, TcoreContextState *state)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*state = po->state;
	return TRUE;
}

gboolean tcore_context_set_id(CoreObject *co, guint id)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->id = id;

	return TRUE;
}

gboolean tcore_context_get_id(CoreObject *co, guint *id)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*id = po->id;
	return TRUE;
}

gboolean tcore_context_set_apn(CoreObject *co, const gchar *apn)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	tcore_free(po->apn);
	po->apn = tcore_strdup(apn);
	return TRUE;
}

gboolean tcore_context_get_apn(CoreObject *co, gchar **apn)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);
	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	*apn =  tcore_strdup(po->apn);
	return TRUE;
}

gboolean tcore_context_set_role(CoreObject *co, TcoreContextRole role)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->role = role;
	return TRUE;
}

gboolean tcore_context_get_role(CoreObject *co, TcoreContextRole *role)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*role = po->role;
	return TRUE;
}

gboolean tcore_context_set_type(CoreObject *co, TcoreContextType type)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->type = type;
	return TRUE;
}

gboolean tcore_context_get_type(CoreObject *co, TcoreContextType *type)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*type = po->type;
	return TRUE;
}

gboolean tcore_context_set_data_compression(CoreObject *co, TcoreContextDComp comp)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->d_comp = comp;
	return TRUE;
}

gboolean tcore_context_get_data_compression(CoreObject *co, TcoreContextDComp *comp)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*comp = po->d_comp;
	return TRUE;
}

gboolean tcore_context_set_header_compression(CoreObject *co, TcoreContextHComp comp)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->h_comp = comp;
	return TRUE;
}

gboolean tcore_context_get_header_compression(CoreObject *co, TcoreContextHComp *comp)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*comp = po->h_comp;
	return TRUE;
}

gboolean tcore_context_set_username(CoreObject *co, const gchar *username)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	tcore_free(po->username);
	po->username = tcore_strdup(username);
	return TRUE;
}

gboolean tcore_context_get_username(CoreObject *co, gchar **username)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*username = tcore_strdup(po->username);
	return TRUE;
}

gboolean tcore_context_set_password(CoreObject *co, const gchar *password)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	tcore_free(po->password);
	po->password = tcore_strdup(password);
	return TRUE;
}

gboolean tcore_context_get_password(CoreObject *co, gchar **password)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*password =  tcore_strdup(po->password);
	return TRUE;
}

gboolean tcore_context_set_auth(CoreObject *co, TcoreContextAuth auth)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->auth = auth;
	return TRUE;
}

gboolean tcore_context_get_auth(CoreObject *co, TcoreContextAuth *auth)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*auth = po->auth;
	return TRUE;
}

gboolean tcore_context_set_proxy(CoreObject *co, const gchar *proxy)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	tcore_free(po->proxy);
	po->proxy = tcore_strdup(proxy);
	return TRUE;
}

gboolean tcore_context_get_proxy(CoreObject *co, gchar **proxy)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*proxy = tcore_strdup(po->proxy);
	return TRUE;
}

gboolean tcore_context_set_mmsurl(CoreObject *co, const gchar *mmsurl)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	tcore_free(po->mmsurl);
	po->mmsurl = tcore_strdup(mmsurl);
	return TRUE;
}

gboolean tcore_context_get_mmsurl(CoreObject *co, gchar **mmsurl)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*mmsurl = tcore_strdup(po->mmsurl);
	return TRUE;
}

gboolean tcore_context_set_profile_name(CoreObject *co, const gchar *profile_name)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	tcore_free(po->profile_name);
	po->profile_name = tcore_strdup(profile_name);
	return TRUE;
}

gboolean tcore_context_get_profile_name(CoreObject *co, gchar **profile_name)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*profile_name =  tcore_strdup(po->profile_name);
	return TRUE;
}

gboolean tcore_context_set_devinfo(CoreObject *co, TcorePsPdpIpConf *devinfo)
{
	PrivateObject *po = NULL;

	dbg("Setup device information");

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(devinfo != NULL, FALSE);

	memcpy(&(po->ip_v4), devinfo->ip_address, sizeof(TcoreIp4Type));
	memcpy(&(po->dns_primary_v4), devinfo->primary_dns, sizeof(TcoreIp4Type));
	memcpy(&(po->dns_secondary_v4), devinfo->secondary_dns, sizeof(TcoreIp4Type));
	memcpy(&(po->gateway_v4), devinfo->gateway, sizeof(TcoreIp4Type));
	memcpy(po->devname, devinfo->devname, sizeof(gchar) * 16);

	msg("	IP Address: [0x%x]", po->ip_v4);
	msg("	DNS - Primary: [0x%x] Secondary: [0x%x]", po->dns_primary_v4, po->dns_secondary_v4);
	msg("	Gateway: [0x%x]", po->gateway_v4);
	msg("	Device Name: [%s]", po->devname);

	return TRUE;
}

gboolean tcore_context_reset_devinfo(CoreObject *co)
{
	PrivateObject *po = NULL;

	dbg("Reset device information");

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	memset(&(po->ip_v4), 0, sizeof(TcoreIp4Type));
	memset(&(po->dns_primary_v4), 0, sizeof(TcoreIp4Type));
	memset(&(po->dns_secondary_v4), 0, sizeof(TcoreIp4Type));
	memset(&(po->gateway_v4), 0, sizeof(TcoreIp4Type));
	memset(po->devname, 0, sizeof(gchar) * 16);

	return TRUE;
}

void tcore_context_cp_service_info(CoreObject *dest, CoreObject *src)
{
	PrivateObject *d_po = NULL;
	PrivateObject *s_po = NULL;

	CORE_OBJECT_CHECK(dest, CORE_OBJECT_TYPE_PS_CONTEXT);
	CORE_OBJECT_CHECK(src, CORE_OBJECT_TYPE_PS_CONTEXT);

	d_po = tcore_object_ref_object(dest);
	s_po = tcore_object_ref_object(src);

	d_po->state = s_po->state;
	d_po->id = s_po->id;
	memcpy(&(d_po->ip_v4), &(s_po->ip_v4), sizeof(TcoreIp4Type));
	memcpy(&(d_po->dns_primary_v4), &(s_po->dns_primary_v4), sizeof(TcoreIp4Type));
	memcpy(&(d_po->dns_secondary_v4), &(s_po->dns_secondary_v4), sizeof(TcoreIp4Type));
	memcpy(&(d_po->gateway_v4), &(s_po->gateway_v4), sizeof(TcoreIp4Type));
	memcpy(d_po->devname, s_po->devname, sizeof(gchar) * 16);

	return;
}

gboolean tcore_context_set_ipv4_addr(CoreObject *co, const gchar *addr)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	if (po->addr) {
		tcore_free(po->addr);
		po->addr = NULL;
	}

	if (addr) {
		po->addr = tcore_strdup(addr);
		__context_set_ipv4_atoi(po->ip_v4.s, addr);
		dbg("IP Address: [%s]", addr);
	}

	return TRUE;
}

gboolean tcore_context_get_ipv4_addr(CoreObject *co, gchar **ip)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*ip =  tcore_util_get_string_by_ip4type(po->ip_v4);
	return TRUE;
}

gboolean tcore_context_set_ipv4_dns(CoreObject *co, const gchar *dns1, const gchar *dns2)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	tcore_free(po->dns1);
	po->dns1 = NULL;

	tcore_free(po->dns2);
	po->dns2 = NULL;

	if (dns1) {
		po->dns1 = tcore_strdup(dns1);
		__context_set_ipv4_atoi(po->dns_primary_v4.s, dns1);
	}

	if (dns2) {
		po->dns2 = tcore_strdup(dns2);
		__context_set_ipv4_atoi(po->dns_secondary_v4.s, dns2);
	}

	return TRUE;
}

gboolean tcore_context_get_ipv4_dns1(CoreObject *co, gchar **ip)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*ip = tcore_util_get_string_by_ip4type(po->dns_primary_v4);
	return TRUE;
}

gboolean tcore_context_get_ipv4_dns2(CoreObject *co, gchar **ip)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*ip = tcore_util_get_string_by_ip4type(po->dns_secondary_v4);
	return TRUE;
}

gboolean tcore_context_get_ipv4_gw(CoreObject *co, gchar **ip)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*ip =  tcore_util_get_string_by_ip4type(po->gateway_v4);
	return TRUE;
}

gboolean tcore_context_set_ipv4_devname(CoreObject *co, const gchar *name)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	if (name) {
		snprintf(po->devname, DEVNAME_LEN_MAX, "%s", name);
	}

	return TRUE;
}

gboolean tcore_context_get_ipv4_devname(CoreObject *co, gchar **dev_name)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_PS_CONTEXT, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	if (po->devname[0] == 0)
		return TRUE;

	*dev_name = tcore_strdup(po->devname);
	return TRUE;
}
