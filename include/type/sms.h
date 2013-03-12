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

#ifndef __TYPE_SMS_H__
#define __TYPE_SMS_H__

__BEGIN_DECLS

/*
	TREQ_SMS_SEND_UMTS_MSG,
	TREQ_SMS_READ_MSG,
	TREQ_SMS_SAVE_MSG,
	TREQ_SMS_DELETE_MSG,
	TREQ_SMS_GET_COUNT,
	TREQ_SMS_GET_SCA,
	TREQ_SMS_SET_SCA,
	TREQ_SMS_GET_CB_CONFIG,
	TREQ_SMS_SET_CB_CONFIG,
	TREQ_SMS_SET_MEM_STATUS,
	TREQ_SMS_GET_PREF_BEARER,
	TREQ_SMS_SET_PREF_BEARER,
	TREQ_SMS_SET_DELIVERY_REPORT,
	TREQ_SMS_SET_MSG_STATUS,
	TREQ_SMS_GET_PARAMS,
	TREQ_SMS_SET_PARAMS,
	TREQ_SMS_GET_PARAMCNT,
	TREQ_SMS_SEND_CDMA_MSG,

	TREQ_PS_ACTIVATE_CONTEXT,
	TREQ_PS_DEACTIVATE_CONTEXT,
	TREQ_PS_PDP_ACTIVATE,
	TREQ_PS_PDP_DEACTIVATE,
	TREQ_PS_SET_DORMANT,
	TREQ_PS_SET_PORT_LIST,
	TREQ_PS_GET_PORT_LIST,
	TREQ_PS_DUN_PIN_CONTROL,
*/

/*==================================================================================================
                                            MACROS
==================================================================================================*/
/* NetText */

#define SMS_SMSP_ADDRESS_LEN			20	/* EF-SMSP digit length */
#define SMS_SMSP_ALPHA_ID_LEN_MAX		128	/* EF-SMSP alpha id length */
#define SMS_MAX_EFSMSP_RECORD_LENGTH		156	/* Maximum number of bytes SMSP Record size (Y + 28), y : 0 ~ 128 */

#define SMS_MSG_SIZE_MAX			918	/**< Maximum Message Size */
#define SMS_CB_SIZE_MAX				93						/** Maximum CB Message Size */
#define SMS_ETWS_SIZE_MAX			56	/** Maximum ETWS Message Size */

#define SMS_ENCODED_SCA_LEN_MAX		12	/* Encoded SCA is 12 bytes long maximum */

#define SMS_CB_PAGE_SIZE_MAX			9	/**< CB maximum page size*/
#define SMS_GSM_SMS_MSG_NUM_MAX		255	/**< Maximum GSM SMS message number*/
#define SMS_GSM_SMS_CBMI_LIST_SIZE_MAX		50	/**< Maximum GSM SMS CBMI list size*/
#define SMS_SMDATA_SIZE_MAX			164	/**< Maximum SMS data size that can be stored*/
#define SMS_MAX_INDEX				255	/**< Maximum index value for SMS */

#define SMS_SMSP_PARAMS_MAX_LEN		28

// ************************  CDMA Features  **************************//
#define SMS_PARAM_TELESERVICE_MASK					1 << 0	/**< Teleservice parameter bit position */
#define SMS_PARAM_SERVICE_CATEGORY_MASK				1 << 1	/**< Service Category parameter bit position */
#define SMS_PARAM_ADDRESS_MASK						1 << 2	/**< Address parameter bit position */
#define SMS_PARAM_SUBADDRESS_MASK					1 << 3	/**< Subaddress parameter bit position */
#define SMS_PARAM_BEARER_REPLY_MASK					1 << 4	/**< Bearer reply parameter bit position */
#define SMS_PARAM_CAUSE_CODES_MASK					1 << 5	/**< Cause Code parameter bit position */
#define SMS_PARAM_MESSAGE_ID_MASK					1 << 6	/**< Message ID parameter bit position */
#define SMS_PARAM_USER_DATA_MASK					1 << 7	/**< User Data parameter bit position */
#define SMS_PARAM_USER_RESPONSE_CODE_MASK			1 << 8	/**< User Response Code parameter bit position */
#define SMS_PARAM_MC_TIME_STAMP_MASK				1 << 9	/**< Message Center Timestamp parameter bit position */
#define SMS_PARAM_VALIDITY_PERIOD_ABS_MASK			1 << 10	/**< Valid period Absolute parameter bit position */
#define SMS_PARAM_VALIDITY_PERIOD_REL_MASK			1 << 11	/**< Valid period Relative parameter bit position */
#define SMS_PARAM_DEFERRED_DELIVERY_ABS_MASK		1 << 12	/**< Deferred delivery absolute parameter bit position */
#define SMS_PARAM_DEFERRED_DELIVERY_REL_MASK		1 << 13	/**< Deferred delivery relative parameter bit position */
#define SMS_PARAM_PRIORITY_MASK						1 << 14	/**< Priority parameter bit position */
#define SMS_PARAM_PRIVACY_MASK						1 << 15	/**< Privacy parameter bit position */
#define SMS_PARAM_REPLY_OPTION_MASK					1 << 16	/**< Reply Option parameter bit position */
#define SMS_PARAM_NUMBER_OF_MESSAGE_MASK			1 << 17	/**< Number of message parameter bit position */
#define SMS_PARAM_ALERT_ON_DELIVERY_MASK			1 << 18	/**< Alert on delivery parameter bit position */
#define SMS_PARAM_LANGUAGE_MASK						1 << 19	/**< Language parameter bit position */
#define SMS_PARAM_CALLBACK_MASK						1 << 20	/**< Callback Number parameter bit position */
#define SMS_PARAM_DISPLAY_MODE_MASK					1 << 21	/**< Display mode parameter bit position */
#define SMS_PARAM_MULTI_ENCODING_USER_DATA_MASK	1 << 22	/**< Multi Encoding user data parameter bit position */

#define SMS_MAXLENGTH_SMS_MT_USER_DATA				160		/**< Maximum MT user data  size*/
#define SMS_MAXLENGTH_SMS_MO_USER_DATA				160		/**< Maximum MO user data  size*/

#define SMS_MAXLENGTH_SMS_ADDRESS					32		/**< MAX sms destination(or origination ) address /call back number */

#define SMS_ERR_CLASS0_STATUS_SEND_OK								0	/**< send success */
#define SMS_ERR_CLASS23_ADDRESS_VACANT								0	/**< address vacant */
#define SMS_ERR_CLASS23_ADDRESS_TRANSLATION_FAILURE				1	/**< address translation failure */
#define SMS_ERR_CLASS23_NETWORK_RESOURCE_SHORTAGE					2	/**< network resource shortage */
#define SMS_ERR_CLASS23_NETWORK_FAILURE								3	/**< network failure */
#define SMS_ERR_CLASS23_INVALID_TELESERVICE_ID						4	/**< invalid teleservice id */
#define SMS_ERR_CLASS23_OTHER_NETWORK_PROBLEM						5	/**< other network problem */
#define SMS_ERR_CLASS23_OTHER_NETWORK_PROBLEM_MORE_FIRST			6	/**< other network problem more first */
#define SMS_ERR_CLASS23_OTHER_NETWORK_PROBLEM_MORE_LAST			31	/**< other network problem more last */
#define SMS_ERR_CLASS23_NO_PAGE_RESPONSE							32	/**< no page response */
#define SMS_ERR_CLASS23_DESTINATION_BUSY							33	/**< destination busy */
#define SMS_ERR_CLASS23_NO_ACK										34	/**< no ack */
#define SMS_ERR_CLASS23_DESTINATION_RESOURCE_SHORTAGE				35	/**< destination resource shortage */
#define SMS_ERR_CLASS23_SMS_DELIVERY_POSTPONED						36	/**< sms delivery postponed */
#define SMS_ERR_CLASS23_DESTINATION_OUT_OF_SERVICE					37	/**< destination out of service */
#define SMS_ERR_CLASS23_DESTINATION_NO_LONGER_AT_THIS_ADDRESS	38	/**< destination no longer at this address */
#define SMS_ERR_CLASS23_OTHER_TERMINAL_PROBLEM						39	/**< other terminal problem */
#define SMS_ERR_CLASS23_OTHER_TERMINAL_PROBLEM_MORE_FIRST		40	/**< other terminal problem more first */
#define SMS_ERR_CLASS23_OTHER_TERMINAL_PROBLEM_MORE_LAST			47	/**< other terminal problem more last */
#define SMS_ERR_CLASS23_SMS_DELIVERY_POSTPONED_MORE_FIRST			48	/**< sms delivery postpone more first */
#define SMS_ERR_CLASS23_SMS_DELIVERY_POSTPONED_MORE_LAST			63	/**< sms delivery postpone more last */
#define SMS_ERR_CLASS23_RADIO_IF_RESOURCE_SHORTAGE					64	/**< radio interface resource shortage */
#define SMS_ERR_CLASS23_RADIO_IF_INCOMPATIBLE						65	/**< radio interface incompatible */
#define SMS_ERR_CLASS23_OTHER_RADIO_IF_PROBLEM						66	/**< other radio interface problem */
#define SMS_ERR_CLASS23_OTHER_RADIO_IF_PROBLEM_MORE_FIRST			67	/**< other radio interface problem more first */
#define SMS_ERR_CLASS23_OTHER_RADIO_IF_PROBLEM_MORE_LAST			95	/**< other radio interface problem more last */
#define SMS_ERR_CLASS23_UNEXPECTED_PARM_SIZE						96	/**< unexpected parameter size */
#define SMS_ERR_CLASS23_SMS_ORIGINATION_DENIED						97	/**< sms origination denied */
#define SMS_ERR_CLASS23_SMS_TERMINATION_DENIED						98	/**< sms termination denied */
#define SMS_ERR_CLASS23_SUPPL_SERVICE_NOT_SUPPORTED				99	/**< supplementary service not supported */
#define SMS_ERR_CLASS23_SMS_NOT_SUPPORTED							100	/**< sms not supported */
#define SMS_ERR_CLASS23_RESERVED_101									101	/**< reserved 101 */
#define SMS_ERR_CLASS23_MISSING_EXPECTED_PARM						102	/**< missing expected parameter */
#define SMS_ERR_CLASS23_MISSING_MANDATORY_PARM					103	/**< missing mandatory parameter value */
#define SMS_ERR_CLASS23_UNRECOGNIZED_PARM_VALUE					104	/**< unrecognized parameter value */
#define SMS_ERR_CLASS23_UNEXPECTED_PARM_VALUE						105	/**< unexpected parameter value */
#define SMS_ERR_CLASS23_USER_DATA_SIZE_ERROR						106	/**< user data size error */
#define SMS_ERR_CLASS23_OTHER_GENERAL_PROBLEMS						107	/**< other general problem */
#define SMS_ERR_CLASS23_OTHER_GENERAL_PROBLEMS_MORE_FIRST		108	/**< other general problem first */
#define SMS_ERR_CLASS23_OTHER_GENERAL_PROBLEMS_MORE_LAST			255	/**< other general problem last */

#define SMS_ERR_CLASS4_WAITING_FOR_TL_ACK							1	/**< waiting for transport ack */
#define SMS_ERR_CLASS4_OUT_OF_RESOURCES								2	/**< out of resource */
#define SMS_ERR_CLASS4_ACCESS_TOO_LARGE								3	/**< access to large */
#define SMS_ERR_CLASS4_DTC_TOO_LARGE									4	/**< DTC too large */
#define SMS_ERR_CLASS4_DTC_CONNECTED									5	/**< DTC connected */
#define SMS_ERR_CLASS4_NETWORK_NOT_READY							6	/**< network not ready */
#define SMS_ERR_CLASS4_NO_SVC											7	/**< no service */
#define SMS_ERR_CLASS4_PHONE_NOT_READY								8	/**< phone not ready */
#define SMS_ERR_CLASS4_NOT_ALLOWED_IN_AMPS							9	/**< fail to allowed in amps */
#define SMS_ERR_CLASS4_CANNOT_SEND_BROADCAST						10	/**< fail to send broadcast */
#define SMS_ERR_CLASS4_INVALID_TRANSACTION_ID
// ********************************************************************//

/*==================================================================================================
                                             ENUMS
==================================================================================================*/
/**
 * @enum telephony_sms_MsgStatus_t
 * This enumeration defines the network text status type.
 */
enum telephony_sms_MsgStatus {
	SMS_STATUS_READ,						/**< MT message, stored and read */
	SMS_STATUS_UNREAD,					/**< MT message, stored and unread */
	SMS_STATUS_SENT,						/**< MO message, stored and  sent */
	SMS_STATUS_UNSENT,					/**< MO message, stored but not sent */
	SMS_STATUS_DELIVERED,					/**< delivered destination */
	SMS_STATUS_DELIVERY_UNCONFIRMED,	/**< Service centre forwarded message but is unable to confirm delivery*/
	SMS_STATUS_MESSAGE_REPLACED,		/**< Message has been replaced*/
	SMS_STATUS_RESERVED					/**< Reserved for future use*/
};

/**
 * @enum telephony_sms_MemStatusType
 * This enumeration defines the memory status type.
 */
enum telephony_sms_MemStatusType {
	SMS_PDA_MEMORY_STATUS_AVAILABLE	= 0x01, /**< PDA Memory Available */
	SMS_PDA_MEMORY_STATUS_FULL			= 0x02,  /**< PDAMemory is Full */
	SMS_PHONE_MEMORY_STATUS_AVAILABLE	= 0x03, /**< Phone memory Available */
	SMS_PHONE_MEMORY_STATUS_FULL		= 0x04, /**< phone memory is full */
};

/**
 * @enum telephony_sms_BearerType_t
 * This enum defines the different bearer types
 */
enum telephony_sms_BearerType {
	SMS_BEARER_PS_ONLY = 0x01,       /**< Send SMS only on PS Bearer */
	SMS_BEARER_CS_ONLY,                  /**< Send SMS only on CS Bearer */
	SMS_BEARER_PS_PREFERRED,        /**<Send SMS preferably on PS Bearer*/
	SMS_BEARER_CS_PREFERRED ,       /**<Send SMS preferably on CS Bearer*/
	SMS_NO_PREFERRED_BEARER         /**<SMS is sent based on default preferred bearer set at OEM based on vendor/operator preference*/
};


/**
* @enum telephony_sms_CbMsgType_t
* This enumeration defines the different CB message types.
*/
enum telephony_sms_CbMsgType {
		SMS_CB_MSG_GSM = 1,	 /**< GSM Cell broadcast message */
		SMS_CB_MSG_UMTS      /**< UMTS Cell broadcast message */
};

/**
* @enum telephony_sms_etws_type
* This enumeration defines the different ETWS message types.
*/
enum telephony_sms_etws_type {
		SMS_ETWS_MSG_PRIMARY = 0,				/**< PRIMARY ETWS Message */
		SMS_ETWS_MSG_SECONDARY_GSM,				/**< SECONDARY GSM ETWS Message */
		SMS_ETWS_MSG_SECONDARY_UMTS				/**< SECONDARY UMTS ETWS Message */
};

/**
 * @enum telephony_sms_Response_t
 * This enum defines the different response types that come in the
 * sent status acknowledgement/notification after sending a message to the network
 */
enum telephony_sms_Response {
	SMS_SENDSMS_SUCCESS = 0x00,					/**<Message send success*/
	SMS_ROUTING_NOT_AVAILABLE,					/**< Message routing not available*/
	SMS_INVALID_PARAMETER,						/**< Invalid parameter present in TPDU*/
	SMS_DEVICE_FAILURE,							/**< Device failure*/
	SMS_SERVICE_RESERVED,						/**< Reserved Service*/
	SMS_INVALID_LOCATION,						/**< Invalid location*/
	SMS_NO_SIM,									/**< No SIM error*/
	SMS_SIM_NOT_READY,							/**< SIM not ready error*/
	SMS_NO_NETWORK_RESP,						/**< No response from network*/
	SMS_DEST_ADDRESS_FDN_RESTRICTED,			/**< Destination address restricted*/
	SMS_SCADDRESS_FDN_RESTRICTED,				/**< Service center address restricted*/
	SMS_RESEND_ALREADY_DONE,					/**< Resend a operation already done*/
	SMS_SCADDRESS_NOT_AVAILABLE,				/**< SCA address not available*/
	SMS_UNASSIGNED_NUMBER = 0x8001,				/**< Unassigned number*/
	SMS_OPERATOR_DETERMINED_BARRING = 0x8008,	/**< Operator determined barring*/
	SMS_CALL_BARRED = 0x800A,					/**< Call barred*/
	SMS_MESSAGE_TRANSFER_REJECTED = 0x8015,		/**< Message transfer rejected*/
	SMS_MEMORY_CAPACITY_EXCEEDED = 0x8016,		/**< Memory capacity exceeded/memory full*/
	SMS_DESTINAITION_OUTOFSERVICE = 0x801B,		/**< Destination number/address out of service*/
	SMS_UNSPECIFIED_SUBSCRIBER = 0x801C,		/**< Unspecified subscriber*/
	SMS_FACILITY_REJECTED = 0x801D,				/**< Facility rejected*/
	SMS_UNKNOWN_SUBSCRIBER = 0x801E,			/**< Unknown subscriber*/
	SMS_NETWORK_OUTOFORDER = 0x8026,			/**< Network out of order*/
	SMS_TEMPORARY_FAILURE = 0x8029,				/**< Temporary failure*/
	SMS_CONGESTION = 0x802A,					/**< Congestion happened*/
	SMS_RESOURCES_UNAVAILABLE = 0x802F,			/**< Resource unavailable*/
	SMS_FACILITY_NOT_SUBSCRIBED = 0x8032,		/**< Facility not subscribed by the user*/
	SMS_FACILITY_NOT_IMPLEMENTED = 0x8045,		/**< Facility not implemented*/
	SMS_INVALID_REF_VALUE = 0x8051,				/**< Invalid reference value*/
	SMS_INVALID_MSG = 0x805F,					/**< Invalid message*/
	SMS_INVALID_MANDATORY_INFO = 0x8060,		/**< Invalid Mandatory information*/
	SMS_MESSAGE_TYPE_NOT_IMPLEMENTED = 0x8061,	/**< Message type not implemented*/
	SMS_MESSAGE_NOT_COMPAT_PROTOCOL = 0x8062,	/**< Message not compact protocol*/
	SMS_IE_NOT_IMPLEMENTED = 0x8063,			/**< Information element not implemented*/
	SMS_PROTOCOL_ERROR = 0x806F,				/**< Protocol error*/
	SMS_INTERWORKING = 0x807F,					/**< Networking error*/
	SMS_ME_FULL = 0x8080,						/**< SMS ME FULL */
	SMS_SIM_FULL = 0x8081,						/**< SMS SIM FULL */
	SMS_TIMEOUT									/**< Timeout error */
};

 /** @enum  telephony_sms_Cause_t
 * This enum defines the different cause types that come in the
 * sent status acknowledgement/notification after sending a message to the network
 */

enum telephony_sms_Cause {

	SMS_SUCCESS,						/**< SMS Operation successful*/
	SMS_INVALID_PARAMETER_FORMAT,    /**< Invalid format for some parameters passed in Data package information(TPDU)*/
	SMS_PHONE_FAILURE,				/**<SMS operation failed due to Modem failure*/
	SMS_SIM_BUSY,						/**< SMS SIM operation cannot be performed as SIM is busy with some other operation*/
	SMS_SIM_FAILURE,					/**< SMS SIM operation cannot be performed due to SIM failure */
	SMS_UNKNOWN,						/**< unknown error*/
	SMS_MEMORY_FAILURE,				/**< Error while accessing memory or allocation of memory for SMS operation.*/
	SMS_OPERATION_NOT_SUPPORTED	/**< operation not allowed/supported*/
} ;

/**
 * @enum telephony_sms_SetResponse
 * This defines the response values
 */
enum telephony_sms_SetResponse {
	SMS_CBSETCONFIG_RSP,			/**<  cellbroadcast config response */
	SMS_SETPREFERREDBEARER_RSP,	/**<  set preferred bearer response */
	SMS_SETPARAMETERS_RSP,		/**<  set parameter response */
	SMS_SETMEMORYSTATUS_RSP,	/**<   set memory status response*/
	SMS_SETMESSAGESTATUS_RSP,	/**<   set message status response*/
	SMS_SETDEVICESTATUS_RSP,		/**<   set device status response*/
	SMS_SETSCADDR_RSP,			/**<   set SCA address response */
	SMS_SET_RSP_MAX				/**<   maximum limit*/
};

/**
 * @enum telephony_sms_3gpp_type
 * This defines the type of 3gpp
 */
enum telephony_sms_3gpp_type {
	SMS_NETTYPE_3GPP = 0x01,						/**< 3gpp type */
	SMS_NETTYPE_3GPP2,    /**< 3gpp2 type (CDMA) */
};


/**
 * This structure defines the fields related to an Sms like SIM index, TPDU
 * and length.
 */
struct telephony_sms_DataPackageInfo {
	/*
	 * Service Centre address - an optional parameter. If this parameter
	 * is not present, then this field will be Null.If present, the valid
	 * service centre address information is filled as per 3GPP TS23.040
	 * 9.1.2.5 Address fields
	 */
	unsigned char sca[SMS_ENCODED_SCA_LEN_MAX];
	/* Size of array szData (which is actual TPDU message) */
	int msgLength;
	/* SMS TPDU message */
	unsigned char tpduData[SMS_SMDATA_SIZE_MAX + 1];
};

/**
 *This structure defines the data Related to SimIndex,MessageStatus and SMS Data Stored.
 */
struct telephony_sms_Data {
	int									simIndex;	/**< Index where SMS is stored. */
	enum telephony_sms_MsgStatus			msgStatus;	/**< Message status.*/
	struct telephony_sms_DataPackageInfo	smsData;	/**<SMS message */
};

struct telephony_sms_AddressInfo {
	unsigned int	dialNumLen;								/* length of address. If Service center address is not set, then this will be zero */
	int			typeOfNum;								/* Type of number*/
	int			numPlanId;								/* Numbering plan identification*/
	unsigned char diallingNum[SMS_SMSP_ADDRESS_LEN + 1];	/* destination address. If Address not set, then set to 0xFF */
};

/**
 * This structure defines different fields involved in setting the parameters of
 * a particular sms in EFsmsp.
 */
struct telephony_sms_Params {

	unsigned char					recordIndex;						/**< Index*/
	unsigned char					recordLen;							/**< SMS Parameter Record length*/
	unsigned long						alphaIdLen;							/**< Alpha id length */
	char							szAlphaId[SMS_SMSP_ALPHA_ID_LEN_MAX + 1];	/**< Alpha id .It is a unique identifier for each row/record in EFsmsp */ //JYGU: TAPI_SIM_SMSP_ALPHA_ID_LEN_MAX 128
	unsigned char					paramIndicator;						/**< SMS parameter indicator is a 1 byte value. Each bit in this value indicates the presence/absence of the sms header parameters.If the parameter is present the corresponding bit value is set to 0.If the parameter is absent then it is set as 1.Refer 3GPP TS 31.02 :4.4.23 */
	struct telephony_sms_AddressInfo	tpDestAddr;							/**< TP-destination address (TP-DA) */
	struct telephony_sms_AddressInfo	tpSvcCntrAddr;						/**< TP-service center address */
	unsigned short					tpProtocolId;						/**< TP-protocol Id */
	unsigned short					tpDataCodingScheme;				/**< TP-data coding scheme */
	unsigned short					tpValidityPeriod;						/**< TP-validity period */
};

/**
 * This structure defines the different parameters that are related to the message count
 *in a particular memory(Phone/SIM)
 */
struct telephony_sms_StoredMsgCountInfo {
	unsigned int	totalCount;								/**< Total count of messages stored in SIM*/
	int			usedCount;								/**< Stored message count in SIM in SIM*/
	int			indexList[SMS_GSM_SMS_MSG_NUM_MAX];	/**< Message index list stored in SIM. And the maximum size of this array can be of totalCount.This array contains the list of index values in which the messages are stored.i.e. index_list[totalcount] = [2,3] denotes that indexs 2 and 3 are stored(valid) and others not(empty).*/
};

/**
 * This structure defines a cell broadcast message.
 */
struct telephony_sms_CbMsg {

	enum telephony_sms_CbMsgType	cbMsgType;							/**< Cell Broadcast  message type */
	unsigned short					length;								/**<Size of array szMsgData (which is actual TPDU message) */
	char							msgData[SMS_CB_SIZE_MAX + 1]; /**<Cell broadcast message data[Refer 3GPP TS 23.041 9.4.1]*/
};

struct telephony_sms_etws_msg {

	enum telephony_sms_etws_type	etwsMsgType;							/**< ETWS  message type */
	unsigned short					length;								/**<Size of array msgData (which is actual TPDU message) */
	char							msgData[SMS_ETWS_SIZE_MAX + 1]; /**< ETWS message data[Refer 3GPP TS 23.041 9.4.1.3]*/
};

struct telephony_sms_cb_msg_info_3gpp {
 unsigned short fromMsgId; /**< Starting point of the range of CBS message ID */
 unsigned short toMsgId; /**< Ending point of the range of CBS message ID */
 unsigned char selected; /**< 0x00 . Not selected. 0x01 . Selected */
};

struct telephony_sms_cb_msg_info_3gpp2 {
 unsigned short cbCategory; /**< CB Service category */
 unsigned short cbLanguage; /**< Language indicator value
								. 0x00 . LANGUAGE_UNKNOWN .
									Unknown or Unspecified
								. 0x01 . LANGUAGE_ENGLISH . English
								. 0x02 . LANGUAGE_FRENCH . French
								. 0x03 . LANGUAGE_SPANISH . Spanish
								. 0x04 . LANGUAGE_JAPANESE . Japanese
								. 0x05 . LANGUAGE_KOREAN . Korean
								. 0x06 . LANGUAGE_CHINESE . Chinese
								. 0x07 . LANGUAGE_HEBREW . Hebrew*/
 unsigned char selected; /**< 0x00 . Not selected. 0x01 . Selected */
};

union telephony_sms_cb_msg_info_u {
	struct telephony_sms_cb_msg_info_3gpp net3gpp; /**< 3GPP Broadcast Configuration Information */
	struct telephony_sms_cb_msg_info_3gpp2 net3gpp2; /**< 3GPP2 Broadcast Configuration Information, CDMA*/
};

struct telephony_sms_CbConfig {
	int net3gppType;  /**< Type of 3gpp, 0x01 . 3gpp. 0x02 . 3gpp2(CDMA) */
	int cbEnabled; /**< CB service state. If cb_enabled is true then cell broadcast service will be enabled and underlying modem will enable CB Channel to receiving CB messages. Otherwise CB service will be disabled, underlying modem will deactivate the CB channel. (enabled/disabled) */
	unsigned char msgIdMaxCount; /**< CB Channel List Max Count For Response */
	int msgIdRangeCount; /**< Range of CB message ID count */
	union telephony_sms_cb_msg_info_u msgIDs[SMS_GSM_SMS_CBMI_LIST_SIZE_MAX]; /**< Range of CB message ID information */
};


// ************************  CDMA Features  **************************//
 /** @enum  telephony_sms_Is637AlertPriority_t
 * This enum defines the type of alerts used to distinguish different priorities
 * of the message
 */
enum telephony_sms_Is637AlertPriority {
	SMS_ALERT_PRIORITY_DEFAULT = 0,	/* Mobile default alert */
	SMS_ALERT_PRIORITY_LOW,			/* Low priority alert */
	SMS_ALERT_PRIORITY_MEDIUM,		/* Medium priority alert */
	SMS_ALERT_PRIORITY_HIGH,			/* High priority alert */
};

 /** @enum  telephony_sms_Is637Privacy_t
 * This enum defines the desired privacy level of the message
 */
enum telephony_sms_Is637Privacy {
	SMS_PRIVACY_NOT_RESTRICTED = 0,	/* Not restricted */
	SMS_PRIVACY_RESTRICTED,			/* Restricted */
	SMS_PRIVACY_CONFIDENTIAL,		/* Confidential */
	SMS_PRIVACY_SECRET,				/* Secret */
};

 /** @enum  telephony_sms_Is637Priority_t
 * This enum defines the priority level of the message
 */
enum telephony_sms_Is637Priority {
	SMS_PRIORITY_NORMAL = 0,		/* Normal */
	SMS_PRIORITY_INTERACTIVE,		/* Interactive */
	SMS_PRIORITY_URGENT,			/* Urgent */
	SMS_PRIORITY_EMERGENCY,		/* Emergency */
};

 /** @enum  telephony_sms_Is637LangIndicator_t
 * This enum defines the language of the message so that the receiving mobile station can
 * discard those messages that are not in the user's preferred language
 */
enum telephony_sms_Is637LangIndicator {
	SMS_LANG_UNKNOWN = 0x0,			/* Unknown or unspecified */
	SMS_LANG_ENGLISH,				/* English */
	SMS_LANG_FRENCH,				/* French */
	SMS_LANG_SPANISH,				/* Spanish */
	SMS_LANG_JAPANESE,			/* Japanese */
	SMS_LANG_KOREAN,				/* Korean */
	SMS_LANG_CHINESE,				/* Chinese */
	SMS_LANG_HEBREW,				/* Hebrew */
};

 /** @enum  telephony_sms_Is637MsgDisplayMode_t
 * This enum defines the display mode to the mobile station when to display the received message
 */
enum telephony_sms_Is637MsgDisplayMode {
	SMS_MSG_DISPLAY_IMMEDIATE = 0x0,		/* The mobile station is to display the received message as soon as possible */
	SMS_MSG_DISPLAY_DEFAULT,				/* he mobile station is to display the received message based on a pre-defined mode in the mobile station. */
	SMS_MSG_DISPLAY_USER_INVOKE = 0x3,	/* The mobile station is to display the received message based on the mode selected by the user. */
	SMS_MSG_DISPLAY_RESERVED,			/* Reserved */
};

 /** @enum  telephony_sms_Is637ErrorClass_t
 * This enum defines the error report class
 */
enum telephony_sms_Is637ErrorClass {
	SMS_MSG_ERROR_CLASS_NONE = 0x0,						/* None error(for SMS ack) */
	SMS_MSG_ERROR_CLASS_TEMPORARY_ERROR = 0x2,			/* Temporary error(for SMS ack) */
	SMS_MSG_ERROR_CLASS_PERMANENT_ERROR = 0x3,			/* Permanent error(for SMS ack) */
	SMS_MSG_ERROR_CLASS_PHONE_INTERNAL_ERROR = 0x4,	/* Phone Internal Status (for Send status) */
};

 /** @enum  telephony_sms_BroadCastCategory_t
 * This enum defines the Broadcast Service Category
 */
enum telephony_sms_BroadCastCategory {
	SMS_CATEGORY_UNKNOWN		= 0x00,			/* Unknown category */
	SMS_CATEGORY_EMERGENCY		= 0x01,			/* Emergency category */
	SMS_CATEGORY_ADMIN			= 0x02,			/* Admin category */
	SMS_CATEGORY_MAINTENANCE	= 0x03,			/* Maintenance category */
	SMS_CATEGORY_GEN_NEWS_LOC	= 0x04,			/* General News(Local) category */
	SMS_CATEGORY_GEN_NEWS_REG	= 0x05,			/* General News(Regional) category */
	SMS_CATEGORY_GEN_NEWS_NAT	= 0x06,			/* General News(National) category */
	SMS_CATEGORY_GEN_NEWS_INT	= 0x07,			/* General News(International) category */
	SMS_CATEGORY_FIN_NEWS_LOC	= 0x08,			/* Business/Financial News(Local) category */
	SMS_CATEGORY_FIN_NEWS_REG	= 0x09,			/* Business/Financial News(Regional) category */
	SMS_CATEGORY_FIN_NEWS_NAT	= 0x0A,			/* Business/Financial News(National) category */
	SMS_CATEGORY_FIN_NEWS_INT	= 0x0B,			/* Business/Financial News(International) category */
	SMS_CATEGORY_SPT_NEWS_LOC	= 0x0C,			/* Sports News(Local) category */
	SMS_CATEGORY_SPT_NEWS_REG	= 0x0D,			/* Sports News(Regional) category */
	SMS_CATEGORY_SPT_NEWS_NAT	= 0x0E,			/* Sports News(National) category */
	SMS_CATEGORY_SPT_NEWS_INT	= 0x0F,			/* Sports News(International) category */
	SMS_CATEGORY_ENT_NEWS_LOC	= 0x10,			/* Entertainment News(Local) category */
	SMS_CATEGORY_ENT_NEWS_REG	= 0x11,			/* Entertainment News(Regional) category */
	SMS_CATEGORY_ENT_NEWS_NAT	= 0x12,			/* Entertainment News(National) category */
	SMS_CATEGORY_ENT_NEWS_INT	= 0x13,			/* Entertainment News(International) category */
	SMS_CATEGORY_LOC_WEATHER	= 0x14,			/* Local Weather category */
	SMS_CATEGORY_AREA_TRAFFIC	= 0x15,			/* Area Traffic Reports category */
	SMS_CATEGORY_AIRPORT_SCHED	= 0x16,			/* Local Airport Flight Schedules category */
	SMS_CATEGORY_RESTAURANTS	= 0x17,			/* Restaurants category */
	SMS_CATEGORY_LODGINGS		= 0x18,			/* Lodgings category */
	SMS_CATEGORY_RETAILS			= 0x19,			/* Retail Directory category */
	SMS_CATEGORY_ADS				= 0x1A,			/* Advertisements category */
	SMS_CATEGORY_STOCK_QUOTES	= 0x1B,			/* Stock Quotes category */
	SMS_CATEGORY_JOBS			= 0x1C,			/* Employment Opportunities category */
	SMS_CATEGORY_MEDICAL			= 0x1D,			/* Medical/Health/Hospitals category */
	SMS_CATEGORY_TECH_NEWS		= 0x1E,			/* Technology News category */
	SMS_CATEGORY_MULTI			=  0x1F,			/* Multi-category */
};

/** @enum  telephony_sms_MsgType_t
 * This enum defines the type of IS637 message
 */
enum telephony_sms_CdmaMsgType {
	SMS_MESSAGETYPE_DELIVER = 0x01,				/* sms deliver message  */
	SMS_MESSAGETYPE_SUBMIT = 0x02,				/* sms submit message  */
	SMS_MESSAGETYPE_CANCEL = 0x03,				/* sms cancellation message  */
	SMS_MESSAGETYPE_DELIVERY_ACK = 0x04,			/* sms delivery acknowledgment message  */
	SMS_MESSAGETYPE_USER_ACK = 0x05,				/* sms user acknowledgment message  */
} telephony_sms_CdmaMsgType_e;

/** @enum  telephony_sms_Is637DigitMode_t
 * This enum defines the type of address whether it is 4-bit mode or 8-bit mode
 */
enum telephony_sms_Is637DigitMode {
	SMS_DIGITMODE_4BITDTMF	= 0x00,		/* 4-bit mode  */
	SMS_DIGITMODE_8BITCODE	= 0x01,		/* 8-bit mode  */
};

/** @enum  telephony_sms_Is637NumberMode_t
 * This enum defines the mode of address number to indicate whether the address type is as defined in ANSI TI.607 or is a data network address
 */
enum telephony_sms_Is637NumberMode {
	SMS_NUMMODE_NONE_DATANETWORK =	0x00,	/* in ANSI TI.607 */
	SMS_NUMMODE_DATANETWORK = 0x01,			/* in Data Network */
};

/** @enum  telephony_sms_Is637NumberType_t
 * This enum defines the type of address
 */
enum telephony_sms_Is637NumberType {
	SMS_NUMBER_TYPE_UNKNOWN		= 0x00,		/*  Unknown */
	SMS_NUMBER_TYPE_INTERNATIONAL	= 0x01,		/*  International number*/
	SMS_NUMBER_TYPE_NATIONAL		= 0x02,		/*  National number */
	SMS_NUMBER_TYPE_NETWORK		= 0x03,		/*  Abbreviated number */
	SMS_NUMBER_TYPE_SUBSCRIBER		= 0x04,		/* Abbreviated number */
	SMS_NUMBER_TYPE_RESERVED_5		= 0x05,		/*  Reserved */
	SMS_NUMBER_TYPE_ABBREVIATED		= 0x06,		/*  Abbreviated number */
	SMS_NUMBER_TYPE_RESERVED_7		= 0x07,		/*  Reserved */
	SMS_NUMBER_TYPE_IP				= 0x11,		/*  Internet Protocol(RFC 791) */
	SMS_NUMBER_TYPE_EMAILADDR		= 0x12,		/*  Internet Email Address(RFC 822) */
} telephony_sms_Is637NumberType_e;

/** @enum  telephony_sms_Is637NumberPlan_t
 * This enum defines the plan of address
 */
enum telephony_sms_Is637NumberPlan {
	SMS_NUMBER_PLAN_UNKNOWN		= 0x00,	/*  Unknown */
	SMS_NUMBER_PLAN_TELEPHONY		= 0x01,	/* ISDN/Telephony numbering plan */
	SMS_NUMBER_PLAN_RESERVED_2		= 0x02,	/*  Reserved */
	SMS_NUMBER_PLAN_DATA			= 0x03,	/* Data numbering plan */
	SMS_NUMBER_PLAN_TELEX			= 0x04,	/* CTelex numbering plan */
	SMS_NUMBER_PLAN_RESERVED_5		= 0x05,	/*  Reserved */
	SMS_NUMBER_PLAN_RESERVED_6		= 0x06,	/*  Reserved */
	SMS_NUMBER_PLAN_RESERVED_7		= 0x07,	/*  Reserved */
	SMS_NUMBER_PLAN_RESERVED_8		= 0x08,	/*  Reserved */
	SMS_NUMBER_PLAN_PRIVATE			= 0x09,	/*  Private numbering plan */
	SMS_NUMBER_PLAN_RESERVED_10	= 0x0A,	/*  Reserved */
	SMS_NUMBER_PLAN_RESERVED_11	= 0x0B,	/*  Reserved */
	SMS_NUMBER_PLAN_RESERVED_12	= 0x0C,	/*  Reserved */
	SMS_NUMBER_PLAN_RESERVED_13	= 0x0D,	/*  Reserved */
	SMS_NUMBER_PLAN_RESERVED_14	= 0x0E,	/*  Reserved */
	SMS_NUMBER_PLAN_RESERVED_15	= 0x0F,	/*  Reserved */
};

/** @enum  telephony_sms_Is637SubAddressType_t
 * This enum defines the plan of address
 */
enum telephony_sms_Is637SubAddressType {

	SMS_SUBADDR_NSAP				= 0x00,	/**< NSAP ( CCITT Recommendation X.213 or ISO 8348 AD2 ) */
	SMS_SUBADDR_USER_SPECIFIED	= 0x01,	/**<  User-specified */
};

/**
 *  This structure defines cause code as an indication whether an SMS error has occurred and
 *  if so, whether the condition is considered temporary or permanent
 **/
struct telephony_sms_Is637CauseCode {
	unsigned char						ReplySeqNumber;	/* Reply sequence number */
	enum telephony_sms_Is637ErrorClass	ErrClass;			/* Error report class */
	unsigned char							Cause;				/* Error cause identifier */
};

/**
 * This structure defines the parameters of address and its type
 */
struct telephony_sms_Is637Address {
	enum telephony_sms_Is637DigitMode		Digit;			/* Digit mode indicator (0x00:4bit_dtmf, 0x01:8bit_code) */
	enum telephony_sms_Is637NumberMode	NumberMode;	/* Number mode indicator (0x00:ANSI TI607, 0x01:Data network) */
	enum telephony_sms_Is637NumberType	NumberType;	/* Type of number */
	enum telephony_sms_Is637NumberPlan	NumberPlan;	/* Numbering plan */
	unsigned char						szAddrLength;	/* The number of CHARi */
	unsigned char						szAddress[SMS_MAXLENGTH_SMS_ADDRESS]; /* The address digit or character */
};

/**
 * This structure defines the parameters of subaddress of originator and destination
 */
struct telephony_sms_Is637SubAddress {
	enum telephony_sms_Is637SubAddressType	SubType;		/* Subaddress type */
	unsigned char							Odd;			/* Odd/even indicator */
	unsigned char							szAddrLength;	/* The number of CHARi */
	unsigned char							szAddress[SMS_MAXLENGTH_SMS_ADDRESS]; /* A subaddress octet */
};

/**
 * This structure defines the message center time stamp may be include
 * with SMS message sent from a message center
 */
struct telephony_sms_TimeStamp {
	unsigned int	year;		/* Year - if the year is 2002, the year field contains 0x02 */
	unsigned int	month;		/* Month (1 ~ 12) */
	unsigned int	day;		/* Day (1 ~ 31) */
	unsigned int	hours;		/* Hours (0 ~ 23) */
	unsigned int	minutes;	/* Minute (0 ~ 59) */
	unsigned int	seconds;	/* Seconds (0 ~ 59) */
};

/**
 * The structure defines the parameter of SMS submit message
 */
struct telephony_sms_Is637OutSubmit {
	struct telephony_sms_Is637Address		DstAddr; /* Destination address */
	struct telephony_sms_Is637SubAddress	DstSubAddr; /* Destination subaddress */
	unsigned short						TeleService; /* Teleservice Identifier */
	int									bBearerReplySeqRequest; /* Bearer reply sequence request */
	unsigned char							ReplySeqNumber; /* Reply sequence number */
	unsigned short						MsgId; /* Message identifier */
	unsigned short						MsgEncoding; /* Message encoding (defined in TSB-58A) */
	unsigned short						MsgLength; /* Message length */
	unsigned char							szData[SMS_MAXLENGTH_SMS_MO_USER_DATA]; /* SMS user data */
	struct telephony_sms_TimeStamp		ValidityPeriodAbs; /* Validity period - Absolute (not supported) */
	unsigned char							ValidityPeriodRel; /* Validity period - Relative (not supported) */
	struct telephony_sms_TimeStamp		DeferredDelTimeAbs; /* Deferred delivery time - Absolute */
	unsigned char							DeferredDelTimeRel; /* Deferred delivery time - Relative */
	enum telephony_sms_Is637Priority		Priority; /* Priority indicator */
	enum telephony_sms_Is637Privacy		Privacy; /* Privacy indicator */
	int									bUserAckRequest; /* User acknowledge request */
	int									bDeliveryAckRequest; /* Delivery acknowledge request */
	enum telephony_sms_Is637AlertPriority	AlertPriority; /* Alert priority of message */
	enum telephony_sms_Is637LangIndicator	MsgLang; /* Language indicator */
	struct telephony_sms_Is637Address		CallBackNumber; /* Callback number address */
};

/**
 * The structure defines the parameter of SMS acknowledgement message for submit
 */
struct telephony_sms_Is637OutAck {
	struct telephony_sms_Is637Address		DstAddr; /* Destination address */
	struct telephony_sms_Is637SubAddress	DstSubAddr; /* Destination subaddress */
	unsigned short						TeleService; /* Teleservice Identifier */
	int									bBearerReplySeqRequest; /* Bearer reply sequence request */
	unsigned char						ReplySeqNumber; /* Reply sequence number */
	unsigned short						MsgId; /* Message identifier */
	unsigned short						MsgEncoding; /* Message encoding (defined in TSB-58A) */
	unsigned short						MsgLength; /* Message length */
	unsigned char							szData[SMS_MAXLENGTH_SMS_MO_USER_DATA]; /* SMS user data */
	unsigned char							UserResponseCode; /* User response code */
};

/**
 * The structure defines the parameter of SMS cancellation message
 */
struct telephony_sms_Is637OutCancel {
	struct telephony_sms_Is637Address		DstAddr; /* Destination address */
	struct telephony_sms_Is637SubAddress	DstSubAddr; /* Destination subaddress */
	unsigned short						TeleService; /* Teleservice Identifier */
	int									bBearerReplySeqRequest; /* Bearer reply sequence request */
	unsigned char							ReplySeqNumber; /* Reply sequence number */
	unsigned short						MsgId; /* Message identifier */
};

/**
 * The structure defines the parameter of SMS devivery message
 */
struct telephony_sms_Is637InDeliver {
	struct telephony_sms_Is637Address			OrigAddr; /* Origination address */
	struct telephony_sms_Is637SubAddress		OrigSubAddr; /* Origination subaddress */
	unsigned short							TeleService; /* Teleservice Identifier */
	int										bBearerReplySeqRequest; /* Bearer reply sequence request */
	unsigned char								ReplySeqNumber; /* Reply sequence number */
	unsigned short							MsgId; /* Message identifier */
	unsigned short							MsgEncoding; /* Message encoding (defined in TSB-58A) */
	unsigned short							MsgLength; /* Message length */
	unsigned char								szData[SMS_MAXLENGTH_SMS_MT_USER_DATA]; /* SMS user data */
	struct telephony_sms_TimeStamp			MessageCenterTimeStamp; /* Message center time stamp */
	struct telephony_sms_TimeStamp			ValidityPeriodAbs; /* Validity period - Absolute */
	unsigned char								ValidityPeriodRel; /* Validity period - Relative */
	struct telephony_sms_TimeStamp			DeferredDelTimeAbs; /* Deferred delivery time - Absolute (not supported) */
	unsigned char								DeferredDelTimeRel; /* Deferred delivery time - Relative (not supported) */
	enum telephony_sms_Is637Priority			Priority; /* Priority indicator */
	enum telephony_sms_Is637Privacy			Privacy; /* Privacy indicator */
	unsigned char								NumMsg; /* Number of voice mail (0-99) */
	int										bUserAckRequest; /* User acknowledge request */
	int										bDeliveryAckRequest; /* Delivery acknowledge request */
	enum telephony_sms_Is637AlertPriority		AlertPriority; /* Alert priority of message */
	enum telephony_sms_Is637LangIndicator		MsgLang; /* Language indicator */
	struct telephony_sms_Is637Address			CallBackNumer; /* Callback number address */
	enum telephony_sms_Is637MsgDisplayMode	Display; /* Message display mode */
};

/**
 * The structure defines the parameter of SMS acknowledge message for deliver
 */
struct telephony_sms_Is637InAck {
	struct telephony_sms_Is637Address		OrigAddr; /* Origination address */
	struct telephony_sms_Is637SubAddress	OrigSubAddr; /* Origination subaddress */
	unsigned short						TeleService; /* Teleservice Identifier */
	int									bBearerReplySeqRequest; /* Bearer reply sequence request */
	unsigned char							ReplySeqNumber; /* Reply sequence number */
	unsigned short						MsgId; /* Message identifier */
	unsigned short						MsgEncoding; /* Message encoding (defined in TSB-58A) */
	unsigned short						MsgLength; /* Message length */
	unsigned char							szData[SMS_MAXLENGTH_SMS_MT_USER_DATA]; /* SMS user data */
	unsigned char							UserResponseCode; /* User response code */
	struct telephony_sms_TimeStamp		MessageCenterTimeStamp; /* Message center time stamp */
};

/**
 * The structure defines the parameter of SMS acknowledge message from message center
 */
struct telephony_sms_Is637InDeliverAck {
	struct telephony_sms_Is637Address		OrigAddr; /* Origination address */
	struct telephony_sms_Is637SubAddress	OrigSubAddr; /* Origination subaddress */
	unsigned short						TeleService; /* Teleservice Identifier */
	int									bBearerReplySeqRequest; /* Bearer reply sequence request */
	unsigned char							ReplySeqNumber; /* Reply sequence number */
	unsigned short						MsgId; /* Message identifier */
	unsigned short						MsgEncoding; /* Message encoding (defined in TSB-58A) */
	unsigned short						MsgLength; /* Message length */
	unsigned char							szData[SMS_MAXLENGTH_SMS_MT_USER_DATA]; /* SMS user data */
	struct telephony_sms_TimeStamp		MessageCenterTimeStamp; /* Message center time stamp */
};

/**
 * The structure defines the parameter of SMS broadcast message
 */
struct telephony_sms_Is637InBroadCast {
	enum telephony_sms_BroadCastCategory		ServiceCategory; /* Broadcast service category */
	unsigned short							MsgId; /* Message identifier */
	int										bBearerReplySeqRequest; /* Bearer reply sequence request */
	unsigned char								ReplySeqNumber; /* Reply sequence number */
	unsigned short							MsgEncoding; /* Message encoding (defined in TSB-58A) */
	unsigned short							MsgLength; /* Message length */
	unsigned char								szData[SMS_MAXLENGTH_SMS_MT_USER_DATA]; /* SMS user data */
	struct telephony_sms_TimeStamp			MessageCenterTimeStamp; /* Message center time stamp */
	struct telephony_sms_TimeStamp			ValidityPeriodAbs; /* Validity period - Absolute */
	unsigned char								ValidityPeriodRel; /* Validity period - Relative */
	enum telephony_sms_Is637Priority			Priority; /* Priority indicator */
	enum telephony_sms_Is637AlertPriority		AlertPriority; /* Alert priority of message */
	enum telephony_sms_Is637LangIndicator		MsgLang; /* Language indicator */
	enum telephony_sms_Is637MsgDisplayMode	Display; /* Message display mode */
} telephony_sms_Is637InBroadCast_t;


/**
 * The structure defines the type of SMS message as union
 */
union telephony_sms_CdmaMessage_u {
	struct telephony_sms_Is637OutSubmit	outSubmit;		/* Outgoing Submit message */
	struct telephony_sms_Is637OutAck		outAck;			/* Outgoing Acknowledge message */
	struct telephony_sms_Is637OutCancel	outCancel;		/* Outgoing Cancel message */
	struct telephony_sms_Is637InDeliver		inDeliver;		/* Incoming Delivery message */
	struct telephony_sms_Is637InAck		inAck;			/* Incoming Acknowledge message */
	struct telephony_sms_Is637InDeliverAck	inDeliverAck;	/* Incoming Delivery Acknowledge message */
	struct telephony_sms_Is637InBroadCast	inBc;			/* Incoming Broadcast message */
};

/**
 * The structure defines the parameter of entire SMS message of each type
 */
struct telephony_sms_CdmaMsgInfo {
	int									ParamMask; /**< Parameter ID mask */
	enum telephony_sms_CdmaMsgType		MsgType; /**< Message type */
	union telephony_sms_CdmaMessage_u	MsgData; /**< Message data */
};

/*-----------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------*/


/*---------------------------------*/
/*                Structs of Requests              */
/*---------------------------------*/
struct treq_sms_send_umts_msg {
	struct telephony_sms_DataPackageInfo	msgDataPackage;
	int									more;
};

struct treq_sms_send_cdma_msg {
	struct telephony_sms_CdmaMsgInfo	cdmaMsgInfo;
	unsigned int						more;
};

struct treq_sms_read_msg {
	int	index;
};

struct treq_sms_save_msg {
	int									simIndex;			/* Index where SMS is stored. */
	enum telephony_sms_MsgStatus			msgStatus;
	struct telephony_sms_DataPackageInfo	msgDataPackage;
};

struct treq_sms_delete_msg {
	int	index;
};

struct treq_sms_get_msg_count {
};

struct treq_sms_get_sca {
	int	index;
};

struct treq_sms_set_sca {
	struct telephony_sms_AddressInfo	scaInfo;
	int								index;
};

struct treq_sms_get_cb_config {
};

struct treq_sms_set_cb_config {
	int net3gppType;  /**< Type of 3gpp, 0x01 . 3gpp. 0x02 . 3gpp2(CDMA) */
	int cbEnabled; /**< CB service state. If cb_enabled is true then cell broadcast service will be enabled and underlying modem will enable CB Channel to receiving CB messages. Otherwise CB service will be disabled, underlying modem will deactivate the CB channel. (enabled/disabled) */
	unsigned char msgIdMaxCount; /**< CB Channel List Max Count For Response */
	int msgIdRangeCount; /**< Range of CB message ID count */
	union telephony_sms_cb_msg_info_u msgIDs[SMS_GSM_SMS_CBMI_LIST_SIZE_MAX]; /**< Range of CB message ID information */
};

struct treq_sms_set_mem_status {
	int	memory_status;
};

struct treq_sms_get_pref_bearer {
};

struct treq_sms_set_pref_bearer {
	int	svc;
};

struct treq_sms_set_delivery_report {
	struct telephony_sms_DataPackageInfo	dataInfo;
	int									rspType;
};

struct treq_sms_set_msg_status {
	int							index;
	enum telephony_sms_MsgStatus	msgStatus;
};

struct treq_sms_get_params {
	int	index;
};

struct treq_sms_set_params {
	struct telephony_sms_Params	params;
};

struct treq_sms_get_paramcnt {
};


/* Structs of Responses */
struct tresp_sms_send_umts_msg {
	int result;
};

struct tresp_sms_read_msg {
	struct telephony_sms_Data	dataInfo;
	int						result;
};

struct tresp_sms_save_msg {
	int	index;
	int	result;
};

struct tresp_sms_delete_msg {
	int	index;
	int	result;
};

struct tresp_sms_get_storedMsgCnt {
	struct telephony_sms_StoredMsgCountInfo	storedMsgCnt;
	int										result;
};

struct tresp_sms_get_sca {
	struct telephony_sms_AddressInfo	scaAddress;
	int								result;
};

struct tresp_sms_set_sca {
	int	result;
};

struct tresp_sms_get_cb_config {
	struct telephony_sms_CbConfig	cbConfig;
	int							result;
};

struct tresp_sms_set_cb_config {
	int	result;
};

struct tresp_sms_set_mem_status {
	int	result;
};

struct tresp_sms_get_pref_bearer {
	int	result;
};

struct tresp_sms_set_pref_bearer {
	int	result;
};

struct tresp_sms_set_delivery_report {
	int	result;
};

struct tresp_sms_set_msg_status {
	int	result;
};

struct tresp_sms_get_params {
	struct telephony_sms_Params	paramsInfo;
	int							result;
};

struct tresp_sms_set_params {
	int	result;
};

struct tresp_sms_get_paramcnt {
	int	recordCount;
	int	result;
};

struct tresp_sms_send_cdma_msg {
	struct telephony_sms_Is637CauseCode	causeCode;
	int									result;
};

/*-----------------------------------*/
/*                Structs of Notifications              */
/*-----------------------------------*/
struct tnoti_sms_umts_msg {
	struct telephony_sms_DataPackageInfo	msgInfo;
};

struct tnoti_sms_cdma_msg {
	struct telephony_sms_CdmaMsgInfo	cdmaMsg;
};

struct tnoti_sms_cellBroadcast_msg {
	struct telephony_sms_CbMsg	cbMsg;
};

struct tnoti_sms_etws_msg {
	struct telephony_sms_etws_msg	etwsMsg;
};

struct tnoti_sms_memory_status {
	int	status;
};

struct tnoti_sms_ready_status {
	gboolean	status;
};

__END_DECLS

#endif

