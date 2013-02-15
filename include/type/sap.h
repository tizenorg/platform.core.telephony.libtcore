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

#ifndef __TYPE_SAP_H__
#define __TYPE_SAP_H__

__BEGIN_DECLS

#include <glib.h>

enum tel_sap_power_mode {
	SAP_POWER_ON,
	SAP_POWER_OFF,
	SAP_POWER_RESET
};

enum tel_sap_protocol_type {
	SAP_PROTOCOL_T0, /**< T = 0, character */
	SAP_PROTOCOL_T1 /**< T = 1, block */
};

enum tel_sap_connection_status {
	SAP_CONNECTION_STATUS_OK = 0x00, /**< connect successfully */
	SAP_CONNECTION_STATUS_UNABLE_TO_ESTABLISH, /**< unable to establish connection */
	SAP_CONNECTION_STATUS_NOT_SUPPORT_MAX_SIZE, /**< when server does not support message length that client want send */
	SAP_CONNECTION_STATUS_TOO_SMALL_MAX_SIZE /**< when client want to connect with very small message length which is not supported by Server */
};

enum tel_sap_disconnect_type {
	SAP_DISCONNECT_TYPE_GRACEFUL = 0x00, /**< disconnection procedure ends after finishing current work */
	SAP_DISCONNECT_TYPE_IMMEDIATE /**<  disconnection procedure ends immediately*/
};

enum tel_sap_status { //CARD READER STATUS
	SAP_STATUS_UNKNOWN = 0x00, /**<  SAP server connection status - unknown*/
	SAP_STATUS_NO_SIM, /**<  SAP server connection status - no SIM*/
	SAP_STATUS_NOT_READY, /**<  SAP server connection status - not ready*/
	SAP_STATUS_READY, /**<  SAP server connection status - ready*/
	SAP_STATUS_CONNECTED /**<  SAP server connection status - connected*/
};

enum tel_sap_status_change {
	SAP_CARD_STATUS_UNKNOWN = 0x00, /**<  SAP server status(card reader status) - unknown*/
	SAP_CARD_STATUS_RESET, /**<  SAP server status(card reader status) - reset*/
	SAP_CARD_STATUS_NOT_ACCESSIBLE, /**<  SAP server status(card reader status) - not accessible*/
	SAP_CARD_STATUS_REMOVED, /**<  SAP server status(card reader status) - removed*/
	SAP_CARD_STATUS_INSERTED, /**<  SAP server status(card reader status) - inserted*/
	SAP_CARD_STATUS_RECOVERED /**<  SAP server status(card reader status) - recovered*/
};

enum tel_sap_result_code {
	SAP_RESULT_CODE_OK = 0x00, /**<  SAP operation result - OK*/
	SAP_RESULT_CODE_NO_REASON, /**<  SAP operation result - no reason*/
	SAP_RESULT_CODE_CARD_NOT_ACCESSIBLE, /**<  SAP operation result - not accessible*/
	SAP_RESULT_CODE_CARD_ALREADY_POWER_OFF, /**<  SAP operation result - card already power off*/
	SAP_RESULT_CODE_CARD_REMOVED, /**<  SAP operation result - card removed*/
	SAP_RESULT_CODE_CARD_ALREADY_POWER_ON, /**<  SAP operation result - card already power on*/
	SAP_RESULT_CODE_DATA_NOT_AVAILABLE, /**<  SAP operation result - data not available*/
	SAP_RESULT_CODE_NOT_SUPPORT /**<  SAP operation result - not support*/
};

struct treq_sap_req_connect {
	unsigned short max_msg_size;
};

struct treq_sap_req_disconnect {
	/*NO Parameter*/
};

struct treq_sap_req_status {
	/*NO Parameter*/
};

struct treq_sap_req_atr {
	/*NO Parameter*/
};

struct treq_sap_transfer_apdu {
	unsigned int apdu_length;
	unsigned char apdu_data[256+2];
};

struct treq_sap_set_protocol {
	enum tel_sap_protocol_type protocol;
};

struct treq_sap_set_power {
	enum tel_sap_power_mode mode;
};

struct treq_sap_req_cardreaderstatus {
	/*NO Parameter*/
};

struct tresp_sap_req_connect {
	enum tel_sap_connection_status status;
	unsigned short max_msg_size;
};

struct tresp_sap_req_disconnect {
	enum tel_sap_result_code result;
};

struct tresp_sap_req_status {
	enum tel_sap_status status;
};

struct tresp_sap_req_atr {
	enum tel_sap_result_code result;
	unsigned int atr_length;
	unsigned char atr[256];
};

struct tresp_sap_transfer_apdu {
	enum tel_sap_result_code result;
	unsigned int resp_apdu_length;
	unsigned char resp_adpdu[256+2];
};

struct tresp_sap_set_protocol {
	enum tel_sap_result_code result;
};

struct tresp_sap_set_power {
	enum tel_sap_result_code result;
};

struct tresp_sap_req_cardreaderstatus {
	enum tel_sap_result_code result;
	char reader_status;
};

struct tnoti_sap_status_changed {
	enum tel_sap_status_change status;
};

struct tnoti_sap_disconnect {
	enum tel_sap_disconnect_type type;
};

__END_DECLS

#endif
