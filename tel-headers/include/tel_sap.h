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


#ifndef __TEL_SAP_H__
#define __TEL_SAP_H__

#include <tel_if.h>

/**
*  @addtogroup TAPI_SAP
*  @{
*
*  @file tel_sap.h
*  @brief TAPI SAP Interface
*/

#ifdef __cplusplus
extern "C" {
#endif

#define TELEPHONY_SAP_INTERFACE         TELEPHONY_SERVICE".Sap"

#define TEL_SAP_APDU_LEN_MAX		261	/**< APDU max command length */
#define TEL_SAP_APDU_RESP_LEN_MAX	258	/**< APDU max response length */
#define TEL_SAP_ATR_LEN_MAX		33	/**< Answer to Reset data max length */

/**
 * @enum TelSapResult
 * This enum gives the SAP result information.
 */

typedef enum {
	TEL_SAP_RESULT_SUCCESS,			/**<  operation successfully completed*/
	TEL_SAP_RESULT_FAILURE_NO_REASON,	/**<  no reason*/
	TEL_SAP_RESULT_CARD_NOT_ACCESSIBLE,	/**<  not accessible*/
	TEL_SAP_RESULT_CARD_ALREADY_POWERED_OFF,/**<  card already powered off*/
	TEL_SAP_RESULT_CARD_REMOVED,		/**<  card removed*/
	TEL_SAP_RESULT_CARD_ALREADY_POWERED_ON,	/**<  card already powered on*/
	TEL_SAP_RESULT_DATA_NOT_AVAILABLE,	/**<  data not available*/
	TEL_SAP_RESULT_NOT_SUPPORTED,		/**<  not supported*/
	TEL_SAP_RESULT_UNABLE_TO_ESTABLISH, 	/**<  unable to establish connection*/
	TEL_SAP_RESULT_NOT_SUPPORT_MAX_SIZE,	/**<  server does not support message length that client want send*/
	TEL_SAP_RESULT_TOO_SMALL_MAX_SIZE,	/**<  client wants to connect with very small message length which is not supported by Server */
	TEL_SAP_RESULT_ONGOING_CALL,		/**<  server cannot reset the SIM due to an ongoing call */
	TEL_SAP_RESULT_OPERATION_NOT_PERMITTED	/** Operation not permitted */
} TelSapResult;

/**
 * @enum TelSapPowerMode
 * This enum gives the SAP message Ids between SAP client and SAP server.
 */
typedef enum {
	TEL_SAP_SIM_POWER_ON_REQ,	/**< SAP Client request for Power ON SIM off in Server */
	TEL_SAP_SIM_POWER_OFF_REQ,	/**< SAP Client request for Power OFF SIM off in Server */
	TEL_SAP_SIM_RESET_REQ,		/**< SAP Client request for reset SIM in Server */
} TelSapPowerMode;

/**
 * @enum TelSapDisconnectType
 * This enum gives the SAP disconnect type information.
 */
typedef enum {
	TEL_SAP_DISCONNECT_TYPE_GRACEFUL,	/**< disconnection procedure ends after finishing current work */
	TEL_SAP_DISCONNECT_TYPE_IMMEDIATE	/**< disconnection procedure ends immediately */
} TelSapDisconnectType;

/**
 * @enum TelSapCardStatus
 * This enum gives the SIM card status if server`s status changed about connection with subscription module
 */
typedef enum {
	TEL_SAP_CARD_STATUS_UNKNOWN,		/**<  SAP server status - unknown*/
	TEL_SAP_CARD_STATUS_RESET, 		/**<  SAP server status - reset*/
	TEL_SAP_CARD_STATUS_NOT_ACCESSIBLE, 	/**<  SAP server status - not accessible*/
	TEL_SAP_CARD_STATUS_REMOVED, 		/**<  SAP server status - removed*/
	TEL_SAP_CARD_STATUS_INSERTED, 		/**<  SAP server status - inserted*/
	TEL_SAP_CARD_STATUS_RECOVERED 		/**<  SAP server status - recovered*/
} TelSapCardStatus;

/**
 * @enum TelSimSapProtocol
 * This enum gives SAP transport protocol type
 */
typedef enum {
	TEL_SIM_SAP_PROTOCOL_T0,	/**< T = 0, character*/
	TEL_SIM_SAP_PROTOCOL_T1		/**< T = 1, block*/
} TelSimSapProtocol;

/**
 * This data structure gives the SAP ATR response data information.
 */
typedef struct {
	unsigned int atr_len; /**<  SAP ATR response data length */
	unsigned char atr[TEL_SAP_ATR_LEN_MAX]; /**<  SAP ATR response data */
} TelSapAtr;

/**
 * This data structure is used to transfer the SAP APDU command.
 */
typedef struct {
	unsigned int apdu_len;  /**<  SAP APDU length */
	unsigned char apdu[TEL_SAP_APDU_LEN_MAX];  /**<  SAP APDU data */
} TelSapApdu;

/**
 * This data structure gives the SAP APDU response.
 */
typedef struct {
	unsigned int apdu_resp_len; /**<  SAP APDU response data length */
	unsigned char apdu_resp[TEL_SAP_APDU_RESP_LEN_MAX]; /**<  SAP APDU response data */
} TelSapApduResp;

/**
 *  \defgroup TAPI_EVENT TAPI Event IDs
 *  @{
 */

/**
 *  \defgroup TEL_EVENT_SAP SAP Event IDs
 *  @{
 */

/**
 *  @hideinitializer
 *  @brief SAP status
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAP_STATUS
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAP_STATUS
 *  @retval data \ref TelSapCardStatus
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAP_STATUS				TELEPHONY_SAP_INTERFACE":Status"

/**
 *  @hideinitializer
 *  @brief SAP disconnect type
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SAP_DISCONNECT
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SAP_DISCONNECT
 *  @retval data \ref TelSapDisconnectType
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SAP_DISCONNECT			TELEPHONY_SAP_INTERFACE":Disconnect"

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

/** @}*/

#endif /* __TEL_SAP_H__ */
