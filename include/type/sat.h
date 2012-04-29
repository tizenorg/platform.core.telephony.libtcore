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

#define SAT_TEXT_STRING_LEN_MAX 500
#define SAT_MENU_ITEM_COUNT_MAX 40
#define SAT_ALPHA_ID_LEN_MAX	255	 /**<	max length of  alpha identifier		*/
#define SAT_ITEM_TEXT_LEN_MAX 45	 /**<	max length of item text 		*/
#define SAT_ITEMS_NEXT_ACTION_INDI_LIST_MAX_COUNT	50	 /**<	max count of items next action indication list	*/
#define SAT_IMG_DATA_FILE_PATH_LEN_MAX 50	 /**<   max length of image data file name (Icon, CLUT)		*/
#define SAT_ICON_LIST_MAX_COUNT	50	 /**<	max count of icon list 		*/
#define SAT_ITEM_TEXT_ATTRIBUTES_LIST_MAX_COUNT	50	 /**<	max count of item text attributes list		*/
#define SAT_ENVELOPE_CMD_LEN_MAX	256

enum tel_sat_proactive_cmd_type {
	SAT_PROATV_CMD_NONE = 0x00, /**<	command type - None 		*/
	SAT_PROATV_CMD_REFRESH = 0x01, /**<	command type - refresh 		*/
	SAT_PROATV_CMD_MORE_TIME = 0x02, /**<	command type - more time		*/
	SAT_PROATV_CMD_SETUP_EVENT_LIST = 0x05, /**<	command type - setup event list 		*/
	SAT_PROATV_CMD_SETUP_CALL = 0x10, /**<	command type - setup call		*/
	SAT_PROATV_CMD_SEND_SS = 0x11, /**<	command type - send ss		*/
	SAT_PROATV_CMD_SEND_USSD = 0x12, /**<	command type - send ussd		*/
	SAT_PROATV_CMD_SEND_SMS = 0x13, /**<	command type - send sms 		*/
	SAT_PROATV_CMD_SEND_DTMF = 0x14, /**<	command type - send dtmf 		*/
	SAT_PROATV_CMD_LAUNCH_BROWSER = 0x15, /**<	command type - launch browser 		*/
	SAT_PROATV_CMD_PLAY_TONE = 0x20, /**<	command type - play tone		*/
	SAT_PROATV_CMD_DISPLAY_TEXT = 0x21, /**<	command type - display text		*/
	SAT_PROATV_CMD_GET_INKEY = 0x22, /**<	command type - get inkey		*/
	SAT_PROATV_CMD_GET_INPUT = 0x23, /**<	command type - get input		*/
	SAT_PROATV_CMD_SELECT_ITEM = 0x24, /**<	command type - select item		*/
	SAT_PROATV_CMD_SETUP_MENU = 0x25, /**<	command type - setup menu		*/
	SAT_PROATV_CMD_PROVIDE_LOCAL_INFO = 0x26, /**<	command type - provide local info 		*/
	SAT_PROATV_CMD_SETUP_IDLE_MODE_TEXT = 0x28, /**<	command type - setup idle mode text		*/
	SAT_PROATV_CMD_LANGUAGE_NOTIFICATION = 0x35, /**<	command type - language notification		*/
	SAT_PROATV_CMD_OPEN_CHANNEL = 0x40, /**<	command type - open channel -class e		*/
	SAT_PROATV_CMD_CLOSE_CHANNEL = 0x41, /**<	command type - close channel - class e		*/
	SAT_PROATV_CMD_RECEIVE_DATA = 0x42, /**<	command type - receive data -class e 		*/
	SAT_PROATV_CMD_SEND_DATA = 0x43, /**<	command type - send data 		*/
	SAT_PROATV_CMD_GET_CHANNEL_STATUS = 0x44, /**<	command type - get channel status -class e 		*/
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
	ALPHABET_FROMAT_SMS_DEFAULT = 0x00,
	ALPHABET_FROMAT_8BIT_DATA = 0x01,
	ALPHABET_FROMAT_UCS2 = 0x02,
	ALPHABET_FROMAT_RESERVED = 0x03
};

enum msg_class{
	MSG_CLASS_NONE =0x00,
	MSG_CLASS_0 =0x01, //class 0
	MSG_CLASS_1, // class 1 Default meaning:ME-specific
	MSG_CLASS_2, // class 2 SIM specific message
	MSG_CLASS_3, // class 3 Default meaning: TE sepcific
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

/*
 * 8.2 Alpha identifier
 */
struct tel_sat_alpha_identifier{
	gboolean is_exist;
	struct data_coding_scheme dcs;
	unsigned char alpha_data_len; /**<	alpha identifier length 		*/
	char alpha_data[SAT_ALPHA_ID_LEN_MAX]; /**<	alpha identifier info 	*/
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

struct tel_sat_cmd_qualifier_setup_menu{
	enum selection_preference_type select_preference;
	gboolean help_info;
};

struct tel_sat_cmd_detail_info{
	int cmd_num; /**< specific command number	*/
	enum tel_sat_proactive_cmd_type cmd_type; /**<	proactive command type		*/
	union {
		struct tel_sat_cmd_qualifier_display_text display_text;
		struct tel_sat_cmd_qualifier_get_inkey get_inkey;
		struct tel_sat_cmd_qualifier_get_input get_input;
		struct tel_sat_cmd_qualifier_select_item select_item;
		struct tel_sat_cmd_qualifier_setup_menu setup_menu; /**<	setup menu command qualifier info	*/

	}cmd_qualifier; /**<	Union  	*/
};

/*
 * 8.7 Device identities
 */
enum device_identity{
	DEVICE_ID_KEYPAD = 0x01,
	DEVICE_ID_DISPLAY = 0x02,
	DEVICE_ID_EARPIECE = 0x03,
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
 * 8.15 Text string
 */
struct tel_sat_text_string_object{
	int is_digit_only;
	struct data_coding_scheme dcs;
	int string_length;
	char string[SAT_TEXT_STRING_LEN_MAX+1];
};

/*
 * 8.24 Items next action indicator
 */
struct tel_sat_item_next_action_indicatior_list{
	unsigned char cnt;
	unsigned char indicator_list[SAT_ITEMS_NEXT_ACTION_INDI_LIST_MAX_COUNT];
};

/*
 * 8.23 Default text
 * It is the same as Text string(8.15)
 */

/*
 * 8.31 Icon identifier
 */
enum img_coding_scheme{
	IMAGE_CODING_SCHEME_BASIC = 0x11,
	IMAGE_CODING_SCHEME_COLOUR = 0x21,
	IMAGE_CODING_SCHEME_RESERVED = 0xFF
};

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
 * 8.72 Text attribute
 */
struct tel_sat_text_attribute{
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
 * 6.6.16 SETUP EVENT LIST
 */
struct tel_sat_setup_event_list_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
//TODO
	//Event LIST
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
	struct tel_sat_item_identifier item_identifier;
	gboolean help_request;
//TODO
};

struct treq_sat_envelop_cmd_data{
	enum sat_envelop_sub_cmd sub_cmd;
	union{
		struct tel_sat_envelop_menu_selection_tlv menu_select;
		struct tel_sat_envelop_event_download_tlv event_download;
	}envelop_data;
};

enum tel_sat_result_type{
	RESULT_SUCCESS = 0x0, /**<	command performed successfully */
	RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION = 0x01, /**<	command performed with partial comprehension 	*/
	RESULT_SUCCESS_WITH_MISSING_INFO = 0x02, /**<	command performed, with missing information 	*/

	RESULT_REFRESH_PERFORMED_WITH_ADDITIONAL_EFS_READ = 0x03, /**<	REFRESH PERFORMED WITH ADDITIONAL EFS READ 		*/
	RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED = 0x04, /**<	command performed but REQUESTED ICON NOT DISPLAYED			*/
	RESULT_SUCCESS_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM = 0x05, /**<	command performed but MODIFIED BY CALL CONTROL BY SIM		*/
	RESULT_SUCCESS_LIMITED_SERVICE = 0x06, /**<	command performed with LIMITED SERVICE		*/
	RESULT_SUCCESS_WITH_MODIFICATION = 0x07, /**<	command performed with MODIFICATION		*/
	RESULT_REFRESH_PRFRMD_BUT_INDICATED_USIM_NOT_ACTIVE = 0x08, /**<	REFRESH PERFORMED BUT INDICATED USIM NOT ACTIVE 		*/

	RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER = 0x10, /**<	proactive sim application session terminated by user		*/
	RESULT_BACKWARD_MOVE_BY_USER = 0x11, /**<	backward move in the proactive sim application session request by the user 		*/
	RESULT_NO_RESPONSE_FROM_USER = 0x12, /**<	no response from user		*/

	RESULT_HELP_INFO_REQUIRED_BY_USER = 0x13, /**<	HELP INFO REQUIRED BY USER 		*/
	RESULT_USSD_OR_SS_TRANSACTION_TERMINATED_BY_USER = 0x14, /**<	USSD OR SS TRANSACTION TERMINATED BY USER	*/

	RESULT_ME_UNABLE_TO_PROCESS_COMMAND = 0x20, /**<	ME currently unable to process command 		*/
	RESULT_NETWORK_UNABLE_TO_PROCESS_COMMAND = 0x21, /**<	Network currently unable to process command 		*/
	RESULT_USER_DID_NOT_ACCEPT_CALL_SETUP_REQ = 0x22, /**<	User did not accept call setup request 		*/
	RESULT_USER_CLEAR_DOWN_CALL_BEFORE_CONN = 0x23, /**<	User cleared down call before connection or network released		*/

	RESULT_INTERACTION_WITH_CC_BY_SIM_IN_TMP_PRBLM = 0x25, /**<	INTERACTION WITH CALL CONTROL BY SIM IN TEMPORARY PROBLEM 		*/
	RESULT_LAUNCH_BROWSER_GENERIC_ERROR_CODE = 0x26, /**<	LAUNCH BROWSER GENERIC ERROR CODE 		*/

	RESULT_BEYOND_ME_CAPABILITIES = 0x30, /**<	command beyond ME's capabilities 		*/
	RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME = 0x31, /**<	command type not understood by ME 		*/
	RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME = 0x32, /**<	command data not understood by ME		*/
	RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME = 0x33, /**<	command number not known by ME 		*/
	RESULT_SS_RETURN_ERROR = 0x34, /**<	SS return error		*/
	RESULT_SMS_RP_ERROR = 0x35, /**<	SMS rp-error		*/
	RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING = 0x36, /**<	Error, required values are missing 		*/

	RESULT_USSD_RETURN_ERROR = 0x37, /**<	USSD_RETURN_ERROR 		*/
	RESULT_INTRCTN_WITH_CC_OR_SMS_CTRL_PRMNT_PRBLM = 0x39, /**<	INTERACTION WITH CALL CONTROL OR SMS CONTROL PERMANENT PROBLEM 		*/
	RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR = 0x3A, /**<	BEARER INDEPENDENT PROTOCOL ERROR 		*/
	RESULT_FRAMES_ERROR = 0x3C /**<	FRAMES ERROR 		*/
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
	ME_PROBLEM_NO_USIM_ACTIVE = 0x0A, /**<	ME problem with NO USIM ACTIVE 		*/
	ME_PROBLEM_INVALID = 0xFF /**<	ME problem with INVALID 		*/
};

struct tel_sat_tr_setup_event_list_tlv{
	struct tel_sat_cmd_detail_info command_detail;
	struct tel_sat_device_identities device_id;
	enum tel_sat_result_type result_type;
	enum tel_sat_me_problem_type me_problem_type;
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

struct treq_sat_terminal_rsp_data{
	int cmd_number;
	enum tel_sat_proactive_cmd_type cmd_type;
	union{
		struct tel_sat_tr_setup_event_list_tlv setup_event_list;
		struct tel_sat_tr_display_text_tlv display_text;
		struct tel_sat_tr_get_inkey_tlv get_inkey;
		struct tel_sat_tr_get_input_tlv get_input;
		struct tel_sat_tr_setup_menu_tlv setup_menu;
		struct tel_sat_tr_select_item_tlv select_item;
	}terminal_rsp_data;
};


/*
 * Define SAT Response structure
 */


/*
 * Define SAT Notification structure
 */
struct tnoti_sat_proactive_ind{
	int cmd_number;
	enum tel_sat_proactive_cmd_type cmd_type;
	union{
		struct tel_sat_setup_event_list_tlv setup_event_list;
		struct tel_sat_setup_menu_tlv setup_menu;
		struct tel_sat_display_text_tlv display_text;
		struct tel_sat_get_inkey_tlv get_inkey;
		struct tel_sat_get_input_tlv get_input;
		struct tel_sat_select_item_tlv select_item;
	}proactive_ind_data;
};
__END_DECLS

#endif
