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
#include "internal/tcore_types.h"
#include "plugin.h"
#include "queue.h"
#include "user_request.h"
#include "core_object.h"
#include "util.h"
#include "co_sat.h"

#define SATK_PROACTIVE_CMD_TAG			0xD0 /* Proactive Command Tag */
#define SATK_MENU_SELECTION_TAG			0xD3 /* Menu Selection Tag */
#define SATK_EVENT_DOWNLOAD_TAG			0xD6 /* Event Download Tag */

/*
 * Tag Values (without Comprehension bit)
 */
#define SATK_COMMAND_DETAILS_TAG			0x01 /* COMMAND DETAILS TAG */
#define SATK_DEVICE_IDENTITY_TAG			0x02 /* DEVICE IDENTITY TAG */
#define SATK_RESULT_TAG				0x03 /* RESULT TAG */
#define SATK_DURATION_TAG				0x04 /* DURATION TAG */
#define SATK_ALPHA_IDENTIFIER_TAG			0x05 /* ALPHA IDENTIFIER TAG */
#define SATK_ADDRESS_TAG				0x06 /* ADDRESS TAG */
#define SATK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG	0x07 /* CAPABILITY CONFIGURATION PARAMETERS TAG */
#define SATK_SUB_ADDRESS_TAG				0x08 /* SUB ADDRESS TAG */
#define SATK_SS_STRING_TAG				0x09 /* SS STRING TAG */
#define SATK_USSD_STRING_TAG				0x0A /* USSD STRING TAG */
#define SATK_SMS_TPDU_TAG				0x0B /* SMS TPDU TAG */
#define SATK_CELL_BROADCAST_PAGE_TAG			0x0C /* CELL BROADCAST PAGE TAG */
#define SATK_TEXT_STRING_TAG				0x0D /* TEXT STRING TAG */
#define SATK_TONE_TAG					0x0E /* TONE TAG */
#define SATK_ITEM_TAG					0x0F /* ITEM TAG */
#define SATK_ITEM_IDENTIFIER_TAG			0x10 /* ITEM IDENTIFIER TAG */
#define SATK_RESPONSE_LENGTH_TAG			0x11 /* RESPONSE LENGTH TAG */
#define SATK_FILE_LIST_TAG				0x12 /* FILE LIST TAG */
#define SATK_LOCATION_INFORMATION_TAG		0x13 /* LOCATION INFORMATION TAG */
#define SATK_IMEI_TAG					0x14 /* IMEI TAG */
#define SATK_HELP_REQUEST_TAG				0x15 /* HELP REQUEST TAG */
#define SATK_NETWORK_MEASUREMENT_RESULTS_TAG	0x16 /* NETWORK MEASUREMENT RESULTS TAG */
#define SATK_DEFAULT_TEXT_TAG				0x17 /* DEFAULT TEXT TAG */
#define SATK_ITEMS_NEXT_ACTION_INDICATOR_TAG	0x18 /* ITEMS NEXT ACTION INDICATOR TAG */
#define SATK_EVENT_LIST_TAG				0x19 /* EVENT LIST TAG */
#define SATK_CAUSE_TAG					0x1A /* CAUSE TAG */
#define SATK_LOCATION_STATUS_TAG			0x1B /* LOCATION STATUS TAG */
#define SATK_BCCH_CHANNEL_LIST_TAG			0x1D /* BCCH CHANNEL LIST TAG */
#define SATK_ICON_IDENTIFIER_TAG			0x1E /* ICON IDENTIFIER TAG */
#define SATK_ITEM_ICON_IDENTIFIER_LIST_TAG		0x1F /* ITEM ICON IDENTIFIER LIST TAG */
#define SATK_DATE_TIME_AND_TIME_ZONE_TAG		0x26 /* DATE TIME AND TIME ZONE TAG */
#define SATK_CALL_CONTROL_REQUESTED_ACTION_TAG	0x27 /* CALL CONTROL REQUESTED ACTION TAG */
#define SATK_AT_COMMAND_TAG				0x28 /* AT COMMAND TAG */
#define SATK_AT_RESPONSE_TAG				0x29 /* AT RESPONSE TAG */
#define SATK_BC_REPEAT_INDICATOR_TAG			0x2A /* BC REPEAT INDICATOR TAG */
#define SATK_IMMEDIATE_RESPONSE_TAG			0x2B /* IMMEDIATE RESPONSE TAG */
#define SATK_DTMF_STRING_TAG				0x2C /* DTMF STRING TAG */
#define SATK_LANGUAGE_TAG				0x2D /* LANGUAGE TAG */
#define SATK_AID_TAG					0x2F /* AID TAG */
#define SATK_BROWSER_IDENTITY_TAG			0x30 /* BROWSER IDENTITY TAG */
#define SATK_URL_TAG					0x31 /* URL TAG */
#define SATK_BEARER_TAG				0x32 /* BEARER TAG */
#define SATK_PROVISIONING_REF_FILE_TAG		0x33 /* PROVISIONING REFERENCE FILE TAG */
#define SATK_BROWSER_TERMINATION_CAUSE_TAG		0x34 /* BROWSER TERMINATION CAUSE TAG */
#define SATK_BEARER_DISCRIPTION_TAG			0x35 /* BEARER DISCRIPTION TAG */
#define SATK_CHANNEL_DATA_TAG			0x36 /* CHANNEL DATA TAG */
#define SATK_CHANNEL_DATA_LEN_TAG			0x37 /* CHANNEL DATA LEN TAG */
#define SATK_CHANNEL_STATUS_TAG			0x38 /* CHANNEL STATUS TAG */
#define SATK_BUFFER_SIZE_TAG				0x39 /* BUFFER SIZE TAG */
#define SATK_CARD_READER_IDENTIFIER_TAG		0x3A /* CARD READER IDENTIFIER TAG */
#define SATK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG	0x3C /* USIM ME INTERFACE TRANSPORT LEVEL  */
#define SATK_OTHER_ADDRESS_TAG			0x3E /* OTHER ADDRESS        */
#define SATK_NETWORK_ACCESS_TAG			0x47 /* NETWORK ACCESS NAME TAG */
#define SATK_CDMA_SMS_TPDU_TAG			0x48 /* CDMA SMS TPDU TAG */
#define SATK_REMOTE_ENTITY_ADDRESS_TAG		0x49 /* REMOTE ENTITY ADDRESS TAG */
#define SATK_TEXT_ATTRIBUTE_TAG			0x50 /* TEXT ATTRIBUTE TAG */
#define SATK_TEXT_ATTRIBUTE_LIST_TAG			0x51 /* TEXT ATTRIBUTE LIST TAG */
#define SATK_IMEISV_TAG 				0x62 /* IMEISV TAG */
#define SATK_BATTERY_STATE_TAG 			0x63 /* BATTERY STATE TAG */
#define SATK_BROWSING_STATUS_TAG 		0x64 /* BROWSING STATUS TAG */
#define SATK_NETWORK_SEARCH_MODE_TAG 	0x65 /* NETWORK SEARCH MODE TAG */
#define SATK_FRAME_LAYOUT_TAG 			0x66 /* FRAME LAYOUT TAG */
#define SATK_FRAME_INFORMATION_TAG 		0x67 /* FRAME INFORMATION TAG */
#define SATK_FRAME_IDENTIFIER_TAG 		0x68 /* FRAME IDENTIFIER TAG */

/* general data object lengths */
#define SATK_DCS_LENGTH				0x01
#define SATK_COMMAND_DETAILS_LENGTH			0x03 /*  COMMAND DETAILS LENGTH */
#define SATK_DEVICE_IDENTITY_LENGTH			0x02 /*  DEVICE IDENTITY LENGTH */
#define SATK_ICON_IDENTITY_LENGTH			0x02 /*  ICON IDENTITY LENGTH */
#define SATK_DURATION_LENGTH				0x02 /*  DURATION LENGTH */
#define SATK_ITEM_IDENTIFIER_LENGTH			0x01 /*  ITEM IDENTIFIER LENGTH */
#define SATK_LOCATION_INFORMATION_LENGTH		0x07 /*  LOCATION INFORMATION LENGTH */
#define SATK_HELP_REQUEST_LENGTH			0x00 /*  HELP REQUEST LENGTH */
#define SATK_LOCATION_STATUS_LENGTH			0x01 /*  LOCATION STATUS LENGTH */
#define SATK_DATE_TIME_AND_TIME_ZONE_LENGTH	0x07 /*  DATE TIME AND TIME ZONE LENGTH */
#define SATK_LANGUAGE_LENGTH				0x02 /*  LANGUAGE LENGTH */
#define SATK_BC_REPEAT_IND_LENGTH			0x01 /*  BC REPEAT INDICATION LENGTH */
#define SATK_RESPONSE_LENGTH_LENGTH			0x02 /*  RESPONSE LENGTH LENGTH */
#define SATK_TONE_LENGTH				0x01 /*  TONE LENGTH */
#define SATK_BROWSER_ID_LENGTH			0x01 /*  BROWSER ID LENGTH */
#define SATK_BROWSER_TERMINATION_CAUSE_LENGTH	0x01 /*  BROWSER TERMINATION CAUSE LENGTH */
#define SATK_BUFFER_SIZE_LENGTH			0x02 /*  BUFFER SIZE LENGTH */
#define SATK_UICC_ME_TRANS_INTERFACE_LEVEL_LENGTH	0x03 /*  UICC TERMINAL TRANSPORT INTERFACE LEVEL LENGTH */
#define SATK_CHANNEL_DATA_LENGTH_VALUE_LENGTH	0x01 /*  CHANNEL DATA LENGTH VALUE LENGTH */
#define SATK_CHANNEL_STATUS_LENGTH				0x02 /*  CHANNEL STATUS LENGTH */
#define SATK_AID_LENGTH							0x01 /*  APPLICATION IDENTIFIER LENGTH */
#define SATK_FRAME_IDENTIFIER_LENGTH			0x01 /*  FRAME IDENTIFIER LENGTH */

struct private_object_data {
	struct tcore_sat_operations *ops[TCORE_OPS_TYPE_MAX];
};

gboolean b_comprehensive = FALSE;

static TReturn _dispatcher(CoreObject *o, UserRequest *ur, enum tcore_ops_type ops_type)
{
	enum tcore_request_command command;
	struct private_object_data *po = tcore_object_ref_object(o);
	struct tcore_sat_operations *ops = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_SAT, TCORE_RETURN_EINVAL);
	CORE_OBJECT_VALIDATE_OPS_RETURN_VAL(ops_type, TCORE_RETURN_EINVAL);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_EINVAL);
	tcore_check_null_ret_err("ur", ur, TCORE_RETURN_EINVAL);

	ops = po->ops[ops_type];
	tcore_check_null_ret_err("ops", ops, TCORE_RETURN_FAILURE);

	command = tcore_user_request_get_command(ur);
	switch (command) {
	case TREQ_SAT_REQ_ENVELOPE:
		tcore_check_null_ret_err("ops->envelope",
			ops->envelope, TCORE_RETURN_ENOSYS);

		return ops->envelope(o, ur);

	case TREQ_SAT_REQ_TERMINALRESPONSE:
		tcore_check_null_ret_err("ops->terminal_response",
			ops->terminal_response, TCORE_RETURN_ENOSYS);

		return ops->terminal_response(o, ur);

	case TREQ_SAT_REQ_USERCONFIRMATION:
		tcore_check_null_ret_err("ops->user_confirmation",
			ops->user_confirmation, TCORE_RETURN_ENOSYS);

		return ops->user_confirmation(o, ur);

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

static gboolean _check_file_for_refresh(enum tel_sim_file_id file_id)
{
	int i;
	enum tel_sim_file_id ef_under_mf[3] = {SIM_EF_DIR, SIM_EF_ELP, SIM_EF_ICCID};
	enum tel_sim_file_id ef_under_df[29] = {
		SIM_EF_IMSI, SIM_EF_SST,
		SIM_EF_EST, SIM_EF_OPLMN_ACT,
		SIM_EF_GID1, SIM_EF_GID2,
		SIM_EF_LP, SIM_EF_ECC,
		SIM_EF_SPN, SIM_EF_SPDI,
		SIM_EF_PNN, SIM_EF_OPL,
		SIM_EF_MBDN, SIM_EF_MSISDN,
		SIM_EF_USIM_MBI, SIM_EF_USIM_MWIS,
		SIM_EF_USIM_CFIS, SIM_EF_CPHS_VOICE_MSG_WAITING,
		SIM_EF_CPHS_SERVICE_STRING_TABLE, SIM_EF_CPHS_CALL_FORWARD_FLAGS,
		SIM_EF_CPHS_OPERATOR_NAME_STRING, SIM_EF_CPHS_CUSTOMER_SERVICE_PROFILE,
		SIM_EF_CPHS_CPHS_INFO, SIM_EF_CPHS_MAILBOX_NUMBERS,
		SIM_EF_CPHS_OPERATOR_NAME_SHORT_FORM_STRING, SIM_EF_CPHS_INFORMATION_NUMBERS,
		SIM_EF_CPHS_DYNAMICFLAGS, SIM_EF_CPHS_DYNAMIC2FLAG,
		SIM_EF_CPHS_CUSTOMER_SERVICE_PROFILE_LINE2};

	dbg("[SAT] SAT PARSER - FILE ID=0x%04x", (unsigned int)file_id);

	if ((file_id & 0x2F00) == 0x2F00) {
		dbg("[SAT] SAT PARSER - MF, EF=0x%04x", file_id);
		for (i = 0; i < 3; i++) {
			if (file_id == ef_under_mf[i]) {
				dbg("[SAT] SAT PARSER - MATCH!");
				return TRUE;
			}
		}
	} else if ((file_id & 0x6F00) == 0x6F00) {
		dbg("[SAT] SAT PARSER - ADF_USIM EF=0x%04x", file_id);
		for (i = 0; i < 29; i++) {
			if (file_id == ef_under_df[i]) {
				dbg("[SAT] SAT PARSER - MATCH!");
				return TRUE;
			}
		}
	}

	return FALSE;
}

static int _get_length_filed_size(unsigned char firstLenByte)
{
	if (firstLenByte <= 0x7F)
		return 1;
	else if (firstLenByte == 0x81)
		return 2;
	else
		return 0; /* return zero, as the length field can only be 1 or 2. */
}

static void _get_string_data(unsigned char *src, int len,
	struct tel_sat_text_string_object *text_obj)
{
	if (!src || !text_obj) return;

	switch (text_obj->dcs.a_format) {
	case ALPHABET_FORMAT_SMS_DEFAULT: {
		char *unpacked_str;

		text_obj->string_length = 0;
		unpacked_str = (char *)tcore_util_unpack_gsm7bit(src, (unsigned int)len);
		if (!unpacked_str)
			return;

		text_obj->dcs.a_format = ALPHABET_FORMAT_8BIT_DATA;
		text_obj->string_length = strlen(unpacked_str);

		if (text_obj->string_length >= SAT_TEXT_STRING_LEN_MAX) {
			text_obj->string_length = SAT_TEXT_STRING_LEN_MAX;
			memcpy(text_obj->string, unpacked_str, SAT_TEXT_STRING_LEN_MAX);
			text_obj->string[SAT_TEXT_STRING_LEN_MAX] = 0x00;
		} else {
			memcpy(text_obj->string, unpacked_str, text_obj->string_length);
			text_obj->string[text_obj->string_length] = 0x00;
		}

		g_free(unpacked_str);
	}
	break;

	case ALPHABET_FORMAT_UCS2:
	case ALPHABET_FORMAT_8BIT_DATA: {
		text_obj->string_length = len;
		if (text_obj->string_length >= SAT_TEXT_STRING_LEN_MAX) {
			text_obj->string_length = SAT_TEXT_STRING_LEN_MAX;
			memcpy(text_obj->string, src, SAT_TEXT_STRING_LEN_MAX);
			text_obj->string[SAT_TEXT_STRING_LEN_MAX] = 0x00;
		} else {
			memcpy(text_obj->string, src, text_obj->string_length);
			text_obj->string[text_obj->string_length + 1] = 0x00;
		}
	}
	break;

	default: {
		dbg("[SAT] SAT PARSER -  Unknown alphabet format(%d)", text_obj->dcs.a_format);
	}
	break;
	}
}

static void _sat_decode_dcs(unsigned char dcs, struct data_coding_scheme *dsc_obj)
{
	dbg("[SAT] SAT PARSER - dcs=[0x%x]", dcs);
	dsc_obj->raw_dcs = dcs;
	/* bit 5 of DCS byte will tell us if the text is compressed or not. */

	if (dcs & 0x20)
		dsc_obj->is_compressed_format = TRUE;
	else
		dsc_obj->is_compressed_format = FALSE;

	/* bit 4 when set, indicates that bits 0 & 1 have message class meaning. */
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
			err("invalid dcs type");
		break;
		}
	}

	/* bits 2 & 3 indicate the character set being used */
	switch (dcs & 0x0C) {
	case 0x00:
	case 0x0C:
		dsc_obj->a_format = ALPHABET_FORMAT_SMS_DEFAULT;
	break;

	case 0x04:
		dsc_obj->a_format = ALPHABET_FORMAT_8BIT_DATA;
	break;

	case 0X08:
		dsc_obj->a_format = ALPHABET_FORMAT_UCS2;
	break;

	default:
		dsc_obj->a_format = ALPHABET_FORMAT_RESERVED;
	break;
	}

	dbg("[SAT] SAT PARSER - is_compressed_format(%d), msgClass(0x%x), alpha_format(0x%x)",
		dsc_obj->is_compressed_format, dsc_obj->m_class, dsc_obj->a_format);
}

static void _sat_decode_ton_npi(unsigned char ton_npi, enum type_of_number *ton, enum numbering_plan_identifier *npi)
{
	int ton_value = 0;
	int npi_value = 0;

	if (!ton || !npi)
		return;

	ton_value = (ton_npi & 0x70) >> 4;
	*ton = ton_value;
	if (*ton > TON_NETWORK_SPECIFIC)
		*ton = TON_RESERVED_FOR_EXT;

	npi_value = (ton_npi & 0x0F);
	switch (npi_value) {
	case NPI_ISDN_TEL:
	case NPI_DATA_NUMBERING_PLAN:
	case NPI_TELEX:
	case NPI_PRIVATE:
	case NPI_RESERVED_FOR_EXT:
		*npi = npi_value;
	break;

	default:
		*npi = NPI_UNKNOWN;
	break;
	}

	dbg("[SAT] SAT PATSER - ton(0x%x) npi(0x%x)", *ton, *npi);
}

static enum tel_sim_language_type _sat_decode_language(unsigned char byte1, unsigned char byte2)
{
	if ((byte1 == 'e') && (byte2 == 'n'))
		return SIM_LANG_ENGLISH;
	else if ((byte1 == 'd') && (byte2 == 'e'))
		return SIM_LANG_GERMAN;
	else if ((byte1 == 'i') && (byte2 == 't'))
		return SIM_LANG_ITALIAN;
	else if ((byte1 == 'f') && (byte2 == 'r'))
		return SIM_LANG_FRENCH;
	else if ((byte1 == 'e') && (byte2 == 's'))
		return SIM_LANG_SPANISH;
	else if ((byte1 == 'n') && (byte2 == 'l'))
		return SIM_LANG_DUTCH;
	else if ((byte1 == 's') && (byte2 == 'v'))
		return SIM_LANG_SWEDISH;
	else if ((byte1 == 'd') && (byte2 == 'a'))
		return SIM_LANG_DANISH;
	else if ((byte1 == 'p') && (byte2 == 't'))
		return SIM_LANG_PORTUGUESE;
	else if ((byte1 == 'f') && (byte2 == 'i'))
		return SIM_LANG_FINNISH;
	else if ((byte1 == 'n') && (byte2 == 'o'))
		return SIM_LANG_NORWEGIAN;
	else if ((byte1 == 'e') && (byte2 == 'l'))
		return SIM_LANG_GREEK;
	else if ((byte1 == 't') && (byte2 == 'r'))
		return SIM_LANG_TURKISH;
	else if ((byte1 == 'h') && (byte2 == 'u'))
		return SIM_LANG_HUNGARIAN;
	else if ((byte1 == 'p') && (byte2 == 'i'))
		return SIM_LANG_POLISH;
	else {
		dbg("[SAT] SAT PARSER -  unknown language, default to english.");
		return SIM_LANG_ENGLISH;
	}
}

/*
 * Decode TLV data object
 */
static enum tcore_sat_result _sat_decode_address_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_address *address_obj, int *consumed_data_len)
{
	unsigned char *src_data;
	int temp_index, len_of_len = 0;
	int address_len = 0;
	gboolean comprehensive_req = FALSE;

	if (tlv_str == NULL || consumed_data_len == NULL || address_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || address_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset  +  1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index] & 0x7F) != SATK_ADDRESS_TAG) {
		dbg("[SAT] SAT PARSER -  address TAG missing");
		return TCORE_SAT_REQUIRED_VALUE_MISSING;
	}

	/* comprehensive required */
	if ((src_data[temp_index++] & 0x80))
		comprehensive_req = TRUE;

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		dbg("[SAT] SAT PARSER -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}
	address_len = src_data[temp_index  +  len_of_len - 1];

	/* check the address length */
	temp_index += len_of_len;
	if ((temp_index  +  address_len) > tlv_len) {
		dbg("[SAT] SAT PARSER  -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	address_obj->dialing_number_len = 0;
	if (address_len > 1) {
		char *str_ascii = NULL;
		_sat_decode_ton_npi(src_data[temp_index++], &address_obj->ton, &address_obj->npi);
		str_ascii = tcore_util_convert_bcd2ascii((const char *)&src_data[temp_index], address_len - 1, SAT_DIALING_NUMBER_LEN_MAX);
		if (str_ascii) {
			memcpy(address_obj->dialing_number, str_ascii, strlen(str_ascii));
			address_obj->dialing_number_len = strlen(str_ascii);
			g_free(str_ascii);
		}
	}

	if (address_obj->dialing_number_len == 0) {
		if (comprehensive_req) {
			err("[SAT] SAT PARSER -  incorrect address");
			return TCORE_SAT_REQUIRED_VALUE_MISSING;
		}

		dbg("comprehensive partial proactive command");
		/* global variable (comprehensive_partial= TRUE) */
	}

	*consumed_data_len = 1  +  len_of_len  +  address_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_subaddress_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_subaddress *sub_address_obj, int *consumed_data_len)
{
	unsigned char *src_data;
	int temp_index, len_of_len = 0;
	int sub_address_len = 0;
	gboolean comprehensive_req = FALSE;

	if (tlv_str == NULL || consumed_data_len == NULL || sub_address_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || sub_address_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset  +  1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index] & 0x7F) != SATK_SUB_ADDRESS_TAG) {
		dbg("[SAT] SAT PARSER -  sub address TAG missing");
		return TCORE_SAT_REQUIRED_VALUE_MISSING;
	}

	/* comprehensive required */
	if ((src_data[temp_index++] & 0x80))
		comprehensive_req = TRUE;

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		dbg("[SAT] SAT PARSER -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	sub_address_len = src_data[temp_index  +  len_of_len - 1];

	/* check the address length */
	temp_index += len_of_len;
	if ((temp_index  +  sub_address_len) > tlv_len) {
		dbg("[SAT] SAT PARSER  -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* sub address */
	if (sub_address_len <= 0) {
		dbg("[SAT] SAT PARSER - no sub address data");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	sub_address_obj->subaddress_len = sub_address_len;
	if (sub_address_obj->subaddress_len > SAT_CCP_DATA_LEN_MAX) {
		if (comprehensive_req) {
			dbg("[SAT] SAT PARSER - sub address is too big");
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
		} else {
			/* bIsComprehensionPartial = TRUE; */
			sub_address_obj->subaddress_len = 0;
		}
	} else
		memcpy(sub_address_obj->subaddress, &src_data[temp_index], sub_address_obj->subaddress_len);

	*consumed_data_len = 1  +  len_of_len  +  sub_address_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_alpha_identifier_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_alpha_identifier *alpha_id_obj, int *consumed_data_len)
{
	unsigned char *src_data;
	int temp_index, len_of_len = 0;
	int alpha_len = 0;

	if (tlv_str == NULL || consumed_data_len == NULL || alpha_id_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || alpha_id_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset  +  1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_ALPHA_IDENTIFIER_TAG) {
		dbg("[SAT] SAT PARSER -  alphaID TAG missing");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* length */
	alpha_id_obj->is_exist = TRUE;
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		dbg("[SAT] SAT PARSER -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	alpha_len = alpha_id_obj->alpha_data_len = src_data[temp_index  +  len_of_len - 1];

	/* alpha identifier */
	temp_index += len_of_len;
	if ((temp_index  +  alpha_len) > tlv_len) {
		dbg("[SAT] SAT PARSER  -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (alpha_id_obj->alpha_data_len > 0) {

		unsigned char dcs;

		if (alpha_id_obj->alpha_data_len >= SAT_ALPHA_ID_LEN_MAX)
			alpha_id_obj->alpha_data_len = SAT_ALPHA_ID_LEN_MAX - 1;

		memcpy(alpha_id_obj->alpha_data, &src_data[temp_index], alpha_id_obj->alpha_data_len);
		alpha_id_obj->alpha_data[alpha_id_obj->alpha_data_len] = 0x00;

		if (src_data[temp_index] == 0x80 || src_data[temp_index] == 0x81 || src_data[temp_index] == 0x82)
			dcs = 0X08;
		else
			dcs = 0x04;

		_sat_decode_dcs(dcs, &alpha_id_obj->dcs);
	}

	*consumed_data_len = 1  +  len_of_len  +  alpha_len;
	dbg("[SAT] SAT PARSER alphaId= %s", alpha_id_obj->alpha_data);
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_sub_address_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_subaddress *sub_address_obj, int *consumed_data_len)
{
	int i = 0;
	int temp_index, len_of_len = 0;
	int sub_address_len = 0;
	unsigned char *src_data;
	gboolean comprehension_req = FALSE;

	if (tlv_str == NULL || consumed_data_len == NULL || sub_address_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || sub_address_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset  +  1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index] & 0x7F) != SATK_SUB_ADDRESS_TAG) {
		dbg("[SAT] SAT PARSER -  address TAG missing");
		return TCORE_SAT_REQUIRED_VALUE_MISSING;
	}

	/* comprehensive required */
	if ((src_data[temp_index++] & 0x80))
		comprehension_req = TRUE;

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		dbg("[SAT] SAT PARSER -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}
	sub_address_len = src_data[temp_index  +  len_of_len - 1];

	/* check the address length */
	temp_index += len_of_len;
	if ((temp_index  +  sub_address_len) > tlv_len) {
		dbg("[SAT] SAT PARSER  -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (sub_address_len <= 0) {
		dbg("[SAT] SAT PARSER  -  no sub address");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	sub_address_obj->subaddress_len = sub_address_len;
	if (sub_address_obj->subaddress_len > SAT_SUB_ADDR_LEN_MAX) {
		if (comprehension_req)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
		else
			sub_address_obj->subaddress_len = 0;
	} else
		memcpy(sub_address_obj->subaddress, &src_data[temp_index], sub_address_obj->subaddress_len);

	dbg("[SAT] SAT PARSER -  subAddressLen=%d", sub_address_obj->subaddress_len);
	for (i = 0; i < sub_address_obj->subaddress_len; i++)
		dbg("[SAT] SAT PARSER - 0x%02x\t", sub_address_obj->subaddress[i]);

	*consumed_data_len = 1 + len_of_len + sub_address_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_ccp_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_ccp *ccp_obj, int *consumed_data_len)
{
	int i = 0;
	int temp_index, len_of_len = 0;
	int ccp_len = 0;
	unsigned char *src_data;
	gboolean comprehension_req = FALSE;

	if (tlv_str == NULL || consumed_data_len == NULL || ccp_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || ccp_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset  +  1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index] & 0x7F)
			!= SATK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG) {
		dbg("[SAT] SAT PARSER -  CCP TAG missing");
		return TCORE_SAT_REQUIRED_VALUE_MISSING;
	}

	/* comprehensive required */
	if ((src_data[temp_index++] & 0x80))
		comprehension_req = TRUE;

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		dbg("[SAT] SAT PARSER -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	ccp_len = src_data[temp_index  +  len_of_len - 1];

	/* check the address length */
	temp_index += len_of_len;
	if ((temp_index  +  ccp_len) > tlv_len) {
		dbg("[SAT] SAT PARSER  -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (ccp_len <= 0) {
		dbg("[SAT] SAT PARSER  -  no ccp data");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	ccp_obj->data_len = ccp_len;
	if (ccp_obj->data_len > SAT_CCP_DATA_LEN_MAX) {
		if (comprehension_req)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
		else
			ccp_obj->data_len = 0;
	} else
		memcpy(ccp_obj->data, &src_data[temp_index], ccp_obj->data_len);

	dbg("[SAT] SAT PARSER -  ccp len=%d", ccp_obj->data_len);
	for (i = 0; i < ccp_obj->data_len; i++)
		dbg("[SAT] SAT PARSER - 0x%02x\t", ccp_obj->data[i]);

	*consumed_data_len = 1 + len_of_len + ccp_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_device_identities_tlv(unsigned char *tlv_str,
	struct tel_sat_device_identities *dev_id_obj)
{
	int temp_index = 0, i;

	if (tlv_str == NULL || dev_id_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str  == NULL || dev_id_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if ((tlv_str[temp_index++] & 0x7F) != SATK_DEVICE_IDENTITY_TAG) {
		dbg("[SAT] SAT PARSER -  device identity tag missing.");
		return TCORE_SAT_REQUIRED_VALUE_MISSING; /* Send TR */
	}

	if (tlv_str[temp_index++] != SATK_DEVICE_IDENTITY_LENGTH) {
		dbg("[SAT] SAT PARSER -  device identity length incorrect.");
		return TCORE_SAT_REQUIRED_VALUE_MISSING; /* Send TR */
	}

	for (i = 0; i < 2; i++) {
		switch (tlv_str[temp_index]) {
		case DEVICE_ID_KEYPAD:
		case DEVICE_ID_DISPLAY:
		case DEVICE_ID_EARPIECE:
		case DEVICE_ID_SIM:
		case DEVICE_ID_ME:
		case DEVICE_ID_NETWORK:
			if (i == 0) dev_id_obj->src = tlv_str[temp_index];
			if (i == 1) dev_id_obj->dest = tlv_str[temp_index];
		break;

		default:
			if (tlv_str[temp_index] >= 0x21 && tlv_str[temp_index] <= 0x27) {
				dbg("BIP channel id(0x%x)", tlv_str[temp_index]);
				if (i == 0) dev_id_obj->src = tlv_str[temp_index];
				if (i == 1) dev_id_obj->dest = tlv_str[temp_index];
			} else {
				dbg("unmatched device id");
				return TCORE_SAT_REQUIRED_VALUE_MISSING;
			}
		break;
		}

		temp_index++;
	}

	dbg("[SAT] SAT PARSER -  source=%d, dest=%d", dev_id_obj->src, dev_id_obj->dest);
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_duration_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_duration *duration_obj, int *consumed_data_len)
{
	int temp_index = 0;
	unsigned char *src_data = NULL;

	if (!tlv_str || !duration_obj || !consumed_data_len) {
		err("[SAT] SAT PARSER data is null");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len < curr_offset  +  3) {
		err("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_DURATION_TAG) {
		err("[SAT] SAT PARSER -  duration tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	if (src_data[temp_index++] != SATK_DURATION_LENGTH) {
		err("[SAT] SAT PARSER -  incorrect length value.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	/* time unit */
	switch (src_data[temp_index]) {
	case TIME_UNIT_MINUTES:
	case TIME_UNIT_SECONDS:
	case TIME_UNIT_TENTHS_OF_SECONDS:
		duration_obj->time_unit = src_data[temp_index];
	break;

	default:
		duration_obj->time_unit = TIME_UNIT_RESERVED;
	break;
	}

	/* interval */
	temp_index++;
	duration_obj->time_interval = src_data[temp_index];
	*consumed_data_len = 4;

	dbg("[SAT] SAT PARSER -  timeUnit=%d, interval=%d",
		duration_obj->time_unit, duration_obj->time_interval);

	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_item_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_item_info *item_obj, int *consumed_data_len)
{
	int temp_index, len_of_len = 0, i = 0;
	int item_len = 0;
	unsigned char dcs;
	unsigned char *src_data = NULL;

	if (tlv_str == NULL || consumed_data_len == NULL || item_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || item_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_ITEM_TAG) {
		dbg("[SAT] SAT PARSER -  tag not found.=%d", src_data[temp_index-1]);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		dbg("[SAT] SAT PARSER -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	item_len = src_data[temp_index + len_of_len-1];

	temp_index += len_of_len; /* temp_index pointing to item. */
	if ((temp_index + item_len) > tlv_len) {
		dbg("[SAT] SAT PARSER -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	memset(item_obj->text, 0x00, (SAT_ITEM_TEXT_LEN_MAX + 1));
	if (item_len <= 0) {
		dbg("[SAT] SAT PARSER -  menu_text is NULL, remove the menu");
		*consumed_data_len = 1 + len_of_len + item_len;
		return TCORE_SAT_SUCCESS;
	}

	/* item */
	item_obj->item_id = src_data[temp_index++];

	/*
	 * fix for orange SIM issue
	 * H0100078487 Strange Character display on SIM SAT
	 * The string length was less than its real length
	 * So garbage characters was displayed. To fix it, we would recalculate the real length.
	 */
	for (i = 0;  i < item_len-1; i++) {
		dbg("[SAT] %d: %c", i, src_data[temp_index + i]);
		if (src_data[temp_index + i] == 0xFF) break;
	}

	item_obj->text_len = i;

	if (item_obj->text_len <= 0) {
		dbg("[SAT] SAT PARSER -  text len = 0");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (item_obj->text_len >= SAT_ITEM_TEXT_LEN_MAX)
		item_obj->text_len = SAT_ITEM_TEXT_LEN_MAX-1;

	memcpy(item_obj->text, &src_data[temp_index], item_obj->text_len);
	if (src_data[temp_index] == 0x80 || src_data[temp_index] == 0x81 || src_data[temp_index] == 0x82)
		dcs = 0X08;
	else
		dcs = 0x04;

	_sat_decode_dcs(dcs, &item_obj->dcs);


	dbg("[SAT] SAT PARSER -  item_text=%s", item_obj->text);
	*consumed_data_len = 1 + len_of_len + item_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_response_length_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_response_length *response_obj, int *consumed_data_len)
{
	int temp_index = 0;
	unsigned char *src_data = NULL;

	if (!tlv_str || !response_obj || !consumed_data_len) {
		err("[SAT] SAT PARSER data is null");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= curr_offset  +  1) {
		err("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_RESPONSE_LENGTH_TAG) {
		err("[SAT] SAT PARSER -  response length tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	if (src_data[temp_index++] != SATK_RESPONSE_LENGTH_LENGTH) {
		err("[SAT] SAT PARSER -  incorrect length value.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	response_obj->min = src_data[temp_index++];
	response_obj->max = src_data[temp_index++];
	dbg("[SAT] SAT PARSER  min length(%d), max length(%d)", response_obj->min, response_obj->max);

	*consumed_data_len = 4;
	if (response_obj->min > response_obj->max) {
		err("[SAT] SAT PARSER - : min length is larger than max length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_sms_tpdu_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_sms_tpdu *sms_tpdu_obj, int *consumed_data_len)
{
	int temp_index = 0, len_of_len = 0;
	int tpdu_len = 0;
	unsigned char *src_data = NULL;

	if (!tlv_str || !sms_tpdu_obj || !consumed_data_len) {
		err("[SAT] SAT PARSER data is null");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= curr_offset  +  1) {
		err("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_SMS_TPDU_TAG) {
		err("[SAT] SAT PARSER -  sat tpdu tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		err("[SAT] parser: invalid length.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}
	tpdu_len = src_data[temp_index + len_of_len-1];
	temp_index += len_of_len;

	if (tpdu_len <= 0)
		return TCORE_SAT_REQUIRED_VALUE_MISSING;

	/* data len */
	sms_tpdu_obj->data_len = tpdu_len;
	if (sms_tpdu_obj->data_len > SAT_SMS_TPDU_SMS_DATA_LEN_MAX)
		sms_tpdu_obj->data_len = SAT_SMS_TPDU_SMS_DATA_LEN_MAX;

	/* data */
	memcpy(sms_tpdu_obj->data, &src_data[temp_index], sms_tpdu_obj->data_len);
	dbg("[SAT] SAT PARSER tpdu_len (%d)", sms_tpdu_obj->data_len);

	*consumed_data_len = 1 + len_of_len + tpdu_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_item_identifier_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_item_identifier *item_identifier_obj, int *consumed_data_len)
{
	int temp_index = 0;
	unsigned char *src_data = NULL;

	if (!tlv_str || !item_identifier_obj || !consumed_data_len) {
		err("[SAT] SAT PARSER data is null");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= curr_offset  +  1) {
		err("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_ITEM_IDENTIFIER_TAG) {
		err("[SAT] SAT PARSER -  Item identifier tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	if (src_data[temp_index++] != SATK_ITEM_IDENTIFIER_LENGTH) {
		err("[SAT] SAT PARSER -  incorrect length value.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	item_identifier_obj->item_identifier = src_data[temp_index++];
	*consumed_data_len = 3;
	dbg("[SAT] SAT PARSER item identifier(0x%02x)", item_identifier_obj->item_identifier);

	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_ss_string_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_ss_string *ss_str_obj, int *consumed_data_len)
{
	char *str_ascii = NULL;
	int temp_index, len_of_len = 0;
	int ss_len = 0;
	unsigned char *src_data;
	gboolean comprehension_req = FALSE;

	if (!tlv_str || !ss_str_obj || !consumed_data_len) {
		err("[SAT] SAT PARSER data is null");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= curr_offset  +  1) {
		err("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index] & 0x7F) != SATK_SS_STRING_TAG) {
		err("[SAT] SAT PARSER -  SS string tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	if (src_data[temp_index++] & 0x80)
		comprehension_req = TRUE;

	dbg("comprehension_req=[%d]", comprehension_req);

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		err("[SAT] parser: invalid length.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	ss_len = src_data[temp_index + len_of_len-1];
	dbg("[SAT] parser: ss_tlv len=%d", ss_len);

	temp_index += len_of_len;
	ss_str_obj->string_len = 0;

	/* ss data */
	if (ss_len <= 1)
		return TCORE_SAT_REQUIRED_VALUE_MISSING;

	_sat_decode_ton_npi(src_data[temp_index++], &ss_str_obj->ton, &ss_str_obj->npi);
	str_ascii = tcore_util_convert_bcd2ascii((const char *)&src_data[temp_index], ss_len-1, SAT_SS_STRING_LEN_MAX);
	if (str_ascii) {
		memcpy(ss_str_obj->ss_string, str_ascii, strlen(str_ascii));
		ss_str_obj->string_len = strlen(str_ascii);
		g_free(str_ascii);
	}

	 /*  1 is the length of Tag. */
	*consumed_data_len = 1  +  len_of_len  +  ss_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_text_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_text_string_object *text_obj, int *consumed_data_len)
{
	int temp_index, len_of_len = 0;
	int text_len = 0;
	unsigned char *src_data;
	gboolean comprehension_req = FALSE;

	if (!tlv_str || !consumed_data_len) {
		err("[SAT] parser: data is null");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {
		err("[SAT] parser: incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index] & 0x7F) != SATK_TEXT_STRING_TAG
			&& (src_data[temp_index] & 0x7F) != SATK_DEFAULT_TEXT_TAG) {
		err("[SAT] parser: text string tag missing, tag=0x%x",
			src_data[temp_index]);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (src_data[temp_index++] & 0x80)
		comprehension_req = TRUE;

	dbg("comprehension_req=[%d]", comprehension_req);

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		err("[SAT] parser: invalid length.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	text_len = src_data[temp_index + len_of_len-1];
	temp_index += len_of_len;
	memset(text_obj->string, 0x00, SAT_TEXT_STRING_LEN_MAX);

	/* text */
	if (text_len <= 1) {
		text_obj->string_length = 0;
	} else {
		_sat_decode_dcs(src_data[temp_index++], &text_obj->dcs);
		_get_string_data(&src_data[temp_index], text_len-1, text_obj);
	}

	dbg("[SAT] parser: text_tlv_len=%d, parsed text length=%d", text_len, text_obj->string_length);

	/*  1 is the length of Tag. */
	*consumed_data_len = 1  +  len_of_len  +  text_len;

	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_tone_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_tone *tone_obj, int *consumed_data_len)
{
	int temp_index;
	unsigned char *src_data;
	gboolean comprehension_req = FALSE;

	if (tlv_str == NULL || consumed_data_len == NULL || tone_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || tone_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER - incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index] & 0x7F) != SATK_TONE_TAG) {
		err("[SAT] parser: tone tag missing");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (src_data[temp_index++] & 0x80)
		comprehension_req = TRUE;

	/* length */
	if (src_data[temp_index++] != SATK_TONE_LENGTH) {
		err("[SAT] SAT PARSER -  incorrect length value.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	if ((temp_index + SATK_TONE_LENGTH) > tlv_len) {
		err("[SAT] SAT PARSER -  incorrect cmd len, expected len = %d, orig_len=%d",
			(temp_index + SATK_TONE_LENGTH), tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tone */
	switch (src_data[temp_index]) {
	/* standard supervisory tones */
	case DIAL_TONE:
	case CALLED_SUBSCRIBER_BUSY:
	case CONGESTION:
	case RADIO_PATH_ACK:
	case RADIO_PATH_NOT_AVAILABLE_CALL_DROPPED:
	case ERR_SPECIAL_INFO:
	case CALL_WAITING_TONE:
	case RINGING_TONE:
	/* ME proprietary tones */
	case GENERAL_BEEP:
	case POSITIVE_ACK_TONE:
	case NEGATIVE_ACK_OR_ERROR_TONE:
	case RINGING_TONE_SLCTD_BY_USR_FOR_INCOM_SPEECH_CALL:
	case ALERT_TONE_SELECTED_BY_USER_FOR_INCOMING_SMS:
	case CRITICAL_ALERT:
	/* Themed tones */
	case HAPPY_TONE:
	case SAD_TONE:
	case URGENT_ACTION_TONE:
	case QUESTION_TONE:
	case MESSAGE_RECEIVED_TONE:
	/* Melody tones */
	case MELODY_1:
	case MELODY_2:
	case MELODY_3:
	case MELODY_4:
	case MELODY_5:
	case MELODY_6:
	case MELODY_7:
	case MELODY_8:
		dbg("[SAT] SAT PARSER -  Tone =0x%x", src_data[temp_index]);
		tone_obj->tone_type = src_data[temp_index];
	break;

	case TONE_TYPE_RESERVED: /* Fallthrough */
	default:
		dbg("[SAT] SAT PARSER -  Reserved value of Tone =0x%x", src_data[temp_index]);
		if (comprehension_req)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	break;
	}

	*consumed_data_len = 3;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_ussd_string_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_ussd_string *ussd_str_obj, int *consumed_data_len)
{
	int temp_index, len_of_len = 0;
	int ussd_len = 0;
	unsigned char *src_data;
	gboolean comprehension_req = FALSE;

	if (!tlv_str || !ussd_str_obj || !consumed_data_len) {
		err("[SAT] SAT PARSER data is null");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= curr_offset  +  1) {
		err("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index] & 0x7F) != SATK_USSD_STRING_TAG) {
		err("[SAT] SAT PARSER -  SS string tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	if (src_data[temp_index++] & 0x80)
		comprehension_req = TRUE;

	dbg("comprehension_req=[%d]", comprehension_req);

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		err("[SAT] parser: invalid length.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	ussd_len = src_data[temp_index + len_of_len-1];
	dbg("[SAT] parser: ussd_tlv len=%d", ussd_len);

	temp_index += len_of_len;
	ussd_str_obj->string_len = 0;

	/* ussd data */
	if (ussd_len <= 1)
		return TCORE_SAT_REQUIRED_VALUE_MISSING;

	_sat_decode_dcs(src_data[temp_index++], &ussd_str_obj->dsc);
	ussd_str_obj->string_len = ussd_len - 1;
	memcpy(ussd_str_obj->ussd_string, &src_data[temp_index], ussd_str_obj->string_len);

	 /* 1 is the length of Tag. */
	*consumed_data_len = 1  +  len_of_len  +  ussd_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_file_list_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_file_list *file_list_obj, int *consumed_data_len)
{
	/* tmp */
	int tmp_cnt;
	int f_count;
	unsigned int ef = 0x0000;

	int temp_index, len_of_len = 0;
	int file_list_len = 0;
	unsigned char *src_data;

	if (tlv_str == NULL || consumed_data_len == NULL || file_list_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || file_list_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER - incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index] & 0x7F) != SATK_FILE_LIST_TAG) {
		err("[SAT] parser: tag missing, tag=0x%x", src_data[temp_index]);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		err("[SAT] parser: invalid length.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	file_list_len = src_data[temp_index + len_of_len-1];
	temp_index += len_of_len;

	if ((temp_index + file_list_len) > tlv_len) {
		dbg("[SAT] SAT PARSER - incorrect cmd len, expected len = %d, orig_len=%d",
			(temp_index + file_list_len), tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	tmp_cnt = src_data[temp_index++];
	file_list_obj->file_count = 0;
	memset(file_list_obj->file_id, 0, SAT_FILE_ID_LIST_MAX_COUNT);

	if (!tmp_cnt) {
		dbg("file cnt = 0");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	f_count = 0;
	do {
		ef = 0x0000;
		if (src_data[temp_index] != 0x3F || src_data[temp_index + 1] != 0x00) {
			temp_index++;
			if (temp_index > tlv_len)
				break;
			else
				continue;
		}

		temp_index += 2; /* MASTER FILE (DIR) 0x3F00 */

		if (src_data[temp_index] == 0x2F) {
			/* ELEMENTRY FILE (VALUE) */
			ef = src_data[temp_index] << 8;
			temp_index++;
			ef = ef | src_data[temp_index];

			if (_check_file_for_refresh((enum tel_sim_file_id)ef)) {/* check file registered for refresh? */
				file_list_obj->file_id[file_list_obj->file_count] = ef;
				file_list_obj->file_count++;
			}
		} else if (src_data[temp_index] == 0x7F && src_data[temp_index + 1] == 0xFF) {
			/* USIM DIRECTORY FILE (DIR) 0x7FFF */
			temp_index += 2;
			if (src_data[temp_index] == 0x6F) {
				ef = 0x6A << 8;
				temp_index++;
				ef = ef | src_data[temp_index];

				if (_check_file_for_refresh((enum tel_sim_file_id)ef)) {/* check file registered for refresh? */
					file_list_obj->file_id[file_list_obj->file_count] = ef;
					file_list_obj->file_count++;
				}
			} else
				temp_index++;
		} else if (src_data[temp_index] == 0x7F && (src_data[temp_index + 1] == 0x10
				|| src_data[temp_index + 1] == 0x20)) {
			/* TELECOM DIRECTORY FILE 0x7F10 or GSM DIRECTORY FILE 0x7F20 */
			temp_index += 2;
			if (src_data[temp_index] == 0x6F) {
				ef = src_data[temp_index] << 8;
				temp_index++;
				ef = ef | src_data[temp_index];

				if (_check_file_for_refresh((enum tel_sim_file_id)ef)) {/* check file registered for refresh? */
					file_list_obj->file_id[file_list_obj->file_count] = ef;
					file_list_obj->file_count++;
				}
			} else
				temp_index++;
		}

		f_count++;
		temp_index++;
	} while (f_count < tmp_cnt);

	dbg("[SAT] SAT PARSER -  total file count=%d, PDA file count = %d", tmp_cnt, file_list_obj->file_count);
	*consumed_data_len = 1  +  len_of_len  +  file_list_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_item_next_action_indicator_tlv(unsigned char *tlv_str, int tlv_len, int curr_offset,
	struct tel_sat_item_next_action_indicatior_list *item_next_act_indi_obj, int *consumed_data_len)
{
	int temp_index;
	int item_nai_len;
	unsigned char *src_data;
	gboolean comprehension_req = FALSE;

	if (tlv_str == NULL || consumed_data_len == NULL || item_next_act_indi_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || item_next_act_indi_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER - incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index] & 0x7F) != SATK_ITEMS_NEXT_ACTION_INDICATOR_TAG) {
		dbg("[SAT] SAT PARSER - tag not found.=%d", src_data[temp_index]);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if ((src_data[temp_index++] & 0x7F))
		comprehension_req = TRUE;

	/* item cnt */
	item_nai_len = item_next_act_indi_obj->cnt = src_data[temp_index++];
	if ((temp_index + item_nai_len) > tlv_len) {
		dbg("[SAT] SAT PARSER - incorrect cmd len, expected len = %d, orig_len=%d",
			(temp_index + item_nai_len), tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (item_next_act_indi_obj->cnt > SAT_ITEMS_NEXT_ACTION_INDI_LIST_MAX_COUNT) {
		if (comprehension_req == TRUE) {
			dbg("[SAT] SAT PARSER - list count exceeds maximum allowed count=%d", item_next_act_indi_obj->cnt);
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
		}

		item_next_act_indi_obj->cnt = 0;
	}

	memset(item_next_act_indi_obj->indicator_list, 0xFF, SAT_ITEMS_NEXT_ACTION_INDI_LIST_MAX_COUNT);
	if (item_next_act_indi_obj->cnt > 0)
		memcpy(item_next_act_indi_obj->indicator_list, &src_data[temp_index], item_next_act_indi_obj->cnt);

	*consumed_data_len = 1 + 1 + item_nai_len;
	dbg("[SAT] SAT PARSER - listCount=%d, consumed_data_len = %d", item_next_act_indi_obj->cnt, *consumed_data_len);
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_event_list_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_event_list *event_list_obj, int *consumed_data_len)
{
	int i = 0;
	int temp_index, len_of_len = 0;
	int evt_list_len;
	unsigned char *src_data;
	gboolean comprehension_req = FALSE;

	if (tlv_str == NULL || consumed_data_len == NULL || event_list_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || event_list_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER - incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index] & 0x7F) != SATK_EVENT_LIST_TAG) {
		dbg("[SAT] SAT PARSER - tag not found.=%d", src_data[temp_index]);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if ((src_data[temp_index++] & 0x80))
		comprehension_req = TRUE;

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		err("[SAT] parser: invalid length.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	evt_list_len = src_data[temp_index + len_of_len-1];
	dbg("[SAT] parser: evt_list_len=%d", evt_list_len);
	temp_index += len_of_len;

	if ((temp_index + evt_list_len) > tlv_len) {
		dbg("[SAT] SAT PARSER - incorrect cmd len, expected len = %d, orig_len=%d",
			(temp_index + evt_list_len), tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (evt_list_len > SAT_EVENT_LIST_MAX) {
		dbg("[SAT] SAT PARSER - event list contains more items than it is supposed to have! len=%d",
			evt_list_len);
		if (comprehension_req)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
		else
			evt_list_len = SAT_EVENT_LIST_MAX;
	}

	event_list_obj->event_list_cnt = 0;
	memset(event_list_obj->evt_list, 0xFF, SAT_EVENT_LIST_MAX);

	/* event list */
	for (i = 0; i < evt_list_len; i++) {
		dbg("[SAT] SAT PARSER - event[%d]=0x%x",
			i, src_data[temp_index]);
		switch (src_data[temp_index]) {
		/* PDA events */
		case EVENT_USER_ACTIVITY:
		case EVENT_IDLE_SCREEN_AVAILABLE:
		case EVENT_LANGUAGE_SELECTION:
		case EVENT_BROWSER_TERMINATION:
		case EVENT_DATA_AVAILABLE:
		case EVENT_CHANNEL_STATUS:
		case EVENT_MT_CALL:
		case EVENT_CALL_CONNECTED:
		case EVENT_CALL_DISCONNECTED:
		case EVENT_LOCATION_STATUS:
		case EVENT_ACCESS_TECHNOLOGY_CHANGED:
			event_list_obj->evt_list[i] = src_data[temp_index];
			event_list_obj->event_list_cnt++;
		break;

		case EVENT_UNKNOWN: /* Fallthrough */
		default:
			if (comprehension_req)
				return TCORE_SAT_BEYOND_ME_CAPABILITY;
		break;
		}
		temp_index++;
	}

	 /*  1 is the length of Tag. */
	*consumed_data_len = 1  +  len_of_len  +  evt_list_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_icon_identifier_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_icon_identifier *icon_id_obj, int *consumed_data_len)
{
	unsigned char *src_data;
	int temp_index = 0;

	if (tlv_str == NULL || icon_id_obj == NULL || consumed_data_len == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || icon_id_obj == NULL ||consumed_data_len == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {/* length of icon id tlv is 4 */
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_ICON_IDENTIFIER_TAG) {
		dbg("[SAT] SAT PARSER -  icon identity tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	if (src_data[temp_index++] != SATK_ICON_IDENTITY_LENGTH) {
		dbg("[SAT] SAT PARSER -  incorrect length value.");
		return FALSE; /* Send TR */
	}

	if ((temp_index + SATK_ICON_IDENTITY_LENGTH) > tlv_len) {
		dbg("[SAT] SAT PARSER -  incorrect cmd len, expected len = %d, orig_len=%d",
			(temp_index + SATK_ICON_IDENTITY_LENGTH), tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	icon_id_obj->is_exist = TRUE;

	if ((src_data[temp_index++] & 0x01))
		icon_id_obj->icon_qualifer = ICON_QUALI_NOT_SELF_EXPLANATORY;
	else
		icon_id_obj->icon_qualifer = ICON_QUALI_SELF_EXPLANATORY;

	if (src_data[temp_index] > 0x00)
		icon_id_obj->icon_identifier = src_data[temp_index];
	else {
		dbg("[SAT] SAT PARSER -  incorrect icon identifier");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	dbg("[SAT] SAT PARSER -  icon_qual=%d, iconId=%d", icon_id_obj->icon_qualifer, icon_id_obj->icon_identifier);
	*consumed_data_len = 4;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_icon_identifier_list_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_icon_identifier_list *icon_list_obj, int *consumed_data_len)
{
	int temp_index, i;
	int len_value = 0;
	unsigned char *src_data;
	gboolean comprehension_req = FALSE;

	if (tlv_str == NULL || consumed_data_len == NULL || icon_list_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || icon_list_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1) + 1) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index] & 0x7F) != SATK_ITEM_ICON_IDENTIFIER_LIST_TAG) {
		dbg("[SAT] SAT PARSER -  icon identity tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	icon_list_obj->is_exist = TRUE;
	if (src_data[temp_index++] & 0x80)
		comprehension_req = TRUE;

	len_value = src_data[temp_index++];
	if (src_data[temp_index++] & 0x01)
		icon_list_obj->icon_qualifer = ICON_QUALI_NOT_SELF_EXPLANATORY;
	else
		icon_list_obj->icon_qualifer = ICON_QUALI_SELF_EXPLANATORY;

	icon_list_obj->icon_cnt = len_value-1;
	if (icon_list_obj->icon_cnt > SAT_ICON_LIST_MAX_COUNT) {
		if (comprehension_req == TRUE) {
			dbg("[SAT] SAT PARSER -  list count exceeds maximum allowed count=%d", icon_list_obj->icon_cnt);
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
		}

		icon_list_obj->icon_cnt = 0;
	} else {

		for (i = 0; i < icon_list_obj->icon_cnt; i++) {
			if (src_data[temp_index] > 0x00) {
				icon_list_obj->icon_id_list[i] = src_data[temp_index++];
			} else {
				dbg("[SAT] SAT PARSER -  incorrect icon identifier");
				return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
			}
		}
	}

	*consumed_data_len = 1 + 1 + len_value;
	dbg("[SAT] SAT PARSER -  icon_qual=%d, iconCount=%d", icon_list_obj->icon_qualifer, icon_list_obj->icon_cnt);
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_dtmf_string_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_dtmf_string *dtmf_string_obj, int *consumed_data_len)
{
	unsigned char *src_data;
	int temp_index, len_of_len = 0;
	int dtmf_len = 0;
	gboolean comprehension_req = FALSE;
	char *str_ascii = NULL;

	if (tlv_str == NULL || consumed_data_len == NULL || dtmf_string_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || dtmf_string_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	src_data = &tlv_str[0];

	if (tlv_len <= (curr_offset  +  1)) {
		dbg("[SAT] SAT PARSER - incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* Tag */
	temp_index = curr_offset;
	if ((src_data[temp_index] & 0x7F) != SATK_DTMF_STRING_TAG) {
		dbg("[SAT] SAT PARSER - address tag missing");
		return TCORE_SAT_REQUIRED_VALUE_MISSING;
	}

	/* comprehensive required */
	if ((src_data[temp_index++] & 0x80))
		comprehension_req = TRUE;

	dbg("comprehension_req=[%d]", comprehension_req);

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		err("[SAT] parser: invalid length.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	dtmf_len = src_data[temp_index  +  len_of_len - 1];
	temp_index += len_of_len; /* temp_index pointing to TON/NPI */

	if ((temp_index  +  dtmf_len) > tlv_len) {
		dbg("[SAT] SAT PARSER - incorrect cmd len, expected len = %d, orig_len=%d",
			(temp_index + dtmf_len), tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	dtmf_string_obj->dtmf_length = 0;

	if (dtmf_len > 0) {
		str_ascii = tcore_util_convert_bcd2ascii((const char *)&src_data[temp_index], dtmf_len, SAT_DTMF_STRING_LEN_MAX);
		if (str_ascii) {
			memcpy(dtmf_string_obj->dtmf_string, str_ascii, strlen(str_ascii));
			dtmf_string_obj->dtmf_length = strlen(str_ascii);
			g_free(str_ascii);
		}
	}

	if (dtmf_string_obj->dtmf_length == 0) {
		dbg("[SAT] SAT PARSER - DTMF string length is either 0 or it is too long for the ME to handle.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	*consumed_data_len = 1  +  len_of_len  +  dtmf_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_language_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, enum tel_sim_language_type *language_obj)
{
	unsigned char *src_data;
	int temp_index = 0;

	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	src_data = &tlv_str[0];
	temp_index = curr_offset;

	if ((src_data[temp_index++] & 0x7F) != SATK_LANGUAGE_TAG)	{
		dbg("[SAT] SAT PARSER -  Language tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (src_data[temp_index++] != SATK_LANGUAGE_LENGTH) {
		dbg("[SAT] SAT PARSER -  incorrect length value.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if ((temp_index + SATK_LANGUAGE_LENGTH) > tlv_len) {
		dbg("[SAT] SAT PARSER -  incorrect cmd len, expected len = %d, orig_len=%d",
			(temp_index + SATK_LANGUAGE_LENGTH), tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	*language_obj = _sat_decode_language(src_data[temp_index], src_data[temp_index + 1]);
	dbg("[SAT] SAT PARSER -  <in> %c %c, <out> %d", src_data[temp_index], src_data[temp_index + 1], *language_obj);
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_browser_identity_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, enum browser_identity *browser_id, int *consumed_data_len)
{
	unsigned char *src_data;
	int temp_index = 0;

	if (tlv_str == NULL || browser_id == NULL || consumed_data_len == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || browser_id == NULL ||consumed_data_len == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset  +  1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	temp_index = curr_offset;
	src_data = &tlv_str[0];

	if ((src_data[temp_index++] & 0x7F) != SATK_BROWSER_IDENTITY_TAG) {
		dbg("[SAT] SAT PARSER -  Browser ID tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (src_data[temp_index++] != SATK_BROWSER_ID_LENGTH) {
		dbg("[SAT] SAT PARSER -  incorrect length value.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	dbg("[SAT] SAT PARSER - : browser ID value:ox%x", src_data[temp_index]);

	switch (src_data[temp_index]) {
	case 0x00:
		*browser_id = BROWSER_ID_DEFAULT;
	break;

	case 0x01:
		*browser_id = BROWSER_ID_WML;
	break;

	case 0x02:
		*browser_id = BROWSER_ID_HTML;
	break;

	case 0x03:
		*browser_id = BROWSER_ID_XHTML;
	break;

	case 0x04:
		*browser_id = BROWSER_ID_CHTML;
	break;

	default:
		*browser_id = BROWSER_ID_RESERVED;
	break;
	}

	*consumed_data_len = 3;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_url_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_url *url, int *consumed_data_len)
{
	unsigned char *src_data;
	int temp_index = curr_offset;
	int len_of_len = 0, url_len = 0;

	if (tlv_str == NULL || url == NULL || consumed_data_len == NULL)	{
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || url == NULL ||consumed_data_len == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	src_data = &tlv_str[0];
	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if ((src_data[temp_index++] & 0x7F) != SATK_URL_TAG) {
		dbg("[SAT] SAT PARSER -  Browser URL tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		err("[SAT] parser: invalid length.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	url_len =  src_data[temp_index + len_of_len-1];
	url->url_length = url_len;
	temp_index += len_of_len; /* temp_index pointing to url. */
	dbg("URL length (%d)", url_len);

	if (url_len < 0) {
		dbg("[SAT] URL is null");
		*consumed_data_len = 1 + len_of_len + url_len;
		return TCORE_SAT_SUCCESS;
	}

	if (url_len > SAT_URL_LEN_MAX) {
		dbg("[SAT] URL length is wrong");
		*consumed_data_len = 1 + len_of_len + url_len;
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	memcpy(url->url, &src_data[temp_index], url_len);
	dbg("[SAT] url(%s)", url->url);
	*consumed_data_len = 1 + len_of_len + url_len;

	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_bearer_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_bearer_list *satk_bearer, int *consumed_data_len)
{
	unsigned char *src_data;
	int temp_index, len_of_len = 0;
	int list_len = 0, list_idx = 0;

	if (tlv_str == NULL || consumed_data_len == NULL || satk_bearer == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || satk_bearer == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset  +  1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d",
			tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	src_data = &tlv_str[0];
	temp_index = curr_offset;

	if ((src_data[temp_index++] & 0x7F) != SATK_BEARER_TAG) {
		dbg("[SAT] SAT PARSER - _sat_decode_bearer_tlv: alphaID TAG missing");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		err("[SAT] parser: invalid length.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	satk_bearer->count = src_data[temp_index  +  len_of_len - 1];
	list_len = satk_bearer->count;
	temp_index += len_of_len;

	if ((temp_index  +  list_len) > tlv_len) {
		dbg("[SAT] SAT PARSER -  incorrect cmd len, expected len = %d, orig_len=%d",
			(temp_index  +  list_len), tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (satk_bearer->count > 0) {
		if (list_len > SAT_BEARER_LIST_MAX_COUNT)
			list_len = SAT_BEARER_LIST_MAX_COUNT;

		for (list_idx = 0; list_idx < list_len; list_idx++) {
			switch (src_data[temp_index]) {
			case 0x00:
				satk_bearer->bear[list_idx] = BEARER_LIST_SMS;
			break;

			case 0x01:
				satk_bearer->bear[list_idx] = BEARER_LIST_CSD;
			break;

			case 0x02:
				satk_bearer->bear[list_idx] = BEARER_LIST_USSD;
			break;

			case 0x03:
				satk_bearer->bear[list_idx] = BEARER_LIST_GPRS;
			break;

			default:
				satk_bearer->bear[list_idx] = BEARER_LIST_RESERVED;
			break;
			}

			dbg("[SAT] SAT PARSER -  bearer[%d]=0x%x", list_idx, satk_bearer->bear[list_idx]);
			temp_index++;
		}
	} else
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;

	*consumed_data_len = 1  +  len_of_len  +  list_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_provisioning_file_ref_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_provisioning_file_ref *prf, int *data_len_consumed)
{
	unsigned char *src_data;
	int temp_index = curr_offset;
	int len_of_len = 0, prf_len = 0;

	if (tlv_str == NULL || prf == NULL || data_len_consumed == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || prf == NULL ||data_len_consumed == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	src_data = &tlv_str[0];
	if (tlv_len <= (curr_offset  +  1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if ((src_data[temp_index++] & 0x7F) != SATK_PROVISIONING_REF_FILE_TAG) {
		dbg("[SAT] SAT PARSER -  PRF tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		err("[SAT] parser: invalid length.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	prf_len = src_data[temp_index  +  len_of_len - 1];
	prf->file_path_length = prf_len;
	temp_index += len_of_len; /* temp_index pointing to prf. */

	if (prf_len > 0) {
		if (prf_len > SAT_PROVISIONING_FILE_PATH_LEN_MAX)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
		else
			memcpy(prf->file_path, &src_data[temp_index], prf_len);
	} else {
		dbg("[SAT] SAT PARSER -  NULL string for PRF");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	*data_len_consumed = 1  +  len_of_len  +  prf_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_bearer_description_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_bearer_description *bearer_desc_obj, int *consumed_data_len)
{
	int temp_index, length = 0;
	unsigned char *src_data;

	if (tlv_len <= (curr_offset + 1) + 1) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	temp_index = curr_offset;
	src_data = &tlv_str[0];
	while (1) {
		if (temp_index >= tlv_len) {
			dbg("bearer desc cannot find. UICC Server mode");
			*consumed_data_len = 0;
			return TCORE_SAT_SUCCESS;
		}

		if ((src_data[temp_index] & 0x7F) == SATK_BEARER_DISCRIPTION_TAG) {
			dbg("find bearer description tag temp_index(%d)", temp_index);
			temp_index++;
			break;
		}

		temp_index++;
	}

	/* length */
	length = src_data[temp_index++];
	dbg("bearer description length (%d)", length);

	/* bearer parameter */
	switch (src_data[temp_index++]) {
	case BEARER_CSD:
		bearer_desc_obj->bearer_type = BEARER_CSD;
		bearer_desc_obj->bearer_parameter.cs_bearer_param.data_rate = src_data[temp_index++];
		bearer_desc_obj->bearer_parameter.cs_bearer_param.service_type = src_data[temp_index++];
		bearer_desc_obj->bearer_parameter.cs_bearer_param.connection_element_type = src_data[temp_index++];
	break;

	case BEARER_GPRS:
		bearer_desc_obj->bearer_type = BEARER_GPRS;
		bearer_desc_obj->bearer_parameter.ps_bearer_param.precedence_class = src_data[temp_index++];
		bearer_desc_obj->bearer_parameter.ps_bearer_param.delay_class = src_data[temp_index++];
		bearer_desc_obj->bearer_parameter.ps_bearer_param.reliability_class = src_data[temp_index++];
		bearer_desc_obj->bearer_parameter.ps_bearer_param.peak_throughput_class = src_data[temp_index++];
		bearer_desc_obj->bearer_parameter.ps_bearer_param.mean_throughput_class = src_data[temp_index++];
		bearer_desc_obj->bearer_parameter.ps_bearer_param.pdp_type = BIP_GPRS_PDP_TYPE_RESERVED;
		if (src_data[temp_index] == BIP_GPRS_PDP_TYPE_IP)
			bearer_desc_obj->bearer_parameter.ps_bearer_param.pdp_type = BIP_GPRS_PDP_TYPE_IP;
	break;

	case BEARER_DEFAULT_BEARER_FROM_TRANSPORT_LAYER:
		bearer_desc_obj->bearer_type = BEARER_DEFAULT_BEARER_FROM_TRANSPORT_LAYER;
	break;

	case BEARER_LOCAL_LINK_TECHNOLOGY_INDEPENDENT:
		bearer_desc_obj->bearer_type = BEARER_DEFAULT_BEARER_FROM_TRANSPORT_LAYER;
	break;

	default:
		bearer_desc_obj->bearer_type = BEARER_RESERVED;
		dbg("bearer type not supported");
		return TCORE_SAT_BEYOND_ME_CAPABILITY;
	}

	*consumed_data_len = 1 + 1 + length;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_channel_data_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_channel_data *channel_data_obj, int *consumed_data_len)
{
	int temp_index = 0;
	int len_of_len = 0, channel_data_len = 0;
	unsigned char *src_data;

	if (tlv_str == NULL || consumed_data_len == NULL || channel_data_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || channel_data_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER - incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_CHANNEL_DATA_TAG) {
		dbg("[SAT] SAT PARSER - tag not found.=%d", src_data[temp_index]);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		err("[SAT] parser: invalid length.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	channel_data_len = src_data[temp_index + len_of_len-1];
	dbg("[SAT] parser: channel_data_len=%d", channel_data_len);
	temp_index += len_of_len;

	if ((temp_index + channel_data_len) > tlv_len) {
		dbg("[SAT] SAT PARSER - incorrect cmd len, expected len = %d, orig_len=%d", (temp_index + channel_data_len), tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* data */
	channel_data_obj->data_string_len = channel_data_len;
	memcpy(channel_data_obj->data_string, &src_data[temp_index], channel_data_len);

	*consumed_data_len = 1 + len_of_len + channel_data_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_channel_data_length_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_channel_data_len *data_len_obj, int *consumed_data_len)
{
	int temp_index;
	unsigned char *src_data;

	if (tlv_str == NULL || consumed_data_len == NULL || data_len_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || data_len_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_CHANNEL_DATA_LEN_TAG) {
		dbg("[SAT] SAT PARSER -  channel data tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	/* length */
	if (src_data[temp_index++] != SATK_CHANNEL_DATA_LENGTH_VALUE_LENGTH) {
		dbg("[SAT] SAT PARSER -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	/* data */
	data_len_obj->data_len = src_data[temp_index];

	*consumed_data_len = 3;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_buffer_size_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_buffer_size *buffer_size_obj, int *consumed_data_len)
{
	int temp_index;
	unsigned char *src_data;

	if (tlv_str == NULL || consumed_data_len == NULL || buffer_size_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || buffer_size_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1) + SATK_BUFFER_SIZE_LENGTH) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_BUFFER_SIZE_TAG) {
		dbg("[SAT] SAT PARSER -  buffer size tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	/* length */
	if (src_data[temp_index++] != SATK_BUFFER_SIZE_LENGTH) {
		dbg("[SAT] SAT PARSER -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	buffer_size_obj->size[0] = src_data[temp_index++];
	buffer_size_obj->size[1] = src_data[temp_index];

	*consumed_data_len = 4;
	dbg("[SAT] SAT PARSER -  buffer size = 0x%x%x", buffer_size_obj->size[0], buffer_size_obj->size[1]);
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_other_address_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_other_address *other_address_obj, int *consumed_data_len)
{
	gchar *address = NULL;
	int temp_index, address_len;
	unsigned char *src_data;

	if (tlv_str == NULL || consumed_data_len == NULL || other_address_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || other_address_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_OTHER_ADDRESS_TAG) {
		dbg("[SAT] SAT PARSER -  other address tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	/* length */
	address_len = src_data[temp_index++];
	if ((temp_index + address_len) > tlv_len) {
		dbg("[SAT] SAT PARSER -  incorrect cmd len, expected len = %d, orig_len=%d", (temp_index + address_len), tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (address_len-1 > SAT_OTHER_ADDR_LEN_MAX) {
		dbg("[SAT] SAT PARSER - address is longer than capability");
		return TCORE_SAT_BEYOND_ME_CAPABILITY;
	}

	/* other address type */
	switch (src_data[temp_index++]) {
		case ADDR_TYPE_IPv4: {
			other_address_obj->address_type = ADDR_TYPE_IPv4;
			address = g_strdup_printf("%d.%d.%d.%d", src_data[temp_index], src_data[temp_index + 1], src_data[temp_index + 2], src_data[temp_index + 3]);
		}
		break;

		case ADDR_TYPE_IPv6: {
			other_address_obj->address_type = ADDR_TYPE_IPv6;
			address = g_strdup_printf("%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:",
					src_data[temp_index], src_data[temp_index + 1], src_data[temp_index + 2], src_data[temp_index + 3],
					src_data[temp_index + 4], src_data[temp_index + 5], src_data[temp_index + 6], src_data[temp_index + 7],
					src_data[temp_index + 8], src_data[temp_index + 9], src_data[temp_index + 10], src_data[temp_index + 11],
					src_data[temp_index + 12], src_data[temp_index + 13], src_data[temp_index + 14], src_data[temp_index + 15]);
		}
		break;

		default: {
			other_address_obj->address_type = ADDR_RESERVED;
			address = g_strdup("");
		}
		break;
	} /* end of switch */

	/* address */
	if (address) {
		memcpy(other_address_obj->address, address, strlen(address));
		other_address_obj->address_len = strlen(address);

		g_free(address);
		dbg("destination address(%s)", other_address_obj->address);
	}

	*consumed_data_len = 2 + address_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_uicc_terminal_interface_tlv(unsigned char *tlv_str,
		int tlv_len, int curr_offset, struct tel_sat_uicc_terminal_interface_transport_level *level_obj,
		int *consumed_data_len)
{
	int temp_index;
	unsigned char *src_data;

	if (tlv_str == NULL || consumed_data_len == NULL || level_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || level_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1) + SATK_UICC_ME_TRANS_INTERFACE_LEVEL_LENGTH) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG) {
		dbg("[SAT] SAT PARSER - UICC/TERMINAL Interface transport level tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	/* length */
	if (src_data[temp_index++] != SATK_UICC_ME_TRANS_INTERFACE_LEVEL_LENGTH) {
		dbg("[SAT] SAT PARSER -  incorrect length");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	level_obj->protocol_type = src_data[temp_index++];
	level_obj->port_number = src_data[temp_index++] << 8;
	level_obj->port_number |= src_data[temp_index];

	*consumed_data_len = 2 + SATK_UICC_ME_TRANS_INTERFACE_LEVEL_LENGTH;
	dbg("[SAT] SAT PARSER -  protocol type(%d) , port number(%d)", level_obj->protocol_type, level_obj->port_number);
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_remote_entity_address_tlv(unsigned char *tlv_str,
		int tlv_len, int curr_offset, struct tel_sat_remote_entity_address *remote_address_obj,
		int *consumed_data_len)
{
	int temp_index = 0;
	int len_of_len = 0, remote_data_len = 0;
	unsigned char *src_data;

	if (tlv_str == NULL || consumed_data_len == NULL || remote_address_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || remote_address_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER - incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index] & 0x7F) != SATK_REMOTE_ENTITY_ADDRESS_TAG) {
		dbg("[SAT] SAT PARSER - tag not found.=%d", src_data[temp_index]);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* length */
	len_of_len = _get_length_filed_size(src_data[temp_index]);
	if (!len_of_len) {
		err("[SAT] parser: invalid length.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	remote_data_len = src_data[temp_index + len_of_len-1];
	dbg("[SAT] parser: remote_data_len=%d", remote_data_len);
	temp_index += len_of_len;

	/* data */
	switch (src_data[temp_index++]) {
	case REMOTE_ENTITY_ADDR_CODING_TYPE_IEEE802_48BIT:
		remote_address_obj->coding_type = REMOTE_ENTITY_ADDR_CODING_TYPE_IEEE802_48BIT;
	break;

	case REMOTE_ENTITY_ADDR_CODING_TYPE_IRDA_32BIT:
		remote_address_obj->coding_type = REMOTE_ENTITY_ADDR_CODING_TYPE_IRDA_32BIT;
	break;

	default:
		remote_address_obj->coding_type = REMOTE_ENTITY_ADDR_CODING_TYPE_RESERVED;
	break;
	}

	remote_address_obj->length = remote_data_len - 1;
	memcpy(remote_address_obj->remote_entity_address, &src_data[temp_index], remote_address_obj->length);

	*consumed_data_len = 1 + len_of_len + remote_data_len;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_network_access_name_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_network_access_name *access_name_obj, int *consumed_data_len)
{
	int temp_index, idx, name_idx, name_length;
	unsigned char *src_data;

	if (tlv_str == NULL || consumed_data_len == NULL || access_name_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || access_name_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_NETWORK_ACCESS_TAG) {
		dbg("[SAT] SAT PARSER -  network access name tag missing.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	/* length */
	name_length = src_data[temp_index++];
	if ((temp_index + name_length) > tlv_len) {
		dbg("[SAT] SAT PARSER -  incorrect cmd len, expected len = %d, orig_len=%d",
			(temp_index + name_length), tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	access_name_obj->length = name_length;
	if (access_name_obj->length > SAT_NET_ACC_NAM_LEN_MAX) {
		dbg("[SAT] SAT PARSER - network access name is longer than capability");
		return TCORE_SAT_BEYOND_ME_CAPABILITY;
	}

	name_idx = 0;
	for (idx = 0; idx < access_name_obj->length; idx++) {
		dbg("data (%c) Bool(%d)", src_data[temp_index], g_ascii_isalpha(src_data[temp_index]));

		if (g_ascii_isalpha(src_data[temp_index])) {
			access_name_obj->network_access_name[name_idx] = src_data[temp_index];
			name_idx++;
		} else {
			if (src_data[temp_index] == 0x02) {/* 02 convert to "." */
				access_name_obj->network_access_name[name_idx] = '.';
				name_idx++;
			}
		}

		temp_index++;
	}

	/* network access name */
	dbg("network access name(%s)", access_name_obj->network_access_name);

	*consumed_data_len = 2 + name_length;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_text_attribute_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_text_attribute *text_attribute_obj, int *consumed_data_len)
{
	int temp_index, length;
	unsigned char *src_data;

	if (tlv_str == NULL || consumed_data_len == NULL || text_attribute_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || text_attribute_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER - incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	src_data = &tlv_str[0];
	if ((src_data[temp_index++] & 0x7F) != SATK_TEXT_ATTRIBUTE_TAG) {
		dbg("[SAT] SAT PARSER - text attribute tag is missing");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	/* length */
	length = src_data[temp_index++];
	if ((temp_index + length) > tlv_len) {
		dbg("[SAT] SAT PARSER -  incorrect cmd len, expected len = %d, orig_len=%d", (temp_index + length), tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* attribute data */
	text_attribute_obj->b_txt_attr = TRUE;
	memcpy(text_attribute_obj->text_formatting, &src_data[temp_index], length);

	*consumed_data_len = 2 + length;
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_application_identifier_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_application_identifier *aid_obj, int *consumed_data_len)
{
	int temp_index = 0;

	if (tlv_str == NULL || consumed_data_len == NULL || aid_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || aid_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 1)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	if ((tlv_str[temp_index++] & 0x7F) != SATK_AID_TAG) {
		dbg("[SAT] SAT PARSER -  AID tag missing.");
		return TCORE_SAT_REQUIRED_VALUE_MISSING; //send TR
	}

	if (tlv_str[temp_index] != SATK_AID_LENGTH) {
		dbg("[SAT] SAT PARSER -  AID length incorrect.");
		return TCORE_SAT_REQUIRED_VALUE_MISSING; //send TR
	}

	/* length */
	aid_obj->length = tlv_str[temp_index++];
	if (aid_obj->length > SAT_AID_LEN_MAX) {
		dbg("[SAT] SAT PARSER -  AID length is longer than SAT_AID_LEN_MAX");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (aid_obj->length > 0) {
		aid_obj->b_is_exist = TRUE;
		memcpy(aid_obj->identifier, &tlv_str[temp_index], aid_obj->length);
	}

	*consumed_data_len = 1 + 1 + aid_obj->length;

	dbg("[SAT] SAT PARSER -  length=%d, RID[%x%x%x%x%x]",
		aid_obj->length, aid_obj->identifier[0],
		aid_obj->identifier[1], aid_obj->identifier[2],
		aid_obj->identifier[3], aid_obj->identifier[4]);
	return TCORE_SAT_SUCCESS;
}

static enum tcore_sat_result _sat_decode_frame_identifier_tlv(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tel_sat_frame_identifier *frame_id_obj, int *consumed_data_len)
{
	int temp_index = 0;

	if (tlv_str == NULL || consumed_data_len == NULL || frame_id_obj == NULL) {
		dbg("[SAT] SAT PARSER -  tlv_str == NULL || consumed_data_len == NULL || frame_id_obj == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_len <= (curr_offset + 2)) {
		dbg("[SAT] SAT PARSER -  incorrect length original_command_len=%d", tlv_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	/* tag */
	temp_index = curr_offset;
	if ((tlv_str[temp_index++] & 0x7F) != SATK_FRAME_IDENTIFIER_TAG) {
		dbg("[SAT] SAT PARSER -  frame id tag missing.");
		return TCORE_SAT_REQUIRED_VALUE_MISSING; //send TR
	}

	if (tlv_str[temp_index++] != SATK_FRAME_IDENTIFIER_LENGTH) {
		dbg("[SAT] SAT PARSER -  frame id length incorrect.");
		return TCORE_SAT_REQUIRED_VALUE_MISSING; //send TR
	}

	/* value */
	frame_id_obj->b_is_exist = TRUE;
	frame_id_obj->identifier = tlv_str[temp_index++];

	*consumed_data_len = 1 + 1 + 1;
	dbg("[SAT] SAT PARSER -  frame id = 0x%x", frame_id_obj->identifier);
	return TCORE_SAT_SUCCESS;
}

/*
 * decode proactive cmd
 */
/*
 * 6.4.1 DISPLAY TEXT
 */
static enum tcore_sat_result _sat_decode_display_text(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	int tlv_length = 0, remain_len = 0, expected_len = 0;
	int data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	tlv_length = cmd_data[curr_offset-1];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.display_text.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.display_text.command_detail.cmd_type = cmd_data[temp_index++];

	/* decode command qualifier */
	if (cmd_data[temp_index] & 0x01) {
		sat_cmd_ind_data->data.display_text.command_detail.cmd_qualifier.display_text.text_priority =
				TEXT_PRIORITY_HIGH;
		dbg("[SAT] SAT PARSER -  msg_prio=TAPI_SAT_MSG_PRIORITY_HIGH.");
	} else {
		sat_cmd_ind_data->data.display_text.command_detail.cmd_qualifier.display_text.text_priority =
				TEXT_PRIORITY_NORMAL;
		dbg("[SAT] SAT PARSER - : msg_prio=TAPI_SAT_MSG_PRIORITY_NORMAL.");
	}

	if (cmd_data[temp_index] & 0x80) {
		sat_cmd_ind_data->data.display_text.command_detail.cmd_qualifier.display_text.text_clear_type =
				TEXT_WAIT_FOR_USER_TO_CLEAR_MSG;
		dbg("[SAT] SAT PARSER - : msgClear=TAPI_SAT_WAIT_FOR_USER_TO_CLEAR_MSG.");
	} else {
		sat_cmd_ind_data->data.display_text.command_detail.cmd_qualifier.display_text.text_clear_type =
				TEXT_AUTO_CLEAR_MSG_AFTER_A_DELAY;
		dbg("[SAT] SAT PARSER -  msgClear=TAPI_SAT_AUTO_CLEAR_MSG_AFTER_A_DELAY.");
	}

	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.display_text.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	temp_index += 4; /* device identities consumes 4 bytes. */

	remain_len = o_length-temp_index;
	expected_len = tlv_length-5-4;
	if (remain_len != expected_len) {
		dbg("[SAT] SAT PARSER - : mismatch!! remain_len=%d, expected_len=%d", remain_len, expected_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	rv = _sat_decode_text_tlv(o_cmd_data, o_length, temp_index,
		&sat_cmd_ind_data->data.display_text.text,
		&data_len_consumed);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	if (sat_cmd_ind_data->data.display_text.text.string_length <= 0) {
		err("[SAT] SAT PARSER - :string length is 0");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	dbg("[SAT] SAT PARSER  text(%s)", sat_cmd_ind_data->data.display_text.text.string);
	dbg("[SAT] SAT PARSER o_len(%d) temp_index(%d) data_len_consumed(%d)",
		o_length , temp_index, data_len_consumed);

	if (o_length-temp_index < data_len_consumed) {
		dbg("[SAT] SAT PARSER - :wrong text TLV.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (o_length-temp_index == data_len_consumed) {
		dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
		return TCORE_SAT_SUCCESS;
	}

	temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */

	/* icon identifier */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.display_text.icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		if (o_length-temp_index == data_len_consumed) {
			dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* immediate response */
	sat_cmd_ind_data->data.display_text.immediate_response_requested = FALSE;
	if ((cmd_data[temp_index] & 0x7F) == SATK_IMMEDIATE_RESPONSE_TAG) {
		data_len_consumed = 2;
		dbg("[SAT] SAT PARSER - :immediate response required.");
		sat_cmd_ind_data->data.display_text.immediate_response_requested = TRUE;
		if (o_length-temp_index == data_len_consumed) {
			dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* time duration - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_DURATION_TAG) {
		rv = _sat_decode_duration_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.display_text.duration,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		if (o_length-temp_index == data_len_consumed) {
			dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* text attribute - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_ATTRIBUTE_TAG) {
		rv = _sat_decode_text_attribute_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.display_text.text_attribute,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		if (o_length-temp_index == data_len_consumed) {
			dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	if (o_length > temp_index) {
		dbg("[SAT] SAT PARSER - : wrong text TLV, remaining data is found!!");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.2 GET INKEY
 */
static enum tcore_sat_result _sat_decode_get_inkey(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	int tlv_length = 0, remain_len = 0, expected_len = 0;
	int data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	tlv_length = cmd_data[curr_offset-1];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.get_inkey.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.get_inkey.command_detail.cmd_type = cmd_data[temp_index++];

	/* decode command qualifier */
	if (cmd_data[temp_index] & 0x01) {
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.alphabet_set = TRUE;
		dbg("[SAT] SAT PARSER - Alphabet set");
	}

	if (cmd_data[temp_index] & 0x02) {
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.alphabet_type = INPUT_ALPHABET_TYPE_UCS2;
		dbg("[SAT] SAT PARSER -  INPUT_ALPHABET_TYPE_UCS2");
	} else {
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.alphabet_type = INPUT_ALPHABET_TYPE_SMS_DEFAULT;
		dbg("[SAT] SAT PARSER -  INPUT_ALPHABET_TYPE_SMS_DEFAULT");
	}

	if (cmd_data[temp_index] & 0x04) {
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.inkey_type = INKEY_TYPE_YES_NO_REQUESTED;
		dbg("[SAT] SAT PARSER -  INKEY_TYPE_YES_NO_REQUESTED");
	} else {
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.inkey_type = INKEY_TYPE_CHARACTER_SET_ENABLED;
		dbg("[SAT] SAT PARSER -  INKEY_TYPE_YES_NO_REQUESTED");
	}

	if (cmd_data[temp_index] & 0x08) {
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.immediate_rsp_required = TRUE;
		dbg("[SAT] SAT PARSER -  immediate response requested");
	}

	if (cmd_data[temp_index] & 0x80) {
		sat_cmd_ind_data->data.get_inkey.command_detail.cmd_qualifier.get_inkey.help_info = TRUE;
		dbg("[SAT] SAT PARSER - Help info");
	}

	/* device identities */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.get_inkey.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* text */
	temp_index += 4;

	remain_len = o_length-temp_index;
	expected_len = tlv_length-5-4;
	if (remain_len != expected_len) {
		dbg("[SAT] SAT PARSER - : mismatch!! remain_len=%d, expected_len=%d", remain_len, expected_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	rv = _sat_decode_text_tlv(o_cmd_data, o_length, temp_index,
		&sat_cmd_ind_data->data.get_inkey.text,
		&data_len_consumed);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	if (sat_cmd_ind_data->data.get_inkey.text.string_length <= 0) {
		err("[SAT] SAT PARSER - :string length is 0");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	dbg("[SAT] SAT PARSER  text(%s)", sat_cmd_ind_data->data.get_inkey.text.string);
	dbg("[SAT] SAT PARSER o_len(%d) temp_index(%d) data_len_consumed(%d)",
		o_length , temp_index, data_len_consumed);

	if (o_length-temp_index < data_len_consumed) {
		dbg("[SAT] SAT PARSER - :wrong text TLV.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (o_length-temp_index == data_len_consumed) {
		dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
		return TCORE_SAT_SUCCESS;
	}

	temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */

	/* icon identifier - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.get_inkey.icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		if (o_length-temp_index == data_len_consumed) {
			dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* time duration - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_DURATION_TAG) {
		rv = _sat_decode_duration_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.get_inkey.duration,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		if (o_length-temp_index == data_len_consumed) {
			dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* text attribute - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_ATTRIBUTE_TAG) {
		rv = _sat_decode_text_attribute_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.get_inkey.text_attribute,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		if (o_length-temp_index == data_len_consumed) {
			dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	if (o_length > temp_index) {
		dbg("[SAT] SAT PARSER - : wrong text TLV, remaining data is found!!");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.3 GET INPUT
 */
static enum tcore_sat_result _sat_decode_get_input(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	int tlv_length = 0, remain_len = 0, expected_len = 0;
	int data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	tlv_length = cmd_data[curr_offset-1];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.get_input.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.get_input.command_detail.cmd_type = cmd_data[temp_index++];

	/* decode command qualifier */
	if (cmd_data[temp_index] & 0x01) {
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.alphabet_set = TRUE;
		dbg("[SAT] SAT PARSER - Alphabet set");
	} else {
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.alphabet_set = FALSE;
		dbg("[SAT] SAT PARSER - Numeric info");
	}

	if (cmd_data[temp_index] & 0x02) {
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.alphabet_type = INPUT_ALPHABET_TYPE_UCS2;
		dbg("[SAT] SAT PARSER -  INPUT_ALPHABET_TYPE_UCS2");
	} else {
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.alphabet_type = INPUT_ALPHABET_TYPE_SMS_DEFAULT;
		dbg("[SAT] SAT PARSER -  INPUT_ALPHABET_TYPE_SMS_DEFAULT");
	}

	if (cmd_data[temp_index] & 0x04) {
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.me_echo_user_input = FALSE;
		dbg("[SAT] SAT PARSER -  user input not be revealed");
	} else {
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.me_echo_user_input = TRUE;
		dbg("[SAT] SAT PARSER -  Me echo user input");
	}

	if (cmd_data[temp_index] & 0x08) {
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.user_input_unpacked_format = FALSE;
		dbg("[SAT] SAT PARSER - packing required");
	} else {
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.user_input_unpacked_format = TRUE;
		dbg("[SAT] SAT PARSER - unpacked format");
	}

	if (cmd_data[temp_index] & 0x80) {
		sat_cmd_ind_data->data.get_input.command_detail.cmd_qualifier.get_input.help_info = TRUE;
		dbg("[SAT] SAT PARSER - Help info");
	}

	/* device identities */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.get_input.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* text - mandatory */
	temp_index += 4;

	remain_len = o_length-temp_index;
	expected_len = tlv_length - 5 - 4;
	if (remain_len != expected_len) {
		dbg("[SAT] SAT PARSER - : mismatch!! remain_len=%d, expected_len=%d", remain_len, expected_len);
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	rv = _sat_decode_text_tlv(o_cmd_data, o_length, temp_index,
		&sat_cmd_ind_data->data.get_input.text,
		&data_len_consumed);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	if (sat_cmd_ind_data->data.get_input.text.string_length <= 0)
		err("[SAT] SAT PARSER - :string length is 0");

	dbg("[SAT] SAT PARSER  text(%s)", sat_cmd_ind_data->data.get_input.text.string);
	dbg("[SAT] SAT PARSER o_len(%d) temp_index(%d) data_len_consumed(%d)",
		o_length , temp_index, data_len_consumed);

	if (o_length-temp_index < data_len_consumed) {
		dbg("[SAT] SAT PARSER - :wrong text TLV.");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	if (o_length-temp_index == data_len_consumed) {
		dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
		return TCORE_SAT_SUCCESS;
	}

	temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */

	/* response length - mandatory */
	rv = _sat_decode_response_length_tlv(o_cmd_data, o_length, temp_index,
		&sat_cmd_ind_data->data.get_input.rsp_len,
		&data_len_consumed);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	if (o_length-temp_index == data_len_consumed) {
		dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
		return TCORE_SAT_SUCCESS;
	}

	temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */

	/* default text - optional */
	if ((o_cmd_data[temp_index] & 0x7F) == SATK_DEFAULT_TEXT_TAG) {
		rv = _sat_decode_text_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.get_input.default_text,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		if (sat_cmd_ind_data->data.get_input.default_text.string_length <= 0)
			err("[SAT] SAT PARSER - :string length is 0");

		dbg("[SAT] SAT PARSER default text(%s)", sat_cmd_ind_data->data.get_input.default_text.string);
		if (o_length-temp_index == data_len_consumed) {
			dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* icon identifier */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.get_input.icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		if (o_length-temp_index == data_len_consumed) {
			dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}
		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* text attribute - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_ATTRIBUTE_TAG) {
		rv = _sat_decode_text_attribute_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.get_input.text_attribute,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		if (o_length-temp_index == data_len_consumed) {
			dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	if (o_length > temp_index) {
		dbg("[SAT] SAT PARSER - : wrong text TLV, remaining data is found!!");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.4 MORE TIME
 */
static enum tcore_sat_result _sat_decode_more_time(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* command detail */
	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.setup_event_list.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.setup_event_list.command_detail.cmd_type = cmd_data[temp_index++];

	/* device identifier */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.setup_event_list.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	temp_index += 4;
	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.5 PLAY TONE
 */
static enum tcore_sat_result _sat_decode_play_tone(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0, data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* command detail */
	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.play_tone.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.play_tone.command_detail.cmd_type = cmd_data[temp_index++];

	/* decode command qualifier */
	if (cmd_data[temp_index] & 0x01)
		sat_cmd_ind_data->data.play_tone.command_detail.cmd_qualifier.play_tone.vibration_alert = VIBRATE_ALERT_REQUIRED;
	else
		sat_cmd_ind_data->data.play_tone.command_detail.cmd_qualifier.play_tone.vibration_alert = VIBRATE_ALERT_OPTIONAL;

	/* device identifier */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.play_tone.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* alpha id - optional */
	temp_index += 4;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.play_tone.alpha_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			dbg("[SAT] SAT PARSER - default value is set - tone type, duration");
			sat_cmd_ind_data->data.play_tone.tone.tone_type = GENERAL_BEEP;
			sat_cmd_ind_data->data.play_tone.duration.time_unit = TIME_UNIT_SECONDS;
			sat_cmd_ind_data->data.play_tone.duration.time_interval = 2;

			return TCORE_SAT_SUCCESS;
		}
	}

	/* tone - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_TONE_TAG) {
		rv = _sat_decode_tone_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.play_tone.tone,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
	} else
		sat_cmd_ind_data->data.play_tone.tone.tone_type = GENERAL_BEEP;

	/* time duration - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_DURATION_TAG) {
		rv = _sat_decode_duration_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.play_tone.duration,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}
	} else {
		dbg("[SAT] SAT PARSER -  Duration TLV not present, ME should use a default value.");
		sat_cmd_ind_data->data.play_tone.duration.time_unit = TIME_UNIT_SECONDS;
		sat_cmd_ind_data->data.play_tone.duration.time_interval = 2;
	}

	/* icon identifier */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.play_tone.icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */

		if (temp_index + data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* text attribute - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_ATTRIBUTE_TAG) {
		rv = _sat_decode_text_attribute_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.play_tone.text_attribute,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		if (temp_index + data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/*
	* TODO:
	*	frames
	*/

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.7 REFRESH
 */
static enum tcore_sat_result _sat_decode_refresh(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0, data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* command detail */
	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.refresh.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.refresh.command_detail.cmd_type = cmd_data[temp_index++];

	/* decode command qualifier */
	switch (cmd_data[temp_index]) {
	case SIM_REFRESH_CMD_INIT_AND_FULL_FCN:
	case SIM_REFRESH_CMD_FCN:
	case SIM_REFRESH_CMD_INIT_AND_FCN:
	case SIM_REFRESH_CMD_INIT:
	case SIM_REFRESH_CMD_RESET:
		sat_cmd_ind_data->data.refresh.command_detail.cmd_qualifier.refresh.refresh = cmd_data[temp_index];
		dbg("[SAT] SAT PARSER - : refresh mode=[0x%02x]:0-init&FFCN, 1-FCN, 2-init&FCN, 3-init, 4-reset",
			cmd_data[temp_index]);
	break;

	case SIM_REFRESH_CMD_3G_APPLICATION_RESET:
	case SIM_REFRESH_CMD_3G_SESSION_RESET:
	case SIM_REFRESH_CMD_RESERVED:
	default:
		dbg("[SAT] SAT PARSER - : refresh mode=0x%02x Not Supported", cmd_data[temp_index]);
		return TCORE_SAT_BEYOND_ME_CAPABILITY;
	}

	/* device identifier */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.refresh.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* check file list */
	temp_index += 4;
	if ((sat_cmd_ind_data->data.refresh.command_detail.cmd_qualifier.refresh.refresh == SIM_REFRESH_CMD_FCN)
			|| (sat_cmd_ind_data->data.refresh.command_detail.cmd_qualifier.refresh.refresh == SIM_REFRESH_CMD_INIT_AND_FCN)) {
		rv = _sat_decode_file_list_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.refresh.file_list,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;
	} else {
		sat_cmd_ind_data->data.refresh.file_list.file_count = 0;
	}

	if (temp_index+data_len_consumed >= o_length) {
		dbg("[SAT] SAT PARSER - no more TLVs to decode.");
		return TCORE_SAT_SUCCESS;
	}

	/* AID - optional */
	if ((cmd_data[temp_index]&0x7F) == SATK_AID_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_application_identifier_tlv(o_cmd_data, o_length, temp_index, &sat_cmd_ind_data->data.refresh.aid, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;
		temp_index+=data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* alpha identifier - optional */
	if ((cmd_data[temp_index]&0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index, &sat_cmd_ind_data->data.refresh.alpha_id, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;
		temp_index+=data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* icon identifier - optional */
	if ((cmd_data[temp_index]&0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index, &sat_cmd_ind_data->data.refresh.icon_id, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;
		temp_index+=data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* text attribute - optional */
	if ((cmd_data[temp_index]&0x7F) == SATK_TEXT_ATTRIBUTE_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_text_attribute_tlv(o_cmd_data, o_length, temp_index, &sat_cmd_ind_data->data.refresh.text_attribute, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;
		temp_index+=data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* frame identifier - optional */
	if ((cmd_data[temp_index]&0x7F) == SATK_FRAME_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_frame_identifier_tlv(o_cmd_data, o_length, temp_index, &sat_cmd_ind_data->data.refresh.frame_id, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;
		temp_index+=data_len_consumed;
	}
	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.8 SETUP MENU
 */
static enum tcore_sat_result _sat_decode_setup_menu(unsigned char *tlv_str, int tlv_len,
	int curr_offset, struct tcore_sat_proactive_command *pactive_cmd_ind_obj)
{
	int temp_index = 0;
	int data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *src_data;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	/* access command detail */
	temp_index = curr_offset + 2; /* move the temp_index to command detail info  + 2(tag and length) */
	src_data = &tlv_str[0];

	/* In this time, the point of temp_index is COMMAND NUMBER */
	/* [1] insert command detail information into each proactive command data structure. */
	pactive_cmd_ind_obj->data.setup_menu.command_detail.cmd_num = src_data[temp_index++];
	pactive_cmd_ind_obj->data.setup_menu.command_detail.cmd_type = src_data[temp_index++];

	/* [2] decode COMMAND QUALIFIER */
	/*
	 * -bit 1:	0 = no selection preference;
	 *	1 = selection using soft key preferred.
	 * -bits 2 to 7:	= RFU.
	 * -bit 8:	0 = no help information available;
	 *	1 = help information available.
	 */

	/* [2-1] selection preference */
	if (src_data[temp_index] & 0x01) {
		pactive_cmd_ind_obj->data.setup_menu.command_detail.cmd_qualifier.setup_menu.select_preference =
				SELECTION_PREFERENCE_USING_SOFT_KEY;
		dbg("[SAT] SAT PARSER -  sel_pref=SAT_SELECTION_PREFERENCE_USING_SOFT_KEY.");
	} else {
		pactive_cmd_ind_obj->data.setup_menu.command_detail.cmd_qualifier.setup_menu.select_preference =
				SELECTION_PREFERENCE_NONE_REQUESTED;
		dbg("[SAT] SAT PARSER - : sel_pref=SAT_SELECTION_PREFERENCE_NONE_REQUESTED.");
	}

	/* [2-2] help available */
	if (src_data[temp_index] & 0x80) {
		pactive_cmd_ind_obj->data.setup_menu.command_detail.cmd_qualifier.setup_menu.help_info =
				TRUE;
		dbg("[SAT] SAT PARSER - : is help Available=TRUE.");
	} else {
		pactive_cmd_ind_obj->data.setup_menu.command_detail.cmd_qualifier.setup_menu.help_info =
				FALSE;
		dbg("[SAT] SAT PARSER -  is help Available=FALSE.");
	}

	/* In this time, the point of temp_index is DEVICE IDENTITIES. */
	/* [3] decode DEVICE IDENTITIES TLV */
	temp_index++;
	memcpy(dev_id, &src_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &pactive_cmd_ind_obj->data.setup_menu.device_id);
	if (rv != TCORE_SAT_SUCCESS) {
		/* send TR in SatkProcessProactiveCmdInd() */
		return rv;
	}

	/* In this time, the point of temp_index is ALPHA IDENTIFIER. 11 or 12. */
	/* [4] decode ALPHA IDENTIFIER TLV */
	temp_index += 4;
	dbg("[SAT] SAT PARSER - :temp_index=%d", temp_index);
	rv = _sat_decode_alpha_identifier_tlv(src_data, tlv_len, temp_index,
		&pactive_cmd_ind_obj->data.setup_menu.alpha_id,
		&data_len_consumed);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* In this time, the point of temp_index is ITEM  TLV */
	/* [5] decode ITEM LIST (at least one is mandatory) */
	temp_index +=  data_len_consumed;
	pactive_cmd_ind_obj->data.setup_menu.menu_item_cnt = 0;
	do {
		data_len_consumed = 0;
		if ((src_data[temp_index] & 0x7F) == SATK_ITEM_TAG) {
			rv = _sat_decode_item_tlv(src_data, tlv_len, temp_index,
				&pactive_cmd_ind_obj->data.setup_menu.menu_item[pactive_cmd_ind_obj->data.setup_menu.menu_item_cnt],
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;

			if (!pactive_cmd_ind_obj->data.setup_menu.menu_item[0].text_len)
				break;
		} else {
			if (pactive_cmd_ind_obj->data.setup_menu.menu_item_cnt == 0) {
				dbg("menu item is not exist.");
				return TCORE_SAT_REQUIRED_VALUE_MISSING;
			}
			/* else */
			break;	/* ??? */
		}
		pactive_cmd_ind_obj->data.setup_menu.menu_item_cnt++;
		temp_index += data_len_consumed;

		if (temp_index >= tlv_len)
			break;
	} while (pactive_cmd_ind_obj->data.setup_menu.menu_item_cnt < SAT_MENU_ITEM_COUNT_MAX);

	dbg("[SAT] SAT PARSER - :setup menu item_count=%d", pactive_cmd_ind_obj->data.setup_menu.menu_item_cnt);
	if (temp_index >= tlv_len) {
		dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
		/* send TR in SatkProcessProactiveCmdInd() */
		return TCORE_SAT_SUCCESS;
	}

	/* [6] (optional TLV) decode ITEMS NEXT ACTION INDICATOR TLV */
	if ((src_data[temp_index] & 0x7F) == SATK_ITEMS_NEXT_ACTION_INDICATOR_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_item_next_action_indicator_tlv(tlv_str, tlv_len, temp_index,
			&pactive_cmd_ind_obj->data.setup_menu.next_act_ind_list,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		if (temp_index + data_len_consumed >= tlv_len) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			/* send the data to Noti manager. */
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	} else {
		dbg("[SAT] SAT PARSER -  ITEMS NEXT ACTION INDICATOR TLV Not present");
	}

	/* [7] (optional TLV) decode ICON IDENTIFIER TLV */
	if ((src_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(tlv_str, tlv_len, temp_index,
			&pactive_cmd_ind_obj->data.setup_menu.icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		if (temp_index + data_len_consumed >= tlv_len) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			/* send the data to Noti manager. */
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	} else {
		dbg("[SAT] SAT PARSER - ICON IDENTIFIER TLV Not present");
	}

	/* [8] (optional TLV) decode ICON IDENTIFIER LIST TLV */
	if ((src_data[temp_index] & 0x7F) == SATK_ITEM_ICON_IDENTIFIER_LIST_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_list_tlv(tlv_str, tlv_len, temp_index,
			&pactive_cmd_ind_obj->data.setup_menu.icon_list,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		if (temp_index + data_len_consumed >= tlv_len) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			/* send the data to Noti manager. */
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */

	} else {
		dbg("[SAT] SAT PARSER - ICON IDENTIFIER LIST TLV not present");
	}

	/* text attribute - optional */
	if ((src_data[temp_index] & 0x7F) == SATK_TEXT_ATTRIBUTE_TAG) {
		rv = _sat_decode_text_attribute_tlv(tlv_str, tlv_len, temp_index,
			&pactive_cmd_ind_obj->data.setup_menu.text_attribute,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		if (temp_index + data_len_consumed >= tlv_len) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* text attribute list - optional */
	if ((src_data[temp_index] & 0x7F) == SATK_TEXT_ATTRIBUTE_LIST_TAG) {
		int attr_item_temp_index = 0, item_cnt = 0;
		int txt_attr_list_len = 0;

		struct tel_sat_text_attribute_list *txt_attr_list = NULL;

		txt_attr_list = &pactive_cmd_ind_obj->data.setup_menu.text_attribute_list;

		/* length */
		temp_index++;
		txt_attr_list_len = src_data[temp_index];
		if (txt_attr_list_len == 0) {
			dbg("[SAT] - Text Attribute List is nothing");
			return TCORE_SAT_REQUIRED_VALUE_MISSING;
		}

		/* item cnt - each text attribute length is 4byte */
		item_cnt = txt_attr_list_len/4;
		txt_attr_list->list_cnt = item_cnt;
		dbg("[SAT] - text attribute item cnt(%d)", item_cnt);

		/* get attribute data */
		temp_index++;
		for (attr_item_temp_index = 0; attr_item_temp_index < item_cnt; attr_item_temp_index++) {
			memcpy(txt_attr_list->text_attribute_list[attr_item_temp_index].text_formatting, &src_data[temp_index], 4);
			temp_index += 4;
		}

		dbg("[SAT] SAT PARSER - done to decode text attribute list");
	}

	dbg("[SAT] SAT PARSER - no more TLVs to decode.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.9 SELECT ITEM
 */
static enum tcore_sat_result _sat_decode_select_item(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	int data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.select_item.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.select_item.command_detail.cmd_type = cmd_data[temp_index++];

	/* decode command qualifier */
	if (cmd_data[temp_index] & 0x01) {
		if (cmd_data[temp_index] & 0x02) {
			sat_cmd_ind_data->data.select_item.command_detail.cmd_qualifier.select_item.presentation_type = PRESENTATION_TYPE_NAVIGATION_OPTION;
			dbg("[SAT] SAT PARSER - PRESENTATION_TYPE_NAVIGATION_OPTION");
		} else {
			sat_cmd_ind_data->data.select_item.command_detail.cmd_qualifier.select_item.presentation_type = PRESENTATION_TYPE_DATA_VALUE;
			dbg("[SAT] SAT PARSER - PRESENTATION_TYPE_DATA_VALUE");
		}
	} else {
		sat_cmd_ind_data->data.select_item.command_detail.cmd_qualifier.select_item.presentation_type = PRESENTATION_TYPE_NOT_SPECIFIED;
		dbg("[SAT] SAT PARSER - PRESENTATION_TYPE_NOT_SPECIFIED");
	}

	if (cmd_data[temp_index] & 0x04) {
		sat_cmd_ind_data->data.select_item.command_detail.cmd_qualifier.select_item.select_preference = SELECTION_PREFERENCE_USING_SOFT_KEY;
		dbg("[SAT] SAT PARSER - SELECTION_PREFERENCE_USING_SOFT_KEY");
	} else {
		sat_cmd_ind_data->data.select_item.command_detail.cmd_qualifier.select_item.select_preference = SELECTION_PREFERENCE_NONE_REQUESTED;
		dbg("[SAT] SAT PARSER - SELECTION_PREFERENCE_NONE_REQUESTED");
	}

	if (cmd_data[temp_index] & 0x80) {
		sat_cmd_ind_data->data.select_item.command_detail.cmd_qualifier.select_item.help_info = TRUE;
		dbg("[SAT] SAT PARSER - Help info");
	}

	/* device identities */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.select_item.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* alpha identifier */
	temp_index += 4;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.select_item.alpha_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
	}

	/* item objects */
	sat_cmd_ind_data->data.select_item.menu_item_cnt = 0;
	do {
		data_len_consumed = 0;

		if ((cmd_data[temp_index] & 0x7F) == SATK_ITEM_TAG) {
			rv = _sat_decode_item_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.select_item.menu_item[sat_cmd_ind_data->data.select_item.menu_item_cnt],
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;
		} else {
			if (sat_cmd_ind_data->data.select_item.menu_item_cnt == 0) {
				dbg("menu item is not exist.");
				return TCORE_SAT_REQUIRED_VALUE_MISSING;
			}
			/* else */
			break;	/* ??? */
		}

		sat_cmd_ind_data->data.select_item.menu_item_cnt++;
		temp_index += data_len_consumed;

		if (temp_index >= o_length)
			break;

	} while (sat_cmd_ind_data->data.select_item.menu_item_cnt < SAT_MENU_ITEM_COUNT_MAX);

	dbg("[SAT] SAT PARSER - select menu item_count=%d", sat_cmd_ind_data->data.select_item.menu_item_cnt);
	if (temp_index >= o_length) {
		dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
		return TCORE_SAT_SUCCESS;
	}

	/* item next action indicator */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ITEMS_NEXT_ACTION_INDICATOR_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_item_next_action_indicator_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.select_item.item_next_act_ind_list, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		if (temp_index + data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed;
	}

	/* item identifier */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ITEM_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_item_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.select_item.item_identifier,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		if (temp_index + data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* icon identifier */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.select_item.icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */

		if (temp_index + data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	if ((cmd_data[temp_index] & 0x7F) == SATK_ITEM_ICON_IDENTIFIER_LIST_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_list_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.select_item.icon_list,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		if (temp_index + data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* text attribute - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_ATTRIBUTE_TAG) {
		rv = _sat_decode_text_attribute_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.select_item.text_attribute,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		if (temp_index + data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* text attribute list - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_ATTRIBUTE_LIST_TAG) {
		int attr_item_temp_index = 0, item_cnt = 0;
		int txt_attr_list_len = 0;

		struct tel_sat_text_attribute_list *txt_attr_list = NULL;

		txt_attr_list = &sat_cmd_ind_data->data.select_item.text_attribute_list;

		/* length */
		temp_index++;
		txt_attr_list_len = cmd_data[temp_index];
		if (txt_attr_list_len == 0) {
			dbg("[SAT] - Text Attribute List is nothing");
			return TCORE_SAT_REQUIRED_VALUE_MISSING;
		}

		/* item cnt - each text attribute length is 4byte */
		item_cnt = txt_attr_list_len/4;
		txt_attr_list->list_cnt = item_cnt;
		dbg("[SAT] - text attribute item cnt(%d)", item_cnt);

		/* get attribute data */
		temp_index++;
		for (attr_item_temp_index = 0; attr_item_temp_index < item_cnt; attr_item_temp_index++) {
			memcpy(txt_attr_list->text_attribute_list[attr_item_temp_index].text_formatting, &cmd_data[temp_index], 4);
			temp_index += 4;
		}

		dbg("[SAT] SAT PARSER - done to decode text attribute list");
	}

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.10 SEND SMS
 */
static enum tcore_sat_result _sat_decode_send_sms(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	int data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.send_sms.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.send_sms.command_detail.cmd_type = cmd_data[temp_index++];

	/* decode command qualifier */
	if (cmd_data[temp_index] & 0x01)
		sat_cmd_ind_data->data.send_sms.command_detail.cmd_qualifier.send_sms.packing_by_me_required = TRUE;
	else {
		sat_cmd_ind_data->data.send_sms.command_detail.cmd_qualifier.send_sms.packing_by_me_required = FALSE;
		dbg("[SAT] SAT PARSER - packing by me required is false");
	}

	/* device identities */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.send_sms.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* alpha identifier */
	temp_index += 4;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.send_sms.alpha_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
	}

	/* address */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ADDRESS_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_address_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.send_sms.address,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
	}

	/* SMS-TPDU */
	data_len_consumed = 0;
	rv = _sat_decode_sms_tpdu_tlv(o_cmd_data, o_length, temp_index,
		&sat_cmd_ind_data->data.send_sms.sms_tpdu,
		&data_len_consumed);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	if (temp_index + data_len_consumed >= o_length) {
		dbg("[SAT] SAT PARSER - no more TLVs to decode.");
		return TCORE_SAT_SUCCESS;
	}

	/* icon identifier */
	temp_index += data_len_consumed;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.send_sms.icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */

		if (temp_index + data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.11 SEND SS
 */
static enum tcore_sat_result _sat_decode_send_ss(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0, data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL || sat_cmd_ind_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.send_ss.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.send_ss.command_detail.cmd_type = cmd_data[temp_index++];

	/* command detail */
	temp_index++; /* RFU */

	/* device identities */
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.send_ss.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* alpha identifier - optional */
	temp_index += 4;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.send_ss.alpha_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
	}

	/* ss string */
	rv = _sat_decode_ss_string_tlv(o_cmd_data, o_length, temp_index,
		&sat_cmd_ind_data->data.send_ss.ss_string,
		&data_len_consumed);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* icon identifier - optional */
	temp_index += data_len_consumed;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;

		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.send_ss.icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.12 SEND USSD
 */
static enum tcore_sat_result _sat_decode_send_ussd(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0, data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL || sat_cmd_ind_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.send_ussd.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.send_ussd.command_detail.cmd_type = cmd_data[temp_index++];

	/* command detail */
	temp_index++; /* RFU */

	/* device identities */
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.send_ussd.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* alpha identifier - optional */
	temp_index += 4;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.send_ussd.alpha_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
	}

	/* ussd string */
	rv = _sat_decode_ussd_string_tlv(o_cmd_data, o_length, temp_index,
		&sat_cmd_ind_data->data.send_ussd.ussd_string,
		&data_len_consumed);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* icon identifier- optional */
	temp_index += data_len_consumed;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;

		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.send_ussd.icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */
	}

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.13 SETUP CALL
 */
static enum tcore_sat_result _sat_decode_setup_call(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0, data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL || sat_cmd_ind_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* command detail */
	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.setup_call.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.setup_call.command_detail.cmd_type = cmd_data[temp_index++];

	/* decode command qualifier */
	switch (cmd_data[temp_index]) {
	case SETUP_CALL_IF_ANOTHER_CALL_NOT_BUSY:
	case SETUP_CALL_IF_ANOTHER_CALL_NOT_BUSY_WITH_REDIAL:
	case SETUP_CALL_PUT_ALL_OTHER_CALLS_ON_HOLD:
	case SETUP_CALL_PUT_ALL_OTHER_CALLS_ON_HOLD_WITH_REDIAL:
	case SETUP_CALL_DISCONN_ALL_OTHER_CALLS:
	case SETUP_CALL_DISCONN_ALL_OTHER_CALLS_WITH_REDIAL:
		sat_cmd_ind_data->data.setup_call.command_detail.cmd_qualifier.setup_call.setup_call = cmd_data[temp_index];
		dbg("[SAT] SAT PARSER -  setup_call.cmd_qualifier= 0x%02x",
			sat_cmd_ind_data->data.setup_call.command_detail.cmd_qualifier.setup_call.setup_call);
	break;

	case SETUP_CALL_RESERVED: /* Fallthrough */
	default:
		dbg("[SAT] SAT PARSER -  setup_call.cmd_qualifier= 0x%02x", cmd_data[temp_index]);
		return TCORE_SAT_BEYOND_ME_CAPABILITY;
	}

	/* device identifier */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.setup_call.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* alpha identifier (user confirmation) - optional */
	temp_index += 4;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.setup_call.user_confirm_alpha_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
	}

	/* address */
	rv = _sat_decode_address_tlv(o_cmd_data, o_length, temp_index,
		&sat_cmd_ind_data->data.setup_call.address,
		&data_len_consumed);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	temp_index += data_len_consumed;
	if (temp_index >= o_length) {
		dbg("[SAT] SAT PARSER - no more TLVs to decode.");
		return TCORE_SAT_SUCCESS;
	}

	/* capability configuration parameter - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG) {
		rv = _sat_decode_ccp_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.setup_call.ccp,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* sub address - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_SUB_ADDRESS_TAG) {
		rv = _sat_decode_sub_address_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.setup_call.subaddress,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* time duration - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_DURATION_TAG) {
		rv = _sat_decode_duration_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.setup_call.duration,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* icon identifier (user confirmation) - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.setup_call.user_confirm_icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */

		temp_index += data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* alpha identifier (call setup) - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.setup_call.call_setup_alpha_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* icon identifier (call setup) - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.setup_call.call_setup_icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */

		temp_index += data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}
	}

	/*
	 * TODO -
	 *	Text Attribute (user_confirmation , call_setup)
	 */

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.15 PROVIDE LOCAL INFO
 */
static enum tcore_sat_result _sat_decode_provide_local_info(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* command detail */
	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.setup_event_list.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.setup_event_list.command_detail.cmd_type = cmd_data[temp_index++];

	/* decode command qualifier */
	switch (cmd_data[temp_index]) {
	case LOCAL_INFO_DATE_TIME_AND_TIMEZONE:
	case LOCAL_INFO_LANGUAGE:
		sat_cmd_ind_data->data.setup_event_list.command_detail.cmd_qualifier.provide_local_info.provide_local_info = cmd_data[temp_index];
	break;

	/*
	 * TODO -
	 *	Other cases
	 */
	default:
		sat_cmd_ind_data->data.setup_event_list.command_detail.cmd_qualifier.provide_local_info.provide_local_info = LOCAL_INFO_RESERVED;
	break;
	}

	/* device identifier */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.setup_event_list.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/*
	 * TODO -
	 *	UTRAN Measurement Qualifier
	 */

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.16 SETUP EVENT LIST
 */
static enum tcore_sat_result _sat_decode_setup_event_list(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0, data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* command detail */
	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.setup_event_list.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.setup_event_list.command_detail.cmd_type = cmd_data[temp_index++];

	/* device identifier */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.setup_event_list.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* event list */
	temp_index += 4;
	rv = _sat_decode_event_list_tlv(o_cmd_data, o_length, temp_index,
		&sat_cmd_ind_data->data.setup_event_list.event_list,
		&data_len_consumed);

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.22 SETUP IDLE MODE TEXT
 */
static enum tcore_sat_result _sat_decode_setup_idle_mode_text(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0, data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* command detail */
	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.setup_idle_mode_text.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.setup_idle_mode_text.command_detail.cmd_type = cmd_data[temp_index++];

	/* device identifier */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.setup_idle_mode_text.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* text string */
	temp_index += 4;
	rv = _sat_decode_text_tlv(o_cmd_data, o_length, temp_index,
		&sat_cmd_ind_data->data.setup_idle_mode_text.text,
		&data_len_consumed);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	if (sat_cmd_ind_data->data.setup_idle_mode_text.text.string_length < 0) {
		err("[SAT] SAT PARSER - :string length is less than 0");
		return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
	}

	dbg("[SAT] SAT PARSER  text(%s)", sat_cmd_ind_data->data.setup_idle_mode_text.text.string);
	dbg("[SAT] SAT PARSER o_len(%d) temp_index(%d) data_len_consumed(%d)",
		o_length , temp_index, data_len_consumed);

	if (temp_index + data_len_consumed > o_length) {
		err("[SAT] SAT PARSER - Wrong String TLV");
		return TCORE_SAT_BEYOND_ME_CAPABILITY;
	} else if (temp_index + data_len_consumed == o_length) {
		dbg("[SAT] SAT PARSER - :no more TLVs to decode.");
		return TCORE_SAT_SUCCESS;
	}

	/* icon identifier */
	temp_index += data_len_consumed;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.setup_idle_mode_text.icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD; /* Send TR */

		if (temp_index + data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/*
	 * TODO:
	 *	Text Attribute
	 */

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.24 SEND DTMF
 */
static enum tcore_sat_result _sat_decode_send_dtmf(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0, data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* command detail */
	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.send_dtmf.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.send_dtmf.command_detail.cmd_type = cmd_data[temp_index++];

	/* device identifier */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.send_dtmf.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* alpha identifier - optional */
	temp_index += 4;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.send_dtmf.alpha_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
	}

	/* DTMF string - mandatory */
	if ((cmd_data[temp_index] & 0x7F) == SATK_DTMF_STRING_TAG) {
		rv = _sat_decode_dtmf_string_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.send_dtmf.dtmf_string,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		if (temp_index  +  data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed;
	} else {
		dbg("[SAT] SAT PARSER - DTMF tlv is missed.");
		return TCORE_SAT_REQUIRED_VALUE_MISSING;
	}

	/* icon identifier - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.send_dtmf.icon_id,
				&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;

		if (temp_index  +  data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/*
	 * TODO:
	 *	Text Attribute, Frame Identifier
	 */

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.25 LANGUAGE NOTIFICATION
 */
static enum tcore_sat_result _sat_decode_language_notification(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* command detail */
	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.language_notification.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.language_notification.command_detail.cmd_type = cmd_data[temp_index++];

	/* decode command qualifier */
	if (cmd_data[temp_index] & 0x01)
		sat_cmd_ind_data->data.language_notification.command_detail.cmd_qualifier.language_notification.specific_language = TRUE;
	else
		sat_cmd_ind_data->data.language_notification.command_detail.cmd_qualifier.language_notification.specific_language = FALSE;

	/* device identifier */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.language_notification.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* language - conditional */
	temp_index += 4;
	if (sat_cmd_ind_data->data.language_notification.command_detail.cmd_qualifier.language_notification.specific_language == TRUE) {
		if ((cmd_data[temp_index] & 0x7F) == SATK_LANGUAGE_TAG) {
			rv = _sat_decode_language_tlv(cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.language_notification.language);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;
		} else {
			dbg("[SAT] SAT PARSER -  Language TLV is required but missing.");
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
		}
	} else {
		sat_cmd_ind_data->data.language_notification.language = SIM_LANG_UNSPECIFIED;
		dbg("[SAT] SAT PARSER -  non-specific language");
	}

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.26 LAUNCH BROWSER
 */
static enum tcore_sat_result _sat_decode_launch_browser(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0, data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* command detail */
	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.launch_browser.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.launch_browser.command_detail.cmd_type = cmd_data[temp_index++];

	/* decode command qualifier */
	switch (cmd_data[temp_index]) {
	case 0x00:
		sat_cmd_ind_data->data.launch_browser.command_detail.cmd_qualifier.launch_browser.launch_browser =
				LAUNCH_BROWSER_IF_NOT_ALREADY_LAUNCHED;
	break;

	case 0x01:
		sat_cmd_ind_data->data.launch_browser.command_detail.cmd_qualifier.launch_browser.launch_browser =
				LAUNCH_BROWSER_NOT_USED;
	break;

	case 0x02:
		sat_cmd_ind_data->data.launch_browser.command_detail.cmd_qualifier.launch_browser.launch_browser =
				LAUNCH_BROWSER_USE_EXISTING_BROWSER;
	break;

	case 0x03:
		sat_cmd_ind_data->data.launch_browser.command_detail.cmd_qualifier.launch_browser.launch_browser =
				LAUNCH_BROWSER_CLOSE_AND_LAUNCH_NEW_BROWSER;
	break;

	case 0x04:
		sat_cmd_ind_data->data.launch_browser.command_detail.cmd_qualifier.launch_browser.launch_browser =
				LAUNCH_BROWSER_NOT_USED2;
	break;

	default:
		sat_cmd_ind_data->data.launch_browser.command_detail.cmd_qualifier.launch_browser.launch_browser =
				LAUNCH_BROWSER_RESERVED;
	break;
	}

	/* device identifier */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.language_notification.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	temp_index += 4;

	/* Browser Identity TLV - Optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_BROWSER_IDENTITY_TAG) {
		rv = _sat_decode_browser_identity_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.launch_browser.browser_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
	} else {
		dbg("[SAT] SAT PARSER - Browser ID NOT present");
	}

	/* URL TLV - Mandatory */
	if ((cmd_data[temp_index] & 0x7F) == SATK_URL_TAG) {
		rv = _sat_decode_url_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.launch_browser.url,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		if (temp_index  +  data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER -  No more TLVs to decode, decoding done.");
			return TCORE_SAT_SUCCESS;
		} else {
			dbg("[SAT] SAT PARSER -  more TLVs to decode, decoding continue.");
			temp_index += data_len_consumed;
		}
	} else {
		dbg("[SAT] SAT PARSER - Browser URL NOT present! BUG! this value is mandatory!!!");
		return TCORE_SAT_REQUIRED_VALUE_MISSING;
	}

	/* bearer - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_BEARER_TAG) {
		rv = _sat_decode_bearer_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.launch_browser.bearer,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		if (temp_index  +  data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER -  No more TLVs to decode, decoding done.");
			return TCORE_SAT_SUCCESS;
		} else
			temp_index += data_len_consumed;
	} else {
		dbg("[SAT] SAT PARSER -  Bearer TLV Not present.");
	}

	/* Provisioning reference file - optional */
	sat_cmd_ind_data->data.launch_browser.file_ref_count = 0;
	while ((cmd_data[temp_index] & 0x7F) == SATK_PROVISIONING_REF_FILE_TAG) {
		if (sat_cmd_ind_data->data.launch_browser.file_ref_count >= SAT_PROVISIONING_REF_MAX_COUNT) {
			dbg("[SAT] SAT PARSER -  More number of PRF entries than can be handled");
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
		}

		rv = _sat_decode_provisioning_file_ref_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.launch_browser.file_list[sat_cmd_ind_data->data.launch_browser.file_ref_count],
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;
		else
			sat_cmd_ind_data->data.launch_browser.file_ref_count++;

		if (temp_index  +  data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER -  No more TLVs to decode, decoding done.");
			return TCORE_SAT_SUCCESS;
		} else
			temp_index += data_len_consumed;
	}

	/* text string(gateway/proxy identity) - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_STRING_TAG) {
		rv = _sat_decode_text_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.launch_browser.gateway_proxy_text,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		if (temp_index  +  data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER -  No more TLVs to decode, decoding done.");
			return TCORE_SAT_SUCCESS;
		} else
			temp_index += data_len_consumed;
	}

	/* alpha identifier - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.launch_browser.user_confirm_alpha_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		if (temp_index  +  data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER -  No more TLVs to decode, decoding done.");
			return TCORE_SAT_SUCCESS;
		}
		temp_index += data_len_consumed;
	} else {
		dbg("[SAT] SAT PARSER -  No Alpha ID TLV.");
	}

	/* icon identifier - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.launch_browser.user_confirm_icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;

		if (temp_index  +  data_len_consumed >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/*
	 * TODO:
	 *	Text Attribute, Frame Identifier
	 */

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.27 OPEN CHANNEL
 */
static enum tcore_sat_result _sat_decode_open_channel(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	gboolean b_1st_duration = FALSE;
	int bearer_desc_len = 0, data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.open_channel.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.open_channel.command_detail.cmd_type = cmd_data[temp_index++];

	/* command detail */
	sat_cmd_ind_data->data.open_channel.command_detail.cmd_qualifier.open_channel.immediate_link = FALSE;
	sat_cmd_ind_data->data.open_channel.command_detail.cmd_qualifier.open_channel.automatic_reconnection = FALSE;
	sat_cmd_ind_data->data.open_channel.command_detail.cmd_qualifier.open_channel.background_mode = FALSE;

	if (cmd_data[temp_index] & 0x01) {
		sat_cmd_ind_data->data.open_channel.command_detail.cmd_qualifier.open_channel.immediate_link = TRUE;
		dbg("[SAT] SAT PARSER - Immediate Link Establishment");
	}

	if (cmd_data[temp_index] & 0x02) {
		sat_cmd_ind_data->data.open_channel.command_detail.cmd_qualifier.open_channel.automatic_reconnection = TRUE;
		dbg("[SAT] SAT PARSER - Auto Reconnection");
	}

	if (cmd_data[temp_index] & 0x04) {
		sat_cmd_ind_data->data.open_channel.command_detail.cmd_qualifier.open_channel.background_mode = TRUE;
		dbg("[SAT] SAT PARSER - Background mode");
	}

	/* device identities */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id,
		&sat_cmd_ind_data->data.open_channel.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* alpha identifier - optional */
	temp_index += 4;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.open_channel.alpha_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
	}

	/* icon id - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.open_channel.icon_id,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* bearer description */
	rv = _sat_decode_bearer_description_tlv(o_cmd_data, o_length, temp_index,
		&sat_cmd_ind_data->data.open_channel.bearer_desc,
		&data_len_consumed);
	bearer_desc_len = data_len_consumed;
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/*
	 * TODO -
	 *	UICC SERVER MODE
	 */

	switch (sat_cmd_ind_data->data.open_channel.bearer_desc.bearer_type) {
	case BEARER_CSD:
		/* address */
		rv = _sat_decode_address_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.open_channel.bearer_detail.cs_bearer.address,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;

		/* sub address - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_SUB_ADDRESS_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_subaddress_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.cs_bearer.subaddress,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;

			temp_index += data_len_consumed;
		}

		/* time duration 1- optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_DURATION_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_duration_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.cs_bearer.duration1,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv; /* Send TR */

			temp_index += data_len_consumed;
			b_1st_duration = TRUE;
		}

		/* time duration 2- optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_DURATION_TAG) {

			if (!b_1st_duration) {
				dbg("duration 1 does not present!");
				return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
			}

			data_len_consumed = 0;
			rv = _sat_decode_duration_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.cs_bearer.duration2,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv; /* Send TR */

			temp_index += data_len_consumed;
		}

		/* bearer description - already did it */
		temp_index += bearer_desc_len;

		/* buffer size */
		rv = _sat_decode_buffer_size_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.open_channel.buffer_size,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		/* other address - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_OTHER_ADDRESS_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_other_address_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.cs_bearer.other_address,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv; /* Send TR */

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* text string - user login - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_STRING_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_text_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.cs_bearer.text_user_login,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* text string - user password - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_STRING_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_text_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.cs_bearer.text_user_pwd,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* UICC/TERMINAL interface transport level - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_uicc_terminal_interface_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.interface_transport_level,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* destination address - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_OTHER_ADDRESS_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_other_address_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.data_destination_address,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv; /* Send TR */

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}
	break;

	case BEARER_GPRS:
		/* bearer description - already did it */
		temp_index += bearer_desc_len;

		/* buffer size */
		rv = _sat_decode_buffer_size_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.open_channel.buffer_size,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		/* Network Access Name - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_NETWORK_ACCESS_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_network_access_name_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.ps_bearer.network_access_name,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv; /* Send TR */

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* other address - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_OTHER_ADDRESS_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_other_address_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.ps_bearer.other_address,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv; /* Send TR */

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* text string - user login - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_STRING_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_text_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.ps_bearer.text_user_login,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* text string - user password - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_STRING_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_text_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.ps_bearer.text_user_pwd,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* UICC/TERMINAL interface transport level - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_uicc_terminal_interface_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.interface_transport_level,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		dbg("the value (0x%x) after interface transport level", cmd_data[temp_index] & 0x7F);

		/* destination address - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_OTHER_ADDRESS_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_other_address_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.data_destination_address,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv; /* Send TR */

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}
	break;

	case BEARER_DEFAULT_BEARER_FROM_TRANSPORT_LAYER:
		/* bearer description - already did it */
		temp_index += bearer_desc_len;

		/* buffer size */
		rv = _sat_decode_buffer_size_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.open_channel.buffer_size,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		/* other address - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_OTHER_ADDRESS_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_other_address_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.default_bearer.other_address,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv; /* Send TR */

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* text string - user login - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_STRING_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_text_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.default_bearer.text_user_login,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* text string - user password - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_STRING_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_text_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.default_bearer.text_user_pwd,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* UICC/TERMINAL interface transport level - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_uicc_terminal_interface_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.interface_transport_level,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* destination address - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_OTHER_ADDRESS_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_other_address_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.data_destination_address,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv; /* Send TR */

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}
	break;

	case BEARER_LOCAL_LINK_TECHNOLOGY_INDEPENDENT:
		/* time duration 1- optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_DURATION_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_duration_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.local_bearer.duration1,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv; /* Send TR */

			temp_index += data_len_consumed;
			b_1st_duration = TRUE;
		}

		/* time duration 2- optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_DURATION_TAG) {
			if (!b_1st_duration) {
				dbg("duration 1 does not present!");
				return TCORE_SAT_COMMAND_NOT_UNDERSTOOD;
			}

			data_len_consumed = 0;
			rv = _sat_decode_duration_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.local_bearer.duration2,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv; /* Send TR */

			temp_index += data_len_consumed;
		}

		/* bearer description - already did it */
		temp_index += bearer_desc_len;

		/* buffer size */
		rv = _sat_decode_buffer_size_tlv(o_cmd_data, o_length, temp_index,
			&sat_cmd_ind_data->data.open_channel.buffer_size,
			&data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed;
		if (temp_index >= o_length) {
			dbg("[SAT] SAT PARSER - no more TLVs to decode.");
			return TCORE_SAT_SUCCESS;
		}

		/* text string - user password - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_TEXT_STRING_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_text_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.local_bearer.text_user_pwd,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* UICC/TERMINAL interface transport level - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_uicc_terminal_interface_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.interface_transport_level,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv;

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* destination address - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_OTHER_ADDRESS_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_other_address_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.data_destination_address,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv; /* Send TR */

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}

		/* remote entity address - optional */
		if ((cmd_data[temp_index] & 0x7F) == SATK_REMOTE_ENTITY_ADDRESS_TAG) {
			data_len_consumed = 0;
			rv = _sat_decode_remote_entity_address_tlv(o_cmd_data, o_length, temp_index,
				&sat_cmd_ind_data->data.open_channel.bearer_detail.local_bearer.remote_entity_address,
				&data_len_consumed);
			if (rv != TCORE_SAT_SUCCESS)
				return rv; /* Send TR */

			temp_index += data_len_consumed;
			if (temp_index >= o_length) {
				dbg("[SAT] SAT PARSER - no more TLVs to decode.");
				return TCORE_SAT_SUCCESS;
			}
		}
	break;

	default:
	break;
	} /* end of switch */

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.28 CLOSE CHANNEL
 */
static enum tcore_sat_result _sat_decode_close_channel(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	int data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.close_channel.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.close_channel.command_detail.cmd_type = cmd_data[temp_index++];

	/* command detail */
	temp_index++; /* RFU */

	/* device identities */
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.close_channel.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* alpha identifier - optional */
	temp_index += 4;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index, &sat_cmd_ind_data->data.close_channel.alpha_id, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
	}

	/* icon id - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index, &sat_cmd_ind_data->data.close_channel.icon_id, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/*
	 * TODO:
	 *	Text Attribute and frames
	 */

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.29 RECEIVE DATA
 */
static enum tcore_sat_result _sat_decode_receive_data(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	int data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.receive_data.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.receive_data.command_detail.cmd_type = cmd_data[temp_index++];

	/* command detail */
	temp_index++; /* RFU */

	/* device identities */
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.receive_data.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* alpha identifier - optional */
	temp_index += 4;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index, &sat_cmd_ind_data->data.receive_data.alpha_id, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
	}

	/* icon id - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index, &sat_cmd_ind_data->data.receive_data.icon_id, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* channel data length */
	rv = _sat_decode_channel_data_length_tlv(o_cmd_data, o_length, temp_index, &sat_cmd_ind_data->data.receive_data.channel_data_len, &data_len_consumed);
	if (rv != TCORE_SAT_SUCCESS)
		return rv; /* Send TR */

	/*
	 * TODO:
	 *	Text Attribute and frames
	 */

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.30 SEND DATA
 */
static enum tcore_sat_result _sat_decode_send_data(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	int data_len_consumed = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.send_data.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.send_data.command_detail.cmd_type = cmd_data[temp_index++];

	/* command detail */
	sat_cmd_ind_data->data.send_data.command_detail.cmd_qualifier.send_data.send_data_immediately = FALSE;
	if (cmd_data[temp_index] & 0x01) {
		sat_cmd_ind_data->data.send_data.command_detail.cmd_qualifier.send_data.send_data_immediately = TRUE;
		dbg("[SAT] SAT PARSER - Send data immediately");
	}

	/* device identities */
	temp_index++;
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.send_data.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	/* alpha identifier - optional */
	temp_index += 4;
	if ((cmd_data[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		rv = _sat_decode_alpha_identifier_tlv(o_cmd_data, o_length, temp_index, &sat_cmd_ind_data->data.send_data.alpha_id, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;
	}

	/* icon id - optional */
	if ((cmd_data[temp_index] & 0x7F) == SATK_ICON_IDENTIFIER_TAG) {
		data_len_consumed = 0;
		rv = _sat_decode_icon_identifier_tlv(o_cmd_data, o_length, temp_index, &sat_cmd_ind_data->data.send_data.icon_id, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed; /* temp_index pointing to the Tag of next TLV */
	}

	/* channel data */
	rv = _sat_decode_channel_data_tlv(o_cmd_data, o_length, temp_index, &sat_cmd_ind_data->data.send_data.channel_data, &data_len_consumed);
	if (rv != TCORE_SAT_SUCCESS)
		return rv; /* Send TR */

	/*
	 * TODO:
	 *	Text Attribute and frames
	 */

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.31 GET CHANNEL STATUS
 */
static enum tcore_sat_result _sat_decode_get_channel_status(unsigned char *o_cmd_data, int o_length,
	int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.get_channel_status.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.get_channel_status.command_detail.cmd_type = cmd_data[temp_index++];

	/* command detail */
	temp_index++; /* RFU */

	/* device identities */
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.get_channel_status.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

/*
 * 6.4.XX Unsupported Command
 */
static enum tcore_sat_result _sat_decode_unsupported_command(unsigned char *o_cmd_data, int o_length,
		int curr_offset, struct tcore_sat_proactive_command *sat_cmd_ind_data)
{
	int temp_index = 0;
	unsigned char dev_id[4];
	unsigned char *cmd_data = NULL;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (o_cmd_data == NULL) {
		dbg("[SAT] SAT PARSER -  o_cmd_data == NULL");
		return TCORE_SAT_ERROR_FATAL;
	}

	cmd_data = &o_cmd_data[0];
	temp_index = curr_offset + 2;
	sat_cmd_ind_data->data.unsupport_cmd.command_detail.cmd_num = cmd_data[temp_index++];
	sat_cmd_ind_data->data.unsupport_cmd.command_detail.cmd_type = cmd_data[temp_index++];

	/* command detail */
	temp_index++; /* RFU */

	/* device identities */
	memcpy(dev_id, &cmd_data[temp_index], 4);
	rv = _sat_decode_device_identities_tlv(dev_id, &sat_cmd_ind_data->data.unsupport_cmd.device_id);
	if (rv != TCORE_SAT_SUCCESS)
		return rv;

	dbg("[SAT] SAT PARSER - :decoding done!.");
	return TCORE_SAT_SUCCESS;
}

int tcore_sat_decode_proactive_command(unsigned char *tlv_origin, unsigned int tlv_length,
	struct tcore_sat_proactive_command *decoded_tlv)
{
	unsigned int temp_index = 0;
	int length_field_len = 0;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (tlv_origin == NULL || tlv_length <= 2) {
		dbg("[SAT] SAT PARSER - pointer pData passed is NULL or invalid length.");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* tag */
	if (tlv_origin[temp_index++] !=  SATK_PROACTIVE_CMD_TAG) {
		dbg("[SAT] SAT PARSER - Did not find Proactive command tag.tag=%d", tlv_origin[temp_index-1]);
		return TCORE_SAT_ERROR_FATAL;
	}

	/* length */
	length_field_len = _get_length_filed_size(tlv_origin[temp_index]);
	if (length_field_len == 0) {
		dbg("[SAT] SAT PARSER - Invalid length.");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* length */
	dbg("[SAT] SAT PARSER - tlv_length=%d", tlv_length);
	temp_index += length_field_len;

	/* check command validation */
	if (tlv_length < temp_index + 5 + 4)/* command detail(5) and device identities(4) */
		return TCORE_SAT_ERROR_FATAL;

	/* check comprehensive value */
	if ((tlv_origin[temp_index] | 0x7F) != 0x7F) {
		dbg("comprehensive value 0x%x", tlv_origin[temp_index] | 0x7F);
		b_comprehensive = TRUE;
	}

	if ((tlv_origin[temp_index] & 0x7F) != SATK_COMMAND_DETAILS_TAG) {
		err("[SAT] no command detail info");
		return TCORE_SAT_ERROR_FATAL;
	}

	if (tlv_origin[temp_index + 1] != SATK_COMMAND_DETAILS_LENGTH) {
		err("[SAT] invalid command detail length");
		return TCORE_SAT_ERROR_FATAL;
	}

	decoded_tlv->cmd_num = tlv_origin[temp_index + 2];
	decoded_tlv->cmd_type = tlv_origin[temp_index + 3];

	switch (decoded_tlv->cmd_type) {
	case SAT_PROATV_CMD_DISPLAY_TEXT: /* 6.4.1 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_DISPLAY_TEXT");
		rv = _sat_decode_display_text(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_GET_INKEY: /* 6.4.2 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_GET_INKEY");
		rv = _sat_decode_get_inkey(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_GET_INPUT: /* 6.4.3 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_GET_INPUT");
		rv = _sat_decode_get_input(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_MORE_TIME: /* 6.4.4 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_MORE_TIME");
		rv = _sat_decode_more_time(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_PLAY_TONE: /* 6.4.5 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_PLAY TONE");
		rv = _sat_decode_play_tone(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	/* case POLL INTERVAL //6.4.6 processing by cp */
	case SAT_PROATV_CMD_REFRESH: /* 6.4.7 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_REFRESH");
		rv = _sat_decode_refresh(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_SETUP_MENU: /* 6.4.8 */
		dbg("[SAT] SAT PARSER - SAT_PROATV_CMD_SETUP_MENU");
		rv = _sat_decode_setup_menu(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_SELECT_ITEM: /* 6.4.9 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SELECT_ITEM");
		rv = _sat_decode_select_item(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_SEND_SMS: /* 6.4.10 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SEND_SMS");
		rv = _sat_decode_send_sms(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_SEND_SS: /* 6.4.11 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SEND_SS");
		rv = _sat_decode_send_ss(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_SEND_USSD: /* 6.4.12 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SEND_USSD");
		rv = _sat_decode_send_ussd(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_SETUP_CALL: /* 6.4.13 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SETUP_CALL");
		rv = _sat_decode_setup_call(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_PROVIDE_LOCAL_INFO: /* 6.4.15 */
		dbg("[SAT] SAT PARSER - SAT_PROATV_CMD_PROVIDE_LOCAL_INFO");
		rv = _sat_decode_provide_local_info(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_SETUP_EVENT_LIST: /* 6.4.16 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SETUP_EVENT_LIST");
		rv = _sat_decode_setup_event_list(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_SETUP_IDLE_MODE_TEXT: /* 6.4.22 */
		dbg("[SAT] SAT PARSER - SAT_PROATV_CMD_SETUP_IDLE_MODE_TEXT");
		rv = _sat_decode_setup_idle_mode_text(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_SEND_DTMF: /* 6.4.24 */
		dbg("[SAT] SAT PARSER - SAT_PROATV_CMD_SEND_DTMF");
		rv = _sat_decode_send_dtmf(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_LANGUAGE_NOTIFICATION: /* 6.4.25 */
		dbg("[SAT] SAT PARSER - SAT_PROATV_CMD_LANGUAGE_NOTIFICATION");
		rv = _sat_decode_language_notification(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_LAUNCH_BROWSER: /* 6.4.26 */
		dbg("[SAT] SAT PARSER - SAT_PROATV_CMD_LAUNCH_BROWSER");
		rv = _sat_decode_launch_browser(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_OPEN_CHANNEL:/* 6.4.27 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_OPEN_CHANNEL");
		rv = _sat_decode_open_channel(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_CLOSE_CHANNEL:/* 6.4.28 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_CLOSE_CHANNEL");
		rv = _sat_decode_close_channel(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_RECEIVE_DATA:/* 6.4.29 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_RECEIVE_DATA");
		rv = _sat_decode_receive_data(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_SEND_DATA:/* 6.4.30 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_SEND_DATA");
		rv = _sat_decode_send_data(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	case SAT_PROATV_CMD_GET_CHANNEL_STATUS:/* 6.4.31 */
		dbg("[SAT] SAT PARSER - SAT_CMD_TYPE_GET_CHANNEL_STATUS");
		rv = _sat_decode_get_channel_status(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;

	default:
		dbg("[SAT] SAT PARSER - ME cannot perform this command =0x[%02x]", decoded_tlv->cmd_type);
		rv = _sat_decode_unsupported_command(tlv_origin, tlv_length, temp_index, decoded_tlv);
	break;
	}

	dbg("[SAT] SAT PARSER - each command parsing done.");
	return rv;
}

int tcore_sat_decode_call_control_result(unsigned char *tlv_origin, unsigned int tlv_length, struct tnoti_sat_call_control_result_ind *call_ctrl_result_tlv)
{
	unsigned int temp_index = 0;
	int length = 0, data_len = 0, data_len_consumed = 0;
	enum tcore_sat_result rv = TCORE_SAT_SUCCESS;

	if (tlv_origin == NULL || tlv_length <= 2) {
		dbg("[SAT] SAT PARSER - pointer pData passed is NULL or invalid length.");
		return TCORE_SAT_ERROR_FATAL;
	}

	/* call conrol result */
	call_ctrl_result_tlv->cc_result = tlv_origin[temp_index++];
	length = _get_length_filed_size(tlv_origin[temp_index]);
	if (length == 0) {
		dbg("[SAT] fail to get the call control result length");
		return TCORE_SAT_ERROR_FATAL;
	}

	temp_index = temp_index + length-1;
	data_len = tlv_origin[temp_index];
	dbg("[SAT] call control result (%d), data len(%d)", call_ctrl_result_tlv->cc_result, data_len);
	if (data_len == 0) {
		dbg("no more call control result - decoding done");
		return rv;
	}
	temp_index++;

	/* address - optional (voice call) */
	if ((tlv_origin[temp_index] & 0x7F) == SATK_ADDRESS_TAG) {
		rv = _sat_decode_address_tlv(tlv_origin, tlv_length, temp_index, &call_ctrl_result_tlv->address, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;

		if (temp_index >= tlv_length) {
			dbg("[SAT] call control decoding done");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* ss string - optional (ss) */
	if ((tlv_origin[temp_index] & 0x7F) == SATK_SS_STRING_TAG) {
		rv = _sat_decode_ss_string_tlv(tlv_origin, tlv_length, temp_index, &call_ctrl_result_tlv->ss_string, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;

		if (temp_index >= tlv_length) {
			dbg("[SAT] call control decoding done");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* ccp1 - optional */
	if ((tlv_origin[temp_index] & 0x7F) == SATK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG) {
		rv = _sat_decode_ccp_tlv(tlv_origin, tlv_length, temp_index, &call_ctrl_result_tlv->ccp1, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed;

		if (temp_index >= tlv_length) {
			dbg("[SAT] call control decoding done");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* sub address */
	if ((tlv_origin[temp_index] & 0x7F) == SATK_SUB_ADDRESS_TAG) {
		rv = _sat_decode_sub_address_tlv(tlv_origin, tlv_length, temp_index, &call_ctrl_result_tlv->sub_address, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;

		if (temp_index >= tlv_length) {
			dbg("[SAT] call control decoding done");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* alpha id */
	if ((tlv_origin[temp_index] & 0x7F) == SATK_ALPHA_IDENTIFIER_TAG) {
		rv = _sat_decode_alpha_identifier_tlv(tlv_origin, tlv_length, temp_index, &call_ctrl_result_tlv->alpha_id, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv;

		temp_index += data_len_consumed;

		if (temp_index >= tlv_length) {
			dbg("[SAT] call control decoding done");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* bc repeated indicator */
	if ((tlv_origin[temp_index] & 0x7F) == SATK_BC_REPEAT_INDICATOR_TAG) {
		int tag = 0;
		int bc_repeat_ind_len = 0;

		tag = tlv_origin[temp_index++];
		bc_repeat_ind_len = tlv_origin[temp_index++];
		call_ctrl_result_tlv->bc_repeat_type.bc_indi_repeat_type = tlv_origin[temp_index++];

		dbg("bc repeated indication tag(%x) len(%x) type(%x)", tag, bc_repeat_ind_len, call_ctrl_result_tlv->bc_repeat_type.bc_indi_repeat_type);
		if (temp_index >= tlv_length) {
			dbg("[SAT] call control decoding done");
			return TCORE_SAT_SUCCESS;
		}
	}

	/* ccp2 */
	if ((tlv_origin[temp_index] & 0x7F) == SATK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG) {
		rv = _sat_decode_ccp_tlv(tlv_origin, tlv_length, temp_index, &call_ctrl_result_tlv->ccp2, &data_len_consumed);
		if (rv != TCORE_SAT_SUCCESS)
			return rv; /* Send TR */

		temp_index += data_len_consumed;

		if (temp_index >= tlv_length) {
			dbg("[SAT] call control decoding done");
			return TCORE_SAT_SUCCESS;
		}
	}

	return rv;
}

static unsigned char _sat_encode_dcs_tlv(const struct data_coding_scheme *src)
{
	unsigned char rv = 0x00;

	if (src == NULL)
		return 0;

	if (src->is_compressed_format)
		rv |= 0x20;

	/* msg class */
	switch (src->m_class) {
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

	/* alphabet format */
	switch (src->a_format) {
	case ALPHABET_FORMAT_SMS_DEFAULT:
		rv &= 0xF3;
	break;

	case ALPHABET_FORMAT_8BIT_DATA:
		rv |= 0x04;
	break;

	case ALPHABET_FORMAT_UCS2:
		rv |= 0x08;
	break;

	default:
		rv |= 0x0C;
	break;
	}

	return rv;
}

static int _sat_encode_command_detail_tlv(const struct tel_sat_cmd_detail_info *src, char *dst, int current_temp_index)
{
	dst[current_temp_index++] = (b_comprehensive ? (SATK_COMMAND_DETAILS_TAG | 0x80) : SATK_COMMAND_DETAILS_TAG);
	dst[current_temp_index++] = SATK_COMMAND_DETAILS_LENGTH;
	dst[current_temp_index++] = src->cmd_num;
	dst[current_temp_index++] = src->cmd_type;
	dst[current_temp_index] = 0x00;

	switch (src->cmd_type) {
	case SAT_PROATV_CMD_DISPLAY_TEXT:
		/* command detail text priority */
		if (src->cmd_qualifier.display_text.text_priority == TEXT_PRIORITY_HIGH)
			dst[current_temp_index] += 0x01;

		/* command detail text clear type */
		if (src->cmd_qualifier.display_text.text_clear_type == TEXT_WAIT_FOR_USER_TO_CLEAR_MSG)
			dst[current_temp_index] += 0x80;
	break;

	case SAT_PROATV_CMD_GET_INKEY:
		/* command detail alphabet set */
		if (src->cmd_qualifier.get_inkey.alphabet_set)
			dst[current_temp_index] += 0x01;

		/* command detail alphabet type */
		if (src->cmd_qualifier.get_inkey.alphabet_type == INPUT_ALPHABET_TYPE_UCS2)
			dst[current_temp_index] += 0x02;

		/* command detail get inkey type */
		if (src->cmd_qualifier.get_inkey.inkey_type == INKEY_TYPE_YES_NO_REQUESTED)
			dst[current_temp_index] += 0x04;


		/* command detail immediate response required */
		if (src->cmd_qualifier.get_inkey.immediate_rsp_required)
			dst[current_temp_index] += 0x08;

		/* command detail help available */
		if (src->cmd_qualifier.get_inkey.help_info)
			dst[current_temp_index] += 0x80;
	break;

	case SAT_PROATV_CMD_GET_INPUT:
		/* command detail alphabet set */
		if (src->cmd_qualifier.get_input.alphabet_set)
			dst[current_temp_index] += 0x01;

		/* command detail alphabet type */
		if (src->cmd_qualifier.get_input.alphabet_type == INPUT_ALPHABET_TYPE_UCS2)
			dst[current_temp_index] += 0x02;

		/* command detail echo user input */
		if (!src->cmd_qualifier.get_input.me_echo_user_input)
			dst[current_temp_index] += 0x04;

		/* command detail user input unpacked format */
		if (!src->cmd_qualifier.get_input.user_input_unpacked_format)
			dst[current_temp_index] += 0x08;

		/* command detail help available */
		if (src->cmd_qualifier.get_input.help_info)
			dst[current_temp_index] += 0x80;
	break;

	case SAT_PROATV_CMD_MORE_TIME:
		dbg("more time : 1bit RFU");
	break;

	case SAT_PROATV_CMD_PLAY_TONE:
		/* command detail vibration alert */
		if (src->cmd_qualifier.play_tone.vibration_alert == VIBRATE_ALERT_REQUIRED)
			dst[current_temp_index] += 0x01;
	break;

	case SAT_PROATV_CMD_REFRESH:
		/* command detail refresh command */
		dst[current_temp_index] += src->cmd_qualifier.refresh.refresh;
	break;

	case SAT_PROATV_CMD_SETUP_MENU:
		/* command detail preferences */
		if (src->cmd_qualifier.setup_menu.select_preference == SELECTION_PREFERENCE_USING_SOFT_KEY)
			dst[current_temp_index] += 0x01;

		/* command detail help available */
		if (src->cmd_qualifier.setup_menu.help_info)
			dst[current_temp_index] += 0x80;
	break;

	case SAT_PROATV_CMD_SELECT_ITEM:
		/* command detail presentation */
		if (src->cmd_qualifier.select_item.presentation_type != PRESENTATION_TYPE_NOT_SPECIFIED) {
			dst[current_temp_index] += 0x01;
			if (src->cmd_qualifier.select_item.presentation_type == PRESENTATION_TYPE_NAVIGATION_OPTION)
				dst[current_temp_index] += PRESENTATION_TYPE_NAVIGATION_OPTION;
		}

		/* command detail selection preference */
		if (src->cmd_qualifier.select_item.select_preference == SELECTION_PREFERENCE_USING_SOFT_KEY)
			dst[current_temp_index] += 0x04;

		/* command detail help available */
		if (src->cmd_qualifier.select_item.help_info)
			dst[current_temp_index] += 0x80;
	break;

	case SAT_PROATV_CMD_SEND_SMS:
		/* command detail sms packing by me required */
		if (src->cmd_qualifier.send_sms.packing_by_me_required)
			dst[current_temp_index] += 0x01;
	break;

	case SAT_PROATV_CMD_SETUP_CALL:
		/* command detail setup call command */
		dst[current_temp_index] += src->cmd_qualifier.setup_call.setup_call;
	break;

	case SAT_PROATV_CMD_SETUP_EVENT_LIST:
		dbg("setup evnet list : 1bit RFU");
	break;

	case SAT_PROATV_CMD_OPEN_CHANNEL:
		if (src->cmd_qualifier.open_channel.immediate_link)
			dst[current_temp_index] += 0x01;

		if (src->cmd_qualifier.open_channel.automatic_reconnection)
			dst[current_temp_index] += 0x02;

		if (src->cmd_qualifier.open_channel.background_mode)
			dst[current_temp_index] += 0x04;
	break;

	case SAT_PROATV_CMD_SEND_DATA:
		if (src->cmd_qualifier.send_data.send_data_immediately)
			dst[current_temp_index] += 0x01;
	break;

	case SAT_PROATV_CMD_PROVIDE_LOCAL_INFO:
		dst[current_temp_index] += src->cmd_qualifier.provide_local_info.provide_local_info;
	break;

	case SAT_PROATV_CMD_LANGUAGE_NOTIFICATION:
		if (src->cmd_qualifier.language_notification.specific_language)
			dst[current_temp_index] += 0x01;
	break;

	case SAT_PROATV_CMD_LAUNCH_BROWSER:
		dst[current_temp_index] += src->cmd_qualifier.launch_browser.launch_browser;
	break;

	default:
		err("no matched cmd type(%d)", src->cmd_type);
	break;
	}

	return 5;
}

static int _sat_encode_device_identities_tlv(const struct tel_sat_device_identities *src, char *dst, int current_temp_index)
{
	dst[current_temp_index++] = SATK_DEVICE_IDENTITY_TAG;
	dst[current_temp_index++] = SATK_DEVICE_IDENTITY_LENGTH;
	dst[current_temp_index++] = src->src;
	dst[current_temp_index++] = src->dest;

	/* device identities total len 4 */
	return 4;
}

static int _sat_encode_item_identifier_tlv(const struct tel_sat_item_identifier *src, char *dst, int current_temp_index)
{
	dst[current_temp_index++] = SATK_ITEM_IDENTIFIER_TAG;
	dst[current_temp_index++] = SATK_ITEM_IDENTIFIER_LENGTH;
	dst[current_temp_index++] = src->item_identifier;

	/* item identifier total len 3 */
	return 3;
}

static int _sat_encode_duration_tlv(const struct tel_sat_duration *src, char *dst, int current_temp_index)
{
	dst[current_temp_index++] = SATK_DURATION_TAG;
	dst[current_temp_index++] = SATK_DURATION_LENGTH;
	dst[current_temp_index++] = src->time_unit;
	dst[current_temp_index++] = src->time_interval;

	/*duration total len 4 */
	return 4;
}

static int _sat_encode_text_tlv(const struct tel_sat_text_string_object *src, char *dst, int current_temp_index, gboolean raw_dcs)
{
	int total_len = 0;
	int length_temp_index = 0;

	/* tag */
	dst[current_temp_index++] = SATK_TEXT_STRING_TAG;

	/* length */
	if (src->string_length <= 0x7F) {
		dst[current_temp_index++] = SATK_DCS_LENGTH  +  src->string_length;
		length_temp_index = 1;
	} else {
		dst[current_temp_index++] = 0x81;
		dst[current_temp_index++] = SATK_DCS_LENGTH  +  src->string_length;
		length_temp_index = 2;
	}

	/* dcs */
	if (raw_dcs)
		dst[current_temp_index++] = src->dcs.raw_dcs;
	else
		dst[current_temp_index++] = _sat_encode_dcs_tlv(&(src->dcs));

	/* value */
	if (src->string_length > 0)
		memcpy(&(dst[current_temp_index]), src->string, src->string_length);

	/* tag + temp_index + dcs + data */
	total_len = 1 + length_temp_index + 1 + src->string_length;

	return total_len;
}

static int _sat_encode_eventlist_tlv(const enum event_list src, char *dst, int current_temp_index)
{
	dst[current_temp_index++] = SATK_EVENT_LIST_TAG;
	dst[current_temp_index++] = 0x01;
	dst[current_temp_index++] = src;

	return 3;
}

static int _sat_encode_date_time_and_timezone_tlv(const struct tel_sat_date_time_and_timezone *src, char *dst, int current_temp_index)
{
	dst[current_temp_index++] = SATK_DATE_TIME_AND_TIME_ZONE_TAG;
	dst[current_temp_index++] = SATK_DATE_TIME_AND_TIME_ZONE_LENGTH;
	dst[current_temp_index++] = src->year;
	dst[current_temp_index++] = src->month;
	dst[current_temp_index++] = src->day;
	dst[current_temp_index++] = src->hour;
	dst[current_temp_index++] = src->minute;
	dst[current_temp_index++] = src->second;
	dst[current_temp_index++] = src->timeZone;

	return 1 + 1 + SATK_DATE_TIME_AND_TIME_ZONE_LENGTH; /* tag length + len field length + value length; */
}

static int _sat_encode_language_tlv(const enum tel_sim_language_type src, char *dst, int current_temp_index)
{
	dst[current_temp_index++] = SATK_LANGUAGE_TAG;
	dst[current_temp_index++] = SATK_LANGUAGE_LENGTH;

	dbg("language (%d)", src);

	switch (src) {
	case SIM_LANG_GERMAN:
		dst[current_temp_index++] = 'd';
		dst[current_temp_index++] = 'e';
	break;

	case SIM_LANG_ENGLISH:
		dst[current_temp_index++] = 'e';
		dst[current_temp_index++] = 'n';
	break;

	case SIM_LANG_ITALIAN:
		dst[current_temp_index++] = 'i';
		dst[current_temp_index++] = 't';
	break;

	case SIM_LANG_FRENCH:
		dst[current_temp_index++] = 'f';
		dst[current_temp_index++] = 'r';
	break;

	case SIM_LANG_SPANISH:
		dst[current_temp_index++] = 'e';
		dst[current_temp_index++] = 's';
	break;

	case SIM_LANG_DUTCH:
		dst[current_temp_index++] = 'n';
		dst[current_temp_index++] = 'l';
	break;

	case SIM_LANG_SWEDISH:
		dst[current_temp_index++] = 's';
		dst[current_temp_index++] = 'v';
	break;

	case SIM_LANG_DANISH:
		dst[current_temp_index++] = 'd';
		dst[current_temp_index++] = 'a';
	break;

	case SIM_LANG_PORTUGUESE:
		dst[current_temp_index++] = 'p';
		dst[current_temp_index++] = 't';
	break;

	case SIM_LANG_FINNISH:
		dst[current_temp_index++] = 'f';
		dst[current_temp_index++] = 'i';
	break;

	case SIM_LANG_NORWEGIAN:
		dst[current_temp_index++] = 'n';
		dst[current_temp_index++] = 'b';
	break;

	case SIM_LANG_GREEK:
		dst[current_temp_index++] = 'e';
		dst[current_temp_index++] = 'l';
	break;

	case SIM_LANG_TURKISH:
		dst[current_temp_index++] = 't';
		dst[current_temp_index++] = 'k';
	break;

	case SIM_LANG_HUNGARIAN:
		dst[current_temp_index++] = 'h';
		dst[current_temp_index++] = 'u';
	break;

	case SIM_LANG_POLISH:
		dst[current_temp_index++] = 'p';
		dst[current_temp_index++] = 'l';
	break;

	case SIM_LANG_KOREAN:
		dst[current_temp_index++] = 'k';
		dst[current_temp_index++] = 'o';
	break;

	case SIM_LANG_CHINESE:
		dst[current_temp_index++] = 'z';
		dst[current_temp_index++] = 'h';
	break;

	case SIM_LANG_RUSSIAN:
		dst[current_temp_index++] = 'r';
		dst[current_temp_index++] = 'u';
	break;

	case SIM_LANG_JAPANESE:
		dst[current_temp_index++] = 'j';
		dst[current_temp_index++] = 'a';
	break;

	default:
		dst[current_temp_index++] = 'e';
		dst[current_temp_index++] = 'n';
		dbg("[SAT] SAT PARSER - Unknown Language: 0x%x", src);
	break;
	}

	return 4;
}

static int _sat_encode_browser_termination_tlv(const enum browser_termination_cause src, char *dst, int current_temp_index)
{
	dst[current_temp_index++] = SATK_BROWSER_TERMINATION_CAUSE_TAG;
	dst[current_temp_index++] = SATK_BROWSER_TERMINATION_CAUSE_LENGTH;
	dst[current_temp_index++] = src;

	return 3;
}

#if 0
static int _sat_encode_bearer_desc_tlv(const struct tel_sat_bearer_description *src, char *dst, int current_temp_index)
{
	int total_len = 0;
	int length_temp_index = 0;

	dst[current_temp_index++] = SATK_BEARER_DISCRIPTION_TAG;

	/* length temp_index */
	length_temp_index = current_temp_index++;

	/* bearer type */
	dst[current_temp_index++] = src->bearer_type;

	switch (src->bearer_type) {
	case BEARER_CSD: {
		dst[current_temp_index++] = src->bearer_parameter.cs_bearer_param.data_rate;
		dst[current_temp_index++] = src->bearer_parameter.cs_bearer_param.service_type;
		dst[current_temp_index++] = src->bearer_parameter.cs_bearer_param.connection_element_type;
	}
	break;

	case BEARER_GPRS: {
		dst[current_temp_index++] = src->bearer_parameter.ps_bearer_param.precedence_class;
		dst[current_temp_index++] = src->bearer_parameter.ps_bearer_param.delay_class;
		dst[current_temp_index++] = src->bearer_parameter.ps_bearer_param.reliability_class;
		dst[current_temp_index++] = src->bearer_parameter.ps_bearer_param.peak_throughput_class;
		dst[current_temp_index++] = src->bearer_parameter.ps_bearer_param.mean_throughput_class;
		dst[current_temp_index++] = src->bearer_parameter.ps_bearer_param.pdp_type;
	}
	break;

	case BEARER_DEFAULT_BEARER_FROM_TRANSPORT_LAYER:
	case BEARER_LOCAL_LINK_TECHNOLOGY_INDEPENDENT:
	default:
	break;
	}

	dst[length_temp_index] = (current_temp_index-1) - length_temp_index;
	total_len = (current_temp_index-1) - length_temp_index  +  2; /* tag and length */

	return total_len;
}

static int _sat_encode_buffer_size_tlv(const struct tel_sat_buffer_size *src, char *dst, int current_temp_index)
{
	dst[current_temp_index++] = SATK_BUFFER_SIZE_TAG;
	dst[current_temp_index++] = SATK_BUFFER_SIZE_LENGTH;
	dst[current_temp_index++] = src->size[0];
	dst[current_temp_index++] = src->size[1];

	return 4;
}
#endif

static int _sat_encode_channel_data_tlv(const struct tel_sat_channel_data *src, char *dst, int current_temp_index)
{
	int total_len = 0;
	int length_temp_index = 0;

	dst[current_temp_index++] = SATK_CHANNEL_DATA_TAG;

	if (src->data_string_len <= 0x7F) {
		dst[current_temp_index++] = src->data_string_len;
		length_temp_index = 1;
	} else {
		dst[current_temp_index++] = 0x81;
		dst[current_temp_index++] = src->data_string_len;
		length_temp_index = 2;
	}

	memcpy(&(dst[current_temp_index]), src->data_string, src->data_string_len);

	total_len = 1 + length_temp_index + src->data_string_len;

	return total_len;
}

static int _sat_encode_channel_data_length_tlv(const struct tel_sat_channel_data_len *src, char *dst, int current_temp_index)
{
	dst[current_temp_index++] = SATK_CHANNEL_DATA_LEN_TAG;
	dst[current_temp_index++] = SATK_CHANNEL_DATA_LENGTH_VALUE_LENGTH;
	dst[current_temp_index++] = src->data_len;

	return 3;
}

static int _sat_encode_channel_status_tlv(const struct tel_sat_channel_status *src, char *dst, int current_temp_index)
{
	dst[current_temp_index++] = SATK_CHANNEL_STATUS_TAG;
	dst[current_temp_index++] = SATK_CHANNEL_STATUS_LENGTH;

	if (src->status == link_or_packet_service_activated) /* (bit 8) */
		dst[current_temp_index] += 0x80;

	dst[current_temp_index++] += src->channel_id; /* (bit 1~3) */
	dst[current_temp_index++] = src->status_info;

	return 4;
}

static int _sat_encode_download_event(const struct tel_sat_envelop_event_download_tlv *evt_dl, char *dst_envelop)
{
	int temp_index = 2;
	int encoded_len = 0;

	dbg("event type(%d)", evt_dl->event);

	/* event list */
	encoded_len = _sat_encode_eventlist_tlv(evt_dl->event, dst_envelop, temp_index);
	temp_index += encoded_len;

	/* device id - len 4 */
	encoded_len = _sat_encode_device_identities_tlv(&(evt_dl->device_identitie), dst_envelop, temp_index);
	temp_index += encoded_len;

	switch (evt_dl->event) {
	case EVENT_LANGUAGE_SELECTION:
		encoded_len = _sat_encode_language_tlv(evt_dl->language, dst_envelop, temp_index);
		temp_index += encoded_len;
	break;

	case EVENT_BROWSER_TERMINATION:
		encoded_len = _sat_encode_browser_termination_tlv(evt_dl->browser_termination, dst_envelop, temp_index);
		temp_index += encoded_len;
	break;

	case EVENT_DATA_AVAILABLE:
		encoded_len = _sat_encode_channel_status_tlv(&(evt_dl->channel_status), dst_envelop, temp_index);
		temp_index += encoded_len;

		encoded_len = _sat_encode_channel_data_length_tlv(&(evt_dl->channel_data_len), dst_envelop, temp_index);
		temp_index += encoded_len;
	break;

	case EVENT_CHANNEL_STATUS:
		encoded_len = _sat_encode_channel_status_tlv(&(evt_dl->channel_status), dst_envelop, temp_index);
		temp_index += encoded_len;
	break;

	default:
	break;
	}

	dst_envelop[0] = SATK_EVENT_DOWNLOAD_TAG;
	dst_envelop[1] = temp_index-2;

	dbg("download envelop cmd len(%d)", temp_index);

	if (temp_index-2 > 0x7F) {
		int idx = 0;
		for (idx = temp_index; idx > 0; idx--)
			dst_envelop[idx] = dst_envelop[idx + 1];
		dst_envelop[1] = 0x81;
		temp_index += 1;
		dbg("download envelop added cmd len(%d)", temp_index);
	}

	return temp_index;
}

int tcore_sat_encode_envelop_cmd(const struct treq_sat_envelop_cmd_data *src_envelop, char *dst_envelop)
{
	int temp_index = 0, encoded_len = 0;

	if (!dst_envelop)
		return 0;

	if (src_envelop->sub_cmd
			 ==  ENVELOP_MENU_SELECTION) {
		temp_index = 2; /* set the cursor to device identity */
		dbg("item id(%d)", src_envelop->envelop_data.menu_select.item_identifier.item_identifier);
		encoded_len = _sat_encode_device_identities_tlv(&(src_envelop->envelop_data.menu_select.device_identitie), dst_envelop, temp_index);
		temp_index += encoded_len;

		/* item identifier */
		encoded_len = _sat_encode_item_identifier_tlv(&(src_envelop->envelop_data.menu_select.item_identifier), dst_envelop, temp_index);
		temp_index += encoded_len;

		if (src_envelop->envelop_data.menu_select.help_request) {
			encoded_len = 2;/* help request */
			dst_envelop[temp_index++] = SATK_HELP_REQUEST_TAG;
			dst_envelop[temp_index++] = SATK_HELP_REQUEST_LENGTH;
		}

		dbg("menu selection cmd len(%d)", temp_index);

		/* main cmd */
		dst_envelop[0] = SATK_MENU_SELECTION_TAG;
		dst_envelop[1] = temp_index-2;
	} else if (src_envelop->sub_cmd
			 ==  ENVELOP_EVENT_DOWNLOAD)
		temp_index = _sat_encode_download_event(&(src_envelop->envelop_data.event_download), dst_envelop);

	return temp_index;
}



static int _sat_encode_display_text(const struct tel_sat_tr_display_text_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);
	switch (src_tr->result_type) {
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
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_FRAMES_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_get_inkey(const struct tel_sat_tr_get_inkey_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;

		encoded_len = _sat_encode_text_tlv(&(src_tr->text), dst_tr, temp_index, FALSE);
		temp_index += encoded_len;
	break;

	case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
	case RESULT_BACKWARD_MOVE_BY_USER:
	case RESULT_HELP_INFO_REQUIRED_BY_USER:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;
	case RESULT_NO_RESPONSE_FROM_USER:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
		if (src_tr->duration.time_interval > 0) {
			encoded_len = _sat_encode_duration_tlv(&src_tr->duration, dst_tr, temp_index);
			temp_index += encoded_len;
		}
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_FRAMES_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_get_input(const struct tel_sat_tr_get_input_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;

		encoded_len = _sat_encode_text_tlv(&(src_tr->text), dst_tr, temp_index, FALSE);
		temp_index += encoded_len;
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
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_FRAMES_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_more_time(const struct tel_sat_tr_more_time_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_play_tone(const struct tel_sat_tr_play_tone_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
	case RESULT_SUCCESS_BUT_TONE_NOT_PLAYED:
	case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_FRAMES_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_refresh(const struct tel_sat_tr_refresh_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_REFRESH_PERFORMED_WITH_ADDITIONAL_EFS_READ:
	case RESULT_REFRESH_PRFRMD_BUT_INDICATED_USIM_NOT_ACTIVE:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_setup_menu(const struct tel_sat_tr_setup_menu_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);
	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_select_item(const struct tel_sat_tr_select_item_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
	case RESULT_HELP_INFO_REQUIRED_BY_USER:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
		encoded_len = _sat_encode_item_identifier_tlv(&(src_tr->item_identifier), dst_tr, temp_index);
		temp_index += encoded_len;
	break;

	case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
	case RESULT_BACKWARD_MOVE_BY_USER:
	case RESULT_NO_RESPONSE_FROM_USER:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_FRAMES_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_send_sms(const struct tel_sat_tr_send_sms_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_NETWORK_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_SMS_RP_ERROR:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	case RESULT_INTRCTN_WITH_CC_OR_SMS_CTRL_PRMNT_PRBLM:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->cc_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_send_ss(const struct tel_sat_tr_send_ss_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
	case RESULT_SUCCESS_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM:
	case RESULT_USSD_OR_SS_TRANSACTION_TERMINATED_BY_USER:
		dst_tr[temp_index++] = 1 + src_tr->text.string_length;
		dst_tr[temp_index++] = src_tr->result_type;

		memcpy(&(dst_tr[temp_index]), src_tr->text.string, src_tr->text.string_length);
		encoded_len = src_tr->text.string_length;
		temp_index += encoded_len;
	break;
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_NETWORK_UNABLE_TO_PROCESS_COMMAND:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	case RESULT_INTRCTN_WITH_CC_OR_SMS_CTRL_PRMNT_PRBLM:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->cc_problem_type;
	break;

	case RESULT_SS_RETURN_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->ss_problem;
	break;

	case RESULT_BEYOND_ME_CAPABILITIES:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_send_ussd(const struct tel_sat_tr_send_ussd_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
	case RESULT_SUCCESS_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM:
	case RESULT_USSD_OR_SS_TRANSACTION_TERMINATED_BY_USER:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;

		encoded_len = _sat_encode_text_tlv(&(src_tr->text), dst_tr, temp_index, TRUE);
		temp_index += encoded_len;
	break;

	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_BEYOND_ME_CAPABILITIES:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_NETWORK_UNABLE_TO_PROCESS_COMMAND:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	case RESULT_INTRCTN_WITH_CC_OR_SMS_CTRL_PRMNT_PRBLM:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->cc_problem_type;
	break;

	case RESULT_USSD_RETURN_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->ussd_problem;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_setup_call(const struct tel_sat_tr_setup_call_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
	case RESULT_SUCCESS_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM:
	case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
	case RESULT_SS_RETURN_ERROR:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_USER_DID_NOT_ACCEPT_CALL_SETUP_REQ:
	case RESULT_USER_CLEAR_DOWN_CALL_BEFORE_CONN:
	case RESULT_FRAMES_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	case RESULT_NETWORK_UNABLE_TO_PROCESS_COMMAND:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->network_problem_type;
	break;

	case RESULT_INTRCTN_WITH_CC_OR_SMS_CTRL_PRMNT_PRBLM:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->cc_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_provide_local_info(const struct tel_sat_tr_provide_local_info_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);
	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_LIMITED_SERVICE:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
		switch (src_tr->command_detail.cmd_qualifier.provide_local_info.provide_local_info) {
		case LOCAL_INFO_DATE_TIME_AND_TIMEZONE:
			encoded_len = _sat_encode_date_time_and_timezone_tlv(&(src_tr->other.date_time_and_timezone), dst_tr, temp_index);
		break;

		case LOCAL_INFO_LANGUAGE:
			encoded_len = _sat_encode_language_tlv(src_tr->other.language, dst_tr, temp_index);
		break;

		default:
			dbg("local info type[%d] is not handled", src_tr->command_detail.cmd_qualifier.provide_local_info.provide_local_info);
		break;
		}

		temp_index += encoded_len;
	break;

	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_setup_event_list(const struct tel_sat_tr_setup_event_list_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);
	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_setup_idle_mode_text(const struct tel_sat_tr_setup_idle_mode_text_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_FRAMES_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_send_dtmf(const struct tel_sat_tr_send_dtmf_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
	case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_language_notification(const struct tel_sat_tr_language_notification_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_FRAMES_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_launch_browser(const struct tel_sat_tr_launch_browser_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);

	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
	case RESULT_BACKWARD_MOVE_BY_USER:
	case RESULT_NO_RESPONSE_FROM_USER:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_NETWORK_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_FRAMES_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	case RESULT_LAUNCH_BROWSER_GENERIC_ERROR_CODE:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->browser_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_open_channel(const struct tel_sat_tr_open_channel_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);
	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
	case RESULT_SUCCESS_WITH_MODIFICATION:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;

		/* set channel status */
		encoded_len = _sat_encode_channel_status_tlv(&(src_tr->channel_status), dst_tr, temp_index);
		temp_index += encoded_len;
	break;

	case RESULT_REFRESH_PRFRMD_BUT_INDICATED_USIM_NOT_ACTIVE:
	case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
	case RESULT_INTERACTION_WITH_CC_BY_SIM_IN_TMP_PRBLM:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
	case RESULT_USER_DID_NOT_ACCEPT_CALL_SETUP_REQ:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_NETWORK_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_FRAMES_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	case RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->bip_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		return 0;
	}

	return temp_index;
}

static int _sat_encode_close_channel(const struct tel_sat_tr_close_channel_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);
	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
	case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_FRAMES_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	case RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->bip_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_send_data(const struct tel_sat_tr_send_data_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);
	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
	case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
		encoded_len = _sat_encode_channel_data_length_tlv(&(src_tr->channel_data_len), dst_tr, temp_index);
		temp_index += encoded_len;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_NETWORK_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_FRAMES_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	case RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->bip_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_receive_data(const struct tel_sat_tr_receive_data_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);
	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
	case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
		encoded_len = _sat_encode_channel_data_tlv(&(src_tr->channel_data), dst_tr, temp_index);
		temp_index += encoded_len;
		encoded_len = _sat_encode_channel_data_length_tlv(&(src_tr->channel_data_len), dst_tr, temp_index);
		temp_index += encoded_len;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_FRAMES_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	case RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->bip_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_get_channel_status(const struct tel_sat_tr_get_channel_status_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);
	switch (src_tr->result_type) {
	case RESULT_SUCCESS:
	case RESULT_SUCCESS_WITH_PARTIAL_COMPREHENSION:
	case RESULT_SUCCESS_WITH_MISSING_INFO:
	case RESULT_SUCCESS_BUT_REQUESTED_ICON_NOT_DISPLAYED:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
		encoded_len = _sat_encode_channel_status_tlv(&(src_tr->channel_status), dst_tr, temp_index);
		temp_index += encoded_len;
	break;

	case RESULT_PROACTIVE_SESSION_TERMINATED_BY_USER:
	case RESULT_BEYOND_ME_CAPABILITIES:
	case RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
	case RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
	case RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING:
		dst_tr[temp_index++] = 1;
		dst_tr[temp_index++] = src_tr->result_type;
	break;

	case RESULT_ME_UNABLE_TO_PROCESS_COMMAND:
	case RESULT_NETWORK_UNABLE_TO_PROCESS_COMMAND:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->me_problem_type;
	break;

	case RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR:
		dst_tr[temp_index++] = 2;
		dst_tr[temp_index++] = src_tr->result_type;
		dst_tr[temp_index++] = src_tr->bip_problem_type;
	break;

	default:
		dbg("invalid response value[0x%x] in current TR", src_tr->result_type);
		temp_index = 0;
	break;
	}

	return temp_index;
}

static int _sat_encode_unsupport_command(const struct tel_sat_tr_unsupport_command_tlv *src_tr, char *dst_tr)
{
	int temp_index = 0, encoded_len = 0;

	/* set command detail info */
	encoded_len = _sat_encode_command_detail_tlv(&(src_tr->command_detail), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set device identities info */
	encoded_len = _sat_encode_device_identities_tlv(&(src_tr->device_id), dst_tr, temp_index);
	temp_index += encoded_len;

	/* set result info */
	dst_tr[temp_index++] = (b_comprehensive ? (SATK_RESULT_TAG | 0x80) : SATK_RESULT_TAG);
	dst_tr[temp_index++] = 1;
	dst_tr[temp_index++] = src_tr->result_type;

	return temp_index;
}

int tcore_sat_encode_terminal_response(const struct treq_sat_terminal_rsp_data *src_tr, char *dst_tr)
{
	int tr_len = 0;

	if (!dst_tr)
		return 0;

	switch (src_tr->cmd_type) {
	case SAT_PROATV_CMD_DISPLAY_TEXT:
		tr_len = _sat_encode_display_text(&(src_tr->terminal_rsp_data.display_text), dst_tr);
	break;

	case SAT_PROATV_CMD_GET_INKEY:
		tr_len = _sat_encode_get_inkey(&(src_tr->terminal_rsp_data.get_inkey), dst_tr);
	break;

	case SAT_PROATV_CMD_GET_INPUT:
		tr_len = _sat_encode_get_input(&(src_tr->terminal_rsp_data.get_input), dst_tr);
	break;

	case SAT_PROATV_CMD_MORE_TIME:
		tr_len = _sat_encode_more_time(&(src_tr->terminal_rsp_data.more_time), dst_tr);
	break;

	case SAT_PROATV_CMD_PLAY_TONE:
		tr_len = _sat_encode_play_tone(&(src_tr->terminal_rsp_data.play_tone), dst_tr);
	break;

	case SAT_PROATV_CMD_REFRESH:
		tr_len = _sat_encode_refresh(&(src_tr->terminal_rsp_data.refresh), dst_tr);
	break;

	case SAT_PROATV_CMD_SETUP_MENU:
		tr_len = _sat_encode_setup_menu(&(src_tr->terminal_rsp_data.setup_menu), dst_tr);
	break;

	case SAT_PROATV_CMD_SELECT_ITEM:
		tr_len = _sat_encode_select_item(&(src_tr->terminal_rsp_data.select_item), dst_tr);
	break;

	case SAT_PROATV_CMD_SEND_SMS:
		tr_len = _sat_encode_send_sms(&(src_tr->terminal_rsp_data.send_sms), dst_tr);
	break;

	case SAT_PROATV_CMD_SEND_SS:
		tr_len = _sat_encode_send_ss(&(src_tr->terminal_rsp_data.send_ss), dst_tr);
	break;

	case SAT_PROATV_CMD_SEND_USSD:
		tr_len = _sat_encode_send_ussd(&(src_tr->terminal_rsp_data.send_ussd), dst_tr);
	break;

	case SAT_PROATV_CMD_SETUP_CALL:
		tr_len = _sat_encode_setup_call(&(src_tr->terminal_rsp_data.setup_call), dst_tr);
	break;

	case SAT_PROATV_CMD_PROVIDE_LOCAL_INFO:
		tr_len = _sat_encode_provide_local_info(&(src_tr->terminal_rsp_data.provide_local_info), dst_tr);
	break;

	case SAT_PROATV_CMD_SETUP_EVENT_LIST:
		tr_len = _sat_encode_setup_event_list(&(src_tr->terminal_rsp_data.setup_event_list), dst_tr);
	break;

	case SAT_PROATV_CMD_SETUP_IDLE_MODE_TEXT:
		tr_len = _sat_encode_setup_idle_mode_text(&(src_tr->terminal_rsp_data.setup_idle_mode_text), dst_tr);
	break;

	case SAT_PROATV_CMD_SEND_DTMF:
		tr_len = _sat_encode_send_dtmf(&(src_tr->terminal_rsp_data.send_dtmf), dst_tr);
	break;

	case SAT_PROATV_CMD_LANGUAGE_NOTIFICATION:
		tr_len = _sat_encode_language_notification(&(src_tr->terminal_rsp_data.language_notification), dst_tr);
	break;

	case SAT_PROATV_CMD_LAUNCH_BROWSER:
		tr_len = _sat_encode_launch_browser(&(src_tr->terminal_rsp_data.launch_browser), dst_tr);
	break;

	case SAT_PROATV_CMD_OPEN_CHANNEL:
		tr_len = _sat_encode_open_channel(&(src_tr->terminal_rsp_data.open_channel), dst_tr);
	break;

	case SAT_PROATV_CMD_CLOSE_CHANNEL:
		tr_len = _sat_encode_close_channel(&(src_tr->terminal_rsp_data.close_channel), dst_tr);
	break;

	case SAT_PROATV_CMD_SEND_DATA:
		tr_len = _sat_encode_send_data(&(src_tr->terminal_rsp_data.send_data), dst_tr);
	break;

	case SAT_PROATV_CMD_RECEIVE_DATA:
		tr_len = _sat_encode_receive_data(&(src_tr->terminal_rsp_data.receive_data), dst_tr);
	break;

	case SAT_PROATV_CMD_GET_CHANNEL_STATUS:
		tr_len = _sat_encode_get_channel_status(&(src_tr->terminal_rsp_data.get_channel_status), dst_tr);
	break;

	default:
		tr_len = _sat_encode_unsupport_command(&(src_tr->terminal_rsp_data.unsupport_cmd), dst_tr);
	break;
	}

	return tr_len;
}

CoreObject *tcore_sat_new(TcorePlugin *p, const char *name,
	struct tcore_sat_operations *ops, TcoreHal *hal)
{
	CoreObject *o = NULL;
	struct private_object_data *po = NULL;

	if (!p)
		return NULL;

	o = tcore_object_new(p, name, hal);
	if (!o)
		return NULL;

	po = calloc(1, sizeof(struct private_object_data));
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	/* set ops to default type when core object is created. */
	po->ops[TCORE_OPS_TYPE_CP] = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_SAT);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_sat_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SAT);

	tcore_object_free(o);
}

void tcore_sat_set_ops(CoreObject *o, struct tcore_sat_operations *ops, enum tcore_ops_type ops_type)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SAT);
	CORE_OBJECT_VALIDATE_OPS_RETURN(ops_type);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		err("po is NULL");
		return;
	}

	po->ops[ops_type] = ops;
}
