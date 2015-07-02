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

#ifndef __TYPE_PS_H__
#define __TYPE_PS_H__

__BEGIN_DECLS

#define IPV6_ADDR_LEN 16
#define IPV4_ADDR_LEN 4

#define MAX_NUM_DEDICATED_BEARER 8

enum telephony_ps_pdp_err {
	PDP_FAILURE_CAUSE_NORMAL						= 0x00,			  // 0x00 : Normal Process ( no problem )
	PDP_FAILURE_CAUSE_REL_BY_USER					= 0x01,			  // Call Released by User
	PDP_FAILURE_CAUSE_REGULAR_DEACTIVATION			= 0x02,			  // Regular deactivation
	PDP_FAILURE_CAUSE_LLC_SNDCP					= 0x03,			  // LLC SNDCP failure
	PDP_FAILURE_CAUSE_INSUFFICIENT_RESOURCE		= 0x04,			  // Insufficient resources
	PDP_FAILURE_CAUSE_UNKNOWN_APN					= 0x05,			  // Missing or unkown apn
	PDP_FAILURE_CAUSE_UNKNOWN_PDP_ADDRESS			= 0x06,			  // Unknown pdp address or type
	PDP_FAILURE_CAUSE_USER_AUTH_FAILED				= 0x07,			  // Unknown pdp address or type
	PDP_FAILURE_CAUSE_ACT_REJ_GGSN					= 0x08,			  // Unknown pdp address or type
	PDP_FAILURE_CAUSE_ACT_REJ_UNSPECIFIED			= 0x09,			  // Unknown pdp address or type
	PDP_FAILURE_CAUSE_SVC_OPTION_NOT_SUPPORTED		= 0x0A,			  // Service option not supported
	PDP_FAILURE_CAUSE_SVC_NOT_SUBSCRIBED			= 0x0B,			  // Requested service option not subscribed
	PDP_FAILURE_CAUSE_SVC_OPT_OUT_ORDER			= 0x0C,			  // Service out of order
    PDP_FAILURE_CAUSE_NSAPI_USED					= 0x0D,			  // NSAPI already used
	PDP_FAILURE_CAUSE_QOS_NOT_ACCEPTED				= 0x0E,			  // QoS not accepted
	PDP_FAILURE_CAUSE_NETWORK_FAILURE				= 0x0F,			  // Network Failure
    PDP_FAILURE_CAUSE_REACT_REQUIRED				= 0x10,			  // Reactivation Required
	PDP_FAILURE_CAUSE_FEATURE_NOT_SUPPORTED		= 0x11,			  // Feature not supported
	PDP_FAILURE_CAUSE_TFT_FILTER_ERROR				= 0x12,			  // TFT or filter error
	PDP_FAILURE_CAUSE_UNKOWN_PDP_CONTEXT			= 0x13,			  // Unkown PDP context
	PDP_FAILURE_CAUSE_INVALID_MSG					= 0x14,			  // Invalied MSG
	PDP_FAILURE_CAUSE_PROTOCOL_ERROR				= 0x15,			  // Protocol error
	PDP_FAILURE_CAUSE_MOBILE_FAILURE_ERROR			= 0x16,			  // Mobile failure error
	PDP_FAILURE_CAUSE_TIMEOUT_ERROR				= 0x17,			  // Timeout error
	PDP_FAILURE_CAUSE_UNKNOWN_ERROR				= 0x18,			  // Unknown error
	PDP_FAILURE_CAUSE_MAX,
};

enum telephony_ps_protocol_status {
	TELEPHONY_HSDPA_OFF = 0x00,
	TELEPHONY_HSDPA_ON = 0x01,
	TELEPHONY_HSUPA_ON = 0x02,
	TELEPHONY_HSPA_ON = 0x03,
	TELEPHONY_HSPAP_ON = 0x04
};

enum telephony_ps_state {
	TELEPHONY_PS_ON,
	TELEPHONY_PS_3G_OFF,
	TELEPHONY_PS_ROAMING_OFF,
	TELEPHONY_PS_FLIGHT_MODE,
	TELEPHONY_PS_NO_SERVICE,
	TELEPHONY_PS_RESTRICTED_SERVICE
};

struct qos_parameter {
	char profile_type;
	char qci;
	unsigned int gbr_dl;
	unsigned int gbr_ul;
	unsigned int max_br_dl;
	unsigned int max_br_ul;
};

struct dedicated_bearer_info {
	int secondary_context_id;
	unsigned char num_dedicated_bearer;
	struct qos_parameter qos[MAX_NUM_DEDICATED_BEARER];
};

struct treq_ps_pdp_activate {
	int context_id;
	int secondary_context_id;
	char apn[102];
	char pdp_address[20];
	int pdp_type;

	char username[32];
	char password[32];
	char dns1[16];
	char dns2[16];
	int auth_type;
};

struct tresp_ps_set_pdp_activate {
	int context_id;
	int secondary_context_id;
	int result;
};


struct treq_ps_pdp_deactivate {
	int context_id;
	int secondary_context_id;

	char username[32];
	char password[32];
	char dns1[16];
	char dns2[16];
	int auth_type;
};

struct tresp_ps_set_pdp_deactivate {
	int context_id;
	int secondary_context_id;
	int result;
};

struct tnoti_ps_call_status {
	int context_id;
	int state;
	int result; /* specified in 3GPP TS 24.008 10.5.6.6. */
};

struct tnoti_ps_pdp_ipconfiguration {
	int context_id;
	int secondary_context_id;

	enum telephony_ps_pdp_err err;
	unsigned short field_flag;

	char devname[16];
	unsigned long mtu; 

	unsigned char ip_address[IPV4_ADDR_LEN];
	unsigned char primary_dns[IPV4_ADDR_LEN];
	unsigned char secondary_dns[IPV4_ADDR_LEN];
	unsigned char gateway[IPV4_ADDR_LEN];
	unsigned char subnet_mask[IPV4_ADDR_LEN];

	char *ipv6_address;
	char *ipv6_primary_dns;
	char *ipv6_secondary_dns;
	char *ipv6_gateway;

	unsigned int pcscf_ipv4_count;
	char **pcscf_ipv4;

	unsigned int pcscf_ipv6_count;
	char **pcscf_ipv6;
};

struct tnoti_ps_external_call {
};

struct tnoti_ps_protocol_status {
	enum telephony_ps_protocol_status status;
};

struct tnoti_MIP_status {
	int result;
};

struct tnoti_ps_dedicated_bearer_info {
	int primary_context_id;
	struct dedicated_bearer_info dedicated_bearer;
};
__END_DECLS

#endif
