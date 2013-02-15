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

#ifndef __TYPE_CALL_H__
#define __TYPE_CALL_H__

__BEGIN_DECLS

#include <glib.h>

enum telephony_call_error {
	CALL_ERROR_NONE,					/**< Success  */
	CALL_ERROR_ACCESSDISCARD,			/**< Access discarded  */
	CALL_ERROR_BARR_BYOPERATOR,			/**< Barred by operator */
	CALL_ERROR_BEARERMODE_NOTIMPL,		/**< Bearer mode not implemented */
	CALL_ERROR_BEARERMODE_RESTRICTED,	/**< Bearer mode restricted */
	CALL_ERROR_BEARERMODE_UNAUTH,		/**< Bearer mode un authorized */
	CALL_ERROR_BEARERMODE_UNAVAIL,		/**< Bearer mode not available */
	CALL_ERROR_BUSY,					/**< (Network/Server) busy */
	CALL_ERROR_CALLMETER_EXPIRED,		/**< Call meter expired */
	CALL_ERROR_CALLNO_ERROR,			/**< Call number error */
	CALL_ERROR_CKTUNAVAIL,				/**< Circuit channel unavailable */			//10
	CALL_ERROR_CONGESTION,				/**< Congestion happened */
	CALL_ERROR_NO_CIRCUIT_AVAIL,		/**< Circuit  not available */
	CALL_ERROR_DESTIN_INCOMPAT,			/**< Destination incompatibility */
	CALL_ERROR_DTMF_NOSPEECH,			/**< No speech in DTMF */
	CALL_ERROR_DTMF_REJECTED,			/**< DTMF rejected */
	CALL_ERROR_FACILITY_NOTIMPL,		/**< Facility not implemented */
	CALL_ERROR_FACILITY_NOTSUBSCRIBED,	/**< Facility not subscribed */
	CALL_ERROR_INCOMINGCUGCALLS_BARRED, /**< Incoming CUG Calls barred */
	CALL_ERROR_INVALNUM,				/**< Invalid number */
	CALL_ERROR_MPTY_ERROR,				/**< Multiparty error */						//20
	CALL_ERROR_NOANSWER,				/**< No answer  */
	CALL_ERROR_NONCUGMEMBER,			/**< Non CUG member */
	CALL_ERROR_NUMBERCHANGED,			/**< Number changed */
	CALL_ERROR_NUMBER_ERROR,			/**< Number error */
	CALL_ERROR_NWFAILURE,				/**< Network failure */
	CALL_ERROR_OPERATIONUNAVAIL,		/**< Operation not available */
	CALL_ERROR_OUTOFORDER,				/**< Out of order */
	CALL_ERROR_PROTOCOL_ERROR,			/**< Protocol error */
	CALL_ERROR_QOSUNAVAIL,				/**< QOS unavailable */
	CALL_ERROR_REJECT,					/**< Rejected */								//30
	CALL_ERROR_REJ_FAIL,				/**< Rejection failed */
	CALL_ERROR_REJ_SRVC_NOT_AVL,		/**< Rejection service not available  */
	CALL_ERROR_REMOTE_CKTUNAVAIL,		/**< Remote Circuit channel unavailable */
	CALL_ERROR_RESOURCEUNAVAIL,			/**< Resource not available */
	CALL_ERROR_SERVICEID_ERROR,			/**< Service id error */
	CALL_ERROR_SERVICE_NOTIMPL,			/**< Service not implemented */
	CALL_ERROR_SERVICE_UNAVAIL,			/**< Service not available */
	CALL_ERROR_MODIFY_SAME_BEARER,		/**< Modify same bearer */
	CALL_ERROR_MODIFY_NOT_PERMITTED,	/**< Modification not permitted */
	CALL_ERROR_HOLD_FAIL,				/**< Call hold fail */							//40
	CALL_ERROR_TEMPFAILURE,				/**< Temporary failure */
	CALL_ERROR_WRONGNUM,				/**< Wrong number */
	CALL_ERROR_NORMAL_UNSPECIFIED,		/**< Normal but unspecified */
	CALL_ERROR_SSERRINVALIDTIVALUE,		/**< SS invalid Transaction ID Value */
	CALL_ERROR_SSERRINVALIDTI,			/**< SS Invalid transaction ID */
	CALL_ERROR_SSINCOMPATIBLESTATE,		/**< SS incompatible state */
	CALL_ERROR_SSERRSYNTAXERROR,		/**< SS syntax error */
	CALL_ERROR_SSPROTOCOLERROR,			/**< Protocol error */
	CALL_ERROR_SSNEGATIVEPASSWORDCHECK, /**< Negative password check */
	CALL_ERROR_SSSENTASPHASE1,			/**< SS sent as phase1 message */				//50
	CALL_ERROR_SSERROR,					/**< Supplementary service error */
	CALL_ERROR_SS_USSD_BUSY,			/**< Second USSD request will be rejected when already USSD transaction is ongoing. */
	CALL_ERROR_IDLE,					/**< Idle */
	CALL_ERROR_NETWORK_SPECIFIC,		/**< Network specific error */
	CALL_ERROR_FADE,					/**< voice call was dropped e.g. because of a loss of signal */
	CALL_ERROR_UNKNOWN,					/**< unknown no details available */
	CALL_ERROR_INCOM_CALL,				/**< terminated by incoming call */
	CALL_ERROR_ALERT_STOP,				/**< terminated by alert stop */
	CALL_ERROR_INTERCEPT,				/**< voice call was dropped by interception */
	CALL_ERROR_REORDER,					/**< voice call was dropped by reordering */	//60
	CALL_ERROR_CLIENT_END,				/**< client ended the call */
	CALL_ERROR_ACCESS_CLASS_BLOCKED,	/**< access class blocked */
	CALL_ERROR_MEMCAPEXCEEDED,			/**< Memory capacity exceeded */
	CALL_ERROR_TYPENOTSUPPORTED,		/**< Type not supported */
	CALL_ERROR_REPLACENOTSUPPORTED,		/**< Replace not supported */
	CALL_ERROR_PROTOCOLID_ERROR,		/**< Protocol id error */
	CALL_ERROR_CLASSNOTSUPPORTED,		/**< Class not supported */
	CALL_ERROR_DATACODING_ERROR,		/**< Data coding error */
	CALL_ERROR_INVAL_MSG,				/**< Invalid message */
	CALL_ERROR_CALL_BARRED,			/**< Call barred */							//70
	CALL_ERROR_SIM_CALL_CONTROL_CHANGED_TO_SS,		/**< Sim call control changed to SS */
	CALL_ERROR_SIM_CALL_CONTROL_CHANGED_TO_CALL,	/**< SIM Call control changed to call */
	CALL_ERROR_SIM_ERROR,				/**< SIM error */
	CALL_ERROR_SIM_CALL_CONTROL_MODIFIED,			/**< SIM call control modified */
	CALL_ERROR_RANDOM_ACCESS_FAILURE,	/**< Random access error */
	CALL_ERROR_LOWER_LAYER_FAILURE,		/**< Lower layer error */
	CALL_ERROR_ACCESS_REJECTED,			/**< Access rejected */
	CALL_ERROR_RESET_NOT_ALLOWED,		/**< Reset not allowed */
	CALL_ERROR_EMERGENCY_CALL_NOT_ALLOWED,			/**< Emergency call not allowed */
	CALL_ERROR_ACCESS_NOT_ALLOWED,		/**< Access not allowed */						//80
	CALL_ERROR_RADIO_LINK_FAILURE,		/**< Radio link failure */
	CALL_ERROR_INVALID_IDENTITY,		/**< Invalid identity */
	CALL_ERROR_UNKNOWN_IDENTITY,		/**< Unknown identity */
	CALL_ERROR_UNACCEPTABLE_IDENTITY,	/**< Un acceptable identity */
	CALL_ERROR_NO_SERVICE_HERE,			/**< No service here */
	CALL_ERROR_SERVICE_NOT_ALLOWED,		/**< Service not allowed */
	CALL_ERROR_SERVICE_NOT_AVAIL,		/**< Service not available */
	CALL_ERROR_SERVICE_NOT_WORKING,		/**< Service not working */
	CALL_ERROR_CANNOT_IDENTIFY_CALL,	/**< Cannot identify the call */
	CALL_ERROR_DEACTIVATION,			/**< Deactivation */							//90
	CALL_ERROR_FATAL_ERROR,				/**< Fatal error */
	CALL_ERROR_SEND_DTMF_SUCCESS,		/**< Sending DTMF Success */
	CALL_ERROR_SEND_DTMF_FAIL,			/**< Sending DTMF Failed */
	CALL_ERROR_FIXED_DIALING_NUMBER_ONLY,/**< Fixed Dialing Number Only */
	CALL_ERROR_TIMER_EXPIRED,			/**< Call Timer Expired */
};

enum telephony_call_answer_type {
	CALL_ANSWER_TYPE_ACCEPT,
	CALL_ANSWER_TYPE_REJECT,
	CALL_ANSWER_TYPE_REPLACE,
	CALL_ANSWER_TYPE_HOLD_ACCEPT,
};

enum telephony_call_type {
	CALL_TYPE_VOICE,
	CALL_TYPE_VIDEO,
	CALL_TYPE_E911
};

enum telephony_call_status {
	CALL_STATUS_IDLE,
	CALL_STATUS_ACTIVE,
	CALL_STATUS_HELD,
	CALL_STATUS_DIALING,
	CALL_STATUS_ALERT,
	CALL_STATUS_INCOMING,
	CALL_STATUS_WAITING,
};

enum telephony_call_end_type {
	CALL_END_TYPE_DEFAULT,
	CALL_END_TYPE_ALL,
	CALL_END_TYPE_ACTIVE_ALL,
	CALL_END_TYPE_HOLD_ALL,
};

enum telephony_call_end_cause {
	CALL_END_CAUSE_NONE = 0x00,				/**< No Cause */

	CALL_END_CAUSE_UNASSIGNED_NUMBER,			/**< Unassigned Number */
	CALL_END_CAUSE_NO_ROUTE_TO_DEST,			/**< No Route to Destination */
	CALL_END_CAUSE_CHANNEL_UNACCEPTABLE,		/**< Channel Unacceptable */
	CALL_END_CAUSE_OPERATOR_DETERMINED_BARRING,	/**< Operator Determined Barring */
	CALL_END_CAUSE_NORMAL_CALL_CLEARING,	/**< Normal Call Clearing */

	CALL_END_CAUSE_USER_BUSY,					/**< User Busy */
	CALL_END_CAUSE_NO_USER_RESPONDING,			/**< No user Responding */
	CALL_END_CAUSE_USER_ALERTING_NO_ANSWER,		/**< User Alerting no Answer */
	CALL_END_CAUSE_CALL_REJECTED,			/**< Call Rejected */
	CALL_END_CAUSE_NUMBER_CHANGED,				/**< Number Changed */

	CALL_END_CAUSE_NON_SELECTED_USER_CLEARING,	/**< Non Selected User Clearing */
	CALL_END_CAUSE_DESTINATION_OUT_OF_ORDER,	/**< Destination out of Order */
	CALL_END_CAUSE_INVALID_NUMBER_FORMAT,		/**< Invalid Number Format */
	CALL_END_CAUSE_FACILITY_REJECTED,			/**< Facility Rejected */
	CALL_END_CAUSE_RESPONSE_TO_STATUS_ENQUIRY,	/**< Response to Status Enquiry */

	CALL_END_CAUSE_NORMAL_UNSPECIFIED,				/**< Normal Unspecified */
	CALL_END_CAUSE_NO_CIRCUIT_CHANNEL_AVAILABLE,	/**< No Circuit Channel Available */
	CALL_END_CAUSE_NETWORK_OUT_OF_ORDER,			/**< Network out of Order */
	CALL_END_CAUSE_TEMPORARY_FAILURE,				/**< Temporary Failure */
	CALL_END_CAUSE_SWITCHING_EQUIPMENT_CONGESTION,	/**< Switching Equipment Congestion */

	CALL_END_CAUSE_ACCESS_INFORMATION_DISCARDED,			/**< Access Information Discarded */
	CALL_END_CAUSE_REQUESTED_CIRCUIT_CHANNEL_NOT_AVAILABLE,	/**< Requested Circuit channel not available */
	CALL_END_CAUSE_RESOURCES_UNAVAILABLE_UNSPECIFIED,		/**< Resources unavailable unspecified */
	CALL_END_CAUSE_QUALITY_OF_SERVICE_UNAVAILABLE,			/**< Quality of service unavailable */
	CALL_END_CAUSE_REQUESTED_FACILITY_NOT_SUBSCRIBED,		/**< Requested facility not subscribed */

	CALL_END_CAUSE_INCOMING_CALL_BARRED_WITHIN_CUG,		/**< incomming call barred within CUG */
	CALL_END_CAUSE_BEARER_CAPABILITY_NOT_AUTHORISED,		/**< Bearer Capability not Authorised */
	CALL_END_CAUSE_BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE,/**< Bearer capability not presently Available */
	CALL_END_CAUSE_SERVICE_OR_OPTION_NOT_AVAILABLE,			/**< Service or Option not available */
	CALL_END_CAUSE_BEARER_SERVICE_NOT_IMPLEMENTED,			/**< Service not implemented */

	CALL_END_CAUSE_ACM_GEQ_ACMMAX,							/**< ACM GEQ ACMAX */
	CALL_END_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED,		/**< Requested facility not implemented */
	CALL_END_CAUSE_ONLY_RESTRICTED_DIGITAL_INFO_BC_AVAILABLE,/**< Resticted digital info BC not available */
	CALL_END_CAUSE_SERVICE_OR_OPTION_NOT_IMPLEMENTED,		/**< Service or Option not Implemented  */
	CALL_END_CAUSE_INVALID_TRANSACTION_ID_VALUE,			/**< Transaction ID value */

	CALL_END_CAUSE_USER_NOT_MEMBER_OF_CUG,				/**< Not member of CUG */
	CALL_END_CAUSE_INCOMPATIBLE_DESTINATION,			/**< Incompatible Destination */
	CALL_END_CAUSE_INVALID_TRANSIT_NETWORK_SELECTION,	/**< Transit Network selection */
	CALL_END_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE,		/**< Semantically Incorrect message */
	CALL_END_CAUSE_INVALID_MANDATORY_INFORMATION,		/**< Invalid Mandatory Message */

	CALL_END_CAUSE_MESSAGE_TYPE_NON_EXISTENT,					/**< Message Type Non Existent */
	CALL_END_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE,	/**< Message type not compatible with Prot state */
	CALL_END_CAUSE_IE_NON_EXISTENT_OR_NOT_IMPLEMENTED,			/**< IE non exitent or not implemented */
	CALL_END_CAUSE_CONDITIONAL_IE_ERROR,						/**< Conditional IE error */
	CALL_END_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE,	/**< Not Compatible with protocol state */

	CALL_END_CAUSE_RECOVERY_ON_TIMER_EXPIRY,	/**< Recovery on timer expiry */
	CALL_END_CAUSE_PROTOCOL_ERROR_UNSPECIFIED,	/**< Protocol error unspecified */
	CALL_END_CAUSE_INTERWORKING_UNSPECIFIED,	/**< Interworking unspecified */

	CALL_END_CAUSE_END = 128,

	/* Reject causes*/
	CALL_END_CAUSE_IMSI_UNKNOWN_IN_HLR,	/**< IMSI unknown in HLR */
	CALL_END_CAUSE_ILLEGAL_MS,			/**< Illegal MS */
	CALL_END_CAUSE_IMSI_UNKNOWN_IN_VLR,	/**< IMSI unknown in VLR */
	CALL_END_CAUSE_IMEI_NOT_ACCEPTED,	/**< IMEI not accepted */
	CALL_END_CAUSE_ILLEGAL_ME,			/**< Illegal ME */

	CALL_END_CAUSE_GPRS_SERVICES_NOT_ALLOWED,						/**< GPRS service not allowed */
	CALL_END_CAUSE_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED,	/**< GPRS services and Non-GPRS services not allowed */
	CALL_END_CAUSE_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK,	/**< MS device cannot be derived by the network */
	CALL_END_CAUSE_IMPLICITLY_DETACHED,								/**< Implicitly detached */
	CALL_END_CAUSE_PLMN_NOT_ALLOWED,								/**< PLMN not allowed */

	CALL_END_CAUSE_LA_NOT_ALLOWED,							/**< LA not allowed */
	CALL_END_CAUSE_NATIONAL_ROAMING_NOT_ALLOWED,			/**< National roaming not allowed */
	CALL_END_CAUSE_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN,	/**< GPRS services not allowed in this PLMN */
	CALL_END_CAUSE_NO_SUITABLE_CELLS_IN_LA,					/**< No suitable cells in the LA */
	CALL_END_CAUSE_MSC_TEMPORARILY_NOT_REACHABLE,			/**< MSC temporarily not reachable */

	CALL_END_CAUSE_NETWORK_FAILURE,		/**< Network unavailable */
	CALL_END_CAUSE_MAC_FAILURE,			/**< MAC failure */
	CALL_END_CAUSE_SYNCH_FAILURE,		/**< SYNCh failure */
	CALL_END_CAUSE_CONGESTTION,			/**< Congestion */
	CALL_END_CAUSE_GSM_AUTH_UNACCEPTED,	/**< GSM Auth unaccepted */

	CALL_END_CAUSE_SERVICE_OPTION_NOT_SUPPORTED, /**< Service option not supported */
	CALL_END_CAUSE_REQ_SERV_OPT_NOT_SUBSCRIBED,	/**< REQ_SERV option not suscribed */
	CALL_END_CAUSE_SERVICE_OPT__OUT_OF_ORDER,	/**< Service OPT out of order */
	CALL_END_CAUSE_CALL_CANNOT_BE_IDENTIFIED,	/**< Call cannot be identified */
	CALL_END_CAUSE_NO_PDP_CONTEXT_ACTIVATED,	/**< No PDP context Activated */

	CALL_END_CAUSE_RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MIN_VALUE,	/**< Retry upon entry into a new call min value */
	CALL_END_CAUSE_RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MAX_VALUE,	/**< Retry upon entry into a new call max value */
	CALL_END_CAUSE_SEMANTICALLY_INCORRECT_MSG,					/**< Semantically incorret message */
	CALL_END_CAUSE_INVALID_MANDATORY_INFO,						/**< Invalid mandatory information */
	CALL_END_CAUSE_MESSAGE_TYPE_NON_EXISTANT,					/**< Message type non-existant */

	CALL_END_CAUSE_MESSAGE_TYPE_NOT_COMP_PRT_ST,		/**< Message type not COMP PRT ST */
	CALL_END_CAUSE_IE_NON_EXISTANT,						/**< IE not existant */
	CALL_END_CAUSE_MSG_NOT_COMPATIBLE_PROTOCOL_STATE,	/**< MSG not compatible protocol state */

	/* Connection Management establishment rejection cause */
	CALL_END_CAUSE_REJ_UNSPECIFIED,	/**< rej unspecified */

	/* AS reject causes */
	CALL_END_CAUSE_AS_REJ_RR_REL_IND,						/**< RR release indication */
	CALL_END_CAUSE_AS_REJ_RR_RANDOM_ACCESS_FAILURE,			/**< Random Access Failure */
	CALL_END_CAUSE_AS_REJ_RRC_REL_IND,						/**< RRC release indication */
	CALL_END_CAUSE_AS_REJ_RRC_CLOSE_SESSION_IND,			/**< RRC close session indication */
	CALL_END_CAUSE_AS_REJ_RRC_OPEN_SESSION_FAILURE,			/**< RRC open session failure */
	CALL_END_CAUSE_AS_REJ_LOW_LEVEL_FAIL,					/**< Low level failure */
	CALL_END_CAUSE_AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED, /**< Low level failure redial not alowed */
	CALL_END_CAUSE_AS_REJ_LOW_LEVEL_IMMED_RETRY,			/**< low level immediate retry */

	/* MM reject causes */
	CALL_END_CAUSE_MM_REJ_INVALID_SIM,			/**< invalid Sim */
	CALL_END_CAUSE_MM_REJ_NO_SERVICE,			/**< No service */
	CALL_END_CAUSE_MM_REJ_TIMER_T3230_EXP,		/**< Timer T3230 expire */
	CALL_END_CAUSE_MM_REJ_NO_CELL_AVAILABLE,	/**< No call available  */
	CALL_END_CAUSE_MM_REJ_WRONG_STATE,			/**< wrong state */
	CALL_END_CAUSE_MM_REJ_ACCESS_CLASS_BLOCKED,	/**< Access class blocked */

	/* Definitions for release ind causes between MM  and CNM*/
	CALL_END_CAUSE_ABORT_MSG_RECEIVED,	/**< Abort Message received */
	CALL_END_CAUSE_OTHER_CAUSE,			/**< Other cause  */

	/* CNM reject causes */
	CALL_END_CAUSE_CNM_REJ_TIMER_T303_EXP,	/**< Timer T303 expiry */
	CALL_END_CAUSE_CNM_REJ_NO_RESOURCES,	/**< Rejected due to unavailibilty of resources */
	CALL_END_CAUSE_CNM_MM_REL_PENDING,		/**< MM release pending */
	CALL_END_CAUSE_CNM_INVALID_USER_DATA,	/**< Invalid user data */
};

enum telephony_call_cli_mode {
	CALL_CLI_MODE_DEFAULT,
	CALL_CLI_MODE_PRESENT,
	CALL_CLI_MODE_RESTRICT,
};

enum telephony_call_cna_mode {
	CALL_CNA_MODE_DEFAULT,
	CALL_CNA_MODE_PRESENT,
	CALL_CNA_MODE_RESTRICT,
};

enum telephony_call_sound_path {
	CALL_SOUND_PATH_HANDSET		= 0x01,
	CALL_SOUND_PATH_HEADSET,
	CALL_SOUND_PATH_HANDFREE,
	CALL_SOUND_PATH_BLUETOOTH,
	CALL_SOUND_PATH_STEREO_BLUETOOTH,
	CALL_SOUND_PATH_SPEAKER,
	CALL_SOUND_PATH_HEADSET_3_5PI,
	CALL_SOUND_PATH_BT_NSEC_OFF,
	CALL_SOUND_PATH_MIC1,
	CALL_SOUND_PATH_MIC2,
	CALL_SOUND_PATH_HEADSET_HAC,
};

enum telephony_call_sound_device {
	CALL_SOUND_DEVICE_RECEIVER	= 0x10,
	CALL_SOUND_DEVICE_SPEAKER	= 0x20,
	CALL_SOUND_DEVICE_HFK		= 0x30,
	CALL_SOUND_DEVICE_BLUETOOTH	= 0x40,
	CALL_SOUND_DEVICE_ECHO_CANCELLER	= 0xA0,
};

enum telephony_call_sound_type {
	CALL_SOUND_TYPE_VOICE	= 0x1,
	CALL_SOUND_TYPE_KEYTONE,
	CALL_SOUND_TYPE_BELL,
	CALL_SOUND_TYPE_MESSAGE,
	CALL_SOUND_TYPE_ALARM,
	CALL_SOUND_TYPE_MISCELLANEOUS,
};

enum telephony_call_sound_volume_level {
	CALL_SOUND_MUTE,
	CALL_SOUND_VOLUME_LEVEL_1,
	CALL_SOUND_VOLUME_LEVEL_2,
	CALL_SOUND_VOLUME_LEVEL_3,
	CALL_SOUND_VOLUME_LEVEL_4,
	CALL_SOUND_VOLUME_LEVEL_5,
	CALL_SOUND_VOLUME_LEVEL_6,
	CALL_SOUND_VOLUME_LEVEL_7,
	CALL_SOUND_VOLUME_LEVEL_8,
	CALL_SOUND_VOLUME_LEVEL_9,
};

enum telephony_call_sound_ringback_tone_status {
	CALL_SOUND_RINGBACK_TONE_START,
	CALL_SOUND_RINGBACK_TONE_END,
};

enum telephony_call_sound_direction {
	CALL_SOUND_DIRECTION_LEFT,
	CALL_SOUND_DIRECTION_RIGHT,
};

/**********
  Struct
  **********/

// Request

#define MAX_CALL_NUMBER_LEN 83
#define MAX_CALL_DIAL_NUM_LEN MAX_CALL_NUMBER_LEN
#define MAX_CALL_DTMF_DIGITS_LEN 32


#define MAX_CALL_CLI_NUM_LEN MAX_CALL_NUMBER_LEN
#define MAX_CALL_CNA_NAME_LEN 83

struct telephony_call_cli_info {
	enum telephony_call_cli_mode mode;
	char number[ MAX_CALL_CLI_NUM_LEN ];
};

struct telephony_call_cna_info {
	enum telephony_call_cna_mode mode;
	char name[ MAX_CALL_CNA_NAME_LEN ];
};



struct treq_call_dial {
	enum telephony_call_type type;
	char number[ MAX_CALL_DIAL_NUM_LEN ];
};

struct treq_call_answer {
	unsigned int id;
	enum telephony_call_answer_type type;
};

struct treq_call_end {
	unsigned int id;
	enum telephony_call_end_type type;
};

struct treq_call_dtmf {
	char digits[ MAX_CALL_DTMF_DIGITS_LEN ];
};

struct treq_call_active {
	unsigned int id;
};

struct treq_call_hold {
	unsigned int id;
};

struct treq_call_swap {
	unsigned int id;
};

struct treq_call_join {
	unsigned int id;
};

struct treq_call_split {
	unsigned int id;
};

struct treq_call_transfer {
	unsigned int id;
};

struct treq_call_deflect {
	unsigned int id;
	char number[MAX_CALL_NUMBER_LEN];
};

struct treq_call_sound_set_path {
	enum telephony_call_sound_path path;
	gboolean extra_volume_on;
};

struct treq_call_sound_set_volume_level {
	enum telephony_call_sound_type sound;
	enum telephony_call_sound_device device;
	enum telephony_call_sound_volume_level volume;
};

struct treq_call_sound_get_volume_level {
	enum telephony_call_sound_type sound;
	enum telephony_call_sound_device device;
};

struct treq_call_sound_set_recording {
	gboolean state;
};

#define MAX_CALL_EQ_PARAMETER_SIZE 6
struct treq_call_sound_set_equalization {
	gboolean mode;
	enum telephony_call_sound_direction direction;
	unsigned short parameter[ MAX_CALL_EQ_PARAMETER_SIZE ];
};

struct treq_call_sound_set_noise_reduction {
	gboolean status;
};

struct treq_call_sound_set_clock_status {
	gboolean status;
};


// Response

struct tresp_call_dial {
	enum telephony_call_error err;
};

struct tresp_call_answer {
	unsigned int id;
	enum telephony_call_error err;
};

struct tresp_call_end {
	enum telephony_call_end_type type;
	unsigned int id;
	enum telephony_call_error err;
};

struct tresp_call_hold {
	unsigned int id;
	enum telephony_call_error err;
};

struct tresp_call_active {
	unsigned int id;
	enum telephony_call_error err;
};

struct tresp_call_swap {
	unsigned int id;
	enum telephony_call_error err;
};

struct tresp_call_join {
	unsigned int id;
	enum telephony_call_error err;
};

struct tresp_call_split {
	unsigned int id;
	enum telephony_call_error err;
};

struct tresp_call_deflect {
	unsigned int id;
	enum telephony_call_error err;
};

struct tresp_call_transfer {
	unsigned int id;
	enum telephony_call_error err;
};

struct tresp_call_dtmf {
	enum telephony_call_error err;
};

struct tresp_call_sound_set_path {
	gboolean err;
};

struct tresp_call_sound_set_volume_level {
	gboolean err;
};

struct tresp_call_sound_get_volume_level {
	int record_num;
	struct volume_info {
		enum telephony_call_sound_type sound;
		enum telephony_call_sound_volume_level volume;
	} *record;
	gboolean err;
};

struct tresp_call_mute {
	gboolean err;
};

struct tresp_call_unmute {
	gboolean err;
};

struct tresp_call_get_mute_status {
	int status;
	gboolean err;
};

struct tresp_call_sound_set_recording {
	gboolean err;
};

struct tresp_call_sound_set_equalization {
	gboolean err;
};

struct tresp_call_sound_set_noise_reduction {
	gboolean err;
};

struct tresp_call_sound_set_clock_status {
	gboolean err;
};


// Notification

struct tnoti_call_status_idle {
	enum telephony_call_type type;
	unsigned int	id;
	enum telephony_call_end_cause cause;
};

struct tnoti_call_status_active {
	enum telephony_call_type type;
	unsigned int	id;
};

struct tnoti_call_status_held {
	enum telephony_call_type type;
	unsigned int	id;
};

struct tnoti_call_status_dialing {
	enum telephony_call_type type;
	unsigned int	id;
};

struct tnoti_call_status_alert {
	enum telephony_call_type type;
	unsigned int	id;
};

struct tnoti_call_status_incoming {
	enum telephony_call_type type;
	unsigned int	id;
	struct telephony_call_cli_info cli;
	struct telephony_call_cna_info cna;
	gboolean		forward;
	unsigned int	active_line;
};

struct tnoti_call_status_waiting {
	enum telephony_call_type type;
	unsigned int	id;
};

struct tnoti_call_sound_path {
	enum telephony_call_sound_path path;
};

struct tnoti_call_sound_ringback_tone {
	enum telephony_call_sound_ringback_tone_status status;
};

struct tnoti_call_sound_wbamr {
	gboolean status;
};

struct tnoti_call_sound_equalization {
	gboolean mode;
	enum telephony_call_sound_direction direction;
};

struct tnoti_call_sound_noise_reduction {
	gboolean status;
};

struct tnoti_call_sound_clock_status {
	gboolean status;
};


__END_DECLS

#endif
