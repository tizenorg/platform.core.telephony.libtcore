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

#ifndef __TEL_SMS_H__
#define __TEL_SMS_H__

#include <tel_if.h>

/**
*  @addtogroup TAPI_SMS
*  @{
*
*  @file tel_sms.h
*  @brief TAPI sms Interface
*/

#ifdef __cplusplus
extern "C"
{
#endif

#define TELEPHONY_SMS_INTERFACE         TELEPHONY_SERVICE".Sms"

#define TEL_SMS_SCA_LEN_MAX		20	/**< Maximum length of SCA (in string format)*/

#define TEL_SMS_CB_DATA_SIZE_MAX	88	/**< Maximum CB Message Size. Cell broadcast message data [Refer 3GPP TS 23.041 9.4.1] */
#define TEL_SMS_ETWS_DATA_SIZE_MAX	1252 /**< Maximum ETWS Message Size */

#define TEL_SMS_GSM_MSG_NUM_MAX		255	/**< Maximum GSM SMS message number*/
#define TEL_SMS_GSM_CBMI_LIST_SIZE_MAX	50	/**< Maximum GSM SMS CBMI list size*/
#define TEL_SMS_SMDATA_SIZE_MAX		164	/**< Maximum SMS data size that can be stored*/

/**
 * @enum TelSmsMsgStatus
 * This enumeration defines the sms Response (Result) codes.
 */
typedef enum {
	TEL_SMS_STATUS_MT_READ,	/**< MT message, stored and read */
	TEL_SMS_STATUS_MT_UNREAD,	/**< MT message, stored and unread */
	TEL_SMS_STATUS_MO_SENT,	/**< MO message, stored and  sent */
	TEL_SMS_STATUS_MO_NOT_SENT,	/**< MO message, stored but not sent */
	TEL_SMS_STATUS_MO_DELIVERED, /**< delivered destination */
	TEL_SMS_STATUS_MO_DELIVERY_NOT_CONFIRMED, /**< Service centre forwarded message but is unable to confirm delivery*/
	TEL_SMS_STATUS_REPLACED /**< Message has been replaced*/
} TelSmsMsgStatus;


/**
* @enum TelSmsCbMsgType
* This enumeration defines the different CB message types.
*/
typedef enum {
	TEL_SMS_CB_MSG_GSM,	/**< GSM Cell broadcast message */
	TEL_SMS_CB_MSG_UMTS	/**< UMTSCell broadcast message */
} TelSmsCbMsgType;

/**
* @enum TelSmsEtwsMsgType
* This enumeration defines the different ETWS message types.
*/
typedef enum {
	TEL_SMS_ETWS_PRIMARY,	 /**< Primary ETWS message */
	TEL_SMS_ETWS_SECONDARY_GSM,	/**< GSM Secondary ETWS message  */
	TEL_SMS_ETWS_SECONDARY_UMTS	/**< UMTS Secondary ETWS message  */
} TelSmsEtwsMsgType;

/**
 * @enum TelSmsResult
 * This enum defines the different SMS error causes that come in the
 * sent status acknowledgement/notification after sending a message to the network
 */
typedef enum {
	TEL_SMS_RESULT_SUCCESS,	/**< Operation completed successfully */
	TEL_SMS_RESULT_FAILURE,	/**< Operation Failed */
	TEL_SMS_RESULT_INVALID_PARAMETER,	/**< Invalid input parameters */
	TEL_SMS_RESULT_MEMORY_FAILURE,	 /**< Memory allocation failed */
	TEL_SMS_RESULT_OPERATION_NOT_SUPPORTED,	/**< Operation not supported */
	TEL_SMS_RESULT_UNKNOWN_FAILURE,	/**< Unknown failure */

	TEL_SMS_RESULT_INVALID_MANDATORY_INFO,	/**< Invalid format for some parameters passed in Data package information(TPDU)*/
	TEL_SMS_RESULT_NETWORK_CONGESTION,	/**< SMS SIM operation cannot be performed due to Network Congestion */
	TEL_SMS_RESULT_SIM_FAILURE,	/**< SMS SIM operation cannot be performed due to SIM failure */
	TEL_SMS_RESULT_NETWORK_FAILURE,	/**< SMS SIM operation cannot be performed due to Network failure */
	TEL_SMS_RESULT_OPERATION_NOT_PERMITTED	/** Operation not permitted */
} TelSmsResult;

/**
 * This structure defines the properties of a dialing number
 * Type of Number, Numbering Plan Indicator and the actual number.
 */
typedef struct {
	unsigned char ton; /**< Type of number*/
	unsigned char npi; /**<Numbering plan identification*/
	char number[TEL_SMS_SCA_LEN_MAX + 1]; /**< Destination address */
} TelSmsSca;

/**
 * This structure defines the different parameters of  CB configuration
 */
typedef struct {
	unsigned short from_msg_id; /**< Starting point of the range of CBS message ID */
	unsigned short to_msg_id; /**< Ending point of the range of CBS message ID */
	gboolean selected;	/**< 0x00 . Not selected. 0x01 . Selected */
} TelSmsCbMsgInfo3gpp;

/**
 * This structure defines the different parameters of  CB configuration informations
 */
typedef struct {
	gboolean cb_enabled; /**< CB service state. If cb_enabled is true then cell broadcast service will be enabled and
						* underlying modem will enable CB Channel to receiving CB messages. Otherwise CB service
						* will be disabled, underlying modem will deactivate the CB channel. (enabled/disabled) */
	unsigned int msg_id_range_cnt;  /**< Range of CB message ID count */
	TelSmsCbMsgInfo3gpp msg_ids[TEL_SMS_GSM_CBMI_LIST_SIZE_MAX]; /**< Range of CB message ID information */
} TelSmsCbConfigInfo;

/**
 * This structure defines different fields involved in setting the parameters of
 * a particular sms in EFsmsp.
 */
typedef struct {
	unsigned int index; /**< Index */
	TelSmsSca sca; /**< Service Centre Address info */
	unsigned short vp; /**< Validity Period */
} TelSmsParamsInfo;

/**
 * This structure defines different fields involved in getting the sms parameters
 */
typedef struct {
	unsigned int count; /**< SMS parameter count */
	TelSmsParamsInfo *params; /**< SMS parameter informations */
} TelSmsParamsInfoList;

/**
 * This structure defines the fields related to an Sms like SIM index, TPDU
 * and length.
 */
typedef struct {
	TelSmsSca sca; /**< Service Centre Address info */
	unsigned int tpdu_length; /**< Size of array tpdu (which is actual TPDU message) */
	unsigned char tpdu[TEL_SMS_SMDATA_SIZE_MAX]; /**< SMS TPDU message */
} TelSmsDatapackageInfo;

/**
 *This structure defines the data Related to MessageStatus and SMS Data Stored.
 */
typedef struct {
	TelSmsMsgStatus status; /**< Message status.*/
	TelSmsDatapackageInfo data; /**<SMS message data*/
} TelSmsSimDataInfo;

/**
 *This structure defines the data Related to SMS Data to be send.
 */
typedef struct {
	gboolean more_msgs; /**< More messages to be send exist */
	TelSmsDatapackageInfo send_data; /**<SMS message data*/
} TelSmsSendInfo;

/**
 *This structure defines the data Related to set message status
 */
typedef struct {
	unsigned int index; /**< Index where message status to be set */
	TelSmsMsgStatus status; /**< Message status.*/
} TelSmsStatusInfo;

/**
 * @enum TelSmsDeliveryReport
 * This enum defines the error causes for delivery report
 */
typedef enum {
	TEL_SMS_DELIVERY_REPORT_SUCCESS,  /**<Message was sent successfully*/
	TEL_SMS_DELIVERY_REPORT_MEMORY_ERROR,	/**< Memory capacity exceeded/memory full*/
	TEL_SMS_DELIVERY_REPORT_UNSPECIFIED_ERROR	/**<Unspecified error */
} TelSmsDeliveryReport;

/**
 *This structure defines the SMS delivery report information
 */
typedef struct {
	TelSmsDeliveryReport report;	/**<Delivery report response types*/
	TelSmsDatapackageInfo data;	/**<SMS data */
} TelSmsDeliverReportInfo;

/**
 * This structure defines the different parameters that are related to the message count
 * in a particular memory(Phone/SIM)
 */
typedef struct {
	unsigned int total_count;	/**< Total count of messages stored in SIM*/
	unsigned int used_count;	/**< Stored message count in SIM in SIM*/
	unsigned int index_list[TEL_SMS_GSM_MSG_NUM_MAX]; /**< Message index list stored in SIM. And the maximum
													   *size of this array can be of totalCount.This array contains the
													   *list of index values in which the messages are stored.
													   *i.e. index_list[totalcount] = [2,3] denotes that indexs
													   *2 and 3 are stored(valid) and others not(empty).
													   */
} TelSmsStoredMsgCountInfo;

/**
 * This structure defines a cell broadcast message.
 */
typedef struct {
	TelSmsCbMsgType cb_type;	/**< Cell Broadcast  message type */
	unsigned int length;	/**<Length of cb_data (which is actual TPDU message) */
	unsigned char *cb_data;	 /**<Cell broadcast message data[Refer 3GPP TS 23.041 9.4.1]*/
} TelSmsCbMsgInfo;

/**
 * This structure defines a ETWS message.
 */
typedef struct {
	TelSmsEtwsMsgType etws_type;	/**< Cell Broadcast  message type */
	unsigned int length;	/**<Length of etws_data (which is actual TPDU message) */
	unsigned char *etws_data;	/**<Cell broadcast message data[Refer 3GPP TS 23.041 9.4.1]*/
} TelSmsEtwsMsgInfo;

/**
 *  \defgroup TAPI_EVENT TAPI Event IDs
 *  @{
 */

/**
 *  \defgroup TEL_EVENT_SMS SMS Event IDs
 *  @{
 */

/**
 *  @hideinitializer
 *  @brief Incoming Message
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SMS_INCOM_MSG
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SMS_INCOM_MSG
 *  @retval data reference to \ref TelSmsDatapackageInfo structure
 *  @retval user_data user data passed while registering the event
 */
 #define TEL_NOTI_SMS_INCOM_MSG			TELEPHONY_SMS_INTERFACE":IncomingMsg"

/**
 *  @hideinitializer
 *  @brief Incoming CB Message
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SMS_CB_INCOM_MSG
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SMS_CB_INCOM_MSG
 *  @retval data reference to \ref TelSmsCbMsgInfo structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SMS_CB_INCOM_MSG			TELEPHONY_SMS_INTERFACE":IncomingCbMsg"

/**
 *  @hideinitializer
 *  @brief Incoming ETWS Message
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SMS_ETWS_INCOM_MSG
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SMS_ETWS_INCOM_MSG
 *  @retval data reference to \ref TelSmsEtwsMsgInfo structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SMS_ETWS_INCOM_MSG		TELEPHONY_SMS_INTERFACE":IncomingEtwsMsg"

/**
 *  @hideinitializer
 *  @brief SIM memory status for SMS
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SMS_SIM_MEMORY_STATUS
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SMS_SIM_MEMORY_STATUS
 *  @retval data <b>TRUE</b>: Memory available\n <b>FALSE</b>: Memory NOT available
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SMS_SIM_MEMORY_STATUS		TELEPHONY_SMS_INTERFACE":SimMemoryStatus"

/**
 *  @hideinitializer
 *  @brief SMS init status
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_SMS_INIT_STATUS
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_SMS_INIT_STATUS
 *  @retval data <b>TRUE</b>: SMS is Initialized\n <b>FALSE</b>: SMS is NOT initialized
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_SMS_INIT_STATUS			TELEPHONY_SMS_INTERFACE":InitStatus"

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif  /* __TEL_SMS_H__ */
