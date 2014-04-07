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

#ifndef __TEL_SS_H__
#define __TEL_SS_H__

#include <tel_if.h>

/**
*  @addtogroup TAPI_SS
*  @{
*
*  @file tel_ss.h
*  @brief TAPI Ss Interface
*/

#ifdef __cplusplus
extern "C"
{
#endif

#define TELEPHONY_SS_INTERFACE TELEPHONY_SERVICE".Ss"

/** Maximum Barring Password length */
#define TEL_SS_BARRING_PASSWORD_LEN_MAX 4	/**<Maximum length of barring password */

/** Maximum Call Forwarding Number length */
#define TEL_SS_NUMBER_LEN_MAX 82	/**<Maximum Call Forwarding Number length */

 /**
 * @enum TelSsResult
 * This enumeration defines the ss Response (Result) codes
 */
 typedef enum {
	TEL_SS_RESULT_SUCCESS,					/**< Operation completed successfully */
	TEL_SS_RESULT_FAILURE,					/**< Operation failed */
	TEL_SS_RESULT_INVALID_PARAMETER,		/**< Invalid input parameters */
	TEL_SS_RESULT_MEMORY_FAILURE,			/**< Memory allocation falied */
	TEL_SS_RESULT_OPERATION_NOT_SUPPORTED,/**< Operation is not supported */
	TEL_SS_RESULT_SERVICE_NOT_AVAILABLE,	/**< Service is not available */
	TEL_SS_RESULT_OPERATION_NOT_ALLOWED,	/**< Operation is not allowed currently */
	TEL_SS_RESULT_INCORRECT_OPERATION,		/**< Incorrect Operation */
	TEL_SS_RESULT_INVALID_PASSWORD,			/**< Password is invalid */
	TEL_SS_RESULT_PASSWORD_BLOCKED,			/**< Password blocked */
	TEL_SS_RESULT_UNKONWN_ALPHABET,		/**< SS error indicating unknown SS data coding of alphabet */
	TEL_SS_RESULT_OPERATION_NOT_PERMITTED	/**< Operation not permitted */
} TelSsResult;

/**
 * @enum TelSsClass
 * This enumeration defines the Ss Class Information.
 */
typedef enum {
	TEL_SS_CLASS_ALL_TELE_BEARER = 0,
	/* TELESERVICE */
	TEL_SS_CLASS_ALL_TELE = 10,			/**< 0x10 : All Teleservices */
	TEL_SS_CLASS_VOICE = 11,				/**< 0x11 : All Voice ( telephony ) */
	TEL_SS_CLASS_ALL_DATA_TELE = 12,		/**< 0x12 : All Data Teleservices */
	TEL_SS_CLASS_FAX = 13,					/**< 0x13 : All Fax Service */
	TEL_SS_CLASS_SMS = 16,					/**< 0x16 : SMS service	 */
	TEL_SS_CLASS_VGCS = 17,				/**< 0x17 : Voice Group Call Service */
	TEL_SS_CLASS_VBS = 18,					/**< 0x18 : Voice Broadcast */
	TEL_SS_CLASS_ALL_TELE_EXPT_SMS = 19,	/**< 0x19 : All teleservice except SMS */

	/* BEARER SERVICE */
	TEL_SS_CLASS_ALL_BEARER = 20,			/**< 0X20 : all bearer services */
	TEL_SS_CLASS_ALL_ASYNC = 21,			/**< 0x21 : All Async services */
	TEL_SS_CLASS_ALL_SYNC = 22,			/**< 0x21 : All Async services */
	TEL_SS_CLASS_ALL_CS_SYNC = 24,		/**< 0x24 : All Circuit switched sync */
	TEL_SS_CLASS_ALL_CS_ASYNC = 25,		/**< 0x25 : All Circuit switched async */
	TEL_SS_CLASS_ALL_DEDI_PS = 26,		/**< 0x26 : All Dedicated packet Access */
	TEL_SS_CLASS_ALL_DEDI_PAD = 27,		/**< 0x27 : All Dedicated PAD Access */
	TEL_SS_CLASS_ALL_DATA_CDA = 28,		/**< 0x28 : All Data CDA*/

	/* CPHS - AUXILIARY SERVICE */
	TEL_SS_CLASS_AUX_VOICE = 89,			/**< 0x89 : All Auxiliary Voice ( Auxiliary telephony ) */

	TEL_SS_CLASS_ALL_GPRS_BEARER = 99,	/**< 0x99 : All GPRS bearer services */
} TelSsClass;

/**
 * @enum TelSsBarringType
 * This enumeration defines the Call Barring Types
 */
typedef enum {
	TEL_SS_CB_TYPE_BAOC, /**< All Outgoing calls */
	TEL_SS_CB_TYPE_BOIC, /**< Outgoing international calls */
	TEL_SS_CB_TYPE_BOIC_NOT_HC, /**< Outgoing international calls except
					to Home Country */
	TEL_SS_CB_TYPE_BAIC, /**< All Incoming Calls */
	TEL_SS_CB_TYPE_BIC_ROAM, /**< Incoming Calls when roam */
	TEL_SS_CB_TYPE_NS, /**< Incoming calls from numbers Not stored to SIM */
	TEL_SS_CB_TYPE_AB,  /**< All barring services */
	TEL_SS_CB_TYPE_AOB, /**< All outgoing barring services */
	TEL_SS_CB_TYPE_AIB, /**< All incoming barring services */
} TelSsBarringType;

/**
 * @enum TelSsForwardCondition
 * This enumeration defines the Call Forwarding Condition
 */
typedef enum {
	TEL_SS_CF_COND_CFU, /**< Unconditional */
	TEL_SS_CF_COND_CFB,  /**< Mobile Subscriber Busy */
	TEL_SS_CF_COND_CFNRY, /**< No Reply */
	TEL_SS_CF_COND_CFNRC, /**< Not Reachable */
	TEL_SS_CF_COND_ALL,   /**< All */
	TEL_SS_CF_COND_ALL_CFC, /**< All Conditional */
} TelSsForwardCondition;

/**
 * @enum TelSsForwardMode
 * This enumeration defines the Call Forwarding Mode
 */
typedef enum {
	TEL_SS_CF_MODE_DISABLE, /**< Deactivate call forwarding  */
	TEL_SS_CF_MODE_ENABLE, /**< Activate call forwarding */
	TEL_SS_CF_MODE_REGISTER, /**< Register Call forwarding  */
	TEL_SS_CF_MODE_DEREGISTER, /**< De-register call forwarding */
} TelSsForwardMode;

/**
 * @enum TelSsClirNetworkStatus
 * This enumeration defines the Clir Network Status
 */
typedef enum {
	TEL_CLIR_STATUS_NOT_PROVISIONED, /**<Service not provided by the service provider */
	TEL_CLIR_STATUS_PROVISIONED, /**<Service is provided by the service provider */
	TEL_CLIR_STATUS_UNKNOWN, /**<Service status is unknown*/
	TEL_CLIR_STATUS_TEMP_RESTRICTED, /**<Service is temporarily restricted */
	TEL_CLIR_STATUS_TEMP_ALLOWED, /**<Service is temporarily allowed */
} TelSsClirNetworkStatus;

/**
 * @enum TelSsClirDeviceStatus
 * This enumeration defines the Clir Device Status
 * The device CLIR status overrides the CLIR subscription when temporary mode is provisioned
 */
typedef enum {
	TEL_CLIR_STATUS_DEFAULT,  /**< According to the subscription of CLIR */
	TEL_CLIR_STATUS_INVOCATION, /**< CLI presentation is restricted */
	TEL_CLIR_STATUS_SUPPRESSION, /**< CLI presentation is allowed */
} TelSsClirDeviceStatus;

/**
 * @enum TelSsCliNetworkStatus
 * This enumeration defines the Cli Network Status
 */
typedef enum {
	TEL_SS_CLI_NOT_PROVISIONED, /**<Service not provided by the service provider */
	TEL_SS_CLI_PROVISIONED, /**<Service is provided by the service provider */
	TEL_SS_CLI_UNKNOWN, /**<Service status is unknown*/
} TelSsCliNetworkStatus;

/**
 * @enum TelSsCliDeviceStatus
 * This enumeration defines the Cli Device Status
 */
typedef enum {
	TEL_SS_CLI_DISABLE, /**<CLI is disabled */
	TEL_SS_CLI_ENABLE, /**<<CLI is enabled */
} TelSsCliDeviceStatus;

/**
 * @enum TelSsCliType
 * This enumeration defines the Cli Service Types
 */
typedef enum {
	TEL_SS_CLI_CLIP, /**< Calling Line Identification Presentation */
	TEL_SS_CLI_CLIR, /**< Calling Line Identification Restriction */
	TEL_SS_CLI_COLP, /**< Connected Line Identification Presentation */
	TEL_SS_CLI_COLR, /**< Connected Line Identification Restriction */
	TEL_SS_CLI_CDIP, /**< Called Line Identification Presentation */
	TEL_SS_CLI_CNAP, /**< Calling Name Presentation */
} TelSsCliType;

/**
 * @enum TelSsUssdStatus
 * This enumeration defines the ussd Indication Type
 */
typedef enum {
	TEL_SS_USSD_STATUS_NO_ACTION_REQUIRED, /**< No further user action required */
	TEL_SS_USSD_STATUS_ACTION_REQUIRED, /**< Further user action required*/
	TEL_SS_USSD_STATUS_TERMINATED_BY_NETWORK, /**< USSD terminated by network */
	TEL_SS_USSD_STATUS_OTHER_CLIENT, /**< Other local client has responded */
	TEL_SS_USSD_STATUS_NOT_SUPPORTED, /**< Operation not supported */
	TEL_SS_USSD_STATUS_TIME_OUT, /**< Time out when there is no response from network */
} TelSsUssdStatus;

/**
 * @enum TelSsUssdType
 * This enum defines the values for USSD type
 */
typedef enum {
	TEL_SS_USSD_TYPE_USER_INIT, /**< USSD request type User Initiated. */
	TEL_SS_USSD_TYPE_USER_RSP, /**< USSD request type User Response. */
	TEL_SS_USSD_TYPE_USER_REL, /**< USSD request type User Release. */
} TelSsUssdType;

/**
 *  This structure  defines the different parameters related to call barring.
 */
 typedef struct {
	TelSsClass class; /**< SS class */
	gboolean enable; /**< Barring enable/disable */
	TelSsBarringType type; /**< Barring type */
	char pwd[TEL_SS_BARRING_PASSWORD_LEN_MAX + 1]; /**< Barring password */
} TelSsBarringInfo;

/**
 *  This structure  defines the different parameters related to Call Barring Service Information to get Status.
 */
typedef struct {
	TelSsClass class; /**< SS class */
	TelSsBarringType type; /**< Call barring types providing various barring conditions on that basis call be barred */
} TelSsBarringGetInfo;

/**
 * This structure defines the values for ss call barring record
 */
typedef struct {
	TelSsClass class; /**< SS class */
	gboolean enable; /**< Barring enable/disable */
	TelSsBarringType type; /**< Call barring types providing various barring conditions on that basis call be barred */
} TelSsBarringInfoRecord;

/**
 * This structure defines the values for ss Call Barring Service Response Information
 */
typedef struct {
	unsigned int record_num; /**< Number of Call barring record */
	TelSsBarringInfoRecord *records; /**< Call barring records */
} TelSsBarringResp;

/**
 * This structure defines the values for ss Call Barring Password Change Information
 */
typedef struct {
	char old_pwd[TEL_SS_BARRING_PASSWORD_LEN_MAX + 1]; /**< Call barring password(old) */
	char new_pwd[TEL_SS_BARRING_PASSWORD_LEN_MAX + 1]; /**< Call barring password(new) */
} TelSsBarringPwdInfo;

/**
 * This structure defines the values for ss Call Forwarding Service Setting Information
 */
typedef struct {
	TelSsClass class; /**< SS class */
	TelSsForwardMode mode; /**< Call Forwarding Mode */
	TelSsForwardCondition condition; /**< Call Forwarding Condition */
	char number[TEL_SS_NUMBER_LEN_MAX + 1]; /**< Call Forwarding Number*/
	unsigned int wait_time; /**< No reply wait time 1-30 secs */
} TelSsForwardInfo;

/**
 * This structure defines the values for ss Call Forwarding Service Information to get Forwarding Status
 */
typedef struct {
	TelSsClass class; /**< SS class */
	TelSsForwardCondition condition; /**< Call Forwarding Condition */
} TelSsForwardGetInfo;

/**
 * This structure defines the values for ss Call Forwarding Service Information to set Forwarding
 */
typedef struct {
	TelSsClass class; /**< SS class */
	gboolean enable; /**< Call Forwarding enable/disable */
	TelSsForwardCondition condition; /**< Call Forwarding Condition */
	char number[TEL_SS_NUMBER_LEN_MAX + 1]; /**< Call forwarding number */
	unsigned int wait_time; /**< Only be available when "no reply" is enabled
					or queried, range: 1 - 30 */
} TelSsForwardingInfoRecord;

/**
 * This structure defines the values for ss Call Forwarding Service Response data
 */
typedef struct {
	unsigned int record_num; /**< Number of Call Forwarding record */
	TelSsForwardingInfoRecord *records; /**< Call Forwarding records */
} TelSsForwardingResp;

/**
 * This structure defines the values for ss Call Waiting Service Setting Information
 */
typedef struct {
	TelSsClass class; /**< SS class */
	gboolean enable; /**< Call waiting enable/disable */
} TelSsWaitingInfo;

/**
 * This structure defines the values for ss Call Waiting Service Response data
 */
typedef struct {
	unsigned int record_num; /**< Number of Call waiting record */
	TelSsWaitingInfo *records; /**< Call waiting records */
} TelSsWaitingResp;

/**
 * This structure defines the values for ss CLIR status Information
 */
typedef struct {
	TelSsClirNetworkStatus net_status; /**< CLIR status (network side) */
	TelSsClirDeviceStatus dev_status;  /**< CLIR status (device side) */
} TelSsClirStatus;

/**
 * This structure defines the values for ss CLI status Information
 */
typedef struct {
	TelSsCliNetworkStatus net_status; /**< CLI status (network side) */
	TelSsCliDeviceStatus dev_status;  /**< CLI status (device side) */
} TelSsCliStatus;

/**
 * This structure defines the values for ss CLI Service Response data
 */
typedef struct {
	TelSsCliType type; /**< line identification service types */
	union {
		TelSsClirStatus clir; /**< CLIR status */
		TelSsCliStatus clip; /**< CLIP status */
		TelSsCliStatus colp; /**< COLP status */
		TelSsCliStatus colr; /**< COLR status */
		TelSsCliStatus cdip; /**< CDIP status */
		TelSsCliStatus cnap; /**< CNAP status */
	} status; /**< union */
} TelSsCliResp;

/**
 * This structure defines the values for ss CLI Service(include CLIR) Setting Information
 */
typedef struct {
	TelSsCliType type; /**< line identification service types */
	union {
		TelSsClirDeviceStatus clir; /**< CLIR status */
		TelSsCliDeviceStatus clip; /**< CLIP status */
		TelSsCliDeviceStatus colp; /**< COLP status */
		TelSsCliDeviceStatus colr; /**< COLR status */
		TelSsCliDeviceStatus cdip; /**< CDIP status */
		TelSsCliDeviceStatus cnap; /**< CNAP status */
	} status; /**< union */
} TelSsCliInfo;

/**
 * This structure defines the values for ss USSD Request Information
 */
typedef struct {
	TelSsUssdType type; /**< USSD type */
	unsigned char *str; /**< NULL terminated UTF-8 USSD string */
} TelSsUssdInfo;

/**
 * This structure defines the values for ss USSD Response data
 */
typedef struct {
	TelSsUssdType type; /**< USSD type */
	TelSsUssdStatus status; /**< USSD status */
	unsigned char *str; /**< NULL terminated UTF-8 USSD string */
} TelSsUssdResp;

/**
 * This structure defines the values for ss USSD Notification data
 */
typedef struct {
	TelSsUssdStatus status; /**< USSD status */
	unsigned char *str; /**< NULL terminated UTF-8 USSD string */
} TelSsUssdNoti;

/**
 *  \defgroup TAPI_EVENT TAPI Event IDs
 *  @{
 */

/**
 *  \defgroup TEL_EVENT_SS SS Event IDs
 *  @{
 */

/**
 *  @hideinitializer
 *  @brief USSD status and string
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SS_USSD
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SS_USSD
 *  @retval data reference to \ref TelSsUssdNoti structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SS_USSD				TELEPHONY_SS_INTERFACE":NotifyUssd"

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __TEL_SS_H__ */
