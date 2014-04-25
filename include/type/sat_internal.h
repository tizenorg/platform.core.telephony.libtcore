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

#ifndef __SAT_INTERNAL_H__
#define __SAT_INTERNAL_H__

/**
*  @addtogroup TAPI_SAT
*  @{
*
*  @file tel_sat.h
*  @brief TAPI sat Interface
*/

#include <tel_sat.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TEL_SAT_DEF_TITLE_LEN_MAX	50 /**< max length for Menu Title */
#define TEL_SAT_DEF_ITEM_STR_LEN_MAX	50 /**< max length for default item string */
#define TEL_SAT_TEXT_STRING_LEN_MAX		500 /**< max length for text string */
#define TEL_SAT_ENVELOPE_CMD_LEN_MAX	256 /**< max length for envelope command */
#define TEL_SAT_AID_LEN_MAX	128 /**< max length for AID */
#define TEL_SAT_PROVISIONING_REF_MAX_COUNT	10 /**< max count of sat provisioning reference */
#define TEL_SAT_BEARER_PARAMS_LEN_MAX 		10 /**< max length of bearer parameters */
#define TEL_SAT_BEARER_LIST_MAX_COUNT	6 /**< max count of bearer list */

/**
 * @enum TelSatDisplayTextPriority
 * This enumeration defines display text priority
*/
typedef enum {
	TEL_SAT_TEXT_PRIORITY_NORMAL = 0, /**< Display text priority -normal */
	TEL_SAT_TEXT_PRIORITY_HIGH = 1 /**< Display text priority - high */
} TelSatDisplayTextPriority;

/**
 * @enum TelSatDisplayTextClearType
 * This enumeration defines Display text clear type
*/
typedef enum {
	TEL_SAT_TEXT_AUTO_CLEAR_MSG_AFTER_A_DELAY = 0, /**< Display text clear type -clear msg after a delay */
	TEL_SAT_TEXT_WAIT_FOR_USER_TO_CLEAR_MSG = 1 /**< Display text clear type -wait for user to clear msg */
} TelSatDisplayTextClearType;

/**
 * @enum TelSatInputAlphabetType
 * This enumeration defines Alphabet types
*/
typedef enum {
	TEL_SAT_INPUT_ALPHABET_TYPE_SMS_DEFAULT = 1, /**< Display input alphabet type -SMS default */
	TEL_SAT_INPUT_ALPHABET_TYPE_UCS2 = 2 /**< Display inkey type -UCS2 type */
} TelSatInputAlphabetType;

/**
 * @enum TelSatCmdQualifierPlayTone
 * This enumeration defines cmd qualifier for play tone
*/
typedef enum {
	TEL_SAT_VIBRATE_ALERT_OPTIONAL = 0, /**< VIBRATE ALERT UPTO THE TERMINAL */
	TEL_SAT_VIBRATE_ALERT_REQUIRED = 1 /**< VIBRATE, IF AVAILABLE, WITH TONE */
} TelSatCmdQualifierPlayTone;

/**
 * @enum TelSatPresentationType
 * This enumeration defines presentation types
*/
typedef enum {
	TEL_SAT_PRESENTATION_TYPE_NOT_SPECIFIED	= 0x00,  /**< presentation type -NOT SPECIFIED */
	TEL_SAT_PRESENTATION_TYPE_DATA_VALUE = 0x01, /**< presentation type -DATA VALUE */
	TEL_SAT_PRESENTATION_TYPE_NAVIGATION_OPTION = 0x02 /**< presentation type -NAVIGATION OPTION */
} TelSatPresentationType;

/**
 * @enum TelSatSelectionPreferenceType
 * This enumeration defines selection preference types
*/
typedef enum  {
	TEL_SAT_SELECTION_PREFERENCE_NONE_REQUESTED = 0, /**< selection preference type -NONE REQUESTED */
	TEL_SAT_SELECTION_PREFERENCE_USING_SOFT_KEY = 1 /**< selection preference type -USING SOFT KEY */
} TelSatSelectionPreferenceType;

/**
 * @enum TelSatSsProblemType
 * This enumeration defines SS problem types
*/
typedef enum {
	TEL_SAT_SS_PROBLEM_NO_SPECIFIC_CAUSE	 = 0, /**< SS problem type -NO SPECIFIC CAUSE */
	TEL_SAT_SS_PROBLEM_FACILITY_NOT_SUPPORTED = 0x15 /**< SS problem type -FACILITY NOT SUPPORTED */
} TelSatSsProblemType;

/**
 * @enum TelSatUssdProblemType
 * This enumeration defines USSD problem types
*/
typedef enum {
	TEL_SAT_USSD_PROBLEM_NO_SPECIFIC_CAUSE = 0, /**< USSD problem type -NO SPECIFIC CAUSE */
	TEL_SAT_USSD_PROBLEM_UNKNOWN_ALPHABET = 0x47 /**< USSD problem type -UNKNOWN ALPHABET */
} TelSatUssdProblemType;

/**
 * @enum TelSatNetworkProblemType
 * This enumeration defines network problem types
*/
typedef enum {
	TEL_SAT_NETWORK_PROBLEM_NO_SPECIFIC_CAUSE = 0x0, /**< Network problem with no specific cause */
	TEL_SAT_NETWORK_PROBLEM_USER_BUSY = 0x91 /**< Network problem with USER BUSY */
} TelSatNetworkProblemType;

/**
 * @enum TelSatBipProblemType
 * This enumeration defines BIP problem types
*/
typedef enum {
	TEL_SAT_BIP_PROBLEM_NO_SPECIFIC_CAUSE = 0x00, /**< BIP problem with no specific cause */
	TEL_SAT_BIP_PROBLEM_NO_CHANNEL_AVAILABLE = 0x01, /**< BIP problem with no channel available */
	TEL_SAT_BIP_PROBLEM_CHANNEL_CLOSED = 0x02, /**< BIP problem with channel closed */
	TEL_SAT_BIP_PROBLEM_CHANNEL_ID_NOT_VALID = 0x03, /**< BIP problem with channel id not valid */
	TEL_SAT_BIP_PROBLEM_BUF_SIZE_NOT_AVAILABLE = 0x04, /**< BIP problem with buffer size not available */
	TEL_SAT_BIP_PROBLEM_SECURITY_ERROR = 0x05, /**< BIP problem with security error */
	TEL_SAT_BIP_PRBLM_SIM_ME_IF_TRNSPRT_LEVEL_NOT_AVL = 0x06, /**< BIP problem with SIM ME interface transport level not available */
	TEL_SAT_BIP_REMOTE_DEV_NOT_REACHABLE = 0x07, /**< BIP problem with remote device not reachable */
	TEL_SAT_BIP_SERVICE_ERROR = 0x08, /**< BIP service error */
	TEL_SAT_BIP_SERVICE_IDENTIFIER_UNKNOWN = 0x09 /**< BIP service identifier unknown */
} TelSatBipProblemType;

/**
 * @enum TelSatBearerDescType
 * This enumeration defines bearer desc types
*/
typedef enum {
	TEL_SAT_BEARER_CSD = 0x1, /**< BEARER DESC CSD */
	TEL_SAT_BEARER_GPRS = 0x2, /**< BEARER DESC GPRS */
	TEL_SAT_BEARER_DEFAULT_BEARER_FROM_TRANSPORT_LAYER = 0x3, /**< BEARER DESC DEFAULT BEARER FROM TRANSPORT LAYER */
	TEL_SAT_BEARER_LOCAL_LINK_TECHNOLOGY_INDEPENDENT = 0x4, /**< BEARER DESC LOCAL LINK TECHNOLOGY INDEPENDENT */
	TEL_SAT_BEARER_BLUETOOTH = 0x5, /**< BEARER DESC BLUETOOTH */
	TEL_SAT_BEARER_IrDA = 0x6, /**< BEARER DESC IrDA */
	TEL_SAT_BEARER_RS232 = 0x7, /**< BEARER DESC RS232	*/
	TEL_SAT_BEARER_USB = 0x10, /**< BEARER DESC USB */
	TEL_SAT_BEARER_RESERVED = 0xFF /**<	RESERVED	*/
} TelSatBearerDescType;

/**
 * @enum TelSatBearerParamCsdDataRateType
 * This enumeration defines bearer param csd data rate types
*/
typedef enum {
	TEL_SAT_BIP_DR_AUTOBAUDING = 0, /**< CSD data rate - AUTOBAUDING */
	TEL_SAT_BIP_DR_300BPS_V21 = 1, /**<  CSD data rate -300BPS V21 */
	TEL_SAT_BIP_DR_1200BPS_V22 = 2, /**< CSD data rate - 1200BPS V22 */
	TEL_SAT_BIP_DR_1200_75BPS_V23 = 3, /**< CSD data rate -1200 75BPS V23 */
	TEL_SAT_BIP_DR_2400BPS_V22 = 4, /**< CSD data rate -2400BPS V22 */
	TEL_SAT_BIP_DR_2400BPS_V26 = 5, /**< CSD data rate - 2400BPS V26 */
	TEL_SAT_BIP_DR_4800BPS_V32 = 6, /**< CSD data rate - 4800BPS V32 */
	TEL_SAT_BIP_DR_9600BPS_V32 = 7, /**< CSD data rate - 9600BPS V32 */
	TEL_SAT_BIP_DR_9600BPS_V34 = 12, /**<	CSD data rate - 9600BPS_V34 */
	TEL_SAT_BIP_DR_14400BPS_V34 = 14, /**< CSD data rate -14400BPS V34 */
	TEL_SAT_BIP_DR_19200BPS_V34 = 15, /**< CSD data rate -19200BPS V34 */
	TEL_SAT_BIP_DR_28800BPS_V34 = 16, /**< CSD data rate -28800BPS V34 */
	TEL_SAT_BIP_DR_33600BPS_V34 = 17, /**< CSD data rate -33600BPS V34 */
	TEL_SAT_BIP_DR_1200BPS_V120 = 34, /**< CSD data rate -1200BPS V120 */
	TEL_SAT_BIP_DR_2400BPS_V120 = 36, /**< CSD data rate -2400BPS V120 */
	TEL_SAT_BIP_DR_4800BPS_V120 = 38, /**< CSD data rate -4800BPS V120 */
	TEL_SAT_BIP_DR_9600BPS_V120 = 39, /**< CSD data rate -9600BPS V120 */
	TEL_SAT_BIP_DR_14400BPS_V120 = 43, /**< CSD data rate -14400BPS V120 	*/
	TEL_SAT_BIP_DR_19200BPS_V120 = 47, /**< CSD data rate -19200BPS V120 	*/
	TEL_SAT_BIP_DR_28800BPS_V120 = 48, /**< CSD data rate -28800BPS V120 	*/
	TEL_SAT_BIP_DR_38400BPS_V120 = 49, /**< CSD data rate -38400BPS V120 	*/
	TEL_SAT_BIP_DR_48000BPS_V120 = 50, /**< CSD data rate -48000BPS V120 	*/
	TEL_SAT_BIP_DR_56000BPS_V120 = 51, /**< CSD data rate -56000BPS V120 	*/
	TEL_SAT_BIP_DR_300BPS_V110 = 65, /**<	CSD data rate - 300BPS V110 */
	TEL_SAT_BIP_DR_1200BPS_V110 = 66, /**< CSD data rate -1200BPS V110 */
	TEL_SAT_BIP_DR_2400BPS_V110_OR_X31_FALG_STUFFING = 68, /**< CSD data rate - 2400BPS V110 OR X31 FALG STUFFING */
	TEL_SAT_BIP_DR_4800BPS_V110_OR_X31_FALG_STUFFING = 70, /**< CSD data rate - 4800BPS V110 OR X31 FALG STUFFING */
	TEL_SAT_BIP_DR_9600BPS_V110_OR_X31_FALG_STUFFING = 71, /**< CSD data rate - 9600BPS V110 OR X31 FALG STUFFING */
	TEL_SAT_BIP_DR_14400BPS_V110_OR_X31_FALG_STUFFING = 75, /**< CSD data rate - 14400BPS V110 OR X31 FALG STUFFING */
	TEL_SAT_BIP_DR_19200BPS_V110_OR_X31_FALG_STUFFING = 79, /**< CSD data rate -19200BPS V110 OR X31 FALG STUFFING */
	TEL_SAT_BIP_DR_28800BPS_V110_OR_X31_FALG_STUFFING = 80, /**<	CSD data rate -28800BPS V110 OR X31 FALG STUFFING */
	TEL_SAT_BIP_DR_38400BPS_V110_OR_X31_FALG_STUFFING = 81, /**<	CSD data rate -38400BPS V110 OR X31 FALG STUFFING */
	TEL_SAT_BIP_DR_48000BPS_V110_OR_X31_FALG_STUFFING = 82, /**<	CSD data rate -48000BPS V110 OR X31 FALG STUFFING */
	TEL_SAT_BIP_DR_56000BPS_V110_OR_X31_FALG_STUFFING = 83, /**<	CSD data rate -56000BPS V110 OR X31 FALG STUFFING */
	TEL_SAT_BIP_DR_64000BPS = 84, /**< CSD data rate -64000BPS */
	TEL_SAT_BIP_DR_56000BPS_BIT_TRANSPERENT = 115, /**< CSD data rate -56000BPS BIT TRANSPERENT	 */
	TEL_SAT_BIP_DR_64000BPS_BIT_TRANSPERENT = 116, /**< CSD data rate -64000BPS BIT TRANSPERENT */
	TEL_SAT_BIP_DR_32000BPS_PIAFS32K = 120, /**<	CSD data rate -32000BPS PIAFS32K */
	TEL_SAT_BIP_DR_64000BPS_PIAFS64K = 121, /**<	CSD data rate - 64000BPS PIAFS64K */
	TEL_SAT_BIP_DR_28800BPS_MULTIMEDIA = 130, /**<	CSD data rate -28800BPS MULTIMEDIA */
	TEL_SAT_BIP_DR_32000BPS_MULTIMEDIA = 131, /**<	CSD data rate -32000BPS MULTIMEDIA */
	TEL_SAT_BIP_DR_33600BPS_MULTIMEDIA = 132, /**<	CSD data rate - 33600BPS MULTIMEDIA */
	TEL_SAT_BIP_DR_56000BPS_MULTIMEDIA = 133, /**<	CSD data rate -56000BPS MULTIMEDIA */
	TEL_SAT_BIP_DR_64000BPS_MULTIMEDIA = 134 /**<	CSD data rate -64000BPS MULTIMEDIA */
} TelSatBearerParamCsdDataRateType;

/**
 * @enum TelSatBearerParamCsdBearerServiceType
 * This enumeration defines bearer param csd bearer service types
*/
typedef enum {
	TEL_SAT_BIP_CSD_BS_DATA_CIRCUIT_ASYNC_UDI = 0, /**< CSD Bearer service - DATA CIRCUIT ASYNCHRONOUS UDI */
	TEL_SAT_BIP_CSD_BS_DATA_CIRCUIT_SYNC = 1, /**< CSD Bearer service - DATA CIRCUIT SYNCHRONOUS UDI */
	TEL_SAT_BIP_CSD_BS_PAD_ACCESS_ASYNC_UDI = 2, /**< CSD Bearer service - PAD ACCESS ASYNCHRONOUS UDI */
	TEL_SAT_BIP_CSD_BS_PACKET_ACCESS_SYNC = 3, /**< CSD Bearer service - PACKET ACCESS SYNCHRONOUS UDI */
	TEL_SAT_BIP_CSD_BS_DATA_CIRCUIT_ASYNC_RDI = 4, /**< CSD Bearer service - DATA CIRCUIT ASYNCHRONOUS RDI */
	TEL_SAT_BIP_CSD_BS_DATA_CIRCUIT_SYNC_RDI = 5, /**< CSD Bearer service - DATA CIRCUIT SYNCHRONOUS RDI */
	TEL_SAT_BIP_CSD_BS_PAD_ACCESS_ASYNC_RDI = 6, /**< CSD Bearer service - PAD ACCESS ASYNCHRONOUS RDI */
	TEL_SAT_BIP_CSD_BS_PACKET_ACCESS_SYNC_RDI = 7 /**< CSD Bearer service - PACKET ACCESS SYNCHRONOUS RDI */
} TelSatBearerParamCsdBearerServiceType;

/**
 * @enum TelSatBearerParamCsdConnectionElementType
 * This enumeration defines bearer param csd connection element types
*/
typedef enum {
	TEL_SAT_BIP_CSD_CONN_ELEM_TRANSPARENT = 0, /**< CSD connection element - TRANSPARENT */
	TEL_SAT_BIP_CSD_CONN_ELEM_NON_TRANSPARENT = 1, /**< CSD connection element - NON TRANSPARENT	*/
	TEL_SAT_BIP_CSD_CONN_ELEM_BOTH_TRANSPARENT_PREF = 2, /**<	CSD connection element -BOTH TRANSPARENT PREFFERED */
	TEL_SAT_BIP_CSD_CONN_ELEM_BOTH_NON_TRANSPARENT_PREF = 3 /**< CSD connection element -  NON TRANSPARENT PREFFERED */
} TelSatBearerParamCsdConnectionElementType;

/**
 * @enum TelSatBearerParamGprsPrecedenceClassType
 * This enumeration defines bearer param gprs precedence class types
*/
typedef enum {
	TEL_SAT_BIP_GPRS_PRECED_CLASS_HIGH_PRIORITY = 0x01, /**<	GPRS precedence class -HIGH PRIORITY */
	TEL_SAT_BIP_GPRS_PRECED_CLASS_NORM_PRIORITY = 0x02, /**< GPRS precedence class -NORM PRIORITY */
	TEL_SAT_BIP_GPRS_PRECED_CLASS_LOW_PRIORITY = 0x03 /**< GPRS precedence class - LOW PRIORITY */
} TelSatBearerParamGprsPrecedenceClassType;

/**
 * @enum TelSatBearerParamGprsDelayClassType
 * This enumeration defines bearer param gprs delay class types
*/
typedef enum {
	TEL_SAT_BIP_GPRS_DELAY_CLASS_1 = 0x01, /**< GPRS delay class - 1 */
	TEL_SAT_BIP_GPRS_DELAY_CLASS_2 = 0x02, /**< GPRS delay class -2 */
	TEL_SAT_BIP_GPRS_DELAY_CLASS_3 = 0x03, /**< GPRS delay class -3 */
	TEL_SAT_BIP_GPRS_DELAY_CLASS_4 = 0x04 /**< GPRS delay class - 4 */
} TelSatBearerParamGprsDelayClassType;

/**
 * @enum TelSatBearerParamGprsReliabilityClassType
 * This enumeration defines bearer param gprs reliability class types
*/
typedef enum {
	TEL_SAT_BIP_GPRS_RELIABILITY_CLASS_1 = 0x01, /**< GPRS Reliability class -1 */
	TEL_SAT_BIP_GPRS_RELIABILITY_CLASS_2 = 0x02, /**< GPRS Reliability class -2 */
	TEL_SAT_BIP_GPRS_RELIABILITY_CLASS_3 = 0x03, /**< GPRS Reliability class -3 */
	TEL_SAT_BIP_GPRS_RELIABILITY_CLASS_4 = 0x04, /**< GPRS Reliability class -4 */
	TEL_SAT_BIP_GPRS_RELIABILITY_CLASS_5 = 0x05 /**<	 GPRS Reliability class -5 */
} TelSatBearerParamGprsReliabilityClassType;

/**
 * @enum TelSatBearerParamGprsPeakThroughputClassType
 * This enumeration defines bearer param gprs peak throughput class types
*/
typedef enum {
	TEL_SAT_BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_8KBPS = 0x01, /**< GPRS peak throughput class- UPTO 8KBPS */
	TEL_SAT_BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_16KBPS = 0x02, /**< GPRS peak throughput class- UPTO 16KBPS */
	TEL_SAT_BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_32KBPS = 0x03, /**< GPRS peak throughput class-UPTO 32KBPS */
	TEL_SAT_BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_64KBPS = 0x04, /**< GPRS peak throughput class-UPTO 64KBPS */
	TEL_SAT_BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_128KBPS = 0x05, /**< GPRS peak throughput class- UPTO 128KBPS */
	TEL_SAT_BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_256KBPS = 0x06, /**< GPRS peak throughput class- UPTO 256KBPS */
	TEL_SAT_BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_512KBPS = 0x07, /**< GPRS peak throughput class- UPTO 512KBPS */
	TEL_SAT_BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_1024KBPS = 0x08, /**< GPRS peak throughput class-UPTO 1024KBPS */
	TEL_SAT_BIP_GPRS_PEAK_THROUGHPUT_CLASS_UPTO_2048KBPS = 0x09 /**<	GPRS peak throughput class- UPTO 2048KBPS */
} TelSatBearerParamGprsPeakThroughputClassType;

/**
 * @enum TelSatBearerParamGprsMeanThroughputClassType
 * This enumeration defines bearer param gprs mean throughput class types
*/
typedef enum {
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_DOT_22_BPS = 0x01, /**< GPRS mean throughput class - DOT 22 BPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_DOT_44_BPS = 0x02, /**< GPRS mean throughput class - DOT 44 BPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_1_DOT_11_BPS = 0x03, /**< GPRS mean throughput class -1 DOT 11 BPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_2_DOT_2_BPS = 0x04, /**< GPRS mean throughput class -2 DOT 2 BPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_2_DOT_4_BPS = 0x05, /**< GPRS mean throughput class -2 DOT 4 BPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_11_DOT_1_BPS = 0x06, /**< GPRS mean throughput class - 11 DOT 1 BPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_22BPS = 0x07, /**< GPRS mean throughput class -22BPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_44BPS = 0x08, /**< GPRS mean throughput class - 44BPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_111BPS = 0x09, /**< GPRS mean throughput class - 111BPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_DOT_22_KBPS = 0x0A, /**< GPRS mean throughput class - DOT 22 KBPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_DOT_44_KBPS = 0x0B, /**< GPRS mean throughput class -DOT 44 KBPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_1_DOT_11_KBPS = 0x0C, /**< GPRS mean throughput class -1 DOT 11 KBPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_2_DOT_2_KBPS = 0x0D, /**<	GPRS mean throughput class -2 DOT 2 KBPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_4_DOT_4_KBPS = 0x0E, /**<	GPRS mean throughput class - 4 DOT 4 KBPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_11_DOT_1_KBPS = 0x0F, /**< GPRS mean throughput class -11 DOT 1 KBPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_22KBPS = 0x10, /**<	GPRS mean throughput class - 22KBPS 	*/
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_44KBPS = 0x11, /**<	GPRS mean throughput class - 44KBPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_111KBPS = 0x12, /**< GPRS mean throughput class -111KBPS */
	TEL_SAT_BIP_GPRS_MEAN_THROUGHPUT_CLASS_BEST_EFFORT = 0x13 /**< GPRS mean throughput class - BEST EFFORT	*/
} TelSatBearerParamGprsMeanThroughputClassType;

/**
 * @enum TelSatBearerParamGprsPdpType
 * This enumeration defines bearer param gprs pdp types
*/
typedef enum {
	TEL_SAT_BIP_GPRS_PDP_TYPE_IP = 0x02, /**< bearer parameter GPRS pdp type - IP */
	TEL_SAT_BIP_GPRS_PDP_TYPE_RESERVED = 0xff /**< reserved */
} TelSatBearerParamGprsPdpType;

/**
 * @enum TelSatBearerParamLocalLinksServiceIdentityType
 * This enumeration defines bearer param local links service identity types
*/
typedef enum {
	TEL_SAT_BIP_LL_SERVICE_IDENT_VALUE_ASSIGNED_BY_USIM = 0x00, /**< local links service identity - value assigned by USIM */
	TEL_SAT_BIP_LL_SERVICE_IDENT_VALUE_ASSIGNED_BY_REMOTED_DEVICE = 0xFF /**< local links service identity - value assigned by remote service */
} TelSatBearerParamLocalLinksServiceIdentityType;

/**
 * @enum TelSatEnvelopSubCmd
 * This enumeration defines envelope sub command types
*/
typedef enum {
	TEL_SAT_ENVELOP_MENU_SELECTION, /**< Envelope sub command type -MENU SELECTION */
	TEL_SAT_ENVELOP_CALL_CONTROL, /**< Envelope sub command type -CALL CONTROL */
	TEL_SAT_ENVELOP_TIMER_EXPIRATION, /**< Envelope sub command type -TIMER EXPIRATION */
	TEL_SAT_ENVELOP_EVENT_DOWNLOAD /**< Envelope sub command type -EVENT DOWNLOAD */
} TelSatEnvelopSubCmd;

/**
 * @enum TelSatEnvelopeResp
 * This enum indicates the general result of sending an envelope command to USIM.
 */
typedef enum {
	TEL_SAT_ENVELOPE_SUCCESS,	/**< envelope result - success */
	TEL_SAT_ENVELOPE_SIM_BUSY,	/**<	envelope result - USIM busy */
	TEL_SAT_ENVELOPE_FAILED	/**<	envelope result - failed	 */
}TelSatEnvelopeResp;

/**
* This structure defines display text command qualifier Info
*/
typedef struct {
	TelSatDisplayTextPriority text_priority; /**< display text priority */
	TelSatDisplayTextClearType text_clear_type; /**< display text clear type */
} TelSatCmdQualifierDisplayText;

/**
* This structure defines Get inkey command qualifier Info
*/
typedef struct {
	TelSatInkeyType inkey_type; /**< inkey type */
	TelSatInputAlphabetType alphabet_type; /**< alphabet type */
	gboolean alphabet_set; /**< alphabet set */
	gboolean immediate_rsp_required; /**< immediate response required */
	gboolean help_info; /**< help info */
} TelSatCmdQualifierGetInkey;

/**
* This structure defines Get Input command qualifier Info
*/
typedef struct {
	TelSatInputAlphabetType alphabet_type;  /**< alphabet type */
	gboolean alphabet_set; /**< alphabet set */
	gboolean me_echo_user_input; /**< me echo user input */
	gboolean user_input_unpacked_format; /**< user input unpacked format */
	gboolean help_info; /**< help info */
} TelSatCmdQualifierGetInput;

/**
* This structure defines Select Item command qualifier Info
*/
typedef struct {
	TelSatPresentationType presentation_type; /**< presentation type */
	TelSatSelectionPreferenceType select_preference; /**< selection preference type */
	gboolean help_info; /**< help info */
} TelSatCmdQualifierSelectItem;

/**
* This structure defines Setup Menu command qualifier Info
*/
typedef struct {
	TelSatSelectionPreferenceType select_preference; /**< selection preference type */
	gboolean help_info; /**< help info */
} TelSatCmdQualifierSetupMenu;

/**
* This structure defines open channel command qualifier Info
*/
typedef struct {
	gboolean immediate_link; /**< open channel - immediate link type */
	gboolean automatic_reconnection; /**< open channel -automatic reconnection type */
	gboolean background_mode; /**< open channel -background mode type */
} TelSatCmdQualifierOpenChannel;

/**
* This structure defines Cmd qualifier details Info
*/
typedef struct {
	int cmd_num; /**< specific command number */
	TelSatProactiveCmdType cmd_type; /**< proactive command type */
	union {
		TelSatCmdQualifierDisplayText display_text;  /**< display text command qualifier info */
		TelSatCmdQualifierGetInkey get_inkey;  /**< get inkey command qualifier info */
		TelSatCmdQualifierGetInput get_input;  /**< get input command qualifier info */
		TelSatCmdQualifierPlayTone play_tone;  /**< play tone command qualifier info */
		TelSatCmdQualifierSetupMenu setup_menu; /**<	setup menu command qualifier info */
		TelSatCmdQualifierSelectItem select_item;  /**< select item command qualifier info */
		gboolean cmd_quali_send_sms;  /**< send sms command qualifier info */
		TelSatCmdQualifierSetupCall setup_call;  /**< setup call command qualifier info */
		TelSatCmdQualifierRefresh refresh;  /**< refresh command qualifier info */
		TelSatCmdQualifierProvideLocalInfo provide_local_info;  /**< provide local info command qualifier info */
		gboolean cmd_quali_language_notification;  /**< language notification command qualifier info */
		TelSatCmdQualifierLaunchBrowser launch_browser;  /**< launch browser command qualifier info */
		TelSatCmdQualifierOpenChannel open_channel;  /**< open channel command qualifier info */
		gboolean cmd_quali_send_data;  /**< send data command qualifier info */
	}cmd_qualifier; /**< cmd qualifier Union */
} TelSatCmdDetailInfo;

/**
* This structure defines DISPLAY TEXT command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< DISPLAY TEXT command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< DISPLAY TEXT command device id info */
	TelSatTextTypeInfo text; /**< DISPLAY TEXT command text type info */
	TelSatIconIdentifierInfo icon_id; /**< DISPLAY TEXT command icon id info */
	gboolean immediate_response_requested; /**< DISPLAY TEXT command immediate response requested check info */
	TelSatDurationInfo duration; /**< DISPLAY TEXT command duration info */
	unsigned char text_attribute[4]; /**< DISPLAY TEXT command text attribute info */
} TelSatDisplayTextTlv;

/**
* This structure defines GET INKEY command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< GET INKEY command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< GET INKEY command device id info */
	TelSatTextTypeInfo text; /**< GET INKEY command device id info */
	TelSatIconIdentifierInfo icon_id; /**< GET INKEY command device id info */
	TelSatDurationInfo duration; /**< GET INKEY command device id info */
	unsigned char text_attribute[4]; /**< GET INKEY command device id info */
} TelSatGetInkeyTlv;

/**
* This structure defines GET INPUT command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< GET INPUT command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< GET INPUT command device id info */
	TelSatTextTypeInfo text; /**< GET INPUT command device id info */
	TelSatResponseLength rsp_len; /**< GET INPUT command rsp len info */
	TelSatTextTypeInfo default_text; /**< GET INPUT command default text info */
	TelSatIconIdentifierInfo icon_id; /**< GET INPUT command icon id info */
	unsigned char text_attribute[4]; /**< GET INPUT command text attribute info */
} TelSatGetInputTlv;

/**
* This structure defines MORE TIME command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< MORE TIME command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< MORE TIME command device id info */
} TelSatMoreTimeTlv;

/**
* This structure defines PLAY TONE command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< PLAY TONE command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< PLAY TONE command device id info */
	TelSatAlphaIdentifierInfo alpha_id; /**< PLAY TONE command alpha id info */
	TelSatToneType tone; /**< PLAY TONE command tone info */
	TelSatDurationInfo duration; /**< PLAY TONE command duration info */
	TelSatIconIdentifierInfo icon_id; /**< PLAY TONE command icon id info */
	unsigned char text_attribute[4]; /**< PLAY TONE command text attribute info */
} TelSatPlayToneTlv;

/**
* This structure defines SETUP MENU command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< SETUP MENU command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< SETUP MENU command device id info */
	TelSatAlphaIdentifierInfo alpha_id; /**< SETUP MENU command alpha id info */
	unsigned char menu_item_cnt; /**< SETUP MENU command menu item count */
	TelSatMenuItemInfo menu_item[TEL_SAT_MENU_ITEM_COUNT_MAX]; /**< SETUP MENU command menu item info */
	TelSatItemsNextActionIndiListInfo next_act_ind_list; /**< SETUP MENU command next act ind list info */
	TelSatIconIdentifierInfo icon_id; /**< SETUP MENU command icon id info */
	TelSatIconIdentifierListInfo icon_list; /**< SETUP MENU command icon list info */
	unsigned char text_attribute[4]; /**< SETUP MENU command text attribute info */
	TelSatTextAttributeListInfo text_attribute_list; /**< SETUP MENU command text attribute list info */
} TelSatSetupMenuTlv;

/**
* This structure defines SELECT ITEM command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< SELECT ITEM command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< SELECT ITEM command device id info */
	TelSatAlphaIdentifierInfo alpha_id; /**< SELECT ITEM command alpha id info */
	unsigned char menu_item_cnt; /**< SELECT ITEM command menu item count info */
	TelSatMenuItemInfo menu_item[TEL_SAT_MENU_ITEM_COUNT_MAX]; /**< SELECT ITEM command menu item info */
	TelSatItemsNextActionIndiListInfo item_next_act_ind_list; /**< SELECT ITEM command next act ind list info */
	unsigned char item_identifier; /**< SELECT ITEM command item identifier info */
	TelSatIconIdentifierInfo icon_id; /**< SELECT ITEM command icon id info */
	TelSatIconIdentifierListInfo icon_list; /**< SELECT ITEM command icon list info */
	unsigned char text_attribute[4]; /**< SELECT ITEM command text attribute info */
	TelSatTextAttributeListInfo text_attribute_list; /**< SELECT ITEM command text attribute list info */
	/* frame identifier 8.80 */
} TelSatSelectItemTlv;

/**
* This structure defines SEND SMS command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< SEND SMS command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< SEND SMS command device id info */
	TelSatAlphaIdentifierInfo alpha_id; /**< SEND SMS command alpha id info */
	TelSatAddressInfo address; /**< SEND SMS command address info */
	TelSatSmsTpduInfo sms_tpdu; /**< SEND SMS command sms tpdu info */
	/* CDMA-SMS TPDU 8.71 */
	TelSatIconIdentifierInfo icon_id; /**< SEND SMS command icon id info */
	unsigned char text_attribute[4]; /**< SEND SMS command text attribute info */
	/* frame identifier 8.80 */
} TelSatSendSmsTlv;

/**
* This structure defines SEND SS command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< SEND SS command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< SEND SS command device id info */
	TelSatAlphaIdentifierInfo alpha_id; /**< SEND SS command alpha id info */
	TelSatSsStringInfo ss_string; /**< SEND SS command ss string info */
	TelSatIconIdentifierInfo icon_id; /**< SEND SS command icon id info */
	unsigned char text_attribute[4]; /**< SEND SS command text attribute info */
	/* frame identifier 8.80 */
} TelSatSendSsTlv;

/**
* This structure defines SEND USSD command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< SEND USSD command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< SEND USSD command device id info */
	TelSatAlphaIdentifierInfo alpha_id; /**< SEND USSD command alpha id info */
	TelSatUssdStringInfo ussd_string; /**< SEND USSD command ussd string info */
	TelSatIconIdentifierInfo icon_id; /**< SEND USSD command icon id info */
	unsigned char text_attribute[4]; /**< SEND USSD command text attribute info */
	/* frame identifier 8.80 */
} TelSatSendUssdTlv;

/**
* This structure defines SETUP CALL command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< SETUP CALL command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< SETUP CALL command device id info */
	TelSatAddressInfo address; /**< SETUP CALL command address info */
	TelSatCcpInfo ccp; /**< SETUP CALL command ccp info */
	TelSatSubAddressInfo subaddress; /**< SETUP CALL command subaddress info */
	TelSatDurationInfo duration; /**< SETUP CALL command duration info */
	TelSatAlphaIdentifierInfo user_confirm_alpha_id; /**< SETUP CALL command user confirm alpha id info */
	TelSatIconIdentifierInfo user_confirm_icon_id; /**< SETUP CALL command user confirm icon id info */
	TelSatAlphaIdentifierInfo call_setup_alpha_id; /**< SETUP CALL command alpha id info */
	TelSatIconIdentifierInfo call_setup_icon_id; /**< SETUP CALL command icon id info */
	unsigned char user_confirm_text_attribute[4]; /**< SETUP CALL command user confirm text attribute info */
	unsigned char call_setup_text_attribute[4]; /**< SETUP CALL command text attribute info */
	/* frame identifier 8.80 */
} TelSatSetupCallTlv;

/**
* This structure defines REFRESH command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< REFRESH command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< REFRESH command device id info */
	TelSatFileListInfo file_list; /**< REFRESH command file list info */
	char aid[16]; /**< 8.60 AID - length is 16 byte, Refer TS 101.220 */
} TelSatRefreshTlv;

/**
* This structure defines PROVIDE LOCAL INFORMATION command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< PROVIDE LOCAL INFORMATION command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< PROVIDE LOCAL INFORMATION command device id info */
} TelSatProvideLocalInfoTlv;

/**
* This structure defines SETUP EVENT LIST command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< SETUP EVENT LIST command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< SETUP EVENT LIST command device id info */
	TelSatEventListInfo event_list; /**< SETUP EVENT LIST command event list info */
	TelSatEventListInfo modem_event_list; /**< SETUP EVENT LIST command modem event list info */
} TelSatSetupEventListTlv;

/**
* This structure defines SETUP IDLE MODE TEXT command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< SETUP IDLE MODE TEXT command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< SETUP IDLE MODE TEXT command device id info */
	TelSatTextTypeInfo text; /**< SETUP IDLE MODE TEXT command textinfo */
	TelSatIconIdentifierInfo icon_id; /**< SETUP IDLE MODE TEXT command icon id info */
	unsigned char text_attribute[4]; /**< SETUP IDLE MODE TEXT command text attribute info */
} TelSatSetupIdleModeTextTlv;

/**
* This structure defines SEND DTMF command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< SEND DTMF command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< SEND DTMF command device id info */
	TelSatAlphaIdentifierInfo alpha_id; /**< SEND DTMF command alpha id info */
	TelSatDtmfStringInfo dtmf_string; /**< SEND DTMF command dtmf string info */
	TelSatIconIdentifierInfo icon_id; /**< SEND DTMF command icon id info */
	unsigned char text_attribute[4]; /**< SEND DTMF command text attribute info */
} TelSatSendDtmfTlv;

/**
* This structure defines LANGUAGE NOTIFICATION command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< LANGUAGE NOTIFICATION command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< LANGUAGE NOTIFICATION command device id info */
	TelSatLanguageInfo language; /**< LANGUAGE NOTIFICATION command language info */
} TelSatLanguageNotificationTlv;

/**
* This structure defines LAUNCH BROWSER command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< LAUNCH BROWSER command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< LAUNCH BROWSER command device id info */
	TelSatBrowserIdentityType browser_id; /**< LAUNCH BROWSER command browser id info */
	TelSatUrlInfo url; /**< LAUNCH BROWSER command url info */
	TelSatBearerList bearer; /**< LAUNCH BROWSER command bearer info */
	int file_ref_count; /**< LAUNCH BROWSER command file ref count info */
	TelSatProvisioningFileRef file_list[TEL_SAT_PROVISIONING_REF_MAX_COUNT]; /**< LAUNCH BROWSER command file list info */
	TelSatTextTypeInfo gateway_proxy_text; /**< LAUNCH BROWSER command gateway proxy text info */
	TelSatAlphaIdentifierInfo user_confirm_alpha_id; /**< LAUNCH BROWSER command user confirm alpha id info */
	TelSatIconIdentifierInfo user_confirm_icon_id; /**< LAUNCH BROWSER command user confirm icon id info */
	unsigned char user_confirm_text_attribute[4]; /**< LAUNCH BROWSER command user confirm text attribute info */
	//frame identifier 8.80
} TelSatLaunchBrowserTlv;

/**
* This structure defines Open Channel CS Bearer Info
*/
typedef struct {
	TelSatAddressInfo address; /**< address info */
	TelSatSubAddressInfo subaddress; /**< OPEN CHANNEL command subaddress info */
	TelSatDurationInfo duration1; /**< OPEN CHANNEL command duration1 info */
	TelSatDurationInfo duration2; /**< OPEN CHANNEL command duration2 info */
	TelSatOtherAddressInfo other_address; /**< OPEN CHANNEL command other address info */
	TelSatTextTypeInfo text_user_login; /**< OPEN CHANNEL command text user login info */
	TelSatTextTypeInfo text_user_pwd; /**< OPEN CHANNEL command text user pwd info */
} TelSatOpenChannelCsBearerInfo;

/**
* This structure defines Open Channel Packet Data Service Bearer Info
*/
typedef struct {
	TelSatNetworkAccessNameInfo network_access_name; /**< network access name info */
	TelSatOtherAddressInfo other_address; /**< other address info */
	TelSatTextTypeInfo text_user_login; /**< user login text info */
	TelSatTextTypeInfo text_user_pwd; /**< user pwd text info */
} TelSatOpenChannelPacketDataServiceBearerInfo;

/**
* This structure defines Open Channel Local Bearer Info
*/
typedef struct {
	TelSatDurationInfo duration1; /**< duration info */
	TelSatDurationInfo duration2; /**< duration info */
	TelSatTextTypeInfo text_user_pwd; /**< user pwd text info */
	TelSatRemoteEntityAddressInfo remote_entity_address; /**< remote entity address info */
} TelSatOpenChannelLocalBearerInfo;

/**
* This structure defines Open Channel Default Bearer Info
*/
typedef struct {
	TelSatOtherAddressInfo other_address; /**< other address info */
	TelSatTextTypeInfo text_user_login; /**< user login text info */
	TelSatTextTypeInfo text_user_pwd; /**< user pwd text info */
} TelSatOpenChannelDefaultBearerInfo;

/**
* This structure defines Bearer Param Csd Info
*/
typedef struct {
	TelSatBearerParamCsdDataRateType data_rate; /**< Bearer Param Csd Data Rate Type */
	TelSatBearerParamCsdBearerServiceType service_type; /**< Bearer Param Csd Bearer Service Type */
	TelSatBearerParamCsdConnectionElementType connection_element_type; /**< Bearer Param Csd Connection Element Type */
} TelSatBearerParametersCsdInfo;

/**
* This structure defines Bearer Param Gprs Info
*/
typedef struct {
	TelSatBearerParamGprsPrecedenceClassType precedence_class; /**< Bearer Param Gprs Precedence Class Type */
	TelSatBearerParamGprsDelayClassType delay_class; /**< Bearer Param Gprs Delay Class Type */
	TelSatBearerParamGprsReliabilityClassType reliability_class; /**< Bearer Param Gprs Reliability Class Type */
	TelSatBearerParamGprsPeakThroughputClassType peak_throughput_class; /**< Bearer Param Gprs Peak Throughput Class Type */
	TelSatBearerParamGprsMeanThroughputClassType mean_throughput_class; /**< Bearer Param Gprs Mean Throughput Class Type */
	TelSatBearerParamGprsPdpType pdp_type; /**< Bearer Param Gprs Pdp Type */
} TelSatBearerParametersGprsInfo;

/**
* This structure defines Bearer Param Local Links Info
*/
typedef struct {
	TelSatBearerParamLocalLinksServiceIdentityType service_type; /**< Bearer Param Local Links Service Identity Type */
	char service_record[TEL_SAT_BEARER_PARAMS_LEN_MAX]; /**< service record data */
} TelSatBearerParametersLocalLinksInfo;

/**
* This structure defines Bearer Desc Info
*/
typedef struct {
	TelSatBearerDescType bearer_type; /**< Bearer Desc Type */
	union{
		TelSatBearerParametersCsdInfo cs_bearer_param; /**< Bearer Parameters Csd Info */
		TelSatBearerParametersGprsInfo ps_bearer_param; /**< Bearer Parameters Gprs Info */
		TelSatBearerParametersLocalLinksInfo local_link_bearer_param; /**< Bearer Parameters Local Links Info */
	}bearer_parameter; /**< Union */
} TelSatBearerDescriptionInfo;

/**
* This structure defines OPEN CHANNEL command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< OPEN CHANNEL command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< OPEN CHANNEL command device id info */
	TelSatAlphaIdentifierInfo alpha_id; /**< OPEN CHANNEL command alpha id info */
	TelSatIconIdentifierInfo icon_id; /**< OPEN CHANNEL command icon id info */
	TelSatBearerDescriptionInfo bearer_desc; /**< OPEN CHANNEL command bearer desc info */
	unsigned char buffer_size[2]; /**< OPEN CHANNEL command buffer size info */
	TelSatSimMeInterfaceTransportLevelInfo interface_transport_level; /**< OPEN CHANNEL command interface transport level info */
	TelSatOtherAddressInfo data_destination_address; /**< OPEN CHANNEL command data destination address info */
	unsigned char text_attribute[4]; /**< OPEN CHANNEL command text attribute info */
	/* frame identifier 8.80 */
	union{
		TelSatOpenChannelCsBearerInfo cs_bearer; /**< OPEN CHANNEL command cs bearer info */
		TelSatOpenChannelPacketDataServiceBearerInfo ps_bearer; /**< OPEN CHANNEL command ps bearer info */
		TelSatOpenChannelLocalBearerInfo local_bearer; /**< OPEN CHANNEL command local bearer info */
		TelSatOpenChannelDefaultBearerInfo default_bearer; /**< OPEN CHANNEL command default bearer info */
	}bearer_detail; /**< Union */
} TelSatOpenChannelTlv;

/**
* This structure defines CLOSE CHANNEL command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< CLOSE CHANNEL command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< CLOSE CHANNEL command device id info */
	TelSatAlphaIdentifierInfo alpha_id; /**< CLOSE CHANNEL command alpha id info */
	TelSatIconIdentifierInfo icon_id; /**< CLOSE CHANNEL command icon id info */
	unsigned char text_attribute[4]; /**< CLOSE CHANNEL command text attribute info */
} TelSatCloseChannelTlv;

/**
* This structure defines RECEIVE DATA command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< RECEIVE DATA command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< RECEIVE DATA command device id info */
	TelSatAlphaIdentifierInfo alpha_id; /**< RECEIVE DATA command alpha id info */
	TelSatIconIdentifierInfo icon_id; /**< RECEIVE DATA command icon id info */
	unsigned char channel_data_len; /**< RECEIVE DATA command channel data len info */
	unsigned char text_attribute[4]; /**< RECEIVE DATA command text attribute info */
} TelSatReceiveChannelTlv;

/**
* This structure defines SEND DATA command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< SEND DATA command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< SEND DATA command device id info */
	TelSatAlphaIdentifierInfo alpha_id; /**< SEND DATA command alpha id info */
	TelSatIconIdentifierInfo icon_id; /**< SEND DATA command icon id info */
	TelSatChannelDataInfo channel_data; /**< SEND DATA command channel data info */
	unsigned char text_attribute[4]; /**< SEND DATA command text attribute info */
} TelSatSendChannelTlv;

/**
* This structure defines GET CHANNEL STATUS command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< GET CHANNEL STATUS command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< GET CHANNEL STATUS command device id info */
} TelSatGetChannelStatusTlv;

/**
* This structure defines Menu Selection envelope data
*/
typedef struct {
	TelSatDeviceIdentitiesInfo device_identitie; /**< device identities info */
	unsigned char item_identifier; /**< item identifier info */
	gboolean help_request; /**< help request info */
} TelSatEnvelopMenuSelectionInfo;

/**
* This structure defines Event Download envelope data
*/
typedef struct {
	TelSatDeviceIdentitiesInfo device_identitie; /**< device identities info */
	TelSatEventListType event; /**< event info */
	gboolean idle_screen; /**< idle screen check */
	TelSatLanguageInfo language; /**< language info */
	TelSatBrowserTerminationCauseType browser_termination; /**< browser termination info */
	TelSatChannelStatusInfo channel_status; /**< channel_status info */
	unsigned char channel_data_len; /**< channel_data_len */
} TelSatEnvelopEventDownloadTlv;

/**
* This structure defines Terminal Response data for Display Text command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Display Text command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Display Text command TR device id info */
	TelSatResultType result_type; /**< Display Text command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Display Text command TR me problem type info */
} TelSatTrDisplayTextTlv;

/**
* This structure defines Terminal Response data for Get Inkey command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Get Inkey command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Get Inkey command TR device id info */
	TelSatResultType result_type; /**< Get Inkey command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Get Inkey command TR me problem type info */
	TelSatTextTypeInfo text; /**< Get Inkey command TR text info */
	TelSatDurationInfo duration; /**< Get Inkey command TR duration info */
} TelSatTrGetInkeyTlv;

/**
* This structure defines Terminal Response data for Get Input command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Get Input command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Get Input command TR device id info */
	TelSatResultType result_type; /**< Get Input command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Get Input command TR me problem type info */
	TelSatTextTypeInfo text; /**< Get Input command TR text info */
} TelSatTrGetInputTlv;

/**
* This structure defines Terminal Response data for More Time command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< More Time command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< More Time command TR device id info */
	TelSatResultType result_type; /**< More Time command TR result type info */
	TelSatMeProblemType me_problem_type; /**< More Time command TR me problem type info */
} TelSatTrMoreTimeTlv;

/**
* This structure defines Terminal Response data for Play Tone command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Play Tone command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Play Tone command TR device id info */
	TelSatResultType result_type; /**< Play Tone command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Play Tone command TR me problem type info */
} TelSatTrPlayToneTlv;

/**
* This structure defines Terminal Response data for Refresh command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Refresh command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Refresh command TR device id info */
	TelSatResultType result_type; /**< Refresh command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Refresh command TR me problem type info */
} TelSatTrRefreshTlv;

/**
* This structure defines Terminal Response data for Setup Menu command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Setup Menu command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Setup Menu command TR device id info */
	TelSatResultType result_type; /**< Setup Menu command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Setup Menu command TR me problem type info */
} TelSatTrSetupMenuTlv;

/**
* This structure defines Terminal Response data for Select Item command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Select Item command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Select Item command TR device id info */
	TelSatResultType result_type; /**< Select Item command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Select Item command TR me problem type info */
	gboolean other_info; /**< Select Item command TR other info */
	unsigned char item_identifier; /**< Select Item command TR item identifier info */
} TelSatTrSelectItemTlv;

/**
* This structure defines Terminal Response data for Send Sms command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Send Sms command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Send Sms command TR device id info */
	TelSatResultType result_type; /**< Send Sms command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Send Sms command TR me problem type info */
	TelSatCallCtrlProblemType cc_problem_type; /**< Send Sms command TR cc problem type info */
} TelSatTrSendSmsTlv;

/**
* This structure defines Terminal Response data for Send SS command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Send SS command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Send SS command TR device id info */
	TelSatResultType result_type; /**< Send SS command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Send SS command TR me problem type info */
	TelSatSsProblemType ss_problem; /**< Send SS command TR ss problem info */
	gboolean other_info; /**< Send SS command TR other info */
	TelSatCallCtrlProblemType cc_problem_type; /**< Send SS command TR cc problem type info */
	TelSatCallCtrlRequestedActionInfo call_ctrl_req_act; /**< Send SS command TR call ctrl req act info */
	TelSatResultType result_type2; /**< Send SS command TR result type2 info */
	TelSatTextTypeInfo text; /**< Send SS command TR text info */
} TelSatTrSendSsTlv;

/**
* This structure defines Terminal Response data for Send Ussd command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Send Ussd command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Send Ussd command TR device id info */
	TelSatResultType result_type; /**< Send Ussd command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Send Ussd command TR me problem type info */
	TelSatUssdProblemType ussd_problem; /**< Send Ussd command TR ussd problem info */
	gboolean other_info; /**< Send Ussd command TR other info */
	gboolean call_ctrl_has_modification; /**< Send Ussd command TR call ctrl has modification check */
	TelSatCallCtrlProblemType cc_problem_type; /**< Send Ussd command TR cc problem type info */
	TelSatCallCtrlRequestedActionInfo call_ctrl_req_act; /**< Send Ussd command TR call ctrl req act info */
	TelSatResultType result_type2; /**< Send Ussd command TR result type info */
	TelSatTextTypeInfo text; /**< Send Ussd command TR text info */
	TelSatTextTypeInfo text2; /**< Send Ussd command TR text info */
} TelSatTrSendUssdTlv;

/**
* This structure defines Terminal Response data for Setup Call command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Setup Call command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Setup Call command TR device id info */
	TelSatResultType result_type; /**< Setup Call command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Setup Call command TR me problem type info */
	gboolean other_info; /**< Setup Call command TR other info */
	TelSatNetworkProblemType network_problem_type; /**< Setup Call command TR network problem type info */
	TelSatCallCtrlProblemType cc_problem_type; /**< Setup Call command TR cc problem type info */
	TelSatCallCtrlRequestedActionInfo call_ctrl_req_act; /**< Setup Call command TR call ctrl req act info */
	TelSatResultType result_type2; /**< Setup Call command TR result type info */
	TelSatTextTypeInfo text; /**< Setup Call command TR text info */
	gboolean tapi_cause; /**< Setup Call command TR tapi cause info */
	int tapi_call_level_cause; /**< Setup Call command TR tapi call level cause info */
	int tapi_ss_level_cause; /**< Setup Call command TR tapi ss level cause info */
} TelSatTrSetupCallTlv;

/**
* This structure defines Terminal Response data for Provide Local Info command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Provide Local Info command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Provide Local Info command TR device id info */
	TelSatResultType result_type; /**< Provide Local Info command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Provide Local Info command TR me problem type info */
	gboolean other_info; /**< Provide Local Info command TR other info */
	union {
		TelSatLocationInformationInfo location_info; /**< Provide Loca lInfo command TR location info */
		char imei[8+1];  /**< length is defined by 8 in TS 102 223 */
		char nmr[10+1];  /**< length is defined by 10 in TS 102 223 */
		TelSatDateTimeZoneInfo date_time_and_timezone; /**< Provide Loca lInfo command TR date time and timezone info */
		TelSatLanguageInfo language; /**< Provide Loca lInfo command TR language info */
	}other; /**< Union */
} TelSatTrProvideLocalInfoTlv;

/**
* This structure defines Terminal Response data for Setup Event List command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Setup Event List command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Setup Event List command TR device id info */
	TelSatResultType result_type; /**< Setup Event List command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Setup Event List command TR me problem type info */
} TelSatTrSetupEventListTlv;

/**
* This structure defines Terminal Response data for Setup Idle Mode Text command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Setup Idle Mode Text command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Setup Idle Mode Text command TR device id info */
	TelSatResultType result_type; /**< Setup Idle Mode Text command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Setup Idle Mode Text command TR me problem type info */
} TelSatTrSetupIdleModeTextTlv;

/**
* This structure defines Terminal Response data for Send Dtmf command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Send Dtmf command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Send Dtmf command TR device id info */
	TelSatResultType result_type; /**< Send Dtmf command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Send Dtmf command TR me problem type info */
} TelSatTrSendDtmfTlv;

/**
* This structure defines Terminal Response data for Language Notification command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Language Notification command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Language Notification command TR device id info */
	TelSatResultType result_type; /**< Language Notification command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Language Notification command TR me problem type info */
} TelSatTrLanguageNotificationTlv;

/**
* This structure defines Terminal Response data for Launch Browser command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Launch Browser command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Launch Browser command TR device id info */
	TelSatResultType result_type; /**< Launch Browser command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Launch Browser command TR me problem type info */
	TelSatLaunchBrowserProblemType browser_problem_type; /**< Launch Browser command TR browser problem type info */
} TelSatTrLaunchBrowserTlv;

/**
* This structure defines Terminal Response data for Open Channel command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Open Channel command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Open Channel command TR device id info */
	TelSatResultType result_type; /**< Open Channel command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Open Channel command TR me problem type info */
	TelSatBipProblemType bip_problem_type; /**< Open Channel command TR bip problem type info */
	TelSatChannelStatusInfo channel_status; /**< Open Channel command TR channel status info */
	TelSatBearerDescriptionInfo bearer_desc; /**< Open Channel command TR bearer desc info */
	unsigned char buffer_size[2]; /**< Open Channel command TR buffer size info */
	/* TelSatOtherAddressInfo other_address; (uicc server mode) */
} TelSatTrOpenChannelTlv;

/**
* This structure defines Terminal Response data for Close Channel command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Close Channel command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Close Channel command TR device id info */
	TelSatResultType result_type; /**< Close Channel command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Close Channel command TR me problem type info */
	TelSatBipProblemType bip_problem_type; /**< Close Channel command TR bip problem type info */
} TelSatTrCloseChannelTlv;

/**
* This structure defines Terminal Response data for Send Data command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Send Data command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Send Data command TR device id info */
	TelSatResultType result_type; /**< Send Data command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Send Data command TR me problem type info */
	TelSatBipProblemType bip_problem_type; /**< Send Data command TR bip problem type info */
	unsigned char channel_data_len; /**< Send Data command TR channel data len info */
} TelSatTrSendDataTlv;

/**
* This structure defines Terminal Response data for Receive Data command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Receive Data command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Receive Data command TR device id info */
	TelSatResultType result_type; /**< Receive Data command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Receive Data command TR me problem type info */
	TelSatBipProblemType bip_problem_type; /**< Receive Data command TR bip problem type info */
	TelSatChannelDataInfo channel_data; /**< Receive Data command TR channel data info */
	unsigned char channel_data_len; /**< Receive Data command TR channel data len info */
} TelSatTrReceiveDataTlv;

/**
* This structure defines Terminal Response data for Get Channel Status command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Get Channel Status command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Get Channel Status command TR device id info */
	TelSatResultType result_type; /**< Get Channel Status command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Get Channel Status command TR me problem type info */
	TelSatBipProblemType bip_problem_type; /**< Get Channel Status command TR bip problem type info */
	TelSatChannelStatusInfo channel_status; /**< Get Channel Status command TR channel status info */
} TelSatTrGetChannelStatusTlv;

/**
* This structure defines Terminal Response data for unsupported command
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Unsupport Command TR details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Unsupport Command TR device id info */
	TelSatResultType result_type; /**< Unsupport Command TR result type info */
	TelSatMeProblemType me_problem_type; /**< Unsupport Command TR me problem type info */
} TelSatTrUnsupportCommandTlv;

/**
* This structure defines unsupported command data
*/
typedef struct {
	TelSatCmdDetailInfo command_detail; /**< Unsupport Command details info */
	TelSatDeviceIdentitiesInfo device_id; /**< Unsupport Command device id info */
} TelSatUnsupportCommandTlv;

/**
* This structure defines Terminal Resonse request data
*/
typedef struct {
	int cmd_number; /**< Command number info */
	TelSatProactiveCmdType cmd_type; /**< Command type info */
	union{
		TelSatTrDisplayTextTlv display_text; /**< display text command Terminal Response data */
		TelSatTrGetInkeyTlv get_inkey; /**< get inkey command Terminal Response data */
		TelSatTrGetInputTlv get_input; /**< get input command Terminal Response data */
		TelSatTrMoreTimeTlv more_time; /**< more time command Terminal Response data */
		TelSatTrPlayToneTlv play_tone; /**< play tone command Terminal Response data */
		TelSatTrRefreshTlv refresh; /**< refresh command Terminal Response data */
		TelSatTrSetupMenuTlv setup_menu; /**< setup menu command Terminal Response data */
		TelSatTrSelectItemTlv select_item; /**< select item command Terminal Response data */
		TelSatTrSendSmsTlv send_sms; /**< send sms command Terminal Response data */
		TelSatTrSendSsTlv send_ss; /**< send ss command Terminal Response data */
		TelSatTrSendUssdTlv send_ussd; /**< send ussd command Terminal Response data */
		TelSatTrSetupCallTlv setup_call; /**< setup call command Terminal Response data */
		TelSatTrProvideLocalInfoTlv provide_local_info; /**< provide local info command Terminal Response data */
		TelSatTrSetupEventListTlv setup_event_list; /**< setup event list command Terminal Response data */
		TelSatTrSetupIdleModeTextTlv setup_idle_mode_text; /**< setup idle mode text command Terminal Response data */
		TelSatTrSendDtmfTlv send_dtmf; /**< send dtmf command Terminal Response data */
		TelSatTrLanguageNotificationTlv language_notification; /**< language notification command Terminal Response data */
		TelSatTrLaunchBrowserTlv launch_browser; /**< launch browser command Terminal Response data */
		TelSatTrOpenChannelTlv open_channel; /**< open channel command Terminal Response data */
		TelSatTrCloseChannelTlv close_channel; /**< close channel command Terminal Response data */
		TelSatTrSendDataTlv send_data; /**< send data command Terminal Response data */
		TelSatTrReceiveDataTlv receive_data; /**< receive data command Terminal Response data */
		TelSatTrGetChannelStatusTlv get_channel_status; /**< get channel status command Terminal Response data */
		TelSatTrUnsupportCommandTlv unsupport_cmd; /**< unsupported command Terminal Response data */
	}terminal_rsp_data; /**< Union */
} TelSatRequestTerminalResponseData;

/**
* This structure defines user confirmation request data
*/
typedef struct {
	int user_conf; /**< User Confirmation Data */
	/* Note: Not being used for now*/
	int icon_conf;  /**< icon Confirmation Data */
} TelSatRequestUserConfirmationData;

/**
* This structure defines envelope request data
*/
typedef struct {
	TelSatEnvelopSubCmd sub_cmd; /**< Envelop Sub Cmd */
	union{
		TelSatEnvelopMenuSelectionInfo menu_select; /**< Envelop Menu Selection info */
		TelSatEnvelopEventDownloadTlv event_download; /**< Envelop Event Download info */
	}envelop_data; /**< Union */
} TelSatRequestEnvelopCmdData;

/**
* This structure defines SAT Notification decoding structure
*/
typedef struct {
	int cmd_num; /**< Command number */
	TelSatProactiveCmdType cmd_type; /**< Command type */
	union {
		TelSatDisplayTextTlv display_text; /**< display text command Decoded Proactive Data */
		TelSatGetInkeyTlv get_inkey; /**< get inkey command Decoded Proactive Data */
		TelSatGetInputTlv get_input; /**< get input command Decoded Proactive Data */
		TelSatMoreTimeTlv more_time; /**< more time command Decoded Proactive Data */
		TelSatPlayToneTlv play_tone; /**< play tone command Decoded Proactive Data */
		TelSatSetupMenuTlv setup_menu; /**< setup menu command Decoded Proactive Data */
		TelSatSelectItemTlv select_item; /**< select item command Decoded Proactive Data */
		TelSatSendSmsTlv send_sms; /**< send sms command Decoded Proactive Data */
		TelSatSendSsTlv send_ss; /**< send ss command Decoded Proactive Data */
		TelSatSendUssdTlv send_ussd; /**< send ussd command Decoded Proactive Data */
		TelSatSetupCallTlv setup_call; /**< setup call command Decoded Proactive Data */
		TelSatRefreshTlv refresh; /**< refresh command Decoded Proactive Data */
		TelSatProvideLocalInfoTlv provide_local_info; /**< provide local info command Decoded Proactive Data */
		TelSatSetupEventListTlv setup_event_list; /**< setup event list command Decoded Proactive Data */
		TelSatSetupIdleModeTextTlv setup_idle_mode_text; /**< setup idle mode text command Decoded Proactive Data */
		TelSatSendDtmfTlv send_dtmf; /**< send dtmf command Decoded Proactive Data */
		TelSatLanguageNotificationTlv language_notification; /**< language notification command Decoded Proactive Data */
		TelSatLaunchBrowserTlv launch_browser; /**< launch browser command Decoded Proactive Data */
		TelSatOpenChannelTlv open_channel; /**< open channel command Decoded Proactive Data */
		TelSatCloseChannelTlv close_channel; /**< close channel command Decoded Proactive Data */
		TelSatReceiveChannelTlv receive_data; /**< receive data command Decoded Proactive Data */
		TelSatSendChannelTlv send_data; /**< send data command Decoded Proactive Data */
		TelSatGetChannelStatusTlv get_channel_status; /**< get channel status command Decoded Proactive Data */
		TelSatUnsupportCommandTlv unsupport_cmd; /**< unsupported command Decoded Proactive Data */
	} data; /**< Union */
}TelSatDecodedProactiveData;

 /**
 * This structure defines SAT Notification structure
 */
typedef struct {
	int decode_err_code; /**< decoded error code */
	int cmd_number; /**< Command number */
	TelSatProactiveCmdType cmd_type; /**< Command type */
	union{
		TelSatDisplayTextTlv display_text; /**< display text command Noti Proactive Data */
		TelSatGetInkeyTlv get_inkey; /**< get inkey command Noti Proactive Data */
		TelSatGetInputTlv get_input; /**< get input command Noti Proactive Data */
		TelSatMoreTimeTlv more_time; /**< more time command Noti Proactive Data */
		TelSatPlayToneTlv play_tone; /**< play tone command Noti Proactive Data */
		TelSatSetupMenuTlv setup_menu; /**< setup menu command Noti Proactive Data */
		TelSatSelectItemTlv select_item; /**< select item command Noti Proactive Data */
		TelSatSendSmsTlv send_sms; /**< send sms command Noti Proactive Data */
		TelSatSendSsTlv send_ss; /**< send ss command Noti Proactive Data */
		TelSatSendUssdTlv send_ussd; /**< send ussd command Noti Proactive Data */
		TelSatSetupCallTlv setup_call; /**< setup call command Noti Proactive Data */
		TelSatRefreshTlv refresh; /**< refresh command Noti Proactive Data */
		TelSatProvideLocalInfoTlv provide_local_info; /**< provide local info command Noti Proactive Data */
		TelSatSetupEventListTlv setup_event_list; /**< setup event list command Noti Proactive Data */
		TelSatSetupIdleModeTextTlv setup_idle_mode_text; /**< setup idle mode text command Noti Proactive Data */
		TelSatSendDtmfTlv send_dtmf; /**< send dtmf command Noti Proactive Data */
		TelSatLanguageNotificationTlv language_notification; /**< language notification command Noti Proactive Data */
		TelSatLaunchBrowserTlv launch_browser; /**< launch browser command Noti Proactive Data */
		TelSatOpenChannelTlv open_channel; /**< open channel command Noti Proactive Data */
		TelSatCloseChannelTlv close_channel; /**< close channel command Noti Proactive Data */
		TelSatReceiveChannelTlv receive_data; /**< receive data command Noti Proactive Data */
		TelSatSendChannelTlv send_data; /**< send data command Noti Proactive Data */
		TelSatGetChannelStatusTlv get_channel_status; /**< get channel status command Noti Proactive Data */
		TelSatUnsupportCommandTlv unsupport_cmd; /**< unsupported command Noti Proactive Data */
	}proactive_ind_data; /**< Union */
} TelSatNotiProactiveData;

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
 #endif /* __SAT_INTERNAL_H__ */
