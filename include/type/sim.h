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
#define SIM_SST_LEN_MAX 20 //telephony defined
#define SIM_SST_SERVICE_CNT_MAX 56 //3gpp
#define SIM_UST_BYTE_LEN_MAX 8 //telephony defined
#define SIM_SPN_LEN_MAX 16 //3gpp
#define SIM_SPDI_PLMN_MAX 41 //telephony defined
#define SIM_XDN_ALPHA_ID_LEN_MAX 30 //telephony defined
#define SIM_XDN_NUMBER_LEN_MAX 20 //telephony defined
#define SIM_ECC_BYTE_LEN_MAX 3 //3gpp
#define SIM_ECC_STRING_LEN_MAX 50 //telephony defined
#define SIM_ECC_RECORD_CNT_MAX 15 //telephony defined
#define SIM_CF_RECORD_CNT_MAX 2 //telephony defined
#define SIM_MSISDN_RECORD_CNT_MAX 10 //telephony defined
#define SIM_GROUP_IDENTIFIER_LEN_MAX 10 //telephony defined
#define SIM_MAIL_BOX_IDENTIFIER_LEN_MAX 5 //3gpp
#define SIM_MSP_CNT_MAX 2 //telephony defined
#define SIM_OPL_PNN_RECORD_CNT_MAX 5//85 //telephony defined
#define SIM_NW_FULL_NAME_LEN_MAX 40 //telephony defined TODO should be defined one value in all.
#define SIM_OPLMNWACT_LEN_MAX 100 //telephony defined
#define SIM_AUTH_REQ_DATA_LEN_MAX 256 //telephony defined
#define SIM_AUTH_RESP_DATA_LEN_MAX 128 //telephony defined
#define SIM_CPHS_CALL_FORWARDING_LEN_MAX 2 //telephony defined
#define SIM_CPHS_CSP_ENTRY_CNT_MAX 11 //telephony defined
#define SIM_CPHS_CSP_LEN_MAX 22 //telephony defined
#define SIM_CPHS_VMWF_LEN_MAX 2 //telephony defined
#define SIM_CPHS_OPERATOR_NAME_LEN_MAX 25 //telephony defined
#define SIM_CPHS_OPERATOR_NAME_SHORT_FORM_LEN_MAX 10 //CPHS spec
#define SIM_CPHS_INFO_LEN_MAX 3 //telephony defined

enum tel_sim_file_id {
	SIM_EF_DIR = 0x2F00, /**< Root Directory for the USIM */
	SIM_EF_ICCID = 0x2FE2, /**< the ICC Identification file	*/
	SIM_EF_IMSI = 0x6F07, /**< the IMSI file */
	SIM_EF_SST = 0x6F38, /**< the SIM Service Table file. EF UST has same address */
	SIM_EF_EST = 0x6F56, /**< the Enabled Service Table file   */
	SIM_EF_OPLMN_ACT = 0x6F61, /**< the OPLMN List file*/
	SIM_EF_GID1 = 0x6F3E, /**< the Group Identifier Level 1 */
	SIM_EF_GID2 = 0x6F3F, /**< the Group Identifier Level 2 */

	SIM_EF_ELP = 0x2F05, /**< the Extended Language Preference file */
	SIM_EF_LP = 0x6F05, /**< SIM: Language preference */
	SIM_EF_ECC = 0x6FB7, /**< the Emergency Call Codes     */
	SIM_EF_SPN = 0x6F46, /**< the Service Provider Name    */
	SIM_EF_SPDI = 0x6FCD, /**< the Service provider display information*/
	SIM_EF_PNN = 0x6FC5, /**< the PLMN Network Name File*/
	SIM_EF_OPL = 0x6FC6, /**< the Operator PLMN List File*/
	SIM_EF_MBDN = 0x6FC7, /** < SIM Mail Box Dialing Number file */
	SIM_EF_MSISDN = 0x6F40, /**< MSISDN */

	SIM_EF_USIM_LI = 0x6A05, /**< USIM: Language Indication */
	SIM_EF_USIM_PL = 0x2A05, /**< the Language Preference file of USIM */
	SIM_EF_USIM_MBI = 0x6FC9, /** < Mailbox Identifier -linear fixed*/
	SIM_EF_USIM_MWIS = 0x6FCA, /** < Message Waiting Indication Status -linear fixed*/
	SIM_EF_USIM_CFIS = 0x6FCB, /** < Call forward indication status -linear fixed*/

	/* CPHS FILE ID */
	SIM_EF_CPHS_VOICE_MSG_WAITING = 0x6F11, /** < CPHS voice MSG waiting indication  */
	SIM_EF_CPHS_SERVICE_STRING_TABLE = 0x6F12, /** < CPHS service string table  */
	SIM_EF_CPHS_CALL_FORWARD_FLAGS = 0x6F13, /** < CPHS call forward flags  */
	SIM_EF_CPHS_OPERATOR_NAME_STRING = 0x6F14, /** < CPHS operator name string  */
	SIM_EF_CPHS_CUSTOMER_SERVICE_PROFILE = 0x6F15, /** < CPHS customer service profile  */
	SIM_EF_CPHS_CPHS_INFO = 0x6F16, /** < CPHS information  */
	SIM_EF_CPHS_MAILBOX_NUMBERS = 0x6F17, /** < CPHS mail box numbers  */
	SIM_EF_CPHS_OPERATOR_NAME_SHORT_FORM_STRING = 0x6F18, /** < CPHS operator name short form string  */
	SIM_EF_CPHS_INFORMATION_NUMBERS = 0x6F19, /** < CPHS information numbers  */
	/*  CPHS ALS FILE ID */
	SIM_EF_CPHS_DYNAMICFLAGS = 0x6F9F, /** < CPHS Dynamics flags  */
	SIM_EF_CPHS_DYNAMIC2FLAG = 0x6F92, /** < CPHS Dynamics2 flags  */
	SIM_EF_CPHS_CUSTOMER_SERVICE_PROFILE_LINE2 = 0x6F98, /** < CPHS CSP2  */

	/* Invalid File ID, All the file ID are less than this Value*/
	SIM_EF_INVALID = 0xFFFF, /**< Invalid file.*/
	SIM_EF_OTHERS, /**< Element to indicate an unknown file.*/
};

enum tel_sim_type {
	SIM_TYPE_UNKNOWN, /**< Unknown card */
	SIM_TYPE_GSM, /**< 2G GSM card*/
	SIM_TYPE_USIM, /**< 3G USIM card */
	SIM_TYPE_RUIM, /** CDMA based card*/
	SIM_TYPE_ISIM /** IMS based card */
};

enum tel_sim_status {
	SIM_STATUS_CARD_ERROR = 0x00, /**< Bad card / On the fly SIM gone bad **/
	SIM_STATUS_CARD_NOT_PRESENT = 0x01, /**<  Card not present **/
	SIM_STATUS_INITIALIZING = 0x02, /**<  SIM is Initializing state **/
	SIM_STATUS_INIT_COMPLETED = 0x03, /**<  SIM Initialization OK **/
	SIM_STATUS_PIN_REQUIRED = 0x04, /**<  PIN  required state **/
	SIM_STATUS_PUK_REQUIRED = 0x05, /**<  PUK required state **/
	SIM_STATUS_CARD_BLOCKED = 0x06, /**<  PIN/PUK blocked(permanently blocked- All the attempts for PIN/PUK failed) **/
	SIM_STATUS_NCK_REQUIRED = 0x07, /**<  Network Control Key required state **/
	SIM_STATUS_NSCK_REQUIRED = 0x08, /**<  Network Subset Control Key required state **/
	SIM_STATUS_SPCK_REQUIRED = 0x09, /**<  Service Provider Control Key required state **/
	SIM_STATUS_CCK_REQUIRED = 0x0a, /**<  Corporate Control Key required state **/
	SIM_STATUS_CARD_REMOVED = 0x0b, /**<  Card removed **/
	SIM_STATUS_LOCK_REQUIRED = 0x0c, /**<  PH-SIM (phone-SIM) locked state **/
	SIM_STATUS_UNKNOWN = 0xff /**<  unknown state. not checked yet. **/
};

enum tel_sim_facility_status {
	SIM_FACILITY_STATUS_DISABLED = 0x00, /**< PIN1 or PIN2 Disabled */
	SIM_FACILITY_STATUS_ENABLED = 0x01, /**< PIN1 or PIN2 Enabled */
	SIM_FACILITY_STATUS_BLOCKED = 0x02, /**< SIM is present, but PIN1 or PIN2  is blocked. need unblocking by PUK or PUK2 */
	SIM_FACILITY_STATUS_PUK_BLOCKED = 0x03, /**< SIM is present, but PUK is blocked. */
	SIM_FACILITY_STATUS_UNKNOWN = 0xFF /**< SIM is in unknown state */
};

enum tel_sim_access_result {
	SIM_ACCESS_SUCCESS, /**< Access to file successful.  */
	SIM_ACCESS_CARD_ERROR, /**< SIM card error    */
	SIM_ACCESS_FILE_NOT_FOUND, /**< File not found  */
	SIM_ACCESS_CONDITION_NOT_SATISFIED, /**< Access condition is not fulfilled  */
	SIM_ACCESS_FAILED, /**< Access failed.  */
};

enum tel_sim_pin_operation_result {
	SIM_PIN_OPERATION_SUCCESS, /**< Operation involving PIN (verification/change/enable/disable, etc) is successful.  */
	SIM_BUSY, /**< SIM is busy  */
	SIM_CARD_ERROR, /**< SIM card error - Permanently blocked and general errors   */
	SIM_INCOMPATIBLE_PIN_OPERATION, /**< SIM Incompatible pin operation that is in case when invalid SIM command is given or incorrect parameters are supplied to the SIM. */
	SIM_INCORRECT_PASSWORD, /**< SIM PIN  Incorrect password */
	SIM_PIN_REQUIRED, /**< PIN Required */
	SIM_PUK_REQUIRED, /**< PUK Required */
	SIM_NCK_REQUIRED, /**< Network Control Key Required */
	SIM_NSCK_REQUIRED, /**< Network Subset Control Key Required */
	SIM_SPCK_REQUIRED, /**< Service Provider Control Key Required */
	SIM_CCK_REQUIRED, /**< Corporate Control Key Required */
	SIM_LOCK_REQUIRED, /**<  PH-SIM (phone-SIM) locked state **/
	SIM_ADM_REQUIRED /**<  Admin key locked state **/
};

enum tel_sim_language_type {
	SIM_LANG_GERMAN = 0x00, /**< German */
	SIM_LANG_ENGLISH = 0x01, /**< English */
	SIM_LANG_ITALIAN = 0x02, /**< Italian */
	SIM_LANG_FRENCH = 0x03, /**< French */
	SIM_LANG_SPANISH = 0x04, /**< Spanish */
	SIM_LANG_DUTCH = 0x05, /**< Dutch */
	SIM_LANG_SWEDISH = 0x06, /**< Swedish */
	SIM_LANG_DANISH = 0x07, /**< Danish */
	SIM_LANG_PORTUGUESE = 0x08, /**< Portuguese */
	SIM_LANG_FINNISH = 0x09, /**< Finnish */
	SIM_LANG_NORWEGIAN = 0x0A, /**< Norwegian */
	SIM_LANG_GREEK = 0x0B, /**< Greek */
	SIM_LANG_TURKISH = 0x0C, /**< Turkish */
	SIM_LANG_HUNGARIAN = 0x0D, /**< Hungarian */
	SIM_LANG_POLISH = 0x0E, /**< Polish */
	SIM_LANG_KOREAN = 0x0F, /**< Korean */
	SIM_LANG_CHINESE = 0x10, /**< Chinese */
	SIM_LANG_RUSSIAN = 0x11, /**< Russian */
	SIM_LANG_JAPANESE = 0x12, /**< Japanese */
	SIM_LANG_UNSPECIFIED = 0xFF /**< Unspecified */
};

enum tel_sim_msgwaiting_idication_mask {
	SIM_MWIS_NONE = 0x00, /**< MWIS none*/
	SIM_MWIS_VOICE = 0x01, /**< MWIS voice*/
	SIM_MWIS_FAX = 0x02, /**< MWIS FAX*/
	SIM_MWIS_EMAIL = 0x04, /**< MWIS email*/
	SIM_MWIS_OTHER = 0x08, /**< MWIS other*/
	SIM_MWIS_VIDEO = 0x10, /**< MWIS video*/
	SIM_MWIS_RFU = 0xff /**< MWIS RFU*/
};

enum tel_sim_pin_type {
	SIM_PTYPE_PIN1 = 0x00, /**< PIN 1 code */
	SIM_PTYPE_PIN2 = 0x01, /**< PIN 2 code */
	SIM_PTYPE_PUK1 = 0x02, /**< PUK 1 code */
	SIM_PTYPE_PUK2 = 0x03, /**< PUK 2 code */
	SIM_PTYPE_UPIN = 0x04, /**< Universal PIN - Unused now */
	SIM_PTYPE_ADM = 0x05, /**< Administrator - Unused now */
	SIM_PTYPE_SIM = 0x06 /**< SIM Lock code */
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
	SIM_LOCK_STATUS_UNLOCKED,
	SIM_LOCK_STATUS_PIN,
	SIM_LOCK_STATUS_PUK,
	SIM_LOCK_STATUS_PIN2,
	SIM_LOCK_STATUS_PUK2,
	SIM_LOCK_STATUS_PERM_BLOCKED,
};

enum tel_sim_cphs_dynamic_flag_selected_line {
	SIM_DYNAMIC_FLAGS_LINE1 = 0x01, /**< line 1 */
	SIM_DYNAMIC_FLAGS_LINE2 = 0x00, /**< line 2*/
	SIM_DYNAMIC_FLAGS_RFU = 0xff /**< rfu*/
};

enum tel_sim_cphs_dynamic2_flag_als_status {
	SIM_PIN2_ACCESSIBLE_FLAGS_LOCKED = 0x01, /**< Dynamic flags locked */
	SIM_PIN2_ACCESSIBLE_FLAGS_UNLOCKED = 0x00, /**< Dynamic flags unlocked */
	SIM_PIN2_ACCESSIBLE_FLAGS_RFU = 0xff /**< rfu */
};

enum tel_sim_cphs_phase {
	SIM_CPHS_PHASE1 = 0x01, /**< phase1  */
	SIM_CPHS_PHASE2 = 0x02, /**< phase2  */
	SIM_CPHS_PHASE_RFU = 0xff /**< RFU  */
};

enum tel_sim_ton {
	SIM_TON_UNKNOWN = 0, /**< unknown */
	SIM_TON_INTERNATIONAL = 1, /**< international number */
	SIM_TON_NATIONAL = 2, /**< national number */
	SIM_TON_NETWORK_SPECIFIC = 3, /**< network specific number */
	SIM_TON_DEDICATED_ACCESS = 4, /**< subscriber number */
	SIM_TON_ALPHA_NUMERIC = 5, /**< alphanumeric, GSM 7-bit default alphabet) */
	SIM_TON_ABBREVIATED_NUMBER = 6, /**< abbreviated number */
	SIM_TON_RESERVED_FOR_EXT = 7 /**< reserved for extension */
};

enum tel_sim_npi {
	SIM_NPI_UNKNOWN = 0, /**< Unknown */
	SIM_NPI_ISDN_TEL = 1, /**< ISDN/Telephone numbering plan */
	SIM_NPI_DATA_NUMBERING_PLAN = 3, /**< Data numbering plan */
	SIM_NPI_TELEX = 4, /**< Telex numbering plan */
	SIM_NPI_SVC_CNTR_SPECIFIC_PLAN = 5, /**< Service Center Specific plan */
	SIM_NPI_SVC_CNTR_SPECIFIC_PLAN2 = 6, /**< Service Center Specific plan */
	SIM_NPI_NATIONAL = 8, /**< National numbering plan */
	SIM_NPI_PRIVATE = 9, /**< Private numbering plan */
	SIM_NPI_ERMES_NUMBERING_PLAN = 10, /**< ERMES numbering plan */
	SIM_NPI_RESERVED_FOR_EXT = 0xF /**< Reserved for extension */
};

enum tel_sim_emergency_service_type {
	SIM_ECC_POLICE = 0x01, /**< Police */
	SIM_ECC_AMBULANCE = 0x02, /**< Ambulance */
	SIM_ECC_FIREBRIGADE = 0x04, /**< Fire brigade */
	SIM_ECC_MARAINEGUARD = 0x08, /**< Marine guard */
	SIM_ECC_MOUTAINRESCUE = 0x10, /**< Mountain rescue */
	SIM_ECC_SPARE = 0x00 /**< Spare */
};

enum tel_sim_mailbox_type {
	SIM_MAILBOX_VOICE = 0x01, /**< voice */
	SIM_MAILBOX_FAX = 0x02, /**< fax*/
	SIM_MAILBOX_DATA = 0x03, /**< data*/
	SIM_MAILBOX_EMAIL = 0x04, /**< email*/
	SIM_MAILBOX_OTHER = 0x05, /**< other*/
};

enum tel_sim_cphs_csp_group {
	SIM_CPHS_CSP_GROUP_CALL_OFFERING = 0x01, /**< Group CSP offering*/
	SIM_CPHS_CSP_GROUP_CALL_RESTRICTION = 0x02, /**< Group CSP restriction*/
	SIM_CPHS_CSP_GROUP_OTHER_SUPP_SERVICES = 0x03, /**< Group CSP supplementary services*/
	SIM_CPHS_CSP_GROUP_CALL_COMPLETION = 0x04, /**< Group CSP completion*/
	SIM_CPHS_CSP_GROUP_TELESERVICES = 0x05, /**< Group CSP teleservices*/
	SIM_CPHS_CSP_GROUP_CPHS_TELESERVICES = 0x06, /**< Group CSP CPHS teleservies*/
	SIM_CPHS_CSP_GROUP_CPHS_FEATURES = 0x07, /**< Group CSP CPHS features*/
	SIM_CPHS_CSP_GROUP_NUMBER_IDENTIFIERS = 0x08, /**< Group CSP number identifiers*/
	SIM_CPHS_CSP_GROUP_PHASE_SERVICES = 0x09, /**< Group CSP phase services*/
	SIM_CPHS_CSP_GROUP_VALUE_ADDED_SERVICES = 0xC0, /**< Group CSP value added services*/
	SIM_CPHS_CSP_GROUP_INFORMATION_NUMBERS = 0xD5 /**< Group CSP information numbers*/
};

enum tel_sim_cphs_index_level {
	SIM_CPHS_INDEX_LEVEL_ONE = 0x01, /**< SIM CPHS index level one */
	SIM_CPHS_INDEX_LEVEL_TWO = 0x02, /**< SIM CPHS index level two */
	SIM_CPHS_INDEX_LEVEL_THREE = 0x03, /**< SIM CPHS index level three */
	SIM_CPHS_INDEX_LEVEL_RFU = 0xff /**< SIM CPHS index level RFU */
};

enum tel_sim_auth_type {
	SIM_AUTH_TYPE_IMS = 0x00, /**< IMS Authentication */
	SIM_AUTH_TYPE_GSM, /**< GSM Authentication */
	SIM_AUTH_TYPE_3G, /**< 3G Authentication */
	SIM_AUTH_TYPE_MAX /**< TBD */
};

enum tel_sim_auth_result {
	SIM_AUTH_NO_ERROR = 0x00, /**< ISIM no error */
	SIM_AUTH_CANNOT_PERFORM, /**< status - can't perform authentication */
	SIM_AUTH_SKIP_RESPONSE, /**< status - skip authentication response */
	SIM_AUTH_MAK_CODE_FAILURE, /**< status - MAK(Multiple Activation Key) code failure */
	SIM_AUTH_SQN_FAILURE, /**< status - SQN(SeQuenceNumber) failure */
	SIM_AUTH_SYNCH_FAILURE, /**< status - synch failure */
	SIM_AUTH_UNSUPPORTED_CONTEXT, /**< status - unsupported context */
	SIM_AUTH_MAX /**< TBD */
};

struct tel_sim_sst {
	// service 1
	char bPIN1DisableFunction; /**< PIN1 disable function */
	char bAdn; /**< abbreviated Dialing number */
	char bFdn; /**< fixed Dialing number */
	char bSms; /**< short message storage */
	char bAoc; /**< advice of charge */
	char bCcp; /**< capability configuration parameters */
	char bPlmnSelector; /**< plmn selector */
	char bRfu1; /**< rfu */

	// service 9
	char bMsisdn; /**< msisdn */
	char bExtension1; /**< extension1	*/
	char bExtension2; /**< extension2 */
	char bSmsParams; /**< sms parameteres */
	char blnd; /**< last number dialed */
	char bCellBroadCastMsgId; /**< cell broadcast message identifier */
	char bGroupIdLevel1; /**< group identifier level 1 */
	char bGroupIdLevel2; /**< group identifier level 2 */

	// service 17
	char bSpn; /**< service provider name */
	char bSdn; /**< service Dialing number */
	char bExtension3; /**< extension3 */
	char bRfu2; /**< rfu */
	char bVgcsGroupIdList; /**< vgcs group identifier (EF-VGCS, EF-VGCSS) */
	char bVbsGroupIDList; /**< vbs group identifier (EF-VBS, EF-VBSS) */
	char bEnhancedMultilevelPrecedencePreemptionService; /**< enhanced multi-level precedence and pre-emption service */
	char bAutoAnswerForeMLPP; /**< automatic answer fro eMLPP */

	// service 25
	char bSmsCbdDataDownload; /**< data download via sms-cb */
	char bSmsPpDataDownload; /**< data download via sms-pp */
	char bMenuSelection; /**< menu selection */
	char bCallControl; /**< call control */
	char bProactiveSimCommand; /**< proactive sim command */
	char bCellBroadcastMsgIdRanges; /**< cell broadcast message identifier ranges */
	char bBdn; /**< barred Dialing numbers */
	char bExtension4; /**< extension 4 */

	// service 33
	char bDePersonalizedControlKeys; /**< de-personalization control keys */
	char bCoOperativeNetworkList; /**< co-operative network list */
	char bSmsr; /**< short message status reports */
	char bNetworkAlertIndInMS; /**< network's indication of alerting in the MS */
	char bMoSMSControl; /**< mobile-originated short message control by sim */
	char bGPRS; /**< gprs */
	char bImage; /**< image */
	char bSolsa; /**< support of local service area */

	//	service 41
	char bUSSDSupportedInCallControl; /**< ussd string data object supported in call control */
	char bRunAtCommand; /**< RUN AT COMMAND command */
	char bUserControlledPLMNSelectorAccessTech; /**< user controlled PLMN selector with Access technology */
	char bOperatorControlledPlmnSelectorAccessTech; /**< operator controlled PLMN selector with Access technology */
	char bHplmnSelectorAccessTech; /**< HPLMN selector with access technology */
	char bCPBCCHInfo; /**< CPBCCH information */
	char bInvestigationScan; /**< investigation scan */
	char bExtenedCapaConfigParams; /**< extended capability configuration parameters */

	//	service 49
	char bMexe; /**< MExE */
	char bRplmnLastUsedAccessTech; /**< RPLMN last used access technology */
	char bPlmnNetworkName; /*PLMN Network Name*/
	char bOperatorPlmnList; /*Operator PLMN List*/
	char bMailboxDiallingNumber; /*Mailbox Dialling Numbers*/
	char bMessageWaitingIndicationStatus; /*Message Waiting Indication Status*/
	char bCallForwardingIndicationStatus; /*Call Forwarding Indication Status*/
	char bServiceProviderDisplayInformation; /*Service Provider Display Information*/
};

struct tel_sim_ust {
	// service 1
	char bLocalPhoneBook; /**< local phone book */
	char bFdn; /**< fixed Dialing number */
	char bExtension2; /**< extension 2 */
	char bSdn; /**< service Dialing number */
	char bExtension3; /**< extension 3 */
	char bBdn; /**< barred Dialing numbers */
	char bExtension4; /**< extension 4 */
	char bOCIOCT; /**< outgoing call information */

	//	service 9
	char bIciIct; /**< incoming call information */
	char bSms; /**< short message storage */
	char bSmsr; /**< short message status reports */
	char bSMSP; /**< short message service parameters */
	char bAoc; /**< advice of charge */
	char bCcp; /**< capability config parameters */
	char bCellBroadcastMsgId; /**< cell broadcast message identifier */
	char bCellBroadcastMsgIdRanges; /**< cell broadcast message identifier charge */

	//	sevice 17
	char bGroupIdLevel1; /**< group identifier level 1 */
	char bGroupIdLevel2; /**< group identifier level 2 */
	char bSpn; /**< service provide name */
	char bUserControlledPLMNSelectorAccessTech; /**< user controlled plmn selector with access technology */
	char bMsisdn; /**< msisdn */
	char bImage; /**< image */
	char bSolsa;
	char bEnhancedMultilevelPrecedencePreemptionService; /**< enhanced multi-level precedence and pre-emption service */

	//	service 25
	char bAutoAnswerForeMLPP; /**< automatic answer fro eMLPP */
	char bRfu1;
	char bGsmAccess; /**< gsm access */
	char bSmsPpDataDownload; /**< data download via sms-pp */
	char bSmsCbdDataDownload; /**< data download via sms-cb */
	char bUsimCallControl; /**< usim call control */
	char bUsimMoSmsControl; /**< mo-sms control by usim */
	char bRunAtCommand; /**< RUN AT COMMAND command */

	//	service 33
	char bShallBeSetToOne; /**< shall be set to 1 */
	char bEnabledServiceTable; /**< enabled service table */
	char bAcl; /**< APN control list */
	char bDePersonalizedControlKeys; /**< de-personalization control keys */
	char bCoOperativeNetworkList; /**< co-operative network list */
	char bGsmSecurityContext; /**< gsm security context */
	char bCpbcchiInformation; /**< cpbcch information */
	char bInvestigationScan; /**< investigation scan */

	//	service 41
	char bMexe; /**< mexe */
	char bOperatorControlledPlmnSelectorAccessTech; /**< Operator controlled PLMN selector with Access Technology */
	char bHplmnSelectorAccessTech; /**< HPLMN selector with access technology */
	char bExtension5; /**< extension 5 */
	char bPlmnNetworkName; /**< plmn network name */
	char bOperatorPlmnList; /**< operator plmn list */
	char bMailBoxDiallingNumbers; /**< mailbox Dialing numbers */
	char bMsgWaitingIndStatus; /**< message waiting indication status */

	//	service 49
	char bCallForwardingIndicationStatus; /**< call forwarding indication status */
	char bRplmnLastUsedAccessTech; /**< RPLMN last used access technology */
	char bServiceProviderDisplayInfo; /**< service provider display information */
	char bMms; /**< multi media messaging service */
	char bExtension8; /**< extension 8 */
	char bUsimGpsCallControl; /**< call control on gprs by usim */
	char bMmsUserConnectivityParameters; /**< mms user connectivity parameters */
	char bNia; /**< Network's indication of alerting in the MS (NIA) */

	//service 57
	char bVgcsIdList; /**< VGCS Group Identifier List (EFVGCS and EFVGCSS) */
	char bVbsGroupId; /**< VBS Group Identifier List (EFVBS and EFVBSS)*/
	char bPseudonym;
	char bUserCtrlPlmnSelectorForWlan;
	char bOperatorCtrlPlmnSelectorForWlan;
	char bUserCtrlWsidList;
	char bOperatorCtrlWsidList;
	char bVgcsSecurity;
};

struct tel_sim_service_table {
	struct tel_sim_sst sst;
	struct tel_sim_ust ust;
};

struct tel_sim_est {
	gboolean bFdnEnabled;
	gboolean bBdnEnabled;
	gboolean bAclEnabled;
};

struct tel_sim_imsi {
	char plmn[6 + 1]; /**< SIM MCC, MNC code. MNC is 2 digit or 3digit */
	char msin[10 + 1]; /**< Mobile Station Identification Number */
};

struct tel_sim_dialing_number {
	int alpha_id_max_len; /**< alpha max length in SIM - READ ONLY*/
	char alpha_id[SIM_XDN_ALPHA_ID_LEN_MAX + 1]; /**< Alpha Identifier */
	enum tel_sim_ton ton; /**< Type Of Number */
	enum tel_sim_npi npi; /**< Number Plan Identity */
	char num[SIM_XDN_NUMBER_LEN_MAX + 1]; /**< Dialing Number/SSC String */
	unsigned char cc_id; /**< Capability/Configuration Identifier */
	unsigned char ext1_id; /**< Extensiion1 Record Identifier */
};

struct tel_sim_gid {
	int GroupIdentifierLen; /**< Group identifier length */
	char szGroupIdentifier[SIM_GROUP_IDENTIFIER_LEN_MAX]; /**< Group identifier */
};

struct tel_sim_cphs_svc_call_offering_s {
	int bCallForwardingUnconditional; /**< CallForwarding Unconditional */
	int bCallForwardingOnUserBusy; /**< CallForwarding On UserBusy */
	int bCallForwardingOnNoReply; /**< CallForwarding On NoReply */
	int bCallForwardingOnUserNotReachable; /**< CallForwarding On User Not Reachable */
	int bCallTransfer; /**< Call Transfer */
};

struct tel_sim_cphs_svc_call_restriction_s {
	int bBarringOfAllOutgoingCalls; /**< Barring Of All Outgoing Calls*/
	int bBarringOfOutgoingInternationalCalls; /**< Barring Of Outgoing International Calls */
	int bBarringOfOutgoingInternationalCallsExceptHplmn; /**< Barring Of Outgoing International Calls Except HPLMN */
	int bBarringOfAllIncomingCallsRoamingOutsideHplmn; /**< Barring Of All Incoming Calls Roaming Outside HPLMN */
	int bBarringOfIncomingCallsWhenRoaming; /**< Barring Of IncomingCalls When Roaming */
};

struct tel_sim_cphs_svc_other_ss_s {
	int bMultiPartyService; /**< MultiPartyService*/
	int bClosedUserGroup; /**< ClosedUserGroup*/
	int bAdviceOfCharge; /**< AdviceOfCharge*/
	int bPreferentialClosedUserGroup; /**< PreferentialClosedUserGroup*/
	int bClosedUserGroupOutgoingAccess; /**< ClosedUserGroupOutgoingAccess*/
};

struct tel_sim_cphs_svc_call_complete_s {
	int bCallHold; /**< Call Hold*/
	int bCallWaiting; /**< Call Waiting*/
	int bCompletionOfCallToBusySubscriber; /**< Completion Of Call To Busy Subscriber*/
	int bUserUserSignalling; /**< User User Signaling*/
};

struct tel_sim_cphs_svc_teleservices_s {
	int bShortMessageMobileOriginated; /**< Short Message Mobile Originated*/
	int bShortMessageMobileTerminated; /**< Short Message Mobile Terminated*/
	int bShortMessageCellBroadcast; /**< Short Message Cell Broadcast*/
	int bShortMessageReplyPath; /**< Short  Message Reply Path*/
	int bShortMessageDeliveryConf; /**< Short Message Delivery Conf*/
	int bShortMessageProtocolIdentifier; /**< Short Message Protocol Identifier*/
	int bShortMessageValidityPeriod; /**< Short Message Validity Period*/
};

struct tel_sim_cphs_svc_cphs_teleservices_s {
	int bAlternativeLineService; /**< Alternative Line Service*/
};

struct tel_sim_cphs_svc_cphs_features_s {
	int bStringServiceTable; /**< String Service Table*/
};

struct tel_sim_cphs_svc_number_identifier_s {
	int bCallingLineIdentificationPresent; /**< Calling Line Identification Present*/
	int bConnectedLineIdentificationRestrict; /**< Connected Line Identification Restrict*/
	int bConnectedLineIdentificationPresent; /**< Connected Line Identification Present*/
	int bMaliciousCallIdentifier; /**< Malicious Call Identifier*/
	int bCallingLineIdentificationSend; /**< Calling Line Identification Send*/
	int bCallingLineIdentificationBlock; /**< Calling Line Identification Block*/
};

struct tel_sim_cphs_svc_phase_services_s {
	int bMenuForGprs; /**< Menu For group*/
	int bMenuForHighSpeedCsd; /**< Menu For HighSpeedCsd*/
	int bMenuForVoiceGroupCall; /**< Menu For VoiceGroupCall*/
	int bMenuForVoiceBroadcastService; /**< Menu For VoiceBroadcastService*/
	int bMenuForMultipleSubscriberProfile; /**< Menu For MultipleSubscriberProfile*/
	int bMenuForMultipleBand; /**< Menu For MultipleBand*/
};

struct tel_sim_cphs_svc_value_added_services_s {
	int bRestrictMenuForManualSelection; /**< RestrictMenu For ManualSelection*/
	int bRestrictMenuForVoiceMail; /**< RestrictMenu For VoiceMail*/
	int bRestrictMenuForMoSmsAndPaging; /**< RestrictMenu For MoSmsAndPaging*/
	int bRestrictMenuForMoSmsWithEmialType; /**< RestrictMenu For MoSmsWithEmialType*/
	int bRestrictMenuForFaxCalls; /**< RestrictMenu For FaxCalls*/
	int bRestrictMenuForDataCalls; /**< RestrictMenu For DataCalls*/
	int bRestrictMenuForChangeLanguage; /**< RestrictMenu For ChangeLanguage*/
};

struct tel_sim_cphs_svc_information_numbers_s {
	int bInformationNumbers; /**< Information Numbers*/
};

struct tel_sim_cphs_csp_entry_s {
	enum tel_sim_cphs_csp_group CustomerServiceGroup; /**< customer service group*/
	union {
		struct tel_sim_cphs_svc_call_offering_s CallOffering; /**< call offering*/
		struct tel_sim_cphs_svc_call_restriction_s CallRestriction; /**< call restriction*/
		struct tel_sim_cphs_svc_other_ss_s OtherSuppServices; /**< other SS services*/
		struct tel_sim_cphs_svc_call_complete_s CallComplete; /**< call complete*/
		struct tel_sim_cphs_svc_teleservices_s Teleservices; /**< teleservices*/
		struct tel_sim_cphs_svc_cphs_teleservices_s CphsTeleservices; /**< CPHS teleservices*/
		struct tel_sim_cphs_svc_cphs_features_s CphsFeatures; /**< CPHS features*/
		struct tel_sim_cphs_svc_number_identifier_s NumberIdentifiers; /**< number identifiers*/
		struct tel_sim_cphs_svc_phase_services_s PhaseServices; /**< phase services*/
		struct tel_sim_cphs_svc_value_added_services_s ValueAddedServices; /**< value added services*/
		struct tel_sim_cphs_svc_information_numbers_s InformationNumbers; /**< information numbers*/
	} u;
};

struct tel_sim_cphs_csp {
	struct tel_sim_cphs_csp_entry_s ServiceProfileEntry[SIM_CPHS_CSP_ENTRY_CNT_MAX]; /**< service profile entry*/
};

struct tel_sim_cphs_dflag {
	enum tel_sim_cphs_dynamic_flag_selected_line DynamicFlags; /**< Dynamic flags information */
};

struct tel_sim_cphs_dflag2 {
	enum tel_sim_cphs_dynamic2_flag_als_status Dynamic2Flag; /**< Dynamic flags status */
};

struct tel_sim_cphs_service_tablie_s {
	/* Byte 2 - bit1 & 2*/
	int bCustomerServiceProfile; /**< Customer Service Profile (CSP)  */
	/* Byte 2 - bit3 & 4*/
	int bServiceStringTable; /**< Service String Table (SST) */
	/* Byte 2 - bit5 & 6*/
	int bMailBoxNumbers; /**< MailBoxNumbers */
	/* Byte 2 - bit7 & 8*/
	int bOperatorNameShortForm; /**< Short form of operator name */
	/* Byte 3 - bit1 & 2*/
	int bInformationNumbers; /**< Information numbers */
};

struct tel_sim_cphs_info {
	enum tel_sim_cphs_phase CphsPhase; /**< CPHS phase type */
	struct tel_sim_cphs_service_tablie_s CphsServiceTable; /**< CPHS service table */
};

struct tel_sim_cphs_info_number {
	int AlphaIdLength; /**< length of alpha identifier */
	enum tel_sim_cphs_index_level IndexLevelIndicator; /**< SIM CPHS index level one */
	int PremiumServiceIndicator; /**< SIM CPHS index level one */
	int NetworkSpecificIndicator; /**< SIM CPHS index level one */
	unsigned char Alpha_id[SIM_XDN_ALPHA_ID_LEN_MAX + 1]; /**<  Alpha Identifier */
	unsigned long DiallingnumLength; /**< Length of BCD number/SSC contents */
	enum tel_sim_ton TypeOfNumber; /**< TON */
	enum tel_sim_npi NumberingPlanIdentity; /**< NPI */
	char DiallingNum[SIM_XDN_NUMBER_LEN_MAX + 1]; /**< Dialing Number/SSC String */
	unsigned char Ext1RecordId; /**< Extensiion1 Record Identifier */
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
	unsigned char apdu[256];
};

struct treq_sim_set_language {
	enum tel_sim_language_type language;
};

struct treq_sim_req_authentication {
	enum tel_sim_auth_type auth_type; /**< Authentication type */
	unsigned int rand_length; /**< the length of RAND */
	unsigned int autn_length; /**< the length of AUTN. it is not used in case of GSM AUTH */
	char rand_data[SIM_AUTH_REQ_DATA_LEN_MAX + 1]; /**< RAND data */
	char autn_data[SIM_AUTH_REQ_DATA_LEN_MAX + 1]; /**< AUTN data. it is not used in case of GSM AUTH */
};

struct tresp_sim_verify_pins {
	enum tel_sim_pin_operation_result result;
	enum tel_sim_pin_type pin_type;
	enum tel_sim_lock_status lock_status;
	int retry_count;
};

struct tresp_sim_verify_puks {
	enum tel_sim_pin_operation_result result;
	enum tel_sim_pin_type pin_type;
	enum tel_sim_lock_status lock_status;
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
	unsigned char apdu_resp[256 + 2];
};

struct tresp_sim_get_atr {
	enum tel_sim_access_result result;
	unsigned int atr_length;
	unsigned char atr[256 + 2];
};

struct tel_sim_ecc {
	char ecc_num[SIM_ECC_BYTE_LEN_MAX * 2 + 1]; /**< Emergency Call Code info-ECC is coded in BCD format. null termination used*/
	char ecc_string[SIM_ECC_STRING_LEN_MAX + 1]; /**< Alphabet identifier. null termination used*/
	enum tel_sim_emergency_service_type ecc_category; /**< ECC emergency service information */
};

struct tel_sim_ecc_list {
	int ecc_count;
	struct tel_sim_ecc ecc[SIM_ECC_RECORD_CNT_MAX];
};

struct tel_sim_language {
	int language_count;
	enum tel_sim_language_type language[SIM_LANG_CNT_MAX];
};

struct tresp_sim_set_data {
	enum tel_sim_access_result result;
};

struct tel_sim_iccid {
	char iccid[SIM_ICCID_LEN_MAX + 1];
};

struct tel_sim_mbi {
	/*	'00' - no mailbox dialing number associated with message waiting indication group type.
	 'xx' - record number in EFMBDN associated with message waiting indication group type.
	 */
	int voice_index;	/**< voice mailbox identifier*/
	int fax_index;	/**< FAX mailbox identifier*/
	int email_index;	/**< email mailbox identifier*/
	int other_index;	/**< other mailbox identifier*/
	int video_index;	/**< video mailbox identifier*/
};

struct tel_sim_mbi_list {
	int profile_count;
	struct tel_sim_mbi mbi[SIM_MSP_CNT_MAX];
};

struct tel_sim_mb_number{
	int rec_index; // SIM record index;
	int profile_number; // Subscriber profile number(ex: line1, line2, lineX)
	enum tel_sim_mailbox_type mb_type;
	struct tel_sim_dialing_number number_info;
};

struct tel_sim_mailbox {
	gboolean b_cphs;
	int count;
	struct tel_sim_mb_number mb[SIM_MSP_CNT_MAX*5]; //each profile mailbox number can exist 5 numbers
};

struct treq_sim_set_mailbox {
	gboolean b_cphs;
	struct tel_sim_mb_number mb_info;
};

struct tel_sim_cfis {
	int rec_index;
	unsigned char msp_num; /**< MSP number*/
	unsigned char cfu_status; /**< call forwarding unconditional indication status*/
	enum tel_sim_ton ton; /**< TON*/
	enum tel_sim_npi npi; /**< NPI*/
	char cfu_num[SIM_XDN_NUMBER_LEN_MAX + 1];/**< Dialing Number/SSC String*/
	unsigned char cc2_id; /**< Capability/Configuration2 Record Identifier */
	unsigned char ext7_id; /**< Extension 7 Record Identifier */
};

struct tel_sim_cf_list {
	int profile_count;
	struct tel_sim_cfis cf[SIM_MSP_CNT_MAX];
};

struct tel_sim_cphs_cf {
	gboolean b_line1; /**< CallForwardUnconditionalLine 1 */
	gboolean b_line2; /**< CallForwardUnconditionalLine 2 */
	gboolean b_fax; /**< CallForwardUnconditional FAX */
	gboolean b_data; /**<CallForwardUnconditional data*/
};

struct tel_sim_callforwarding {
	gboolean b_cphs;
	struct tel_sim_cf_list cf_list;
	struct tel_sim_cphs_cf cphs_cf;
};

struct treq_sim_set_callforwarding {
	gboolean b_cphs;
	struct tel_sim_cfis cf;
	struct tel_sim_cphs_cf cphs_cf;
};

struct tel_sim_mw {
	int rec_index;
	unsigned char indicator_status; /**< Indicator status*/
	int voice_count; /**< VoiceMail Count*/
	int fax_count; /**< FAX Count*/
	int email_count; /**< Email Count*/
	int other_count; /**< Other Count*/
	int video_count; /**< VideoMail Count*/
};

struct tel_sim_mw_list {
	int profile_count;
	struct tel_sim_mw mw[SIM_MSP_CNT_MAX];
};

struct tel_sim_cphs_mw {
	gboolean b_voice1; /**< VoiceMsgLine1 message waiting flag */
	gboolean b_voice2; /**< VoiceMsgLine2 message waiting flag */
	gboolean b_fax; /**< FAX message waiting flag */
	gboolean b_data; /**< Data message waiting flag */
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
	unsigned char num[SIM_XDN_NUMBER_LEN_MAX + 1];
	enum tel_sim_ton ton;
	unsigned char name[SIM_XDN_ALPHA_ID_LEN_MAX + 1];
};

struct tel_sim_msisdn_list {
	int count;
	struct tel_sim_msisdn msisdn[SIM_MSISDN_RECORD_CNT_MAX];
};

struct tel_sim_spn {
	unsigned char display_condition; /**< display condition (1 byte) */
	unsigned char spn[SIM_SPN_LEN_MAX + 1]; /**< SPN */
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
	struct tel_sim_opl opl[SIM_OPL_PNN_RECORD_CNT_MAX];
};

struct tel_sim_pnn{
	unsigned char full_name[SIM_NW_FULL_NAME_LEN_MAX + 1];
	unsigned char short_name[SIM_NW_FULL_NAME_LEN_MAX + 1];
};

struct tel_sim_pnn_list {
	int pnn_count;
	struct tel_sim_pnn pnn[SIM_OPL_PNN_RECORD_CNT_MAX];
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
		struct tel_sim_msisdn_list msisdn_list;
		struct tel_sim_spn spn;
		struct tel_sim_spdi spdi;
		struct tel_sim_opl_list opl;
		struct tel_sim_pnn_list pnn;
		struct tel_sim_cphs_netname cphs_net;
		struct tel_sim_oplmnwact_list opwa;
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

struct tnoti_sim_status {
	enum tel_sim_status sim_status;
	gboolean b_changed;
};

__END_DECLS

#endif
