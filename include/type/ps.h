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

#ifndef __TYPE_PS_H__
#define __TYPE_PS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define TCORE_PS_NUM_IP_ADDRESS_LEN_MAX	4
#define TCORE_PS_STR_IP_ADDRESS_LEN_MAX	16

#define TCORE_PS_PDP_ADDRESS_LEN_MAX	20
#define TCORE_PS_APN_LEN_MAX			102

#define TCORE_PS_DEVNAME_LEN_MAX		16
#define TCORE_PS_USERNAME_LEN_MAX		32
#define TCORE_PS_PASSWORD_LEN_MAX		32

typedef enum {
	TCORE_PS_FAILURE_CAUSE_NORMAL,
	TCORE_PS_FAILURE_CAUSE_REL_BY_USER,
	TCORE_PS_FAILURE_CAUSE_REGULAR_DEACTIVATION,
	TCORE_PS_FAILURE_CAUSE_LLC_SNDCP,
	TCORE_PS_FAILURE_CAUSE_INSUFFICIENT_RESOURCE,
	TCORE_PS_FAILURE_CAUSE_UNKNOWN_APN,
	TCORE_PS_FAILURE_CAUSE_UNKNOWN_PDP_ADDRESS,
	TCORE_PS_FAILURE_CAUSE_USER_AUTH_FAILED,
	TCORE_PS_FAILURE_CAUSE_ACT_REJ_GGSN,
	TCORE_PS_FAILURE_CAUSE_ACT_REJ_UNSPECIFIED,
	TCORE_PS_FAILURE_CAUSE_SVC_OPTION_NOT_SUPPORTED,
	TCORE_PS_FAILURE_CAUSE_SVC_NOT_SUBSCRIBED,
	TCORE_PS_FAILURE_CAUSE_SVC_OPT_OUT_ORDER,
	TCORE_PS_FAILURE_CAUSE_NSAPI_USED,
	TCORE_PS_FAILURE_CAUSE_QOS_NOT_ACCEPTED,
	TCORE_PS_FAILURE_CAUSE_NETWORK_FAILURE,
	TCORE_PS_FAILURE_CAUSE_REACT_REQUIRED,
	TCORE_PS_FAILURE_CAUSE_FEATURE_NOT_SUPPORTED,
	TCORE_PS_FAILURE_CAUSE_TFT_FILTER_ERROR,
	TCORE_PS_FAILURE_CAUSE_UNKOWN_PDP_CONTEXT,
	TCORE_PS_FAILURE_CAUSE_INVALID_MSG,
	TCORE_PS_FAILURE_CAUSE_PROTOCOL_ERROR,
	TCORE_PS_FAILURE_CAUSE_MOBILE_FAILURE_ERROR,
	TCORE_PS_FAILURE_CAUSE_TIMEOUT_ERROR,
	TCORE_PS_FAILURE_CAUSE_UNKNOWN_ERROR,
	TCORE_PS_FAILURE_CAUSE_MAX
} TcorePsFailureCause;

typedef enum {
	TCORE_PS_STATE_ON,
	TCORE_PS_STATE_3G_OFF,
	TCORE_PS_STATE_ROAMING_OFF,
	TCORE_PS_STATE_FLIGHT_MODE,
	TCORE_PS_STATE_NO_SERVICE,
} TcorePsState;

typedef enum {
	TCORE_PS_CALL_STATE_CTX_DEFINED,
	TCORE_PS_CALL_STATE_NOT_CONNECTED,
	TCORE_PS_CALL_STATE_CONNECTED
} TcorePsCallState;

#if 0
typedef struct {
	gint context_id;
	gint secondary_context_id;
	gchar apn[TCORE_PS_APN_LEN_MAX];
	gchar pdp_address[TCORE_PS_PDP_ADDRESS_LEN_MAX];
	gint pdp_type;

	gchar username[TCORE_PS_USERNAME_LEN_MAX];
	gchar password[TCORE_PS_PASSWORD_LEN_MAX];
	gchar dns1[TCORE_PS_STR_IP_ADDRESS_LEN_MAX];
	gchar dns2[TCORE_PS_STR_IP_ADDRESS_LEN_MAX];
	gint auth_type;
} TreqPsPdpActivate;

typedef struct {
	gint context_id;
	gint secondary_context_id;
	gint result;
} TrespPsSetPdpActivate;

typedef struct {
	gint context_id;
	gint secondary_context_id;

	gchar username[TCORE_PS_USERNAME_LEN_MAX];
	gchar password[TCORE_PS_PASSWORD_LEN_MAX];
	gchar dns1[TCORE_PS_STR_IP_ADDRESS_LEN_MAX];
	gchar dns2[TCORE_PS_STR_IP_ADDRESS_LEN_MAX];
	gint auth_type;
} TreqPsPdpDeactivate;

typedef struct {
	gint context_id;
	gint secondary_context_id;
	gint result;
} TrespPsSetPdpDeactivate;
#endif

typedef struct {
	guint context_id;
	TcorePsCallState state;
} TcorePsCallStatusInfo;

typedef struct {
	gint context_id;
	gint secondary_context_id;

	TcorePsFailureCause err;
	gushort field_flag;
	guchar ip_address[TCORE_PS_NUM_IP_ADDRESS_LEN_MAX];
	guchar primary_dns[TCORE_PS_NUM_IP_ADDRESS_LEN_MAX];
	guchar secondary_dns[TCORE_PS_NUM_IP_ADDRESS_LEN_MAX];
	guchar gateway[TCORE_PS_NUM_IP_ADDRESS_LEN_MAX];
	guchar subnet_mask[TCORE_PS_NUM_IP_ADDRESS_LEN_MAX];
	gchar devname[TCORE_PS_DEVNAME_LEN_MAX];
} TcorePsPdpIpConf;

#ifdef __cplusplus
}
#endif

#endif	/* __TYPE_PS_H__ */
