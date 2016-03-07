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

#ifndef __TYPE_NOTIFICATION_H__
#define __TYPE_NOTIFICATION_H__

#include <type/common.h>

__BEGIN_DECLS

enum tcore_notification_command {
	TNOTI_UNKNOWN = 0,

	TNOTI_SERVER = TCORE_NOTIFICATION | TCORE_TYPE_SERVER,
	TNOTI_SERVER_RUN,
	TNOTI_SERVER_ADDED_PLUGIN,
	TNOTI_SERVER_REMOVED_PLUGIN,
	TNOTI_SERVER_ADDED_COMMUNICATOR,
	TNOTI_SERVER_REMOVED_COMMUNICATOR,
	TNOTI_SERVER_ADDED_HAL,
	TNOTI_SERVER_REMOVED_HAL,
	TNOTI_SERVER_ADDED_STORAGE,
	TNOTI_SERVER_REMOVED_STORAGE,
	TNOTI_SERVER_ADDED_MODEM_PLUGIN,
	TNOTI_SERVER_REMOVED_MODEM_PLUGIN,
	TNOTI_SERVER_ADDED_MODEM_PLUGIN_COMPLETED,
	TNOTI_SERVER_EXIT,

	TNOTI_CALL = TCORE_NOTIFICATION | TCORE_TYPE_CALL,
	TNOTI_CALL_STATUS_IDLE,
	TNOTI_CALL_STATUS_ACTIVE,
	TNOTI_CALL_STATUS_HELD,
	TNOTI_CALL_STATUS_DIALING,
	TNOTI_CALL_STATUS_ALERT,
	TNOTI_CALL_STATUS_INCOMING,
	TNOTI_CALL_STATUS_WAITING,
	TNOTI_CALL_INFO_CALL_CONNECTED_LINE,
	TNOTI_CALL_INFO_WAITING,
	TNOTI_CALL_INFO_CUG,
	TNOTI_CALL_INFO_FORWARDED,
	TNOTI_CALL_INFO_BARRED_INCOMING,
	TNOTI_CALL_INFO_BARRED_OUTGOING,
	TNOTI_CALL_INFO_DEFLECTED,
	TNOTI_CALL_INFO_CLIR_SUPPRESSION_REJECT,
	TNOTI_CALL_INFO_FORWARD_UNCONDITIONAL,
	TNOTI_CALL_INFO_FORWARD_CONDITIONAL,
	TNOTI_CALL_INFO_CALL_LINE_IDENTITY,
	TNOTI_CALL_INFO_CALL_NAME_INFORMATION,
	TNOTI_CALL_INFO_FORWARDED_CALL,
	TNOTI_CALL_INFO_CUG_CALL,
	TNOTI_CALL_INFO_DEFLECTED_CALL,
	TNOTI_CALL_INFO_TRANSFERED_CALL,
	TNOTI_CALL_INFO_HELD,
	TNOTI_CALL_INFO_ACTIVE,
	TNOTI_CALL_INFO_JOINED,
	TNOTI_CALL_INFO_RELEASED_ON_HOLD,
	TNOTI_CALL_INFO_TRANSFER_ALERT,
	TNOTI_CALL_INFO_TRANSFERED,
	TNOTI_CALL_INFO_CF_CHECK_MESSAGE,
	TNOTI_CALL_INFO_EXTRA_INFORMATION,
	TNOTI_CALL_INFO_PRIVACY_MODE,
	TNOTI_CALL_INFO_REC,
	TNOTI_CALL_INFO_FALLBACK,			/*notification of call fallback to other bearer - CSFB(IMS to CS) or Reverse-CSFB(CS to IMS)*/
	TNOTI_CALL_OTASP_STATUS,
	TNOTI_CALL_OTAPA_STATUS,
	TNOTI_CALL_SIGNAL_INFO,
	TNOTI_CALL_SOUND_PATH,
	TNOTI_CALL_SOUND_RINGBACK_TONE,
	TNOTI_CALL_SOUND_WBAMR,
	TNOTI_CALL_SOUND_EQUALIZATION,
	TNOTI_CALL_SOUND_NOISE_REDUCTION,
	TNOTI_CALL_SOUND_CLOCK_STATUS,
	TNOTI_CALL_PREFERRED_VOICE_SUBSCRIPTION,
	TNOTI_CALL_DISPLAY_INFORMATION,
	TNOTI_CALL_OTASP_RESULT,
	TNOTI_CALL_SRVCC_HANDOVER,
	TNOTI_CALL_MODIFY_REQUEST,

	TNOTI_SS = TCORE_NOTIFICATION | TCORE_TYPE_SS,
	TNOTI_SS_INFO,
	TNOTI_SS_USSD,
	TNOTI_SS_FORWARDING_STATUS,
	TNOTI_SS_BARRING_STATUS,
	TNOTI_SS_WAITING_STATUS,
	TNOTI_SS_RELEASE_COMPLETE,

	TNOTI_MODEM = TCORE_NOTIFICATION | TCORE_TYPE_MODEM,
	TNOTI_MODEM_POWER,
	TNOTI_MODEM_FLIGHT_MODE,
	TNOTI_MODEM_DUN_PIN_CONTROL,
	TNOTI_MODEM_DUN_EXTERNAL_CALL,
	TNOTI_MODEM_ADDED,
	TNOTI_MODEM_BOOTUP,
	TNOTI_MODEM_DONGLE_STATUS,
	TNOTI_MODEM_DONGLE_LOGIN,

	TNOTI_SIM = TCORE_NOTIFICATION | TCORE_TYPE_SIM,
	TNOTI_SIM_STATUS,
	TNOTI_SIM_REFRESHED,
	TNOTI_SIM_CALL_FORWARD_STATE,

	TNOTI_SAT = TCORE_NOTIFICATION | TCORE_TYPE_SAT,
	TNOTI_SAT_PROACTIVE_CMD,
	TNOTI_SAT_SESSION_END,
	TNOTI_SAT_CALL_CTRL_RESULT,
	TNOTI_SAT_MO_SM_CTRL_RESULT,

	TNOTI_SAP = TCORE_NOTIFICATION | TCORE_TYPE_SAP,
	TNOTI_SAP_STATUS,
	TNOTI_SAP_DISCONNECT,

	TNOTI_NETWORK = TCORE_NOTIFICATION | TCORE_TYPE_NETWORK,
	TNOTI_NETWORK_REGISTRATION_STATUS,
	TNOTI_NETWORK_LOCATION_CELLINFO,
	TNOTI_NETWORK_ICON_INFO,
	TNOTI_NETWORK_CHANGE,
	TNOTI_NETWORK_TIMEINFO,
	TNOTI_NETWORK_IDENTITY,
	TNOTI_NETWORK_NEIGHBORING_CELL_INFO,
	TNOTI_NETWORK_RESTRICTED_STATE,
	TNOTI_NETWORK_SIGNAL_STRENGTH,
	TNOTI_NETWORK_DEFAULT_DATA_SUBSCRIPTION,
	TNOTI_NETWORK_DEFAULT_SUBSCRIPTION,
	TNOTI_NETWORK_EMERGENCY_CALLBACK_MODE,
	TNOTI_NETWORK_IMS_VOICE_SUPPORT_STATUS,
	TNOTI_NETWORK_ECC_RAT,
	TNOTI_NETWORK_IMS_REGISTRATION_STATUS,
	TNOTI_NETWORK_LOCATION_CELLINFO_CDMA,

	TNOTI_PS = TCORE_NOTIFICATION | TCORE_TYPE_PS,
	TNOTI_PS_CALL_STATUS,
	TNOTI_PS_CURRENT_SESSION_DATA_COUNTER,
	TNOTI_PS_PDP_IPCONFIGURATION,
	TNOTI_PS_DEDICATED_BEARER_INFO,
	TNOTI_PS_EXTERNAL_CALL,
	TNOTI_PS_PROTOCOL_STATUS,
	TNOTI_PS_ALL_PDP_DISCONNECTED,

	TNOTI_SMS = TCORE_NOTIFICATION | TCORE_TYPE_SMS,
	TNOTI_SMS_INCOM_MSG,			/* This is an unsolicited Incoming SMS Event*/
	TNOTI_SMS_CB_INCOM_MSG,		/* This is an unsolicited Incoming cell broadcast message*/
	TNOTI_SMS_INCOM_EX_MSG,		/* This is an extended unsolicited Incoming SMS Event*/
	TNOTI_SMS_CB_INCOM_EX_MSG,	/* This is an extended unsolicited Incoming cell broadcast message*/
	TNOTI_SMS_MEMORY_STATUS,		/* This event is an unsolicited event indicating SIM memory status */
	TNOTI_SMS_DEVICE_READY,		/* Device Ready indication */
	TNOTI_SMS_IND_MAX,
	TNOTI_SMS_ETWS_INCOM_MSG,		/* This is an unsolicited Incoming ETWS message*/

	TNOTI_PHONEBOOK = TCORE_NOTIFICATION | TCORE_TYPE_PHONEBOOK,
	TNOTI_PHONEBOOK_STATUS,
	TNOTI_PHONEBOOK_CONTACT_CHANGE,

	TNOTI_GPS = TCORE_NOTIFICATION | TCORE_TYPE_GPS,
	TNOTI_GPS_ASSIST_DATA,
	TNOTI_GPS_MEASURE_POSITION,
	TNOTI_GPS_RESET_ASSIST_DATA,
	TNOTI_GPS_FREQUENCY_AIDING_DATA,
	TNOTI_SMART_ASSISTANT_AREA_STATUS,
	TNOTI_SMART_ASSISTANT_SYNC_STATUS,

	TNOTI_CUSTOM = TCORE_NOTIFICATION | TCORE_TYPE_CUSTOM,
	TNOTI_CUSTOM_MIP_ERROR,
};

struct tnoti_server_run {
};

struct tnoti_server_added_plugin {
};

struct tnoti_server_removed_plugin {
};

struct tnoti_server_added_communicator {
};

struct tnoti_server_removed_communicator {
};

struct tnoti_server_added_hal {
};

struct tnoti_server_removed_hal {
};

struct tnoti_server_added_storage {
};

struct tnoti_server_removed_storage {
};

struct tnoti_server_exit {
};

__END_DECLS

#endif
