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

enum telephony_ps_pdp_err {
	PDP_FAILURE_CAUSE_NORMAL,
	PDP_FAILURE_CAUSE_REL_BY_USER,
	PDP_FAILURE_CAUSE_REGULAR_DEACTIVATION,
	PDP_FAILURE_CAUSE_LLC_SNDCP,
	PDP_FAILURE_CAUSE_INSUFFICIENT_RESOURCE,
	PDP_FAILURE_CAUSE_UNKNOWN_APN,
	PDP_FAILURE_CAUSE_UNKNOWN_PDP_ADDRESS,
	PDP_FAILURE_CAUSE_USER_AUTH_FAILED,
	PDP_FAILURE_CAUSE_ACT_REJ_GGSN,
	PDP_FAILURE_CAUSE_ACT_REJ_UNSPECIFIED,
	PDP_FAILURE_CAUSE_SVC_OPTION_NOT_SUPPORTED,
	PDP_FAILURE_CAUSE_SVC_NOT_SUBSCRIBED,
	PDP_FAILURE_CAUSE_SVC_OPT_OUT_ORDER,
	PDP_FAILURE_CAUSE_NSAPI_USED,
	PDP_FAILURE_CAUSE_QOS_NOT_ACCEPTED,
	PDP_FAILURE_CAUSE_NETWORK_FAILURE,
	PDP_FAILURE_CAUSE_REACT_REQUIRED,
	PDP_FAILURE_CAUSE_FEATURE_NOT_SUPPORTED,
	PDP_FAILURE_CAUSE_TFT_FILTER_ERROR,
	PDP_FAILURE_CAUSE_UNKOWN_PDP_CONTEXT,
	PDP_FAILURE_CAUSE_INVALID_MSG,
	PDP_FAILURE_CAUSE_PROTOCOL_ERROR,
	PDP_FAILURE_CAUSE_MOBILE_FAILURE_ERROR,
	PDP_FAILURE_CAUSE_TIMEOUT_ERROR,
	PDP_FAILURE_CAUSE_UNKNOWN_ERROR,
	PDP_FAILURE_CAUSE_MAX
};

enum telephony_ps_protocol_status {
	TELEPHONY_HSDPA_OFF,
	TELEPHONY_HSDPA_ON,
	TELEPHONY_HSUPA_ON,
	TELEPHONY_HSPA_ON
};

enum telephony_ps_state {
	TELEPHONY_PS_ON,
	TELEPHONY_PS_3G_OFF,
	TELEPHONY_PS_ROAMING_OFF,
	TELEPHONY_PS_FLIGHT_MODE,
	TELEPHONY_PS_NO_SERVICE,
};

enum ps_data_call_status {
	PS_DATA_CALL_NOT_CONNECTED,
	PS_DATA_CALL_CONNECTED
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
	unsigned int context_id;
	enum ps_data_call_status state;
};

struct tnoti_ps_pdp_ipconfiguration {
	int context_id;
	int secondary_context_id;

	enum telephony_ps_pdp_err err;
	unsigned short field_flag;
	unsigned char ip_address[4];
	unsigned char primary_dns[4];
	unsigned char secondary_dns[4];
	unsigned char gateway[4];
	unsigned char subnet_mask[4];
	char devname[16];
};

struct tnoti_ps_external_call {
};

struct tnoti_ps_protocol_status {
	enum telephony_ps_protocol_status status;
};

__END_DECLS

#endif
