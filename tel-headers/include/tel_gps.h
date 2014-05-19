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

#ifndef __TEL_GPS_H__
#define __TEL_GPS_H__

#include <tel_if.h>

/**
*  @addtogroup TAPI_GPS
*  @{
*
*  @file tel_gps.h
*  @brief TAPI GPS Interface
*/

#ifdef __cplusplus
extern "C" {
#endif

#define TELEPHONY_GPS_INTERFACE TELEPHONY_SERVICE".Gps"

/**
 * @enum TelGpsResult
 * Response (Return) values.
 */
typedef enum {
	TEL_GPS_RESULT_SUCCESS,						/**< Operation completed successfully */
	TEL_GPS_RESULT_FAILURE,						/**< Operation Failed */
	TEL_GPS_RESULT_INVALID_PARAMETER,			/**< Invalid input parameters */
	TEL_GPS_RESULT_MEMORY_FAILURE,				/**< Memory allocation failed */
	TEL_GPS_RESULT_OPERATION_NOT_SUPPORTED,	/**< Operation not supported */
	TEL_GPS_RESULT_OPERATION_NOT_PERMITTED,	/**< Operation not permitted */
	TEL_GPS_RESULT_UNKNOWN_FAILURE				/**< Unknown failure */
} TelGpsResult;

/**
 * GPS info.
 */
typedef struct {
	unsigned int data_len;		/**< Data length of GPS info */
	unsigned char *data;		/**< GPS information */
} TelGpsDataInfo;

/**
 *  \defgroup TAPI_EVENT TAPI Event IDs
 *  @{
 */

/**
 *  \defgroup TEL_EVENT_GPS GPS Event IDs
 *  @{
 */

/**
 *  @hideinitializer
 *  @brief GPS assist data
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_GPS_ASSIST_DATA
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_GPS_ASSIST_DATA
 *  @retval data reference to GPS data (as a string)
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_GPS_ASSIST_DATA			TELEPHONY_GPS_INTERFACE":AssistData"

/**
 *  @hideinitializer
 *  @brief GPS position information
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_GPS_MEASURE_POSITION
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_GPS_MEASURE_POSITION
 *  @retval data reference to GPS data (as a string)
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_GPS_MEASURE_POSITION		TELEPHONY_GPS_INTERFACE":MeasurePosition"

/**
 *  @hideinitializer
 *  @brief GPS reset assist data
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_GPS_RESET_ASSIST_DATA
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_GPS_RESET_ASSIST_DATA
 *  @retval data None
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_GPS_RESET_ASSIST_DATA		TELEPHONY_GPS_INTERFACE":ResetAssistData"

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif /* __TEL_GPS_H__ */
