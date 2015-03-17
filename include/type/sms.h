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

#define SMS_SMSP_ADDRESS_LEN				20		/* EF-SMSP digit length */
#define SMS_SMSP_ALPHA_ID_LEN_MAX			128		/* EF-SMSP alpha id length */
#define SMS_MAX_EFSMSP_RECORD_LENGTH		156		/* Maximum number of bytes SMSP Record size (Y + 28), y : 0 ~ 128 */

#define SMS_MSG_SIZE_MAX					918		/**< Maximum Message Size */
#define SMS_CB_SIZE_MAX						1252		/** Maximum CB Message Size */
#define SMS_ETWS_SIZE_MAX					1252	/** Maximum ETWS Message Size */

#define SMS_ADDRESS_LEN_MAX					20		/* Nettext Address Length */
#define SMS_SCADDRESS_LEN_MAX				18		/* SC Address Length */

#define SMS_CB_PAGE_SIZE_MAX				9		/**< CB maximum page size*/
#define SMS_GSM_SMS_MSG_NUM_MAX				255		/**< Maximum GSM SMS message number*/
#define SMS_GSM_SMS_CBMI_LIST_SIZE_MAX		50		/**< Maximum GSM SMS CBMI list size*/
#define SMS_SMDATA_SIZE_MAX					255		/**< Maximum SMS data size that can be stored*/
#define SMS_MAX_SMS_SERVICE_CENTER_ADDR		12		/**<Maximum SMS service center address*/
#define SMS_MAX_INDEX						255		/**< Maximum index value for SMS */

#define SMS_SMSP_PARAMS_MAX_LEN				28

/*==================================================================================================
                                             ENUMS
==================================================================================================*/
/**
 * @enum telephony_sms_ready_status
 * This enumeration defines the ready status type.
 */
enum telephony_sms_ready_status {
  SMS_READY_STATUS_NONE = 0x00,
  SMS_READY_STATUS_3GPP = 0x01,
  SMS_READY_STATUS_3GPP2 = 0x02,
  SMS_READY_STATUS_3GPP_AND_3GPP2 = 0x03,
};

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
		SMS_CB_MSG_UMTS,     /**< UMTS Cell broadcast message */
		SMS_CB_MSG_CDMA
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
	SMS_NETTYPE_MWI,    /**< Message waiting indication */
};


/**
 * This structure defines the fields related to an Sms like SIM index, TPDU  and length
 */
struct telephony_sms_DataPackageInfo {
	enum telephony_sms_3gpp_type format;
	unsigned char	sca[SMS_SMSP_ADDRESS_LEN];			/* Service Centre address - an optional parameter. If this parameter is not present, then this field will be Null.If present, the valid service centre address information is filled as per 3GPP TS23.040 9.1.2.5 Address fields */
	int			msgLength;							/* Size of array szData (which is actual TPDU message) */
	unsigned char tpduData[SMS_SMDATA_SIZE_MAX + 1];	/* SMS TPDU message */
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
 * in a particular memory(Phone/SIM)
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

/*-----------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------*/


/*---------------------------------*/
/*                Structs of Requests              */
/*---------------------------------*/
/*Request structure common for both 3GPP and 3GPP2*/
struct treq_sms_send_msg {
	struct telephony_sms_DataPackageInfo msgDataPackage;
	int more;
};

struct treq_sms_read_msg {
	int	index;
};

struct treq_sms_save_msg {
	int simIndex;			/* Index where SMS is stored. */
	enum telephony_sms_MsgStatus msgStatus;
	struct telephony_sms_DataPackageInfo msgDataPackage;
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
	struct telephony_sms_AddressInfo scaInfo;
	int index;
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
	struct telephony_sms_DataPackageInfo dataInfo;
	int rspType;
};

struct treq_sms_set_msg_status {
	int index;
	enum telephony_sms_MsgStatus msgStatus;
};

struct treq_sms_get_params {
	int	index;
};

struct treq_sms_set_params {
	struct telephony_sms_Params	params;
};

struct treq_sms_get_paramcnt {
};

/*----------------------------------*/
/*                Structs of Responses              */
/*----------------------------------*/
struct tresp_sms_send_msg {
	int	result;
};

struct tresp_sms_read_msg {
	struct telephony_sms_Data	dataInfo;
	int	result;
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
	int result;
};

struct tresp_sms_get_sca {
	struct telephony_sms_AddressInfo	scaAddress;
	int result;
};

struct tresp_sms_set_sca {
	int	result;
};

struct tresp_sms_get_cb_config {
	struct telephony_sms_CbConfig	cbConfig;
	int result;
};

struct tresp_sms_set_cb_config {
	int	result;
};

struct tresp_sms_set_mem_status {
	int	result;
};

struct tresp_sms_get_pref_bearer {
	int	svc;
	int	result;
};

struct tresp_sms_set_pref_bearer {
	int	svc;
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
	int result;
};

struct tresp_sms_set_params {
	int	result;
};

struct tresp_sms_get_paramcnt {
	int	recordCount;
	int	result;
};

/*-----------------------------------*/
/*                Structs of Notifications              */
/*-----------------------------------*/
struct tnoti_sms_incoming_msg {
	struct telephony_sms_DataPackageInfo msgInfo;
};
struct tnoti_sms_cellBroadcast_msg {
	struct telephony_sms_CbMsg cbMsg;
};

struct tnoti_sms_etws_msg {
	struct telephony_sms_etws_msg etwsMsg;
};

struct tnoti_sms_memory_status {
	int	status;
};

struct tnoti_sms_ready_status {
	enum telephony_sms_ready_status status;
};

__END_DECLS

#endif

