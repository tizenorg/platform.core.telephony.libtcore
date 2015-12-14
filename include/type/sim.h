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

#ifndef __TYPE_SIM_H__
#define __TYPE_SIM_H__

__BEGIN_DECLS

#include <glib.h>

#define SIM_ICCID_LEN_MAX 20 // 3gpp
#define SIM_LANG_CNT_MAX 30 //telephony defined
#define SIM_SST_SERVICE_CNT_MAX 56 //3gpp
#define SIM_UST_SERVICE_CNT_MAX 64
#define SIM_CDMA_ST_SERVICE_LEN_MAX 12
#define SIM_CDMA_ST_SERVICE_CNT_MAX 47
#define SIM_CSIM_ST_SERVICE_LEN_MAX 6
#define SIM_CSIM_ST_SERVICE_CNT_MAX 41
#define SIM_IST_BYTE_LEN_MAX 2 //telephony defined
#define SIM_IST_SERVICE_CNT_MAX 11 //telephony defined
#define SIM_SPN_LEN_MAX 16 //3gpp
#define SIM_CDMA_SPN_LEN_MAX 32 //3gpp2
#define SIM_SPDI_PLMN_MAX 41 //telephony defined
#define SIM_XDN_ALPHA_ID_LEN_MAX 30 //telephony defined
#define SIM_XDN_NUMBER_LEN_MAX 20 //telephony defined
#define SIM_MSISDN_NUMBER_LEN_MAX 26 //telephony defined
#define SIM_ECC_BYTE_LEN_MAX 3 //3gpp
#define SIM_ECC_STRING_LEN_MAX 30 //telephony defined
#define SIM_ECC_RECORD_CNT_MAX 15 //telephony defined
#define SIM_EXT_RECORD_CNT_MAX 15 //telephony defined
#define SIM_CF_RECORD_CNT_MAX 2 //telephony defined
#define SIM_MSISDN_RECORD_CNT_MAX 10 //telephony defined
#define SIM_GROUP_IDENTIFIER_LEN_MAX 10 //telephony defined
#define SIM_MAIL_BOX_IDENTIFIER_LEN_MAX 5 //3gpp
#define SIM_MSP_CNT_MAX 2 //telephony defined
#define SIM_NW_FULL_NAME_LEN_MAX 40 //telephony defined TODO should be defined one value in all.
#define SIM_OPLMNWACT_LEN_MAX 100 //telephony defined
#define SIM_AUTH_REQ_DATA_LEN_MAX 256 //telephony defined
#define SIM_AUTH_RESP_DATA_LEN_MAX 128 //telephony defined
#define SIM_APDU_DATA_LEN_MAX 2048 //telephony defined. (2048 is for QMI definition. (1024 : general length, 1024 : extra length for additional response)).
#define SIM_CPHS_CALL_FORWARDING_LEN_MIN 2 //telephony defined
#define SIM_CPHS_CSP_ENTRY_CNT_MAX 11 //telephony defined
#define SIM_CPHS_CSP_LEN_MAX 22 //telephony defined
#define SIM_CPHS_VMWF_LEN_MAX 2 //telephony defined
#define SIM_CPHS_OPERATOR_NAME_LEN_MAX 25 //telephony defined
#define SIM_CPHS_OPERATOR_NAME_SHORT_FORM_LEN_MAX 10 //CPHS spec
#define SIM_CPHS_INFO_LEN_MAX 3 //telephony defined
#define SIM_GID_LEN_MAX 16 //telephony defined
#define SIM_IMPU_CNT_MAX 15 //telephony defined
#define SIM_PCSCF_CNT_MAX 15 //telephony defined
#define SIM_FILE_ID_LIST_MAX_COUNT 255 //followed SAT_FILE_ID_LIST_MAX_COUNT in sat.h

#define SIM_APP_TYPE_SIM 0x01
#define SIM_APP_TYPE_USIM 0x02
#define SIM_APP_TYPE_CSIM 0x04
#define SIM_APP_TYPE_ISIM 0x08

enum tel_sim_file_id {
	SIM_EF_DIR = 0x2F00,					/**< Root Directory for the USIM */
	SIM_EF_ICCID = 0x2FE2,					/**< the ICC Identification file	*/
	SIM_EF_IMSI = 0x6F07,					/**< the IMSI file */
	SIM_EF_SST = 0x6F38,					/**< the SIM Service Table file. EF UST has same address */
	SIM_EF_EST = 0x6F56,					/**< the Enabled Service Table file   */
	SIM_EF_OPLMN_ACT = 0x6F61,				/**< the OPLMN List file*/
	SIM_EF_GID1 = 0x6F3E,					/**< the Group Identifier Level 1 */
	SIM_EF_GID2 = 0x6F3F,					/**< the Group Identifier Level 2 */

	SIM_EF_ELP = 0x2F05,					/**< the Extended Language Preference file */
	SIM_EF_LP = 0x6F05,					/**< SIM: Language preference */
	SIM_EF_ECC = 0x6FB7,					/**< the Emergency Call Codes     */
	SIM_EF_SPN = 0x6F46,					/**< the Service Provider Name    */
	SIM_EF_SPDI = 0x6FCD,					/**< the Service provider display information*/
	SIM_EF_PNN = 0x6FC5,					/**< the PLMN Network Name File*/
	SIM_EF_OPL = 0x6FC6,					/**< the Operator PLMN List File*/
	SIM_EF_MBDN = 0x6FC7,					/** < SIM Mail Box Dialing Number file */
	SIM_EF_MSISDN = 0x6F40,					/**< MSISDN */
	SIM_EF_EXT1 = 0x6F4A,					/**< Extension File-1 */
	SIM_EF_EXT5 = 0x6F4E,					/**< Extension File-5 */
	SIM_EF_EXT6 = 0x6FC8,					/**< Extension File-6 */
	SIM_EF_IMG = 0x4F20,					/**Image file*/

	SIM_EF_USIM_LI = 0x6A05,				/**< USIM: Language Indication */
	SIM_EF_USIM_PL = 0x2A05,				/**< the Language Preference file of USIM */
	SIM_EF_USIM_MBI = 0x6FC9,				/** < Mailbox Identifier -linear fixed*/
	SIM_EF_USIM_MWIS = 0x6FCA,				/** < Message Waiting Indication Status -linear fixed*/
	SIM_EF_USIM_CFIS = 0x6FCB,				/** < Call forward indication status -linear fixed*/

	SIM_EF_ISIM_IMPI = 0x6F02, /**< IMS private user identity */
	SIM_EF_ISIM_DOMAIN = 0x6F03, /**< Home Network Domain Name */
	SIM_EF_ISIM_IMPU = 0x6F04, /**< IMS public user identity */
	SIM_EF_ISIM_IST = 0x6FFF, /**< IMS Service Table ,changing the value to 6FFF as it clashes with IMSI*/
	SIM_EF_ISIM_PCSCF = 0x6F09, /**< P-CSCF Address */

	/* CPHS FILE ID */
	SIM_EF_CPHS_VOICE_MSG_WAITING = 0x6F11,			/** < CPHS voice MSG waiting indication  */
	SIM_EF_CPHS_SERVICE_STRING_TABLE = 0x6F12,		/** < CPHS service string table  */
	SIM_EF_CPHS_CALL_FORWARD_FLAGS = 0x6F13,		/** < CPHS call forward flags  */
	SIM_EF_CPHS_OPERATOR_NAME_STRING = 0x6F14,		/** < CPHS operator name string  */
	SIM_EF_CPHS_CUSTOMER_SERVICE_PROFILE = 0x6F15,		/** < CPHS customer service profile  */
	SIM_EF_CPHS_CPHS_INFO = 0x6F16,				/** < CPHS information  */
	SIM_EF_CPHS_MAILBOX_NUMBERS = 0x6F17,			/** < CPHS mail box numbers  */
	SIM_EF_CPHS_OPERATOR_NAME_SHORT_FORM_STRING = 0x6F18,	/** < CPHS operator name short form string  */
	SIM_EF_CPHS_INFORMATION_NUMBERS = 0x6F19,		/** < CPHS information numbers  */
	/*  CPHS ALS FILE ID */
	SIM_EF_CPHS_DYNAMICFLAGS = 0x6F9F,			/** < CPHS Dynamics flags  */
	SIM_EF_CPHS_DYNAMIC2FLAG = 0x6F92,			/** < CPHS Dynamics2 flags  */
	SIM_EF_CPHS_CUSTOMER_SERVICE_PROFILE_LINE2 = 0x6F98,	/** < CPHS CSP2  */
	SIM_EF_PBR = 0x4F30,

	/* CDMA FILE ID*/
	SIM_EF_CDMA_IMSI = 0x6F22,				/** < CDMA IMSI_M  */
	SIM_EF_CST = 0x6F32,					/** < CDMA Service table */
	SIM_EF_CDMA_SPN = 0x6F41,				/** < CDMA Service Provider Name  */
	SIM_EF_MDN = 0x6F44,					/** < CDMA Mobile Directory Number  */
	SIM_EF_CDMA_ECC = 0x6F47,				/** < CDMA Emergency Call Codes  */
	SIM_EF_CDMA_PL = 0x6F3A,				/** < CDMA Preferred Languages  */

	/* Invalid File ID, All the file ID are less than this Value*/
	SIM_EF_INVALID = 0xFFFF,				/**< Invalid file.*/
	SIM_EF_OTHERS,						/**< Element to indicate an unknown file.*/
};

enum tel_sim_type {
	SIM_TYPE_UNKNOWN,	/**< Unknown card */
	SIM_TYPE_GSM,		/**< 2G GSM card*/
	SIM_TYPE_USIM,		/**< 3G USIM card */
	SIM_TYPE_RUIM, 		/** CDMA based card*/
	SIM_TYPE_NVSIM,		/**< CDMA NV SIM */
	SIM_TYPE_ISIM		/**< Deprecated, do not use */
};

enum tel_sim_status {
	SIM_STATUS_CARD_ERROR = 0x00,			/**< Bad card / On the fly SIM gone bad **/
	SIM_STATUS_CARD_NOT_PRESENT = 0x01,		/**<  Card not present **/
	SIM_STATUS_INITIALIZING = 0x02,			/**<  SIM is Initializing state **/
	SIM_STATUS_INIT_COMPLETED = 0x03,		/**<  SIM Initialization OK **/
	SIM_STATUS_PIN_REQUIRED = 0x04,			/**<  PIN  required state **/
	SIM_STATUS_PUK_REQUIRED = 0x05,			/**<  PUK required state **/
	SIM_STATUS_CARD_BLOCKED = 0x06,			/**<  PIN/PUK blocked(permanently blocked- All the attempts for PIN/PUK failed) **/
	SIM_STATUS_NCK_REQUIRED = 0x07,			/**<  Network Control Key required state **/
	SIM_STATUS_NSCK_REQUIRED = 0x08,		/**<  Network Subset Control Key required state **/
	SIM_STATUS_SPCK_REQUIRED = 0x09,		/**<  Service Provider Control Key required state **/
	SIM_STATUS_CCK_REQUIRED = 0x0a,			/**<  Corporate Control Key required state **/
	SIM_STATUS_CARD_REMOVED = 0x0b,			/**<  Card removed **/
	SIM_STATUS_LOCK_REQUIRED = 0x0c,		/**<  PH-SIM (phone-SIM) locked state **/
	SIM_STATUS_CARD_CRASHED = 0x0d,			/**< Runtime SIM card error **/
	SIM_STATUS_CARD_POWEROFF = 0x0e,        /**< SIM Powered OFF **/
	SIM_STATUS_UNKNOWN = 0xff			    /**<  unknown state. not checked yet. **/

};

enum tel_sim_facility_status {
	SIM_FACILITY_STATUS_DISABLED = 0x00,	/**< PIN1 or PIN2 Disabled */
	SIM_FACILITY_STATUS_ENABLED = 0x01,	/**< PIN1 or PIN2 Enabled */
	SIM_FACILITY_STATUS_BLOCKED = 0x02,	/**< SIM is present, but PIN1 or PIN2  is blocked. need unblocking by PUK or PUK2 */
	SIM_FACILITY_STATUS_PUK_BLOCKED = 0x03,	/**< SIM is present, but PUK is blocked. */
	SIM_FACILITY_STATUS_UNKNOWN = 0xFF	/**< SIM is in unknown state */
};

enum tel_sim_access_result {
	SIM_ACCESS_SUCCESS,			/**< Access to file successful.  */
	SIM_ACCESS_CARD_ERROR,			/**< SIM card error    */
	SIM_ACCESS_FILE_NOT_FOUND,		/**< File not found  */
	SIM_ACCESS_CONDITION_NOT_SATISFIED,	/**< Access condition is not fulfilled  */
	SIM_ACCESS_FAILED,			/**< Access failed.  */
};

enum tel_sim_pin_operation_result {
	SIM_PIN_OPERATION_SUCCESS,	/**< Operation involving PIN (verification/change/enable/disable, etc) is successful.  */
	SIM_BUSY,			/**< SIM is busy  */
	SIM_CARD_ERROR,			/**< SIM card error - General errors   */
	SIM_INCOMPATIBLE_PIN_OPERATION,	/**< SIM Incompatible pin operation that is in case when invalid SIM command is given or incorrect parameters are supplied to the SIM. */
	SIM_INCORRECT_PASSWORD,		/**< SIM PIN  Incorrect password */
	SIM_PIN_REQUIRED,		/**< PIN Required */
	SIM_PUK_REQUIRED,		/**< PUK Required */
	SIM_NCK_REQUIRED,		/**< Network Control Key Required */
	SIM_NSCK_REQUIRED,		/**< Network Subset Control Key Required */
	SIM_SPCK_REQUIRED,		/**< Service Provider Control Key Required */
	SIM_CCK_REQUIRED,		/**< Corporate Control Key Required */
	SIM_LOCK_REQUIRED,		/**<  PH-SIM (phone-SIM) locked state **/
	SIM_ADM_REQUIRED,		/**<  Admin key locked state **/
	SIM_PERM_BLOCKED		/**< Permanently Blocked **/
};

enum tel_sim_language_type {
	SIM_LANG_GERMAN = 0x00,		/**< German */
	SIM_LANG_ENGLISH = 0x01,	/**< English */
	SIM_LANG_ITALIAN = 0x02,	/**< Italian */
	SIM_LANG_FRENCH = 0x03,		/**< French */
	SIM_LANG_SPANISH = 0x04,	/**< Spanish */
	SIM_LANG_DUTCH = 0x05,		/**< Dutch */
	SIM_LANG_SWEDISH = 0x06,	/**< Swedish */
	SIM_LANG_DANISH = 0x07,		/**< Danish */
	SIM_LANG_PORTUGUESE = 0x08, 	/**< Portuguese */
	SIM_LANG_FINNISH = 0x09,	/**< Finnish */
	SIM_LANG_NORWEGIAN = 0x0A,	/**< Norwegian */
	SIM_LANG_GREEK = 0x0B,		/**< Greek */
	SIM_LANG_TURKISH = 0x0C,	/**< Turkish */
	SIM_LANG_HUNGARIAN = 0x0D,	/**< Hungarian */
	SIM_LANG_POLISH = 0x0E,		/**< Polish */
	SIM_LANG_KOREAN = 0x0F,		/**< Korean */
	SIM_LANG_CHINESE = 0x10,	/**< Chinese */
	SIM_LANG_RUSSIAN = 0x11,	/**< Russian */
	SIM_LANG_JAPANESE = 0x12,	/**< Japanese */
	SIM_LANG_UNSPECIFIED = 0xFF	/**< Unspecified */
};

enum tel_sim_msgwaiting_idication_mask {
	SIM_MWIS_NONE = 0x00,	/**< MWIS none*/
	SIM_MWIS_VOICE = 0x01,	/**< MWIS voice*/
	SIM_MWIS_FAX = 0x02,	/**< MWIS FAX*/
	SIM_MWIS_EMAIL = 0x04,	/**< MWIS email*/
	SIM_MWIS_OTHER = 0x08,	/**< MWIS other*/
	SIM_MWIS_VIDEO = 0x10,	/**< MWIS video*/
	SIM_MWIS_RFU = 0xff	/**< MWIS RFU*/
};

enum tel_sim_pin_type {
	SIM_PTYPE_PIN1 = 0x00,	/**< PIN 1 code */
	SIM_PTYPE_PIN2 = 0x01,	/**< PIN 2 code */
	SIM_PTYPE_PUK1 = 0x02,	/**< PUK 1 code */
	SIM_PTYPE_PUK2 = 0x03,	/**< PUK 2 code */
	SIM_PTYPE_UPIN = 0x04,	/**< Universal PIN - Unused now */
	SIM_PTYPE_ADM = 0x05,	/**< Administrator - Unused now */
	SIM_PTYPE_SIM = 0x06	/**< SIM Lock code */
};

enum tel_sim_facility_type {
	SIM_FACILITY_PS, /**< PH-SIM, Lock PHone to SIM/UICC card*/
	SIM_FACILITY_SC, /**< Lock SIM/UICC card, simply PIN1 */
	SIM_FACILITY_FD, /**< Fixed Dialing Number feature, need PIN2 */
	SIM_FACILITY_PN, /**< Network Personalization */
	SIM_FACILITY_PU, /**< network sUbset Personalization */
	SIM_FACILITY_PP, /**< service Provider Personalization */
	SIM_FACILITY_PC, /**< Corporate Personalization */
};

enum tel_sim_lock_status {
	SIM_LOCK_STATUS_UNLOCKED,	/**< 0x00 : Not necessary */
	SIM_LOCK_STATUS_PIN,		/**< 0x01 : PIN required as a password */
	SIM_LOCK_STATUS_PUK,		/**< 0x02 : PUK required as a password */
	SIM_LOCK_STATUS_PIN2,		/**< 0x03 : PIN2 required as a password */
	SIM_LOCK_STATUS_PUK2,		/**< 0x04 : PUK2 required as a password */
	SIM_LOCK_STATUS_PERM_BLOCKED,	/**< 0x05 : PIN Permanent Blocked */
};

enum tel_sim_cphs_dynamic_flag_selected_line {
	SIM_DYNAMIC_FLAGS_LINE1 = 0x01,	/**< line 1 */
	SIM_DYNAMIC_FLAGS_LINE2 = 0x00,	/**< line 2*/
	SIM_DYNAMIC_FLAGS_RFU = 0xff	/**< rfu*/
};

enum tel_sim_cphs_dynamic2_flag_als_status {
	SIM_PIN2_ACCESSIBLE_FLAGS_LOCKED = 0x01,	/**< Dynamic flags locked */
	SIM_PIN2_ACCESSIBLE_FLAGS_UNLOCKED = 0x00,	/**< Dynamic flags unlocked */
	SIM_PIN2_ACCESSIBLE_FLAGS_RFU = 0xff		/**< rfu */
};

enum tel_sim_cphs_phase {
	SIM_CPHS_PHASE1 = 0x01,		/**< phase1  */
	SIM_CPHS_PHASE2 = 0x02,		/**< phase2  */
	SIM_CPHS_PHASE_RFU = 0xff	/**< RFU  */
};

enum tel_sim_ton {
	SIM_TON_UNKNOWN = 0,		/**< unknown */
	SIM_TON_INTERNATIONAL = 1,	/**< international number */
	SIM_TON_NATIONAL = 2,		/**< national number */
	SIM_TON_NETWORK_SPECIFIC = 3,	/**< network specific number */
	SIM_TON_DEDICATED_ACCESS = 4,	/**< subscriber number */
	SIM_TON_ALPHA_NUMERIC = 5,	/**< alphanumeric, GSM 7-bit default alphabet) */
	SIM_TON_ABBREVIATED_NUMBER = 6,	/**< abbreviated number */
	SIM_TON_RESERVED_FOR_EXT = 7	/**< reserved for extension */
};

enum tel_sim_npi {
	SIM_NPI_UNKNOWN = 0,			/**< Unknown */
	SIM_NPI_ISDN_TEL = 1,			/**< ISDN/Telephone numbering plan */
	SIM_NPI_DATA_NUMBERING_PLAN = 3,	/**< Data numbering plan */
	SIM_NPI_TELEX = 4,			/**< Telex numbering plan */
	SIM_NPI_SVC_CNTR_SPECIFIC_PLAN = 5,	/**< Service Center Specific plan */
	SIM_NPI_SVC_CNTR_SPECIFIC_PLAN2 = 6,	/**< Service Center Specific plan */
	SIM_NPI_NATIONAL = 8,			/**< National numbering plan */
	SIM_NPI_PRIVATE = 9,			/**< Private numbering plan */
	SIM_NPI_ERMES_NUMBERING_PLAN = 10,	/**< ERMES numbering plan */
	SIM_NPI_RESERVED_FOR_EXT = 0xF		/**< Reserved for extension */
};

enum tel_sim_emergency_service_type {
	SIM_ECC_POLICE = 0x01,		/**< Police */
	SIM_ECC_AMBULANCE = 0x02,	/**< Ambulance */
	SIM_ECC_FIREBRIGADE = 0x04, 	/**< Fire brigade */
	SIM_ECC_MARAINEGUARD = 0x08,	/**< Marine guard */
	SIM_ECC_MOUTAINRESCUE = 0x10,	/**< Mountain rescue */
	SIM_ECC_SPARE = 0x00		/**< Spare */
};

enum tel_sim_mailbox_type {
	SIM_MAILBOX_VOICE = 0x01,	/**< Voicemail*/
	SIM_MAILBOX_FAX = 0x02,		/**< Fax*/
	SIM_MAILBOX_EMAIL = 0x03,	/**< Email*/
	SIM_MAILBOX_OTHER = 0x04,	/**< Other*/
	SIM_MAILBOX_VIDEO = 0x05,	/**< Videomail*/
	SIM_MAILBOX_DATA = 0x06,	/**< Data*/
};

enum tel_sim_cphs_csp_group {
	SIM_CPHS_CSP_GROUP_CALL_OFFERING = 0x01,	/**< Group CSP offering*/
	SIM_CPHS_CSP_GROUP_CALL_RESTRICTION = 0x02,	/**< Group CSP restriction*/
	SIM_CPHS_CSP_GROUP_OTHER_SUPP_SERVICES = 0x03,	/**< Group CSP supplementary services*/
	SIM_CPHS_CSP_GROUP_CALL_COMPLETION = 0x04,	/**< Group CSP completion*/
	SIM_CPHS_CSP_GROUP_TELESERVICES = 0x05,		/**< Group CSP teleservices*/
	SIM_CPHS_CSP_GROUP_CPHS_TELESERVICES = 0x06,	/**< Group CSP CPHS teleservies*/
	SIM_CPHS_CSP_GROUP_CPHS_FEATURES = 0x07,	/**< Group CSP CPHS features*/
	SIM_CPHS_CSP_GROUP_NUMBER_IDENTIFIERS = 0x08,	/**< Group CSP number identifiers*/
	SIM_CPHS_CSP_GROUP_PHASE_SERVICES = 0x09,	/**< Group CSP phase services*/
	SIM_CPHS_CSP_GROUP_VALUE_ADDED_SERVICES = 0xC0, /**< Group CSP value added services*/
	SIM_CPHS_CSP_GROUP_INFORMATION_NUMBERS = 0xD5	/**< Group CSP information numbers*/
};

enum tel_sim_cphs_index_level {
	SIM_CPHS_INDEX_LEVEL_ONE = 0x01,	/**< SIM CPHS index level one */
	SIM_CPHS_INDEX_LEVEL_TWO = 0x02,	/**< SIM CPHS index level two */
	SIM_CPHS_INDEX_LEVEL_THREE = 0x03,	/**< SIM CPHS index level three */
	SIM_CPHS_INDEX_LEVEL_RFU = 0xff		/**< SIM CPHS index level RFU */
};

enum tel_sim_auth_type {
	SIM_AUTH_TYPE_IMS = 0x00,	/**< IMS Authentication */
	SIM_AUTH_TYPE_GSM,		/**< GSM Authentication */
	SIM_AUTH_TYPE_3G,		/**< 3G Authentication */
	SIM_AUTH_RUIM_CAVE,		/**< CDMA CAVE Authentication */
	SIM_AUTH_RUIM_CHAP,		/**< CDMA CHAP Authentication */
	SIM_AUTH_RUIM_MNHA,		/**< CDMA MNHA Authentication */
	SIM_AUTH_RUIM_MIPRRQ,		/**< CDMA MIPRRQ Authentication */
	SIM_AUTH_RUIM_MNAAA,		/**< CDMA MNAAA Authentication */
	SIM_AUTH_RUIM_HRPD,		/**< CDMA HRPD Authentication */
	SIM_AUTH_TYPE_MAX		/**< TBD */
};

enum tel_sim_auth_result {
	SIM_AUTH_NO_ERROR = 0x00,		/**< ISIM no error */
	SIM_AUTH_CANNOT_PERFORM,		/**< status - can't perform authentication */
	SIM_AUTH_SKIP_RESPONSE,			/**< status - skip authentication response */
	SIM_AUTH_MAK_CODE_FAILURE,		/**< status - MAK(Multiple Activation Key) code failure */
	SIM_AUTH_SQN_FAILURE,			/**< status - SQN(SeQuenceNumber) failure */
	SIM_AUTH_SYNCH_FAILURE,			/**< status - synch failure */
	SIM_AUTH_UNSUPPORTED_CONTEXT,		/**< status - unsupported context */
	SIM_AUTH_MAX				/**< TBD */
};

enum tel_sim_power_set_result {
	SIM_POWER_SET_SUCCESS = 0x00, /**< Change Power State Success */
	SIM_POWER_SET_FAILURE         /**< Change Power State Failure*/
};


enum tel_sim_sst_service {
	// service 1 ~ 8
	SIM_SST_CHV1_DISABLE_FUNC = 0,				/**< CHV1 disable function */
	SIM_SST_ADN,						/**< abbreviated Dialing number */
	SIM_SST_FDN,						/**< fixed Dialing number */
	SIM_SST_SMS,						/**< short message storage */
	SIM_SST_AOC,						/**< advice of charge */
	SIM_SST_CCP,						/**< capability configuration parameters */
	SIM_SST_PLMN_SELECTOR,					/**< plmn selector */
	SIM_SST_RFU1,						/**< rfu */

	// service 9 ~ 16
	SIM_SST_MSISDN = 8,					/**< msisdn */
	SIM_SST_EXT1,						/**< extension1	*/
	SIM_SST_EXT2,						/**< extension2 */
	SIM_SST_SMS_PARAMS,					/**< sms parameteres */
	SIM_SST_LND,						/**< last number dialed */
	SIM_SST_CELL_BROADCAST_MSG_ID,				/**< cell broadcast message identifier */
	SIM_SST_GID_LV1,					/**< group identifier level 1 */
	SIM_SST_GID_LV2,					/**< group identifier level 2 */

	// service 17 ~ 24
	SIM_SST_SPN = 16,					/**< service provider name */
	SIM_SST_SDN,						/**< service Dialing number */
	SIM_SST_EXT3,						/**< extension3 */
	SIM_SST_RFU2,						/**< rfu */
	SIM_SST_VGCS_GID_LIST,					/**< vgcs group identifier (EF-VGCS, EF-VGCSS) */
	SIM_SST_VBS_GID_LIST,					/**< vbs group identifier (EF-VBS, EF-VBSS) */
	SIM_SST_ENHANCED_MULTI_LV_PRECEDENCE_PREEMPTION_SRVC,	/**< enhanced multi-level precedence and pre-emption service */
	SIM_SST_AUTO_ANSWER_FOR_EMLPP,				/**< automatic answer fro eMLPP */

	// service 25 ~ 32,
	SIM_SST_DATA_DOWNLOAD_VIA_SMS_CB = 24,			/**< data download via sms-cb */
	SIM_SST_DATA_DOWNLOAD_VIA_SMS_PP,			/**< data download via sms-pp */
	SIM_SST_MENU_SELECTION,					/**< menu selection */
	SIM_SST_CALL_CTRL,					/**< call control */
	SIM_SST_PROACTIVE_SIM,					/**< proactive sim command */
	SIM_SST_CELL_BROADCAST_MSG_ID_RANGES,			/**< cell broadcast message identifier ranges */
	SIM_SST_BDN,						/**< barred Dialing numbers */
	SIM_SST_EXT4,						/**< extension 4 */

	// service 33 ~ 40
	SIM_SST_DEPERSONALIZATION_CTRL_KEYS = 32,		/**< de-personalization control keys */
	SIM_SST_COOPERATIVE_NETWORK_LIST,			/**< co-operative network list */
	SIM_SST_SMS_STATUS_REPORTS,				/**< short message status reports */
	SIM_SST_NIA,						/**< network's indication of alerting in the MS (NIA) */
	SIM_SST_MO_SMS_CTRL_BY_SIM,				/**< mobile-originated short message control by sim */
	SIM_SST_GPRS,						/**< gprs */
	SIM_SST_IMG,						/**< image */
	SIM_SST_SOLSA,						/**< support of local service area */

	// service 41 ~ 48
	SIM_SST_USSD_STR_DATA_OBJECT_SUPPORTED_IN_CALL_CTRL = 40,/**< ussd string data object supported in call control */
	SIM_SST_RUN_AT_CMD_CMD,					/**< RUN AT COMMAND command */
	SIM_SST_USER_CTRLED_PLMN_SELECTOR_WACT,			/**< user controlled PLMN selector with Access technology */
	SIM_SST_OPERATOR_CTRLED_PLMN_SELECTOR_WACT,		/**< operator controlled PLMN selector with Access technology */
	SIM_SST_HPLMN_SELECTOR_WACT,				/**< HPLMN selector with access technology */
	SIM_SST_CPBCCH_INFO,					/**< CPBCCH information */
	SIM_SST_INVESTIGATION_SCAN,				/**< investigation scan */
	SIM_SST_EXTENDED_CAPA_CONF_PARAMS,			/**< extended capability configuration parameters */

	//	service 49 ~ 56
	SIM_SST_MEXE = 48,					/**< MExE */
	SIM_SST_RPLMN_LAST_USED_ACCESS_TECH,			/**< RPLMN last used access technology */
	SIM_SST_PLMN_NETWORK_NAME,				/**< PLMN Network Name*/
	SIM_SST_OPERATOR_PLMN_LIST,				/**< Operator PLMN List*/
	SIM_SST_MBDN,						/**< Mailbox Dialling Numbers*/
	SIM_SST_MWIS,						/**< Message Waiting Indication Status*/
	SIM_SST_CFIS,						/**< Call Forwarding Indication Status*/
	SIM_SST_SPDI,						/**< Service Provider Display Information*/
};

enum tel_sim_ust_service {
	// service 1 ~ 8
	SIM_UST_LOCAL_PB = 0,					/**< local phone book */
	SIM_UST_FDN,						/**< fixed Dialing number */
	SIM_UST_EXT2,						/**< extension2 */
	SIM_UST_SDN,						/**< service Dialing number */
	SIM_UST_EXT3,						/**< extension3 */
	SIM_UST_BDN,						/**< barred Dialing numbers */
	SIM_UST_EXT4,						/**< extension 4 */
	SIM_UST_OUTGOING_CALL_INFO,				/**< outgoing call information */

	// service 9 ~ 16
	SIM_UST_INCOMING_CALL_INFO = 8,				/**< incoming call information */
	SIM_UST_SMS,						/**< short message storage */
	SIM_UST_SMS_STATUS_REPORTS,				/**< short message status reports */
	SIM_UST_SMS_PARAMS,					/**< sms parameteres */
	SIM_UST_AOC,						/**< advice of charge */
	SIM_UST_CCP,						/**< capability configuration parameters */
	SIM_UST_CELL_BROADCAST_MSG_ID,				/**< cell broadcast message identifier */
	SIM_UST_CELL_BROADCAST_MSG_ID_RANGES,			/**< cell broadcast message identifier ranges */

	// service 17 ~ 24
	SIM_UST_GID_LV1 = 16,					/**< group identifier level 1 */
	SIM_UST_GID_LV2,					/**< group identifier level 2 */
	SIM_UST_SPN,						/**< service provider name */
	SIM_UST_USER_CTRLED_PLMN_SELECTOR_WACT,			/**< user controlled PLMN selector with Access technology */
	SIM_UST_MSISDN,						/**< msisdn */
	SIM_UST_IMG,						/**< image */
	SIM_UST_SOLSA,						/**< support of local service area */
	SIM_UST_ENHANCED_MULTI_LV_PRECEDENCE_PREEMPTION_SRVC,	/**< enhanced multi-level precedence and pre-emption service */

	// service 25 ~ 32
	SIM_UST_AUTO_ANSWER_FOR_EMLPP = 24,			/**< automatic answer fro eMLPP */
	SIM_UST_RFU1,						/**< rfu */
	SIM_UST_GSM_ACCESS,					/**< gsm access */
	SIM_UST_DATA_DOWNLOAD_VIA_SMS_PP,			/**< data download via sms-pp */
	SIM_UST_DATA_DOWNLOAD_VIA_SMS_CB,			/**< data download via sms-cb */
	SIM_UST_CALL_CTRL,					/**< call control by usim*/
	SIM_UST_MO_SMS_CTRL,					/**< mobile-originated short message control by usim */
	SIM_UST_RUN_AT_CMD_CMD,					/**< RUN AT COMMAND command */

	// service 33 ~ 40
	SIM_UST_SHALL_BE_SET_TO_ONE = 32,			/**< shall be set to 1 */
	SIM_UST_ENABLED_SRVC_TABLE,				/**< enabled service table */
	SIM_UST_ACL,						/**< APN control list */
	SIM_UST_DEPERSONALIZATION_CTRL_KEYS,			/**< de-personalization control keys */
	SIM_UST_COOPERATIVE_NETWORK_LIST,			/**< co-operative network list */
	SIM_UST_GSM_SEC_CONTEXT,				/**< gsm security context */
	SIM_UST_CPBCCH_INFO,					/**< CPBCCH information */
	SIM_UST_INVESTIGATION_SCAN,				/**< investigation scan */

	// service 41 ~ 48
	SIM_UST_MEXE = 40,					/**< MExE */
	SIM_UST_OPERATOR_CTRLED_PLMN_SELECTOR_WACT,		/**< operator controlled PLMN selector with Access technology */
	SIM_UST_HPLMN_SELECTOR_WACT,				/**< HPLMN selector with access technology */
	SIM_UST_EXT5,						/**< extension 5 */
	SIM_UST_PLMN_NETWORK_NAME,				/**< PLMN Network Name*/
	SIM_UST_OPERATOR_PLMN_LIST,				/**< Operator PLMN List*/
	SIM_UST_MBDN,						/**< Mailbox Dialling Numbers*/
	SIM_UST_MWIS,						/**< Message Waiting Indication Status*/

	// service 49 ~ 56
	SIM_UST_CFIS = 48,					/**<Call Forwarding Indication Status*/
	SIM_UST_RPLMN_LAST_USED_ACCESS_TECH,			/**< RPLMN last used access technology */
	SIM_UST_SPDI,						/**< Service Provider Display Information*/
	SIM_UST_MMS,						/**< multi media messaging service */
	SIM_UST_EXT8,						/**< extension 8 */
	SIM_UST_CALL_CTRL_ON_GPRS,				/**< call control on gprs by usim */
	SIM_UST_MMS_USER_CONNECTIVITY_PARAMS,			/**< mms user connectivity parameters */
	SIM_UST_NIA,						/**< network's indication of alerting in the MS (NIA) */

	// service 57 ~ 64
	SIM_UST_VGCS_GID_LIST,					/**< vgcs group identifier List (EF-VGCS, EF-VGCSS) */
	SIM_UST_VBS_GID_LIST,					/**< vbs group identifier List (EF-VBS, EF-VBSS) */
	SIM_UST_PSEUDONYM,
	SIM_UST_USER_CTRLED_PLMN_SELECTOR_IWLAN,		/**< user controlled PLMN selector for I-WLAN access */
	SIM_UST_OPERATOR_CTRLED_PLMN_SELECTOR_IWLAN,		/**< operator controlled PLMN selector for I-WLAN access */
	SIM_UST_USER_CTRLED_WSID_LIST,
	SIM_UST_OPERATOR_CTRLED_WSID_LIST,
	SIM_UST_VGCS_SEC,
};

/**
 * @enum tel_sim_cdma_service
 *  This enumeration defines the list of CST services in CDMA Service Table.
 */
enum tel_sim_cdma_service {
	// service 1 ~ 8
	SIM_CDMA_ST_CHV_DISABLE = 0,	/**< CHV Disable Option*/
	SIM_CDMA_ST_ADN,		/**< Abbreviated Dialing number */
	SIM_CDMA_ST_FDN,		/**< Fixed Dialing number */
	SIM_CDMA_ST_SMS,		/**< short message storage  */
	SIM_CDMA_ST_HRPD,		/**< HRPD*/
	SIM_CDMA_ST_EPB,		/**< Enhanced Phone Book */
	SIM_CDMA_ST_MMD,		/**< Multimedia domain */
	SIM_CDMA_ST_EUIMID,		/**< SF_EUIMID- based  EUIMID*/

	// service 9 ~ 16
	SIM_CDMA_ST_MEID = 8,		/**< MEID */
	SIM_CDMA_ST_EXT1,		/**< Extension1 */
	SIM_CDMA_ST_EXT2,		/**< Extension2 */
	SIM_CDMA_ST_SMSP,		/**< sms parameteres */
	SIM_CDMA_ST_LND,		/**< Last number dialled */
	SIM_CDMA_ST_SCP,		/**< Service Category Program for BC-SMS */
	SIM_CDMA_ST_RFU1,		/**< RFU */
	SIM_CDMA_ST_RFU2,		/**< RFU */

	// service 17 ~ 24
	SIM_CDMA_ST_HSPN = 16,		/**< CDMA Home Service Provider Name */
	SIM_CDMA_ST_SDN,		/**< Service Dialing number */
	SIM_CDMA_ST_EXT3,		/**< Extension3 */
	SIM_CDMA_ST_3GPDSIP,		/**< 3GPD-SIP */
	SIM_CDMA_ST_RFU3,		/**< RFU */
	SIM_CDMA_ST_RFU4,		/**< RFU */
	SIM_CDMA_ST_RFU5,		/**< RFU */
	SIM_CDMA_ST_RFU6,		/**< RFU */

	// service 25 ~ 32
	SIM_CDMA_ST_DDSMSB = 24,	/**< Data download by SMS broadcast */
	SIM_CDMA_ST_DDSMSPP,		/**< Data download by SMS PP */
	SIM_CDMA_ST_MENU,		/**< Menu Selection */
	SIM_CDMA_ST_CALLC,		/**< Call Control */
	SIM_CDMA_ST_PROACTIVE,		/**< Proactive RUIM */
	SIM_CDMA_ST_AKA,		/**< AKA*/
	SIM_CDMA_ST_RFU7,		/**< RFU */
	SIM_CDMA_ST_RFU8,		/**< RFU */

	// service 33 ~ 40
	SIM_CDMA_ST_RFU9 = 32,		/**< RFU */
	SIM_CDMA_ST_RFU10,		/**< RFU */
	SIM_CDMA_ST_RFU11,		/**< RFU */
	SIM_CDMA_ST_RFU12,		/**< RFU */
	SIM_CDMA_ST_RFU13,		/**< RFU */
	SIM_CDMA_ST_3GPDMIP,		/**< 3GPD- MIP */
	SIM_CDMA_ST_BCMCS,		/**< BCMCS */
	SIM_CDMA_ST_MMS,		/**< Multimedia messaging service */

	// service 41 ~ 48
	SIM_CDMA_ST_EXT8 = 40,		/**< Extension 8 */
	SIM_CDMA_ST_MMSUCP,		/**< MMS User Connectivity Parameters */
	SIM_CDMA_ST_AA,			/**< Application Authentication */
	SIM_CDMA_ST_GIL1,		/**< Group Identifier Level 1 */
	SIM_CDMA_ST_GIL2,		/**< Group Identifier Level 2 */
	SIM_CDMA_ST_DEPERSO,		/**< Depersonalisation control keys */
	SIM_CDMA_ST_CNL			/**< Co-operative Network List */
};

/**
 * @enum tel_sim_csim_service
 *  This enumeration defines the list of CST services in CSIM Service Table. (CSIM)
 */
enum tel_sim_csim_service {
	// service 1 ~ 8
	SIM_CSIM_ST_LOCAL_PHONEBOOK = 0,	/**< Local Phone book */
	SIM_CSIM_ST_FDN,			/**< Fixed Dialing Numbers (FDN) */
	SIM_CSIM_ST_EXT2,			/**< Extension 2 */
	SIM_CSIM_ST_SDN,			/**< Service Dialing Numbers (SDN)  */
	SIM_CSIM_ST_EXT3,			/**< Extension 3 */
	SIM_CSIM_ST_SMS,			/**< Short Message Storage (SMS) */
	SIM_CSIM_ST_SMSP,			/**< Short Message Parameters */
	SIM_CSIM_ST_HRPD,			/**< HRPD */

	// service 9 ~ 16
	SIM_CSIM_ST_SCP = 8,			/**< Service Category Program for BC-SMS */
	SIM_CSIM_ST_HSPN,			/**< CDMA Home Service Provider Name */
	SIM_CSIM_ST_DDSMSB,			/**< Data Download via SMS Broadcast */
	SIM_CSIM_ST_DDSMSPP,			/**< Data Download via SMS-PP */
	SIM_CSIM_ST_CALLC,			/**< Call Control */
	SIM_CSIM_ST_3GPDSIP,			/**< 3GPD-SIP */
	SIM_CSIM_ST_3GPDMIP,			/**< 3GPD-MIP */
	SIM_CSIM_ST_AKA,			/**< AKA */

	// service 17 ~ 24
	SIM_CSIM_ST_IP_LCS = 16,		/**< IP-based Location Services (LCS) */
	SIM_CSIM_ST_BCMCS,			/**< BCMCS */
	SIM_CSIM_ST_MMS,			/**< Multimedia Messaging Service (MMS) */
	SIM_CSIM_ST_EXT8,			/**< Extension 8 */
	SIM_CSIM_ST_MMSUCP,			/**< MMS User Connectivity Parameters */
	SIM_CSIM_ST_AA,				/**< Application Authentication */
	SIM_CSIM_ST_GIL1,			/**< Group Identifier Level 1 */
	SIM_CSIM_ST_GIL2,			/**< Group Identifier Level 2 */

	// service 25 ~ 32
	SIM_CSIM_ST_DEPERSO = 24,		/**< De-Personalization Control Keys */
	SIM_CSIM_ST_CNL,			/**< Cooperative Network List */
	SIM_CSIM_ST_OCI,			/**< Outgoing Call Information (OCI) */
	SIM_CSIM_ST_ICI,			/**< Incoming Call Information (ICI) */
	SIM_CSIM_ST_EXT5,			/**< Extension 5 */
	SIM_CSIM_ST_MM_STORAGE,			/**< Multimedia Storage */
	SIM_CSIM_ST_IMG,			/**< Image (EFIMG) */
	SIM_CSIM_ST_EST,			/**< Enabled Services Table */

	// service 33 ~ 41
	SIM_CSIM_ST_CCP = 32,			/**< Capability Configuration Parameters (CCP) */
	SIM_CSIM_ST_EUIMIDl,			/**< SF_EUIMID-based EUIMID */
	SIM_CSIM_ST_3GPD_EXT,			/**< Messaging and 3GPD Extensions */
	SIM_CSIM_ST_ROOT_CERTI,			/**< Root Certificates */
	SIM_CSIM_ST_WAP,			/**< WAP Browser */
	SIM_CSIM_ST_JAVA,			/**< Java */
	SIM_CSIM_ST_RSVD_CDG1,			/**< Reserved for CDG */
	SIM_CSIM_ST_RSVD_CDG2,			/**< Reserved for CDG */
	SIM_CSIM_ST_IPV6 = 40,			/**< IPv6 */
};

enum tel_sim_ist_service {
	SIM_IST_PCSCF_ADDR = 0,		/**< P-CSCF address */
	SIM_IST_GBA,			/**< Generic Bootstrapping Architecture */
	SIM_IST_HTTP_DIGEST,		/**< HTTP Digest */
	SIM_IST_GBA_LOCAL_KEY,		/**< GBA-based Local Key Establishment Mechanism */
	SIM_IST_PCSCF_LOCAL_BREAK_OUT,	/**< Support of P-CSCF discovery for IMS Local Break Out */
	SIM_IST_SMS,			/**< Short Message Storage */
	SIM_IST_SMSR,			/**< Short Message Status Reports */
	SIM_IST_SM_OVER_IP,		/**< Support for SM-over-IP including data download via SMS-PP as defined in TS 31.111 [31] */

	SIM_IST_COMMUNICATION_CONTROL = 8, /**< Communication Control for IMS by ISIM */
	SIM_IST_ACCESS_TO_IMS,		/**< Support of UICC access to IMS */
	SIM_IST_URI_SUPPORT,		/**< URI support by UICC */
};

enum tel_sim_refresh_command{
	SIM_REFRESH_CMD_INIT_AND_FULL_FCN = 0,		/**<command qualifier for REFRESH SIM INIT AND FULL FILE CHANGE_NOTIFICATION */
	SIM_REFRESH_CMD_FCN = 1,			/**<command qualifier for REFRESH FILE CHANGE NOTIFICATION */
	SIM_REFRESH_CMD_INIT_AND_FCN = 2,		/**<command qualifier for REFRESH SIM INIT AND FILE CHANGE NOTIFICATION */
	SIM_REFRESH_CMD_INIT = 3,			/**<command qualifier for REFRESH SIM INIT */
	SIM_REFRESH_CMD_RESET = 4,			/**<command qualifier for REFRESH SIM RESET */
	SIM_REFRESH_CMD_3G_APPLICATION_RESET = 5,	/**<command qualifier for REFRESH 3G APPLICATION RESET */
	SIM_REFRESH_CMD_3G_SESSION_RESET = 6,		/**<command qualifier for REFRESH 3G SESSION RESET */
	SIM_REFRESH_CMD_RESERVED = 0xFF			/**<command qualifier for REFRESH RESERVED */
};

enum tel_sim_cst_type {
	SIM_CDMA_SVC_TABLE = 0,
	SIM_CSIM_SVC_TABLE,
	SIM_MAX_SVC_TABLE
};

enum tel_sim_pcscf_type {
	SIM_PCSCF_TYPE_FQDN, /**< Fully Qualified Domain Name */
	SIM_PCSCF_TYPE_IPV4, /**< IPv4 */
	SIM_PCSCF_TYPE_IPV6, /**< IPv6 */
};

enum tel_sim_powerstate {
	SIM_POWER_OFF = 0x00, /**< OFF */
	SIM_POWER_ON = 0x01, /**< ON */
	SIM_POWER_UNSPECIFIED = 0xFF /**< Unspecified */
};

enum tel_sim_provision {
	SIM_PROVISION_DEACTIVATE = 0x00, /**< De-activate Provisioning */
	SIM_PROVISION_ACTIVATE = 0x01, /**< Activate Provisioning */
	SIM_PROVISION_UNSPECIFIED = 0xFF /**< Unspecified */
};

enum tel_sim_img_coding_scheme{
	SIM_IMG_CODING_SCHEME_BASIC = 0x11,
	SIM_IMG_CODING_SCHEME_COLOUR = 0x21,
	SIM_IMG_CODING_SCHEME_RESERVED = 0xFF
};

struct tel_sim_sst {
	char service[SIM_SST_SERVICE_CNT_MAX];	/**< should accessed with 'enum tel_sim_sst_service' as index */
};

struct tel_sim_ust {
	char service[SIM_UST_SERVICE_CNT_MAX];	/**< should accessed with 'enum tel_sim_ust_service' as index */
};

struct tel_sim_cst {
	enum tel_sim_cst_type cdma_svc_table;
	union {
		char cdma_service[SIM_CDMA_ST_SERVICE_CNT_MAX];	/**< should accessed with 'enum tel_sim_cdma_service' as index */
		char csim_service[SIM_CSIM_ST_SERVICE_CNT_MAX];	/**< should accessed with 'enum tel_sim_csim_service' as index */
	} service;
};

struct tel_sim_ist {
	char service[SIM_IST_SERVICE_CNT_MAX];  // should accessed with 'enum tel_sim_ist_service' as index
};

struct tel_sim_service_table {
	enum tel_sim_type sim_type;
	union {
		struct tel_sim_sst sst;
		struct tel_sim_ust ust;
		struct tel_sim_cst cst;
	} table;
};

struct tel_sim_est {
	gboolean bFdnEnabled;
	gboolean bBdnEnabled;
	gboolean bAclEnabled;
};

struct tel_sim_imsi {
	char plmn[6 + 1];	/**< SIM MCC, MNC code. MNC is 2 digit or 3digit */
	char msin[10 + 1];	/**< Mobile Station Identification Number */
};

struct tel_sim_dialing_number {
	int alpha_id_max_len;				/**< alpha max length in SIM - READ ONLY*/
	char alpha_id[SIM_XDN_ALPHA_ID_LEN_MAX + 1];	/**< Alpha Identifier */
	enum tel_sim_ton ton;				/**< Type Of Number */
	enum tel_sim_npi npi;				/**< Number Plan Identity */
	char num[SIM_XDN_NUMBER_LEN_MAX + 1];		/**< Dialing Number/SSC String */
	unsigned char cc_id;				/**< Capability/Configuration Identifier */
	unsigned char ext1_id;				/**< Extensiion1 Record Identifier */
};

struct tel_sim_gid {
	int GroupIdentifierLen;					/**< Group identifier length */
	unsigned char szGroupIdentifier[SIM_GROUP_IDENTIFIER_LEN_MAX];	/**< Group identifier */
};

struct tel_sim_cphs_svc_call_offering_s {
	int b_call_forwarding_unconditional; /**< CallForwarding Unconditional */
	int b_call_forwarding_on_user_busy; /**< CallForwarding On UserBusy */
	int b_call_forwarding_on_no_reply; /**< CallForwarding On NoReply */
	int b_call_forwarding_on_user_not_reachable; /**< CallForwarding On User Not Reachable */
	int b_call_transfer; /**< Call Transfer */
};

struct tel_sim_cphs_svc_call_restriction_s {
	int b_barring_of_all_outgoing_calls; /**< Barring Of All Outgoing Calls*/
	int b_barring_of_outgoing_international_calls; /**< Barring Of Outgoing International Calls */
	int b_barring_of_outgoing_international_calls_except_hplmn; /**< Barring Of Outgoing International Calls Except HPLMN */
	int b_barring_of_all_incoming_calls_roaming_outside_hplmn; /**< Barring Of All Incoming Calls Roaming Outside HPLMN */
	int b_barring_of_incoming_calls_when_roaming; /**< Barring Of IncomingCalls When Roaming */
};

struct tel_sim_cphs_svc_other_ss_s {
	int b_multi_party_service; /**< MultiPartyService*/
	int b_closed_user_group; /**< ClosedUserGroup*/
	int b_advice_of_charge; /**< AdviceOfCharge*/
	int b_preferential_closed_user_group; /**< PreferentialClosedUserGroup*/
	int b_closed_user_group_outgoing_access; /**< ClosedUserGroupOutgoingAccess*/
};

struct tel_sim_cphs_svc_call_complete_s {
	int b_call_hold; /**< Call Hold*/
	int b_call_waiting; /**< Call Waiting*/
	int b_completion_of_call_to_busy_subscriber; /**< Completion Of Call To Busy Subscriber*/
	int b_user_user_signalling; /**< User User Signaling*/
};

struct tel_sim_cphs_svc_teleservices_s {
	int b_short_message_mobile_originated; /**< Short Message Mobile Originated*/
	int b_short_message_mobile_terminated; /**< Short Message Mobile Terminated*/
	int b_short_message_cell_broadcast; /**< Short Message Cell Broadcast*/
	int b_short_message_reply_path; /**< Short	Message Reply Path*/
	int b_short_message_delivery_conf; /**< Short Message Delivery Conf*/
	int b_short_message_protocol_identifier; /**< Short Message Protocol Identifier*/
	int b_short_message_validity_period; /**< Short Message Validity Period*/
};

struct tel_sim_cphs_svc_cphs_teleservices_s {
	int b_alternative_line_service; /**< Alternative Line Service*/
};

struct tel_sim_cphs_svc_cphs_features_s {
	int b_string_service_table; /**< String Service Table*/
};

struct tel_sim_cphs_svc_number_identifier_s {
	int b_calling_line_identification_present; /**< Calling Line Identification Present*/
	int b_connected_line_identification_restrict; /**< Connected Line Identification Restrict*/
	int b_connected_line_identification_present; /**< Connected Line Identification Present*/
	int b_malicious_call_identifier; /**< Malicious Call Identifier*/
	int b_calling_line_identification_send; /**< Calling Line Identification Send*/
	int b_calling_line_identification_block; /**< Calling Line Identification Block*/
};

struct tel_sim_cphs_svc_phase_services_s {
	int b_menu_for_gprs; /**< Menu For GPRS*/
	int b_menu_for_high_speed_csd; /**< Menu For HighSpeedCsd*/
	int b_menu_for_voice_group_call; /**< Menu For VoiceGroupCall*/
	int b_menu_for_voice_broadcast_service; /**< Menu For VoiceBroadcastService*/
	int b_menu_for_multiple_subscriber_profile; /**< Menu For MultipleSubscriberProfile*/
	int b_menu_for_multiple_band; /**< Menu For MultipleBand*/
};

struct tel_sim_cphs_svc_value_added_services_s {
	int b_restrict_menu_for_manual_selection; /**< RestrictMenu For ManualSelection*/
	int b_restrict_menu_for_voice_mail; /**< RestrictMenu For VoiceMail*/
	int b_restrict_menu_for_mo_sms_and_paging; /**< RestrictMenu For MoSmsAndPaging*/
	int b_restrict_menu_for_mo_sms_with_emial_type; /**< RestrictMenu For MoSmsWithEmialType*/
	int b_restrict_menu_for_fax_calls; /**< RestrictMenu For FaxCalls*/
	int b_restrict_menu_for_data_calls; /**< RestrictMenu For DataCalls*/
	int b_restrict_menu_for_change_language; /**< RestrictMenu For ChangeLanguage*/
};

struct tel_sim_cphs_svc_information_numbers_s {
	int b_information_numbers; 	/**< Information Numbers*/
};

struct tel_sim_cphs_csp_entry_s {
	enum tel_sim_cphs_csp_group customer_service_group; /**< customer service group*/
	union {
		struct tel_sim_cphs_svc_call_offering_s call_offering;			/**< call offering*/
		struct tel_sim_cphs_svc_call_restriction_s call_restriction;		/**< call restriction*/
		struct tel_sim_cphs_svc_other_ss_s other_supp_services;			/**< other SS services*/
		struct tel_sim_cphs_svc_call_complete_s call_complete;			/**< call complete*/
		struct tel_sim_cphs_svc_teleservices_s teleservices;			/**< teleservices*/
		struct tel_sim_cphs_svc_cphs_teleservices_s cphs_teleservices;		/**< CPHS teleservices*/
		struct tel_sim_cphs_svc_cphs_features_s cphs_features;			/**< CPHS features*/
		struct tel_sim_cphs_svc_number_identifier_s number_identifiers;		/**< number identifiers*/
		struct tel_sim_cphs_svc_phase_services_s phase_services;			/**< phase services*/
		struct tel_sim_cphs_svc_value_added_services_s value_added_services;	/**< value added services*/
		struct tel_sim_cphs_svc_information_numbers_s information_numbers;	/**< information numbers*/
	} service;
};

struct tel_sim_cphs_csp {
	struct tel_sim_cphs_csp_entry_s service_profile_entry[SIM_CPHS_CSP_ENTRY_CNT_MAX];	/**< service profile entry*/
};

#if defined TIZEN_GLOBALCONFIG_ENABLE_CSP
struct treq_sim_set_cphs_csp_info {
	struct tel_sim_cphs_csp cphs_csp_info;
};
#endif

struct tel_sim_cphs_dflag {
	enum tel_sim_cphs_dynamic_flag_selected_line DynamicFlags;	/**< Dynamic flags information */
};

struct tel_sim_cphs_dflag2 {
	enum tel_sim_cphs_dynamic2_flag_als_status Dynamic2Flag;	/**< Dynamic flags status */
};

struct tel_sim_cphs_service_tablie_s {
	/* Byte 2 - bit1 & 2*/
	int bCustomerServiceProfile;	/**< Customer Service Profile (CSP)  */
	/* Byte 2 - bit3 & 4*/
	int bServiceStringTable;	/**< Service String Table (SST) */
	/* Byte 2 - bit5 & 6*/
	int bMailBoxNumbers;		/**< MailBoxNumbers */
	/* Byte 2 - bit7 & 8*/
	int bOperatorNameShortForm;	/**< Short form of operator name */
	/* Byte 3 - bit1 & 2*/
	int bInformationNumbers;	/**< Information numbers */
};

struct tel_sim_cphs_info {
	enum tel_sim_cphs_phase CphsPhase;			/**< CPHS phase type */
	struct tel_sim_cphs_service_tablie_s CphsServiceTable;	/**< CPHS service table */
};

struct tel_sim_cphs_info_number {
	int AlphaIdLength;					/**< length of alpha identifier */
	enum tel_sim_cphs_index_level IndexLevelIndicator;	/**< SIM CPHS index level one */
	int PremiumServiceIndicator;				/**< SIM CPHS index level one */
	int NetworkSpecificIndicator;				/**< SIM CPHS index level one */
	unsigned char Alpha_id[SIM_XDN_ALPHA_ID_LEN_MAX + 1];	/**<  Alpha Identifier */
	unsigned long DiallingnumLength;			/**< Length of BCD number/SSC contents */
	enum tel_sim_ton TypeOfNumber;				/**< TON */
	enum tel_sim_npi NumberingPlanIdentity;			/**< NPI */
	char DiallingNum[SIM_XDN_NUMBER_LEN_MAX + 1];		/**< Dialing Number/SSC String */
	unsigned char Ext1RecordId;				/**< Extensiion1 Record Identifier */
};

struct tel_sim_ecc {
	char ecc_num[SIM_ECC_BYTE_LEN_MAX * 2 + 1];		/**< Emergency Call Code info-ECC is coded in BCD format. null termination used*/
	char ecc_string[SIM_ECC_STRING_LEN_MAX + 1];		/**< Alphabet identifier. null termination used*/
	enum tel_sim_emergency_service_type ecc_category;	/**< ECC emergency service information */
};

struct tel_sim_ecc_list {
	int ecc_count;
	struct tel_sim_ecc ecc[SIM_ECC_RECORD_CNT_MAX];
};

struct tel_sim_ext {
	int ext_len;
	char ext[SIM_EXT_RECORD_CNT_MAX + 1];
	unsigned short next_record;
};

struct tel_sim_language {
	int language_count;
	enum tel_sim_language_type language[SIM_LANG_CNT_MAX];
};

struct tel_sim_iccid {
	char iccid[SIM_ICCID_LEN_MAX + 1];
};

struct tel_sim_mbi {
	/* '00' - no mailbox dialing number associated with message waiting indication group type.
	 'xx' - record number in EFMBDN associated with message waiting indication group type.
	 */
	int voice_index;	/**< voice mailbox identifier*/
	int fax_index;		/**< FAX mailbox identifier*/
	int email_index;	/**< email mailbox identifier*/
	int other_index;	/**< other mailbox identifier*/
	int video_index;	/**< video mailbox identifier*/
};

struct tel_sim_mbi_list {
	int profile_count;
	struct tel_sim_mbi mbi[SIM_MSP_CNT_MAX];
};

struct tel_sim_mb_number{
	int rec_index;			/**< SIM record index; */
	int profile_number;		/**< Subscriber profile number(ex: line1, line2, lineX) */
	enum tel_sim_mailbox_type mb_type;
	struct tel_sim_dialing_number number_info;
};

struct tel_sim_mailbox {
	gboolean b_cphs;
	int count;
	struct tel_sim_mb_number mb[SIM_MSP_CNT_MAX*5]; /**< each profile mailbox number can exist 5 numbers */
};

struct tel_sim_cfis {
	int rec_index;
	unsigned char msp_num;				/**< MSP number*/
	unsigned char cfu_status;			/**< call forwarding unconditional indication status*/
	enum tel_sim_ton ton;				/**< TON*/
	enum tel_sim_npi npi;				/**< NPI*/
	char cfu_num[SIM_XDN_NUMBER_LEN_MAX + 1];	/**< Dialing Number/SSC String*/
	unsigned char cc2_id;				/**< Capability/Configuration2 Record Identifier */
	unsigned char ext7_id;				/**< Extension 7 Record Identifier */
};

struct tel_sim_cf_list {
	int profile_count;
	struct tel_sim_cfis cf[SIM_MSP_CNT_MAX];
};

struct tel_sim_cphs_cf {
	gboolean b_line1;	/**< CallForwardUnconditionalLine 1 */
	gboolean b_line2;	/**< CallForwardUnconditionalLine 2 */
	gboolean b_fax;		/**< CallForwardUnconditional FAX */
	gboolean b_data;	/**<CallForwardUnconditional data*/
};

struct tel_sim_callforwarding {
	gboolean b_cphs;
	struct tel_sim_cf_list cf_list;
	struct tel_sim_cphs_cf cphs_cf;
};

struct tel_sim_mw {
	int rec_index;
	unsigned char indicator_status;	/**< Indicator status*/
	int voice_count;		/**< VoiceMail Count*/
	int fax_count;			/**< FAX Count*/
	int email_count;		/**< Email Count*/
	int other_count;		/**< Other Count*/
	int video_count;		/**< VideoMail Count*/
};

struct tel_sim_mw_list {
	int profile_count;
	struct tel_sim_mw mw[SIM_MSP_CNT_MAX];
};

struct tel_sim_cphs_mw {
	gboolean b_voice1;	/**< VoiceMsgLine1 message waiting flag */
	gboolean b_voice2;	/**< VoiceMsgLine2 message waiting flag */
	gboolean b_fax;		/**< FAX message waiting flag */
	gboolean b_data;	/**< Data message waiting flag */
};

struct tel_sim_messagewaiting {
	gboolean b_cphs;
	struct tel_sim_mw_list mw_list;
	struct tel_sim_cphs_mw cphs_mw;
};

struct treq_sim_set_messagewaiting {
	gboolean b_cphs;
	struct tel_sim_mw mw;
	struct tel_sim_cphs_mw cphs_mw;
};

struct tel_sim_msisdn {
	unsigned char num[SIM_MSISDN_NUMBER_LEN_MAX + 1];
	enum tel_sim_ton ton;
	unsigned char name[SIM_XDN_ALPHA_ID_LEN_MAX + 1];
	unsigned short next_record;
};

struct tel_sim_msisdn_list {
	int count;
	struct tel_sim_msisdn msisdn[SIM_MSISDN_RECORD_CNT_MAX];
};

struct tel_sim_spn {
	unsigned char display_condition;		/**< display condition (1 byte) */
	unsigned char spn[SIM_CDMA_SPN_LEN_MAX + 1];	/**< SPN */
};

struct tel_sim_spdi {
	int plmn_count;
	struct {
		unsigned char plmn[6+1];
	}list[SIM_SPDI_PLMN_MAX];
};

struct tel_sim_opl {
	unsigned char plmn[6+1];
	unsigned short lac_from;
	unsigned short lac_to;
	unsigned char rec_identifier;
};

struct tel_sim_opl_list {
	int opl_count;
	struct tel_sim_opl *list;
};

struct tel_sim_pnn{
	unsigned char full_name[SIM_NW_FULL_NAME_LEN_MAX + 1];
	unsigned char short_name[SIM_NW_FULL_NAME_LEN_MAX + 1];
};

struct tel_sim_pnn_list {
	int pnn_count;
	struct tel_sim_pnn *list;
};

struct tel_sim_cphs_netname {
	unsigned char full_name[SIM_CPHS_OPERATOR_NAME_LEN_MAX+1];
	unsigned char short_name[SIM_CPHS_OPERATOR_NAME_SHORT_FORM_LEN_MAX+1];
};

struct tel_sim_oplmnwact {
	unsigned char plmn[6+1];
	gboolean b_umts;
	gboolean b_gsm;
};

struct tel_sim_oplmnwact_list {
	int opwa_count;
	struct tel_sim_oplmnwact opwa[SIM_OPLMNWACT_LEN_MAX];
};

struct tel_sim_file_list{
	int file_count;
	enum tel_sim_file_id file_id[SIM_FILE_ID_LIST_MAX_COUNT];
};

struct tel_sim_img{
	unsigned char width;
	unsigned char height;
	enum tel_sim_img_coding_scheme ics;
	unsigned short iidf_fileid;
	unsigned short offset;
	unsigned short length;
	unsigned char icon_data[1000];	/*Data length should be telephony defined as no restriction as per spec - Need to decide*/
};

struct tel_sim_impi {
	char *impi; /**< ISIM IMPI data */
};

struct tel_sim_impu {
	char *impu; /**< ISIM IMPU data */
};

struct tel_sim_impu_list {
	unsigned int count; /**< ISIM IMPU data count */
	struct tel_sim_impu impu[SIM_IMPU_CNT_MAX]; /**< ISIM IMPU list */
};

struct tel_sim_domain {
	char *domain; /**< ISIM Domain data */
};

struct tel_sim_pcscf {
	enum tel_sim_pcscf_type type; /**< ISIM P-CSCF type */
	char *pcscf; /**< ISIM P-CSCF data */
};

struct tel_sim_pcscf_list {
	unsigned int count; /**< ISIM P-CSCF data count */
	struct tel_sim_pcscf pcscf[SIM_PCSCF_CNT_MAX]; /**< ISIM P-CSCF list */
};

struct treq_sim_verify_pins {
	enum tel_sim_pin_type pin_type;
	unsigned int pin_length;
	char pin[9];
};

struct treq_sim_verify_puks {
	enum tel_sim_pin_type puk_type;
	unsigned int puk_length;
	char puk[9];
	unsigned int pin_length;
	char pin[9];
};

struct treq_sim_change_pins {
	enum tel_sim_pin_type type;
	unsigned int old_pin_length;
	char old_pin[9];
	unsigned int new_pin_length;
	char new_pin[9];
};

struct treq_sim_get_facility_status {
	enum tel_sim_facility_type type;
};

struct treq_sim_disable_facility {
	enum tel_sim_facility_type type;
	unsigned int password_length;
	char password[39];
};

struct treq_sim_enable_facility {
	enum tel_sim_facility_type type;
	unsigned int password_length;
	char password[39];
};

struct treq_sim_get_lock_info {
	enum tel_sim_facility_type type;
};

struct treq_sim_transmit_apdu {
	unsigned int apdu_length;
	unsigned char apdu[SIM_APDU_DATA_LEN_MAX];
};

struct treq_sim_set_language {
	enum tel_sim_language_type language;
};

struct treq_sim_req_authentication {
	enum tel_sim_auth_type auth_type;		/**< Authentication type */
	unsigned int rand_length;			/**< the length of RAND */
	unsigned int autn_length;			/**< the length of AUTN. it is not used in case of GSM AUTH */
	char rand_data[SIM_AUTH_REQ_DATA_LEN_MAX + 1];	/**< RAND data */
	char autn_data[SIM_AUTH_REQ_DATA_LEN_MAX + 1];	/**< AUTN data. it is not used in case of GSM AUTH */
};

struct treq_sim_set_powerstate {
	enum tel_sim_powerstate state;
};

struct treq_sim_set_mailbox {
	gboolean b_cphs;
	struct tel_sim_mb_number mb_info;
};

struct treq_sim_set_callforwarding {
	gboolean b_cphs;
	struct tel_sim_cfis cf;
	struct tel_sim_cphs_cf cphs_cf;
};

struct treq_sim_set_provisioning {
	enum tel_sim_provision cmd;
};

struct tresp_sim_set_powerstate {
	enum tel_sim_power_set_result result;
};

struct tresp_sim_verify_pins {
	enum tel_sim_pin_operation_result result;
	enum tel_sim_pin_type pin_type;
	int retry_count;
};

struct tresp_sim_verify_puks {
	enum tel_sim_pin_operation_result result;
	enum tel_sim_pin_type pin_type;
	int retry_count;
};

struct tresp_sim_change_pins {
	enum tel_sim_pin_operation_result result;
	enum tel_sim_pin_type pin_type;
	int retry_count;
};

struct tresp_sim_get_facility_status {
	enum tel_sim_pin_operation_result result;
	enum tel_sim_facility_type type;
	gboolean b_enable;
};

struct tresp_sim_disable_facility {
	enum tel_sim_pin_operation_result result;
	enum tel_sim_facility_type type;
	int retry_count;
};

struct tresp_sim_enable_facility {
	enum tel_sim_pin_operation_result result;
	enum tel_sim_facility_type type;
	int retry_count;
};

struct tresp_sim_get_lock_info {
	enum tel_sim_pin_operation_result result;
	enum tel_sim_facility_type type;
	enum tel_sim_lock_status lock_status;
	int retry_count;
};

struct tresp_sim_transmit_apdu {
	enum tel_sim_access_result result;
	unsigned int apdu_resp_length;
	unsigned char *apdu_resp;
};

struct tresp_sim_get_atr {
	enum tel_sim_access_result result;
	unsigned int atr_length;
	unsigned char atr[256 + 2];
};

struct tresp_sim_set_data {
	enum tel_sim_access_result result;
};

struct tresp_sim_read {
	enum tel_sim_access_result result;
	union {
		struct tel_sim_ecc_list ecc;
		struct tel_sim_language language;
		struct tel_sim_iccid iccid;
		struct tel_sim_mailbox mb;
		struct tel_sim_callforwarding cf;
		struct tel_sim_messagewaiting mw;
		struct tel_sim_cphs_info cphs;
		struct tel_sim_service_table svct;
		struct tel_sim_msisdn_list msisdn_list;
		struct tel_sim_spn spn;
		struct tel_sim_spdi spdi;
		struct tel_sim_opl_list opl;
		struct tel_sim_pnn_list pnn;
		struct tel_sim_cphs_netname cphs_net;
		struct tel_sim_oplmnwact_list opwa;
		struct tel_sim_img img;
		struct tel_sim_gid gid;
		struct tel_sim_impi impi;
		struct tel_sim_impu_list impu_list;
		struct tel_sim_domain domain;
		struct tel_sim_pcscf_list pcscf_list;
		struct tel_sim_ist ist;
	} data;
};

struct tresp_sim_req_authentication {
	enum tel_sim_access_result result;
	enum tel_sim_auth_type auth_type; /**< authentication type */
	enum tel_sim_auth_result auth_result; /**< authentication result */
	unsigned int resp_length; /**< response length. IMS and 3G case, it stands for RES_AUTS. GSM case, it stands for SRES. */
	char resp_data[SIM_AUTH_RESP_DATA_LEN_MAX + 1]; /**< response data. IMS and 3G case, it stands for RES_AUTS. GSM case, it stands for SRES. */
	unsigned int authentication_key_length; /**< the length of authentication key, Kc*/
	char authentication_key[SIM_AUTH_RESP_DATA_LEN_MAX + 1]; /**< the data of of authentication key, Kc*/
	unsigned int cipher_length; /**< the length of cipher key length */
	char cipher_data[SIM_AUTH_RESP_DATA_LEN_MAX + 1]; /**< cipher key */
	unsigned int integrity_length; /**< the length of integrity key length */
	char integrity_data[SIM_AUTH_RESP_DATA_LEN_MAX + 1]; /**< integrity key */
};

struct tresp_sim_set_provisioning {
	enum tel_sim_access_result result;
};

struct tnoti_sim_status {
	enum tel_sim_status sim_status;
	gboolean b_changed;
};

struct tnoti_sim_refreshed {
	struct tel_sim_file_list file_list;
	gboolean b_full_file_changed;
	enum tel_sim_refresh_command cmd_type;
};

struct tnoti_sim_call_forward_state {
	gboolean b_forward;
};

__END_DECLS

#endif
