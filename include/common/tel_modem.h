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

#ifndef __TEL_MODEM_H__
#define __TEL_MODEM_H__

#include <tel_if.h>

/**
*  @addtogroup TAPI_MODEM
*  @{
*
*  @file tel_modem.h
*  @brief TAPI Modem Interface
*/

#ifdef __cplusplus
extern "C" {
#endif

#define TELEPHONY_MODEM_INTERFACE   TELEPHONY_SERVICE".Modem"

#define TEL_MODEM_IMEI_LENGTH_MAX 15	/**< Maximum IMEI length */

#define TEL_MODEM_VERSION_LENGTH_MAX 32	/**< Maximum version information length */

/**
 * @enum TelModemResult
 * Response (Result) codes
 */
typedef enum {
	TEL_MODEM_RESULT_SUCCESS,					/**< Operation completed successfully */
	TEL_MODEM_RESULT_FAILURE,					/**< Operation Failed */
	TEL_MODEM_RESULT_INVALID_PARAMETER,		/**< Invalid input parameters */
	TEL_MODEM_RESULT_MEMORY_FAILURE,			/**< Memory allocation failed */
	TEL_MODEM_RESULT_OPERATION_NOT_SUPPORTED,/**< Operation not supported */
	TEL_MODEM_RESULT_OPERATION_NOT_PERMITTED,	/**< Operation not permitted */
	TEL_MODEM_RESULT_UNKNOWN_FAILURE			/**< Unknown failure */
} TelModemResult;

/**
 * @enum TelModemPowerStatus
 * Power Status
 */
typedef enum {
	TEL_MODEM_POWER_OFF, /**< Modem power off */
	TEL_MODEM_POWER_ON, /**< Modem power on */
	TEL_MODEM_POWER_ERROR /**< Modem power error */
} TelModemPowerStatus;

/**
 * @enum TelModemFlightModeStatus
 * Flight Mode Status
 */
typedef enum {
	TEL_MODEM_FLIGHT_MODE_UNKNOWN, /**< Status is unknown at the moment */
	TEL_MODEM_FLIGHT_MODE_OFF, /**< Flight mode is OFF. Device is Online */
	TEL_MODEM_FLIGHT_MODE_ON /**< Flight mode is ON. Device is in Low Power */
} TelModemFlightModeStatus;

/**
 * Version Information
 */
typedef struct {
	char software_version[TEL_MODEM_VERSION_LENGTH_MAX + 1]; /**< Software Version */
	char hardware_version[TEL_MODEM_VERSION_LENGTH_MAX + 1]; /**< Hardware Version */
	char calibration_date[TEL_MODEM_VERSION_LENGTH_MAX + 1]; /**< Calibration Date */
	char product_code[TEL_MODEM_VERSION_LENGTH_MAX + 1]; /**< Product Code */
} TelModemVersion;

/**
 *  \defgroup TAPI_EVENT TAPI Event IDs
 *  @{
 */

/**
 *  \defgroup TAPI_EVENT_MODEM Modem Event IDs
 *  @{
 */

/**
 *  @hideinitializer
 *  @brief Power Status
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_MODEM_POWER_STATUS
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_MODEM_POWER_STATUS
 *  @retval data \ref TelModemPowerStatus
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_MODEM_POWER_STATUS		TELEPHONY_MODEM_INTERFACE":power_status"

/**
 *  @hideinitializer
 *  @brief Flight Mode Status
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_MODEM_FLIGHT_MODE_STATUS
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_MODEM_FLIGHT_MODE_STATUS
 *  @retval data <b>TRUE</b>: Flight mode is ON\n <b>FALSE</b>: Flight mode is OFF
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_MODEM_FLIGHT_MODE_STATUS		TELEPHONY_MODEM_INTERFACE":flight_mode_status"

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

/** @}*/

#endif /* __TEL_MODEM_H__ */
