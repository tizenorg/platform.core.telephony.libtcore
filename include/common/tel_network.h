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

#ifndef __TEL_NETWORK_H__
#define __TEL_NETWORK_H__

#include <tel_if.h>

/**
*  @addtogroup TAPI_NETWORK
*  @{
*
*  @file tel_network.h
*  @brief TAPI Network Interface
*/

#ifdef __cplusplus
extern "C" {
#endif

#define TELEPHONY_NETWORK_INTERFACE TELEPHONY_SERVICE".Network"

#define TEL_NETWORK_PLMN_LEN_MAX 6	/**< Maximum PLMN(MCCMNC) length */

/**
 * @enum TelNetworkResult
 * Response (Return) values.
 */
typedef enum {
	TEL_NETWORK_RESULT_SUCCESS,						/**< Operation completed successfully */
	TEL_NETWORK_RESULT_FAILURE,						/**< Operation Failed */
	TEL_NETWORK_RESULT_INVALID_PARAMETER,			/**< Invalid input parameters */
	TEL_NETWORK_RESULT_MEMORY_FAILURE,				/**< Memory allocation failed */
	TEL_NETWORK_RESULT_OPERATION_NOT_PERMITTED,	/**< Operation not permitted */
	TEL_NETWORK_RESULT_OPERATION_NOT_SUPPORTED	/**< Operation not supported */
} TelNetworkResult;

/**
 * @enum TelNetworkMode
 * Network Mode
 */
typedef enum {
	TEL_NETWORK_MODE_AUTO, /**< auto select network mode or hybrid */
	TEL_NETWORK_MODE_2G, /**< 2G: GSM, GPRS, EDGE etc */
	TEL_NETWORK_MODE_3G,	/**< 3G: UMTS, HSDPA, HSUPA etc */
	TEL_NETWORK_MODE_LTE, /**< LTE */
} TelNetworkMode;

/**
 * @enum TelNetworkSelectionMode
 * Network Selection Mode
 */
typedef enum {
	TEL_NETWORK_SELECTION_MODE_AUTOMATIC, /**< Automatic selection mode*/
	TEL_NETWORK_SELECTION_MODE_MANUAL, /**< Manual selection mode */
} TelNetworkSelectionMode;

/**
 * @enum TelNetworkAct
 * Network Access Technology
 */
typedef enum {
	TEL_NETWORK_ACT_UNKNOWN, /**< ACT is not presented */
	TEL_NETWORK_ACT_GSM, /**< GSM */
	TEL_NETWORK_ACT_GPRS, /**< GPRS */
	TEL_NETWORK_ACT_EGPRS, /**< EDGE */
	TEL_NETWORK_ACT_UMTS, /**< 3G */
	TEL_NETWORK_ACT_GSM_AND_UMTS, /**< Both GSM and UMTS available */
	TEL_NETWORK_ACT_HSDPA, /**< HSDPA */
	TEL_NETWORK_ACT_HSUPA, /**< HSUPA */
	TEL_NETWORK_ACT_HSPA, /**< HSDPA & HSUPA */
	TEL_NETWORK_ACT_LTE, /**< LTE */
} TelNetworkAct;

/**
 * @enum TelNetworkRegStatus
 * Network Registration Status
 */
typedef enum {
	TEL_NETWORK_REG_STATUS_UNREGISTERED, /**< Not registered, not searching */
	TEL_NETWORK_REG_STATUS_REGISTERED, /**< Registered, home network */
	TEL_NETWORK_REG_STATUS_SEARCHING, /**< Not regsitered, searching */
	TEL_NETWORK_REG_STATUS_DENIED, /**< Registration is denied */
	TEL_NETWORK_REG_STATUS_UNKNOWN, /**< Unknown */
	TEL_NETWORK_REG_STATUS_ROAMING, /**< Registered, roaming */
} TelNetworkRegStatus;

/**
 * @enum TelNetworkPlmnStatus
 * Network Plmn Status
 */
typedef enum
{
	TEL_NETWORK_STATUS_UNKNOWN, /**< Unknown Plmn */
	TEL_NETWORK_STATUS_AVAILABLE, /**< Available Plmn */
	TEL_NETWORK_STATUS_CURRENT, /**< Network currently registered */
	TEL_NETWORK_STATUS_FORBIDDEN, /**< Forbidden Plmn  */
} TelNetworkPlmnStatus;

/**
 * Network Registration Status Information
 */
typedef struct {
	TelNetworkRegStatus cs_status; /**< CS domain registration status */
	TelNetworkRegStatus ps_status; /**< PS domain registration status */
	TelNetworkAct act; /**< Access technology */
} TelNetworkRegStatusInfo;

/**
 * Network Cell Information
 */
typedef struct {
	unsigned int lac; /**< Location Area Code */
	unsigned int cell_id; /**< Cell id */
	unsigned int rac; /**< Routing Area Code - PS network specific */
} TelNetworkCellInfo;

/**
 * Network Registration Information
 */
typedef struct {
	TelNetworkRegStatusInfo reg_status_info; /**< Registration Status Information */
	TelNetworkCellInfo cell_info; /**< Cell Information */
} TelNetworkRegistrationInfo;

/**
 * Network Identity Information
 */
typedef struct {
	char *plmn; /**< numeric name (MCCMNC) */
	char *short_name; /**< short alphanumeric operator name */
	char *long_name; /**< long alphanumeric operator name */
} TelNetworkIdentityInfo;

/**
 * Network Selection Manual Information
 */
typedef struct {
	char *plmn; /**< numeric name (MCCMNC) */
	TelNetworkAct act; /**< Access technology */
} TelNetworkSelectManualInfo;

/**
 * Network Information
 */
typedef struct {
	TelNetworkPlmnStatus plmn_status; /**< Plmn Status */
	TelNetworkAct act; /**< Access technology */
	TelNetworkIdentityInfo network_identity; /**< Network Identity Information */
} TelNetworkInfo;

/**
 * Network Plmn List
 */
typedef struct {
	unsigned int count; /**< Network Plmn count */
	TelNetworkInfo *network_list; /**< Network list */
} TelNetworkPlmnList;

/**
 * Network Preferred Plmn Information
 */
typedef struct {
	unsigned int index; /**< Preferred Plmn list index, start from 1 */
	char *plmn; /**< numeric operator name (MCCMNC) */
	TelNetworkAct act; /**< Access technology */
} TelNetworkPreferredPlmnInfo;

/**
 * Network Preferred Plmn List
 */
typedef struct {
	unsigned int count; /**< Number of preferred plmns in the list */
	TelNetworkPreferredPlmnInfo *list; /**< Preferred Plmns List */
} TelNetworkPreferredPlmnList;

/**
 * Network Gsm Neighbour Cell Information
 */
typedef struct {
	int cell_id; /**< Cell ID (-1 indicates cell ID not present)*/
	int lac; /**< Location area code (ignored when cell_id not present) */
	int bcch; /**< Broadcast Control Channel Frequency number */
	int bsic; /**< Base Station Identification Code  */
	int rxlev; /**< Received Signal Strength level. Valid values: (0-63,99)
				* Reference: 33GPP TS 45.008 [20] subclause 8.1.4
				- Rxlev 0 is a signal strength less than -110 dBm
				- Rxlev 1 is -110 dBm to -109 dBm
				- Rxlev 2 is -109 dBm to -108 dBm
				 - ...
				- Rxlev 62 is -49 dBm to -48 dBm
				- Rxlev 63 is greater than -48 dBm
				- Rxlev 99 is not known or not detectable
			*/
} TelNetworkGsmNeighbourCellInfo;

/**
 * Network Umts Neighbour Cell Information
 */
typedef struct {
	int cell_id; /**< Cell ID (-1 indicates cell ID not present)*/
	int lac; /**< Location area code (ignored if cell_id not present)*/
	int arfcn; /**< UTRA Absolute RF Channel Number */
	int psc; /**< Primary scrambling Code	*/
	int rscp; /**< Received Signal Code Power. Valid values: (0-96, 255)
				* Reference : 3GPP TS 25.133 [95] subclause 9.1.1.3
				 and 3GPP TS 25.123 [96] subclause 9.1.1.1.3)
				- Rscp 0 is a signal strength less than -120 dBm
				- Rscp 1 is -120 dBm to -119 dBm
				- Rscp 2 is -119 dBm to -118 dBm
				- ...
				- Rscp 95 is -26 dBm to -25 dBm
				- Rscp 96 is greater than -25 dBm
				- Rscp 255 is not known or not detectable
			*/
} TelNetworkUmtsNeighbourCellInfo;

/**
 * Network Neighbour Cell Information
 */
typedef struct {
	unsigned int gsm_list_count; /**< GSM cells count */
	TelNetworkGsmNeighbourCellInfo *gsm_list; /**< GSM cells information */
	unsigned int umts_list_count; /**< UMTS cells count */
	TelNetworkUmtsNeighbourCellInfo *umts_list; /**< UMTS cells information */
} TelNetworkNeighbourCellInfo;

/**
 * Network Nitz Information
 */
typedef struct {
	unsigned int year; /**< Last two digits of year */
	unsigned int month; /**< Month */
	unsigned int day; /**< Day */
	unsigned int hour; /**< Hour */
	unsigned int minute; /**< Minute */
	unsigned int second; /**< Second */
	int gmtoff; /**< GMT Offset */
	gboolean isdst; /**< Daylight Saving Time Valid */
	int dstoff; /**< Daylight Saving Time Offset */
	char *plmn; /**< numeric name (MCCMNC) */
} TelNetworkNitzInfoNoti;

/**
 *  \defgroup TAPI_EVENT TAPI Event IDs
 *  @{
 */

/**
 *  \defgroup TAPI_EVENT_NETWORK Network Event IDs
 *  @{
 */

/**
 *  @hideinitializer
 *  @brief Network registration status
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_NETWORK_REGISTRATION_STATUS
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_NETWORK_REGISTRATION_STATUS
 *  @retval data reference to \ref TelNetworkRegStatusInfo structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_NETWORK_REGISTRATION_STATUS	TELEPHONY_NETWORK_INTERFACE":RegistrationStatus"

/**
 *  @hideinitializer
 *  @brief Network cell information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_NETWORK_CELL_INFO
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_NETWORK_CELL_INFO
 *  @retval data reference to \ref TelNetworkCellInfo structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_NETWORK_CELL_INFO			TELEPHONY_NETWORK_INTERFACE":CellInfo"

/**
 *  @hideinitializer
 *  @brief Network Identity information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_NETWORK_IDENTITY
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_NETWORK_IDENTITY
 *  @retval data reference to \ref TelNetworkIdentityInfo structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_NETWORK_IDENTITY			TELEPHONY_NETWORK_INTERFACE":Identity"

/**
 *  @hideinitializer
 *  @brief Network RSSI
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_NETWORK_RSSI
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_NETWORK_RSSI
 *  @retval data \ref unsigned int rssi
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_NETWORK_RSSI				TELEPHONY_NETWORK_INTERFACE":Rssi"

/**
 *  @hideinitializer
 *  @brief Network time information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_NETWORK_TIME_INFO
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_NETWORK_TIME_INFO
 *  @retval data reference to \ref TelNetworkNitzInfoNoti structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_NETWORK_TIME_INFO			TELEPHONY_NETWORK_INTERFACE":TimeInfo"

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

/** @}*/

#endif /* __TEL_NETWORK_H__ */

