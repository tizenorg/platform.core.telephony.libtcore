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
#include "util.h"
#include "plugin.h"
#include "user_request.h"
#include "co_context.h"

#define DEVNAME_LEN_MAX 16

struct private_object_data {
	enum co_context_state state;
	unsigned int id;
	enum co_context_role role;

	char *apn;
	char *addr;
	enum co_context_type type;
	enum co_context_d_comp d_comp;
	enum co_context_h_comp h_comp;

	char *username;
	char *password;
	char *dns1;
	char *dns2;
	enum co_context_auth auth;

	union tcore_ip4_type ip_v4;
	union tcore_ip4_type gateway_v4;
	union tcore_ip4_type dns_primary_v4;
	union tcore_ip4_type dns_secondary_v4;

	/*IPv6 will be supported*/

	char *proxy;
	char *mmsurl;
	char *profile_name;
	char devname[DEVNAME_LEN_MAX];
};

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (po) {
		g_free(po);
		tcore_object_link_object(o, NULL);
	}
}

CoreObject *tcore_context_new(TcorePlugin *p, const char *name, TcoreHal *hal)
{
	CoreObject *o = NULL;
	struct private_object_data *po = NULL;

	if (!p)
		return NULL;

	o = tcore_object_new(p, name, hal);
	if (!o)
		return NULL;

	po = calloc(1, sizeof(struct private_object_data));
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	po->type = CONTEXT_TYPE_IP;
	po->d_comp = CONTEXT_D_COMP_OFF;
	po->h_comp = CONTEXT_H_COMP_OFF;
	po->role = CONTEXT_ROLE_UNKNOWN;
	po->auth = CONTEXT_AUTH_NONE;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_PS_CONTEXT);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);

	return o;
}

CoreObject *tcore_context_clone(TcorePlugin *p, const char *name, TcoreHal *hal)
{
	CoreObject *o = NULL;

	if (!p)
		return NULL;

	o = tcore_object_clone_template_object(p, name, CORE_OBJECT_TYPE_PS_CONTEXT);
	if (!o)
		return NULL;

	tcore_object_set_hal(o, hal);

	return o;
}

void tcore_context_free(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_PS_CONTEXT);

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	g_free(po);
	tcore_object_link_object(o, NULL);
	tcore_object_free(o);
}

TReturn tcore_context_set_state(CoreObject *o, enum co_context_state state)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->state = state;

	return TCORE_RETURN_SUCCESS;
}

enum co_context_state    tcore_context_get_state(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, 0);

	po = tcore_object_ref_object(o);
	if (!po)
		return 0;

	return po->state;
}

TReturn tcore_context_set_id(CoreObject *o, unsigned int id)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->id = id;

	return TCORE_RETURN_SUCCESS;
}

unsigned int tcore_context_get_id(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, 0);

	po = tcore_object_ref_object(o);
	if (!po)
		return 0;

	return po->id;
}

TReturn tcore_context_set_apn(CoreObject *o, const char *apn)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return FALSE;

	if (po->apn) {
		free(po->apn);
		po->apn = NULL;
	}

	if (apn) {
		po->apn = g_strdup(apn);
	}

	return TCORE_RETURN_SUCCESS;
}

char *tcore_context_get_apn(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, NULL);

	po = tcore_object_ref_object(o);
	if (!po)
		return NULL;

	if (!po->apn)
		return NULL;

	return g_strdup(po->apn);
}

TReturn tcore_context_set_role(CoreObject *o, enum co_context_role role)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->role = role;

	return TCORE_RETURN_SUCCESS;
}

enum co_context_role tcore_context_get_role(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, 0);

	po = tcore_object_ref_object(o);
	if (!po)
		return 0;

	return po->role;
}

TReturn tcore_context_set_type(CoreObject *o, enum co_context_type type)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->type = type;

	return TCORE_RETURN_SUCCESS;
}

enum co_context_type tcore_context_get_type(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, 0);

	po = tcore_object_ref_object(o);
	if (!po)
		return 0;

	return po->type;
}

TReturn tcore_context_set_data_compression(CoreObject *o, enum co_context_d_comp comp)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->d_comp = comp;

	return TCORE_RETURN_SUCCESS;
}

enum co_context_d_comp tcore_context_get_data_compression(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, 0);

	po = tcore_object_ref_object(o);
	if (!po)
		return 0;

	return po->d_comp;
}

TReturn tcore_context_set_header_compression(CoreObject *o, enum co_context_h_comp comp)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->h_comp = comp;

	return TCORE_RETURN_SUCCESS;
}

enum co_context_h_comp tcore_context_get_header_compression(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, 0);

	po = tcore_object_ref_object(o);
	if (!po)
		return 0;

	return po->h_comp;
}

TReturn tcore_context_set_username(CoreObject *o, const char *username)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (po->username) {
		free(po->username);
		po->username = NULL;
	}

	if (username) {
		po->username = g_strdup(username);
	}

	return TCORE_RETURN_SUCCESS;
}

char *tcore_context_get_username(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, NULL);

	po = tcore_object_ref_object(o);
	if (!po)
		return NULL;

	if (!po->username)
		return NULL;

	return g_strdup(po->username);
}

TReturn tcore_context_set_password(CoreObject *o, const char *password)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (po->password) {
		free(po->password);
		po->password = NULL;
	}

	if (password) {
		po->password = g_strdup(password);
	}

	return TCORE_RETURN_SUCCESS;
}

char *tcore_context_get_password(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, NULL);

	po = tcore_object_ref_object(o);
	if (!po)
		return NULL;

	if (!po->password)
		return NULL;

	return g_strdup(po->password);
}

TReturn tcore_context_set_auth(CoreObject *o, enum co_context_auth auth)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	po->auth = auth;

	return TCORE_RETURN_SUCCESS;
}

enum co_context_auth tcore_context_get_auth(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, 0);

	po = tcore_object_ref_object(o);
	if (!po)
		return 0;

	return po->auth;
}

TReturn tcore_context_set_proxy(CoreObject *o, const char *proxy)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return FALSE;

	if (po->proxy) {
		free(po->proxy);
		po->apn = NULL;
	}

	if (proxy) {
		po->proxy = g_strdup(proxy);
	}

	return TCORE_RETURN_SUCCESS;
}

char *tcore_context_get_proxy(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, NULL);

	po = tcore_object_ref_object(o);
	if (!po)
		return NULL;

	if (!po->proxy)
		return NULL;

	return g_strdup(po->proxy);
}

TReturn tcore_context_set_mmsurl(CoreObject *o, const char *mmsurl)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return FALSE;

	if (po->mmsurl) {
		free(po->mmsurl);
		po->mmsurl = NULL;
	}

	if (mmsurl) {
		po->mmsurl = g_strdup(mmsurl);
	}

	return TCORE_RETURN_SUCCESS;
}

char *tcore_context_get_mmsurl(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, NULL);

	po = tcore_object_ref_object(o);
	if (!po)
		return NULL;

	if (!po->mmsurl)
		return NULL;

	return g_strdup(po->mmsurl);
}

TReturn tcore_context_set_profile_name(CoreObject *o, const char *profile_name)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return FALSE;

	if (po->profile_name) {
		free(po->profile_name);
		po->profile_name = NULL;
	}

	if (profile_name) {
		po->profile_name = g_strdup(profile_name);
	}

	return TCORE_RETURN_SUCCESS;
}

char *tcore_context_get_profile_name(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, NULL);

	po = tcore_object_ref_object(o);
	if (!po)
		return NULL;

	if (!po->profile_name)
		return NULL;

	return g_strdup(po->profile_name);
}

TReturn tcore_context_set_devinfo(CoreObject *o, struct tnoti_ps_pdp_ipconfiguration *devinfo)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return FALSE;
	if (!devinfo)
		return FALSE;

	memcpy(&(po->ip_v4), devinfo->ip_address, sizeof(union tcore_ip4_type));
	memcpy(&(po->dns_primary_v4), devinfo->primary_dns, sizeof(union tcore_ip4_type));
	memcpy(&(po->dns_secondary_v4), devinfo->secondary_dns, sizeof(union tcore_ip4_type));
	memcpy(&(po->gateway_v4), devinfo->gateway, sizeof(union tcore_ip4_type));
	memcpy(po->devname, devinfo->devname, sizeof(char) * 16);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_context_reset_devinfo(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return FALSE;

	memset(&(po->ip_v4), 0, sizeof(union tcore_ip4_type));
	memset(&(po->dns_primary_v4), 0, sizeof(union tcore_ip4_type));
	memset(&(po->dns_secondary_v4), 0, sizeof(union tcore_ip4_type));
	memset(&(po->gateway_v4), 0, sizeof(union tcore_ip4_type));
	memset(po->devname, 0, sizeof(char) * 16);

	return TCORE_RETURN_SUCCESS;
}

void tcore_context_cp_service_info(CoreObject *dest, CoreObject *src)
{
	struct private_object_data *d_po = NULL;
	struct private_object_data *s_po = NULL;

	CORE_OBJECT_CHECK(dest, CORE_OBJECT_TYPE_PS_CONTEXT);
	CORE_OBJECT_CHECK(src, CORE_OBJECT_TYPE_PS_CONTEXT);

	d_po = tcore_object_ref_object(dest);
	s_po = tcore_object_ref_object(src);

	d_po->state = s_po->state;
	d_po->id = s_po->id;
	memcpy(&(d_po->ip_v4), &(s_po->ip_v4), sizeof(union tcore_ip4_type));
	memcpy(&(d_po->dns_primary_v4), &(s_po->dns_primary_v4), sizeof(union tcore_ip4_type));
	memcpy(&(d_po->dns_secondary_v4), &(s_po->dns_secondary_v4), sizeof(union tcore_ip4_type));
	memcpy(&(d_po->gateway_v4), &(s_po->gateway_v4), sizeof(union tcore_ip4_type));
	memcpy(d_po->devname, s_po->devname, sizeof(char) * 16);

	return;
}

static void tcore_context_set_ipv4_atoi(unsigned char *ip4, const char *str)
{
	char *token = NULL;
	char *temp = NULL;
	int index = 0;

	temp = g_strdup(str);
	token = strtok(temp, ".");
	while (token != NULL) {
		ip4[index++] = atoi(token);
		token = strtok(NULL, ".");
	}
	g_free(temp);
}

TReturn tcore_context_set_ipv4_addr(CoreObject *o, const char *addr)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (po->addr) {
		free(po->addr);
		po->addr = NULL;
	}

	if (addr) {
		po->addr = g_strdup(addr);
		tcore_context_set_ipv4_atoi(po->ip_v4.s, addr);
	}

	return TCORE_RETURN_SUCCESS;
}

char* tcore_context_get_ipv4_addr(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, NULL);

	po = tcore_object_ref_object(o);
	if (!po)
		return NULL;

	return tcore_util_get_string_by_ip4type(po->ip_v4);
}

TReturn tcore_context_set_ipv4_dns(CoreObject *o, const char *dns1, const char *dns2)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	g_free(po->dns1);
	po->dns1 = NULL;

	g_free(po->dns2);
	po->dns2 = NULL;

	if (dns1) {
		po->dns1 = g_strdup(dns1);
		tcore_context_set_ipv4_atoi(po->dns_primary_v4.s, dns1);
	}

	if (dns2) {
		po->dns2 = g_strdup(dns2);
		tcore_context_set_ipv4_atoi(po->dns_secondary_v4.s, dns2);
	}

	return TCORE_RETURN_SUCCESS;
}

char* tcore_context_get_ipv4_dns1(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, NULL);

	po = tcore_object_ref_object(o);
	if (!po)
		return NULL;

	return tcore_util_get_string_by_ip4type(po->dns_primary_v4);
}

char* tcore_context_get_ipv4_dns2(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, NULL);

	po = tcore_object_ref_object(o);
	if (!po)
		return NULL;

	return tcore_util_get_string_by_ip4type(po->dns_secondary_v4);
}

char* tcore_context_get_ipv4_gw(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, NULL);

	po = tcore_object_ref_object(o);
	if (!po)
		return NULL;

	return tcore_util_get_string_by_ip4type(po->gateway_v4);
}

TReturn tcore_context_set_ipv4_devname(CoreObject *o, const char *name)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po)
		return TCORE_RETURN_EINVAL;

	if (name) {
		snprintf(po->devname, DEVNAME_LEN_MAX, "%s", name);
	}

	return TCORE_RETURN_SUCCESS;
}

char* tcore_context_get_ipv4_devname(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_PS_CONTEXT, NULL);

	po = tcore_object_ref_object(o);
	if (!po)
		return NULL;

	if (po->devname[0] == 0)
		return NULL;

	return g_strdup(po->devname);
}
