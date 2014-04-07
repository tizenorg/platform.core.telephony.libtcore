/*
 * tel-headers
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

#ifndef __TEL_SAT_H__
#define __TEL_SAT_H__

#include <glib.h>
#include <tel_if.h>
#include <tel_call.h>
#include <tel_ss.h>

/**
*  @addtogroup TAPI_SAT
*  @{
*
*  @file tel_sat.h
*  @brief TAPI sat Interface
*/

#ifdef __cplusplus
extern "C" {
#endif

#define TELEPHONY_SAT_INTERFACE TELEPHONY_SERVICE".Sat"

#define TEL_SAT_DEF_TEXT_STRING_LEN_MAX	500 /**< max length for Text String  */
#define TEL_SAT_DEF_TITLE_LEN_MAX	50 /**< max length for Menu Title */
#define TEL_SAT_DEF_ITEM_STR_LEN_MAX	50 /**< max length for default item string */
#define TEL_SAT_TEXT_STRING_LEN_MAX		500 /**< max length for text string */
#define TEL_SAT_DEF_BIT_MASK_CONTACT	0x01 /**< Bit Mask for Contact */
#define TEL_SAT_DEF_BIT_MASK_MSG	0x02 /**< Bit Mask for Msg */
#define TEL_SAT_DEF_BIT_MASK_OTHER	0x04 /**< Bit Mask for Psh */
#define TEL_SAT_REFRESH_FILE_LIST		20 /**< Refresh File List */
#define TEL_SAT_DEF_SS_LEN_MAX	250 /**< max length for SS */
#define TEL_SAT_DEF_USSD_LEN_MAX	250 /**< max length for USSD */
#define TEL_SAT_DIALING_NUMBER_LEN_MAX	200 /**< max length for dailing number */
#define TEL_SAT_ALPHA_ID_LEN_MAX	255	/**< max length of  alpha identifier */
#define TEL_SAT_SUB_ADDR_LEN_MAX	30 /**< max length for Sub address */
#define TEL_SAT_CCP_DATA_LEN_MAX	30 /**< max length of ccp data */
#define TEL_SAT_SS_STRING_LEN_MAX	160 /**< max length for SS string */
#define TEL_SAT_USSD_STRING_LEN_MAX	255 /**< max length for USSD string */
#define TEL_SAT_ITEM_TEXT_LEN_MAX	45 /**< max length of item text */
#define TEL_SAT_SMS_TPDU_SMS_DATA_LEN_MAX	175 /**< max length for SMS TPDU SMS data */
#define TEL_SAT_FILE_ID_LIST_MAX_COUNT	255 /**< max length for file id list */
#define TEL_SAT_ITEMS_NEXT_ACTION_INDI_LIST_MAX_COUNT	50 /**< max count of items next action indication list */
#define TEL_SAT_EVENT_LIST_MAX	17 /**< max count of event list */
#define TEL_SAT_IMG_DATA_FILE_PATH_LEN_MAX	50 /**< max length of image data file name (Icon, CLUT) */
#define TEL_SAT_ICON_LIST_MAX_COUNT	50 /**< max count of icon list */
#define TEL_SAT_DTMF_STRING_LEN_MAX	30 /**< max length of dtmf string */
#define TEL_SAT_URL_LEN_MAX	129 /**< max length of URL*/
#define TEL_SAT_PROVISIONING_FILE_PATH_LEN_MAX	50 /**< max length of provisioning file path */
#define TEL_SAT_CHANNEL_DATA_STRING_LEN_MAX	255 /**< max length for channel data string */
#define TEL_SAT_OTHER_ADDR_LEN_MAX	30 /**< max length of other address */
#define TEL_SAT_NET_ACC_NAM_LEN_MAX	30 /**< max length of net acc name */
#define TEL_SAT_REMOTE_ENTITY_ADDR_LEN_MAX	50 /**<	max length of remote entity address */
#define TEL_SAT_ITEM_TEXT_ATTRIBUTES_LIST_MAX_COUNT	50 /**< max count of item text attributes list */
#define TEL_SAT_MENU_ITEM_COUNT_MAX	40 /**< max count for Menu items */

/**
 * @enum TelSatResult
 * Response (Result) codes
*/
typedef enum {
	TEL_SAT_RESULT_SUCCESS, /**< Operation completed successfully */
	TEL_SAT_RESULT_FAILURE, /**< Operation Failed */
	TEL_SAT_RESULT_INVALID_PARAMETER, /**< Invalid input parameters */
	TEL_SAT_RESULT_MEMORY_FAILURE, /**< Memory allocation failed */
	TEL_SAT_RESULT_OPERATION_NOT_SUPPORTED, /**< Operation not supported */
	TEL_SAT_RESULT_UNKNOWN_FAILURE, /**< Unknown failure */

	TEL_SAT_RESULT_REQUIRED_VALUE_MISSING, /**< Required value missing */
	TEL_SAT_RESULT_COMMAND_NOT_UNDERSTOOD, /**< Command not understood by ME */
	TEL_SAT_RESULT_BEYOND_ME_CAPABILITY /**< Beyond ME capability */
} TelSatResult;

/**
 * @enum TelSatUiUserConfirmType
 * This enumeration defines UI user confirm types
*/
typedef enum {
	TEL_SAT_USER_CONFIRM_YES, /**<This Enum Informs That user confirms yes */
	TEL_SAT_USER_CONFIRM_NO_OR_CANCEL, /**<This enum informs that user confirms no/cancel */
	TEL_SAT_USER_CONFIRM_HELP_INFO, /**<This enum informs that user wants help information */
	TEL_SAT_USER_CONFIRM_END, /**<This enum informs that user confirms end */
	TEL_SAT_USER_CONFIRM_TIMEOUT, /**<This enum informs that user did not respond */
} TelSatUiUserConfirmType;

/**
 * @enum TelSatUiDisplayStatusType
 * This enumeration defines the UI Display Status.
 */
typedef enum
{
	TEL_SAT_DISPLAY_SUCCESS	= 0x00, /**< This enum informs  UI display success */
	TEL_SAT_DISPLAY_FAIL = 0x01, /**< This enum informs  UI display failure */
}TelSatUiDisplayStatusType;

/**
 * @enum TelSatRefreshAppType
 * This enumeration defines the Refresh Application Type.
 */
 typedef enum
{
	TEL_SAT_REFRESH_CONTACT = 0x00,	 /**< refresh application type - Phonebook */
	TEL_SAT_REFRESH_MSG, /**< refresh application type - SMS */
	TEL_SAT_REFRESH_OTHER, /**< refresh application type - other */
	TEL_SAT_REFRESH_MAX	 /**< Maximum Enumeration Value */
}TelSatRefreshAppType;

/**
 * @enum TelSatCallCtrlType
 * This enumeration defines Call Control Type.
 */
typedef enum
{
	TEL_SAT_CALL_CNF_NONE = 0x00, /**< call control confirm type - None */
	TEL_SAT_CALL_CNF_CALL, /**< call control confirm type - call */
	TEL_SAT_CALL_CNF_SS, /**< call control confirm type - ss */
	TEL_SAT_CALL_CNF_USSD, /**< call control confirm type - ussd */
	TEL_SAT_CALL_CNF_MAX /**< Maximum Enumeration Value */
}TelSatCallCtrlType;

/**
 * @enum TelSatProactiveCmdType
 * This enumeration defines Proactive command types
*/
typedef enum {
	TEL_SAT_PROATV_CMD_NONE = 0x00, /**< command type - None */
	TEL_SAT_PROATV_CMD_REFRESH = 0x01, /**< command type - refresh */
	TEL_SAT_PROATV_CMD_MORE_TIME = 0x02, /**< command type - more time	*/
	TEL_SAT_PROATV_CMD_SETUP_EVENT_LIST = 0x05, /**< command type - setup event list */
	TEL_SAT_PROATV_CMD_SETUP_CALL = 0x10, /**<	command type - setup call */
	TEL_SAT_PROATV_CMD_SEND_SS = 0x11, /**<	command type - send ss */
	TEL_SAT_PROATV_CMD_SEND_USSD = 0x12, /**< command type - send ussd	 */
	TEL_SAT_PROATV_CMD_SEND_SMS = 0x13, /**< command type - send sms */
	TEL_SAT_PROATV_CMD_SEND_DTMF = 0x14, /**< command type - send dtmf	*/
	TEL_SAT_PROATV_CMD_LAUNCH_BROWSER = 0x15, /**< command type - launch browser	 */
	TEL_SAT_PROATV_CMD_PLAY_TONE = 0x20, /**< command type - play tone */
	TEL_SAT_PROATV_CMD_DISPLAY_TEXT = 0x21, /**< command type - display text */
	TEL_SAT_PROATV_CMD_GET_INKEY = 0x22, /**< command type - get inkey */
	TEL_SAT_PROATV_CMD_GET_INPUT = 0x23, /**< command type - get input */
	TEL_SAT_PROATV_CMD_SELECT_ITEM = 0x24, /**< command type - select item */
	TEL_SAT_PROATV_CMD_SETUP_MENU = 0x25, /**< command type - setup menu */
	TEL_SAT_PROATV_CMD_PROVIDE_LOCAL_INFO = 0x26, /**< command type - provide local info */
	TEL_SAT_PROATV_CMD_SETUP_IDLE_MODE_TEXT = 0x28, /**< command type - setup idle mode text */
	TEL_SAT_PROATV_CMD_LANGUAGE_NOTIFICATION = 0x35, /**< command type - language notification */
	TEL_SAT_PROATV_CMD_OPEN_CHANNEL = 0x40, /**< command type - open channel -class e */
	TEL_SAT_PROATV_CMD_CLOSE_CHANNEL = 0x41, /**< command type - close channel - class e */
	TEL_SAT_PROATV_CMD_RECEIVE_DATA = 0x42, /**< command type - receive data -class e */
	TEL_SAT_PROATV_CMD_SEND_DATA = 0x43, /**< command type - send data */
	TEL_SAT_PROATV_CMD_GET_CHANNEL_STATUS = 0x44, /**< command type - get channel status -class e */
	TEL_SAT_PROATV_CMD_TYPE_END_PROACTIVE_SESSION = 0xFE, /**< command type - end proactive session */
	TEL_SAT_PROATV_CMD_RESERVED = 0xFF /**< command type - reserved */
} TelSatProactiveCmdType;

/**
 * @enum TelSatAlphabetFormatType
 * This enumeration defines Alphabet format type
*/
typedef enum {
	TEL_SAT_ALPHABET_FORMAT_SMS_DEFAULT = 0x00, /**< Alphabet type -SMS default format */
	TEL_SAT_ALPHABET_FORMAT_8BIT_DATA = 0x01, /**< Alphabet type -8 bit format data */
	TEL_SAT_ALPHABET_FORMAT_UCS2 = 0x02, /**< Alphabet type -UCS2 format */
	TEL_SAT_ALPHABET_FORMAT_RESERVED = 0x03 /**< Alphabet type -format reserved */
} TelSatAlphabetFormatType;

/**
 * @enum TelSatMsgClassType
 * This enumeration defines message class types
*/
typedef enum {
	TEL_SAT_MSG_CLASS_NONE = 0x00, /**< class none */
	TEL_SAT_MSG_CLASS_0 = 0x01, /**< class 0 */
	TEL_SAT_MSG_CLASS_1, /**< class 1 Default meaning:ME-specific */
	TEL_SAT_MSG_CLASS_2, /**< class 2 SIM specific message */
	TEL_SAT_MSG_CLASS_3, /**< class 3 Default meaning: TE specific */
	TEL_SAT_MSG_CLASS_RESERVED = 0xFF /**< class reserved */
} TelSatMsgClassType;

/**
 * @enum TelSatCallCtrlResultType
 * This enum indicates the result of call control by SIM.
 */
typedef enum {
	TEL_SAT_CALL_CTRL_R_ALLOWED_NO_MOD = 0, /**<	call control result type -  ALLOWED WITH NO MOD */
	TEL_SAT_CALL_CTRL_R_NOT_ALLOWED = 1, /**< call control result type -  NOT ALLOWED */
	TEL_SAT_CALL_CTRL_R_ALLOWED_WITH_MOD = 2, /**< call control result type -  ALLOWED WITH MOD	*/
	TEL_SAT_CALL_CTRL_R_RESERVED = 0xFF /**< call control result type -  RESERVED */
} TelSatCallCtrlResultType;

/**
 * @enum TelSatCallType
 * This enum indicates the SAT call type
 */
typedef enum {
	TEL_SAT_CALL_TYPE_MO_VOICE = 0X00,	/**<	call type -mo voice */
	TEL_SAT_CALL_TYPE_MO_SMS, /**< call type - mo sms */
	TEL_SAT_CALL_TYPE_SS, /**< call type -  ss */
	TEL_SAT_CALL_TYPE_USSD, /**< call type -  ussd */
	TEL_SAT_PDP_CNTXT_ACT, /**< call type -  pdp context action */
	TEL_SAT_CALL_TYPE_MAX /**< call type -  max */
}TelSatCallType;

/**
 * @enum TelSatUseInputAlphabetType
 * This is associated with the command qualifier for get input.
 */
typedef enum {
	TEL_SAT_USER_INPUT_ALPHABET_TYPE_SMS_DEFAULT = 1, /**< command qualifier for  ALPHABET TYPE SMS DEFAULT */
	TEL_SAT_USER_INPUT_ALPHABET_TYPE_UCS2 = 2 /**< command qualifier for  ALPHABET TYPE UCS2 */
} TelSatUseInputAlphabetType;

/**
 * @enum TelSatResultType
 * This enumeration defines sat result types
*/
typedef enum {
	TEL_SAT_RESULT_SUCCESS_RETURN = 0x0, /**< command performed successfully */
	TEL_SAT_RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION = 0x01, /**< command performed with partial comprehension */
	TEL_SAT_RESULT_SUCCESS_WITH_MISSING_INFO = 0x02, /**< command performed, with missing information */

	TEL_SAT_RESULT_REFRESH_PERFORMED_WITH_ADDITIONAL_EFS_READ = 0x03, /**< REFRESH PERFORMED WITH ADDITIONAL EFS READ */
	TEL_SAT_RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED = 0x04, /**<	 command performed but REQUESTED ICON NOT DISPLAYED */
	TEL_SAT_RESULT_SUCCESS_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM = 0x05, /**<	command performed but MODIFIED BY CALL CONTROL BY SIM */
	TEL_SAT_RESULT_SUCCESS_LIMITED_SERVICE = 0x06, /**< command performed with LIMITED SERVICE */
	TEL_SAT_RESULT_SUCCESS_WITH_MODIFICATION = 0x07, /**< command performed with MODIFICATION */
	TEL_SAT_RESULT_REFRESH_PRFRMD_BUT_INDICATED_USIM_NOT_ACTIVE = 0x08, /**< REFRESH PERFORMED BUT INDICATED USIM NOT ACTIVE */
	TEL_SAT_RESULT_SUCCESS_BUT_TONE_NOT_PLAYED = 0x09, /**< command performed successfully, tone not played */

	TEL_SAT_RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER = 0x10, /**< proactive sim application session terminated by user	 */
	TEL_SAT_RESULT_BACKWARD_MOVE_BY_USER = 0x11, /**< backward move in the proactive sim application session request by the user */
	TEL_SAT_RESULT_NO_RESPONSE_FROM_USER = 0x12, /**< no response from user */

	TEL_SAT_RESULT_HELP_INFO_REQUIRED_BY_USER = 0x13, /**< HELP INFO REQUIRED BY USER */
	TEL_SAT_RESULT_USSD_OR_SS_TRANSACTION_TERMINATED_BY_USER = 0x14, /**< USSD OR SS TRANSACTION TERMINATED BY USER */

	TEL_SAT_RESULT_ME_UNABLE_TO_PROCESS_COMMAND = 0x20, /**<	ME currently unable to process command	*/
	TEL_SAT_RESULT_NETWORK_UNABLE_TO_PROCESS_COMMAND = 0x21, /**< Network currently unable to process command */
	TEL_SAT_RESULT_USER_DID_NOT_ACCEPT_CALL_SETUP_REQ = 0x22, /**< User did not accept call setup request */
	TEL_SAT_RESULT_USER_CLEAR_DOWN_CALL_BEFORE_CONN = 0x23, /**< User cleared down call before connection or network released */

	TEL_SAT_RESULT_INTERACTION_WITH_CC_BY_SIM_IN_TMP_PRBLM = 0x25, /**< INTERACTION WITH CALL CONTROL BY SIM IN TEMPORARY PROBLEM */
	TEL_SAT_RESULT_LAUNCH_BROWSER_GENERIC_ERROR_CODE = 0x26, /**< LAUNCH BROWSER GENERIC ERROR CODE */

	TEL_SAT_RESULT_BEYOND_ME_CAPABILITIES = 0x30, /**< command beyond ME's capabilities */
	TEL_SAT_RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME = 0x31, /**< command type not understood by ME */
	TEL_SAT_RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME = 0x32, /**< command data not understood by ME */
	TEL_SAT_RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME = 0x33, /**<	command number not known by ME */
	TEL_SAT_RESULT_SS_RETURN_ERROR = 0x34, /**< SS return error */
	TEL_SAT_RESULT_SMS_RP_ERROR = 0x35, /**< SMS rp-error */
	TEL_SAT_RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING = 0x36, /**< Error, required values are missing */

	TEL_SAT_RESULT_USSD_RETURN_ERROR = 0x37, /**<	USSD_RETURN_ERROR */
	TEL_SAT_RESULT_INTRCTN_WITH_CC_OR_SMS_CTRL_PRMNT_PRBLM = 0x39, /**<	INTERACTION WITH CALL CONTROL OR SMS CONTROL PERMANENT PROBLEM */
	TEL_SAT_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR = 0x3A, /**< BEARER INDEPENDENT PROTOCOL ERROR */
	TEL_SAT_RESULT_FRAMES_ERROR = 0x3C, /**< FRAMES ERROR */
} TelSatResultType;

/**
 * @enum TelSatMeProblemType
 * This enumeration defines ME problem types
*/
typedef enum {
	TEL_SAT_ME_PROBLEM_NO_SPECIFIC_CAUSE = 0x0, /**< ME problem with NO SPECIFIC CAUSE */
	TEL_SAT_ME_PROBLEM_SCREEN_BUSY = 0x01, /**< ME problem with SCREEN BUSY */
	TEL_SAT_ME_PROBLEM_ME_BUSY_ON_CALL = 0x02, /**< ME problem with ME BUSY ON CALL */
	TEL_SAT_ME_PROBLEM_ME_BUSY_ON_SS = 0x03, /**< ME problem with ME_BUSY ON SS */
	TEL_SAT_ME_PROBLEM_NO_SERVICE = 0x04, /**<	ME problem with NO SERVICE */
	TEL_SAT_ME_PROBLEM_ACCESS_CONTROL_CLASS_BAR = 0x05, /**< ME problem with ACCESS CONTROL CLASS BAR */
	TEL_SAT_ME_PROBLEM_RADIO_RES_NOT_GRANTED = 0x06, /**< ME problem with RADIO RES NOT GRANTED */
	TEL_SAT_ME_PROBLEM_NOT_IN_SPEECH_CALL = 0x07, /**< ME problem with NOT IN SPEECH CALL */
	TEL_SAT_ME_PROBLEM_ME_BUSY_ON_USSD = 0x08, /**< ME problem with ME BUSY ON USSD */
	TEL_SAT_ME_PROBLEM_ME_BUSY_ON_SEND_DTMF_CMD = 0x09, /**<	ME problem with ME BUSY ON SEND DTMF CMD */
	TEL_SAT_ME_PROBLEM_NO_USIM_ACTIVE = 0x0A, /**< ME problem with NO USIM ACTIVE */
	TEL_SAT_ME_PROBLEM_INVALID = 0xFF /**< ME problem with INVALID */
} TelSatMeProblemType;

/**
 * @enum TelSatCallControlResult
 * This enumeration defines call control result types
*/
typedef enum {
	TEL_SAT_CALL_CTRL_RESULT_ALLOWED_NO_MOD = 0x00, /**< call control result type -ALLOWED NO MOD */
	TEL_SAT_CALL_CTRL_RESULT_NOT_ALLOWED = 0x01, /**< call control result type -NOT ALLOWED */
	TEL_SAT_CALL_CTRL_RESULT_ALLOWED_WITH_MOD = 0x02, /**< call control result type -ALLOWED WITH MOD */
	TEL_SAT_CALL_CTRL_RESULT_RESERVED /**< call control result type -RESERVED */
} TelSatCallControlResult;

/**
 * @enum TelSatTypeOfNum
 * This enumeration defines sat type of number
*/
typedef enum {
	TEL_SAT_TON_UNKNOWN = 0,			/*< unknown */
	TEL_SAT_TON_INTERNATIONAL = 1,		/*< international number */
	TEL_SAT_TON_NATIONAL = 2,			/*< national number */
	TEL_SAT_TON_NETWORK_SPECIFIC = 3,	/*< network specific number */
	TEL_SAT_TON_DEDICATED_ACCESS = 4,	/*< subscriber number */
	TEL_SAT_TON_ALPHA_NUMERIC = 5,		/*< alphanumeric, GSM 7-bit default alphabet) */
	TEL_SAT_TON_ABBREVIATED_NUMBER = 6,	/*< abbreviated number */
	TEL_SAT_TON_RESERVED_FOR_EXT = 7	/*< reserved for extension */
} TelSatTypeOfNum;

/**
 * @enum TelSatNumberingPlanIdentity
 * This enumeration defines numbering plan identity
*/
typedef enum {
	TEL_SAT_NPI_UNKNOWN = 0,				/*< Unknown */
	TEL_SAT_NPI_ISDN_TEL = 1,				/*< ISDN Telephone numbering plan */
	TEL_SAT_NPI_DATA_NUMBERING_PLAN = 3,	/*< Data numbering plan */
	TEL_SAT_NPI_TELEX = 4,					/*< Telex numbering plan */
	TEL_SAT_NPI_SVC_CNTR_SPECIFIC_PLAN = 5,	/*< Service Center Specific plan */
	TEL_SAT_NPI_SVC_CNTR_SPECIFIC_PLAN2 = 6,/*< Service Center Specific plan */
	TEL_SAT_NPI_NATIONAL = 8,				/*< National numbering plan */
	TEL_SAT_NPI_PRIVATE = 9,				/*< Private numbering plan */
	TEL_SAT_NPI_ERMES_NUMBERING_PLAN = 10,	/*< ERMES numbering plan */
	TEL_SAT_NPI_RESERVED_FOR_EXT = 0xF		/*< Reserved for extension */
} TelSatNumberingPlanIdentity;

/**
 * @enum TelSatDeviceIdentitiesTagType
 * This enumeration defines device identities tag types
*/
typedef enum {
	TEL_SAT_DEVICE_ID_KEYPAD = 0x01, /**< device identities tag type -ID KEYPAD */
	TEL_SAT_DEVICE_ID_DISPLAY = 0x02, /**< device identities tag type -ID DISPLAY */
	TEL_SAT_DEVICE_ID_EARPIECE = 0x03, /**< device identities tag type -ID EARPIECE */
	/* 21-27 for channel */
	TEL_SAT_DEVICE_ID_SIM = 0x81, /**< device identities tag type -ID SIM */
	TEL_SAT_DEVICE_ID_ME = 0x82, /**< device identities tag type -ID ME */
	TEL_SAT_DEVICE_ID_NETWORK = 0x83, /**< device identities tag type -ID NETWORK */
	TEL_SAT_DEVICE_ID_RESERVED = 0XFF /**< device identities tag type -ID RESERVED */
} TelSatDeviceIdentitiesTagType;

/**
 * @enum TelSatTimeUnitType
 * This enumeration defines time unit types
*/
typedef enum {
	TEL_SAT_TIME_UNIT_MINUTES = 0x0, /**< Time unit type -MINUTES */
	TEL_SAT_TIME_UNIT_SECONDS = 0x01, /**< Time unit type -SECONDS */
	TEL_SAT_TIME_UNIT_TENTHS_OF_SECONDS	= 0x02, /**< Time unit type -TENTHS_OF_SECONDS */
	TEL_SAT_TIME_UNIT_RESERVED	 = 0xFF /**< Time unit type -RESERVED */
} TelSatTimeUnitType;

/**
 * @enum TelSatSmsTpduType
 * This enumeration defines SMS TPDU types
*/
typedef enum {
	TEL_SAT_SMS_TPDU_TYPE_DELIVER_TPDU = 0, /**< SMS TPDU type -DELIVER TPDU */
	TEL_SAT_SMS_TPDU_TYPE_DELIVER_RPT = 1, /**< SMS TPDU type -DELIVER RPT */
	TEL_SAT_SMS_TPDU_TYPE_SUBMIT_TPDU = 2, /**< SMS TPDU type -SUBMIT TPDU */
	TEL_SAT_SMS_TPDU_TYPE_SUBMIT_RPT	= 3, /**< SMS TPDU type -SUBMIT RPT */
	TEL_SAT_SMS_TPDU_TYPE_STATUS_RPT	= 4, /**< SMS TPDU type -STATUS RPT */
	TEL_SAT_SMS_TPDU_TYPE_TPDU_CMD = 5 /**< SMS TPDU type -TPDU CMD */
} TelSatSmsTpduType;

/**
 * @enum TelSatEventListType
 * This enumeration defines event list types
*/
typedef enum {
	TEL_SAT_EVENT_MT_CALL = 0, /**< event type -MT CALL */
	TEL_SAT_EVENT_CALL_CONNECTED = 1, /**< event type -CALL CONNECTED */
	TEL_SAT_EVENT_CALL_DISCONNECTED = 2, /**< event type -CALL DISCONNECTED */
	TEL_SAT_EVENT_LOCATION_STATUS = 3, /**< event type -LOCATION STATUS */
	TEL_SAT_EVENT_USER_ACTIVITY = 4, /**< event type - USER_ACTIVITY */
	TEL_SAT_EVENT_IDLE_SCREEN_AVAILABLE = 5, /**< event type - IDLE SCREEN AVAILABLE */
	TEL_SAT_EVENT_CARD_READER_STATUS = 6, /**< event type -CARD READER STATUS */
	TEL_SAT_EVENT_LANGUAGE_SELECTION = 7, /**<	event type - LANGUAGE SELECTION */
	TEL_SAT_EVENT_BROWSER_TERMINATION = 8, /**< event type - BROWSER TERMINATION */
	TEL_SAT_EVENT_DATA_AVAILABLE = 9, /**< event type -DATA AVAILABLE */
	TEL_SAT_EVENT_CHANNEL_STATUS = 0x0A, /**< event type - CHANNEL STATUS	 */
	TEL_SAT_EVENT_ACCESS_TECHNOLOGY_CHANGED = 0x0B, /**< event type -TECHNOLOGY CHANGED */
	TEL_SAT_EVENT_DISPLAY_PARAMETERS_CHANGED = 0x0C, /**< event type -PARAMETERS CHANGED */
	TEL_SAT_EVENT_LOCAL_CONNECTION = 0x0D, /**< event type -LOCAL CONNECTION */
	TEL_SAT_EVENT_NW_SEARCH_MODE_CHANGED = 0X0E, /**< event type -NW SEARCH MODE CHANGED */
	TEL_SAT_EVENT_BROWSING_STATUS = 0X0F, /**< event type -BROWSING STATUS */
	TEL_SAT_EVENT_FRAMES_INFORMATION_CHANGED = 0X10, /**< event type -FRAMES INFORMATION CHANGED */
	TEL_SAT_EVENT_RESERVED_FOR_3GPP = 0X11, /**< event type -RESERVED FOR 3GPP */
	TEL_SAT_EVENT_UNKNOWN = 0xFF /**< event type - UNKNOWN */
} TelSatEventListType;

/**
 * @enum TelSatCallCtrlStringType
 * This enumeration defines call control string type.
 */
typedef enum {
	TEL_SAT_CC_VOICE = 0x00, /**< Call Control String Type - voice */
	TEL_SAT_CC_SS = 0x01, /**< Call Control String Type - ss */
	TEL_SAT_CC_USSD = 0x02, /**< Call Control String Type - ussd */
	TEL_SAT_CC_NONE = 0xFF /**< Call Control String Type - none */
} TelSatCallCtrlStringType;

/**
 * @enum TelSatInkeyType
 * This enumeration defines inkey type
*/
typedef enum {
	TEL_SAT_INKEY_TYPE_CHARACTER_SET_ENABLED = 0, /**< Display inkey type -character set enabled */
	TEL_SAT_INKEY_TYPE_YES_NO_REQUESTED = 1 /**< Display inkey type -yes/No requested */
} TelSatInkeyType;

/**
 * @enum TelSatToneType
 * This enumeration defines tone types
*/
typedef enum {
	/* standard supervisory tones */
	TEL_SAT_DIAL_TONE = 0x01, /**<	TONE TYPE DIAL TONE */
	TEL_SAT_CALLED_SUBSCRIBER_BUSY = 0x02, /**< TONE TYPE CALLED SUBSCRIBER BUSY */
	TEL_SAT_CONGESTION = 0x03, /**< TONE TYPE CONGESTION */
	TEL_SAT_RADIO_PATH_ACK = 0x04, /**< TONE TYPE RADIO PATH ACK */
	TEL_SAT_RADIO_PATH_NOT_AVAILABLE_CALL_DROPPED = 0x05, /**< TONE TYPE RADIO PATH NOT AVAILABLE CALL DROPPED */
	TEL_SAT_ERR_SPECIAL_INFO = 0x06, /**<	TONE TYPE ERR SPECIAL INFO */
	TEL_SAT_CALL_WAITING_TONE = 0x07, /**< TONE TYPE CALL WAITING TONE */
	TEL_SAT_RINGING_TONE = 0x08, /**< TONE TYPE RINGING TONE */

	/* ME proprietary tones */
	TEL_SAT_GENERAL_BEEP = 0x10, /**<	TONE TYPE GENERAL BEEP */
	TEL_SAT_POSITIVE_ACK_TONE = 0x11, /**< TONE TYPE POSITIVE ACK TONE */
	TEL_SAT_NEGATIVE_ACK_OR_ERROR_TONE = 0x12, /**< TONE TYPE NEGATIVE ACK OR ERROR TONE */
	TEL_SAT_RINGING_TONE_SLCTD_BY_USR_FOR_INCOM_SPEECH_CALL = 0x13, /**<	TONE TYPE RINGING TONE SELECTED BY USER FOR INCOMING SPEECH CALL */
	TEL_SAT_ALERT_TONE_SELECTED_BY_USER_FOR_INCOMING_SMS = 0x14, /**< TONE TYPE ALERT TONE SELECTED BY USER FOR INCOMING SMS */
	TEL_SAT_CRITICAL_ALERT = 0x15, /**< TONE TYPE CRITICAL ALERT */

	/* Themed tones */
	TEL_SAT_HAPPY_TONE = 0x30, /**< TONE TYPE HAPPY TONE */
	TEL_SAT_SAD_TONE = 0x31, /**< TONE TYPE SAD TONE */
	TEL_SAT_URGENT_ACTION_TONE = 0x32, /**< TONE TYPE URGENT ACTION TONE */
	TEL_SAT_QUESTION_TONE = 0x33, /**< TONE TYPE QUESTION TONE */
	TEL_SAT_MESSAGE_RECEIVED_TONE = 0x34, /**< TONE TYPE MESSAGE RECEIVED TONE */

	/* Melody tones */
	TEL_SAT_MELODY_1 = 0x40, /**<	TONE TYPE MELODY 1 */
	TEL_SAT_MELODY_2 = 0x41, /**<	TONE TYPE MELODY 2 */
	TEL_SAT_MELODY_3 = 0x42, /**<	TONE TYPE MELODY 3 */
	TEL_SAT_MELODY_4 = 0x43, /**<	TONE TYPE MELODY 4 */
	TEL_SAT_MELODY_5 = 0x44, /**<	TONE TYPE MELODY 5 */
	TEL_SAT_MELODY_6 = 0x45, /**<	TONE TYPE MELODY 6 */
	TEL_SAT_MELODY_7 = 0x46, /**<	TONE TYPE MELODY 7 */
	TEL_SAT_MELODY_8 = 0x47, /**<	TONE TYPE MELODY 8 */

	TEL_SAT_TONE_TYPE_RESERVED = 0xFF /**< TONE TYPE RESERVED */
} TelSatToneType;

/**
 * @enum TelSatBcRepeatIndicatorType
 * This enumeration defines BC repeat indicator types
*/
typedef enum {
	TEL_SAT_BC_REPEAT_INDI_ALTERNATE_MODE = 0x01, /**< BC REPEAT ALTERNATE MODE */
	TEL_SAT_BC_REPEAT_INDI_SEQUENTIAL_MODE = 0x03, /**< BC REPEAT SEQUENTIAL MODE */
	TEL_SAT_BC_REPEAT_INDI_RESERVED = 0xFF /**< RESERVED */
} TelSatBcRepeatIndicatorType;

/**
 * @enum TelSatImageCodingSchemeType
 * This enumeration defines image coding scheme types
*/
typedef enum {
	TEL_SAT_SIM_IMAGE_CODING_SCHEME_BASIC = 0x11, /**< Image coding scheme type - BASIC */
	TEL_SAT_SIM_IMAGE_CODING_SCHEME_COLOUR = 0x21, /**< Image coding scheme type - COLOUR */
	TEL_SAT_SIM_IMAGE_CODING_SCHEME_RESERVED = 0xFF /**< Image coding scheme type - RESERVED */
} TelSatImageCodingSchemeType;

/**
 * @enum TelSatIconQualifierType
 * This enumeration defines icon qualifier types
*/
typedef enum {
	TEL_SAT_ICON_QUALI_SELF_EXPLANATORY = 0, /**< Icon qualifier type - SELF EXPLANATORY */
	TEL_SAT_ICON_QUALI_NOT_SELF_EXPLANATORY = 1, /**< Icon qualifier type - NOT SELF EXPLANATORY */
	TEL_SAT_ICON_QUALI_RESERVED = 0xFF /**< Icon qualifier type - RESERVED */
} TelSatIconQualifierType;

/**
 * @enum TelSatBrowserIdentityType
 * This enumeration defines browser identity types
*/
typedef enum {
	TEL_SAT_BROWSER_ID_DEFAULT = 0, /**< DEFAULT BROWSER */
	TEL_SAT_BROWSER_ID_WML, /**< BROWSER WML */
	TEL_SAT_BROWSER_ID_HTML, /**< BROWSER HTML */
	TEL_SAT_BROWSER_ID_XHTML, /**< BROWSER XHTML */
	TEL_SAT_BROWSER_ID_CHTML, /**<	BROWSER CHTML */
	TEL_SAT_BROWSER_ID_RESERVED = 0xFF /**< RESERVED */
} TelSatBrowserIdentityType;

/**
 * @enum TelSatBearerListType
 * This enumeration defines bearer list types
*/
typedef enum {
	TEL_SAT_BEARER_LIST_SMS = 0, /**<	BEARER SMS */
	TEL_SAT_BEARER_LIST_CSD = 1, /**<	BEARER CSD */
	TEL_SAT_BEARER_LIST_USSD = 2, /**< BEARER USSD	*/
	TEL_SAT_BEARER_LIST_GPRS = 3, /**< BEARER GPRS */
	TEL_SAT_BEARER_LIST_RESERVED = 0xFF /**< BEARER RESERVED */
} TelSatBearerListType;

/**
 * @enum TelSatChannelStatusType
 * This enumeration defines channel status types
*/
typedef enum {
	TEL_SAT_LINK_OR_PACKET_SERVICE_NOT_ACTIVATED, /**< channel status type -LINK OR PACKET SERVICE NOT ACTIVATED */
	TEL_SAT_LINK_OR_PACKET_SERVICE_ACTIVATED /**< channel status type -LINK OR PACKET SERVICE ACTIVATED */
} TelSatChannelStatusType;

/**
 * @enum TelSatChannelStatusInfoType
 * This enumeration defines channel status info types
*/
typedef enum {
	TEL_SAT_CHANNEL_STATUS_NO_FURTHER_INFO = 0x00, /**< channel status info type - NO FURTHER INFO */
	TEL_SAT_CHANNEL_STATUS_LINK_DROPPED = 0x05 /**< channel status info type -LINK DROPPED */
} TelSatChannelStatusInfoType;

/**
 * @enum TelSatAddressType
 * This enumeration defines address types
*/
typedef enum {
	TEL_SAT_ADDR_TYPE_IPv4 = 0x21, /**< address type - IPv4 */
	TEL_SAT_ADDR_TYPE_IPv6 = 0x57, /**< address type - IPv6 */
	TEL_SAT_ADDR_RESERVED = 0xFF /**< reserved */
} TelSatAddressType;

/**
 * @enum TelSatTransportProtocolType
 * This enumeration defines transport protocal types
*/
typedef enum {
	TEL_SAT_TP_TYPE_UDP_UICC_CLIENT = 0x01, /**< transport protocol type- UDP UICC CLIENT */
	TEL_SAT_TP_TYPE_TCP_UICC_CLIENT = 0x02, /**< transport protocol type-TCP UICC CLIENT */
	TEL_SAT_TP_TYPE_TCP_UICC_SERVER = 0x03 /**< transport protocol type- TCP UICC SERVER	*/
} TelSatTransportProtocolType;

/**
 * @enum TelSatRemoteEntityAddrCodingType
 * This enumeration defines remote entity address coding types
*/
typedef enum {
	TEL_SAT_REMOTE_ENTITY_ADDR_CODING_TYPE_IEEE802_48BIT = 0, /**<	remote entity address coding type- IEEE802 48BIT */
	TEL_SAT_REMOTE_ENTITY_ADDR_CODING_TYPE_IRDA_32BIT = 1, /**< remote entity address coding type- IRDA 32BIT */
	TEL_SAT_REMOTE_ENTITY_ADDR_CODING_TYPE_RESERVED = 0xFF /**< reserved */
} TelSatRemoteEntityAddrCodingType;

/**
 * @enum TelSatLanguageInfo
 * This enumeration defines lists the language values.
 */
typedef enum {
	TEL_SAT_LP_GERMAN = 0x00, /**< GERMAN */
	TEL_SAT_LP_ENGLISH = 0x01, /**< ENGLISH */
	TEL_SAT_LP_ITALIAN = 0x02, /**<	ITALIAN */
	TEL_SAT_LP_FRENCH = 0x03, /**< FRENCH */
	TEL_SAT_LP_SPANISH = 0x04, /**< SPANISH */
	TEL_SAT_LP_DUTCH = 0x05, /**<	DUTCH */
	TEL_SAT_LP_SWEDISH = 0x06, /**< SWEDISH */
	TEL_SAT_LP_DANISH = 0x07, /**<	DANISH	*/
	TEL_SAT_LP_PORTUGUESE = 0x08, /**< PORTUGUESE	*/
	TEL_SAT_LP_FINNISH = 0x09, /**< FINNISH */
	TEL_SAT_LP_NORWEGIAN = 0x0A, /**< NORWEGIAN */
	TEL_SAT_LP_GREEK = 0x0B, /**< GREEK */
	TEL_SAT_LP_TURKISH = 0x0C, /**< TURKISH */
	TEL_SAT_LP_HUNGARIAN = 0x0D, /**< HUNGARIAN */
	TEL_SAT_LP_POLISH = 0x0E, /**<	POLISH	*/
	TEL_SAT_LP_LANG_UNSPECIFIED = 0x0F /**<	LANGUAGE UNSPECIFIED */
} TelSatLanguageInfo;

/**
 * @enum TelSatCmdQualifierLaunchBrowser
 * This enumeration defines cmd qualifier for launch browser
*/
typedef enum {
	TEL_SAT_LAUNCH_BROWSER_IF_NOT_ALREADY_LAUNCHED = 0, /**<	command qualifier for  LAUNCH BROWSER */
	TEL_SAT_LAUNCH_BROWSER_NOT_USED = 1, /**< command qualifier for  NOT USED */
	TEL_SAT_LAUNCH_BROWSER_USE_EXISTING_BROWSER = 2, /**< command qualifier for  USE EXISTING BROWSER if secure session, do not use it */
	TEL_SAT_LAUNCH_BROWSER_CLOSE_AND_LAUNCH_NEW_BROWSER = 3, /**<	command qualifier for CLOSE AND LAUNCH NEW BROWSER	*/
	TEL_SAT_LAUNCH_BROWSER_NOT_USED2 = 4, /**< command qualifier for  NOT USED2 */
	TEL_SAT_LAUNCH_BROWSER_RESERVED = 0xFF /**<	reserved	*/
} TelSatCmdQualifierLaunchBrowser;

/**
 * @enum TelSatCmdQualifierRefresh
 * This enumeration defines cmd qualifier for refresh cmd
*/
typedef enum {
	TEL_SAT_REFRESH_SIM_INIT_AND_FULL_FCN = 0, /**< command qualifier for REFRESH SIM INIT AND FULL FILE CHANGE_NOTIFICATION */
	TEL_SAT_REFRESH_FCN = 1, /**<	command qualifier for REFRESH FILE CHANGE NOTIFICATION */
	TEL_SAT_REFRESH_SIM_INIT_AND_FCN = 2, /**< command qualifier for REFRESH SIM INIT AND FILE CHANGE NOTIFICATION	*/
	TEL_SAT_REFRESH_SIM_INIT = 3, /**< command qualifier for REFRESH SIM INIT */
	TEL_SAT_REFRESH_SIM_RESET = 4, /**< command qualifier for REFRESH SIM RESET */
	TEL_SAT_REFRESH_3G_APPLICATION_RESET = 5, /**<	command qualifier for REFRESH 3G APPLICATION RESET */
	TEL_SAT_REFRESH_3G_SESSION_RESET = 6, /**< command qualifier for REFRESH 3G SESSION RESET	*/
	TEL_SAT_REFRESH_RESERVED = 0xFF /**< command qualifier for REFRESH RESERVED */
} TelSatCmdQualifierRefresh;

/**
 * @enum TelSatCmdQualifierProvideLocalInfo
 * This enumeration defines cmd qualifier for local info
*/
typedef enum {
	TEL_SAT_LOCAL_INFO_LOCATION = 0, /**< local info cmd qualifier -location */
	TEL_SAT_LOCAL_INFO_IMEI = 1, /**< local info cmd qualifier -IMEI */
	TEL_SAT_LOCAL_INFO_NMR = 2, /**< local info cmd qualifier -NMR */
	TEL_SAT_LOCAL_INFO_DATE_TIME_AND_TIMEZONE = 3, /**< local info cmd qualifier -DATE TIME AND TIMEZONE */
	TEL_SAT_LOCAL_INFO_LANGUAGE = 4, /**< local info cmd qualifier -LANGUAGE */
	TEL_SAT_LOCAL_INFO_TIMING_ADVANCE = 5, /**< local info cmd qualifier -TIMING ADVANCE */
	TEL_SAT_LOCAL_INFO_ACT = 6, /**< local info cmd qualifier -ACT */
	TEL_SAT_LOCAL_INFO_ESN = 7, /**< local info cmd qualifier -ESN */
	TEL_SAT_LOCAL_INFO_IMEISV = 8, /* *<local info cmd qualifier -IMEISV */
	TEL_SAT_LOCAL_INFO_MEID = 0x0B, /**< local info cmd qualifier -MEID */
	TEL_SAT_LOCAL_INFO_RESERVED = 0xFF /* local info cmd qualifier -RESERVED */
} TelSatCmdQualifierProvideLocalInfo;

/**
 * @enum TelSatCmdQualifierSetupCall
 * This enumeration defines cmd qualifier for setup call
*/
typedef enum {
	TEL_SAT_SETUP_CALL_IF_ANOTHER_CALL_NOT_BUSY = 0x00, /**< command qualifier for SETUP CALL IF ANOTHER CALL NOT BUSY	 */
	TEL_SAT_SETUP_CALL_IF_ANOTHER_CALL_NOT_BUSY_WITH_REDIAL = 0x01, /**< command qualifier for SETUP CALL IF ANOTHER CALL NOT BUSY WITH REDIAL */
	TEL_SAT_SETUP_CALL_PUT_ALL_OTHER_CALLS_ON_HOLD = 0x02, /**< command qualifier for SETUP CALL PUTTING ALL OTHER CALLS ON HOLD */
	TEL_SAT_SETUP_CALL_PUT_ALL_OTHER_CALLS_ON_HOLD_WITH_REDIAL = 0x03, /**< command qualifier for SETUP CALL PUTTING ALL OTHER CALLS ON HOLD WITH REDIAL */
	TEL_SAT_SETUP_CALL_DISCONN_ALL_OTHER_CALLS = 0x04, /**< command qualifier for SETUP CALL DISCONNECTING ALL OTHER CALLS */
	TEL_SAT_SETUP_CALL_DISCONN_ALL_OTHER_CALLS_WITH_REDIAL = 0x05, /**< command qualifier for SETUP CALL DISCONNECTING ALL OTHER CALLS WITH REDIAL */
	TEL_SAT_SETUP_CALL_RESERVED = 0xFF /**<	 command qualifier for SETUP CALL RESERVED	*/
} TelSatCmdQualifierSetupCall;

/**
 * @enum TelSatCallCtrlProblemType
 * This enumeration defines call control problem types
*/
typedef enum {
	TEL_SAT_CC_PROBLEM_NO_SPECIFIC_CAUSE = 0, /**< Call Control problem with no specific cause */
	TEL_SAT_CC_PROBLEM_ACTION_NOT_ALLOWED = 1, /**<	Call Control problem with action not allowed */
	TEL_SAT_CC_PROBLEM_REQUEST_TYPE_HAS_CHANGED = 2 /**< Call Control problem with request type has changed	*/
} TelSatCallCtrlProblemType;

/**
 * @enum TelSatLaunchBrowserProblemType
 * This enumeration defines launch browser problem types
*/
typedef enum {
	TEL_SAT_BROWSER_PROBLEM_NO_SPECIFIC_CAUSE = 0, /**<	Browser problem with no specific cause */
	TEL_SAT_BROWSER_PROBLEM_BEARER_UNAVAILABLE = 1, /**< Browser problem with bearer unavailable */
	TEL_SAT_BROWSER_PROBLEM_BROWSER_UNAVAILABLE = 2, /**<	Browser problem with browser unavailable */
	TEL_SAT_BROWSER_PRBLM_ME_UNABLE_TO_READ_PROV_DATA = 3 /**< Browser problem with ME unable to read provisioning data */
} TelSatLaunchBrowserProblemType;

/**
 * @enum TelSatBrowserTerminationCauseType
 * This enumeration defines browser termination cause types
*/
typedef enum {
	TEL_SAT_BROWSER_TERMINATED_BY_USER = 0, /**<	BROWSER TERMINATED BY USER */
	TEL_SAT_BROWSER_TERMINATED_BY_ERROR = 1, /**< BROWSER TERMINATED BY ERROR */
} TelSatBrowserTerminationCauseType;

/**
 * This structure defines Result data object.
 */
typedef struct {
	TelSatResultType general_result; /**< general result */
	TelSatMeProblemType me_problem; /**< additional information on general result */
} TelSatResultInfo;

/**
 * This structure defines expected user response length.
 */
typedef struct {
	unsigned char min; /**< user response length minimum value */
	unsigned char max; /**< user response length maximum value */
} TelSatRespLenInfo;

/**
* This structure defines Address Info
*/
typedef struct {
	TelSatTypeOfNum ton; /**< type of number */
	TelSatNumberingPlanIdentity npi; /**< numbering plan identity */
	unsigned int dialing_number_len; /**< Dialing Number length */
	char dialing_number[TEL_SAT_DIALING_NUMBER_LEN_MAX]; /**< Dialing Number */
} TelSatAddressInfo;

/**
* This structure defines Data Coding Scheme Info
*/
typedef struct {
	gboolean is_compressed_format;		/**< if FALSE, unpacked format */
	TelSatAlphabetFormatType a_format; /**< Alphabet format type */
	TelSatMsgClassType m_class; /**< msg class type */
	unsigned char raw_dcs; /**< raw dcs data */
} TelSatDataCodingSchemeInfo;

/**
* This structure defines Alpha identifier Info
*/
typedef struct {
	gboolean is_exist; /**< identifier check */
	TelSatDataCodingSchemeInfo dcs; /**< data coding scheme */
	unsigned char alpha_data_len; /**< alpha identifier length */
	char alpha_data[TEL_SAT_ALPHA_ID_LEN_MAX]; /**< alpha identifier info */
} TelSatAlphaIdentifierInfo;

/**
* This structure defines the Sub Address data object.
*/
typedef struct {
	unsigned int subaddress_len; /**< sub address length */
	char subaddress[TEL_SAT_SUB_ADDR_LEN_MAX]; /**< sub address data */
} TelSatSubAddressInfo;

/**
* This structure defines Capability configuration parameters Info
*/
typedef struct {
	unsigned int data_len; /**< ccp info data length */
	char data[TEL_SAT_CCP_DATA_LEN_MAX]; /**< ccp info data */
} TelSatCcpInfo;

/**
* This structure defines device identities tag Info
*/
typedef struct {
	TelSatDeviceIdentitiesTagType src; /**< src device identities tag type */
	TelSatDeviceIdentitiesTagType dest; /**< dest device identities tag type */
} TelSatDeviceIdentitiesInfo;

/**
* This structure defines duration Info
*/
typedef struct {
	TelSatTimeUnitType time_unit;  /**< duration info -time unit type */
	unsigned char time_interval; /**< duration info -time interval type */
} TelSatDurationInfo;

/**
* This structure defines menu item Info
*/
typedef struct {
	unsigned char item_id; /**< menu item info -item id */
	unsigned char text_len; /**< menu item info -text len */
	unsigned char text[TEL_SAT_ITEM_TEXT_LEN_MAX + 1]; /**< menu item info -text */
} TelSatMenuItemInfo;

/**
* This structure defines Response length Info
*/
typedef struct {
	unsigned char min; /**< min Response length */
	unsigned char max; /**< max Response length */
} TelSatResponseLength;

/**
* This structure defines SMS TPDU Info
*/
typedef struct {
	TelSatSmsTpduType tpdu_type;  /**< SMS TPDU info -tpdu type */
	unsigned int data_len; /**< SMS TPDU info -data len */
	char data[TEL_SAT_SMS_TPDU_SMS_DATA_LEN_MAX]; /**< SMS TPDU info -data */
} TelSatSmsTpduInfo;

/**
* This structure defines SS string Info
*/
typedef struct {
	TelSatTypeOfNum ton; /**< SS string info -type of number */
	TelSatNumberingPlanIdentity npi; /**< SS string info -numbering plan identity */
	unsigned char string_len; /**< SS string length */
	char ss_string[TEL_SAT_SS_STRING_LEN_MAX];	/**< SS string */
} TelSatSsStringInfo;

/**
* This structure defines Text string Info
*/
typedef struct {
	gboolean is_digit_only; /**< text type is digit check */
	TelSatDataCodingSchemeInfo dcs; /**< text data coding scheme */
	unsigned int string_length; /**< text string length */
	char string[TEL_SAT_TEXT_STRING_LEN_MAX+1]; /**< text string */
} TelSatTextTypeInfo;

/**
* This structure defines USSD string Info
*/
typedef struct {
	TelSatDataCodingSchemeInfo dsc; /**< USSD string data coding scheme */
	unsigned char string_len; /**< USSD string length */
	char ussd_string[TEL_SAT_USSD_STRING_LEN_MAX]; /**< USSD string */
} TelSatUssdStringInfo;

/**
* This structure defines File List Info
*/
typedef struct {
	int file_count; /**< file count */
	int file_id[TEL_SAT_FILE_ID_LIST_MAX_COUNT]; /**< file id */
} TelSatFileListInfo;

/**
* This structure defines Location information Info
*/
typedef struct {
	char mcc[3+1]; /**< MCC */
	char mnc[3+1]; /**< MNC */
	char lac [4+1]; /**< LAC */
	char cell_id[4+1]; /**< cell id */
	char extended_cell_id[4+1]; /**< extended cell id */
} TelSatLocationInformationInfo;

/**
* This structure defines Items next action indicator List Info
*/
typedef struct {
	unsigned char cnt; /**< next action indicator items count */
	unsigned char indicator_list[TEL_SAT_ITEMS_NEXT_ACTION_INDI_LIST_MAX_COUNT]; /**< next action indicator items list */
} TelSatItemsNextActionIndiListInfo;

/**
* This structure defines Event List Info
*/
typedef struct {
	int event_list_cnt; /**< event list count */
	TelSatEventListType evt_list[TEL_SAT_EVENT_LIST_MAX]; /**< event list type */
} TelSatEventListInfo;

/**
 * This structure defines call control strings.
 */
typedef struct {
	TelSatCallCtrlStringType call_ctrl_string_type; /**< call control type	*/
	union {
		TelSatAddressInfo voice_string; /**< voice call string */
		TelSatSsStringInfo ss_string; /**< ss string */
		TelSatUssdStringInfo ussd_string; /**< ussd string */
	} call_ctrl_data; /**< call ctrl data Union */
} TelSatCallCtrlAddressStringInfo;

/**
* This structure defines Call control requested action Info
*/
typedef struct {
	TelSatCallCtrlAddressStringInfo call_ctrl_add_string; /**< Call control address string info */
	TelSatCcpInfo ccp1; /**< Call control CCP info */
	TelSatSubAddressInfo sub_address; /**< Call control sub address info */
	TelSatAlphaIdentifierInfo alpha_id; /**< Call control alpha identifier info */
	TelSatBcRepeatIndicatorType bc_repeat_type; /**< Call control bc repeat indicator type info */
	TelSatCcpInfo ccp2; /**< Call control CCP info */
} TelSatCallCtrlRequestedActionInfo;

/**
* This structure defines Item icon Info
*/
typedef struct {
	unsigned char width; /**< icon info -width */
	unsigned char height;  /**< icon info -height */
	TelSatImageCodingSchemeType ics; /**< icon info -image coding scheme */
	unsigned short icon_data_len; /**< icon info -data len */
	unsigned short clut_data_len; /**< icon info -CLUT data len */
	char icon_file[TEL_SAT_IMG_DATA_FILE_PATH_LEN_MAX]; /**< icon info -image data file data */
	char clut_file[TEL_SAT_IMG_DATA_FILE_PATH_LEN_MAX]; /**< icon info -clut file data */
} TelSatIconInfo;

/**
* This structure defines Item icon identifier Info
*/
typedef struct {
	gboolean is_exist; /**< icon identifier check */
	TelSatIconQualifierType icon_qualifer; /**< icon qualifer type */
	unsigned char icon_identifier; /**< icon identifier data */
	TelSatIconInfo icon_info; /**< icon info data */
} TelSatIconIdentifierInfo;

/**
* This structure defines Item icon identifier list Info
*/
typedef struct {
	gboolean is_exist; /**< icon identifier list present check */
	TelSatIconQualifierType icon_qualifer; /**< icon qualifer type */
	unsigned char icon_cnt; /**< icon count */
	unsigned char icon_id_list[TEL_SAT_ICON_LIST_MAX_COUNT]; /**< icon id list data */
	TelSatIconInfo icon_info[TEL_SAT_ICON_LIST_MAX_COUNT]; /**< icon info data */
} TelSatIconIdentifierListInfo;

/**
* This structure defines date, time and timezone Info
*/
typedef struct {
	unsigned char year; /**< year */
	unsigned char month; /**<	month */
	unsigned char day; /**< day */
	unsigned char hour; /**< hour */
	unsigned char minute; /**< minute */
	unsigned char second; /**< second */
	unsigned char time_zone; /**< timeZone	*/
} TelSatDateTimeZoneInfo;

/**
* This structure defines Dtmf String Info
*/
typedef struct {
	unsigned int dtmf_length; /**< DTMF string length */
	char dtmf_string[TEL_SAT_DTMF_STRING_LEN_MAX]; /**< DTMF string */
} TelSatDtmfStringInfo;

/**
 * 8.48 URL
 * URL shall be coded as defined in RFC 1738 on using the "SMS 7bit default alphabet" with bit 8 set to 0.
 */
typedef struct {
	unsigned int url_length; /**< URL length */
	char url[TEL_SAT_URL_LEN_MAX]; /**< URL data */
} TelSatUrlInfo;

/**
* This structure defines Bearer List Info
*/
typedef struct {
	int count; /**< Bearer List count */
	TelSatBearerListType bear[6]; /**< Bearer List type */
} TelSatBearerList;

/**
* This structure defines Provisioning file Info
*/
typedef struct {
	unsigned int file_path_length; /**< Provisioning file path length */
	char file_path[TEL_SAT_PROVISIONING_FILE_PATH_LEN_MAX]; /**< Provisioning file path data */
} TelSatProvisioningFileRef;

/**
* This structure defines channel data Info
*/
typedef struct {
	unsigned char data_string_len; /**< channel data string length */
	char	data_string[TEL_SAT_CHANNEL_DATA_STRING_LEN_MAX]; /**< channel data string */
} TelSatChannelDataInfo;

/**
* This structure defines Channel Status Info
*/
typedef struct {
	unsigned char channel_id; /**< channel id */
	TelSatChannelStatusType status; /**< channel Status Type */
	TelSatChannelStatusInfoType status_info; /**< channel Status info Type */
} TelSatChannelStatusInfo;

/**
* This structure defines Other Address Info
*/
typedef struct {
	TelSatAddressType address_type; /**< channel address type */
	unsigned char address_len; /**< channel address length */
	char address[TEL_SAT_OTHER_ADDR_LEN_MAX]; /**< channel address */
} TelSatOtherAddressInfo;

/**
* This structure defines Me Interface Transport Level Info
*/
typedef struct {
	TelSatTransportProtocolType protocol_type; /**< Transport Protocol Type */
	unsigned short port_number; /**< Transport Protocol port number */
} TelSatSimMeInterfaceTransportLevelInfo;

/**
* This structure defines Remote Entity Address info
*/
typedef struct {
	TelSatRemoteEntityAddrCodingType coding_type; /**< Remote Entity Addr Coding Type */
	unsigned short length;  /**< Remote Entity Addr length */
	unsigned char remote_entity_address[TEL_SAT_REMOTE_ENTITY_ADDR_LEN_MAX]; /**< Remote Entity Addr data */
} TelSatRemoteEntityAddressInfo;

/**
* This structure defines Network access name info
*/
typedef struct {
	unsigned short length; /**< Network access name length */
	unsigned char network_access_name[TEL_SAT_NET_ACC_NAM_LEN_MAX]; /**< Network access name data */
} TelSatNetworkAccessNameInfo;

/**
* This structure defines Item text attribute list
*/
typedef struct {
	unsigned int list_cnt; /**< text attribute list count */
	unsigned char text_attribute_list[TEL_SAT_ITEM_TEXT_ATTRIBUTES_LIST_MAX_COUNT]; /**< text attribute list data */
} TelSatTextAttributeListInfo;


/**
 * This structure defines the call control result ind data.
 */
typedef struct {
	TelSatCallControlResult cc_result; /**< Call Control Result */

	TelSatAddressInfo address; /**< Address Info */
	TelSatSsStringInfo ss_string; /**< Ss String Info  */

	TelSatCcpInfo ccp1; /**< Ccp Info */
	TelSatSubAddressInfo sub_address; /**< Sub Address Info */
	TelSatAlphaIdentifierInfo alpha_id; /**< alpha id */
	TelSatBcRepeatIndicatorType bc_repeat_type; /**< Bc Repeat Indicator Type */
	TelSatCcpInfo ccp2; /**< Ccp Info */
} TelSatNotiCallControlResultInd;

/**
 * This structure defines the mo sms control result ind data.
 */
typedef struct {
	TelSatCallControlResult cc_result; /**< Call Control Result */

	TelSatAddressInfo rp_dst_address; /**< Address Info */
	TelSatAddressInfo tp_dst_address; /**< Address Info */
	TelSatAlphaIdentifierInfo alpha_id; /**< Alpha Identifier Info */
} TelSatNotiMoSmControlResultInd;

/**
 * This structure defines the user confirmation data.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	TelSatProactiveCmdType command_type; /**< Proactive Command Type */
	TelSatUiUserConfirmType key_type; /**< User Response Type */
	unsigned char* additional_data; /**< Additional Data */
	unsigned int data_len; /**< Additional Data Length */
} TelSatUiUserConfirmInfo;

/**
 * This structure defines the character data for sat engine data structure.
 */
typedef struct
{
	unsigned short string_len; 	/**< character data length */
	unsigned char string[TEL_SAT_DEF_TEXT_STRING_LEN_MAX+1]; /**< character data */
} TelSatTextInfo;

/**
 * This structure defines the main menu title to check sat menu.
 */
typedef struct
{
	gboolean is_main_menu_present; /**<flag to check sat main menu existence */
	TelSatTextInfo main_menu_title; /**< main menu title data */
} TelSatMainMenuTitleInfo;

/**
 * This structure defines the display text proactive command for sat ui.
 */
typedef struct
{
	int command_id;	/**< Proactive Command Number sent by USIM */
	TelSatTextInfo text; /**< character data to display on screen */
	unsigned int duration; /**< the duration of display */
	gboolean is_priority_high;	 /**< indicates whether the text is to be displayed if some other app is using the screen */
	gboolean is_user_resp_required; /**< indicates whether user response required or Not */
	gboolean immediately_resp; /**< indicates whether response is immediate or not */
	TelSatIconIdentifierInfo icon_id; /**< Icon Identifier */
} TelSatDisplayTextInd;

/**
 * This structure defines the get inkey proactive command data for sat ui.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	TelSatInkeyType key_type; /**< input Type:Character Set or Yes/No */
	TelSatUseInputAlphabetType input_char_mode; /**< input character mode(SMS default, UCS2) */
	gboolean is_numeric;	/**< is input character numeric(0-9, *, # and +) */
	gboolean is_help_info_available; /**< help info request flag */
	TelSatTextInfo text; /**< character data to display on screen */
	unsigned int duration;	 /**< the duration of display */
	TelSatIconIdentifierInfo icon_id; /**< Icon Identifier */
} TelSatGetInkeyInd;

/**
 * This structure defines the get input proactive command data for sat ui.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	TelSatUseInputAlphabetType input_char_mode; /**< input character mode(SMS default, UCS2) */
	gboolean is_numeric;	/**< is input character numeric(0-9, *, # and +) */
	gboolean is_help_info_available; /**< flag for help info request */
	gboolean is_echo_input; /**< indicates whether to show input data on screen or not */
	TelSatTextInfo text; /**< character data to display on screen */
	TelSatRespLenInfo resp_len; /**< input data min, max length */
	TelSatTextInfo default_text; /**< default input character data */
	TelSatIconIdentifierInfo icon_id; /**< Icon Identifier */
} TelSatGetInputInd;

/**
 * This structure defines the play tone proactive command data for application.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	TelSatTextInfo text; /**< character data to display on screen */
	TelSatToneType tone; /**< tone info */
	unsigned int duration; /**< the duration for playing tone */
	TelSatIconIdentifierInfo icon_id; /**< Icon Identifier */
} TelSatPlayToneInd;

/**
 * This structure defines the data for sat ui.
 */
typedef struct
{
	int command_id;	/**< Proactive Command Number sent by USIM */
	int user_confirm; /**< user confirmation */
	TelSatTextInfo text; /**< character data to display on screen */
	TelSatIconIdentifierInfo icon_id; /**< Icon Identifier */
} TelSatSendUiInfo;

/**
 * This structure defines the menu item info for setup menu.
 */
typedef struct
{
	char item_string[TEL_SAT_DEF_ITEM_STR_LEN_MAX + 6]; /**< menu item character data */
	char item_id; /**< identifies the item on the menu that user selected */
} TelSatMenuInfo;

/**
 * This structure defines the sat main menu info.
 */
typedef struct
{
	int command_id;	/**< Proactive Command Number sent by USIM */
	gboolean is_main_menu_present; /**< is main menu present check */
	char sat_main_title[TEL_SAT_DEF_TITLE_LEN_MAX + 1]; /**< menu title text */
	unsigned short sat_main_menu_item_count; /**< number of menu items */
	TelSatMenuInfo sat_main_menu_item[TEL_SAT_MENU_ITEM_COUNT_MAX]; /**< menu items */
	gboolean is_sat_main_menu_help_info; /**< flag for help information request */
	gboolean is_updated_sat_main_menu; /**< check whether sat_main_menu is updated or not */
	TelSatIconIdentifierInfo icon_id; /**< Icon Identifier */
	TelSatIconIdentifierListInfo icon_id_list; /**< List of Icon Identifiers */
} TelSatSetupMenuInfo;

/**
 * This structure defines the select item proactive command data for sat ui.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	gboolean is_help_info_available; /**< flag for help information request */
	TelSatTextInfo text; /**< menu title text */
	char default_item_index; /**< selected default item - default value is 0 */
	char menu_item_count; /**< number of menu items */
	TelSatMenuItemInfo menu_item[TEL_SAT_MENU_ITEM_COUNT_MAX]; /**< menu items */
	TelSatIconIdentifierInfo icon_id; /**< Icon Identifier */
	TelSatIconIdentifierListInfo icon_id_list; /**< List of Icon Identifiers */
}TelSatSelectItemInd;

/**
 * This structure defines the setup idle mode text proactive command for idle application.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	TelSatTextInfo text; /**< character data to display on screen */
	TelSatIconIdentifierInfo icon_id; /**< Icon Identifier */
} TelSatSetupIdleModeTextInd;

/**
 * This structure defines the refresh proactive command data for sat ui.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	unsigned int duration; /**< the duration of display */
	TelSatCmdQualifierRefresh	refresh_type; /**< refresh mode */
} TelSatRefreshIndUiInfo;

/**
 * This structure defines the refresh proactive command data for applications which are concerned with files resident on USIM .
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	TelSatRefreshAppType	 app_type; /**< concerned application type */
	TelSatCmdQualifierRefresh	refresh_mode; /**< refresh mode */
	unsigned char file_count; /**< refresh file count */
	int file_id[TEL_SAT_REFRESH_FILE_LIST]; /**< refresh file identifier */
} TelSatRefreshInd;

/**
 * This structure defines the data objects to indicate to sat ui, the end of execution of a specific proactive command by other application.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	char command_type; 	/**< Command Type */
	gboolean result; /**< result of command execution by application */
} TelSatProactiveCmdEndIndInfo;

/**
 * This structure defines the send sms proactive command data for sms application.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	gboolean is_packing_required; /**< flag to check if packing required for sms tpdu */
	TelSatAddressInfo address; /**< destination address */
	TelSatSmsTpduInfo sms_tpdu; /**< SMS TPDU data */
} TelSatSendSmsIndSmsData;

/**
 * This structure defines the send ss proactive command data for ss application.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	unsigned short ss_string_len; /**< SS string data length */
	unsigned char ss_string[TEL_SAT_DEF_SS_LEN_MAX+1]; /**< SS string data */
} TelSatSendSsIndSsData;

/**
 * This structure defines the send ussd proactive command data for ussd application.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	unsigned char raw_dcs; /**< data coding scheme */
	unsigned short ussd_string_len; /**< USSD string data length */
	unsigned char ussd_string[TEL_SAT_DEF_USSD_LEN_MAX+1]; /**< USSD string data */
} TelSatSendUssdIndUssdData;

/**
 * This structure defines the send dtmf proactive command data for dtmf application.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	gboolean is_hidden_mode; /**< hidden mode flag */
	TelSatTextInfo dtmf_string; /**< dtmf string data */
} TelSatSendDtmfIndDtmfData;

/**
 * This structure defines the setup call proactive command data for call application.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	TelSatCmdQualifierSetupCall call_type; /**< call type */
	TelSatTextInfo disp_text; /**< display data for calling */
	TelSatTextInfo call_number; /**< call number */
	unsigned int duration; /**< maximum repeat duration */
	TelSatIconIdentifierInfo icon_id; /**< icon identifier for call application */
} TelSatSetupCallIndCallData;

/**
 * This structure defines the launch browser proactive command data for browser application.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	char url[TEL_SAT_URL_LEN_MAX + 1]; /**< url to connect */
	TelSatCmdQualifierLaunchBrowser launch_type; /**< launch type */
	TelSatBrowserIdentityType identity_type; /**< Browser Identity -default, html, etc */
} TelSatLaunchBrowserIndBrowserData;

/**
 * This structure defines the provide local info proactive command data for application.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	TelSatCmdQualifierProvideLocalInfo local_info_type; /**< Provide Local Information Type */
} TelSatProvideLocalInfoInd;

/**
 * This structure defines the language notification proactive command data for application.
 */
typedef struct
{
	int command_id; /**< Proactive Command Number sent by USIM */
	gboolean specific_language_notification;  /**< flag for checking specific language notification. if FALSE, non-specific language notification */
	TelSatLanguageInfo language; /**< language info from USIM application */
} TelSatLanguageNotiInfoInd;

/**
 * This structure defines the return result data for refresh proactive command.
 */
typedef struct
{
	TelSatRefreshAppType	 app_type; /**< application type */
	TelSatResultType resp; /**< result response value */
} TelSatRefreshRetInfo;

/**
 * This structure defines the return result data for setup call proactive command.
 */
typedef struct
{
	TelSatResultType resp; /**< result response value */
	gboolean is_tapi_cause_exist; /**< flag for checking tapi error cause */
	TelCallResult call_cause; /**< call error(result) cause */
	TelSsResult ss_cause; /**< ss error(result) cause */
	TelSatMeProblemType me_problem; /**< me problem error cause */
	gboolean is_other_info_exist; /**< call control result existence flag */
	TelSatCallCtrlProblemType permanent_call_ctrl_problem; /**< call control problem type */
	TelSatCallCtrlRequestedActionInfo call_ctrl_requested_action; /**< call control request data */
	TelSatResultInfo result2; /**< call control envelope result value */
	TelSatTextTypeInfo text; /**< call control envelope display data */
} TelSatCallRetInfo;

/**
 * This structure defines the return result data for send ss proactive command.
 */
typedef struct
{
	TelSatResultType resp; /**< result response value */
	TelSsResult ss_cause; /**< ss error(result) cause */
	TelSatTextInfo ss_string; /**< ss result string */
	TelSatMeProblemType me_problem; /**< error - me problem */
	gboolean is_other_info_exist; /**< call control result exist flag */
	TelSatCallCtrlProblemType additional_call_ctrl_problem_info; /**< call control problem */
	TelSatCallCtrlRequestedActionInfo call_ctrl_requested_action; /**< call control request data */
	TelSatResultInfo result2; /**< call control envelope result value */
} TelSatSsRetInfo;

/**
 * This structure defines the return result data for send ussd proactive command.
 */
typedef struct
{
	TelSatResultType resp; /**< result response value */
	TelSsResult ss_cause; /**< ss error(result) cause */
	TelSatTextInfo ussd_string; /**< ussd result string */
	TelSatDataCodingSchemeInfo dcs_ussd_string; /**< dcs of ussd result string */
	TelSatMeProblemType me_problem; /**< error - me problem */
	gboolean is_other_info_exist; /**< call control result exist flag */
	TelSatCallCtrlProblemType additional_call_ctrl_problem_info; /**< call control problem */
	gboolean is_call_ctrl_has_modification; /**< call control request modification flag */
	TelSatCallCtrlRequestedActionInfo call_ctrl_requested_action; /**< call control request data */
	TelSatResultInfo result2; /**< call control envelope result value */
	TelSatTextTypeInfo text2; /**< cc envelope display data */
} TelSatUssdRetInfo;

/**
 * This structure defines the return result data for launch browser proactive command.
 */
typedef struct
{
	TelSatResultType resp; /**< result response value */
	TelSatLaunchBrowserProblemType browser_problem; /**< specific browser problem */
} TelSatBrowserRetInfo;

/**
 * This structure defines the return result data for setup idle mode text proactive command.
 */
typedef struct
{
	TelSatResultType resp; /**< result response value */
	gboolean other_info; /**< flag to check whether other information are required or not */
	TelSatCmdQualifierProvideLocalInfo	info_type; /**< local info type - e.g. time zone or language info, etc */
	union
	{
		TelSatDateTimeZoneInfo time_zone_info; /**< current time zone info */
		TelSatLanguageInfo language_info; /**< current ME language setting info */
	}local_info; /**< Union */
} TelSatProvideLocalRetInfo;

/**
 * This structure defines the return result data for setup idle mode text proactive command.
 */
typedef struct
{
	TelSatResultType resp; /**< result response value */
	TelSatMeProblemType	 me_problem; /**< Me Problem Type */
} TelSatDiplayTextRetInfo;

/**
 * This structure defines the common return result data for applications proactive command.
 */
typedef struct
{
	TelSatProactiveCmdType command_type; /**< Proactive Command type */
	int command_id;	/**< Proactive Command Number sent by USIM */
	union
		{
			TelSatResultType setup_menu_resp; /**< result response value for setup menu */
			TelSatRefreshRetInfo refresh; /**< result response value for refresh */
			TelSatCallRetInfo setup_call; /**< result response value for setup call */
			TelSatSsRetInfo send_ss; /**< result response value for send ss */
			TelSatUssdRetInfo send_ussd; /**< result response value for send ussd */
			TelSatResultType send_sms_resp; /**< result response value for send sms */
			TelSatResultType send_dtmf_resp; /**< result response value for send dtmf */
			TelSatBrowserRetInfo launch_browser; /**< result response value for launch browser */
			TelSatResultType setup_idle_mode_text_resp; /**< result response value for setup idle mode text */
			TelSatResultType language_noti_resp; /**< result response value for language notification */
			TelSatProvideLocalRetInfo provide_local_info; /**< result response value for provide local info */
			TelSatDiplayTextRetInfo display_text; /**< result response value for dsiplay text */
			TelSatResultType play_tone_resp; /**< result response value for play tone */
		}apps_ret; /**< common union result value */
}TelSatAppsRetInfo;

/**
 * This structure defines the call control confirm data for call.
 */
typedef struct
{
	TelSatTextInfo address; /**< call destination address */
	TelSatTextInfo sub_address; /**< call SUB address */
	TelSatBcRepeatIndicatorType bc_repeat_indicator; /**< bc repeat indicator*/
	TelSatTextInfo ccp1; /**< Configuration Capability Parameter 1*/
	TelSatTextInfo ccp2; /**< Configuration Capability Parameter 2*/
} TelSatCallCtrlIndCallData;

/**
 * This structure defines the call control confirm data for ss.
 */
typedef struct
{
	TelSatTextInfo ss_string; /**< ss destination address */
	TelSatTextInfo sub_address; /**< call SUB address */
	TelSatBcRepeatIndicatorType bc_repeat_indicator; /**< bc repeat indicator */
	TelSatTextInfo ccp1; /**< Configuration Capability Parameter 1 */
	TelSatTextInfo ccp2; /**< Configuration Capability Parameter 2 */
} TelSatCallCtrlIndSsData;

/**
 * This structure defines the common call control confirm data.
 */
typedef struct
{
	TelSatCallType call_ctrl_cnf_type; /**< call control confirm type - call, ss or ussd */
	TelSatCallCtrlResultType call_ctrl_result; /**< call control result */
	TelSatTextInfo disp_data; /**< call control display data */
	gboolean is_user_info_display_enabled; /**< flag for checking existence of call control display */

	union
		{
			TelSatCallCtrlIndCallData call_ctrl_cnf_call_data; /**< call control call address */
			TelSatCallCtrlIndSsData call_ctrl_cnf_ss_data; /**< call control ss string */
			TelSatTextInfo call_ctrl_cnf_ussd_data; /**< call control ussd string */
		}call_ctrl_data; /**< Union */
} TelSatCallCtrlIndData;

/**
 * This structure defines the mo sms control confirm data.
 */
typedef struct
{
	TelSatCallCtrlResultType mo_sms_ctrl_result; /**< envelope result */
	gboolean is_user_info_display_enabled; /**< display present flag */
	TelSatTextTypeInfo disp_data; /**< display data for sending SMS */
	TelSatTextTypeInfo rp_dest_addr; /**< the RP_Destination_Address of the Service Center */
	TelSatTextTypeInfo tp_dest_addr; /**< the TP_Destination_Address */
} TelSatMoSmCtrlIndData;

/**
 * This structure defines the Event List Info.
 */
typedef struct
{
	gboolean is_event_download_active; /**< Is Event Download Active */
	gboolean is_mt_call_event; /**< Is Mt Call Event */
	gboolean is_call_connected; /**< Is Call Connected */
	gboolean is_call_disconnected; /**< Is Call Disconnected */
	gboolean is_location_status; /**< Is Location Status */
	gboolean is_user_activity; 	/**< Is User Activity */
	gboolean is_idle_screen_available;	/**< Is Idle Screen Available */
	gboolean is_card_reader_status; /**< Is Card Reader Status */
	gboolean is_language_selection; /**< Is Language Selection */
	gboolean is_browser_termination; 	/**< Is Browser Termination */
	gboolean is_data_available; /**< Is Data Available */
	gboolean is_channel_status; /**< Is Channel Status */
} TelSatEventListData;

/**
 * This structure contains the data objects for MENU SELECTION envelope.
 */
typedef struct
{
	unsigned char item_identifier; /**< menu selection item identifier */
	gboolean is_help_requested; /**< flag to check whether help information required or not */
} TelSatMenuSelectionReqInfo;

/**
 * This struct contains the data objects for Call Control result data sent by USIM.
 */
typedef struct
{
	TelSatAddressInfo address; /**< call number */
	TelSatSubAddressInfo sub_address; /**< call number sub address */
	TelSatBcRepeatIndicatorType bc_repeat_indicator; /**< bc repeat indicator */
	TelSatCcpInfo ccp1; /**< capability configuration parameter1 */
	TelSatCcpInfo ccp2; /**< capability configuration parameter2 */
}TelSatVoiceCallCtrlIndInfo;

/**
 * This struct contains SAT ss control result data sent by USIM.
 */
typedef struct
{
	TelSatSsStringInfo ss_string; /**< ss number */
	TelSatSubAddressInfo sub_address; /**< ss sub address */
	TelSatBcRepeatIndicatorType bc_repeat_indicator; /**< bc repeat indicator */
	TelSatCcpInfo ccp1; /**< capability configuration parameter1 */
	TelSatCcpInfo ccp2; /**< capability configuration parameter2 */
}TelSatSsCtrlIndInfo;

/**
 * This struct contains SAT mo SMS control configuration data
 */
typedef struct
{
	TelSatAddressInfo rp_dest_address; /**< sms control rp destination address */
	TelSatAddressInfo	tp_dest_address; /**<	 sms control tp destination address */
} TelSatMoSmsCtrlIndInfo;

/**
 * This struct contains SAT call control configuration data
 */
typedef struct
{
	TelSatCallType call_type; /**< call type */
	TelSatCallCtrlResultType call_ctrl_result; /**< call control result	*/
	TelSatAlphaIdentifierInfo alpha_identifier; /**< alpha identifier */
	unsigned char call_id; /**< call id */
	TelSatCallType old_call_type; /**< old call type */
	union
	{
		TelSatVoiceCallCtrlIndInfo 	voice_call_data; /**< voice call control data */
		TelSatSsCtrlIndInfo ss_data; /**< ss control data */
		TelSatMoSmsCtrlIndInfo sms_data; /**< Mo Sms Ctrl Ind Info */
	}call_ctrl_data; /**< Union */
} TelSatCallCtrlIndInfo;

/**
 * This struct contains SAT mo ss control request data
 */
 typedef struct
{
	TelSatCallCtrlResultType call_ctrl_result; /**< call control result	*/
	TelSatAlphaIdentifierInfo alpha_identifier; /**< alpha identifier */
	TelSatMoSmsCtrlIndInfo sms_data; 	/**< sms control data */
}TelSatMoSMCtrlResult;

/**
 * This struct contains SAT user activity event request data
 */
typedef struct
{
	TelSatEventListInfo event_list; /**< event List	 */
	TelSatDeviceIdentitiesInfo device_identities;	/**<	device identities info */

} TelSatUserActivityEventReqInfo;

/**
 * This struct contains SAT data available event request data
 */
typedef struct
{
	TelSatEventListInfo event_list; /**< event List	 */
	TelSatDeviceIdentitiesInfo device_identities;	/**<	device identities info */
	TelSatChannelStatusInfo channel_status; /**< channel status */
	unsigned char channel_data_len; /**< channel data length */

} TelSatDataAvailableEventReqInfo;

/**
 * This struct contains SAT channel status even request data
 */
typedef struct
{
	TelSatEventListInfo event_list; /**< event list */
	TelSatDeviceIdentitiesInfo device_identities;	/**<	device identities info */
	TelSatChannelStatusInfo channel_status; /**< channel Status */
} TelSatChannelStatusEventReqInfo;

/**
 * This is the structure to be used by the Application to send envelope/event download data.
 * This contains the data structures to be used to send any envelope/event download data.
 */
typedef struct
{
	TelSatEventListType event_download_type; /**< eventDownload Type */
	union
	{
		gboolean is_idle_screen_available; /**< flag to specify whether Idle Screen is Available or not */
		TelSatLanguageInfo language_selection_event_req_info; /**< Selected Language Information */
		TelSatBrowserTerminationCauseType browser_termination_event_req_info; /**< BrowserTermination Event Information */
		TelSatDataAvailableEventReqInfo data_available_event_req_info; /**< dataAvailableEventReqInfo */
		TelSatChannelStatusEventReqInfo channel_status_event_req_info; /**< channelStatusEventReqInfo */
	} event_download_data; /**< Union */
} TelSatEventDownloadReqInfo;

/**
 *  \defgroup TAPI_EVENT TAPI Event IDs
 *  @{
 */

/**
 *  \defgroup TEL_EVENT_SAT SAT Event IDs
 *  @{
 */

/**
 *  @hideinitializer
 *  @brief Setup menu information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAT_SETUP_MENU
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAT_SETUP_MENU
 *  @retval data reference to \ref TelSatSetupMenuInfo structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAT_SETUP_MENU					TELEPHONY_SAT_INTERFACE":SetupMenu"

/**
 *  @hideinitializer
 *  @brief Display text information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAT_DISPLAY_TEXT
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAT_DISPLAY_TEXT
 *  @retval data reference to \ref TelSatDisplayTextInd structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAT_DISPLAY_TEXT					TELEPHONY_SAT_INTERFACE":DisplayText"

/**
 *  @hideinitializer
 *  @brief Select item information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAT_SELECT_ITEM
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAT_SELECT_ITEM
 *  @retval data reference to \ref TelSatSelectItemInd structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAT_SELECT_ITEM					TELEPHONY_SAT_INTERFACE":SelectItem"

/**
 *  @hideinitializer
 *  @brief Get inkey information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAT_GET_INKEY
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAT_GET_INKEY
 *  @retval data reference to \ref TelSatGetInkeyInd structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAT_GET_INKEY						TELEPHONY_SAT_INTERFACE":GetInkey"

/**
 *  @hideinitializer
 *  @brief Get input information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAT_GET_INPUT
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAT_GET_INPUT
 *  @retval data reference to \ref TelSatGetInputInd structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAT_GET_INPUT						TELEPHONY_SAT_INTERFACE":GetInput"

/**
 *  @hideinitializer
 *  @brief Refresh information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAT_REFRESH
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAT_REFRESH
 *  @retval data reference to \ref TelSatRefreshInd structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAT_REFRESH						TELEPHONY_SAT_INTERFACE":Refresh"

/**
 *  @hideinitializer
 *  @brief Send SMS information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAT_SEND_SMS
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAT_SEND_SMS
 *  @retval data reference to \ref TelSatSendSmsIndSmsData structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAT_SEND_SMS						TELEPHONY_SAT_INTERFACE":SendSMS"

/**
 *  @hideinitializer
 *  @brief Setup event list information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAT_SETUP_EVENT_LIST
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAT_SETUP_EVENT_LIST
 *  @retval data reference to \ref TelSatEventListData structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAT_SETUP_EVENT_LIST				TELEPHONY_SAT_INTERFACE":SetupEventList"

/**
 *  @hideinitializer
 *  @brief Send DTMF information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAT_SEND_DTMF
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAT_SEND_DTMF
 *  @retval data reference to \ref TelSatSendDtmfIndDtmfData structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAT_SEND_DTMF						TELEPHONY_SAT_INTERFACE":SendDtmf"

/**
 *  @hideinitializer
 *  @brief Session end event information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAT_SESSION_END_EVENT
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAT_SESSION_END_EVENT
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAT_SESSION_END_EVENT			TELEPHONY_SAT_INTERFACE":EndProactiveSession"

/**
 *  @hideinitializer
 *  @brief Call control information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAT_CALL_CONTROL_RESULT
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAT_CALL_CONTROL_RESULT
 *  @retval data reference to \ref TelSatCallCtrlIndData structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAT_CALL_CONTROL_RESULT			TELEPHONY_SAT_INTERFACE":CallControlResult"

/**
 *  @hideinitializer
 *  @brief MO SM control information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAT_MO_SM_CONTROL_RESULT
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAT_MO_SM_CONTROL_RESULT
 *  @retval data reference to \ref TelSatMoSmCtrlIndData structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAT_MO_SM_CONTROL_RESULT		TELEPHONY_SAT_INTERFACE":MoSmControlResult"

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
 #endif /* __TEL_SAT_H_ */
