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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "queue.h"
#include "user_request.h"
#include "core_object.h"
#include "util.h"
#include "co_sat.h"

#define SATK_PROACTIVE_CMD_TAG	0xD0  /*Proactive Command Tag*/
#define SATK_MENU_SELECTION_TAG	0xD3  /*Menu Selection Tag*/
#define SATK_EVENT_DOWNLOAD_TAG	0xD6  /*Event Download Tag*/

/*Tag Values (without Comprehension bit)*/
#define SATK_COMMAND_DETAILS_TAG	0x01 /* COMMAND DETAILS TAG */
#define SATK_DEVICE_IDENTITY_TAG	0x02 /* DEVICE IDENTITY TAG */
#define SATK_RESULT_TAG	0x03 /* RESULT TAG */
#define SATK_DURATION_TAG	0x04 /* DURATION TAG */
#define SATK_ALPHA_IDENTIFIER_TAG	0x05 /* ALPHA IDENTIFIER TAG */
#define SATK_ADDRESS_TAG	0x06 /* ADDRESS TAG */
#define SATK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG	0x07 /* CAPABILITY CONFIGURATION PARAMETERS TAG */
#define SATK_SUB_ADDRESS_TAG	0x08 /* SUB ADDRESS TAG */
#define SATK_SS_STRING_TAG	0x09 /* SS STRING TAG */
#define SATK_USSD_STRING_TAG	0x0A /* USSD STRING TAG */
#define SATK_SMS_TPDU_TAG	0x0B /* SMS TPDU TAG */
#define SATK_CELL_BROADCAST_PAGE_TAG	0x0C /* CELL BROADCAST PAGE TAG */
#define SATK_TEXT_STRING_TAG	0x0D /* TEXT STRING TAG */
#define SATK_TONE_TAG	0x0E /* TONE TAG */
#define SATK_ITEM_TAG	0x0F /* ITEM TAG */
#define SATK_ITEM_IDENTIFIER_TAG	0x10 /* ITEM IDENTIFIER TAG */
#define SATK_RESPONSE_LENGTH_TAG	0x11 /* RESPONSE LENGTH TAG */
#define SATK_FILE_LIST_TAG	0x12 /* FILE LIST TAG */
#define SATK_LOCATION_INFORMATION_TAG	0x13 /* LOCATION INFORMATION TAG */
#define SATK_IMEI_TAG	0x14 /* IMEI TAG */
#define SATK_HELP_REQUEST_TAG	0x15 /* HELP REQUEST TAG */
#define SATK_NETWORK_MEASUREMENT_RESULTS_TAG	0x16 /* NETWORK MEASUREMENT RESULTS TAG */
#define SATK_DEFAULT_TEXT_TAG	0x17 /* DEFAULT TEXT TAG */
#define SATK_ITEMS_NEXT_ACTION_INDICATOR_TAG	0x18 /* ITEMS NEXT ACTION INDICATOR TAG */
#define SATK_EVENT_LIST_TAG	0x19 /* EVENT LIST TAG */
#define SATK_CAUSE_TAG	0x1A /* CAUSE TAG */
#define SATK_LOCATION_STATUS_TAG	0x1B /* LOCATION STATUS TAG */
#define SATK_BCCH_CHANNEL_LIST_TAG	0x1D /* BCCH CHANNEL LIST TAG */
#define SATK_ICON_IDENTIFIER_TAG	0x1E /* ICON IDENTIFIER TAG */
#define SATK_ITEM_ICON_IDENTIFIER_LIST_TAG	0x1F /* ITEM ICON IDENTIFIER LIST TAG */
#define SATK_DATE_TIME_AND_TIME_ZONE_TAG	0x26 /* DATE TIME AND TIME ZONE TAG */
#define SATK_CALL_CONTROL_REQUESTED_ACTION_TAG	0x27 /* CALL CONTROL REQUESTED ACTION TAG */
#define SATK_AT_COMMAND_TAG	0x28 /* AT COMMAND TAG */
#define SATK_AT_RESPONSE_TAG	0x29 /* AT RESPONSE TAG */
#define SATK_BC_REPEAT_INDICATOR_TAG	0x2A /* BC REPEAT INDICATOR TAG */
#define SATK_IMMEDIATE_RESPONSE_TAG	0x2B /* IMMEDIATE RESPONSE TAG */
#define SATK_DTMF_STRING_TAG	0x2C /* DTMF STRING TAG */
#define SATK_LANGUAGE_TAG	0x2D /* LANGUAGE TAG */
#define SATK_BROWSER_IDENTITY_TAG	0x30 /* BROWSER IDENTITY TAG */
#define SATK_URL_TAG	0x31 /* URL TAG */
#define SATK_BEARER_TAG	0x32 /* BEARER TAG */
#define SATK_PROVISIONING_REF_FILE_TAG	0x33 /* PROVISIONING REFERENCE FILE TAG */
#define SATK_BROWSER_TERMINATION_CAUSE_TAG	0x34 /* BROWSER TERMINATION CAUSE TAG */
#define SATK_BEARER_DISCRIPTION_TAG	0x35 /* BEARER DISCRIPTION TAG */
#define SATK_CHANNEL_DATA_TAG	0x36 /* CHANNEL DATA TAG */
#define SATK_CHANNEL_DATA_LEN_TAG	0x37 /* CHANNEL DATA LEN TAG */
#define SATK_CHANNEL_STATUS_TAG	0x38 /* CHANNEL STATUS TAG */
#define SATK_BUFFER_SIZE_TAG	0x39 /* BUFFER SIZE TAG */
#define SATK_CARD_READER_IDENTIFIER_TAG	0x3A /* CARD READER IDENTIFIER TAG */
#define SATK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG	0x3C /* USIM ME INTERFACE TRANSPORT LEVEL  */
#define SATK_OTHER_ADDRESS_TAG	0x3E /* OTHER ADDRESS        */
#define SATK_CDMA_SMS_TPDU_TAG	0x48 /* CDMA SMS TPDU TAG */
#define SATK_NETWORK_ACCESS_TAG	0x48 /* NETWORK ACCESS NAME TAG */
#define SATK_REMOTE_ENTITY_ADDRESS_TAG	0x48 /* REMOTE ENTITY ADDRESS TAG */

/* general data object lengths */
#define SATK_DCS_LENGTH 0x01
#define SATK_COMMAND_DETAILS_LENGTH	0x03  /*  COMMAND DETAILS LENGTH */
#define SATK_DEVICE_IDENTITY_LENGTH	0x02  /*  DEVICE IDENTITY LENGTH */
#define SATK_ICON_IDENTITY_LENGTH	0x02  /*  ICON IDENTITY LENGTH */
#define SATK_DURATION_LENGTH	0x02  /*  DURATION LENGTH */
#define SATK_ITEM_IDENTIFIER_LENGTH	0x01  /*  ITEM IDENTIFIER LENGTH */
#define SATK_LOCATION_INFORMATION_LENGTH	0x07  /*  LOCATION INFORMATION LENGTH */
#define SATK_HELP_REQUEST_LENGTH	0x00  /*  HELP REQUEST LENGTH */
#define SATK_LOCATION_STATUS_LENGTH	0x01  /*  LOCATION STATUS LENGTH */
#define SATK_DATE_TIME_AND_TIME_ZONE_LENGTH	0x07  /*  DATE TIME AND TIME ZONE LENGTH */
#define SATK_LANGUAGE_LENGTH	0x02  /*  LANGUAGE LENGTH */
#define SATK_BC_REPEAT_IND_LENGTH	0x01  /*  BC REPEAT INDICATION LENGTH */
#define SATK_RESPONSE_LENGTH_LENGTH	0x02  /*  RESPONSE LENGTH LENGTH */
#define SATK_TONE_LENGTH	0x01  /*  TONE LENGTH */
#define SATK_BROWSER_ID_LENGTH	0x01  /*  BROWSER ID LENGTH */
#define SATK_BROWSER_TERMINATION_CAUSE_LENGTH	0x01  /*  BROWSER TERMINATION CAUSE LENGTH */
#define SATK_BUFFER_SIZE_LENGTH	0x02 /*  BUFFER SIZE LENGTH */
#define SATK_UICC_ME_TRANS_INTERFACE_LEVEL_LENGTH	0x03 /*  UICC TERMINAL TRANSPORT INTERFACE LEVEL LENGTH */
#define SATK_CHANNEL_DATA_LENGTH_VALUE_LENGTH	0x01 /*  CHANNEL DATA LENGTH VALUE LENGTH */
#define SATK_CHANNEL_STATUS_LENGTH	0x01 /*  CHANNEL STATUS LENGTH */

struct private_object_data {
	struct tcore_sat_operations *ops;
};

static TReturn _dispatcher(CoreObject *o, UserRequest *ur)
{
	enum tcore_request_command command;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_SAT, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po || !po->ops)
		return TCORE_RETURN_ENOSYS;

	command = tcore_user_request_get_command(ur);
	switch (command) {
		case TREQ_SAT_REQ_ENVELOPE:
			if (!po->ops->envelope)
				return TCORE_RETURN_ENOSYS;

			return po->ops->envelope(o, ur);
			break;

		case TREQ_SAT_REQ_TERMINALRESPONSE:
			if (!po->ops->terminal_response)
				return TCORE_RETURN_ENOSYS;

			return po->ops->terminal_response(o, ur);
			break;

		default:
			break;
	}

	return TCORE_RETURN_SUCCESS;
}

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SAT);

	po = tcore_object_ref_object(o);
	if (po) {
		free(po);
		tcore_object_link_object(o, NULL);
	}
}

static int _get_length_filed_size(unsigned char firstLenByte)
{
	if (firstLenByte <= 0x7F) return 1;
	else if (firstLenByte == 0x81) return 2;
	else return 0; //return zero, as the length field can only be 1 or 2.
}

static void _get_string_data(unsigned char* src, int len,
		struct tel_sat_text_string_object *text_obj)
{
	if (!src || !text_obj) return;

	switch (text_obj->dcs.a_format) {
		case ALPHABET_FROMAT_SMS_DEFAULT: {
			char* unpacked_str;

			text_obj->string_length = 0;
			unpacked_str = tcore_util_unpack_gsm7bit((const char*)src, (unsigned int)len);
			if (!unpacked_str) return;

			text_obj->dcs.a_format = ALPHABET_FROMAT_8BIT_DATA;
			text_obj->string_length = strlen(unpacked_str);

			if (text_obj->string_length >= SAT_TEXT_STRING_LEN_MAX) {
				text_obj->string_length = SAT_TEXT_STRING_LEN_MAX;
				memcpy(text_obj->string, unpacked_str, SAT_TEXT_STRING_LEN_MAX);
				text_obj->string[SAT_TEXT_STRING_LEN_MAX] = 0x00;
			}
			else {
				memcpy(text_obj->string, unpacked_str, text_obj->string_length);
				text_obj->string[text_obj->string_length] = 0x00;
			}

			g_free(unpacked_str);
		}break;
		case ALPHABET_FROMAT_UCS2:
		case ALPHABET_FROMAT_8BIT_DATA: {

			if (text_obj->string_length >= SAT_TEXT_STRING_LEN_MAX) {
				text_obj->string_length = SAT_TEXT_STRING_LEN_MAX;
				memcpy(text_obj->string, src, SAT_TEXT_STRING_LEN_MAX);
				text_obj->string[SAT_TEXT_STRING_LEN_MAX] = 0x00;
			}
			else {
				memcpy(text_obj->string, src, text_obj->string_length);
				text_obj->string[text_obj->string_length] = 0x00;
			}
		}break;
		default: {
			dbg("[SAT] SAT PARSER -  Unknown alphabet format(%d)", text_obj->dcs.a_format);
		}break;
	}

	return;
}

static void _sat_decode_dcs(unsigned char dcs, struct data_coding_scheme* dsc_obj)
{
	dbg("[SAT] SAT PARSER - dcs=[0x%x]", dcs);
	dsc_obj->raw_dcs = dcs;
	//bit 5 of DCS byte will tell us if the text is compressed or not.

	if (dcs & 0x20)
		dsc_obj->is_compressed_format = TRUE;
	else
		dsc_obj->is_compressed_format = FALSE;

	//bit 4 when set, indicates that bits 0 & 1 have message class meaning.
	dsc_obj->m_class = MSG_CLASS_NONE;
	if (dcs & 0x10) {
		switch (dcs & 0x03) {
			case 0x00:
				dsc_obj->m_class = MSG_CLASS_0;
				break;
			case 0x01:
				dsc_obj->m_class = MSG_CLASS_1;
				break;
			case 0x02:
				dsc_obj->m_class = MSG_CLASS_2;
				break;
			case 0x03:
				dsc_obj->m_class = MSG_CLASS_3;
				break;
			default:
				dsc_obj->m_class = MSG_CLASS_RESERVED;
				break;
		}
	}

	/*bits 2 & 3 indicate the character set being used*/
	switch (dcs & 0x0C) {
		case 0x00:
		case 0x0C:
			dsc_obj->a_format = ALPHABET_FROMAT_SMS_DEFAULT;
			break;

		case 0x04:
			dsc_obj->a_format = ALPHABET_FROMAT_8BIT_DATA;
			break;

		case 0X08:
			dsc_obj->a_format = ALPHABET_FROMAT_UCS2;
			break;

		default:
			dsc_obj->a_format = ALPHABET_FROMAT_RESERVED;
			break;
	}

	dbg("[SAT] SAT PARSER - is_compressed_format(%d), msgClass(0x%x), alpha_format(0x%x)",
			dsc_obj->is_compressed_format, dsc_obj->m_class, dsc_obj->a_format);

	return;
}

/*
 * Decode TLV data object
 */
static enum tcore_sat_result _sat_decode_alpha_identifier_tlv(unsigned char* tlv_str, int tlv_len,
		int curr_offset, struct tel_sat_alpha_identifier* alpha_id_obj, int* consumed_data_len)
{
	unsigned char* src_data;
	int index, len_of_len = 0;
	int alpha_len = 0;

	if (tlv_str == NULL || consumed_data_len == NULL || alpha_id_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || alpha_id_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	//tag
	index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[index++] & 0x7F) != SATK_ALPHA_IDENTIFIER_TAG) {
		dbg("[SAT] SAT PARSER -  alphaID TAG missing");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	//length
	alpha_id_obj->is_exist = TRUE;
	len_of_len = _get_length_filed_size(src_data[index]);
	if (!len_of_len) {
		dbg("[SAT] SAT PARSER -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}
	alpha_len = alpha_id_obj->alpha_data_len = src_data[index + len_of_len - 1];

	//alpha identifier
	index += len_of_len;
	if ((index + alpha_len) > tlv_len) {
		dbg("[SAT] SAT PARSER  -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (alpha_id_obj->alpha_data_len > 0) {

		unsigned char dcs;

		if (alpha_id_obj->alpha_data_len >= SAT_ALPHA_ID_LEN_MAX)
			alpha_id_obj->alpha_data_len = SAT_ALPHA_ID_LEN_MAX - 1;

		memcpy(alpha_id_obj->alpha_data, &src_data[index], alpha_id_obj->alpha_data_len);
		alpha_id_obj->alpha_data[alpha_id_obj->alpha_data_len] = 0x00;

		if (src_data[index] == 0x80 || src_data[index] == 0x81 || src_data[index] == 0x82)
			dcs = 0X08;
		else
			dcs = 0x04;

		_sat_decode_dcs(dcs, &alpha_id_obj->dcs);
	}

	*consumed_data_len = 1 + len_of_len + alpha_len;
	dbg("[SAT] SAT PARSER alphaId= %s", alpha_id_obj->alpha_data);
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_device_identities_tlv(unsigned char* tlv_str,
		struct tel_sat_device_identities* dev_id_obj)
{
	int index = 0, i;

	if (tlv_str == NULL || dev_id_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str ==NULL || dev_id_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if ((tlv_str[index++] & 0x7F) != SATK_DEVICE_IDENTITY_TAG) {
		dbg("[SAT] SAT PARSER -  device identity tag missing.");
		return TCORE_SAT_REQUIRED_VALUE_MISSING; //send TR
	}

	if (tlv_str[index++] != SATK_DEVICE_IDENTITY_LENGTH) {
		dbg("[SAT] SAT PARSER -  device identity length incorrect.");
		return TCORE_SAT_REQUIRED_VALUE_MISSING; //send TR
	}

	for (i = 0; i < 2; i++) {
		switch (tlv_str[index]) {
			case DEVICE_ID_KEYPAD:
			case DEVICE_ID_DISPLAY:
			case DEVICE_ID_EARPIECE:
			case DEVICE_ID_SIM:
			case DEVICE_ID_ME:
			case DEVICE_ID_NETWORK:
				if (i == 0) dev_id_obj->src = tlv_str[index];
				if (i == 1) dev_id_obj->dest = tlv_str[index];
				break;

			default:
				return TCORE_SAT_REQUIRED_VALUE_MISSING;
				//send TR
				break;
		}
		index++;
	}

	dbg("[SAT] SAT PARSER -  source=%d, dest=%d", dev_id_obj->src, dev_id_obj->dest);
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_duration_tlv(unsigned char* tlv_str, int tlv_len,
		int curr_offset, struct tel_sat_duration *duration_obj, int* consumed_data_len)
{
	int index = 0;
	unsigned char* src_data = NULL;

	if (!tlv_str || !duration_obj || !consumed_data_len) {
		err("[SAT] SAT PARSER data is null");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len < curr_offset + 3) {
		err("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[index++] & 0x7F) != SATK_DURATION_TAG) {
		err("[SAT] SAT PARSER -  duration tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //send TR
	}

	if (src_data[index++] != SATK_DURATION_LENGTH) {
		err("[SAT] SAT PARSER -  incorrect length value.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //send TR
	}

	//time unit
	switch (src_data[index]) {
		case TIME_UNIT_MINUTES:
		case TIME_UNIT_SECONDS:
		case TIME_UNIT_TENTHS_OF_SECONDS:
			duration_obj->time_unit = src_data[index];
			break;
		default:
			duration_obj->time_unit = TIME_UNIT_RESERVED;
			break;
	}

	//interval
	index++;
	duration_obj->time_interval = src_data[index];
	*consumed_data_len = 4;

	dbg("[SAT] SAT PARSER -  timeUnit=%d, interval=%d", duration_obj->time_unit, duration_obj->time_interval);

	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_item_tlv(unsigned char* tlv_str, int tlv_len,
		int curr_offset, struct tel_sat_item_info* item_obj, int* consumed_data_len)
{
	int index, len_of_len=0, i=0;
	int item_len =0;
	unsigned char* src_data = NULL;

	if(tlv_str == NULL || consumed_data_len == NULL || item_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || item_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if(tlv_len <= (curr_offset+1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	//tag
	index = curr_offset;
	src_data = &tlv_str[0];
	if((src_data[index++]&0x7F) != SATK_ITEM_TAG){
		dbg("[SAT] SAT PARSER -  tag not found.=%d",src_data[index-1]);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	//length
	len_of_len = _get_length_filed_size(src_data[index]);
	if(!len_of_len){
		dbg("[SAT] SAT PARSER -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}
	item_len = src_data[index+len_of_len-1];

	index+=len_of_len; //index pointing to item.
	if((index+item_len) > tlv_len){
		dbg("[SAT] SAT PARSER -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	memset(item_obj->text, 0x00,(SAT_ITEM_TEXT_LEN_MAX+1));
	if(item_len <= 0){
		dbg("[SAT] SAT PARSER -  menu_text is NULL, remove the menu");
		*consumed_data_len = 1+len_of_len+item_len;
		return TCORE_SAT_SUCCESS;
	}

	//item
	item_obj->item_id = src_data[index++];
	// fix for orange SIM issue
	// H0100078487 Strange Character display on SIM SAT
	// The string length was less than its real length
	// So garbage characters was displayed. To fix it, we would recalculate the real length.
	for(i=0; i<item_len-1;i++){
		dbg("[SAT] %d: %c", i, src_data[index+i]);
		if(src_data[index+i] == 0xFF) break;
	}
	item_obj->text_len = i;

	if(item_obj->text_len <= 0){
		dbg("[SAT] SAT PARSER -  text len = 0");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if(item_obj->text_len > SAT_ITEM_TEXT_LEN_MAX)
		item_obj->text_len = SAT_ITEM_TEXT_LEN_MAX;

	memcpy(item_obj->text, &src_data[index], item_obj->text_len);

	dbg("[SAT] SAT PARSER -  item_text=%s", item_obj->text);
	*consumed_data_len = 1+len_of_len+item_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_response_length_tlv(unsigned char* tlv_str, int tlv_len,
		int curr_offset, struct tel_sat_response_length *response_obj, int* consumed_data_len)
{
	int index = 0;
	unsigned char* src_data = NULL;

	if (!tlv_str || !response_obj || !consumed_data_len) {
		err("[SAT] SAT PARSER data is null");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= curr_offset + 1) {
		err("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[index++] & 0x7F) != SATK_RESPONSE_LENGTH_TAG) {
		err("[SAT] SAT PARSER -  response length tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //send TR
	}

	if (src_data[index++] != SATK_RESPONSE_LENGTH_LENGTH) {
		err("[SAT] SAT PARSER -  incorrect length value.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //send TR
	}

	response_obj->min = src_data[index++];
	response_obj->max = src_data[index++];
	dbg("[SAT] SAT PARSER  min length(%d), max length(%d)", response_obj->min, response_obj->max);

	*consumed_data_len = 4;
	if(response_obj->min > response_obj->max){
		err("[SAT] SAT PARSER - : min length is larger than max length");
		return TCORE_SAT_BEYOND_ME_CAPABILITY;
	}

	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_item_identifier_tlv(unsigned char* tlv_str, int tlv_len,
		int curr_offset, struct tel_sat_item_identifier *item_identifier_obj, int* consumed_data_len)
{
	int index = 0;
	unsigned char* src_data = NULL;

	if (!tlv_str || !item_identifier_obj || !consumed_data_len) {
		err("[SAT] SAT PARSER data is null");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= curr_offset + 1) {
		err("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[index++] & 0x7F) != SATK_ITEM_IDENTIFIER_TAG) {
		err("[SAT] SAT PARSER -  Item identifier tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //send TR
	}

	if (src_data[index++] != SATK_ITEM_IDENTIFIER_LENGTH) {
		err("[SAT] SAT PARSER -  incorrect length value.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //send TR
	}

	item_identifier_obj->item_identifier = src_data[index++];
	*consumed_data_len = 3;
	dbg("[SAT] SAT PARSER item identifier(0x%02x)", item_identifier_obj->item_identifier);

	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_text_tlv(unsigned char* tlv_str, int tlv_len,
		int curr_offset, struct tel_sat_text_string_object *text_obj, int* consumed_data_len)
{
	int index, len_of_len=0;
	int text_len =0;
	unsigned char* src_data;
	gboolean comprehension_req = FALSE;

	if (!tlv_str || !consumed_data_len ) {
		err("[SAT] parser: data is null");
		return TCORE_SAT_ERROR_FATAL;
	}

	if(tlv_len <= (curr_offset+1)){
		err("[SAT] parser: incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	//tag
	index = curr_offset;
	src_data = &tlv_str[0];
	if((src_data[index]&0x7F) != SATK_TEXT_STRING_TAG){
		err("[SAT] parser: text string tag missing, tag=0x%x",src_data[index]);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if(src_data[index++]&0x80)
		comprehension_req = TRUE;

	//length
	len_of_len = _get_length_filed_size(src_data[index]);
	if(!len_of_len){
		err("[SAT] parser: invalid length.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	text_len = src_data[index+len_of_len-1];
	dbg("[SAT] parser: text_tlv_len=%d",text_len);

	index += len_of_len;
	memset(text_obj->string, 0x00, SAT_TEXT_STRING_LEN_MAX);

	//text
	if(text_len <=1){
		text_obj->string_length = 0;
	}else{
		text_obj->string_length = text_len-1;
		_sat_decode_dcs(src_data[index++], &text_obj->dcs);
		_get_string_data(&src_data[index], text_obj->string_length, text_obj);
	}

	 // 1 is the length of Tag.
	*consumed_data_len = 1 + len_of_len + text_len;

	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_item_next_action_indicator_tlv(unsigned char* tlv_str,
		int tlv_len, int curr_offset,
		struct tel_sat_item_next_action_indicatior_list* item_next_act_indi_obj,
		int* consumed_data_len)
{
	int index;
	int item_nai_len;
	unsigned char* src_data;
	gboolean comprehension_req = FALSE;

	if(tlv_str == NULL || consumed_data_len == NULL || item_next_act_indi_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || item_next_act_indi_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if(tlv_len <= (curr_offset+1)) {
		dbg("[SAT] SAT PARSER - incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	//tag
	index = curr_offset;
	src_data = &tlv_str[0];
	if((src_data[index]&0x7F) != SATK_ITEMS_NEXT_ACTION_INDICATOR_TAG){
		dbg("[SAT] SAT PARSER - tag not found.=%d",src_data[index]);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if((src_data[index++]&0x7F))
		comprehension_req = TRUE;

	//item cnt
	item_nai_len = item_next_act_indi_obj->cnt = src_data[index++];
	if((index+item_nai_len) > tlv_len) {
		dbg("[SAT] SAT PARSER - incorrect cmd len, expected len = %d, orig_len=%d", (index+item_nai_len),tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if(item_next_act_indi_obj->cnt > SAT_ITEMS_NEXT_ACTION_INDI_LIST_MAX_COUNT) {
		if(comprehension_req == TRUE) {
			dbg("[SAT] SAT PARSER - list count exceeds maximum allowed count=%d",item_next_act_indi_obj->cnt);
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
		}
		item_next_act_indi_obj->cnt = 0;
	}

	memset(item_next_act_indi_obj->indicator_list, 0xFF, SAT_ITEMS_NEXT_ACTION_INDI_LIST_MAX_COUNT);
	if(item_next_act_indi_obj->cnt > 0)
		memcpy(item_next_act_indi_obj->indicator_list, &src_data[index], item_next_act_indi_obj->cnt);

	*consumed_data_len = 1+1+item_nai_len;
	dbg("[SAT] SAT PARSER - listCount=%d, consumed_data_len = %d",item_next_act_indi_obj->cnt, *consumed_data_len);
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_icon_identifier_tlv(unsigned char* tlv_str, int tlv_len,
		int curr_offset, struct tel_sat_icon_identifier* icon_id_obj, int* consumed_data_len)
{
	unsigned char* src_data;
	int index = 0;

	if(tlv_str == NULL || icon_id_obj == NULL ||consumed_data_len == NULL)	{
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || icon_id_obj == NULL ||consumed_data_len == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if(tlv_len <= (curr_offset+1)) {//length of icon id tlv is 4
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	//tag
	index = curr_offset;
	src_data = &tlv_str[0];
	if((src_data[index++]&0x7F) != SATK_ICON_IDENTIFIER_TAG) {
		dbg("[SAT] SAT PARSER -  icon identity tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //send TR
	}

	if(src_data[index++] != SATK_ICON_IDENTITY_LENGTH) {
		dbg("[SAT] SAT PARSER -  incorrect length value.");
		return FALSE; //send TR
	}

	if((index+SATK_ICON_IDENTITY_LENGTH) > tlv_len) {
		dbg("[SAT] SAT PARSER -  incorrect cmd len, expected len = %d, orig_len=%d", (index+SATK_ICON_IDENTITY_LENGTH),tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}
	icon_id_obj->is_exist = TRUE;

	if((src_data[index++]&0x01))
		icon_id_obj->icon_qualifer = ICON_QUALI_NOT_SELF_EXPLANATORY;
	else
		icon_id_obj->icon_qualifer = ICON_QUALI_SELF_EXPLANATORY;

	if(src_data[index] > 0x00) {
		icon_id_obj->icon_identifier = src_data[index];
	}
	else {
		dbg("[SAT] SAT PARSER -  incorrect icon identifier");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //send TR
	}

	dbg("[SAT] SAT PARSER -  icon_qual=%d, iconId=%d",icon_id_obj->icon_qualifer, icon_id_obj->icon_identifier);
	*consumed_data_len = 4;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_icon_identifier_list_tlv(unsigned char* tlv_str,
		int tlv_len, int curr_offset, struct tel_sat_icon_identifier_list* icon_list_obj,
		int* consumed_data_len)
{
	int index, i;
	int len_value =0;
	unsigned char* src_data;
	gboolean comprehension_req = FALSE;

	if(tlv_str == NULL || consumed_data_len == NULL || icon_list_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || icon_list_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if(tlv_len <= (curr_offset+1)+1) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	//tag
	index = curr_offset;
	src_data = &tlv_str[0];
	if((src_data[index]&0x7F) != SATK_ITEM_ICON_IDENTIFIER_LIST_TAG) {
		dbg("[SAT] SAT PARSER -  icon identity tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //send TR
	}

	icon_list_obj->is_exist = TRUE;
	if(src_data[index++]&0x80)
		comprehension_req = TRUE;

	len_value = src_data[index++];
	if(src_data[index++]&0x01)
		icon_list_obj->icon_qualifer = ICON_QUALI_NOT_SELF_EXPLANATORY;
	else
		icon_list_obj->icon_qualifer = ICON_QUALI_SELF_EXPLANATORY;

	icon_list_obj->icon_cnt = len_value-1;
	if(icon_list_obj->icon_cnt > SAT_ICON_LIST_MAX_COUNT){
		if(comprehension_req == TRUE) {
			dbg("[SAT] SAT PARSER -  list count exceeds maximum allowed count=%d",icon_list_obj->icon_cnt);
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
		}
		icon_list_obj->icon_cnt = 0;
	}
	else{

		for(i=0;i<icon_list_obj->icon_cnt;i++) {
			if(src_data[index] > 0x00) {
				icon_list_obj->icon_id_list[i]= src_data[index++];
			} else	{
				dbg("[SAT] SAT PARSER -  incorrect icon identifier");
				return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //send TR
			}
		}
	}



	*consumed_data_len = 1+1+len_value;
	dbg("[SAT] SAT PARSER -  icon_qual=%d, iconCount=%d",icon_list_obj->icon_qualifer, icon_list_obj->icon_cnt);
	return TCORE_SAT_SUCCESS;
}

//decode proactive cmd
static enum tcore_sat_result _sat_decode_setup_event_list(unsigned char* o_cmd_data, int o_length,
		int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int index = 0;
	unsigned char dev_id[4];
	unsigned char* cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if(o_cmd_data == NULL){
		dbg("[SAT] SAT PARSER -  orig_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	index = curr_offset+2;
	sat_cmd_ind_data->data.setup_event_list.command_detail.cmd_num = cmd_data[index++];
	sat_cmd_ind_data->data.setup_event_list.command_detail.cmd_type = cmd_data[index++];

	index++;
	memcpy(dev_id,&cmd_data[index],4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.setup_event_list.device_id);
	if(rv != TCORE_SAT_SUCCESS)
		return rv;

	index+=4;

	//TOdo Eventlist parsing

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_display_text(unsigned char* o_cmd_data, int o_length,
		int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int index = 0;
	int data_len_consumed=0;
	unsigned char dev_id[4];
	unsigned char* cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if(o_cmd_data == NULL){
		dbg("[SAT] SAT PARSER -  orig_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	index = curr_offset+2;
	sat_cmd_ind_data->data.display_text.command_detail.cmd_num = cmd_data[index++];
	sat_cmd_ind_data->data.display_text.command_detail.cmd_type = cmd_data[index++];

	/* ******decode command qualifier****** */
	if (cmd_data[index] & 0x01) {
		sat_cmd_ind_data->data.display_text.command_detail.cmd_qualifier.display_text.text_priority =
				TEXT_PRIORITY_HIGH;
		dbg("[SAT] SAT PARSER -  msg_prio=TAPI_SAT_MSG_PRIORITY_HIGH.");
	}
	else {
		sat_cmd_ind_data->data.display_text.command_detail.cmd_qualifier.display_text.text_priority =
				TEXT_PRIORITY_NORMAL;
		dbg("[SAT] SAT PARSER - : msg_prio=TAPI_SAT_MSG_PRIORITY_NORMAL.");
	}

	if (cmd_data[index] & 0x80) {
		sat_cmd_ind_data->data.display_text.command_detail.cmd_qualifier.display_text.text_clear_type =
				TEXT_WAIT_FOR_USER_TO_CLEAR_MSG;
		dbg("[SAT] SAT PARSER - : msgClear=TAPI_SAT_WAIT_FOR_USER_TO_CLEAR_MSG.");
	}
	else {
		sat_cmd_ind_data->data.display_text.command_detail.cmd_qualifier.display_text.text_clear_type =
				TEXT_AUTO_CLEAR_MSG_AFTER_A_DELAY;
		dbg("[SAT] SAT PARSER -  msgClear=TAPI_SAT_AUTO_CLEAR_MSG_AFTER_A_DELAY.");
	}

	index++;
	memcpy(dev_id,&cmd_data[index],4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.display_text.device_id);
	if(rv != TCORE_SAT_SUCCESS)
		return rv;

	index+=4;
	rv = _sat_decode_text_tlv(o_cmd_data, o_length, index, &sat_cmd_ind_data->data.display_text.text, &data_len_consumed);
	if(rv != TCORE_SAT_SUCCESS)
		return rv;

	if(sat_cmd_ind_data->data.display_text.text.string_length <= 0){
		err("[SAT] SAT PARSER - :string length is 0");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	dbg("[SAT] SAT PARSER  text(%s)",sat_cmd_ind_data->data.display_text.text.string);
	dbg("[SAT] SAT PARSER o_len(%d) index(%d) data_len_consumed(%d)",o_length , index, data_len_consumed);

	if(index+data_len_consumed > o_length){
		err("[SAT] SAT PARSER - Wrong String TLV");
		return TCORE_SAT_BEYOND_ME_CAPABILITY;
	}
	else if(index+data_len_consumed == o_length){
		dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
		return TCORE_SAT_SUCCESS;
	}

	//icon identifier
	index+=data_len_consumed;
	if((cmd_data[index]&0x7F) == SATK_ICON_IDENTIFIER_TAG){
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, index, &sat_cmd_ind_data->data.display_text.icon_id, &data_len_consumed);

		if(rv != TCORE_SAT_SUCCESS){
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //SEND TR
		}

		if(index+data_len_consumed >= o_length){
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		index+=data_len_consumed; //index pointing to the Tag of next TLV
	}

	//immediate response
	sat_cmd_ind_data->data.display_text.immediate_response_requested = FALSE;
	if((cmd_data[index]&0x7F) == SATK_IMMEDIATE_RESPONSE_TAG){
		dbg("[SAT] SAT PARSER - :immediate response required.");
		sat_cmd_ind_data->data.display_text.immediate_response_requested = TRUE;
		index+=2;
	}

	if(index >= o_length){
		dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
		return TCORE_SAT_SUCCESS;
	}

	//time duration
	if((cmd_data[index]&0x7F)==SATK_DURATION_TAG){
		rv =_sat_decode_duration_tlv(o_cmd_data, o_length, index, &sat_cmd_ind_data->data.display_text.duration, &data_len_consumed);
		if(rv != TCORE_SAT_SUCCESS){
			return rv; //SEND TR
		}
	}

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_get_inkey(unsigned char* o_cmd_data, int o_length,
		int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int index = 0;
	int data_len_consumed=0;
	unsigned char dev_id[4];
	unsigned char* cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if(o_cmd_data == NULL){
		dbg("[SAT] SAT PARSER -  orig_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	index = curr_offset+2;
	sat_cmd_ind_data->data.get_inkey.command_detail.cmd_num = cmd_data[index++];
	sat_cmd_ind_data->data.get_inkey.command_detail.cmd_type = cmd_data[index++];

	/* ******decode command qualifier****** */
	if (cmd_data[index] & 0x01) {
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.alphabet_set = TRUE;
		dbg("[SAT] SAT PARSER - Alphabet set");
	}

	if(cmd_data[index]&0x02){
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.alphabet_type = INPUT_ALPHABET_TYPE_UCS2;
		dbg("[SAT] SAT PARSER -  INPUT_ALPHABET_TYPE_UCS2");
	}
	else{
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.alphabet_type = INPUT_ALPHABET_TYPE_SMS_DEFAULT;
		dbg("[SAT] SAT PARSER -  INPUT_ALPHABET_TYPE_SMS_DEFAULT");
	}

	if(cmd_data[index]&0x04){
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.inkey_type = INKEY_TYPE_YES_NO_REQUESTED;
		dbg("[SAT] SAT PARSER -  INKEY_TYPE_YES_NO_REQUESTED");
	}
	else{
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.inkey_type = INKEY_TYPE_CHARACTER_SET_ENABLED;
		dbg("[SAT] SAT PARSER -  INKEY_TYPE_YES_NO_REQUESTED");
	}

	if(cmd_data[index]&0x08){
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.immediate_rsp_required = TRUE;
		dbg("[SAT] SAT PARSER -  immediate response requested");
	}

	if (cmd_data[index] & 0x80) {
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.help_info = TRUE;
		dbg("[SAT] SAT PARSER - Help info");
	}

	//device identities
	index++;
	memcpy(dev_id,&cmd_data[index],4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.get_inkey.device_id);
	if(rv != TCORE_SAT_SUCCESS)
		return rv;

	//text
	index+=4;
	rv = _sat_decode_text_tlv(o_cmd_data, o_length, index, &sat_cmd_ind_data->data.get_inkey.text, &data_len_consumed);
	if(rv != TCORE_SAT_SUCCESS)
		return rv;

	if(sat_cmd_ind_data->data.get_inkey.text.string_length <= 0){
		err("[SAT] SAT PARSER - :string length is 0");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	dbg("[SAT] SAT PARSER  text(%s)",sat_cmd_ind_data->data.get_inkey.text.string);
	dbg("[SAT] SAT PARSER o_len(%d) index(%d) data_len_consumed(%d)",o_length , index, data_len_consumed);

	if(index+data_len_consumed > o_length){
		err("[SAT] SAT PARSER - Wrong String TLV");
		return TCORE_SAT_BEYOND_ME_CAPABILITY;
	}
	else if(index+data_len_consumed == o_length){
		dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
		return TCORE_SAT_SUCCESS;
	}

	//icon identifier
	index+=data_len_consumed;
	if((cmd_data[index]&0x7F) == SATK_ICON_IDENTIFIER_TAG){
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, index, &sat_cmd_ind_data->data.get_inkey.icon_id, &data_len_consumed);

		if(rv != TCORE_SAT_SUCCESS){
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //SEND TR
		}

		if(index+data_len_consumed >= o_length){
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		index+=data_len_consumed; //index pointing to the Tag of next TLV
	}

	//time duration
	if((cmd_data[index]&0x7F)==SATK_DURATION_TAG){
		rv =_sat_decode_duration_tlv(o_cmd_data, o_length, index, &sat_cmd_ind_data->data.get_inkey.duration, &data_len_consumed);
		if(rv != TCORE_SAT_SUCCESS){
			return rv; //SEND TR
		}
	}

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_get_input(unsigned char* o_cmd_data, int o_length,
		int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int index = 0;
	int data_len_consumed=0;
	unsigned char dev_id[4];
	unsigned char* cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if(o_cmd_data == NULL){
		dbg("[SAT] SAT PARSER -  orig_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	index = curr_offset+2;
	sat_cmd_ind_data->data.get_input.command_detail.cmd_num = cmd_data[index++];
	sat_cmd_ind_data->data.get_input.command_detail.cmd_type = cmd_data[index++];

	/* ******decode command qualifier****** */
	if (cmd_data[index] & 0x01) {
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_inkey.alphabet_set = TRUE;
		dbg("[SAT] SAT PARSER - Alphabet set");
	}

	if(cmd_data[index]&0x02){
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.alphabet_type = INPUT_ALPHABET_TYPE_UCS2;
		dbg("[SAT] SAT PARSER -  INPUT_ALPHABET_TYPE_UCS2");
	}
	else{
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.alphabet_type = INPUT_ALPHABET_TYPE_SMS_DEFAULT;
		dbg("[SAT] SAT PARSER -  INPUT_ALPHABET_TYPE_SMS_DEFAULT");
	}

	if(cmd_data[index]&0x04){
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.me_echo_user_input = TRUE;
		dbg("[SAT] SAT PARSER -  Me Echo user input");
	}

	if(cmd_data[index]&0x08){
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.user_input_unpacked_format = FALSE;
		dbg("[SAT] SAT PARSER - User input unpacked format - FALSE");
	}
	else{
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.user_input_unpacked_format = TRUE;
		dbg("[SAT] SAT PARSER - User input unpacked format - TRUE");
	}

	if (cmd_data[index] & 0x80) {
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.help_info = TRUE;
		dbg("[SAT] SAT PARSER - Help info");
	}

	//device identities
	index++;
	memcpy(dev_id,&cmd_data[index],4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.get_input.device_id);
	if(rv != TCORE_SAT_SUCCESS)
		return rv;

	//text
	index+=4;
	rv = _sat_decode_text_tlv(o_cmd_data, o_length, index, &sat_cmd_ind_data->data.get_input.text, &data_len_consumed);
	if(rv != TCORE_SAT_SUCCESS)
		return rv;

	if(sat_cmd_ind_data->data.get_input.text.string_length <= 0){
		err("[SAT] SAT PARSER - :string length is 0");
	}
	dbg("[SAT] SAT PARSER  text(%s)",sat_cmd_ind_data->data.get_input.text.string);

	//response length
	index+=data_len_consumed;
	rv = _sat_decode_response_length_tlv(o_cmd_data, o_length, index, &sat_cmd_ind_data->data.get_input.rsp_len, &data_len_consumed);
	if(rv != TCORE_SAT_SUCCESS)
		return rv;

	if(index+data_len_consumed >= o_length){
		err("[SAT] SAT PARSER - no more TLVs");
		return TCORE_SAT_SUCCESS;
	}

	//default text
	index+=data_len_consumed;
	if((o_cmd_data[index]&0x7F) == SATK_DEFAULT_TEXT_TAG){
		rv = _sat_decode_text_tlv(o_cmd_data, o_length, index, &sat_cmd_ind_data->data.get_input.default_text, &data_len_consumed);
		if(rv != TCORE_SAT_SUCCESS)
			return rv;

		if(sat_cmd_ind_data->data.get_input.default_text.string_length <= 0){
			err("[SAT] SAT PARSER - :string length is 0");
		}
		dbg("[SAT] SAT PARSER default text(%s)",sat_cmd_ind_data->data.get_input.default_text.string);
		index+=data_len_consumed;
	}

	//icon identifier
	if((cmd_data[index]&0x7F) == SATK_ICON_IDENTIFIER_TAG){
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, index, &sat_cmd_ind_data->data.display_text.icon_id, &data_len_consumed);

		if(rv != TCORE_SAT_SUCCESS){
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //SEND TR
		}

		if(index+data_len_consumed >= o_length){
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		index+=data_len_consumed; //index pointing to the Tag of next TLV
	}

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_select_item(unsigned char* o_cmd_data, int o_length,
		int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int index = 0;
	int data_len_consumed=0;
	unsigned char dev_id[4];
	unsigned char* cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if(o_cmd_data == NULL){
		dbg("[SAT] SAT PARSER -  orig_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	index = curr_offset+2;
	sat_cmd_ind_data->data.select_item.command_detail.cmd_num = cmd_data[index++];
	sat_cmd_ind_data->data.select_item.command_detail.cmd_type = cmd_data[index++];

	/* ******decode command qualifier****** */
	if (cmd_data[index] & 0x01) {
		if (cmd_data[index] & 0x02) {
			sat_cmd_ind_data->data.select_item.command_detail.cmd_qualifier.select_item.presentation_type = PRESENTATION_TYPE_NAVIGATION_OPTION;
			dbg("[SAT] SAT PARSER - PRESENTATION_TYPE_NAVIGATION_OPTION");
		}
		else{
			sat_cmd_ind_data->data.select_item.command_detail.cmd_qualifier.select_item.presentation_type = PRESENTATION_TYPE_DATA_VALUE;
			dbg("[SAT] SAT PARSER - PRESENTATION_TYPE_DATA_VALUE");
		}
	}
	else {
		sat_cmd_ind_data->data.select_item.command_detail.cmd_qualifier.select_item.presentation_type = PRESENTATION_TYPE_NOT_SPECIFIED;
		dbg("[SAT] SAT PARSER - PRESENTATION_TYPE_NOT_SPECIFIED");
	}

	if (cmd_data[index] & 0x04) {
		sat_cmd_ind_data->data.select_item.command_detail.cmd_qualifier.select_item.select_preference = SELECTION_PREFERENCE_USING_SOFT_KEY;
		dbg("[SAT] SAT PARSER - SELECTION_PREFERENCE_USING_SOFT_KEY");
	}
	else {
		sat_cmd_ind_data->data.select_item.command_detail.cmd_qualifier.select_item.select_preference = SELECTION_PREFERENCE_NONE_REQUESTED;
		dbg("[SAT] SAT PARSER - SELECTION_PREFERENCE_NONE_REQUESTED");
	}

	if (cmd_data[index] & 0x80) {
		sat_cmd_ind_data->data.select_item.command_detail.cmd_qualifier.select_item.help_info = TRUE;
		dbg("[SAT] SAT PARSER - Help info");
	}

	//device identities
	index++;
	memcpy(dev_id,&cmd_data[index],4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.select_item.device_id);
	if(rv != TCORE_SAT_SUCCESS)
		return rv;

	//alpha identifier
	index+=4;
	if((cmd_data[index]&0x7F) == SATK_ALPHA_IDENTIFIER_TAG){
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, index, &sat_cmd_ind_data->data.select_item.alpha_id, &data_len_consumed);
		if(rv != TCORE_SAT_SUCCESS) {
			return rv;
		}
		index+=data_len_consumed;
	}

	//item objects
	sat_cmd_ind_data->data.select_item.menu_item_cnt = 0;
	do{
		data_len_consumed=0;

		if((cmd_data[index]&0x7F) == SATK_ITEM_TAG) {
			rv = _sat_decode_item_tlv(o_cmd_data, o_length, index,
				&sat_cmd_ind_data->data.select_item.menu_item[sat_cmd_ind_data->data.select_item.menu_item_cnt],
				&data_len_consumed);

			if(rv != TCORE_SAT_SUCCESS)
				return rv;
		}
		else {
			if(sat_cmd_ind_data->data.select_item.menu_item_cnt == 0) {
				dbg("menu item is not exist.");
				return TCORE_SAT_REQUIRED_VALUE_MISSING;
			}
			//else
			break;	//???
		}
		sat_cmd_ind_data->data.select_item.menu_item_cnt++;
		index += data_len_consumed;

		if(index >= o_length)
			break;

	}while(sat_cmd_ind_data->data.select_item.menu_item_cnt < SAT_MENU_ITEM_COUNT_MAX);

	dbg("[SAT] SAT PARSER - select menu item_count=%d",sat_cmd_ind_data->data.select_item.menu_item_cnt);
	if(index >= o_length){
		dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
		return TCORE_SAT_SUCCESS;
	}

	//item next action indicator
	if((cmd_data[index]&0x7F) == SATK_ITEMS_NEXT_ACTION_INDICATOR_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_item_next_action_indicator_tlv(o_cmd_data, o_length, index,
				&sat_cmd_ind_data->data.select_item.item_next_act_ind_list, &data_len_consumed);
		if(rv!=TCORE_SAT_SUCCESS)
			return rv;

		if(index+data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		index+=data_len_consumed;
	}

	//item identifier
	if((cmd_data[index]&0x7F) == SATK_ITEM_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_item_identifier_tlv(o_cmd_data, o_length, index,
				&sat_cmd_ind_data->data.select_item.item_identifier, &data_len_consumed);
		if(rv !=TCORE_SAT_SUCCESS)
			return rv;

		if(index+data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		index+=data_len_consumed; //index pointing to the Tag of next TLV
	}

	//icon identifier
	if((cmd_data[index]&0x7F) == SATK_ICON_IDENTIFIER_TAG){
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, index,
				&sat_cmd_ind_data->data.select_item.icon_id, &data_len_consumed);

		if(rv != TCORE_SAT_SUCCESS){
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; //SEND TR
		}

		if(index+data_len_consumed >= o_length){
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		index+=data_len_consumed; //index pointing to the Tag of next TLV
	}

	if((cmd_data[index]&0x7F) == SATK_ITEM_ICON_IDENTIFIER_LIST_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_list_tlv(o_cmd_data, o_length, index,
				&sat_cmd_ind_data->data.select_item.icon_list, &data_len_consumed);
		if(rv !=TCORE_SAT_SUCCESS)
			return rv; //SEND TR

		if(index+data_len_consumed >= o_length){
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}
		index+=data_len_consumed; //index pointing to the Tag of next TLV
	}

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_setup_menu(unsigned char* tlv_str, int tlv_len,
		int curr_offset, struct tcore_sat_proactive_command *pactive_cmd_ind_obj)
{
	int index = 0;
	int data_len_consumed=0;
	unsigned char dev_id[4];
	unsigned char* src_data;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	//access command detail
	index = curr_offset+2; //move the index to command detail info +2(tag and length)
	src_data = &tlv_str[0];

// In this time, the point of index is COMMAND NUMBER
// [1] insert command detail information into each proactive command data structure.
	pactive_cmd_ind_obj->data.setup_menu.command_detail.cmd_num = src_data[index++];
	pactive_cmd_ind_obj->data.setup_menu.command_detail.cmd_type = src_data[index++];

// [2] decode COMMAND QUALIFIER
/*
-bit 1:	0 = no selection preference;
		1 = selection using soft key preferred.
-bits 2 to 7:	= RFU.
-bit 8:	0 = no help information available;
		1 = help information available.
*/

//[2-1] selection preference
	if (src_data[index] & 0x01) {
		pactive_cmd_ind_obj->data.setup_menu.command_detail.cmd_qualifier.setup_menu.select_preference =
				SELECTION_PREFERENCE_USING_SOFT_KEY;
		dbg("[SAT] SAT PARSER -  sel_pref=SAT_SELECTION_PREFERENCE_USING_SOFT_KEY.");
	}
	else {
		pactive_cmd_ind_obj->data.setup_menu.command_detail.cmd_qualifier.setup_menu.select_preference =
				SELECTION_PREFERENCE_NONE_REQUESTED;
		dbg("[SAT] SAT PARSER - : sel_pref=SAT_SELECTION_PREFERENCE_NONE_REQUESTED.");
	}

//[2-2] help available
	if (src_data[index] & 0x80) {
		pactive_cmd_ind_obj->data.setup_menu.command_detail.cmd_qualifier.setup_menu.help_info =
				TRUE;
		;dbg("[SAT] SAT PARSER - : is help Available=TRUE.");
	}
	else {
		pactive_cmd_ind_obj->data.setup_menu.command_detail.cmd_qualifier.setup_menu.help_info =
				FALSE;
		dbg("[SAT] SAT PARSER -  is help Available=FALSE.");
	}

// In this time, the point of index is DEVICE IDENTITIES.
//[3] decode DEVICE IDENTITIES TLV
	index++;
	memcpy(dev_id, &src_data[index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &pactive_cmd_ind_obj->data.setup_menu.device_id);
	if (rv != TCORE_SAT_SUCCESS) {
		//send TR in SatkProcessProactiveCmdInd()
		return rv;
	}


// In this time, the point of index is ALPHA IDENTIFIER. 11 or 12.
//[4] decode ALPHA IDENTIFIER TLV
	index+=4;
	dbg("[SAT] SAT PARSER - :index=%d",index);
	rv = _sat_decode_alpha_identifier_tlv(src_data, tlv_len, index,
			&pactive_cmd_ind_obj->data.setup_menu.alpha_id, &data_len_consumed);
	if(rv != TCORE_SAT_SUCCESS) {
		return rv;
	}

// In this time, the point of index is ITEM  TLV
//[5] decode ITEM LIST (at least one is mandatory)
	index+= data_len_consumed;
	pactive_cmd_ind_obj->data.setup_menu.menu_item_cnt = 0;
	do{
		data_len_consumed=0;
		if((src_data[index]&0x7F) == SATK_ITEM_TAG) {
			rv = _sat_decode_item_tlv(src_data, tlv_len, index,
						&pactive_cmd_ind_obj->data.setup_menu.menu_item[pactive_cmd_ind_obj->data.setup_menu.menu_item_cnt],
						&data_len_consumed);

			if(rv != TCORE_SAT_SUCCESS)
				return rv;
		}
		else {
			if(pactive_cmd_ind_obj->data.setup_menu.menu_item_cnt == 0) {
				dbg("menu item is not exist.");
				return TCORE_SAT_REQUIRED_VALUE_MISSING;
			}
			//else
			break;	//???
		}
		pactive_cmd_ind_obj->data.setup_menu.menu_item_cnt++;
		index += data_len_consumed;

		if(index >= tlv_len)
			break;
	}while(pactive_cmd_ind_obj->data.setup_menu.menu_item_cnt < SAT_MENU_ITEM_COUNT_MAX);

	dbg("[SAT] SAT PARSER - :setup menu item_count=%d",pactive_cmd_ind_obj->data.setup_menu.menu_item_cnt);
	if(index >= tlv_len){
		dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
		//send TR in SatkProcessProactiveCmdInd()
		return TCORE_SAT_SUCCESS;
	}

//[6] (optional TLV) decode ITEMS NEXT ACTION INDICATOR TLV
	if((src_data[index]&0x7F) == SATK_ITEMS_NEXT_ACTION_INDICATOR_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_item_next_action_indicator_tlv(tlv_str, tlv_len, index,
				&pactive_cmd_ind_obj->data.setup_menu.next_act_ind_list, &data_len_consumed);
		if(rv!=TCORE_SAT_SUCCESS) return rv;

		if(index+data_len_consumed >= tlv_len) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			//send the data to Noti manager.
			return TCORE_SAT_SUCCESS;
		}

		index+=data_len_consumed; //index pointing to the Tag of next TLV
	}
	else {
		dbg("[SAT] SAT PARSER -  ITEMS NEXT ACTION INDICATOR TLV Not present");
	}

//[7] (optional TLV) decode ICON IDENTIFIER TLV
	if((src_data[index]&0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(tlv_str, tlv_len, index,
				&pactive_cmd_ind_obj->data.setup_menu.icon_id, &data_len_consumed);
		if(rv !=TCORE_SAT_SUCCESS) return rv;

		if(index+data_len_consumed >= tlv_len) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			//send the data to Noti manager.
			return TCORE_SAT_SUCCESS;
		}

		index+=data_len_consumed; //index pointing to the Tag of next TLV
	}
	else{
		dbg("[SAT] SAT PARSER - ICON IDENTIFIER TLV Not present");
	}

//[8] (optional TLV) decode ICON IDENTIFIER LIST TLV
	if((src_data[index]&0x7F) == SATK_ITEM_ICON_IDENTIFIER_LIST_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_list_tlv(tlv_str, tlv_len, index,
				&pactive_cmd_ind_obj->data.setup_menu.icon_list, &data_len_consumed);
		if(rv !=TCORE_SAT_SUCCESS) return rv; //SEND TR

		if(index+data_len_consumed >= tlv_len){
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			//send the data to Noti manager.
			return TCORE_SAT_SUCCESS;
		}
		index+=data_len_consumed; //index pointing to the Tag of next TLV

	}
	else {
		dbg("[SAT] SAT PARSER - ICON IDENTIFIER LIST TLV not present");
	}

	//decoding done, send data to client
	dbg("[SAT] SAT PARSER - no more TLVs to decode.");
	return TCORE_SAT_SUCCESS;
//ToDo:  Text Attribute, Text Attribute list. refer ETSI 102.223.
}

int tcore_sat_decode_proactive_command(unsigned char* tlv_origin, unsigned int tlv_length,
		struct tcore_sat_proactive_command* decoded_tlv)
{
	unsigned int index = 0;
	int length_field_len = 0;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if(tlv_origin == NULL || tlv_length <=2) {
		dbg("[SAT] SAT PARSER - pointer pData passed is NULL or invalid length.");
		return TCORE_SAT_ERROR_FATAL;
	}

	//tag
	if(tlv_origin[index++]!= SATK_PROACTIVE_CMD_TAG) {
		dbg("[SAT] SAT PARSER - Did not find Proactive command tag.tag=%d", tlv_origin[index-1]);
		return TCORE_SAT_ERROR_FATAL;
	}

	//length
	length_field_len = _get_length_filed_size(tlv_origin[index]);
	if(length_field_len == 0) {
		dbg("[SAT] SAT PARSER - Invalid length.");
		return TCORE_SAT_ERROR_FATAL;
	}

	index+=length_field_len;

	//check command validation
	if(tlv_length < index+5+4)//command detail(5) and device identities(4)
		return TCORE_SAT_ERROR_FATAL;

	if( (tlv_origin[index] & 0x7F) != SATK_COMMAND_DETAILS_TAG){
		err("[SAT] no command detail info");
		return TCORE_SAT_ERROR_FATAL;
	}

	if( tlv_origin[index+1] != SATK_COMMAND_DETAILS_LENGTH){
		err("[SAT] invalid command detail length");
		return TCORE_SAT_ERROR_FATAL;
	}

	decoded_tlv->cmd_num= tlv_origin[index+2];
	decoded_tlv->cmd_type = tlv_origin[index+3];

	switch(decoded_tlv->cmd_type) {
		case SAT_PROATV_CMD_SETUP_EVENT_LIST:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SETUP_EVENT_LIST");
			//TOdo
			rv = _sat_decode_setup_event_list(tlv_origin, tlv_length, index, decoded_tlv);
			break;
		case SAT_PROATV_CMD_DISPLAY_TEXT:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_DISPLAY_TEXT");
			rv = _sat_decode_display_text(tlv_origin, tlv_length, index, decoded_tlv);
			break;
		case SAT_PROATV_CMD_GET_INKEY:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_GET_INKEY");
			rv = _sat_decode_get_inkey(tlv_origin, tlv_length, index, decoded_tlv);
			break;
		case SAT_PROATV_CMD_GET_INPUT:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_GET_INPUT");
			rv = _sat_decode_get_input(tlv_origin, tlv_length, index, decoded_tlv);
			break;
		case SAT_PROATV_CMD_SELECT_ITEM:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SELECT_ITEM");
			rv = _sat_decode_select_item(tlv_origin, tlv_length, index, decoded_tlv);
			break;
		case SAT_PROATV_CMD_SETUP_MENU:
			dbg("[SAT] SAT PARSER - SAT_PROATV_CMD_SETUP_MENU");
			rv = _sat_decode_setup_menu(tlv_origin, tlv_length, index, decoded_tlv);
			break;
#if 0
		case SAT_CMD_TYPE_MORE_TIME:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_MORE_TIME");
			rv = SatkProcessMoreTimeInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_REFRESH:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_REFRESH");
			rv = SatkProcessRefreshInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_SEND_SMS:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SEND_SMS");
			rv = SatkProcessSendSmsInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_SEND_SS:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SEND_SS");
			rv = SatkProcessSendSsInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_SEND_USSD:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SEND_USSD");
			rv = SatkProcessSendUssdInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_SETUP_CALL:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SETUP_CALL");
			rv = SatkProcessSetupCallInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_PLAY_TONE:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_PLAY_TONE");
			rv = SatkProcessPlayToneInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;
			//others to be added later

		case SAT_CMD_TYPE_SETUP_IDLE_MODE_TEXT:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SETUP_IDLE_MODE_TEXT");
			rv = SatkProcessSetupIdleModeTextInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_PROVIDE_LOCAL_INFO:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_PROVIDE_LOCAL_INFO");
			rv = SatkProcessProvideLocalInfoInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_LANGUAGE_NOTIFICATION:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_LANGUAGE_NOTIFICATION");
			rv = SatkProcessLanguageNotificationInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_SEND_DTMF:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SEND_DTMF");
			rv = SatkProcessSendDtmfInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_LAUNCH_BROWSER:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_LAUNCH_BROWSER");
			rv = SatkProcessLaunchBrowserInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;
#endif
	#ifdef SAT_BIP
		case SAT_CMD_TYPE_OPEN_CHANNEL:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_OPEN_CHANNEL");
			rv = SatkProcessOpenChannelInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_CLOSE_CHANNEL:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_CLOSE_CHANNEL");
			rv = SatkProcessCloseChannelInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_RECEIVE_DATA:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_RECEIVE_DATA");
			rv = SatkProcessReceiveDataInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_SEND_DATA:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SEND_DATA");
			rv = SatkProcessSendDataInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;

		case SAT_CMD_TYPE_GET_CHANNEL_STATUS:
			dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_GET_CHANNEL_STATUS");
			rv = SatkProcessGetChannelStatusInd(tlv_origin, tlv_length, index, decoded_tlv);
			break;
	#endif
		default:
			dbg("[SAT] SAT PARSER - ME cannot perform this command =0x[%02x]", decoded_tlv->cmd_type);
			//SEND TR with command not understood by ME, those command that are defined but not implemented by ME should be sent as beyond me's capability.
			rv = TCORE_SAT_BEYOND_ME_CAPABILITY;
			break;
	}

	//return value will be success if the proactive command has been successfuly decoded, then send it to clients.
	dbg("[SAT] SAT PARSER - each command parsing done.");
	return rv;
}

static unsigned char _sat_encode_dcs_tlv(const struct data_coding_scheme* src)
{
	unsigned char rv = 0x00;

	if(src == NULL)
		return 0;

	if(src->is_compressed_format)
		rv |= 0x20;

	//msg class
	switch(src->m_class){
		case MSG_CLASS_0:
		case MSG_CLASS_1:
		case MSG_CLASS_2:
		case MSG_CLASS_3:
			rv |= 0x10;
			rv |= src->m_class;
		break;

		case MSG_CLASS_RESERVED:
		case MSG_CLASS_NONE:
		default:
			rv &= 0xEF;
			break;
	}

	//alphabet format
	switch(src->a_format)
	{
		case ALPHABET_FROMAT_SMS_DEFAULT:
			rv &= 0xF3;
			break;

		case ALPHABET_FROMAT_8BIT_DATA:
			rv |= 0x04;
			break;

		case ALPHABET_FROMAT_UCS2:
			rv |= 0x08;
			break;

		default:
			rv |= 0x0C;
			break;
	}

	return rv;
}

static int _sat_encode_command_detail_tlv(const struct tel_sat_cmd_detail_info* src, char *dst, int current_index)
{
	dst[current_index++] = SATK_COMMAND_DETAILS_TAG;
	dst[current_index++] = SATK_COMMAND_DETAILS_LENGTH;
	dst[current_index++] = src->cmd_num;
	dst[current_index++] = src->cmd_type;

	switch(src->cmd_type){
		case SAT_PROATV_CMD_SETUP_EVENT_LIST:{
			dbg("setup evnet list : 1bit RFU")
		}break;
		case SAT_PROATV_CMD_DISPLAY_TEXT:{
			dst[current_index++] = 0x00;

			//command detail text priority
			if (src->cmd_qualifier.display_text.text_priority == TEXT_PRIORITY_HIGH)
				dst[current_index] += 0x01;

			//command detail text clear type
			if (src->cmd_qualifier.display_text.text_clear_type == TEXT_WAIT_FOR_USER_TO_CLEAR_MSG)
				dst[current_index] += 0x80;
		}break;
		case SAT_PROATV_CMD_GET_INKEY:{
			dst[current_index++] = 0x00;

			//command detail alphabet set
			if(src->cmd_qualifier.get_inkey.alphabet_set)
				dst[current_index] += 0x01;

			//command detail alphabet type
			if(src->cmd_qualifier.get_inkey.alphabet_type == INPUT_ALPHABET_TYPE_UCS2)
				dst[current_index] += 0x02;

			//command detail get inkey type
			if(src->cmd_qualifier.get_inkey.inkey_type == INKEY_TYPE_YES_NO_REQUESTED)
				dst[current_index] += 0x04;


			//command detail immediate response required
			if(src->cmd_qualifier.get_inkey.immediate_rsp_required)
				dst[current_index] += 0x08;

			//command detail help available
			if (src->cmd_qualifier.get_inkey.help_info)
				dst[current_index] += 0x80;
		}break;
		case SAT_PROATV_CMD_GET_INPUT:{
			dst[current_index++] = 0x00;

			//command detail alphabet set
			if(src->cmd_qualifier.get_input.alphabet_set)
				dst[current_index] += 0x01;

			//command detail alphabet type
			if(src->cmd_qualifier.get_input.alphabet_type == INPUT_ALPHABET_TYPE_UCS2)
				dst[current_index] += 0x02;

			//command detail echo user input
			if(src->cmd_qualifier.get_input.me_echo_user_input)
				dst[current_index] += 0x04;

			//command detail user input unpacked format
			if(src->cmd_qualifier.get_input.user_input_unpacked_format)
				dst[current_index] += 0x08;

			//command detail help available
			if (src->cmd_qualifier.get_input.help_info)
				dst[current_index] += 0x80;
		}break;
		case SAT_PROATV_CMD_SELECT_ITEM:{
			dst[current_index++] = 0x00;

			//command detail presentation
			dst[current_index] += src->cmd_qualifier.select_item.presentation_type;

			//command detail selection preference
			if(src->cmd_qualifier.select_item.select_preference == SELECTION_PREFERENCE_USING_SOFT_KEY)
				dst[current_index] += 0x04;

			//command detail help available
			if (src->cmd_qualifier.select_item.help_info)
				dst[current_index] += 0x80;
		}break;
		case SAT_PROATV_CMD_SETUP_MENU:{
			dst[current_index++] = 0x00;

			//command detail preferences
			if (src->cmd_qualifier.setup_menu.select_preference == SELECTION_PREFERENCE_USING_SOFT_KEY)
				dst[current_index] += 0x01;

			//command detail help available
			if (src->cmd_qualifier.setup_menu.help_info)
				dst[current_index] += 0x80;
		}break;
		default:
			err("no matched cmd type(%d)", src->cmd_type);
			break;
	}

	return 5;
}

static int _sat_encode_device_identities_tlv(const struct tel_sat_device_identities* src, char *dst, int current_index)
{
	dst[current_index++] =SATK_DEVICE_IDENTITY_TAG;
	dst[current_index++] =SATK_DEVICE_IDENTITY_LENGTH;
	dst[current_index++] = src->src;
	dst[current_index++] = src->dest;

	//device identities total len 4
	return 4;
}

static int _sat_encode_item_identifier_tlv(const struct tel_sat_item_identifier* src, char *dst, int current_index)
{
	dst[current_index++] =SATK_ITEM_IDENTIFIER_TAG;
	dst[current_index++] =SATK_ITEM_IDENTIFIER_LENGTH;
	dst[current_index++] = src->item_identifier;

	//item identifier total len 3
	return 3;
}

static int _sat_encode_duration_tlv(const struct tel_sat_duration* src, char *dst, int current_index)
{
	dst[current_index++] =SATK_DURATION_TAG;
	dst[current_index++] =SATK_DURATION_LENGTH;
	dst[current_index++] = src->time_unit;
	dst[current_index++] = src->time_interval;

	//duration total len 4
	return 4;
}

static int _sat_encode_text_tlv(const struct tel_sat_text_string_object* src, char *dst, int current_index, gboolean raw_dcs)
{
	int index = 0;

	if(src == NULL)
		return 0;

	//tag
	index++;
	dst[current_index++] =SATK_TEXT_STRING_TAG;
	if(src->string_length > 0x7F){
		index++;
		dst[current_index++] =0x81;
	}

	//length
	index++;
	dst[current_index++] =SATK_DCS_LENGTH+src->string_length;

	//dcs
	index++;
	if(raw_dcs)
		dst[current_index++] = src->dcs.raw_dcs;
	else
		dst[current_index++] =_sat_encode_dcs_tlv(&(src->dcs));

	//value
	if(src->string_length > 0){
		index += src->string_length;
		memcpy(&(dst[current_index]), src->string, src->string_length);
	}

	return index;
}

int tcore_sat_encode_envelop_cmd(const struct treq_sat_envelop_cmd_data *src_envelop, char *dst_envelop)
{
	int index = 0, encoded_len= 0;

	if(!dst_envelop)
		return 0;

	if(src_envelop->sub_cmd == ENVELOP_MENU_SELECTION){
		index = 2; //set the cursor to device identity
		dbg("item id(%d)", src_envelop->envelop_data.menu_select.item_identifier.item_identifier);
		encoded_len =_sat_encode_device_identities_tlv(&(src_envelop->envelop_data.menu_select.device_identitie), dst_envelop, index);
		index += encoded_len;

		//item identifier
		encoded_len = _sat_encode_item_identifier_tlv(&(src_envelop->envelop_data.menu_select.item_identifier), dst_envelop, index);
		index += encoded_len;

		if(src_envelop->envelop_data.menu_select.help_request){
			encoded_len = 2;//help request
			dst_envelop[index++] = SATK_HELP_REQUEST_TAG;
			dst_envelop[index++] = SATK_HELP_REQUEST_LENGTH;
		}

		dbg("menu selection cmd len(%d)", index);

		//main cmd
		dst_envelop[0] = SATK_MENU_SELECTION_TAG;
		dst_envelop[1] = index-2;
	}

	return index;
}

static int _sat_encode_setup_event_list(const struct tel_sat_tr_setup_event_list_tlv *src_tr, char *dst_tr)
{
	int index = 0, encoded_len = 0;

	//set command detail info
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, index);
	index += encoded_len;

	//set device identities info
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, index);
	index += encoded_len;

	//set result info
	dst_tr[index++] = SATK_RESULT_TAG;
	switch(src_tr->result_type){
		case RESULT_SUCCESS:
		case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
		case RESULT_SUCCESS_WITH_MISSING_INFO:
		case RESULT_BEYOND_ME_CAPABILITIES:
		case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
		case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
		case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
		case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
			dst_tr[index++] = 1;
			dst_tr[index++] = src_tr->result_type;
			break;
		case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
			dst_tr[index++] = 2;
			dst_tr[index] = src_tr->result_type;
			dst_tr[index++] = src_tr->me_problem_type;
			break;
		default:
			dbg("invalid response value[0x%x] in current TR",src_tr->result_type);
			break;
	}

	return index;
}

static int _sat_encode_display_text(const struct tel_sat_tr_display_text_tlv *src_tr, char *dst_tr)
{
	int index = 0, encoded_len = 0;

	//set command detail info
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, index);
	index += encoded_len;

	//set device identities info
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, index);
	index += encoded_len;

	//set result info
	dst_tr[index++] = SATK_RESULT_TAG;
	switch(src_tr->result_type){
		case RESULT_SUCCESS:
		case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
		case RESULT_SUCCESS_WITH_MISSING_INFO:
		case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
		case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
		case RESULT_BACKWARD_MOVE_BY_USER:
		case RESULT_NO_RESPONSE_FROM_USER:
		case RESULT_BEYOND_ME_CAPABILITIES:
		case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
		case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
		case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
		case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
			dst_tr[index++] = 1;
			dst_tr[index++] = src_tr->result_type;
			break;
		case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
		case RESULT_FRAMES_ERROR:
			dst_tr[index++] = 2;
			dst_tr[index] = src_tr->result_type;
			dst_tr[index++] = src_tr->me_problem_type;
			break;
		default:
			dbg("invalid response value[0x%x] in current TR",src_tr->result_type);
			break;
	}

	return index;
}

static int _sat_encode_get_inkey(const struct tel_sat_tr_get_inkey_tlv *src_tr, char *dst_tr)
{
	int index = 0, encoded_len = 0;

	//set command detail info
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, index);
	index += encoded_len;

	//set device identities info
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, index);
	index += encoded_len;

	//set result info
	dst_tr[index++] = SATK_RESULT_TAG;

	switch(src_tr->result_type){
		case RESULT_SUCCESS:
		case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
		case RESULT_SUCCESS_WITH_MISSING_INFO:
		case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
			dst_tr[index++] = 1;
			dst_tr[index++] = src_tr->result_type;

			encoded_len = _sat_encode_text_tlv(&(src_tr->text), dst_tr, index, FALSE);
			index += encoded_len;
			break;
		case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
		case RESULT_BACKWARD_MOVE_BY_USER:
		case RESULT_HELP_INFO_REQUIRED_BY_USER:
		case RESULT_BEYOND_ME_CAPABILITIES:
		case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
		case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
		case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
		case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
			dst_tr[index++] = 1;
			dst_tr[index++] = src_tr->result_type;
			break;
		case RESULT_NO_RESPONSE_FROM_USER:
			dst_tr[index++] = 1;
			dst_tr[index++] = src_tr->result_type;

			encoded_len = _sat_encode_duration_tlv(&(src_tr->duration), dst_tr, index);
			index += encoded_len;
			break;
		case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
		case RESULT_FRAMES_ERROR:
			dst_tr[index++] = 2;
			dst_tr[index] = src_tr->result_type;
			dst_tr[index++] = src_tr->me_problem_type;
			break;
		default:
			dbg("invalid response value[0x%x] in current TR",src_tr->result_type);
			break;
	}

	return index;
}

static int _sat_encode_get_input(const struct tel_sat_tr_get_input_tlv *src_tr, char *dst_tr)
{
	int index = 0, encoded_len = 0;

	//set command detail info
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, index);
	index += encoded_len;

	//set device identities info
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, index);
	index += encoded_len;

	//set result info
	dst_tr[index++] = SATK_RESULT_TAG;

	switch(src_tr->result_type){
		case RESULT_SUCCESS:
		case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
		case RESULT_SUCCESS_WITH_MISSING_INFO:
		case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
			dst_tr[index++] = 1;
			dst_tr[index++] = src_tr->result_type;

			encoded_len = _sat_encode_text_tlv(&(src_tr->text), dst_tr, index, FALSE);
			index += encoded_len;
			break;
		case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
		case RESULT_BACKWARD_MOVE_BY_USER:
		case RESULT_NO_RESPONSE_FROM_USER:
		case RESULT_HELP_INFO_REQUIRED_BY_USER:
		case RESULT_BEYOND_ME_CAPABILITIES:
		case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
		case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
		case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
		case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
			dst_tr[index++] = 1;
			dst_tr[index++] = src_tr->result_type;
			break;
		case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
		case RESULT_FRAMES_ERROR:
			dst_tr[index++] = 2;
			dst_tr[index] = src_tr->result_type;
			dst_tr[index++] = src_tr->me_problem_type;
			break;
		default:
			dbg("invalid response value[0x%x] in current TR",src_tr->result_type);
			break;
	}

	return index;
}

static int _sat_encode_select_item(const struct tel_sat_tr_select_item_tlv *src_tr, char *dst_tr)
{
	int index = 0, encoded_len = 0;

	//set command detail info
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, index);
	index += encoded_len;

	//set device identities info
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, index);
	index += encoded_len;

	//set result info
	dst_tr[index++] = SATK_RESULT_TAG;

	switch(src_tr->result_type){
		case RESULT_SUCCESS:
		case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
		case RESULT_SUCCESS_WITH_MISSING_INFO:
		case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
			dst_tr[index++] = 1;
			dst_tr[index++] = src_tr->result_type;
			encoded_len = _sat_encode_item_identifier_tlv(&(src_tr->item_identifier), dst_tr, index);
			index += encoded_len;
			break;
		case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
		case RESULT_BACKWARD_MOVE_BY_USER:
		case RESULT_NO_RESPONSE_FROM_USER:
		case RESULT_HELP_INFO_REQUIRED_BY_USER:
		case RESULT_BEYOND_ME_CAPABILITIES:
		case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
		case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
		case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
		case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
			dst_tr[index++] = 1;
			dst_tr[index++] = src_tr->result_type;
			break;
		case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
		case RESULT_FRAMES_ERROR:
			dst_tr[index++] = 2;
			dst_tr[index] = src_tr->result_type;
			dst_tr[index++] = src_tr->me_problem_type;
			break;
		default:
			dbg("invalid response value[0x%x] in current TR",src_tr->result_type);
			break;
	}

	return index;
}

static int _sat_encode_setup_menu(const struct tel_sat_tr_setup_menu_tlv *src_tr, char *dst_tr)
{
	int index = 0, encoded_len = 0;

	//set command detail info
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, index);
	index += encoded_len;

	//set device identities info
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, index);
	index += encoded_len;

	//set result info
	dst_tr[index++] = SATK_RESULT_TAG;
	switch(src_tr->result_type){
		case RESULT_SUCCESS:
		case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
			dst_tr[index++] = 1;
			dst_tr[index++] = src_tr->result_type;
			break;
		case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
			dst_tr[index++] = 2;
			dst_tr[index] = src_tr->result_type;
			dst_tr[index++] = src_tr->me_problem_type;
			break;
		default:
			break;
	}

	return index;
}

int tcore_sat_encode_terminal_response(const struct treq_sat_terminal_rsp_data *src_tr, char *dst_tr){
	int tr_len = 0;

	if(!dst_tr)
		return 0;

	switch(src_tr->cmd_type){
		case SAT_PROATV_CMD_SETUP_EVENT_LIST:{
			tr_len = _sat_encode_setup_event_list(&(src_tr->terminal_rsp_data.setup_event_list), dst_tr);
		}break;
		case SAT_PROATV_CMD_DISPLAY_TEXT:{
			tr_len = _sat_encode_display_text(&(src_tr->terminal_rsp_data.display_text), dst_tr);
		}break;
		case SAT_PROATV_CMD_GET_INKEY:{
			tr_len = _sat_encode_get_inkey(&(src_tr->terminal_rsp_data.get_inkey), dst_tr);
		}break;
		case SAT_PROATV_CMD_GET_INPUT:{
			tr_len = _sat_encode_get_input(&(src_tr->terminal_rsp_data.get_input), dst_tr);
		}break;
		case SAT_PROATV_CMD_SELECT_ITEM:{
			tr_len = _sat_encode_select_item(&(src_tr->terminal_rsp_data.select_item), dst_tr);
		}break;
		case SAT_PROATV_CMD_SETUP_MENU:{
			tr_len = _sat_encode_setup_menu(&(src_tr->terminal_rsp_data.setup_menu), dst_tr);
		}break;

		default:
			err("no matched cmd type(%d)", src_tr->cmd_type);
			break;
	}

	return tr_len;
}

CoreObject *tcore_sat_new(TcorePlugin *p, const char *name, struct tcore_sat_operations *ops)
{
	CoreObject *o = NULL;
	struct private_object_data *po = NULL;

	if (!p)
		return NULL;

	o = tcore_object_new(p, name);
	if (!o)
		return NULL;

	po = calloc(sizeof(struct private_object_data), 1);
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	po->ops = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_SAT);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_sat_free(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SAT);

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	free(po);
	tcore_object_free(o);
}
