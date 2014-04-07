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

#ifndef __TEL_CALL_H__
#define __TEL_CALL_H__

#include <glib.h>
#include <tel_if.h>

/**
*  @addtogroup TAPI_CALL
*  @{
*
*  @file tel_call.h
*  @brief TAPI Call Interface
*/

#ifdef __cplusplus
extern "C" {
#endif

#define TELEPHONY_CALL_INTERFACE TELEPHONY_SERVICE".Call"

#define TEL_CALL_CALLING_NAME_LEN_MAX		82	/**< The maximum length of the string for calling party name.   */
#define TEL_CALL_CALLING_NUMBER_LEN_MAX		82	/**< Maximum Dialling Digit Length  */
#define TEL_CALL_MAX_CALLS_COUNT		6			/**< Maximum number of call can be possible at a time */
#define TEL_CALL_MAX_SOUND_EQ_PARAMETER_SIZE	6	/**< Maximum number of sound equalization parameter size */
#define TEL_CALL_MAX_VOLUME_INFO_RECORD		20	/**< Maximum number of record for volume information */

/**
 * @enum TelCallResult
 * Response (Return) values.
 */
typedef enum {
	TEL_CALL_RESULT_SUCCESS,						/**< Operation completed successfully */
	TEL_CALL_RESULT_FAILURE,						/**< Operation Failed */
	TEL_CALL_RESULT_INVALID_PARAMETER,			/**< Invalid input parameters */
	TEL_CALL_RESULT_MEMORY_FAILURE,				/**< Memory allocation failed */
	TEL_CALL_RESULT_OPERATION_NOT_SUPPORTED,	/**< Operation not supported */
	TEL_CALL_RESULT_FDN_RESTRICTED,				/**< FDN calls only */
	TEL_CALL_RESULT_NETWORK_BUSY,				/**< Network busy */
	TEL_CALL_RESULT_SERVICE_NOT_ALLOWED,		/**< Service is not allowed */
	TEL_CALL_RESULT_OPERATION_NOT_PERMITTED	/**< Operation not permitted */
} TelCallResult;

/**
 * @enum TelCallEndCause
 * Call End Cause.
 */
typedef enum {
	TEL_CALL_END_CAUSE_NONE,							/**< No Cause */
	TEL_CALL_END_CAUSE_FAILED,						/**< Call failed */
	TEL_CALL_END_CAUSE_UNASSIGNED_NUMBER,			/**< Unassigned Number */
	TEL_CALL_END_CAUSE_NO_ROUTE_TO_DEST,			/**< No Route to Destination */
	TEL_CALL_END_CAUSE_OPERATOR_DETERMINED_BARRING,	/**< Operator Determined Barring */
	TEL_CALL_END_CAUSE_NORMAL_CALL_CLEARING,		/**< Normal Call Clearing */
	TEL_CALL_END_CAUSE_USER_BUSY,					/**< User Busy */
	TEL_CALL_END_CAUSE_NO_USER_RESPONDING,		/**< No user Responding */
	TEL_CALL_END_CAUSE_USER_ALERTING_NO_ANSWER,		/**< User Alerting no Answer */
	TEL_CALL_END_CAUSE_CALL_REJECTED,				/**< Call Rejected */
	TEL_CALL_END_CAUSE_NUMBER_CHANGED,				/**< Number Changed */
	TEL_CALL_END_CAUSE_DESTINATION_OUT_OF_ORDER,	/**< Destination out of Order */
	TEL_CALL_END_CAUSE_INVALID_NUMBER_FORMAT,		/**< Invalid Number Format */
	TEL_CALL_END_CAUSE_FACILITY_REJECTED,			/**< Facility Rejected */
	TEL_CALL_END_CAUSE_NO_CIRCUIT_CHANNEL_AVAILABLE,	/**< No Circuit Channel Available */
	TEL_CALL_END_CAUSE_NETWORK_OUT_OF_ORDER,			/**< Network out of Order */
	TEL_CALL_END_CAUSE_TEMPORARY_FAILURE,				/**< Temporary Failure */
	TEL_CALL_END_CAUSE_SWITCHING_EQUIPMENT_CONGESTION,	/**< Switching Equipment Congestion */
	TEL_CALL_END_CAUSE_REQUESTED_CIRCUIT_CHANNEL_NOT_AVAILABLE,	/**< Requested Circuit channel not available */
	TEL_CALL_END_CAUSE_REQUESTED_FACILITY_NOT_SUBSCRIBED,		/**< Requested facility not subscribed */
	TEL_CALL_END_CAUSE_ACM_GEQ_ACMMAX,			/**< ACM equal to greater than ACMAX */
	TEL_CALL_END_CAUSE_IMEI_NOT_ACCEPTED,			/**< IMEI not accepted */
	TEL_CALL_END_CAUSE_NETWORK_FAILURE,		/**< Network unavailable */
	TEL_CALL_END_CAUSE_CONGESTION,			/**< Congestion */
	TEL_CALL_END_CAUSE_SERVICE_OPTION_OUT_OF_ORDER, /**< Service option temporarily out of order */
	TEL_CALL_END_CAUSE_ACCESS_CLASS_BLOCKED,		/**< Access class blocked */
} TelCallEndCause;

/**
 * @enum TelCallState
 * Call States.
 */
typedef enum {
	TEL_CALL_STATE_IDLE,		/**< Call is in idle state - i.e. no call */
	TEL_CALL_STATE_ACTIVE,		/**< Call is in connected and conversation state */
	TEL_CALL_STATE_HELD,		/**< Call is in held state */
	TEL_CALL_STATE_DIALING,		/**< Call is in dialing state */
	TEL_CALL_STATE_ALERT,		/**< Call is in alerting state */
	TEL_CALL_STATE_INCOMING,	/**< Call is in incoming state */
	TEL_CALL_STATE_WAITING,		/**< Call is in waiting state */
} TelCallState;

/**
 * @enum TelCallActiveLine
 * IN GSM ONLY: Call Active Line.
 */
typedef enum {
	TEL_CALL_ACTIVE_LINE1, 		/**< Line 1 */
	TEL_CALL_ACTIVE_LINE2		/**< Line 2 */
} TelCallActiveLine;

/**
 * @enum TelCallAnswerType
 * Call Answer Types.
 */
typedef enum {
	TEL_CALL_ANSWER_ACCEPT,			/**< Used to answer an incoming call when there are no current active calls. */
	TEL_CALL_ANSWER_REJECT,			/**< Used to reject the incoming call */
	TEL_CALL_ANSWER_REPLACE,		/**< Release current active call and accept the incoming call */
	TEL_CALL_ANSWER_HOLD_AND_ACCEPT,	/**< Hold the current active call, and accept the wating call */
} TelCallAnswerType;

/**
 * @enum TelCallEndType
 * Call End Types.
 */
typedef enum {
	TEL_CALL_END,				/**< Used to end single call with call handle */
	TEL_CALL_END_ALL,			/**< Used to end all calls */
	TEL_CALL_END_ACTIVE_ALL,		/**< Used to end all active calls */
	TEL_CALL_END_HOLD_ALL,			/**< Used to end all hold calls */
} TelCallEndType;

/**
 * @enum TelCallType
 * Call Type.
 */
typedef enum {
	TEL_CALL_TYPE_VOICE,	/**< Voice call type. */
	TEL_CALL_TYPE_VIDEO,	/**< Video call type  */
	TEL_CALL_TYPE_E911	/**< Emergency call type. */
} TelCallType;

/**
 * @enum TelCallActiveState
 * Current Call State.
 */
typedef enum {
	TEL_CALL_CONNECT_IDLE,		/**< Call is in Idle state. */
	TEL_CALL_CONNECT_ACTIVE,	/**< Call is in Active state.*/
	TEL_CALL_CONNECT_HELD		/**< Call is in Held state. */
} TelCallActiveState;

/**
 * @enum TelCallCliValidity
 * CLI(Calling Line Identification) Validity.
 */
typedef enum {
	TEL_CALL_CLI_VALIDITY_VALID,				/**< CLI is valid */
	TEL_CALL_CLI_VALIDITY_WITHHELD,			/**< CLI is withheld ("Rejected by user")*/
	TEL_CALL_CLI_VALIDITY_NOT_AVAILABLE, 		/**< CLI is not available ("Interaction with other service" or "Unavailable")*/
	TEL_CALL_CLI_VALIDITY_NOT_AVAILABLE_PAYPHONE, 	/**< CLI is not available ("Coin line/payphone")*/
} TelCallCliValidity;

/**
 * @enum TelCallCniValidity
 * CNI(Calling Name Identification) Validity.
 */
typedef enum {
	TEL_CALL_CNI_VALIDITY_VALID,				/**< CLI is valid */
	TEL_CALL_CNI_VALIDITY_WITHHELD,			/**< CLI is withheld */
	TEL_CALL_CNI_VALIDITY_NOT_AVAILABLE, 		/**< CLI is not available */
} TelCallCniValidity;

/**
 * @enum TelCallEmergencyCategory
 * Emergency Call Category.
 */
typedef enum {
	TEL_CALL_ECC_DEFAULT = 0x00,		/**< ECC is default */
	TEL_CALL_ECC_POLICE = 0x01,		/**< ECC is police */
	TEL_CALL_ECC_AMBULANCE = 0x02,	/**< ECC is ambulance */
	TEL_CALL_ECC_FIREBRIGADE = 0x04,	/**< ECC is fire station */
	TEL_CALL_ECC_MARINEGUARD = 0x08,	/**< ECC is marine gaurd */
	TEL_CALL_ECC_MOUNTAINRESCUE = 0x10,	/**< ECC is mountaion rescue */
	TEL_CALL_ECC_MANUAL_ECALL = 0x20,	/**< ECC is manual Ecall */
	TEL_CALL_ECC_AUTO_ECALL = 0x40	/**< ECC is Auto Ecall */
} TelCallEmergencyCategory;

/**
 * Call Dial Info.
 */
typedef struct {
	TelCallType call_type;					/**< Sets type of call (voice, data, video, emergency) */
	TelCallEmergencyCategory ecc;			/**< In case of emergency call, may provide category instead of number */
	char number[TEL_CALL_CALLING_NUMBER_LEN_MAX + 1];	/**< A string containing the destination phone number. This follows the dial number format.*/
} TelCallDial;

/**
 * Call End Info.
 */
typedef struct {
	unsigned int call_id;			/**< Unique id for referring the call to release */
	TelCallEndType end_type;		/**< End type used */
} TelCallEnd;

/**
 * Call Status Info.
 */
typedef struct {
	unsigned int call_id;					/**< Unique id for referring the call */

	TelCallType call_type;					/**< Specifies type of call (voice, data, emergency) */
	TelCallState call_state;					/**< Current Call state */

	gboolean mo_call;					/**< TRUE for MO Call; FALSE if MT call*/
	gboolean mpty;						/**< TRUE for Multi-party Call; FALSE if not Multi-party Call*/

	TelCallCliValidity cli_validity;				/**< Calling Line Identificatoin (only if mobile terminated call) */
	char number[TEL_CALL_CALLING_NUMBER_LEN_MAX + 1];	/**< Mobile Number */

	TelCallCniValidity cni_validity;				/**< Calling Name Identification (only if mobile terminated call) */
	char name[TEL_CALL_CALLING_NAME_LEN_MAX + 1];		/**< Caller name */

	gboolean forward;					/**< TRUE if call is forwared (only if mobile terminated call) */

	TelCallActiveLine active_line;				/**< Active line */
} TelCallStatus;

/**
 * All Call Status Info.
 */
typedef struct {
	unsigned int count;	/**< Number of calls */
	TelCallStatus status[TEL_CALL_MAX_CALLS_COUNT];	/**< Call Status Info */
} TelCallStatusList;

/**
 * Incoming Call Info.
 */
typedef struct {
	unsigned int call_id;					/**< Unique id for referring the call */

	TelCallCliValidity cli_validity;				/**< Calling Line Identificatoin (only if mobile terminated call) */
	char number[TEL_CALL_CALLING_NUMBER_LEN_MAX + 1];	/**< Mobile Number */

	TelCallCniValidity cni_validity;				/**< Calling Name Identification (only if mobile terminated call) */
	char name[TEL_CALL_CALLING_NAME_LEN_MAX + 1];		/**< Caller name */

	gboolean forward;					/**< TRUE if call is forwared (only if mobile terminated call) */

	TelCallActiveLine active_line;				/**< Active line */
}TelCallIncomingInfo;

/**
 * All Call Status idle notification.
 */
typedef struct {
	unsigned int call_id;	/**< Unique id for referring the call */
	TelCallEndCause cause;	/**< End cause for the call indicates whether the call is released normally or due to other cause*/
} TelCallStatusIdleNoti;

/**
 * @enum TelCallSoundRecording
 * Call Sound Recording status.
 */
typedef enum {
	TEL_SOUND_RECORDING_STOP,	/**< Stop call recording */
	TEL_SOUND_RECORDING_START,	/**< Start call recording */
	TEL_SOUND_RECORDING_PAUSE	/**< Pause call recording */
} TelCallSoundRecording;

/**
 * @enum TelCallSoundDevice
 * Call Sound Device type.
 */
typedef enum {
	TEL_SOUND_DEVICE_RECEIVER = 0x00,			/**< Receiving the sound on receiver */
	TEL_SOUND_DEVICE_SPEAKER_PHONE = 0x10,	/**< Receiving the sound on speaker phone */
	TEL_SOUND_DEVICE_HFK = 0x20,				/**< Receiving the sound on HFK */
	TEL_SOUND_DEVICE_HEADSET = 0x30,			/**< Receiving the sound on Headset */
	TEL_SOUND_DEVICE_BLUETOOTH = 0x40,		/**< Receiving the sound on Bluetooth */
	TEL_SOUND_DEVICE_EC = 0xA0,				/**< Receiving the sound on Echo Cancellation device */
} TelCallSoundDevice;

/**
 * Call Sound Volume Info.
 */
typedef struct {
	TelCallSoundDevice device;		/**< Sound device */
	unsigned int volume;		/**<Volume level(0 ~ 100)*/
} TelCallVolumeInfo;

/**
 * @enum TelCallSoundPath
 * Call Sound Path.
 */
typedef enum {
	TEL_SOUND_PATH_HANDSET	,		/**<Audio path is handset*/
	TEL_SOUND_PATH_HEADSET,			/**<Audio path is headset*/
	TEL_SOUND_PATH_HANDSFREE,		/**<Audio path is Handsfree*/
	TEL_SOUND_PATH_BLUETOOTH,		/**<Audio path is bluetooth*/
	TEL_SOUND_PATH_STEREO_BLUETOOTH,	/**<Audio path is stereo bluetooth*/
	TEL_SOUND_PATH_SPK_PHONE,		/**<Audio path is speaker phone*/
	TEL_SOUND_PATH_HEADSET_3_5PI,		/**<Audio path is headset_3_5PI*/
	TEL_SOUND_PATH_BT_NSEC_OFF,		/**<Audio path is bluetooth NSEC off*/
	TEL_SOUND_PATH_MIC2,				/**<Audio path is Mic 1*/
	TEL_SOUND_PATH_MIC1,				/**<Audio path is Mic 2*/
	TEL_SOUND_PATH_HEADSET_HAC,		/**<Audio path is Hearing aid*/
} TelCallSoundPath;

/**
 * Call Sound Path Info
 */
typedef struct {
	TelCallSoundPath path;	/**< Sound path */
	gboolean ex_volume;	/**< TEL_SOUND_EX_VOLUME_ OFF/ON*/
} TelCallSoundPathInfo;

/**
 * @enum TelCallSoundEqualizationMode
 * Call Sound Equalization Mode.
 */
typedef enum {
	TEL_SOUND_EQUALIZATION_MODE_OFF,		/**< Sound Equalization is off */
	TEL_SOUND_EQUALIZATION_MODE_ON,		/**< Sound Equalization is on */
	TEL_SOUND_EQUALIZATION_MODE_FLAG_OFF,	/**< Sound Equalization mode flag is off */
	TEL_SOUND_EQUALIZATION_MODE_FLAG_ON,	/**< Sound Equalization mode flag is on */
	TEL_SOUND_EQUALIZATION_MODE_SOFT1,		/**< Sound Equalization is in soft1 mode */
	TEL_SOUND_EQUALIZATION_MODE_SOFT2,		/**< Sound Equalization is in soft2 mode */
} TelCallSoundEqualizationMode;

/**
 * @enum TelCallSoundDirection
 * Call Sound Direction.
 */
typedef enum {
	TEL_SOUND_DIRECTION_LEFT,	/**< Sound direction is left */
	TEL_SOUND_DIRECTION_RIGHT,	/**< Sound direction is right */
} TelCallSoundDirection;

/**
 * Call Sound Equalization Info
 */
typedef struct {
	TelCallSoundEqualizationMode mode;	/**< Sound equalization mode */
	TelCallSoundDirection direction;		/**< Sound direction */
	unsigned short parameter[TEL_CALL_MAX_SOUND_EQ_PARAMETER_SIZE];	/**< Sound equalization parameter */
} TelCallSoundEqualization;

/**
 * Call Volume Info
 */
typedef struct {
	unsigned int record_num;	/**< Number of call records */
	TelCallVolumeInfo record[TEL_CALL_MAX_VOLUME_INFO_RECORD];	/**< Call volume info records */
} TelCallGetVolumeInfo;

/**
 * @enum TelCallSoundRingbackToneNoti
 * Call Sound ring back tone.
 */
typedef enum {
	TEL_CALL_SOUND_RINGBACK_TONE_START,	/**< Sound ringback tone start */
	TEL_CALL_SOUND_RINGBACK_TONE_END,		/**< Sound ringback tone end */
} TelCallSoundRingbackToneNoti;

/**
 * @enum TelCallSoundWbamrNoti
 * Call AMR-WB Status
 */
typedef enum {
	TEL_CALL_SOUND_WBAMR_STATUS_OFF,	/**< Call sound WBAMR is off */
	TEL_CALL_SOUND_WBAMR_STATUS_ON,	/**< Call sound WBAMR is on */
} TelCallSoundWbamrNoti;

/**
 * Call Sound Equilization Notification Info.
 */
typedef struct {
	TelCallSoundEqualizationMode mode;	/**< Call sound equalization mode */
	TelCallSoundDirection direction;		/**< Call sound direction */
} TelCallSoundEqualizationNoti;

/**
 *  \defgroup TAPI_EVENT TAPI Event IDs
 *  @{
 */

/**
 *  \defgroup TEL_EVENT_CALL Call Event IDs
 *  @{
 */

/**
 *  @hideinitializer
 *  @brief Voice Call status idle
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_VOICE_CALL_STATUS_IDLE
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_VOICE_CALL_STATUS_IDLE
 *  @retval data \ref TelCallStatusIdleNoti
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_VOICE_CALL_STATUS_IDLE		TELEPHONY_CALL_INTERFACE":VoiceCallStatusIdle"

 /**
 *  @hideinitializer
 *  @brief Voice Call status active
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_VOICE_CALL_STATUS_ACTIVE
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_VOICE_CALL_STATUS_ACTIVE
 *  @retval data \ref unsigned int call_id
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_VOICE_CALL_STATUS_ACTIVE		TELEPHONY_CALL_INTERFACE":VoiceCallStatusActive"

 /**
 *  @hideinitializer
 *  @brief Voice Call status held
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_VOICE_CALL_STATUS_HELD
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_VOICE_CALL_STATUS_HELD
 *  @retval data \ref unsigned int call_id
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_VOICE_CALL_STATUS_HELD		TELEPHONY_CALL_INTERFACE":VoiceCallStatusHeld"

 /**
 *  @hideinitializer
 *  @brief Voice Call status dialing
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_VOICE_CALL_STATUS_DIALING
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_VOICE_CALL_STATUS_DIALING
 *  @retval data \ref unsigned int call_id
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_VOICE_CALL_STATUS_DIALING		TELEPHONY_CALL_INTERFACE":VoiceCallStatusDialing"

/**
 *  @hideinitializer
 *  @brief Voice Call status alert
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_VOICE_CALL_STATUS_ALERT
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_VOICE_CALL_STATUS_ALERT
 *  @retval data \ref unsigned int call_id
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_VOICE_CALL_STATUS_ALERT		TELEPHONY_CALL_INTERFACE":VoiceCallStatusAlert"

/**
 *  @hideinitializer
 *  @brief Voice Call status incoming
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_VOICE_CALL_STATUS_INCOMING
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_VOICE_CALL_STATUS_INCOMING
 *  @retval data \ref unsigned int call_id
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_VOICE_CALL_STATUS_INCOMING	TELEPHONY_CALL_INTERFACE":VoiceCallStatusIncoming"

/**
 *  @hideinitializer
 *  @brief Video Call status idle
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_VIDEO_CALL_STATUS_IDLE
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_VIDEO_CALL_STATUS_IDLE
 *  @retval data reference to \ref TelCallStatusIdleNoti structure
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_VIDEO_CALL_STATUS_IDLE		TELEPHONY_CALL_INTERFACE":VideoCallStatusIdle"

/**
 *  @hideinitializer
 *  @brief Video Call status active
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_VIDEO_CALL_STATUS_ACTIVE
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_VIDEO_CALL_STATUS_ACTIVE
 *  @retval data \ref unsigned int call_id
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_VIDEO_CALL_STATUS_ACTIVE		TELEPHONY_CALL_INTERFACE":VideoCallStatusActive"

 /**
 *  @hideinitializer
 *  @brief Video Call status dialing
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_VIDEO_CALL_STATUS_DIALING
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_VIDEO_CALL_STATUS_DIALING
 *  @retval data \ref unsigned int call_id
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_VIDEO_CALL_STATUS_DIALING		TELEPHONY_CALL_INTERFACE":VideoCallStatusDialing"

 /**
 *  @hideinitializer
 *  @brief Video Call status alert
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_VIDEO_CALL_STATUS_ALERT
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_VIDEO_CALL_STATUS_ALERT
 *  @retval data \ref unsigned int call_id
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_VIDEO_CALL_STATUS_ALERT		TELEPHONY_CALL_INTERFACE":VideoCallStatusAlert"

 /**
 *  @hideinitializer
 *  @brief Video Call status incoming
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_VIDEO_CALL_STATUS_INCOMING
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_VIDEO_CALL_STATUS_INCOMING
 *  @retval data \ref unsigned int call_id
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_VIDEO_CALL_STATUS_INCOMING	TELEPHONY_CALL_INTERFACE":VideoCallStatusIncoming"

 /**
 *  @hideinitializer
 *  @brief MO Call wating
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_INFO_MO_WAITING
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_INFO_MO_WAITING
 *  @retval data none
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_INFO_MO_WAITING			TELEPHONY_CALL_INTERFACE":MoWaiting"

 /**
 *  @hideinitializer
 *  @brief MO Call forwarded
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_INFO_MO_FORWARDED
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_INFO_MO_FORWARDED
 *  @retval data none
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_INFO_MO_FORWARDED		TELEPHONY_CALL_INTERFACE":MoForwarded"

 /**
 *  @hideinitializer
 *  @brief MO Incoming call barred
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_INFO_MO_BARRED_INCOMING
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_INFO_MO_BARRED_INCOMING
 *  @retval data none
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_INFO_MO_BARRED_INCOMING		TELEPHONY_CALL_INTERFACE":MoBarredIncoming"

 /**
 *  @hideinitializer
 *  @brief MO Outgoing call barred
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_INFO_MO_BARRED_OUTGOING
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_INFO_MO_BARRED_OUTGOING
 *  @retval data none
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_INFO_MO_BARRED_OUTGOING		TELEPHONY_CALL_INTERFACE":MoBarredOutgoing"

 /**
 *  @hideinitializer
 *  @brief MO Call forward unconditional
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_INFO_MO_FORWARD_UNCONDITIONAL
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_INFO_MO_FORWARD_UNCONDITIONAL
 *  @retval data none
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_INFO_MO_FORWARD_UNCONDITIONAL	TELEPHONY_CALL_INTERFACE":MoForwardUnconditional"

 /**
 *  @hideinitializer
 *  @brief MO Call forward conditional
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_INFO_MO_FORWARD_CONDITIONAL
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_INFO_MO_FORWARD_CONDITIONAL
 *  @retval data none
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_INFO_MO_FORWARD_CONDITIONAL	TELEPHONY_CALL_INTERFACE":MoForwardConditional"

 /**
 *  @hideinitializer
 *  @brief MO Call deflected
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_INFO_MO_DEFLECTED
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_INFO_MO_DEFLECTED
 *  @retval data none
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_INFO_MO_DEFLECTED			TELEPHONY_CALL_INTERFACE":MoDeflected"

 /**
 *  @hideinitializer
 *  @brief MT Call forwarded
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_INFO_MT_FORWARDED
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_INFO_MT_FORWARDED
 *  @retval data none
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_INFO_MT_FORWARDED			TELEPHONY_CALL_INTERFACE":MtForwarded"

 /**
 *  @hideinitializer
 *  @brief MT Call deflected
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_INFO_MT_DEFLECTED
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_INFO_MT_DEFLECTED
 *  @retval data none
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_INFO_MT_DEFLECTED			TELEPHONY_CALL_INTERFACE":MtDeflected"

 /**
 *  @hideinitializer
 *  @brief Call Transfered
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_INFO_TRANSFERED
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_INFO_TRANSFERED
 *  @retval data none
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_INFO_TRANSFERED		TELEPHONY_CALL_INTERFACE":Transfered"

 /**
 *  @hideinitializer
 *  @brief Call held
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_INFO_HELD
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_INFO_HELD
 *  @retval data none
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_INFO_HELD			TELEPHONY_CALL_INTERFACE":CallHeld"

 /**
 *  @hideinitializer
 *  @brief Call active
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_INFO_ACTIVE
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_INFO_ACTIVE
 *  @retval data none
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_INFO_ACTIVE			TELEPHONY_CALL_INTERFACE":CallActive"

 /**
 *  @hideinitializer
 *  @brief Call joined
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_INFO_JOINED
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_INFO_JOINED
 *  @retval data none
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_INFO_JOINED			TELEPHONY_CALL_INTERFACE":CallJoined"

 /**
 *  @hideinitializer
 *  @brief Call Sound Ringback Tone
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_SOUND_RINGBACK_TONE
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_SOUND_RINGBACK_TONE
 *  @retval data \ref TelCallSoundRingbackToneNoti
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_SOUND_RINGBACK_TONE		TELEPHONY_CALL_INTERFACE":CallSoundRingbackToneControl"

 /**
 *  @hideinitializer
 *  @brief Call Sound WBAMR
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_SOUND_WBAMR
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_SOUND_WBAMR
 *  @retval data \ref TelCallSoundWbamrNoti
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_SOUND_WBAMR			TELEPHONY_CALL_INTERFACE":CallSoundWbamr"

 /**
 *  @hideinitializer
 *  @brief Call Sound Equalization
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_SOUND_EQUALIZATION
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_SOUND_EQUALIZATION
 *  @retval data \ref TelCallSoundEqualizationNoti
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_SOUND_EQUALIZATION		TELEPHONY_CALL_INTERFACE":CallSoundEqualiztion"

 /**
 *  @hideinitializer
 *  @brief Call Sound Clock Status
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_CALL_SOUND_CLOCK_STATUS
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_CALL_SOUND_CLOCK_STATUS
 *  @retval data <b>TRUE</b>: Sound clock is ON\n <b>FALSE</b>: Sound clock is OFF
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_CALL_SOUND_CLOCK_STATUS		TELEPHONY_CALL_INTERFACE":CallSoundClockStatus"

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
 #endif  /* __TEL_CALL_H__ */
