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

#ifndef __TYPE_SAT_H__
#define __TYPE_SAT_H__

__BEGIN_DECLS

#include <glib.h>
#include <type/sim.h>

#define SAT_DEF_TITLE_LEN_MAX 50 /**< max length for Menu Title */
#define SAT_DEF_ITEM_STR_LEN_MAX 50

#define SAT_TEXT_STRING_LEN_MAX 500
#define SAT_MENU_ITEM_COUNT_MAX 40
#define SAT_DIALING_NUMBER_LEN_MAX 200
#define SAT_SS_STRING_LEN_MAX 160
#define SAT_USSD_STRING_LEN_MAX 255
#define SAT_ALPHA_ID_LEN_MAX 255		/**<	max length of  alpha identifier		*/
#define SAT_ITEM_TEXT_LEN_MAX 255		/**<	max length of item text		*/
#define SAT_SMS_TPDU_SMS_DATA_LEN_MAX 175
#define SAT_ITEMS_NEXT_ACTION_INDI_LIST_MAX_COUNT 50	 /**<	max count of items next action indication list	*/
#define SAT_IMG_DATA_FILE_PATH_LEN_MAX 50	 /**<   max length of image data file name (Icon, CLUT)		*/
#define SAT_ICON_LIST_MAX_COUNT	50	 /**<	max count of icon list		*/
#define SAT_DTMF_STRING_LEN_MAX 30	 /**<	max length of dtmf string		*/
#define SAT_URL_LEN_MAX 129	 /**<	max length of URL		*/
#define SAT_ITEM_TEXT_ATTRIBUTES_LIST_MAX_COUNT	50	 /**<	max count of item text attributes list		*/
#define SAT_EVENT_LIST_MAX	17	/**< max count of event list */
#define SAT_FILE_ID_LIST_MAX_COUNT 255
#define SAT_ENVELOPE_CMD_LEN_MAX 256
#define SAT_AID_LEN_MAX 128
#define SAT_SUB_ADDR_LEN_MAX 30
#define SAT_CCP_DATA_LEN_MAX 30	 /**<	max length of ccp data		*/
#define SAT_URL_LEN_MAX 129	 /**<	max length of url		*/
#define SAT_BEARER_LIST_MAX_COUNT 5	 /**<	max count of bearer list		*/
#define SAT_PROVISIONING_REF_MAX_COUNT 10	/**<	max count of sat provisioning reference	*/
#define SAT_PROVISIONING_FILE_PATH_LEN_MAX 50	 /**<	max length of provisioning file path	*/
#define SAT_BEARER_PARAMS_LEN_MAX 10	 /**<	max length of bearer parameters		*/
#define SAT_OTHER_ADDR_LEN_MAX 30	 /**<	max length of other address		*/
#define SAT_NET_ACC_NAM_LEN_MAX 30	 /**<	max length of net acc name		*/
#define SAT_REMOTE_ENTITY_ADDR_LEN_MAX 50	 /**<	max length of remote entity address	*/
#define SAT_CHANNEL_DATA_STRING_LEN_MAX 255

enum tel_sat_proactive_cmd_type {
	SAT_PROATV_CMD_NONE = 0x00, /**<	command type - None		*/
	SAT_PROATV_CMD_REFRESH = 0x01, /**<	command type - refresh		*/
	SAT_PROATV_CMD_MORE_TIME = 0x02, /**<	command type - more time		*/
	SAT_PROATV_CMD_SETUP_EVENT_LIST = 0x05, /**<	command type - setup event list		*/
	SAT_PROATV_CMD_SETUP_CALL = 0x10, /**<	command type - setup call		*/
	SAT_PROATV_CMD_SEND_SS = 0x11, /**<	command type - send ss		*/
	SAT_PROATV_CMD_SEND_USSD = 0x12, /**<	command type - send ussd		*/
	SAT_PROATV_CMD_SEND_SMS = 0x13, /**<	command type - send sms		*/
	SAT_PROATV_CMD_SEND_DTMF = 0x14, /**<	command type - send dtmf		*/
	SAT_PROATV_CMD_LAUNCH_BROWSER = 0x15, /**<	command type - launch browser		*/
	SAT_PROATV_CMD_PLAY_TONE = 0x20, /**<	command type - play tone		*/
	SAT_PROATV_CMD_DISPLAY_TEXT = 0x21, /**<	command type - display text		*/
	SAT_PROATV_CMD_GET_INKEY = 0x22, /**<	command type - get inkey		*/
	SAT_PROATV_CMD_GET_INPUT = 0x23, /**<	command type - get input		*/
	SAT_PROATV_CMD_SELECT_ITEM = 0x24, /**<	command type - select item		*/
	SAT_PROATV_CMD_SETUP_MENU = 0x25, /**<	command type - setup menu		*/
	SAT_PROATV_CMD_PROVIDE_LOCAL_INFO = 0x26, /**<	command type - provide local info		*/
	SAT_PROATV_CMD_SETUP_IDLE_MODE_TEXT = 0x28, /**<	command type - setup idle mode text		*/
	SAT_PROATV_CMD_LANGUAGE_NOTIFICATION = 0x35, /**<	command type - language notification		*/
	SAT_PROATV_CMD_OPEN_CHANNEL = 0x40, /**<	command type - open channel -class e		*/
	SAT_PROATV_CMD_CLOSE_CHANNEL = 0x41, /**<	command type - close channel - class e		*/
	SAT_PROATV_CMD_RECEIVE_DATA = 0x42, /**<	command type - receive data -class e		*/
	SAT_PROATV_CMD_SEND_DATA = 0x43, /**<	command type - send data		*/
	SAT_PROATV_CMD_GET_CHANNEL_STATUS = 0x44, /**<	command type - get channel status -class e		*/
	SAT_PROATV_CMD_TYPE_END_PROACTIVE_SESSION = 0xFE,
	SAT_PROATV_CMD_RESERVED = 0xFF /**<	command type - reserved		*/
};

/*
 * TS 102.223 V6.12.0
 * SAT DATA OBJECT DEFINE
 */

/*
 * Data Coding Scheme Object
 */
enum alphabet_format{
	ALPHABET_FORMAT_SMS_DEFAULT = 0x00,
	ALPHABET_FORMAT_8BIT_DATA = 0x01,
	ALPHABET_FORMAT_UCS2 = 0x02,
	ALPHABET_FORMAT_RESERVED = 0x03
};

enum msg_class{
	MSG_CLASS_NONE =0x00,
	MSG_CLASS_0 =0x01, //class 0
	MSG_CLASS_1, // class 1 Default meaning:ME-specific
	MSG_CLASS_2, // class 2 SIM specific message
	MSG_CLASS_3, // class 3 Default meaning: TE specific
	MSG_CLASS_RESERVED =0xFF
};

struct data_coding_scheme{
	int is_compressed_format;		// if FALSE, unpacked format
	enum alphabet_format a_format;
	enum msg_class m_class;
	unsigned char raw_dcs;
};

/*
 * 8.1 Address
 */
enum type_of_number{
	TON_UNKNOWN = 0,			/*< unknown */
	TON_INTERNATIONAL = 1,		/*< international number */
	TON_NATIONAL = 2,			/*< national number */
	TON_NETWORK_SPECIFIC = 3,	/*< network specific number */
	TON_DEDICATED_ACCESS = 4,	/*< subscriber number */
	TON_ALPHA_NUMERIC = 5,		/*< alphanumeric, GSM 7-bit default alphabet) */
	TON_ABBREVIATED_NUMBER = 6,	/*< abbreviated number */
	TON_RESERVED_FOR_EXT = 0xF	/*< reserved for extension */
};

enum numbering_plan_identifier{
	NPI_UNKNOWN = 0,				/*< Unknown */
	NPI_ISDN_TEL = 1,				/*< ISDN/Telephone numbering plan */
	NPI_DATA_NUMBERING_PLAN = 3,	/*< Data numbering plan */
	NPI_TELEX = 4,					/*< Telex numbering plan */
	NPI_SVC_CNTR_SPECIFIC_PLAN = 5,	/*< Service Center Specific plan */
	NPI_SVC_CNTR_SPECIFIC_PLAN2 = 6,/*< Service Center Specific plan */
	NPI_NATIONAL = 8,				/*< National numbering plan */
	NPI_PRIVATE = 9,				/*< Private numbering plan */
	NPI_ERMES_NUMBERING_PLAN = 10,	/*< ERMES numbering plan */
	NPI_RESERVED_FOR_EXT = 0xF		/*< Reserved for extension */
};

struct tel_sat_address{
	enum type_of_number ton;
	enum numbering_plan_identifier npi;
	int dialing_number_len;					/**< Dialing Number length */
	char dialing_number[SAT_DIALING_NUMBER_LEN_MAX];	/**< Dialing Number*/
};

/*
 * 8.2 Alpha identifier
 */
struct tel_sat_alpha_identifier{
	gboolean is_exist;
	struct data_coding_scheme dcs;
	unsigned char alpha_data_len; /**<	alpha identifier length		*/
	char alpha_data[SAT_ALPHA_ID_LEN_MAX]; /**<	alpha identifier info	*/
};

/*
 * 8.3 Subaddress
 */
struct tel_sat_subaddress{
	int subaddress_len;
	char subaddress[SAT_SUB_ADDR_LEN_MAX];
};

/*
 * 8.4 Capability configuration parameters
 */
struct tel_sat_ccp{
	int data_len;
	char data[SAT_CCP_DATA_LEN_MAX];
};

/*
 * 8.6 Command details
 */
enum display_text_priority{
	TEXT_PRIORITY_NORMAL = 0,
	TEXT_PRIORITY_HIGH = 1
};

enum display_text_clear_type{
	TEXT_AUTO_CLEAR_MSG_AFTER_A_DELAY = 0,
	TEXT_WAIT_FOR_USER_TO_CLEAR_MSG = 1
};

struct tel_sat_cmd_qualifier_display_text{
	enum display_text_priority text_priority;
	enum display_text_clear_type text_clear_type;
};

enum inkey_type{
	INKEY_TYPE_CHARACTER_SET_ENABLED = 0,
	INKEY_TYPE_YES_NO_REQUESTED = 1

};

enum input_alphabet_type{
	INPUT_ALPHABET_TYPE_SMS_DEFAULT = 1,
	INPUT_ALPHABET_TYPE_UCS2 = 2
};

struct tel_sat_cmd_qualifier_get_inkey{
	enum inkey_type inkey_type;
	enum input_alphabet_type alphabet_type;
	gboolean alphabet_set;
	gboolean immediate_rsp_required;
	gboolean help_info;
};

struct tel_sat_cmd_qualifier_get_input{
	enum input_alphabet_type alphabet_type;
	gboolean alphabet_set;
	gboolean me_echo_user_input;
	gboolean user_input_unpacked_format;
	gboolean help_info;
};

enum vibration_alert{
	VIBRATE_ALERT_OPTIONAL = 0, /**<	VIBRATE ALERT UPTO THE TERMINAL		*/
	VIBRATE_ALERT_REQUIRED = 1 /**<	VIBRATE, IF AVAILABLE, WITH TONE.		*/
};

struct tel_sat_cmd_qualifier_play_tone{
	enum vibration_alert vibration_alert;
};

struct tel_sat_cmd_qualifier_refresh{
	enum tel_sim_refresh_command refresh;
};

enum provide_local_info_command{
	LOCAL_INFO_LOCATION = 0,
	LOCAL_INFO_IMEI = 1,
	LOCAL_INFO_NMR = 2,
	LOCAL_INFO_DATE_TIME_AND_TIMEZONE = 3,
	LOCAL_INFO_LANGUAGE = 4,
	LOCAL_INFO_TIMING_ADVANCE = 5,
	LOCAL_INFO_ACT = 6,
	LOCAL_INFO_ESN = 7,
	LOCAL_INFO_IMEISV = 8,
	LOCAL_INFO_MEID = 0x0B,
	LOCAL_INFO_RESERVED = 0xFF
};

struct tel_sat_cmd_qualifier_provide_local_info{
	enum provide_local_info_command provide_local_info;
};

enum setup_call_command{
	SETUP_CALL_IF_ANOTHER_CALL_NOT_BUSY = 0x00, /**<	command qualifier for SETUP CALL IF ANOTHER CALL NOT BUSY		*/
	SETUP_CALL_IF_ANOTHER_CALL_NOT_BUSY_WITH_REDIAL = 0x01, /**<	command qualifier for SETUP CALL IF ANOTHER CALL NOT BUSY WITH REDIAL		*/
	SETUP_CALL_PUT_ALL_OTHER_CALLS_ON_HOLD = 0x02, /**<	command qualifier for SETUP CALL PUTTING ALL OTHER CALLS ON HOLD		*/
	SETUP_CALL_PUT_ALL_OTHER_CALLS_ON_HOLD_WITH_REDIAL = 0x03, /**<	command qualifier for SETUP CALL PUTTING ALL OTHER CALLS ON HOLD WITH REDIAL		*/
	SETUP_CALL_DISCONN_ALL_OTHER_CALLS = 0x04, /**<	command qualifier for SETUP CALL DISCONNECTING ALL OTHER CALLS		*/
	SETUP_CALL_DISCONN_ALL_OTHER_CALLS_WITH_REDIAL = 0x05, /**<	command qualifier for SETUP CALL DISCONNECTING ALL OTHER CALLS WITH REDIAL		*/
	SETUP_CALL_RESERVED = 0xFF /**<	command qualifier for SETUP CALL RESERVED		*/
};

struct tel_sat_cmd_qualifier_setup_call{
	enum setup_call_command setup_call;
};

enum presentation_type{
	PRESENTATION_TYPE_NOT_SPECIFIED	= 0x00,
	PRESENTATION_TYPE_DATA_VALUE			= 0x01,
	PRESENTATION_TYPE_NAVIGATION_OPTION	= 0x02

};

enum selection_preference_type {
	SELECTION_PREFERENCE_NONE_REQUESTED = 0,
	SELECTION_PREFERENCE_USING_SOFT_KEY = 1
};

struct tel_sat_cmd_qualifier_select_item{
	enum presentation_type presentation_type;
	enum selection_preference_type select_preference;
	gboolean help_info;
};

struct tel_sat_cmd_qualifier_send_sms{
	gboolean packing_by_me_required;
};

struct tel_sat_cmd_qualifier_language_notification{
	gboolean specific_language;
};

enum launch_browser_command{
	LAUNCH_BROWSER_IF_NOT_ALREADY_LAUNCHED = 0, /**<	command qualifier for  LAUNCH BROWSER		*/
	LAUNCH_BROWSER_NOT_USED = 1, /**<	command qualifier for  NOT USED		*/
	LAUNCH_BROWSER_USE_EXISTING_BROWSER = 2, /**<	command qualifier for  USE EXISTING BROWSER if secure session, do not use it.		*/
	LAUNCH_BROWSER_CLOSE_AND_LAUNCH_NEW_BROWSER = 3, /**<	command qualifier for CLOSE AND LAUNCH NEW BROWSER		*/
	LAUNCH_BROWSER_NOT_USED2 = 4, /**<	command qualifier for  NOT USED2		*/
	LAUNCH_BROWSER_RESERVED = 0xFF /**<	reserved	*/
};

struct tel_sat_cmd_qualifier_launch_browser{
	enum launch_browser_command launch_browser;
};

struct tel_sat_cmd_qualifier_setup_menu{
	enum selection_preference_type select_preference;
	gboolean help_info;
};

struct tel_sat_cmd_qualifier_open_channel{
	gboolean immediate_link;
	gboolean automatic_reconnection;
	gboolean background_mode;
};

struct tel_sat_cmd_qualifier_send_data{
	gboolean send_data_immediately;
};

struct tel_sat_cmd_detail_info{
	int cmd_num; /**< specific command number	*/
	enum tel_sat_proactive_cmd_type cmd_type; /**<	proactive command type		*/
	union {
		struct tel_sat_cmd_qualifier_display_text display_text;
		struct tel_sat_cmd_qualifier_get_inkey get_inkey;
		struct tel_sat_cmd_qualifier_get_input get_input;
		struct tel_sat_cmd_qualifier_play_tone play_tone;
		struct tel_sat_cmd_qualifier_setup_menu setup_menu; /**<	setup menu command qualifier info	*/
		struct tel_sat_cmd_qualifier_select_item select_item;
		struct tel_sat_cmd_qualifier_send_sms send_sms;
		struct tel_sat_cmd_qualifier_setup_call setup_call;
		struct tel_sat_cmd_qualifier_refresh refresh;
		struct tel_sat_cmd_qualifier_provide_local_info provide_local_info;
		struct tel_sat_cmd_qualifier_language_notification language_notification;
		struct tel_sat_cmd_qualifier_launch_browser launch_browser;
		struct tel_sat_cmd_qualifier_open_channel open_channel;
		struct tel_sat_cmd_qualifier_send_data send_data;
	}cmd_qualifier; /**<	Union	*/
};

/*
 * 8.7 Device identities
 */
enum device_identity{
	DEVICE_ID_KEYPAD = 0x01,
	DEVICE_ID_DISPLAY = 0x02,
	DEVICE_ID_EARPIECE = 0x03,
	//21-27 for channel
	DEVICE_ID_SIM = 0x81,
	DEVICE_ID_ME = 0x82,
	DEVICE_ID_NETWORK = 0x83,
	DEVICE_ID_RESERVED = 0XFF
};

struct tel_sat_device_identities{
	enum device_identity src;
	enum device_identity dest;
};

/*
 * 8.8 Duration
 */
enum time_unit_type{
	TIME_UNIT_MINUTES						= 0x0,
	TIME_UNIT_SECONDS						= 0x01,
	TIME_UNIT_TENTHS_OF_SECONDS			= 0x02,
	TIME_UNIT_RESERVED						= 0xFF
};

struct tel_sat_duration{
	enum time_unit_type time_unit;
	unsigned char time_interval;
};

/*
 * 8.9 Item
 */
struct tel_sat_item_info{
	unsigned char item_id;
	unsigned char text_len;
	unsigned char text[SAT_ITEM_TEXT_LEN_MAX + 1];
	struct data_coding_scheme dcs;
};

/*
 * 8.10 Item identifier
 */
struct tel_sat_item_identifier{
	unsigned char item_identifier;
};

/*
 * 8.11 Response length
 */
struct tel_sat_response_length{
	unsigned char min;
	unsigned char max;
};

/*
 * 8.12 Result
 */
enum tel_sat_result_type{
	RESULT_SUCCESS = 0x0, /**<	command performed successfully */
	RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION = 0x01, /**<	command performed with partial comprehension	*/
	RESULT_SUCCESS_WITH_MISSING_INFO = 0x02, /**<	command performed, with missing information	*/

	RESULT_REFRESH_PERFORMED_WITH_ADDITIONAL_EFS_READ = 0x03, /**<	REFRESH PERFORMED WITH ADDITIONAL EFS READ		*/
	RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED = 0x04, /**<	command performed but REQUESTED ICON NOT DISPLAYED			*/
	RESULT_SUCCESS_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM = 0x05, /**<	command performed but MODIFIED BY CALL CONTROL BY SIM		*/
	RESULT_SUCCESS_LIMITED_SERVICE = 0x06, /**<	command performed with LIMITED SERVICE		*/
	RESULT_SUCCESS_WITH_MODIFICATION = 0x07, /**<	command performed with MODIFICATION		*/
	RESULT_REFRESH_PRFRMD_BUT_INDICATED_USIM_NOT_ACTIVE = 0x08, /**<	REFRESH PERFORMED BUT INDICATED USIM NOT ACTIVE		*/
	RESULT_SUCCESS_BUT_TONE_NOT_PLAYED = 0x09, /**< command performed successfully, tone not played*/

	RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER = 0x10, /**<	proactive sim application session terminated by user		*/
	RESULT_BACKWARD_MOVE_BY_USER = 0x11, /**<	backward move in the proactive sim application session request by the user		*/
	RESULT_NO_RESPONSE_FROM_USER = 0x12, /**<	no response from user		*/

	RESULT_HELP_INFO_REQUIRED_BY_USER = 0x13, /**<	HELP INFO REQUIRED BY USER		*/
	RESULT_USSD_OR_SS_TRANSACTION_TERMINATED_BY_USER = 0x14, /**<	USSD OR SS TRANSACTION TERMINATED BY USER	*/

	RESULT_ME_UNABLE_TO_PROCESS_COMMAND = 0x20, /**<	ME currently unable to process command		*/
	RESULT_NETWORK_UNABLE_TO_PROCESS_COMMAND = 0x21, /**<	Network currently unable to process command		*/
	RESULT_USER_DID_NOT_ACCEPT_CALL_SETUP_REQ = 0x22, /**<	User did not accept call setup request		*/
	RESULT_USER_CLEAR_DOWN_CALL_BEFORE_CONN = 0x23, /**<	User cleared down call before connection or network released		*/

	RESULT_INTERACTION_WITH_CC_BY_SIM_IN_TMP_PRBLM = 0x25, /**<	INTERACTION WITH CALL CONTROL BY SIM IN TEMPORARY PROBLEM		*/
	RESULT_LAUNCH_BROWSER_GENERIC_ERROR_CODE = 0x26, /**<	LAUNCH BROWSER GENERIC ERROR CODE		*/

	RESULT_BEYOND_ME_CAPABILITIES = 0x30, /**<	command beyond ME's capabilities		*/
	RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME = 0x31, /**<	command type not understood by ME		*/
	RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME = 0x32, /**<	command data not understood by ME		*/
	RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME = 0x33, /**<	command number not known by ME		*/
	RESULT_SS_RETURN_ERROR = 0x34, /**<	SS return error		*/
	RESULT_SMS_RP_ERROR = 0x35, /**<	SMS rp-error		*/
	RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING = 0x36, /**<	Error, required values are missing	*/

	RESULT_USSD_RETURN_ERROR = 0x37, /**<	USSD_RETURN_ERROR		*/
	RESULT_INTRCTN_WITH_CC_OR_SMS_CTRL_PRMNT_PRBLM = 0x39, /**<	INTERACTION WITH CALL CONTROL OR SMS CONTROL PERMANENT PROBLEM	*/
	RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR = 0x3A, /**<	BEARER INDEPENDENT PROTOCOL ERROR		*/
	RESULT_FRAMES_ERROR = 0x3C /**<	FRAMES ERROR		*/
};

enum tel_sat_me_problem_type{
	ME_PROBLEM_NO_SPECIFIC_CAUSE = 0x0, /**<	ME problem with NO SPECIFIC CAUSE		*/
	ME_PROBLEM_SCREEN_BUSY = 0x01, /**<	ME problem with SCREEN BUSY		*/
	ME_PROBLEM_ME_BUSY_ON_CALL = 0x02, /**<	ME problem with ME BUSY ON CALL		*/
	ME_PROBLEM_ME_BUSY_ON_SS = 0x03, /**<	ME problem with ME_BUSY ON SS		*/
	ME_PROBLEM_NO_SERVICE = 0x04, /**<	ME problem with NO SERVICE		*/
	ME_PROBLEM_ACCESS_CONTROL_CLASS_BAR = 0x05, /**<	ME problem with ACCESS CONTROL CLASS BAR		*/
	ME_PROBLEM_RADIO_RES_NOT_GRANTED = 0x06, /**<	ME problem with RADIO RES NOT GRANTED		*/
	ME_PROBLEM_NOT_IN_SPEECH_CALL = 0x07, /**<	ME problem with NOT IN SPEECH CALL	*/
	ME_PROBLEM_ME_BUSY_ON_USSD = 0x08, /**<	ME problem with ME BUSY ON USSD		*/
	ME_PROBLEM_ME_BUSY_ON_SEND_DTMF_CMD = 0x09, /**<	ME problem with ME BUSY ON SEND DTMF CMD	*/
	ME_PROBLEM_NO_USIM_ACTIVE = 0x0A, /**<	ME problem with NO USIM ACTIVE		*/
	ME_PROBLEM_INVALID = 0xFF /**<	ME problem with INVALID		*/
};

enum tel_sat_user_confirm_type{
	USER_CONFIRM_NOT_SPECIFIED = -1,			/**<This Enum Informs That not specified */
	USER_CONFIRM_YES = 0,						/**<This Enum Informs That user confirms yes */
	USER_CONFIRM_NO_OR_CANCEL,				/**<This enum informs that user confirms no/cancel */
	USER_CONFIRM_HELP_INFO,				/**<This enum informs that user wants help information */
	USER_CONFIRM_END,						/**<This enum informs that user confirms end */
	USER_CONFIRM_TIMEOUT,					/**<This enum informs that user did not respond */
};

enum tel_sat_call_ctrl_problem_type{
	CC_PROBLEM_NO_SPECIFIC_CAUSE = 0, /**<	Call Control problem with no specific cause		*/
	CC_PROBLEM_ACTION_NOT_ALLOWED = 1, /**<	Call Control problem with action not allowed	*/
	CC_PROBLEM_REQUEST_TYPE_HAS_CHANGED = 2 /**<	Call Control problem with request type has changed	*/
};

enum ss_problem_type{
	SATK_SS_PROBLEM_NO_SPECIFIC_CAUSE			= 0,
	SATK_SS_PROBLEM_FACILITY_NOT_SUPPORTED		= 0x15
};

enum ussd_problem_type{
	SATK_USSD_PROBLEM_NO_SPECIFIC_CAUSE			= 0,
	SATK_USSD_PROBLEM_UNKNOWN_ALPHABET			= 0x47
};

enum network_problem_type{
	NETWORK_PROBLEM_NO_SPECIFIC_CAUSE = 0x0, /**<	Network problem with no specific cause		*/
	NETWORK_PROBLEM_USER_BUSY = 0x91 /**<	Network problem with USER BUSY		*/
};

enum browser_problem_type{
	BROWSER_PROBLEM_NO_SPECIFIC_CAUSE = 0, /**<	Browser problem with no specific cause		*/
	BROWSER_PROBLEM_BEARER_UNAVAILABLE = 1, /**<	Browser problem with bearer unavailable		*/
	BROWSER_PROBLEM_BROWSER_UNAVAILABLE = 2, /**<	Browser problem with browser unavailable		*/
	BROWSER_PRBLM_ME_UNABLE_TO_READ_PROV_DATA = 3 /**<	Browser problem with ME unable to read provisioning data		*/
};

enum bip_problem_type{
	BIP_PROBLEM_NO_SPECIFIC_CAUSE = 0x00, /**<	BIP problem with no specific cause	*/
	BIP_PROBLEM_NO_CHANNEL_AVAILABLE = 0x01, /**<	BIP problem with no channel available		*/
	BIP_PROBLEM_CHANNEL_CLOSED = 0x02, /**<	BIP problem with channel closed		*/
	BIP_PROBLEM_CHANNEL_ID_NOT_VALID = 0x03, /**<	BIP problem with channel id not valid		*/
	BIP_PROBLEM_BUF_SIZE_NOT_AVAILABLE = 0x04, /**<	BIP problem with buffer size not available		*/
	BIP_PROBLEM_SECURITY_ERROR = 0x05, /**<	BIP problem with security error		*/
	BIP_PRBLM_SIM_ME_IF_TRNSPRT_LEVEL_NOT_AVL = 0x06, /**<	BIP problem with SIM ME interface transport level not available		*/
	BIP_REMOTE_DEV_NOT_REACHABLE = 0x07, /**<	BIP problem with remote device not reachable		*/
	BIP_SERVICE_ERROR = 0x08, /**<	BIP service error		*/
	BIP_SERVICE_IDENTIFIER_UNKNOWN = 0x09 /**<	BIP service identifier unknown		*/
};

/*
 * 8.13 3GPP SMS TPDU
 */
enum sms_tpdu_type{
	SMS_TPDU_TYPE_DELIVER_TPDU	= 0,
	SMS_TPDU_TYPE_DELIVER_RPT	= 1,
	SMS_TPDU_TYPE_SUBMIT_TPDU	= 2,
	SMS_TPDU_TYPE_SUBMIT_RPT	= 3,
	SMS_TPDU_TYPE_STATUS_RPT	= 4,
	SMS_TPDU_TYPE_TPDU_CMD		= 5
};

struct tel_sat_sms_tpdu{
	enum sms_tpdu_type tpdu_type;
	int data_len;
	char data[SAT_SMS_TPDU_SMS_DATA_LEN_MAX];
};

/*
 * 8.14 SS string
 */
struct tel_sat_ss_string{
	enum type_of_number ton;
	enum numbering_plan_identifier npi;
	unsigned char string_len;					/**< SS string length */
	char ss_string[SAT_SS_STRING_LEN_MAX];	/**< SS stringr*/
};

/*
 * 8.15 Text string
 */
struct tel_sat_text_string_object{
	int is_digit_only;
	struct data_coding_scheme dcs;
	int string_length;
	char string[SAT_TEXT_STRING_LEN_MAX+1];
};

/*
 * 8.16 TONE
 */
enum tone_type{
	// standard supervisory tones
	DIAL_TONE = 0x01, /**<	TONE TYPE DIAL TONE		*/
	CALLED_SUBSCRIBER_BUSY = 0x02, /**<	TONE TYPE CALLED SUBSCRIBER BUSY		*/
	CONGESTION = 0x03, /**<	TONE TYPE CONGESTION	*/
	RADIO_PATH_ACK = 0x04, /**<	TONE TYPE RADIO PATH ACK	*/
	RADIO_PATH_NOT_AVAILABLE_CALL_DROPPED = 0x05, /**<	TONE TYPE RADIO PATH NOT AVAILABLE CALL DROPPED	*/
	ERR_SPECIAL_INFO = 0x06, /**<	TONE TYPE ERR SPECIAL INFO	*/
	CALL_WAITING_TONE = 0x07, /**<	TONE TYPE CALL WAITING TONE		*/
	RINGING_TONE = 0x08, /**<	TONE TYPE RINGING TONE	*/

	// ME proprietary tones
	GENERAL_BEEP = 0x10, /**<	TONE TYPE GENERAL BEEP		*/
	POSITIVE_ACK_TONE = 0x11, /**<	TONE TYPE POSITIVE ACK TONE		*/
	NEGATIVE_ACK_OR_ERROR_TONE = 0x12, /**<	TONE TYPE NEGATIVE ACK OR ERROR TONE		*/
	RINGING_TONE_SLCTD_BY_USR_FOR_INCOM_SPEECH_CALL = 0x13, /**<	TONE TYPE RINGING TONE SELECTED BY USER FOR INCOMING SPEECH CALL	*/
	ALERT_TONE_SELECTED_BY_USER_FOR_INCOMING_SMS = 0x14, /**<	TONE TYPE ALERT TONE SELECTED BY USER FOR INCOMING SMS		*/
	CRITICAL_ALERT = 0x15, /**<	TONE TYPE CRITICAL ALERT		*/

	//Themed tones
	HAPPY_TONE = 0x30, /**<	TONE TYPE HAPPY TONE		*/
	SAD_TONE = 0x31, /**<	TONE TYPE SAD TONE			*/
	URGENT_ACTION_TONE = 0x32, /**<	TONE TYPE URGENT ACTION TONE		*/
	QUESTION_TONE = 0x33, /**<	TONE TYPE QUESTION TONE		*/
	MESSAGE_RECEIVED_TONE = 0x34, /**<	TONE TYPE MESSAGE RECEIVED TONE		*/

	//Melody tones
	MELODY_1 = 0x40, /**<	TONE TYPE MELODY 1		*/
	MELODY_2 = 0x41, /**<	TONE TYPE MELODY 2		*/
	MELODY_3 = 0x42, /**<	TONE TYPE MELODY 3		*/
	MELODY_4 = 0x43, /**<	TONE TYPE MELODY 4		*/
	MELODY_5 = 0x44, /**<	TONE TYPE MELODY 5		*/
	MELODY_6 = 0x45, /**<	TONE TYPE MELODY 6		*/
	MELODY_7 = 0x46, /**<	TONE TYPE MELODY 7		*/
	MELODY_8 = 0x47, /**<	TONE TYPE MELODY 8		*/

	TONE_TYPE_RESERVED = 0xFF /**<	TONE TYPE RESERVED		*/
};

struct tel_sat_tone{
	enum tone_type tone_type;
};

/*
 * 8.17 USSD string
 */
struct tel_sat_ussd_string{
	struct data_coding_scheme dsc;
	unsigned char string_len;					/**< USSD string length */
	char ussd_string[SAT_USSD_STRING_LEN_MAX];	/**< USSD stringr*/
};

/*
 * 8.18 File List
 */
/* use enum tel_sim_file_id in sim.h*/

struct tel_sat_file_list{
	int file_count;
	enum tel_sim_file_id file_id[SAT_FILE_ID_LIST_MAX_COUNT];
};

/*
 * 8.19 Location information
 */
struct tel_sat_location_information{
	char mcc[3+1];
	char mnc[3+1];
	char lac [4+1];
	char cell_id[4+1];
	char extended_cell_id[4+1];
};

/*
 * 8.20 IMEI
 */
struct tel_sat_imei{
	char imei[8+1]; // length is defined by 8 in TS 102 223
};

/*
 * 8.22 Network measurement results
 */
struct tel_sat_network_measurement_results{
	char nmr[10+1]; // length is defined by 10 in TS 102 223
};

/*
 * 8.23 Default text
 * It is the same as Text string(8.15)
 */

/*
 * 8.24 Items next action indicator
 */
struct tel_sat_item_next_action_indicatior_list{
	unsigned char cnt;
	unsigned char indicator_list[SAT_ITEMS_NEXT_ACTION_INDI_LIST_MAX_COUNT];
};

/*
 * 8.25 Event list
 */
enum event_list{
	EVENT_MT_CALL = 0,
	EVENT_CALL_CONNECTED = 1,
	EVENT_CALL_DISCONNECTED = 2,
	EVENT_LOCATION_STATUS = 3,
	EVENT_USER_ACTIVITY = 4, /**<	data download type - USER_ACTIVITY		*/
	EVENT_IDLE_SCREEN_AVAILABLE = 5, /**<	data download type - IDLE SCREEN AVAILABLE		*/
	EVENT_CARD_READER_STATUS = 6,
	EVENT_LANGUAGE_SELECTION = 7, /**<	data download type - LANGUAGE SELECTION		*/
	EVENT_BROWSER_TERMINATION = 8, /**<	data download type - BROWSER TERMINATION		*/
	EVENT_DATA_AVAILABLE = 9, /**<	data download type -DATA AVAILABLE		*/
	EVENT_CHANNEL_STATUS = 0x0A, /**<	data download type - CHANNEL STATUS		*/
	EVENT_ACCESS_TECHNOLOGY_CHANGED = 0x0B,
	EVENT_DISPLAY_PARAMETERS_CHANGED = 0x0C,
	EVENT_LOCAL_CONNECTION = 0x0D,
	EVENT_NW_SEARCH_MODE_CHANGED = 0X0E,
	EVENT_BROWSING_STATUS = 0X0F,
	EVENT_FRAMES_INFORMATION_CHANGED = 0X10,
	EVENT_RESERVED_FOR_3GPP = 0X11,
	EVENT_UNKNOWN = 0xFF /**<	data download type - unknown		*/
};

struct tel_sat_event_list{
	int event_list_cnt;
	enum event_list evt_list[SAT_EVENT_LIST_MAX];
};

/*
 * 8.42 BC repeat indicator
 */
enum bc_repeat_indi_type{
	BC_REPEAT_INDI_ALTERNATE_MODE = 0x01, /**<	BC REPEAT ALTERNATE MODE		*/
	BC_REPEAT_INDI_SEQUENTIAL_MODE = 0x03, /**<	BC REPEAT SEQUENTIAL MODE		*/
	BC_REPEAT_INDI_RESERVED = 0xFF /**<	RESERVED	*/
};

struct tel_sat_repeat_indicator_type{
	enum bc_repeat_indi_type bc_indi_repeat_type;
};

/*
 * 8.30 Call control requested action
 */
struct tel_sat_call_ctrl_req_action{
	struct tel_sat_address address;
	struct tel_sat_ccp ccp1;
	struct tel_sat_subaddress sub_address;
	struct tel_sat_alpha_identifier alpha_id;
	struct tel_sat_repeat_indicator_type bc_repeat_type;
	struct tel_sat_ccp ccp2;
};

/*
 * 8.31 Icon identifier
 */
/*enum img_coding_scheme{
	IMAGE_CODING_SCHEME_BASIC = 0x11,
	IMAGE_CODING_SCHEME_COLOUR = 0x21,
	IMAGE_CODING_SCHEME_RESERVED = 0xFF
};*/

enum icon_qualifier{
	ICON_QUALI_SELF_EXPLANATORY = 0,
	ICON_QUALI_NOT_SELF_EXPLANATORY = 1,
	ICON_QUALI_RESERVED = 0xFF
};

struct tel_sat_icon{
	unsigned char width;
	unsigned char height;
	enum img_coding_scheme ics;
	unsigned short icon_data_len;
	unsigned short clut_data_len;
	char icon_file[SAT_IMG_DATA_FILE_PATH_LEN_MAX];
	char clut_file[SAT_IMG_DATA_FILE_PATH_LEN_MAX];
};

struct tel_sat_icon_identifier{
	gboolean is_exist;
	enum icon_qualifier icon_qualifer;
	unsigned char icon_identifier;
	struct tel_sat_icon icon_info;
};

/*
 * 8.32 Item icon identifier next_action_indicator_list
 */
struct tel_sat_icon_identifier_list{
	gboolean is_exist;
	enum icon_qualifier icon_qualifer;
	unsigned char icon_cnt;
	unsigned char icon_id_list[SAT_ICON_LIST_MAX_COUNT];
	struct tel_sat_icon icon_info[SAT_ICON_LIST_MAX_COUNT];
};

/*
 * 8.39 date, time and timezone
 */
struct tel_sat_date_time_and_timezone{
	unsigned char year; /**<	year	*/
	unsigned char month; /**<	month	*/
	unsigned char day; /**<	day	*/
	unsigned char hour; /**<	hour	*/
	unsigned char minute; /**<	minute	*/
	unsigned char second; /**<	second	*/
	unsigned char timeZone; /**<	timeZone	*/
};

/*
 * 8.44 DTMF string
 */
struct tel_sat_dtmf_string{
	int dtmf_length;
	char dtmf_string[SAT_DTMF_STRING_LEN_MAX];
};

/*
 * 8.45 Language
 */
/*
 * Refer enum tel_sim_language_type in sim.h
 */

/*
 * 8.47 Browser Identity
 */
enum browser_identity{
	BROWSER_ID_DEFAULT = 0, /**<	DEFAULT BROWSER		*/
	BROWSER_ID_WML, /**<	BROWSER WML	*/
	BROWSER_ID_HTML, /**<	BROWSER HTML	*/
	BROWSER_ID_XHTML, /**<	BROWSER XHTML	*/
	BROWSER_ID_CHTML, /**<	BROWSER CHTML	*/
	BROWSER_ID_RESERVED = 0xFF /**<	RESERVED	*/
};

/*
 * 8.48 URL
 * URL shall be coded as defined in RFC 1738 on using the "SMS 7bit default alphabet" with bit 8 set to 0.
 */
struct tel_sat_url{
	int url_length;
	char url[SAT_URL_LEN_MAX];
};

/*
 * 8.49 Bear
 */
enum bearer_list_type{
	BEARER_LIST_SMS = 0, /**<	BEARER SMS		*/
	BEARER_LIST_CSD = 1, /**<	BEARER CSD		*/
	BEARER_LIST_USSD = 2, /**<	BEARER USSD		*/
	BEARER_LIST_GPRS = 3, /**<	BEARER GPRS		*/
	BEARER_LIST_RESERVED = 0xFF /**<	BEARER RESERVED		*/
};

struct tel_sat_bearer_list{
	int count;
	enum bearer_list_type bear[6];
};

/*
 * 8.50 Provisioning file reference
 */
struct tel_sat_provisioning_file_ref{
	int file_path_length;
	char file_path[SAT_PROVISIONING_FILE_PATH_LEN_MAX];
};


/*
 * 8.51 Browser termination cause
 */
enum browser_termination_cause{
	BROWSER_TERMINATED_BY_USER = 0, /**<	BROWSER TERMINATED BY USER		*/
	BROWSER_TERMINATED_BY_ERROR = 1, /**<	BROWSER TERMINATED BY ERROR		*/
};

/*
 * 8.52 Bearer description
 */
enum bearer_type{
	BEARER_CSD = 0x1, /**<	BEARER DESC CSD		*/
	BEARER_GPRS = 0x2, /**<	BEARER DESC GPRS		*/
	BEARER_DEFAULT_BEARER_FROM_TRANSPORT_LAYER = 0x3, /**<	BEARER DESC DEFAULT BEARER FROM TRANSPORT LAYER		*/
	BEARER_LOCAL_LINK_TECHNOLOGY_INDEPENDENT = 0x4, /**<	BEARER DESC LOCAL LINK TECHNOLOGY INDEPENDENT		*/
	BEARER_BLUETOOTH = 0x5, /**<	BEARER DESC BLUETOOTH	*/
	BEARER_IrDA = 0x6, /**<	BEARER DESC IrDA	*/
	BEARER_RS232 = 0x7, /**<	BEARER DESC RS232	*/
	BEARER_USB = 0x10, /**<	BEARER DESC USB	*/
	BEARER_RESERVED = 0xFF /**<	RESERVED	*/
};

enum bearer_param_cs_data_rate{
	BIP_DR_AUTOBAUDING = 0, /**<	CSD data rate - AUTOBAUDING		*/
	BIP_DR_300BPS_V21 = 1, /**<	CSD data rate -300BPS V21		*/
	BIP_DR_1200BPS_V22 = 2, /**<	CSD data rate - 1200BPS V22		*/
	BIP_DR_1200_75BPS_V23 = 3, /**<	CSD data rate -1200 75BPS V23			*/
	BIP_DR_2400BPS_V22 = 4, /**<	CSD data rate -2400BPS V22		*/
	BIP_DR_2400BPS_V26 = 5, /**<	CSD data rate - 2400BPS V26			*/
	BIP_DR_4800BPS_V32 = 6, /**<	CSD data rate - 4800BPS V32		*/
	BIP_DR_9600BPS_V32 = 7, /**<	CSD data rate - 9600BPS V32		*/
	BIP_DR_9600BPS_V34 = 12, /**<	CSD data rate - 9600BPS_V34		*/
	BIP_DR_14400BPS_V34 = 14, /**<	CSD data rate -14400BPS V34		*/
	BIP_DR_19200BPS_V34 = 15, /**<	CSD data rate -19200BPS V34		*/
	BIP_DR_28800BPS_V34 = 16, /**<	CSD data rate -28800BPS V34		*/
	BIP_DR_33600BPS_V34 = 17, /**<	CSD data rate -33600BPS V34		*/
	BIP_DR_1200BPS_V120 = 34, /**<	CSD data rate -1200BPS V120		*/
	BIP_DR_2400BPS_V120 = 36, /**<	CSD data rate -2400BPS V120		*/
	BIP_DR_4800BPS_V120 = 38, /**<	CSD data rate -4800BPS V120		*/
	BIP_DR_9600BPS_V120 = 39, /**<	CSD data rate -9600BPS V120		*/
	BIP_DR_14400BPS_V120 = 43, /**<	CSD data rate -14400BPS V120		*/
	BIP_DR_19200BPS_V120 = 47, /**<	CSD data rate -19200BPS V120		*/
	BIP_DR_28800BPS_V120 = 48, /**<	CSD data rate -28800BPS V120		*/
	BIP_DR_38400BPS_V120 = 49, /**<	CSD data rate -38400BPS V120		*/
	BIP_DR_48000BPS_V120 = 50, /**<	CSD data rate -48000BPS V120		*/
	BIP_DR_56000BPS_V120 = 51, /**<	CSD data rate -56000BPS V120		*/
	BIP_DR_300BPS_V110 = 65, /**<	CSD data rate - 300BPS V110		*/
	BIP_DR_1200BPS_V110 = 66, /**<	CSD data rate -1200BPS V110		*/
	BIP_DR_2400BPS_V110_OR_X31_FALG_STUFFING = 68, /**<	CSD data rate - 2400BPS V110 OR X31 FALG STUFFING		*/
	BIP_DR_4800BPS_V110_OR_X31_FALG_STUFFING = 70, /**<	CSD data rate - 4800BPS V110 OR X31 FALG STUFFING		*/
	BIP_DR_9600BPS_V110_OR_X31_FALG_STUFFING = 71, /**<	CSD data rate - 9600BPS V110 OR X31 FALG STUFFING		*/
	BIP_DR_14400BPS_V110_OR_X31_FALG_STUFFING = 75, /**<	CSD data rate - 14400BPS V110 OR X31 FALG STUFFING		*/
	BIP_DR_19200BPS_V110_OR_X31_FALG_STUFFING = 79, /**<	CSD data rate -19200BPS V110 OR X31 FALG STUFFING		*/
	BIP_DR_28800BPS_V110_OR_X31_FALG_STUFFING = 80, /**<	CSD data rate -28800BPS V110 OR X31 FALG STUFFING		*/
	BIP_DR_38400BPS_V110_OR_X31_FALG_STUFFING = 81, /**<	CSD data rate -38400BPS V110 OR X31 FALG STUFFING		*/
	BIP_DR_48000BPS_V110_OR_X31_FALG_STUFFING = 82, /**<	CSD data rate -48000BPS V110 OR X31 FALG STUFFING		*/
	BIP_DR_56000BPS_V110_OR_X31_FALG_STUFFING = 83, /**<	CSD data rate -56000BPS V110 OR X31 FALG STUFFING		*/
	BIP_DR_64000BPS = 84, /**<	CSD data rate -64000BPS		*/
	BIP_DR_56000BPS_BIT_TRANSPERENT = 115, /**<	CSD data rate -56000BPS BIT TRANSPERENT		*/
	BIP_DR_64000BPS_BIT_TRANSPERENT = 116, /**<	CSD data rate -64000BPS BIT TRANSPERENT		*/
	BIP_DR_32000BPS_PIAFS32K = 120, /**<	CSD data rate -32000BPS PIAFS32K		*/
	BIP_DR_64000BPS_PIAFS64K = 121, /**<	CSD data rate - 64000BPS PIAFS64K		*/
	BIP_DR_28800BPS_MULTIMEDIA = 130, /**<	CSD data rate -28800BPS MULTIMEDIA		*/
	BIP_DR_32000BPS_MULTIMEDIA = 131, /**<	CSD data rate -32000BPS MULTIMEDIA		*/
	BIP_DR_33600BPS_MULTIMEDIA = 132, /**<	CSD data rate - 33600BPS MULTIMEDIA		*/
	BIP_DR_56000BPS_MULTIMEDIA = 133, /**<	CSD data rate -56000BPS MULTIMEDIA		*/
	BIP_DR_64000BPS_MULTIMEDIA = 134 /**<	CSD data rate -64000BPS MULTIMEDIA		*/
};

enum bearer_param_cs_service_type{
	BIP_CSD_BS_DATA_CIRCUIT_ASYNC_UDI = 0, /**<	CSD Bearer service - DATA CIRCUIT ASYNCHRONOUS UDI		*/
	BIP_CSD_BS_DATA_CIRCUIT_SYNC = 1, /**<	CSD Bearer service - DATA CIRCUIT SYNCHRONOUS UDI		*/
	BIP_CSD_BS_PAD_ACCESS_ASYNC_UDI = 2, /**<	CSD Bearer service - PAD ACCESS ASYNCHRONOUS UDI			*/
	BIP_CSD_BS_PACKET_ACCESS_SYNC = 3, /**<	CSD Bearer service - PACKET ACCESS SYNCHRONOUS UDI		*/
	BIP_CSD_BS_DATA_CIRCUIT_ASYNC_RDI = 4, /**<	CSD Bearer service - DATA CIRCUIT ASYNCHRONOUS RDI		*/
	BIP_CSD_BS_DATA_CIRCUIT_SYNC_RDI = 5, /**<	CSD Bearer service - DATA CIRCUIT SYNCHRONOUS RDI		*/
	BIP_CSD_BS_PAD_ACCESS_ASYNC_RDI = 6, /**<	CSD Bearer service - PAD ACCESS ASYNCHRONOUS RDI		*/
	BIP_CSD_BS_PACKET_ACCESS_SYNC_RDI = 7 /**<	CSD Bearer service - PACKET ACCESS SYNCHRONOUS RDI		*/
};

enum bearer_param_cs_connection_element_type{
	BIP_CSD_CONN_ELEM_TRANSPARENT = 0, /**<	CSD connection element - TRANSPARENT		*/
	BIP_CSD_CONN_ELEM_NON_TRANSPARENT = 1, /**<	CSD connection element - NON TRANSPARENT		*/
	BIP_CSD_CONN_ELEM_BOTH_TRANSPARENT_PREF = 2, /**<	CSD connection element -BOTH TRANSPARENT PREFFERED		*/
	BIP_CSD_CONN_ELEM_BOTH_NON_TRANSPARENT_PREF = 3 /**<	CSD connection element -  NON TRANSPARENT PREFFERED		*/
};

struct tel_sat_bearer_param_cs_bearer{
	enum bearer_param_cs_data_rate data_rate;
	enum bearer_param_cs_service_type service_type;
	enum bearer_param_cs_connection_element_type connection_element_type;
};

enum bearer_param_ps_precedence_class{
	BIP_GPRS_PRECED_CLASS_HIGH_PRIORITY = 0x01, /**<	GPRS precedence class -HIGH PRIORITY		*/
	BIP_GPRS_PRECED_CLASS_NORM_PRIORITY = 0x02, /**<	GPRS precedence class -NORM PRIORITY		*/
	BIP_GPRS_PRECED_CLASS_LOW_PRIORITY = 0x03 /**<	GPRS precedence class - LOW PRIORITY		*/
};

enum bearer_param_ps_delay_class{
	BIP_GPRS_DELAY_CLASS_1 = 0x01, /**<	GPRS delay class - 1		*/
	BIP_GPRS_DELAY_CLASS_2 = 0x02, /**<	GPRS delay class -	2	*/
	BIP_GPRS_DELAY_CLASS_3 = 0x03, /**<	GPRS delay class -	3	*/
	BIP_GPRS_DELAY_CLASS_4 = 0x04 /**<	GPRS delay class - 4		*/
};

enum bearer_param_ps_reliability_class{
	BIP_GPRS_RELIABILITY_CLASS_1 = 0x01, /**<	GPRS Reliability class -1		*/
	BIP_GPRS_RELIABILITY_CLASS_2 = 0x02, /**<	GPRS Reliability class -2		*/
	BIP_GPRS_RELIABILITY_CLASS_3 = 0x03, /**<	GPRS Reliability class -3		*/
	BIP_GPRS_RELIABILITY_CLASS_4 = 0x04, /**<	GPRS Reliability class -4		*/
	BIP_GPRS_RELIABILITY_CLASS_5 = 0x05 /**<	GPRS Reliability class -5		*/
};

enum bearer_param_ps_peak_throughput_class{
	BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_8KBPS = 0x01, /**<	GPRS peak throughput class- UPTO 8KBPS		*/
	BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_16KBPS = 0x02, /**<	GPRS peak throughput class- UPTO 16KBPS		*/
	BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_32KBPS = 0x03, /**<	GPRS peak throughput class-UPTO 32KBPS		*/
	BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_64KBPS = 0x04, /**<	GPRS peak throughput class-UPTO 64KBPS		*/
	BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_128KBPS = 0x05, /**<	GPRS peak throughput class- UPTO 128KBPS			*/
	BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_256KBPS = 0x06, /**<	GPRS peak throughput class- UPTO 256KBPS			*/
	BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_512KBPS = 0x07, /**<	GPRS peak throughput class- UPTO 512KBPS		*/
	BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_1024KBPS = 0x08, /**<	GPRS peak throughput class-UPTO 1024KBPS		*/
	BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_2048KBPS = 0x09 /**<	GPRS peak throughput class- UPTO 2048KBPS		*/
};

enum bearer_param_ps_mean_throughput_class{
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_DOT_22_BPS = 0x01, /**<	GPRS mean throughput class - DOT 22 BPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_DOT_44_BPS = 0x02, /**<	GPRS mean throughput class - DOT 44 BPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_1_DOT_11_BPS = 0x03, /**<	GPRS mean throughput class -1 DOT 11 BPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_2_DOT_2_BPS = 0x04, /**<	GPRS mean throughput class -2 DOT 2 BPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_2_DOT_4_BPS = 0x05, /**<	GPRS mean throughput class -2 DOT 4 BPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_11_DOT_1_BPS = 0x06, /**<	GPRS mean throughput class - 11 DOT 1 BPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_22BPS = 0x07, /**<	GPRS mean throughput class -22BPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_44BPS = 0x08, /**<	GPRS mean throughput class - 44BPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_111BPS = 0x09, /**<	GPRS mean throughput class - 111BPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_DOT_22_KBPS = 0x0A, /**<	GPRS mean throughput class - DOT 22 KBPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_DOT_44_KBPS = 0x0B, /**<	GPRS mean throughput class -DOT 44 KBPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_1_DOT_11_KBPS = 0x0C, /**<	GPRS mean throughput class -1 DOT 11 KBPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_2_DOT_2_KBPS = 0x0D, /**<	GPRS mean throughput class -2 DOT 2 KBPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_4_DOT_4_KBPS = 0x0E, /**<	GPRS mean throughput class - 4 DOT 4 KBPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_11_DOT_1_KBPS = 0x0F, /**<	GPRS mean throughput class -11 DOT 1 KBPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_22KBPS = 0x10, /**<	GPRS mean throughput class - 22KBPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_44KBPS = 0x11, /**<	GPRS mean throughput class - 44KBPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_111KBPS = 0x12, /**<	GPRS mean throughput class -111KBPS		*/
	BIP_GPRS_MEAN_THROUGHPUT_CLASS_BEST_EFFORT = 0x13 /**<	GPRS mean throughput class - BEST EFFORT		*/
};

enum bearer_param_ps_pdp_type{
	BIP_GPRS_PDP_TYPE_IP = 0x02, /**<	bearer parameter GPRS pdp type - IP		*/
	BIP_GPRS_PDP_TYPE_RESERVED = 0xff /**<	reserved		*/
};

struct tel_sat_bearer_param_ps_bearer{
	enum bearer_param_ps_precedence_class precedence_class;
	enum bearer_param_ps_delay_class delay_class;
	enum bearer_param_ps_reliability_class reliability_class;
	enum bearer_param_ps_peak_throughput_class peak_throughput_class;
	enum bearer_param_ps_mean_throughput_class mean_throughput_class;
	enum bearer_param_ps_pdp_type pdp_type;
};

enum bearer_param_local_link_service_type{
	BIP_LL_SERVICE_IDENT_VALUE_ASSIGNED_BY_USIM = 0x00, /**<	local links service identity - value assigned by USIM		*/
	BIP_LL_SERVICE_IDENT_VALUE_ASSIGNED_BY_REMOTED_DEVICE = 0xFF /**<	local links service identity - value assigned by remote service	*/
};

struct tel_sat_bearer_param_local_link_bearer{
	enum bearer_param_local_link_service_type service_type;
	char service_record[SAT_BEARER_PARAMS_LEN_MAX];
};

struct tel_sat_bearer_description{
	enum bearer_type bearer_type;
	union{
		struct tel_sat_bearer_param_cs_bearer cs_bearer_param;
		struct tel_sat_bearer_param_ps_bearer ps_bearer_param;
		struct tel_sat_bearer_param_local_link_bearer local_link_bearer_param;
	}bearer_parameter;
};

/*
 * 8.53 Channel data
 */
struct tel_sat_channel_data{
	unsigned char data_string_len; /**<	channel data string length	*/
	char	data_string[SAT_CHANNEL_DATA_STRING_LEN_MAX];
};

/*
 * 8.54 Channel data length
 */
struct tel_sat_channel_data_len{
	unsigned char data_len; /**<	channel data length	*/
};


/*
 * 8.55 Buffer size
 */
struct tel_sat_buffer_size{
	unsigned char size[2]; /**<	channel data buffer size	*/
};

/*
 * 8.56 Channel status
 */
enum channel_status{
	link_or_packet_service_not_activated,
	link_or_packet_service_activated
};

enum channel_status_info{
	no_futher_info = 0x00,
	link_dropped = 0x05
};

struct tel_sat_channel_status{
	unsigned char channel_id;
	enum channel_status status;
	enum channel_status_info status_info;
};

/*
 * 8.58 Other Address
 */
enum address_type{
	ADDR_TYPE_IPv4 = 0x21, /**<	address type - IPv4	*/
	ADDR_TYPE_IPv6 = 0x57, /**<	address type - IPv6	*/
	ADDR_RESERVED = 0xFF /**<	reserved		*/
};

struct tel_sat_other_address{
	enum address_type address_type; /**<	channel address type	*/
	unsigned char address_len; /**<	channel address length	*/
	char address[SAT_OTHER_ADDR_LEN_MAX]; /**<	channel address	*/
};

/*
 * 8.59 UICC/terminal interface transport level
 */
enum transport_protocol_type{
	TP_TYPE_UDP_UICC_CLIENT = 0x01, /**<	transport protocol type- UDP UICC CLIENT	*/
	TP_TYPE_TCP_UICC_CLIENT = 0x02, /**<	transport protocol type-TCP UICC CLIENT	*/
	TP_TYPE_TCP_UICC_SERVER = 0x03 /**<	transport protocol type- TCP UICC SERVER	*/
};

struct tel_sat_uicc_terminal_interface_transport_level{
	enum transport_protocol_type protocol_type;
	unsigned short port_number;
};

/*
 * 8.68 Remote entity address
 */
enum remote_entity_coding_type{
	REMOTE_ENTITY_ADDR_CODING_TYPE_IEEE802_48BIT = 0, /**<	remote entity address coding type- IEEE802 48BIT		*/
	REMOTE_ENTITY_ADDR_CODING_TYPE_IRDA_32BIT = 1, /**<	remote entity address coding type- IRDA 32BIT		*/
	REMOTE_ENTITY_ADDR_CODING_TYPE_RESERVED = 0xFF /**<	reserved		*/
};

struct tel_sat_remote_entity_address{
	enum remote_entity_coding_type coding_type;
	unsigned short length;
	unsigned char remote_entity_address[SAT_REMOTE_ENTITY_ADDR_LEN_MAX];
};

/*
 * 8.70 Network access name
 */
struct tel_sat_network_access_name{
	unsigned short length;
	unsigned char network_access_name[SAT_NET_ACC_NAM_LEN_MAX];
};

/*
 * 8.72 Text attribute
 */
struct tel_sat_text_attribute{
	gboolean b_txt_attr;
	unsigned char text_formatting[4];
};

/*
 * 8.73 Item text attribute list
 */
struct tel_sat_text_attribute_list{
	unsigned int list_cnt;
	struct tel_sat_text_attribute text_attribute_list[SAT_ITEM_TEXT_ATTRIBUTES_LIST_MAX_COUNT];
};

/*
 * 6.6 Structure of proactive UICC commands
 */

/*
 * 6.6.1 DISPLAY TEXT
 */
struct tel_sat_display_text_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_text_string_object text;
	struct tel_sat_icon_identifier icon_id;
	gboolean immediate_response_requested;
	struct tel_sat_duration duration;
	struct tel_sat_text_attribute text_attribute;
};

/*
 * 6.6.2 GET INKEY
 */
struct tel_sat_get_inkey_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_text_string_object text;
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_duration duration;
	struct tel_sat_text_attribute text_attribute;
};

/*
 * 6.6.3 GET INPUT
 */
struct tel_sat_get_input_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_text_string_object text;
	struct tel_sat_response_length rsp_len;
	struct tel_sat_text_string_object default_text;
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_text_attribute text_attribute;
};

/*
 * 6.6.4 MORE TIME
 */
struct tel_sat_more_time_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
};

/*
 * 6.6.5 PLAY TONE
 */
struct tel_sat_play_tone_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_alpha_identifier alpha_id;
	struct tel_sat_tone tone;
	struct tel_sat_duration duration;
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_text_attribute text_attribute;
};

/*
 * 6.6.7 SET-UP MENU
 */
struct tel_sat_setup_menu_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_alpha_identifier alpha_id;
	unsigned char menu_item_cnt;
	struct tel_sat_item_info menu_item[SAT_MENU_ITEM_COUNT_MAX];
	struct tel_sat_item_next_action_indicatior_list next_act_ind_list;
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_icon_identifier_list icon_list;
	struct tel_sat_text_attribute text_attribute;
	struct tel_sat_text_attribute_list text_attribute_list;
};

/*
 * 6.6.8 SELECT ITEM
 */
struct tel_sat_select_item_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_alpha_identifier alpha_id;
	unsigned char menu_item_cnt;
	struct tel_sat_item_info menu_item[SAT_MENU_ITEM_COUNT_MAX];
	struct tel_sat_item_next_action_indicatior_list item_next_act_ind_list;
	struct tel_sat_item_identifier item_identifier;
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_icon_identifier_list icon_list;
	struct tel_sat_text_attribute text_attribute;
	struct tel_sat_text_attribute_list text_attribute_list;
	//frame identifier 8.80
};

/*
 * 6.6.9 SEND SMS
 */
struct tel_sat_send_sms_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_alpha_identifier alpha_id;
	struct tel_sat_address address;
	struct tel_sat_sms_tpdu sms_tpdu;
	//CDMA-SMS TPDU 8.71
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_text_attribute text_attribute;
	//frame identifier 8.80
};

/*
 * 6.6.10 SEND SS
 */
struct tel_sat_send_ss_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_alpha_identifier alpha_id;
	struct tel_sat_ss_string ss_string;
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_text_attribute text_attribute;
	//frame identifier 8.80
};

/*
 * 6.6.11 SEND USSD
 */
struct tel_sat_send_ussd_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_alpha_identifier alpha_id;
	struct tel_sat_ussd_string ussd_string;
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_text_attribute text_attribute;
	//frame identifier 8.80
};

/*
 * 6.6.12 SETUP CALL
 */
struct tel_sat_setup_call_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_address address;
	struct tel_sat_ccp ccp;
	struct tel_sat_subaddress subaddress;
	struct tel_sat_duration duration;
	struct tel_sat_alpha_identifier user_confirm_alpha_id;
	struct tel_sat_icon_identifier user_confirm_icon_id;
	struct tel_sat_alpha_identifier call_setup_alpha_id;
	struct tel_sat_icon_identifier call_setup_icon_id;
	struct tel_sat_text_attribute user_confirm_text_attribute;
	struct tel_sat_text_attribute call_setup_text_attribute;
	//frame identifier 8.80
};

/*
 * 6.6.13 REFRESH
 */
struct tel_sat_refresh_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_file_list file_list;
	char aid[16]; // 8.60 AID - length is 16 byte, Refer TS 101.220
};

/*
 * 6.6.15 PROVIDE LOCAL INFORMATION
 */
struct tel_sat_provide_local_info_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
};

/*
 * 6.6.16 SETUP EVENT LIST
 */
struct tel_sat_setup_event_list_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_event_list event_list;
};

/*
 * 6.6.22 SETUP IDLE MODE TEXT
 */
struct tel_sat_setup_idle_mode_text_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_text_string_object text;
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_text_attribute text_attribute;
};

/*
 * 6.6.24 SEND DTMF
 */
struct tel_sat_send_dtmf_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_alpha_identifier alpha_id;
	struct tel_sat_dtmf_string dtmf_string;
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_text_attribute text_attribute;
};

/*
 * 6.6.25 LANGUAGE NOTIFICATION
 */
struct tel_sat_language_notification_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sim_language_type language;
};

/*
 * 6.6.26 LAUNCH BROWSER
 */
struct tel_sat_launch_browser_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum browser_identity browser_id;
	struct tel_sat_url url;
	struct tel_sat_bearer_list bearer;
	int file_ref_count;
	struct tel_sat_provisioning_file_ref file_list[SAT_PROVISIONING_REF_MAX_COUNT];
	struct tel_sat_text_string_object gateway_proxy_text;
	struct tel_sat_alpha_identifier user_confirm_alpha_id;
	struct tel_sat_icon_identifier user_confirm_icon_id;
	struct tel_sat_text_attribute user_confirm_text_attribute;
	//frame identifier 8.80
};

/*
 * 6.6.27 OPEN CHANNEL
 */
struct tel_sat_open_channel_cs_bearer{
	struct tel_sat_address address;
	struct tel_sat_subaddress subaddress;
	struct tel_sat_duration duration1;
	struct tel_sat_duration duration2;
	struct tel_sat_other_address other_address;
	struct tel_sat_text_string_object text_user_login;
	struct tel_sat_text_string_object text_user_pwd;
};

struct tel_sat_open_channel_packet_data_service_bearer{
	struct tel_sat_network_access_name network_access_name;
	struct tel_sat_other_address other_address;
	struct tel_sat_text_string_object text_user_login;
	struct tel_sat_text_string_object text_user_pwd;
};

struct tel_sat_open_channel_local_bearer{
	struct tel_sat_duration duration1;
	struct tel_sat_duration duration2;
	struct tel_sat_text_string_object text_user_pwd;
	struct tel_sat_remote_entity_address remote_entity_address;
};

struct tel_sat_open_channel_default_bearer{
	struct tel_sat_other_address other_address;
	struct tel_sat_text_string_object text_user_login;
	struct tel_sat_text_string_object text_user_pwd;
};

struct tel_sat_open_channel_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_alpha_identifier alpha_id;
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_bearer_description bearer_desc;
	struct tel_sat_buffer_size buffer_size;
	struct tel_sat_uicc_terminal_interface_transport_level interface_transport_level;
	struct tel_sat_other_address data_destination_address;
	struct tel_sat_text_attribute text_attribute;
	//frame identifier 8.80
	union{
		struct tel_sat_open_channel_cs_bearer cs_bearer;
		struct tel_sat_open_channel_packet_data_service_bearer ps_bearer;
		struct tel_sat_open_channel_local_bearer local_bearer;
		struct tel_sat_open_channel_default_bearer default_bearer;
	}bearer_detail;
};

/*
 * 6.6.28 CLOSE CHANNEL
 */
struct tel_sat_close_channel_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_alpha_identifier alpha_id;
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_text_attribute text_attribute;
};

/*
 * 6.6.29 RECEIVE DATA
 */
struct tel_sat_receive_channel_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_alpha_identifier alpha_id;
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_channel_data_len channel_data_len;
	struct tel_sat_text_attribute text_attribute;
};

/*
 * 6.6.30 SEND DATA
 */
struct tel_sat_send_channel_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	struct tel_sat_alpha_identifier alpha_id;
	struct tel_sat_icon_identifier icon_id;
	struct tel_sat_channel_data channel_data;
	struct tel_sat_text_attribute text_attribute;
};

/*
 * 6.6.31 GET CHANNEL STATUS
 */
struct tel_sat_get_channel_status_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
};

/*
 * 6.6.XX UNSUPPORTED CMD
 */
struct tel_sat_unsupproted_command_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
};

/*
 * CoreObject
 * Structure of Req. Rsp. Noti.
 */

/*
 * Define SAT Request structure
 */
enum sat_envelop_sub_cmd{
	ENVELOP_MENU_SELECTION,
	ENVELOP_CALL_CONTROL,
	ENVELOP_TIMER_EXPIRATION,
	ENVELOP_EVENT_DOWNLOAD,
};

struct tel_sat_envelop_menu_selection_tlv{
	struct tel_sat_device_identities device_identitie;
	struct tel_sat_item_identifier item_identifier;
	gboolean help_request;
};

struct tel_sat_envelop_event_download_tlv{
	struct tel_sat_device_identities device_identitie;
	enum event_list event;
	gboolean idle_screen;
	enum tel_sim_language_type language;
	enum browser_termination_cause browser_termination;
	struct tel_sat_channel_status channel_status;
	struct tel_sat_channel_data_len channel_data_len;
};

struct treq_sat_envelop_cmd_data{
	enum sat_envelop_sub_cmd sub_cmd;
	union{
		struct tel_sat_envelop_menu_selection_tlv menu_select;
		struct tel_sat_envelop_event_download_tlv event_download;
	}envelop_data;
};

struct tel_sat_tr_display_text_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
};

struct tel_sat_tr_get_inkey_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	struct tel_sat_text_string_object text;
	struct tel_sat_duration duration;
};

struct tel_sat_tr_get_input_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	struct tel_sat_text_string_object text;
};

struct tel_sat_tr_more_time_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
};

struct tel_sat_tr_play_tone_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
};

struct tel_sat_tr_refresh_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
};

struct tel_sat_tr_setup_menu_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
};

struct tel_sat_tr_select_item_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	gboolean other_info;
	struct tel_sat_item_identifier item_identifier;
};

struct tel_sat_tr_send_sms_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	enum tel_sat_call_ctrl_problem_type cc_problem_type;
};

struct tel_sat_tr_send_ss_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	enum ss_problem_type ss_problem;
	gboolean other_info;
	enum tel_sat_call_ctrl_problem_type cc_problem_type;
	struct tel_sat_call_ctrl_req_action call_ctrl_req_act;
	enum tel_sat_result_type result_type2;
	struct tel_sat_text_string_object text;
};

struct tel_sat_tr_send_ussd_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	enum ussd_problem_type ussd_problem;
	gboolean other_info;
	gboolean call_ctrl_has_modification;
	enum tel_sat_call_ctrl_problem_type cc_problem_type;
	struct tel_sat_call_ctrl_req_action call_ctrl_req_act;
	enum tel_sat_result_type result_type2;
	struct tel_sat_text_string_object text;
	struct tel_sat_text_string_object text2;
};

struct tel_sat_tr_setup_call_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	gboolean other_info;
	enum network_problem_type network_problem_type;
	enum tel_sat_call_ctrl_problem_type cc_problem_type;
	struct tel_sat_call_ctrl_req_action call_ctrl_req_act;
	enum tel_sat_result_type result_type2;
	struct tel_sat_text_string_object text;
	gboolean tapi_cause;
	int tapi_call_level_cause;
	int tapi_ss_level_cause;
};

struct tel_sat_tr_provide_local_info_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	gboolean other_info;
	union {
		struct tel_sat_location_information location_info;
		struct tel_sat_imei imei;
		struct tel_sat_network_measurement_results nmr;
		struct tel_sat_date_time_and_timezone date_time_and_timezone;
		enum tel_sim_language_type language;
	}other;
};

struct tel_sat_tr_setup_event_list_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
};

struct tel_sat_tr_setup_idle_mode_text_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
};

struct tel_sat_tr_send_dtmf_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
};

struct tel_sat_tr_language_notification_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
};

struct tel_sat_tr_launch_browser_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	enum browser_problem_type browser_problem_type;
};

struct tel_sat_tr_open_channel_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	enum bip_problem_type bip_problem_type;
	struct tel_sat_channel_status channel_status;
	struct tel_sat_bearer_description bearer_desc;
	struct tel_sat_buffer_size buffer_size;
	//struct tel_sat_other_address other_address; (uicc server mode)
};

struct tel_sat_tr_close_channel_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	enum bip_problem_type bip_problem_type;
};

struct tel_sat_tr_send_data_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	enum bip_problem_type bip_problem_type;
	struct tel_sat_channel_data_len channel_data_len;
};

struct tel_sat_tr_receive_data_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	enum bip_problem_type bip_problem_type;
	struct tel_sat_channel_data channel_data;
	struct tel_sat_channel_data_len channel_data_len;
};

struct tel_sat_tr_get_channel_status_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
	enum bip_problem_type bip_problem_type;
	struct tel_sat_channel_status channel_status;
};

struct tel_sat_tr_unsupport_command_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
};

struct treq_sat_terminal_rsp_data{
	int cmd_number;
	enum tel_sat_proactive_cmd_type cmd_type;
	union{
		struct tel_sat_tr_display_text_tlv display_text;
		struct tel_sat_tr_get_inkey_tlv get_inkey;
		struct tel_sat_tr_get_input_tlv get_input;
		struct tel_sat_tr_more_time_tlv more_time;
		struct tel_sat_tr_play_tone_tlv play_tone;
		struct tel_sat_tr_refresh_tlv refresh;
		struct tel_sat_tr_setup_menu_tlv setup_menu;
		struct tel_sat_tr_select_item_tlv select_item;
		struct tel_sat_tr_send_sms_tlv send_sms;
		struct tel_sat_tr_send_ss_tlv send_ss;
		struct tel_sat_tr_send_ussd_tlv send_ussd;
		struct tel_sat_tr_setup_call_tlv setup_call;
		struct tel_sat_tr_provide_local_info_tlv provide_local_info;
		struct tel_sat_tr_setup_event_list_tlv setup_event_list;
		struct tel_sat_tr_setup_idle_mode_text_tlv setup_idle_mode_text;
		struct tel_sat_tr_send_dtmf_tlv send_dtmf;
		struct tel_sat_tr_language_notification_tlv language_notification;
		struct tel_sat_tr_launch_browser_tlv launch_browser;
		struct tel_sat_tr_open_channel_tlv open_channel;
		struct tel_sat_tr_close_channel_tlv close_channel;
		struct tel_sat_tr_send_data_tlv send_data;
		struct tel_sat_tr_receive_data_tlv receive_data;
		struct tel_sat_tr_get_channel_status_tlv get_channel_status;
		struct tel_sat_tr_unsupport_command_tlv unsupport_cmd;
	}terminal_rsp_data;
};

struct treq_sat_user_confirmation_data{
	int user_conf;
	int icon_conf; /* Notice: Not being used for now*/
};

/*
 * Define SAT Response structure
 */
enum envelop_rsp{
	ENVELOPE_SUCCESS,	/**<	envelope result - success	*/
	ENVELOPE_SIM_BUSY,	/**<	envelope result - USIM busy	*/
	ENVELOPE_FAILED	/**<	envelope result - failed	*/
};

struct tresp_sat_envelop_data{
	int result;
	enum sat_envelop_sub_cmd sub_cmd;
	enum envelop_rsp envelop_resp;
};

/*
 * Define SAT Notification structure
 */
enum call_control_result{
	call_control_allowed_no_mod = 0x00,
	call_control_not_allowed = 0x01,
	call_control_allowed_with_mod = 0x02
};

struct tnoti_sat_proactive_ind{
	int decode_err_code;
	int cmd_number;
	enum tel_sat_proactive_cmd_type cmd_type;
	union{
		struct tel_sat_display_text_tlv display_text;
		struct tel_sat_get_inkey_tlv get_inkey;
		struct tel_sat_get_input_tlv get_input;
		struct tel_sat_more_time_tlv more_time;
		struct tel_sat_play_tone_tlv play_tone;
		struct tel_sat_setup_menu_tlv setup_menu;
		struct tel_sat_select_item_tlv select_item;
		struct tel_sat_send_sms_tlv send_sms;
		struct tel_sat_send_ss_tlv send_ss;
		struct tel_sat_send_ussd_tlv send_ussd;
		struct tel_sat_setup_call_tlv setup_call;
		struct tel_sat_refresh_tlv refresh;
		struct tel_sat_provide_local_info_tlv provide_local_info;
		struct tel_sat_setup_event_list_tlv setup_event_list;
		struct tel_sat_setup_idle_mode_text_tlv setup_idle_mode_text;
		struct tel_sat_send_dtmf_tlv send_dtmf;
		struct tel_sat_language_notification_tlv language_notification;
		struct tel_sat_launch_browser_tlv launch_browser;
		struct tel_sat_open_channel_tlv open_channel;
		struct tel_sat_close_channel_tlv close_channel;
		struct tel_sat_receive_channel_tlv receive_data;
		struct tel_sat_send_channel_tlv send_data;
		struct tel_sat_get_channel_status_tlv get_channel_status;
		struct tel_sat_unsupproted_command_tlv unsupport_cmd;
	}proactive_ind_data;
};

struct tnoti_sat_call_control_result_ind{
	enum call_control_result cc_result;

	struct tel_sat_address address;
	struct tel_sat_ss_string ss_string;

	struct tel_sat_ccp ccp1;
	struct tel_sat_subaddress sub_address;
	struct tel_sat_alpha_identifier alpha_id;
	struct tel_sat_repeat_indicator_type bc_repeat_type;
	struct tel_sat_ccp ccp2;
};

struct tnoti_sat_mo_sm_control_result_ind{
	enum call_control_result cc_result;

	struct tel_sat_address rp_dst_address;
	struct tel_sat_address tp_dst_address;
	struct tel_sat_alpha_identifier alpha_id;
};

__END_DECLS

#endif
