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

#ifndef __TEL_SIM_H__
#define __TEL_SIM_H__

#include <tel_if.h>

/**
*  @addtogroup TAPI_SIM
*  @{
*
*  @file tel_sim.h
*  @brief TAPI sim Interface
*/

#ifdef __cplusplus
extern "C"
{
#endif

#define TELEPHONY_SIM_INTERFACE                 TELEPHONY_SERVICE".Sim"

#define TEL_SIM_ICCID_LEN_MAX			20	/**< Maximum ICCID Length */
#define TEL_SIM_MSIN_LEN_MAX			10	/**< Mobile Station Identification Number max length */
#define TEL_SIM_MCC_MNC_LEN_MAX			3	/**< MCC/MNC max length */

#define TEL_SIM_PLMN_LEN_MAX			6	/**< PLMN max length */
#define TEL_SIM_SPN_LEN_MAX			16	/**< Service Provider Name max length */

#define TEL_SIM_ECC_NUMBER_LEN_MAX		6	/**< Emergency code max length */
#define TEL_SIM_ECC_LIST_MAX			16	/**< Emergency list max items */
#define TEL_SIM_ECC_TEXT_MAX			255	/**< Emergency alpha identifier max length */

#define TEL_SIM_EXT_RECORD_CNT_MAX 15  /** EXT record count max length */
#define TEL_SIM_OPL_PNN_RECORD_CNT_MAX 5 /* OPL PNN record count max  */
#define TEL_SIM_NW_FULL_NAME_LEN_MAX 40 /* Full name length max */
#define TEL_SIM_MBDN_NUM_LEN_MAX		20	/**< Max length of the Number supported*/
#define TEL_SIM_ALPHA_ID_LEN_MAX		241	/**< Max length of the Alpha ID supported*/

#define TEL_SIM_MSP_CNT_MAX			4	/**< The Profile Identity shall be between 1and 4 as defined in TS 23.097 for MSP.*/

#define TEL_SIM_AUTH_MAX_REQ_DATA_LEN		16	/**< Authentication code max length */
#define TEL_SIM_AUTH_MAX_RESP_DATA_LEN		16	/**< Authentication response data max length */

#define TEL_SIM_APDU_LEN_MAX			261	/**< APDU max command length */
#define TEL_SIM_APDU_RESP_LEN_MAX		258	/**< APDU max response length */
#define TEL_SIM_ATR_LEN_MAX			33	/**< Answer to Reset data max length */

#define TEL_SIM_MAILBOX_TYPE_MAX		5	/**< Mailbox type count */
#define TEL_SIM_CALL_FORWARDING_TYPE_MAX		5	/** Call forwarding type count */

/**
 * @enum TelSimResult
 * Response (Return) values.
 */
typedef enum {
	TEL_SIM_RESULT_SUCCESS, /**< Operation completed successfully */
	TEL_SIM_RESULT_FAILURE, /**< Operation Failed */
	TEL_SIM_RESULT_INVALID_PARAMETER, /**< Invalid input parameters */
	TEL_SIM_RESULT_MEMORY_FAILURE, /**< Memory allocation failed */
	TEL_SIM_RESULT_OPERATION_NOT_SUPPORTED, /**< Operation not supported */

	TEL_SIM_RESULT_CARD_ERROR,  /**< SIM card error - Permanently blocked and general errors   */
	TEL_SIM_RESULT_ACCESS_CONDITION_NOT_SATISFIED, /**< SIM Incompatible pin operation that is in case when invalid SIM command is given or incorrect parameters are supplied to the SIM. */
	TEL_SIM_RESULT_INCOMPATIBLE_PIN_OPERATION, /**< SIM PIN  Incorrect password */
	TEL_SIM_RESULT_INCORRECT_PASSWORD, /**< SIM Incorrect password */
	TEL_SIM_RESULT_PIN_REQUIRED, /**< PIN Required */
	TEL_SIM_RESULT_PUK_REQUIRED, /**< PUK Required */
	TEL_SIM_RESULT_PERM_BLOCKED, /**< Permanent block SIM */
	TEL_SIM_RESULT_SQN_FAILURE, /**< status - SQN(SeQuenceNumber) failure */
	TEL_SIM_RESULT_OPERATION_NOT_PERMITTED	/**< Operation not permitted */
} TelSimResult;

/**
 * @enum TelSimCardType
 * This enumeration defines the card type.
 */
typedef enum {
	TEL_SIM_CARD_TYPE_UNKNOWN,	/**< Unknown card */
	TEL_SIM_CARD_TYPE_GSM,		/**< 2G SIM(GSM) card */
	TEL_SIM_CARD_TYPE_USIM,		/**< 3G USIM card */
	TEL_SIM_CARD_TYPE_RUIM,		/**< CDMA based card */
	TEL_SIM_CARD_TYPE_ISIM		/**< IMS based card */
} TelSimCardType;

typedef enum {
	TEL_SIM_MWIS_NONE = 0x00, /**< MWIS none*/
	TEL_SIM_MWIS_VOICE = 0x01, /**< MWIS voice*/
	TEL_SIM_MWIS_FAX = 0x02, /**< MWIS FAX*/
	TEL_SIM_MWIS_EMAIL = 0x04, /**< MWIS email*/
	TEL_SIM_MWIS_OTHER = 0x08, /**< MWIS other*/
	TEL_SIM_MWIS_VIDEO = 0x10, /**< MWIS video*/
	TEL_SIM_MWIS_RFU = 0xff /**< MWIS RFU*/
} TelSimMwisMask;

/** * @enum TelSimFacilityStatus
 * This enumeration defines the pin status.
 */
typedef enum {
	TEL_SIM_FACILITY_STATUS_UNKNOWN = -1, /**< SIM facility UNKNOWN status*/
	TEL_SIM_FACILITY_STATUS_DISABLED, /**< SIM facility DISABLED status*/
	TEL_SIM_FACILITY_STATUS_ENABLED /**< SIM facility ENABLED status*/
} TelSimFacilityStatus;

/**
 * @enum TelSimPinType
 * This enumeration defines the pin type.
 */
typedef enum {
	TEL_SIM_PIN_TYPE_PIN1, /**< PIN 1 code */
	TEL_SIM_PIN_TYPE_PIN2 /**< PIN 2 code */
} TelSimPinType;

/**
 * @enum TelSimPukType
 * This enumeration defines the puk type.
 */
typedef enum {
	TEL_SIM_PUK_TYPE_PUK1, /**< PUK 1 code */
	TEL_SIM_PUK_TYPE_PUK2 /**< PUK 2 code */
} TelSimPukType;

/**
 * @enum TelSimEccEmergencyServiceInfo
 * This enumeration defines the emergency service type.
 */
typedef enum {
	TEL_SIM_ECC_ESC_DEFAULT =		0x00, /**< DEFAULT */
	TEL_SIM_ECC_ESC_POLICE =		0x01, /**< Police */
	TEL_SIM_ECC_ESC_AMBULANCE =		0x02, /**< Ambulance */
	TEL_SIM_ECC_ESC_FIREBRIGADE =		0x04, /**< Fire brigade */
	TEL_SIM_ECC_ESC_MARINEGUARD =		0x08, /**< Marine guard */
	TEL_SIM_ECC_ESC_MOUNTAINRESCUE =	0x10, /**< Mountain rescue */
	TEL_SIM_ECC_ESC_MANUAL_ECALL =		0x20, /**< Manual Emergency call */
	TEL_SIM_ECC_ESC_AUTO_ECALL =		0x40 /**< Automatic Emergency call */
} TelSimEccEmergencyServiceInfo;

/**
 * @enum TelSimLanguagePreferenceCode
 * This enumeration defines the language indication code.
 */
typedef enum {
	TEL_SIM_LP_GERMAN =		0x00, /**< German */
	TEL_SIM_LP_ENGLISH =		0x01, /**< English */
	TEL_SIM_LP_ITALIAN =		0x02, /**< Italian */
	TEL_SIM_LP_FRENCH =		0x03, /**< French */
	TEL_SIM_LP_SPANISH =		0x04, /**< Spanish */
	TEL_SIM_LP_DUTCH =		0x05, /**< Dutch */
	TEL_SIM_LP_SWEDISH =		0x06, /**< Swedish */
	TEL_SIM_LP_DANISH =		0x07, /**< Danish */
	TEL_SIM_LP_PORTUGUESE =		0x08, /**< Portuguese */
	TEL_SIM_LP_FINNISH =		0x09, /**< Finnish */
	TEL_SIM_LP_NORWEGIAN =		0x0A, /**< Norwegian */
	TEL_SIM_LP_GREEK =		0x0B, /**< Greek */
	TEL_SIM_LP_TURKISH =		0x0C, /**< Turkish */
	TEL_SIM_LP_HUNGARIAN =		0x0D, /**< Hungarian */
	TEL_SIM_LP_POLISH =		0x0E, /**< Polish */
	TEL_SIM_LP_KOREAN =		0x0F, /**< Korean */
	TEL_SIM_LP_CHINESE =		0x10, /**< Chinese */
	TEL_SIM_LP_RUSSIAN =		0x11, /**< Russian */
	TEL_SIM_LP_JAPANESE =		0x12, /**< Japanese */
	TEL_SIM_LP_LANG_UNSPECIFIED =	0xFF /**< Unspecified */
} TelSimLanguagePreferenceCode;

/**
 * @enum TelSimCardStatus
 * This enumeration defines the SIM card status
 */
typedef enum {
	TEL_SIM_STATUS_UNKNOWN,				/**<  Unknown status. It can be initial status **/
	TEL_SIM_STATUS_CARD_ERROR,			/**<  Bad card / On the fly SIM gone bad **/
	TEL_SIM_STATUS_SIM_CARD_POWEROFF,	/**<  SIM Card Powered OFF **/
	TEL_SIM_STATUS_CARD_NOT_PRESENT,	/**<  Card not present **/
	TEL_SIM_STATUS_SIM_INITIALIZING,		/**<  SIM is Initializing state **/
	TEL_SIM_STATUS_SIM_INIT_COMPLETED,	/**<  SIM Initialization COMPLETE **/
	TEL_SIM_STATUS_SIM_PIN_REQUIRED,	/**<  PIN  required state **/
	TEL_SIM_STATUS_SIM_PUK_REQUIRED,	/**<  PUK  required state **/
	TEL_SIM_STATUS_CARD_BLOCKED,		/**<  PIN/PUK blocked(permanently blocked - All the attempts for PIN/PUK failed) **/
	TEL_SIM_STATUS_SIM_NCK_REQUIRED,	/**<  Network Control Key required state **/
	TEL_SIM_STATUS_SIM_NSCK_REQUIRED,	/**<  Network Subset Control Key required state **/
	TEL_SIM_STATUS_SIM_SPCK_REQUIRED,	/**<  Service Provider Control Key required state **/
	TEL_SIM_STATUS_SIM_CCK_REQUIRED,	/**<  Corporate Control Key required state **/
	TEL_SIM_STATUS_CARD_REMOVED,		/**<  Card removed **/
	TEL_SIM_STATUS_SIM_LOCK_REQUIRED	/**<  PH-SIM (phone-SIM) locked state **/
} TelSimCardStatus;

/**
 * @enum TelSimCardChangeStatus
 * This enumeration defines whether SIM card is changed or not
 */
typedef enum {
	TEL_SIM_CHANGE_STATUS_UNKNOWN, /**< Unknown */
	TEL_SIM_CHANGE_STATUS_SAME, /**< SIM card is same */
	TEL_SIM_CHANGE_STATUS_CHANGED /**< SIM card is changed */
} TelSimCardChangeStatus;

/**
 * @enum TelSimRefreshCommandType
 * This enumeration defines the SIM card status
 */
typedef enum {
	TEL_SIM_REFRESH_CMD_INIT_AND_FULL_FCN 		= 0x00, /**<	command qualifier for REFRESH SIM INIT AND FULL FILE CHANGE_NOTIFICATION		*/
	TEL_SIM_REFRESH_CMD_FCN 					= 0x01, /**<	command qualifier for REFRESH FILE CHANGE NOTIFICATION		*/
	TEL_SIM_REFRESH_CMD_INIT_AND_FCN 			= 0x02, /**<	command qualifier for REFRESH SIM INIT AND FILE CHANGE NOTIFICATION		*/
	TEL_SIM_REFRESH_CMD_INIT 					= 0x03, /**<	command qualifier for REFRESH SIM INIT		*/
	TEL_SIM_REFRESH_CMD_RESET 					= 0x04, /**<	command qualifier for REFRESH SIM RESET		*/
	TEL_SIM_REFRESH_CMD_3G_APPLICATION_RESET 	= 0x05, /**<	command qualifier for REFRESH 3G APPLICATION RESET		*/
	TEL_SIM_REFRESH_CMD_3G_SESSION_RESET 		= 0x06, /**<	command qualifier for REFRESH 3G SESSION RESET		*/
	TEL_SIM_REFRESH_CMD_UNSPECIFIED 			= 0xFF  /**<	command qualifier for REFRESH RESERVED		*/
}TelSimRefreshCommandType;

/**
 * @enum TelSimMailboxType
 * This enum gives mailbox type.
 */
typedef enum {
	TEL_SIM_MAILBOX_VOICE,	/**< Mailbox identifier voice */
	TEL_SIM_MAILBOX_FAX,	/**< Mailbox identifier fax */
	TEL_SIM_MAILBOX_EMAIL,	/**< Mailbox identifier email */
	TEL_SIM_MAILBOX_OTHER,	/**< Mailbox identifier other */
	TEL_SIM_MAILBOX_VIDEO	/**< Mailbox identifier video */
} TelSimMailboxType;

/**
 * @enum TelSimCallForwardingType
 * This enum gives Call forwarding type.
 */
typedef enum {
	TEL_SIM_CALL_FORWARDING_VOICE,	/**< Call forwarding identifier voice */
	TEL_SIM_CALL_FORWARDING_FAX,	/**< Call forwarding identifier fax */
	TEL_SIM_CALL_FORWARDING_EMAIL,	/**< Call forwarding identifier email */
	TEL_SIM_CALL_FORWARDING_OTHER,	/**< Call forwarding identifier other */
	TEL_SIM_CALL_FORWARDING_VIDEO	/**< Call forwarding identifier video */
} TelSimCallForwardingType;

/**
 * @enum TelSimAuthenticationType
 * This is used for Authentication Procedure by using SIM.
 */
typedef enum {
	TEL_SIM_AUTH_GSM, /**< GSM Authentication */
	TEL_SIM_AUTH_GSM_CTX, /**< GSM CTX Authentication */
	TEL_SIM_AUTH_3G_CTX, /**< 3G CTX Authentication */
	TEL_SIM_AUTH_GBA, /**< GBA Authentication */
	TEL_SIM_AUTH_GBA_NAF, /**< GBA NAF Authentication */
	TEL_SIM_AUTH_IMS_AKA /**< IMS Authentication */
} TelSimAuthenticationType;

/**
 * @enum TelSimLockType
 *	This structure gives security lock type enum values
 */
typedef enum {
	TEL_SIM_LOCK_PS,
	/**< PH-SIM (phone-SIM) locked.Lock Phone to SIM/UICC card
	 *	(MT asks password when other than current SIM/UICC card inserted;
	 *	MT may remember certain amount of previously used cards thus not
	 *	requiring password when they are inserted
	 */
	TEL_SIM_LOCK_PF,
	/**< PH-FSIM (phone-first-SIM) Lock Phone to the very
	 *	First inserted SIM/UICC card(MT asks password when other than the
	 *	first SIM/UICC card is inserted
	 */
	TEL_SIM_LOCK_SC,
	 /**< SIM Lock (PIN, PIN2, PUK, PUK2) Lock SIM/UICC card ( SIM asks
	 *	password in ME power-up and when this command is issued */
	TEL_SIM_LOCK_FD,
	/**< FDN - SIM card or active application in the UICC (GSM or USIM)
	 *	fixed dialing memory feature */
	TEL_SIM_LOCK_PN, /**< Network Personalization */
	TEL_SIM_LOCK_PU, /**< Network subset Personalization */
	TEL_SIM_LOCK_PP, /**< Service Provider Personalization */
	TEL_SIM_LOCK_PC, /**< Corporate Personalization */
} TelSimLockType;

/**
 * @enum TelSimLockStatus
 * This structure gives security lock key information enum values
 */
typedef enum {
	TEL_SIM_LOCK_KEY_NOT_NEED,	/**< key not need */
	TEL_SIM_LOCK_KEY_PIN,		/**< PIN required */
	TEL_SIM_LOCK_KEY_PUK,		/**< PUK required */
	TEL_SIM_LOCK_KEY_PIN2,		/**< PIN2 required */
	TEL_SIM_LOCK_KEY_PUK2,		/**< PUK2 required */
	TEL_SIM_LOCK_PERM_BLOCKED	/**< Permanent block SIM */
} TelSimLockStatus;

/**
 * @enum TelSimSpnDispCondition
 * This enumeration defines Service Provider Name display condition type.
 */
typedef enum
{
	TEL_SIM_DISP_INVALID, /**< Invalid display condition */
	TEL_SIM_DISP_SPN, /**< SPN display condition */
	TEL_SIM_DISP_PLMN, /**< PLMN display condition */
	TEL_SIM_DISP_SPN_PLMN /**< SPN/PLMN display condition */
} TelSimSpnDispCondition;

/**
 * This data structure defines plmn informations
 */
typedef struct {
	unsigned int index;	/**< Plmn list index */
	char *plmn;		/**< numeric operator name (MCCMNC) */
} TelSimSpPlmn;

/**
 * This data structure defines plmn list
 */
typedef struct {
	unsigned int count;	/**< Number of service provider plmns in the list */
	TelSimSpPlmn *list;	/**< service provider plmns list */
} TelSimSpPlmnList;

/**
 * This data structure defines the status of sim initialization
 */
typedef struct {
	TelSimCardStatus status; /**< Sim card status */
	TelSimCardChangeStatus change_status; /**< Sim card change status */
} TelSimCardStatusInfo;

/**
 * This data structure defines the data for the Imsi information.
 */
typedef struct {
	char mcc[TEL_SIM_MCC_MNC_LEN_MAX + 1];	/**< mobile country code 3 digits*/
	char mnc[TEL_SIM_MCC_MNC_LEN_MAX + 1];	/**< mobile network code 2 or 3 digits*/
	char msin[TEL_SIM_MSIN_LEN_MAX + 1];	/**< Mobile Station Identification Number */
} TelSimImsiInfo;

/**
 * This data structure defines the data for the SIM ECC information.
 */
typedef struct {
	char number[TEL_SIM_ECC_NUMBER_LEN_MAX + 1];	/**< Emergency Call Code. null termination used*/
	char name[TEL_SIM_ECC_TEXT_MAX + 1];		/**< Alpha identifier */
	TelSimEccEmergencyServiceInfo category;		/**< ECC emergency service information */
} TelSimEcc;

/**
 * This data structure defines the data for the SIM ECC lists.
 */
typedef struct {
	unsigned int count; /**< Total count for ECC list */
	TelSimEcc list[TEL_SIM_ECC_LIST_MAX];	/**< List of emergency numbers */
} TelSimEccList;

typedef struct {
	int ext_len;
	char ext[TEL_SIM_EXT_RECORD_CNT_MAX + 1];
	unsigned short next_record;
} TelSimExt;

typedef struct {
	unsigned char plmn[6+1];
	unsigned short lac_from;
	unsigned short lac_to;
	unsigned char rec_identifier;
} TelSimOpl;

typedef struct {
	int opl_count;
	TelSimOpl list[TEL_SIM_OPL_PNN_RECORD_CNT_MAX];
} TelSimOplList;

typedef struct {
	unsigned char full_name[TEL_SIM_NW_FULL_NAME_LEN_MAX + 1];
	unsigned char short_name[TEL_SIM_NW_FULL_NAME_LEN_MAX + 1];
} TelSimPnn;

typedef struct {
	int pnn_count;
	TelSimPnn list[TEL_SIM_OPL_PNN_RECORD_CNT_MAX];
} TelSimPnnList;

typedef struct {
	unsigned char profile_id;		/**< Profile Identity of the subscriber profile*/
	TelSimMailboxType mb_type;	/**< Mailbox type */
	unsigned int alpha_id_len; /**< Mailbox Alpha Identifier length */
	char alpha_id[TEL_SIM_ALPHA_ID_LEN_MAX]; /**< Mailbox Alpha Identifier */
	char number[TEL_SIM_MBDN_NUM_LEN_MAX]; /**< Dialing Number/SSC String */
} TelSimMailBoxNumber;

/**
 * This data structure defines the data for the Mailbox lists.
 */
typedef struct {
	unsigned int alpha_id_max_len;		/**< Alpha id max length in SIM */
	unsigned int count;					/**< Number of mailbox count*/
	TelSimMailBoxNumber list[TEL_SIM_MSP_CNT_MAX * TEL_SIM_MAILBOX_TYPE_MAX]; /**< Mailbox list info */
} TelSimMailboxList;

/**
 * This data structure defines the data for the Call Forwarding.
 */
typedef struct {
	unsigned char profile_id;	/**< Profile Identity of the subscriber profile*/
	gboolean indication[TEL_SIM_CALL_FORWARDING_TYPE_MAX]; /**< 0: voice , 1 : fax, 2 : email, 3:other, 4 :video */
} TelSimCfis;

/**
 * This data structure defines the data for the Call Forwarding lists.
 */
typedef struct {
	unsigned int profile_count; /**< Call forwarding list count in SIM */
	TelSimCfis cf[TEL_SIM_MSP_CNT_MAX]; /**< Call forwarding list info in SIM */
} TelSimCfisList;

/**
 * This data structure defines indication status for the Message Waiting.
 */
 typedef struct {
	gboolean indication; /**< indication status for the Message Waiting */
	unsigned char count; /**< indication count for the Message Waiting */
} TelSimMsgWaitingIndicator;

/**
 * This data structure defines the data for the Message Waiting.
 */
typedef struct {
	unsigned char profile_id; /**< Profile Identity of the subscriber profile */
	gboolean count_valid; /**< whether count is valid or not (in case of cphs, count is not valid) */
	TelSimMsgWaitingIndicator msg_waiting[TEL_SIM_MAILBOX_TYPE_MAX]; /**< 0: voice , 1 : fax, 2 : email, 3:other, 4 :video */
} TelSimMwis;

/**
 * This data structure defines the data for the Message Waiting lists.
 */
typedef struct {
	unsigned int profile_count; /**< Message waiting list count in SIM */
	TelSimMwis mw[TEL_SIM_MSP_CNT_MAX]; /**< Message waiting list info in SIM */
} TelSimMwisList;

/**
 * This data structure defines the data for the MSISDN.
 */
typedef struct {
	char *alpha_id; /**< MSISDN Alpha Identifier. If not exist, Null string will be returned */
	char *num;  /**< MSISDN number. If not exist, Null string will be returned*/
} TelSimSubscriberInfo;

/**
 * This data structure defines the data for the MSISDN lists.
 */
typedef struct {
	unsigned int count; /**< MSISDN list count in SIM */
	TelSimSubscriberInfo *list; /**< MSISDN list info in SIM */
} TelSimMsisdnList;

/**
 * This data structure defines the data for the SPN.
 */
typedef struct {
	TelSimSpnDispCondition display_condition;	/**< display condition */
	char spn[TEL_SIM_SPN_LEN_MAX + 1];		/**< SPN */
} TelSimSpn;

/**
 * This data structure defines the data for the CPHS Network Name.
 */
typedef struct {
	char *full_name; /**< Short Network Operator Name */
	char *short_name; /**< Long Network Operator Name */
}TelSimCphsNetName;

typedef struct {
	TelSimResult result;
	union {
		TelSimEccList ecc;
		TelSimLanguagePreferenceCode language;
		char iccid[TEL_SIM_ICCID_LEN_MAX + 1];
		TelSimMailboxList mb;
		TelSimCfisList cf;
		TelSimMwisList mw;
		TelSimMsisdnList msisdn_list;
		TelSimSpn spn;
		TelSimOplList opl;
		TelSimPnnList pnn;
		TelSimSpPlmnList spdi;
		TelSimCphsNetName cphs_net;
	} data;
} TelSimFileResult;

typedef struct {
	/*	'00' - no mailbox dialing number associated with message waiting indication group type.
	 'xx' - record number in EFMBDN associated with message waiting indication group type.
	 */
	int voice_index;	/**< voice mailbox identifier*/
	int fax_index;	/**< FAX mailbox identifier*/
	int email_index;	/**< email mailbox identifier*/
	int other_index;	/**< other mailbox identifier*/
	int video_index;	/**< video mailbox identifier*/
} TelSimMbi;

/**
 *This is used for authentication request procedure.
 */
typedef struct {
	TelSimAuthenticationType auth_type;		/**< Authentication type */
	unsigned int rand_length;			/**< the length of RAND */
	unsigned int autn_length;			/**< the length of AUTN. it is not used in case of GSM AUTH */
	char rand_data[TEL_SIM_AUTH_MAX_REQ_DATA_LEN];	/**< RAND data */
	char autn_data[TEL_SIM_AUTH_MAX_REQ_DATA_LEN];	/**< AUTN data. it is not used in case of GSM AUTH */
} TelSimAuthenticationData;

/**
 * @enum TelSimAuthenticationResult
 * This is used to get detailed result for Authentication Procedure.
 */
typedef enum {
	TEL_SIM_AUTH_NO_ERROR = 0x00, /**< ISIM no error */
	TEL_SIM_AUTH_CANNOT_PERFORM, /**< status - can't perform authentication */
	TEL_SIM_AUTH_SKIP_RESPONSE, /**< status - skip authentication response */
	TEL_SIM_AUTH_MAK_CODE_FAILURE, /**< status - MAK(Multiple Activation Key) code failure */
	TEL_SIM_AUTH_SQN_FAILURE, /**< status - SQN(SeQuenceNumber) failure */
	TEL_SIM_AUTH_SYNCH_FAILURE, /**< status - synch failure */
	TEL_SIM_AUTH_UNSUPPORTED_CONTEXT, /**< status - unsupported context */
	TEL_SIM_AUTH_UNSPECIFIED /**< Unspecified error */
} TelSimAuthenticationResult;

/**
 * This is used for result data of authentication.
 */
typedef struct {
	TelSimAuthenticationType auth_type;				/**< authentication type */
	TelSimAuthenticationResult detailed_result;		/**<Detailed result for authentication procedure*/
	unsigned int resp_length;					/**< response length. IMS and 3G case, it stands for RES_AUTS. GSM case, it stands for SRES. */
	char resp_data[TEL_SIM_AUTH_MAX_RESP_DATA_LEN];			/**< response data. IMS and 3G case, it stands for RES_AUTS. GSM case, it stands for SRES. */
	unsigned int authentication_key_length;				/**< the length of authentication key, Kc*/
	char authentication_key[TEL_SIM_AUTH_MAX_RESP_DATA_LEN];	/**< the data of of authentication key, Kc*/
	unsigned int cipher_length;					/**< the length of cipher key length */
	char cipher_data[TEL_SIM_AUTH_MAX_RESP_DATA_LEN];		/**< cipher key */
	unsigned int integrity_length;					/**< the length of integrity key length */
	char integrity_data[TEL_SIM_AUTH_MAX_RESP_DATA_LEN];		/**< integrity key */
} TelSimAuthenticationResponse;

/**
 * @enum TelSimPowerState
 * This enumeration defines the power state of SIM.
 */
typedef enum {
	TEL_SIM_POWER_OFF = 0x00, /**< OFF */
	TEL_SIM_POWER_ON = 0x01, /**< ON */
	TEL_SIM_POWER_UNSPECIFIED = 0xFF /**< Unspecified */
} TelSimPowerState;

/**
 * This is used to verify PIN code.
 */
typedef struct {
	TelSimPinType pin_type;	/**< Specifies the PIN type */
	char *pw;		/**< Pin password*/
} TelSimSecPinPw;

/**
 * This is used to verify PUK code.
 */
typedef struct {
	TelSimPukType puk_type;	/**< Specifies the PUK type */
	char *puk_pw;		/**< Puk password */
	char *new_pin_pw;	/**< New corresponding pin password */
} TelSimSecPukPw;

/**
 * This is used to change PIN code .
 */
typedef struct {
	TelSimPinType pin_type;	/**< Specifies the PIN type */
	char *old_pw;		/**< Current password */
	char *new_pw;		/**< New pin password */
} TelSimSecChangePinPw;

/**
 * This is used to PIN code verification.
 */
typedef struct {
	TelSimPinType pin_type;		/**< Specifies the PIN type */
	unsigned int retry_count;	/**< Number of attempts remaining for PIN verification */
} TelSimSecPinResult;

/**
 * This is used to PUK code verification.
 */
typedef struct {
	TelSimPukType puk_type;		/**< Specifies the PUK type */
	unsigned int retry_count;	/**< Number of attempts remaining for PUK verification */
} TelSimSecPukResult;

/**
 * This structure is used to enable/disable facility
 */
typedef struct {
	TelSimLockType lock_type;	/**< Facility type */
	char *pw;			/**< password */
} TelSimFacilityPw;

/**
 * This structure is used to PIN/PUK verification
 */
typedef struct {
	TelSimLockType type;		/**< Specifies the PIN or PUK type.*/
	unsigned int retry_count;	/**< Number of attempts remaining for PIN/PUK verification.*/
} TelSimFacilityResult;

/**
 * This structure is used to enable/disable facility
 */
typedef struct {
	TelSimLockType type; /**< Security lock type */
	TelSimFacilityStatus f_status; /**< Lock status (enable/disable) */
}TelSimFacilityInfo;

/**
 *
 * This structure is used to get information about LOCK_TYPE
 */
typedef struct {
	TelSimLockType lock_type;	/**< Lock type */
	TelSimLockStatus lock_status;	/**< Lock key */
	unsigned int retry_count;	/**< retry counts */
} TelSimLockInfo;

/**
 * This data structure defines the data for the apdu command.
 */
typedef struct {
	unsigned int apdu_len; /**<  SIM APDU length */
	unsigned char apdu[TEL_SIM_APDU_LEN_MAX]; /**<  SIM APDU data */
} TelSimApdu;

/**
 * This data structure defines the data for the Response of sending apdu.
 */
typedef struct {
	unsigned int apdu_resp_len; /**< Response apdu data length */
	unsigned char apdu_resp[TEL_SIM_APDU_RESP_LEN_MAX]; /**< Response apdu data */
} TelSimApduResp;

/**
 * This data structure defines the data for the Response of ATR request.
 */
typedef struct {
	unsigned int atr_len; /**<  SIM ATR data length */
	unsigned char atr[TEL_SIM_ATR_LEN_MAX]; /**<  SIM ATR data */
} TelSimAtr;

/**
 *  \defgroup TAPI_EVENT TAPI Event IDs
 *  @{
 */

/**
 *  \defgroup TEL_EVENT_SIM SIM Event IDs
 *  @{
 */

/**
 *  @hideinitializer
 *  @brief SIM status
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SIM_STATUS
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SIM_STATUS
 *  @retval data \ref TelSimCardStatus
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SIM_STATUS				TELEPHONY_SIM_INTERFACE":card_status"

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif /* __TEL_SIM_H__ */
