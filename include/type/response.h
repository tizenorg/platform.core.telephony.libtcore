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

#ifndef __TYPE_RESPONSE_H__
#define __TYPE_RESPONSE_H__

#include <type/common.h>

__BEGIN_DECLS

enum tcore_response_command {
	TRESP_UNKNOWN = 0,

	TRESP_SERVER = TCORE_RESPONSE | TCORE_TYPE_SERVER,

	TRESP_MODEM = TCORE_RESPONSE | TCORE_TYPE_MODEM,
	TRESP_MODEM_POWER_ON,
	TRESP_MODEM_POWER_OFF,
	TRESP_MODEM_POWER_RESET,
	TRESP_MODEM_POWER_LOW,
	TRESP_MODEM_SET_FLIGHTMODE,
	TRESP_MODEM_GET_FLIGHTMODE,
	TRESP_MODEM_GET_IMEI,
	TRESP_MODEM_GET_VERSION,
	TRESP_MODEM_GET_SN,
	TRESP_MODEM_SET_DUN_PIN_CONTROL,

	TRESP_CALL = TCORE_RESPONSE | TCORE_TYPE_CALL,
	TRESP_CALL_DIAL,
	TRESP_CALL_ANSWER,
	TRESP_CALL_END,
	TRESP_CALL_HOLD,
	TRESP_CALL_ACTIVE,
	TRESP_CALL_SWAP,
	TRESP_CALL_JOIN,
	TRESP_CALL_SPLIT,
	TRESP_CALL_DEFLECT,
	TRESP_CALL_TRANSFER,
	TRESP_CALL_START_CONT_DTMF,
	TRESP_CALL_STOP_CONT_DTMF,
	TRESP_CALL_SEND_BURST_DTMF,
	TRESP_CALL_SET_PRIVACY_MODE,
	TRESP_CALL_GET_PRIVACY_MODE,
	TRESP_CALL_SET_SOUND_PATH,
	TRESP_CALL_GET_SOUND_VOLUME_LEVEL,
	TRESP_CALL_SET_SOUND_VOLUME_LEVEL,
	TRESP_CALL_SET_SOUND_MUTE_STATUS,
	TRESP_CALL_GET_SOUND_MUTE_STATUS,
	TRESP_CALL_SET_SOUND_RECORDING,
	TRESP_CALL_SET_SOUND_EQUALIZATION,
	TRESP_CALL_SET_SOUND_NOISE_REDUCTION,
	TRESP_CALL_SET_SOUND_CLOCK_STATUS,
	TRESP_CALL_SET_ACTIVE_LINE,
	TRESP_CALL_GET_ACTIVE_LINE,
	TRESP_CALL_ACTIVATE_CCBS,
	TRESP_CALL_SET_PREFERRED_VOICE_SUBSCRIPTION,
	TRESP_CALL_GET_PREFERRED_VOICE_SUBSCRIPTION,
	TRESP_CALL_MODIFY,
	TRESP_CALL_CONFIRM_MODIFY,

	TRESP_SS = TCORE_RESPONSE | TCORE_TYPE_SS,
	TRESP_SS_BARRING_ACTIVATE,
	TRESP_SS_BARRING_DEACTIVATE,
	TRESP_SS_BARRING_CHANGE_PASSWORD,
	TRESP_SS_BARRING_GET_STATUS,
	TRESP_SS_FORWARDING_ACTIVATE,
	TRESP_SS_FORWARDING_DEACTIVATE,
	TRESP_SS_FORWARDING_REGISTER,
	TRESP_SS_FORWARDING_DEREGISTER,
	TRESP_SS_FORWARDING_GET_STATUS,
	TRESP_SS_WAITING_ACTIVATE,
	TRESP_SS_WAITING_DEACTIVATE,
	TRESP_SS_WAITING_GET_STATUS,
	TRESP_SS_CLI_ACTIVATE,
	TRESP_SS_CLI_DEACTIVATE,
	TRESP_SS_CLI_SET_STATUS,
	TRESP_SS_CLI_GET_STATUS,
	TRESP_SS_SEND_USSD,
	TRESP_SS_SET_AOC,
	TRESP_SS_GET_AOC,

	TRESP_SIM = TCORE_RESPONSE | TCORE_TYPE_SIM,
	TRESP_SIM_VERIFY_PINS,
	TRESP_SIM_VERIFY_PUKS,
	TRESP_SIM_CHANGE_PINS,
	TRESP_SIM_GET_FACILITY_STATUS,
	TRESP_SIM_DISABLE_FACILITY,
	TRESP_SIM_ENABLE_FACILITY,
	TRESP_SIM_TRANSMIT_APDU,
	TRESP_SIM_GET_ATR,
	TRESP_SIM_GET_ECC,
	TRESP_SIM_GET_LANGUAGE,
	TRESP_SIM_SET_LANGUAGE,
	TRESP_SIM_GET_ICCID,
	TRESP_SIM_GET_MAILBOX,
	TRESP_SIM_SET_MAILBOX,
	TRESP_SIM_GET_CALLFORWARDING,
	TRESP_SIM_SET_CALLFORWARDING,
	TRESP_SIM_GET_MESSAGEWAITING,
	TRESP_SIM_SET_MESSAGEWAITING,
	TRESP_SIM_GET_CPHS_INFO,
	TRESP_SIM_GET_SERVICE_TABLE,
	TRESP_SIM_GET_MSISDN,
	TRESP_SIM_GET_SPN,
	TRESP_SIM_GET_SPDI,
	TRESP_SIM_GET_OPL,
	TRESP_SIM_GET_PNN,
	TRESP_SIM_GET_CPHS_NETNAME,
	TRESP_SIM_GET_OPLMNWACT,
	TRESP_SIM_REQ_AUTHENTICATION,
	TRESP_SIM_GET_LOCK_INFO,
	TRESP_SIM_SET_POWERSTATE,
	TRESP_SIM_GET_GID,
#if defined TIZEN_GLOBALCONFIG_ENABLE_CSP
	TRESP_SIM_GET_CPHS_CSP_INFO,
	TRESP_SIM_SET_CPHS_CSP_INFO,
#endif
	TRESP_SIM_GET_IMPI,
	TRESP_SIM_GET_IMPU,
	TRESP_SIM_GET_DOMAIN,
	TRESP_SIM_GET_PCSCF,
	TRESP_SIM_GET_ISIM_SERVICE_TABLE,

	TRESP_SAT = TCORE_RESPONSE | TCORE_TYPE_SAT,
	TRESP_SAT_REQ_ENVELOPE,
	TRESP_SAT_REQ_TERMINALRESPONSE,

	TRESP_SAP = TCORE_RESPONSE | TCORE_TYPE_SAP,
	TRESP_SAP_REQ_CONNECT,
	TRESP_SAP_REQ_DISCONNECT,
	TRESP_SAP_REQ_STATUS,
	TRESP_SAP_REQ_ATR,
	TRESP_SAP_TRANSFER_APDU,
	TRESP_SAP_SET_PROTOCOL,
	TRESP_SAP_SET_POWER,
	TRESP_SAP_REQ_CARDREADERSTATUS,

	TRESP_NETWORK = TCORE_RESPONSE | TCORE_TYPE_NETWORK,
	TRESP_NETWORK_SEARCH,
	TRESP_NETWORK_SET_PLMN_SELECTION_MODE,
	TRESP_NETWORK_GET_PLMN_SELECTION_MODE,
	TRESP_NETWORK_SET_SERVICE_DOMAIN,
	TRESP_NETWORK_GET_SERVICE_DOMAIN,
	TRESP_NETWORK_SET_BAND,
	TRESP_NETWORK_GET_BAND,
	TRESP_NETWORK_SET_PREFERRED_PLMN,
	TRESP_NETWORK_GET_PREFERRED_PLMN,
	TRESP_NETWORK_SET_ORDER,
	TRESP_NETWORK_GET_ORDER,
	TRESP_NETWORK_SET_POWER_ON_ATTACH,
	TRESP_NETWORK_GET_POWER_ON_ATTACH,
	TRESP_NETWORK_SET_CANCEL_MANUAL_SEARCH,
	TRESP_NETWORK_GET_SERVING_NETWORK,
	TRESP_NETWORK_SET_MODE,
	TRESP_NETWORK_GET_MODE,
	TRESP_NETWORK_SET_NEIGHBORING_CELL_INFO,
	TRESP_NETWORK_GET_NEIGHBORING_CELL_INFO,
	TRESP_NETWORK_SET_DEFAULT_DATA_SUBSCRIPTION,
	TRESP_NETWORK_GET_DEFAULT_DATA_SUBSCRIPTION,
	TRESP_NETWORK_SET_DEFAULT_SUBSCRIPTION,
	TRESP_NETWORK_GET_DEFAULT_SUBSCRIPTION,
	TRESP_NETWORK_SET_EMERGENCY_CALLBACK_MODE,
	TRESP_NETWORK_SET_ROAMING_PREFERENCE,
	TRESP_NETWORK_GET_ROAMING_PREFERENCE,
	TRESP_NETWORK_GET_SUBSCRIPTION_INFO,

	TRESP_PS = TCORE_RESPONSE | TCORE_TYPE_PS,
	TRESP_PS_SET_PDP_ACTIVATE,
	TRESP_PS_SET_PDP_DEACTIVATE,

	TRESP_SMS = TCORE_RESPONSE | TCORE_TYPE_SMS,
	TRESP_SMS_SEND_UMTS_MSG, /* This event is returned in response to SMS  message sent */
	TRESP_SMS_READ_MSG, /* This event is returned in response to SMS Read message*/
	TRESP_SMS_SAVE_MSG, /* This confirmation event is a returned indicating the Save SMS into SIM status*/
	TRESP_SMS_DELETE_MSG, /* This confirmation event is a returned indicating the delete message status*/
	TRESP_SMS_GET_STORED_MSG_COUNT, /* This event is returned in response to Get Number of messages stored in SIM*/
	TRESP_SMS_GET_SCA, /* This event is returned with Get SCA */
	TRESP_SMS_SET_SCA,
	TRESP_SMS_GET_CB_CONFIG, /* This event is returned with Get CB configuration details*/
	TRESP_SMS_SET_CB_CONFIG, /* This event is returned as a confirmation for SET CB configuration */
	TRESP_SMS_SET_MEM_STATUS,
	TRESP_SMS_GET_PREF_BEARER, /* This event is returned as a response to get preferred bearer used for SMS Transport*/
	TRESP_SMS_SET_PREF_BEARER,
	TRESP_SMS_SET_DELIVERY_REPORT, /* This event is returned in response to send delivery report */
	TRESP_SMS_SET_MSG_STATUS,
	TRESP_SMS_GET_PARAMS, /* This event is returned in response to SMS Get parameters*/
	TRESP_SMS_SET_PARAMS,
	TRESP_SMS_GET_PARAMCNT, /* Parameter count response */
	TRESP_SMS_SEND_CDMA_MSG, /* This event is returned in response to SMS  message extended sent*/

	TRESP_PHONEBOOK = TCORE_RESPONSE | TCORE_TYPE_PHONEBOOK,
	TRESP_PHONEBOOK_GETCOUNT,
	TRESP_PHONEBOOK_GETMETAINFO,
	TRESP_PHONEBOOK_GETUSIMINFO,
	TRESP_PHONEBOOK_READRECORD,
	TRESP_PHONEBOOK_UPDATERECORD,
	TRESP_PHONEBOOK_DELETERECORD,

	TRESP_SOUND = TCORE_RESPONSE | TCORE_TYPE_SOUND,

	TRESP_GPS = TCORE_RESPONSE | TCORE_TYPE_GPS,
	TRESP_GPS_SET_FREQUENCY_AIDING,
	TRESP_ENABLE_SMART_ASSISTANT,
	TRESP_DISABLE_SMART_ASSISTANT,
	TRESP_SYNC_SMART_ASSISTANT_AREA_LIST,
	TRESP_DEL_SMART_ASSISTANT_AREA_LIST,
	TRESP_ADD_SMART_ASSISTANT_AREA,
	TRESP_MODIFY_SMART_ASSISTANT_AREA,
	TRESP_SET_SMART_ASSISTANT_INFO,

	TRESP_CUSTOM = TCORE_RESPONSE | TCORE_TYPE_CUSTOM,
};

__END_DECLS

#endif
