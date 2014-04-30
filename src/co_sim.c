/*
 * libtcore
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "queue.h"
#include "core_object.h"
#include "co_sim.h"

typedef struct {
	TcoreSimOps *ops;

	TelSimCardType type; /**< Provides the SIM card type*/
	TelSimCardStatus sim_status; /**< Provides the card status*/
	TelSimCardChangeStatus change_status; /**< Provides SIM card change status */
	TelSimImsiInfo imsi; /**< Provides IMSI information*/
	gboolean is_cphs; /**< Whether current SIM is for CPHS or not*/
	TelSimServiceTable svct; /**< (U)SIM Service Table information*/
	gchar *spn;
	TelSimSpnDispCondition disp_condition;
	void *userdata; /**< free use data*/
} PrivateObject;

static TelReturn _dispatcher(CoreObject *co, TcoreCommand command,
	const void *request, TcoreObjectResponseCallback cb, const void *user_data)
{
	PrivateObject *po = NULL;
	po = tcore_object_ref_object(co);

	tcore_check_return_value_assert(po != NULL, TEL_RETURN_INVALID_PARAMETER);
	tcore_check_return_value_assert(po->ops != NULL, TEL_RETURN_INVALID_PARAMETER);

	dbg("command [0x%x]", command);

	switch (command) {
	case TCORE_COMMAND_SIM_GET_IMSI:
		if (po->ops->get_imsi)
			return po->ops->get_imsi(co, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_GET_ECC:
		if (po->ops->get_ecc)
			return po->ops->get_ecc(co, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_GET_ICCID:
		if (po->ops->get_iccid)
			return po->ops->get_iccid(co, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_GET_LANGUAGE:
		if (po->ops->get_language)
			return po->ops->get_language(co, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_SET_LANGUAGE:
		if (po->ops->set_language)
			return po->ops->set_language(co, *(TelSimLanguagePreferenceCode *) request, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_GET_CALL_FORWARDING_INFO:
		if (po->ops->get_callforwarding_info)
			return po->ops->get_callforwarding_info(co, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_GET_MESSAGE_WAITING_INFO:
		if (po->ops->get_messagewaiting_info)
			return po->ops->get_messagewaiting_info(co, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_SET_MESSAGE_WAITING_INFO:
		if (po->ops->set_messagewaiting_info)
			return po->ops->set_messagewaiting_info(co, (TelSimMwis *)request, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_GET_MAILBOX_INFO:
		if (po->ops->get_mailbox_info)
			return po->ops->get_mailbox_info(co, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_SET_MAILBOX_INFO:
		if (po->ops->set_mailbox_info)
			return po->ops->set_mailbox_info(co, (TelSimMailBoxNumber *)request, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_GET_MSISDN:
		if (po->ops->get_msisdn)
			return po->ops->get_msisdn(co, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_GET_SPN:
		if (po->ops->get_spn)
			return po->ops->get_spn(co, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_GET_CPHS_NET_NAME:
		if (po->ops->get_cphs_netname)
			return po->ops->get_cphs_netname(co, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_GET_SP_DISPLAY_INFO:
		if (po->ops->get_sp_display_info)
			return po->ops->get_sp_display_info(co, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_REQ_AUTHENTICATION:
		if (po->ops->req_authentication)
			return po->ops->req_authentication(co, (TelSimAuthenticationData *)request, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_VERIFY_PINS:
		if (po->ops->verify_pins)
			return po->ops->verify_pins(co, (TelSimSecPinPw *)request, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_VERIFY_PUKS:
		if (po->ops->verify_puks)
			return po->ops->verify_puks(co, (TelSimSecPukPw *)request, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_CHANGE_PINS:
		if (po->ops->change_pins)
			return po->ops->change_pins(co, (TelSimSecChangePinPw *)request, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_DISABLE_FACILITY:
		if (po->ops->disable_facility)
			return po->ops->disable_facility(co, (TelSimFacilityPw *)request, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_ENABLE_FACILITY:
		if (po->ops->enable_facility)
			return po->ops->enable_facility(co, (TelSimFacilityPw *)request, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_GET_FACILITY:
		if (po->ops->get_facility)
			return po->ops->get_facility(co, *(TelSimLockType *)request, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_GET_LOCK_INFO:
		if (po->ops->get_lock_info)
			return po->ops->get_lock_info(co, *(TelSimLockType *)request, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_REQ_APDU:
		if (po->ops->req_apdu)
			return po->ops->req_apdu(co, (TelSimApdu *)request, cb, (void *)user_data);
	break;
	case TCORE_COMMAND_SIM_REQ_ATR:
		if (po->ops->req_atr)
			return po->ops->req_atr(co, cb, (void *)user_data);
	break;
	default:
		err("Unsupported operation");
		return  TEL_RETURN_INVALID_PARAMETER;
	}

	err("Operation NOT Permitted");
	return TEL_RETURN_OPERATION_NOT_SUPPORTED;
}

static void _po_clone_hook(CoreObject *src, CoreObject *dest)
{
	PrivateObject *src_po = NULL;
	PrivateObject *dest_po = NULL;

	tcore_check_return_assert(src != NULL);
	tcore_check_return_assert(dest != NULL);

	src_po = tcore_object_ref_object(src);
	if (NULL == src_po) {
		err("invalid source private object");
		tcore_object_link_object(dest, NULL);
		return;
	}

	tcore_check_return_assert(src_po->ops != NULL);

	dest_po = tcore_malloc0(sizeof(PrivateObject));
	dest_po->ops = tcore_memdup(src_po->ops, sizeof(TcoreSimOps));

	tcore_object_link_object(dest, dest_po);
}

static void _po_free_hook(CoreObject *co)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return(po != NULL);

	tcore_free(po->spn);
	tcore_free(po->ops);
	tcore_free(po);
	tcore_object_link_object(co, NULL);
}

/**
 * According to ETSI 102.221 ( 3GPP specification refers it ), EF-ICCID is coded by BCD, left justified and padded with 'F'.
 * This EF is mandatory and byte length is fixed with 10 bytes. So actual maximum length of ICCID is 20 digits.
 */
gboolean tcore_sim_decode_iccid(unsigned char *enc_iccid, int enc_iccid_len, char *dec_iccid)
{
	int bcd_byte = 0;
	guint char_length = 0;

	tcore_check_return_value_assert(enc_iccid != NULL, FALSE);
	tcore_check_return_value_assert(enc_iccid_len != 0, FALSE);
	tcore_check_return_value_assert(dec_iccid != NULL, FALSE);

	if (enc_iccid_len == 0 || enc_iccid_len > 10) {
		err("invalid length");
		return FALSE;
	}
	memset((void*) dec_iccid, 0, TEL_SIM_ICCID_LEN_MAX+1);

	bcd_byte = (int) tcore_util_get_valid_bcd_byte(enc_iccid, enc_iccid_len);

	char_length = tcore_util_convert_bcd_to_digit(dec_iccid, (char*)enc_iccid, bcd_byte);
	dbg("ICCID string length is[%d]", char_length);

	dec_iccid[char_length] = '\0';

	return TRUE;
}

/**
 * This function is used to decode EFLP (2G)
 */
gboolean tcore_sim_decode_lp(unsigned char *enc_lang, int enc_lang_len, TelSimLanguagePreferenceCode *dec_lang)
{
	tcore_check_return_value_assert(enc_lang != NULL, FALSE);
	tcore_check_return_value_assert(enc_lang_len != 0, FALSE);

	*dec_lang = TEL_SIM_LP_LANG_UNSPECIFIED;
	/*Consider only highest priority language */
	if (enc_lang[0] != 0xFF)
		*dec_lang = (TelSimLanguagePreferenceCode)enc_lang[0];

	dbg("enc_lang_len [%d] language[0x%x]", enc_lang_len, *dec_lang);
	return TRUE;
}

/**
 * This function is used to encode EFLP (2G)
 */
gboolean tcore_sim_encode_lp(TelSimLanguagePreferenceCode dec_lang,
	char **enc_lang, int *enc_lang_len)
{
	char *language_code[] = {"00", "01", "02", "03", "04", "05", "06", "07",
		"08", "09", "10", "11","12", "13", "14", "15", "16", "17", "18"};

	tcore_check_return_value_assert(enc_lang != NULL, FALSE);
	tcore_check_return_value_assert(enc_lang_len != NULL, FALSE);

	*enc_lang = g_strdup_printf("%s", language_code[dec_lang]);
	*enc_lang_len = 1;
	dbg("Encoded Language: [%s] Length: [%d]", *enc_lang, *enc_lang_len);

	return TRUE;
}

/**
 * This function is used to decode LI (3G)
 */
gboolean tcore_sim_decode_li(unsigned char *enc_lang, int enc_lang_len,
	TelSimFileId file_id, TelSimLanguagePreferenceCode *dec_lang)
{
	gint default_li;

	tcore_check_return_value_assert(enc_lang != NULL, FALSE);
	tcore_check_return_value_assert(enc_lang_len != 0, FALSE);
	tcore_check_return_value_assert(dec_lang != NULL, FALSE);

	*dec_lang = TEL_SIM_LP_LANG_UNSPECIFIED;

	/*
	 * Description of problem: language decoding was not correctly done
	 * if we used 7layers's test SIM Patch Description : TS31.102
	 * If the EFLI has the value 'FFFF' in its highest priority position,
	 * then the preferred language selection shall be the language
	 * preference in the EFPL
	 */
	if ((file_id == TEL_SIM_EF_USIM_LI)
			|| (file_id == TEL_SIM_EF_LP)) {
		default_li = enc_lang[0];
		default_li = ((default_li << 8) & 0xFF00) + enc_lang[1];

		/* Validate the 'default' language */
		if (default_li == 0xFFFF) {
			dbg("'default' Laguage is UNDEFINED: [0x%x]",
				default_li);
			return FALSE;
		}
	}

	if (enc_lang[0] == 'e') {
		switch (enc_lang[1]) {
		case 'n':
			*dec_lang = TEL_SIM_LP_ENGLISH;
		break;
		case 's':
			*dec_lang = TEL_SIM_LP_SPANISH;
		break;
		case 'l':
			*dec_lang = TEL_SIM_LP_GREEK;
		break;
		}
	} else if (enc_lang[0] == 'd') {
		switch (enc_lang[1]) {
		case 'e':
			*dec_lang = TEL_SIM_LP_GERMAN;
		break;
		case 'a':
			*dec_lang = TEL_SIM_LP_DANISH;
		break;
		}
	} else if (enc_lang[0] == 'p') {
		switch (enc_lang[1]) {
		case 'l':
			*dec_lang = TEL_SIM_LP_POLISH;
		break;
		case 't':
			*dec_lang = TEL_SIM_LP_PORTUGUESE;
		break;
		}
	} else if (enc_lang[0] == 'i' && enc_lang[1] == 't') {
		*dec_lang = TEL_SIM_LP_ITALIAN;
	} else if (enc_lang[0] == 'f' && enc_lang[1] == 'r') {
		*dec_lang = TEL_SIM_LP_FRENCH;
	} else if (enc_lang[0] == 'n' && enc_lang[1] == 'l') {
		*dec_lang = TEL_SIM_LP_DUTCH;
	} else if (enc_lang[0] == 's' && enc_lang[1] == 'v') {
		*dec_lang = TEL_SIM_LP_SWEDISH;
	} else if (enc_lang[0] == 'f' && enc_lang[1] == 'i') {
		*dec_lang = TEL_SIM_LP_FINNISH;
	} else if (enc_lang[0] == 'n' && enc_lang[1] == 'o') {
		*dec_lang = TEL_SIM_LP_NORWEGIAN;
	} else if (enc_lang[0] == 't' && enc_lang[1] == 'r') {
		*dec_lang = TEL_SIM_LP_TURKISH;
	} else if (enc_lang[0] == 'h' && enc_lang[1] == 'u') {
		*dec_lang = TEL_SIM_LP_HUNGARIAN;
	} else if (enc_lang[0] == 'k' && enc_lang[1] == 'o') {
		*dec_lang = TEL_SIM_LP_KOREAN;
	} else if (enc_lang[0] == 'z' && enc_lang[1] == 'h') {
		*dec_lang = TEL_SIM_LP_CHINESE;
	} else if (enc_lang[0] == 'r' && enc_lang[1] == 'u') {
		*dec_lang = TEL_SIM_LP_RUSSIAN;
	} else if (enc_lang[0] == 'j' && enc_lang[1] == 'a') {
		*dec_lang = TEL_SIM_LP_JAPANESE;
	}

	dbg( "Language %d ", *dec_lang);
	return TRUE;
}

/**
 * This function is used to encode EFLI (3G)
 */
gboolean tcore_sim_encode_li(TelSimLanguagePreferenceCode dec_lang,
	char **en_lang, int *en_lang_len)
{
	char *language_code[] = {"de", "en", "it", "fr", "es", "nl", "sv", "da",
		"pt", "fi", "no", "el", "tr", "hu", "pl", "ko", "zh", "ru", "ja" };

	tcore_check_return_value_assert(en_lang != NULL, FALSE);
	tcore_check_return_value_assert(en_lang_len != NULL, FALSE);

	*en_lang_len = 0;

	if (dec_lang <= TEL_SIM_LP_JAPANESE) {
		*en_lang = (char *) tcore_strdup((gchar *)language_code[dec_lang]);
		*en_lang_len = strlen(*en_lang);
		dbg("Encoded Language: [%s] Length: [%d]", *en_lang, *en_lang_len);
	}

	return TRUE;
}

gboolean tcore_sim_decode_imsi(unsigned char *en_imsi, int en_imsi_len, TelSimImsiInfo *dec_imsi)
{
	int i, j=0;
	char imsi_raw[16];
	unsigned int mnc_len = 0;

	dbg( "Func Entrance");

	tcore_check_return_value_assert(dec_imsi != NULL, FALSE);
	tcore_check_return_value_assert(en_imsi != NULL, FALSE);

	/*
		According to 3GPP specification, the length of raw IMSI data is 9 bytes.
		first byte is length of IMSI
		second byte byte has parity nibble, so second byte has one digit of IMSI. other byte has two digit of IMSI
	*/
	if ((en_imsi_len == 0) || (en_imsi_len == 0xff) || (4 > en_imsi_len) || (9 <en_imsi_len)) {
		dbg("No valid IMSI present to convert - length:[%x]",	en_imsi_len);
		return FALSE;
	}
	dbg("imsi length[%d], input data length[%d]", en_imsi[0], en_imsi_len);

	/* Decode IMSI value from nibbles */
	for (i = 1; i < en_imsi_len; i++) {
		if (i == 1) { /* first byte, ignore lower nibble */
			imsi_raw[j++] = ((en_imsi[i] & 0xF0) >> 4) + '0';
		} else if (i == en_imsi[0]+1) { /* last byte */
			imsi_raw[j++] = (en_imsi[i] & 0x0F)+ '0';
			if (en_imsi[0]%2) /* count the last one if odd digits */
				imsi_raw[j++] = ((en_imsi[i] & 0xF0) >> 4) + '0';
		} else {
			imsi_raw[j++] = (en_imsi[i]  & 0x0F) + '0';
			imsi_raw[j++] = ((en_imsi[i] & 0xF0) >> 4) + '0';
		}
	}
	/* Terminate string */
	imsi_raw[j] = '\0';
	dbg("imsi_raw[%s], size[%d]", imsi_raw, strlen(imsi_raw));

	mnc_len = strlen(imsi_raw) - TEL_SIM_MSIN_LEN_MAX - TEL_SIM_MCC_MNC_LEN_MAX;

	memcpy(dec_imsi->mcc, imsi_raw, TEL_SIM_MCC_MNC_LEN_MAX);
	dec_imsi->mcc[TEL_SIM_MCC_MNC_LEN_MAX] = '\0';
	memcpy(dec_imsi->mnc, imsi_raw+TEL_SIM_MCC_MNC_LEN_MAX, mnc_len);
	dec_imsi->mnc[mnc_len] = '\0';
	memcpy(dec_imsi->msin, imsi_raw+TEL_SIM_MCC_MNC_LEN_MAX+mnc_len, TEL_SIM_MSIN_LEN_MAX);
	dec_imsi->msin[TEL_SIM_MSIN_LEN_MAX] = '\0';

	dbg("dec_imsi->mcc[%s], dec_imsi->mcc[%s], dec_imsi->msin[%s]",
				dec_imsi->mcc, dec_imsi->mnc, dec_imsi->msin);

	return TRUE;
}

gboolean tcore_sim_decode_sst(unsigned char *enc_sst, int enc_sst_len, char *dec_sst)
{
	unsigned char sst_byte, rast, mask = 0;
	char sim_service_id = 1;	// set "CHV1 disable function"
	int i, svc_count;

	tcore_check_return_value_assert(enc_sst != NULL, FALSE);
	tcore_check_return_value_assert(enc_sst_len != 0, FALSE);

	memset((void*)dec_sst, 0, TEL_SIM_SST_SERVICE_CNT_MAX);

	if (enc_sst_len == 0 || enc_sst_len > TEL_SIM_SST_LEN_MAX) {
		err("invalid length");
		return FALSE;
	}

	// get count of SIM service id. one byte has four service status.
	svc_count = enc_sst_len * 4;

	/*3GPP 51.011 SST shows 56 kinds of service types. current char *p_sst has also 56 elements*/
	if (svc_count > TEL_SIM_SST_SERVICE_CNT_MAX)
		svc_count = TEL_SIM_SST_SERVICE_CNT_MAX;

	for (i = 0; i < svc_count; i++) {
		sst_byte = enc_sst[(sim_service_id - 1) / 4];
		rast = sim_service_id - 4 * (sim_service_id / 4);

		switch (rast) {
		case 0:
			mask = 0x80;
		break;
		case 1:
			mask = 0x02;
		break;
		case 2:
			mask = 0x08;
		break;
		case 3:
			mask = 0x20;
		break;
		}

		if (sst_byte & mask)
			*dec_sst = 1;
		else
			*dec_sst = 0;

		*dec_sst += sizeof(char);
		sim_service_id++;	// next service id
	}
	return TRUE;
}

gboolean tcore_sim_decode_spn(unsigned char *enc_spn, int enc_spn_len, TelSimSpn *dec_spn)
{
	int i;

	tcore_check_return_value_assert(enc_spn != NULL, FALSE);
	tcore_check_return_value_assert(enc_spn_len != 0, FALSE);
	tcore_check_return_value_assert(dec_spn != NULL, FALSE);

	dec_spn->display_condition = enc_spn[0];
	dbg( "The display condition is [%d]", dec_spn->display_condition);

	for (i = 1; i < TEL_SIM_SPN_LEN_MAX + 1; i++) {
		if (enc_spn[i] == 0xFF)
			break; /* loop break*/

		dec_spn->spn[i - 1] = enc_spn[i];
		dbg("EF-SPN name[%d][%c]", i, enc_spn[i]);
	}
	dec_spn->spn[i-1] = '\0';

	return TRUE;
}

gboolean tcore_sim_decode_spdi(unsigned char *enc_spdi, int enc_spdi_len, TelSimSpPlmnList *dec_spdi)
{
	unsigned int i, src_plmn_start_len;

	tcore_check_return_value_assert(enc_spdi != NULL, FALSE);
	tcore_check_return_value_assert(enc_spdi_len != 0, FALSE);
	tcore_check_return_value_assert(dec_spdi != NULL, FALSE);

	if (enc_spdi[0] == 0xff){
		dbg("file is exist but there is no valid records");
		dec_spdi->count = 0;
		dec_spdi->list = NULL;
		return TRUE;
	}

	//Display info tag('A3')
	if (enc_spdi[0] == 0xA3) {
		 //PLMN list tag('80')
		if (enc_spdi[2] == 0x80) {
			dec_spdi->count = enc_spdi[3] / 3;

			/*
			 *  plmn tag 1byte
			 *  length 1byte
			 *  each plmn entry 3byte
			 */
			if (dec_spdi->count > TEL_SIM_SPDI_PLMN_MAX)
				dec_spdi->count = TEL_SIM_SPDI_PLMN_MAX;

			src_plmn_start_len = 4;

			dbg( "dec_spdi->num_of_plmn_entries[%d]", dec_spdi->count);
			/* Allocate resources */
			dec_spdi->list = tcore_malloc0(sizeof(TelSimSpPlmn) * dec_spdi->count);

			for (i = 0; i < dec_spdi->count; i++) {
				unsigned char packet_in_digit[3 * 2 + 1];
				tcore_util_convert_bcd_to_digit((char*) packet_in_digit, (char*) &enc_spdi[src_plmn_start_len], 3);
				/* Allocate resources */
				dec_spdi->list[i].plmn = tcore_malloc0(6 + 1);
				// get MCC (mobile country code)
				memcpy(dec_spdi->list[i].plmn, &(packet_in_digit[0]), 6);
				dec_spdi->list[i].plmn[6] = '\0';
				dbg( "SPDI PLMN[%d][%s]", i, dec_spdi->list[i].plmn);

				src_plmn_start_len = src_plmn_start_len + 3;
			}
			return TRUE;
		}
		dbg("Current EF-SPDI has invalid data");
		return FALSE;
	}
	dbg("Current EF-SPDI has invalid data");
	return FALSE;
}

gboolean tcore_sim_decode_msisdn(unsigned char *enc_msisdn,
	int enc_msisdn_len, TelSimSubscriberInfo *dec_msisdn)
{
	int X = 0;	// alpha id max length
	int alpha_id_length = 0;
	int value_length = 0;
	int bcd_byte = 0;	// dialing number max length

	tcore_check_return_value_assert(enc_msisdn != NULL, FALSE);
	tcore_check_return_value_assert(enc_msisdn_len != 0, FALSE);
	tcore_check_return_value_assert(dec_msisdn != NULL, FALSE);

	if (tcore_util_is_empty(enc_msisdn, enc_msisdn_len) == TRUE) {
		err("empty record");
		return FALSE;
	}

	X = enc_msisdn_len - 14;	// get alpha id max length

	if (X != 0) {
		alpha_id_length = X;
		dbg("alpha_id_length[%d]", alpha_id_length);
		if (alpha_id_length > TEL_SIM_XDN_ALPHA_ID_LEN_MAX)
			alpha_id_length = TEL_SIM_XDN_ALPHA_ID_LEN_MAX;

		dec_msisdn->alpha_id = tcore_malloc0(alpha_id_length + 1);
		value_length = tcore_util_get_string((guchar *)dec_msisdn->alpha_id,
			enc_msisdn, alpha_id_length);
		dec_msisdn->alpha_id[value_length] = '\0';
	}

	// get dialing number length
	// in[X] is BCD length of dialing number length plus TON/NPI 1 bytes.
	// Convert to digit length and subtract TON/NPI length.
	if (enc_msisdn[X] != 0xFF) {
		dbg( "Dialing number Length %d, BCD length 0x%x ",  (enc_msisdn[X] - 1) * 2, enc_msisdn[X]);

		// get TON and NPI
		//p_msisdn->ton = (in[X + 1] >> 4) & 0x07;

		// get actual dialing number length
		bcd_byte = (int) tcore_util_get_valid_bcd_byte(&enc_msisdn[X + 2], TEL_SIM_XDN_NUMBER_LEN_MAX / 2);
		dbg( "bcd_byte[%x]", bcd_byte);

		// get dialing number/SSC string
		dec_msisdn->num = tcore_malloc0((bcd_byte * 2) + 1);
		value_length = tcore_util_convert_bcd_to_digit((char*)dec_msisdn->num,
			(char*) &enc_msisdn[X + 2], bcd_byte); // actual dialing number length in BCD.
		dec_msisdn->num[value_length] = '\0';
		dbg( "dec_msisdn->num[%s]", dec_msisdn->num);
	}
	return TRUE;
}

gboolean tcore_sim_decode_xdn(unsigned char *enc_xdn, int enc_xdn_len,
	char *alpha_id, unsigned int *alpha_id_len, char *num)
{
	int X = 0;	// alpha id max length
	int bcd_byte;	// dialing number max length

	memset((void*) alpha_id, 0, TEL_SIM_XDN_ALPHA_ID_LEN_MAX);
	memset((void*) num, 0, TEL_SIM_XDN_NUMBER_LEN_MAX);

	tcore_check_return_value_assert(enc_xdn != NULL, FALSE);
	tcore_check_return_value_assert(enc_xdn_len != 0, FALSE);

	if (tcore_util_is_empty(enc_xdn, enc_xdn_len) == TRUE) {
		err("empty record");
		return FALSE;	// this is empty record
	}

	X = enc_xdn_len - 14;	// get alpha id max length
	*alpha_id_len = 0;

	if (X != 0) {
		*alpha_id_len = tcore_util_get_string((unsigned char *)alpha_id, enc_xdn, X);
		dbg("alpha_id_len:[%d]", *alpha_id_len);
	}

	// get dialing number length
	// in[X] is BCD length of dialing number length plus TON/NPI 1 bytes.
	// Convert to digit length and subtract TON/NPI length.
	if (enc_xdn[X] != 0xFF) {
		dbg( "Dialing number Length %d, BCD length 0x%x ", (enc_xdn[X] - 1) * 2, enc_xdn[X]);

/*		if (p_xdn->num_max_len > SIM_XDN_NUMBER_LEN_MAX)	{

			 this may be broken record.
			 p_xdn->b_used = FALSE;
			 memset((void*)p_xdn, 0, sizeof(tapi_sim_dialing_number_info_t));
			 return FALSE;

			 ADN record cannot have more than 20 digits. Anyway we can restrict this as per 31.102
			 X+1	Length of BCD number/SSC contents	M	1 byte
			 X+2	TON and NPI	M	1 byte
			 X+3 to X+12	Dialing Number/SSC String	M	10 bytes
			 X+13	Capability/Configuration1 Identifier	M	1 byte
			 X+14	Extension1 Record Identifier	M	1 byte

			 Anyway we are doing this check @
			 bcd_byte = (int) tcore_util_get_valid_bcd_byte (&in[X+2], TAPI_SIM_XDN_DIALING_NUMBER_LEN/2);
			 by using the 20/2; so don`t return false.

			if (in[X] == 0x00)
				p_xdn->num_max_len = 0;
			else
				p_xdn->num_max_len = SIM_XDN_NUMBER_LEN_MAX;
		}*/

		// get TON and NPI
		//p_xdn->ton = (in[X + 1] >> 4) & 0x07;
		//p_xdn->npi = in[X + 1] & 0x0F;

		// get actual dialing number length
		bcd_byte = (int) tcore_util_get_valid_bcd_byte(&enc_xdn[X + 2], TEL_SIM_XDN_NUMBER_LEN_MAX / 2);
		dbg( "bcd_byte[%x]", bcd_byte);

		// get dialing number/SSC string
		tcore_util_convert_bcd_to_digit((char*) num, (char*) &enc_xdn[X + 2], bcd_byte); // actual dialing number length in BCD.
		dbg( "p_xdn->DiallingNum[%s]", num);
		// get Capability/Configuration id
		//p_xdn->cc_id = enc_xdn[X + 12];
		// get Extension1 id
		//p_xdn->ext1_id = enc_xdn[X + 13];
	}
	return TRUE;
}

gboolean tcore_sim_encode_xdn(gchar *alpha_id, gchar *number, guint enc_xdn_len,
	gchar **enc_data)
{
#define TEL_SIM_CCI_OFFSET	12
#define TEL_SIM_ERI_OFFSET	13

	gint alpha_id_length = 0;
	gchar *enc_xdn;

	enc_xdn = tcore_malloc0(enc_xdn_len + 1);
	memset((void*)enc_xdn, 0xFF, enc_xdn_len);

	/* Alpha ID length */
	alpha_id_length = enc_xdn_len - 14;
	if (alpha_id_length < 0)
		alpha_id_length = 0;

	/* Alpha ID */
	if (alpha_id) {
		guint str_len = strlen(alpha_id);

		if (str_len) {
			/* Validate Alpha ID length */
			if (alpha_id_length < str_len) {
				/* alpha id is too big */
				dbg("SIM space for alpha_id is [%d] but input alpha_id " \
					"length is [%d]. so we will use [%d] byte",
					alpha_id_length, str_len, alpha_id_length);
				str_len = alpha_id_length;
			}

			/* Set Alpha ID */
			tcore_util_set_string((unsigned char *)enc_xdn,
				(unsigned char *)alpha_id, str_len);
		}
	}

	/* Dialing number/SSC string */
	if (number) {
		guchar number_index = 0;
		guint number_length = 0;

		if (number[0] == '+')
			number_index = 1;

		number_length = strlen(&number[number_index]);
		if (number_length) {
			gchar bcd_number[TEL_SIM_XDN_NUMBER_LEN_MAX / 2];
			memset((void*)bcd_number, 0xFF, TEL_SIM_XDN_NUMBER_LEN_MAX / 2);

			/* Validate Dialing number/SSC string length */
			if (number_length > TEL_SIM_XDN_NUMBER_LEN_MAX) {
				dbg("SIM space for number is [%d] but input number length " \
					"is [%d]. so we will use [%d] byte",
					TEL_SIM_XDN_NUMBER_LEN_MAX,
					number_length, TEL_SIM_XDN_NUMBER_LEN_MAX);
				number_length = TEL_SIM_XDN_NUMBER_LEN_MAX;
			}

			tcore_util_convert_digit_to_bcd((char*)bcd_number,
				(char *)&number[number_index], number_length);

			/* Set length of BCD number/SSC contents */
			enc_xdn[alpha_id_length] = ( (number_length + 1) / 2 ) + 1;

			/* Set TON and NPI */
			enc_xdn[alpha_id_length + 1] = 0x80;
			enc_xdn[alpha_id_length + 1] |= (number_index & 0x07) << 4;
			enc_xdn[alpha_id_length + 1] |= (number_index & 0x0F);

			memcpy((void*)&enc_xdn[alpha_id_length + 2],
				bcd_number, TEL_SIM_XDN_NUMBER_LEN_MAX / 2);
		}
	}

	/* Set Capability/Configuration Identifier */
	enc_xdn[alpha_id_length + TEL_SIM_CCI_OFFSET] = 0xFF;

	/* Set Extension1 Record Identifier */
	enc_xdn[alpha_id_length + TEL_SIM_ERI_OFFSET] = 0xFF;

	*enc_data = enc_xdn;
	enc_xdn[enc_xdn_len] = '\0';

	return TRUE;
}

gboolean tcore_sim_decode_ecc(unsigned char *enc_ecc, int enc_ecc_len, TelSimEccList *dec_ecc)
{
	int bcd_byte;	// dialing number max length
	int i;
	int valid_ecc_length;

	tcore_check_return_value_assert(enc_ecc != NULL, FALSE);
	tcore_check_return_value_assert(enc_ecc_len != 0, FALSE);
	tcore_check_return_value_assert(dec_ecc != NULL, FALSE);

	memset((void*)dec_ecc, 0x00, sizeof(TelSimEccList));

	if (enc_ecc_len%3 != 0) {
		err("error - invalid data length");
		return FALSE;
	}

	for(i=0; i < enc_ecc_len/3; i++){
		//get the BCD length of the ECC
		bcd_byte = (int) tcore_util_get_valid_bcd_byte((unsigned char*) enc_ecc+(i*3), 3);
		if (bcd_byte != 0) {
			valid_ecc_length = tcore_util_convert_bcd_to_digit(dec_ecc->list[dec_ecc->count].number, (char *)enc_ecc+(i*3), bcd_byte);
			dec_ecc->list[dec_ecc->count].number[valid_ecc_length] = '\0';
			dec_ecc->count++;
		}
	}
	return TRUE;
}

gboolean tcore_sim_decode_ust(unsigned char *enc_ust, int enc_ust_len, char *dec_ust)
{
	int i, j;
	char mask;

	tcore_check_return_value_assert(enc_ust != NULL, FALSE);
	tcore_check_return_value_assert(enc_ust_len != 0, FALSE);

	memset((void*) dec_ust, 0, TEL_SIM_UST_SERVICE_CNT_MAX);

/*	UST service is described to 74(1 byte includes 8 service status) in 31.102 r7.
	current sim_ust_s has 64 services. so enc_ust_len should be under 8 byte. */
	if (enc_ust_len > TEL_SIM_UST_BYTE_LEN_MAX)
		enc_ust_len = TEL_SIM_UST_BYTE_LEN_MAX;

	for (i = 0; i < enc_ust_len; i++) {
		mask = 0x01;	// reset mast to check first bit

		for (j = 0; j < 8; j++) {
			if (enc_ust[i] & mask) {
				*dec_ust = 1;
			}
			dec_ust += sizeof(char);
			mask = mask << 1;
		}
	}
	return TRUE;
}

gboolean tcore_sim_decode_uecc(unsigned char* enc_uecc, int enc_uecc_len, TelSimEcc *dec_ecc)
{
	int bcd_byte;	// dialing number max length
	unsigned char ecc_service_category;

	tcore_check_return_value_assert(enc_uecc != NULL, FALSE);
	tcore_check_return_value_assert(enc_uecc_len != 0, FALSE);
	tcore_check_return_value_assert(dec_ecc != NULL, FALSE);

	if (tcore_util_is_empty(enc_uecc, enc_uecc_len) == TRUE) {
		err("empty record");
		return FALSE;
	}

	//get the BCD length of the ECC
	bcd_byte = (int) tcore_util_get_valid_bcd_byte(&enc_uecc[0], TEL_SIM_ECC_BYTE_LEN_MAX);

	//get the ECC codes in digits and the length as well
	tcore_util_convert_bcd_to_digit((char*) dec_ecc->number, (char*) &enc_uecc[0], bcd_byte);

	//get the alpha identifier of ECC (
	tcore_util_get_string((unsigned char*) dec_ecc->name, (unsigned char*) &enc_uecc[3], enc_uecc_len - 3);

	ecc_service_category = enc_uecc[enc_uecc_len - 1] & 0xFF;

	/*
	 Assign the service category
	 3GPP TS24.008 Emergency Service Category Value.
	 Bit 8,7,6 are spare, 5~1 bit is used.
	 The meaning of the Emergency Category Value is derived from the following settings
	 (see 3GPP TS 22.101 clause 10):
	 Bit 1	Police 0x01
	 Bit 2	Ambulance 0x02
	 Bit 3	Fire Brigade 0x04
	 Bit 4	Marine Guard 0x08
	 Bit 5	Mountain Rescue 0x10
	 Bit 6	manually initiated eCall
	 Bit 7	automatically initiated eCall
	 Bit 8	is spare and set to "0"
	 */
	switch (ecc_service_category) {
	case 0x01:
		dec_ecc->category = TEL_SIM_ECC_ESC_POLICE;
	break;
	case 0x02:
		dec_ecc->category = TEL_SIM_ECC_ESC_AMBULANCE;
	break;
	case 0x04:
		dec_ecc->category = TEL_SIM_ECC_ESC_FIREBRIGADE;
	break;
	case 0x08:
		dec_ecc->category = TEL_SIM_ECC_ESC_MARINEGUARD;
	break;
	case 0x10:
		dec_ecc->category = TEL_SIM_ECC_ESC_MOUNTAINRESCUE;
	break;
	case 0x20:
		dec_ecc->category = TEL_SIM_ECC_ESC_MANUAL_ECALL;
	break;
	case 0x40:
		dec_ecc->category = TEL_SIM_ECC_ESC_AUTO_ECALL;
	break;
	default:
		dec_ecc->category = TEL_SIM_ECC_ESC_DEFAULT;
		dbg("default ecc->category");
	}
	return TRUE;
}

gboolean tcore_sim_decode_mbi(unsigned char *enc_mbi, int enc_mbi_len, TelSimMbi *dec_mbi)
{
	tcore_check_return_value_assert(enc_mbi != NULL, FALSE);
	tcore_check_return_value_assert(enc_mbi_len != 0, FALSE);
	tcore_check_return_value_assert(dec_mbi != NULL, FALSE);

	/* EF-MBI is defined 4 mandatory, 1 optional byte in 31.102 */
	if (enc_mbi_len == 0 || enc_mbi_len > TEL_SIM_MAIL_BOX_IDENTIFIER_LEN_MAX) {
		err("invalid length");
		return FALSE;
	}

	if ( tcore_util_is_empty(enc_mbi, enc_mbi_len) == TRUE) {
		err("empty record");
		return FALSE; // this is empty record
	}

	dec_mbi->voice_index = enc_mbi[0];
	dec_mbi->fax_index = enc_mbi[1];
	dec_mbi->email_index = enc_mbi[2];
	dec_mbi->other_index = enc_mbi[3];

	// 5th byte is optional
	if (enc_mbi_len == 5)
		dec_mbi->video_index = enc_mbi[4];

	return TRUE;
}

gboolean tcore_sim_decode_cff(unsigned char *enc_cff, int enc_cff_len, TelSimMwis *dec_mwis)
{
	int i;
	unsigned char type = 0;
	unsigned char mask = 0x01;

	tcore_check_return_value_assert(enc_cff != NULL, FALSE);
	tcore_check_return_value_assert(enc_cff_len != 0, FALSE);
	tcore_check_return_value_assert(dec_mwis != NULL, FALSE);

	memset((void*) dec_mwis, 0, sizeof(TelSimMwis));
	/* 1st byte is mandatory */
	type = enc_cff[0];

	if (type) {
		for (i = 0; i < 2; i++) {
			switch (type & mask) {
			case 0x01:
				dec_mwis->profile_id = 1;
				dec_mwis->msg_waiting[0].indication = TRUE;
			break;
			case 0x05:
				dec_mwis->profile_id = 2;
				dec_mwis->msg_waiting[0].indication = TRUE;
			break;
			default:
				dbg("CFF is NONE");
			}
			mask = mask << 1;
		}
	}

	/* 2nd byte is optional */
	if (enc_cff_len > 1) {
		type = enc_cff[1];
		if (type) {
			for (i = 0; i < 2; i++) {
				switch (type & mask) {
				case 0x01:
					dec_mwis->profile_id = 1;
					dec_mwis->msg_waiting[1].indication = TRUE;
				break;
				case 0x05:
					dec_mwis->profile_id = 2;
					dec_mwis->msg_waiting[1].indication = TRUE;
				break;
				default:
					dbg("CFF is NONE");
				}
				mask = mask << 1;
			}
		}
	}
	return TRUE;
}

gboolean tcore_sim_decode_mwis(unsigned char *enc_mwis, int enc_mwis_len, TelSimMwis *dec_mwis)
{
	gint i;
	guchar indicator_status = 0;
	guchar mask_bit = 0x01;

	tcore_check_return_value_assert(enc_mwis != NULL, FALSE);
	tcore_check_return_value_assert(enc_mwis_len != 0, FALSE);
	tcore_check_return_value_assert(dec_mwis != NULL, FALSE);

	memset((void*)dec_mwis, 0x0, sizeof(TelSimMwis));

	/*
	 * Indicator Status
	 *	| b8 | b7 | b6 | b5 | b4 | b3 | b2 | b1 |
	 *
	 * b1 -Voicemail
	 * b2 - Fax
	 * b3 - Electronic Mail
	 * b4 - Other
	 * b5 - Videomail
	 * b6-b8 - RFU
	 */
	indicator_status = enc_mwis[0];  //0x07
	if (indicator_status) {
		/* Count is valid */
		dec_mwis->count_valid = TRUE;

		/*
		 * enc_mwis_len includes 'Indicator Status' byte also,
		 * (-1) ensures that length doesn't consider 'Indicator Status'
		 * byte.
		 */
		for (i = 0; i < enc_mwis_len-1; i++) {
			/*
			 * '0th' index of Encoded MWIS is dedicated to
			 * 'Indicator Status'; 'i+1' while dereferencing 'enc_mwis'
			 * ensures that this byte is skipped.
			 */
			switch (indicator_status & (mask_bit << i)) {
			case TEL_SIM_MWIS_VOICE:
				dec_mwis->msg_waiting[i].indication = TRUE;
				dec_mwis->msg_waiting[i].count = enc_mwis[i+1];
			break;
			case TEL_SIM_MWIS_FAX:
				dec_mwis->msg_waiting[i].indication = TRUE;
				dec_mwis->msg_waiting[i].count = enc_mwis[i+1];
			break;
			case TEL_SIM_MWIS_EMAIL:
				dec_mwis->msg_waiting[i].indication = TRUE;
				dec_mwis->msg_waiting[i].count = enc_mwis[i+1];
			break;
			case TEL_SIM_MWIS_OTHER:
				dec_mwis->msg_waiting[i].indication = TRUE;
				dec_mwis->msg_waiting[i].count = enc_mwis[i+1];
			break;
			case TEL_SIM_MWIS_VIDEO:
				dec_mwis->msg_waiting[i].indication = TRUE;
				dec_mwis->msg_waiting[i].count = enc_mwis[i+1];
			break;
			default:
				dbg("MWIS is NONE");
			}
		}
	}

	return TRUE;
}

gboolean tcore_sim_encode_mwis(const TelSimMwis *mwis, guint field_count,
	gchar **encoded_mwis, guint *encoded_mwis_len)
{
	gchar *mwis_buf;
	guint i;

	tcore_check_return_value_assert(mwis != NULL, FALSE);
	tcore_check_return_value_assert(field_count >= 5, FALSE);

	/*
	 * Allocate memory -
	 *
	 *	field_count >=5 (1 - Indicator status, 2-5 Mandatory fields)
	 *		6 is possible if Videomail is supported (which is Optional)
	 *	+1 - NULL termination
	 */
	mwis_buf = tcore_malloc(field_count + 1);

	/*
	 * 3GPP TS 31.102 version 11.6.0 Release 11
	 *	4.2.63 EFMWIS (Message Waiting Indication Status)
	 */
	for (i = 1; i < field_count; i++) {
		/*
		 * '0th' index of Encoded MWIS contains 'Indicator Status',
		 * hence 'i' starts from '1'.
		 *
		 * msg_waiting is aligned from 0-field_count, hence (i-1) is used.
		 */
		mwis_buf[i] = mwis->msg_waiting[i-1].count;

		/*
		 * Indicator Status
		 *	| b8 | b7 | b6 | b5 | b4 | b3 | b2 | b1 |
		 *
		 * b1 -Voicemail
		 * b2 - Fax
		 * b3 - Electronic Mail
		 * b4 - Other
		 * b5 - Videomail
		 * b6-b8 - RFU
		 */
		if (mwis_buf[i])
			mwis_buf[0] |= (0x01 << (i-1));
	}

	*encoded_mwis = mwis_buf;
	*encoded_mwis_len = field_count;

	return TRUE;
}

gboolean tcore_sim_decode_vmwf(unsigned char* enc_vmwf, unsigned long enc_vmwf_len, TelSimMwis *dec_vmwf)
{
	int i, j = 0;
	unsigned char* temp = (unsigned char*) dec_vmwf;
	unsigned char mask = 0x0F;
	unsigned char voice_msg_flg = 0;

	tcore_check_return_value_assert(enc_vmwf != NULL, FALSE);
	tcore_check_return_value_assert(enc_vmwf_len != 0, FALSE);
	tcore_check_return_value_assert(dec_vmwf != NULL, FALSE);

	/* current telephony supports 2 byte cphs-vmwf data */
	for (i = 0; i < TEL_SIM_CPHS_VMWF_LEN_MAX; i++) {
		voice_msg_flg = enc_vmwf[i];
		for (j = 0; j < 2; j++) {
			if ((voice_msg_flg & mask) == 0x0A) {
				*temp = 1;  //TRUE
			} else if ((voice_msg_flg & mask) == 0x05) {
				*temp = 0;  // FALSE
			}
			temp += sizeof(gboolean);
			voice_msg_flg = voice_msg_flg >> 4;
		}
	}

	return TRUE;
}

gboolean tcore_sim_decode_ons(unsigned  char* enc_ons, int enc_ons_len, unsigned char *dec_ons)
{
	int length;
	memset((void*) dec_ons, 0, TEL_SIM_CPHS_OPERATOR_NAME_LEN_MAX+1);

	tcore_check_return_value_assert(enc_ons != NULL, FALSE);
	tcore_check_return_value_assert(enc_ons_len != 0, FALSE);

	if (tcore_util_is_empty(enc_ons, enc_ons_len) == TRUE) {
		err("empty record");
		return FALSE;
	}

	/* current telephony supports 25 byte cphs-operator name string.*/
	if (enc_ons_len >= TEL_SIM_CPHS_OPERATOR_NAME_LEN_MAX)
		enc_ons_len = TEL_SIM_CPHS_OPERATOR_NAME_LEN_MAX;

	length = tcore_util_get_string(dec_ons, enc_ons, enc_ons_len);
	dec_ons[length] = '\0';
	dbg("Operator Name is (%s) & Length (%d) ", dec_ons, length);

	return TRUE;
}

gboolean tcore_sim_decode_cfis(unsigned char *enc_cfis, int enc_cfis_len, TelSimCfis *dec_cfis)
{
	unsigned char type = 0;
	unsigned char mask = 0x01;
	int i = 0;

	tcore_check_return_value_assert(enc_cfis != NULL, FALSE);
	tcore_check_return_value_assert(enc_cfis_len != 0, FALSE);
	tcore_check_return_value_assert(dec_cfis != NULL, FALSE);

	if (tcore_util_is_empty(enc_cfis, enc_cfis_len) == TRUE) {
		err("empty record. all data is set 0xff");
		return TRUE;	// this is empty record
	}

	dec_cfis->profile_id = enc_cfis[i++];
	type = enc_cfis[i++];

	if (type) {
		for (i = 0; i < 5; i++) {
			switch (type & mask) {
			case 0x01:
				dec_cfis->indication[0] = TRUE;
			break;
			case 0x02:
				dec_cfis->indication[1] = TRUE;
			break;
			case 0x04:
				dec_cfis->indication[2] = TRUE;
			break;
			case 0x08:
				dec_cfis->indication[3] = TRUE;
			break;
			case 0x10:
				dec_cfis->indication[4] = TRUE;
			break;
			default:
				dbg("CFIS is NONE");
			}
			mask = mask << 1;
		}
	}
#if 0
	// get TON and NPI
	p_cfis->ton = (enc_cfis[++i] >> 4) & 0x07;
	p_cfis->npi = enc_cfis[i++] & 0x0F;

	// get actual dialing number length
	/* current telephony supports 20 byte dialing number format. */
	bcd_byte = (int) tcore_util_get_valid_bcd_byte(&enc_cfis[i], SIM_XDN_NUMBER_LEN_MAX / 2);

	// get dialing number/SSC string
	digit_len = tcore_util_convert_bcd_to_digit((char*) p_cfis->cfu_num, (char*) &enc_cfis[i], bcd_byte); // actual dialing number length in BCD.
	dbg( "Dialing number Length[%d]", digit_len);

	i = i + SIM_XDN_NUMBER_LEN_MAX / 2;

	// get Capability/Configuration id
	p_cfis->cc2_id = enc_cfis[i++];

	// get Extension1 id
	p_cfis->ext7_id = enc_cfis[i];
#endif
	dbg( "profile_id 0x%x", dec_cfis->profile_id);
/*
	dbg( "TypeOfNumber %d", p_cfis->ton);
	dbg( "NumberingPlanIdent %d", p_cfis->npi);
	dbg( "Dialing number[%s]", p_cfis->cfu_num);
*/
	return TRUE;
}

gboolean tcore_sim_decode_short_ons(unsigned char *enc_short_ons, int enc_short_ons_len, unsigned char *dec_short_ons)
{
	int length;

	tcore_check_return_value_assert(enc_short_ons != NULL, FALSE);
	tcore_check_return_value_assert(enc_short_ons_len != 0, FALSE);

	memset(dec_short_ons, 0x00, TEL_SIM_CPHS_OPERATOR_NAME_SHORT_FORM_LEN_MAX+1);

	if (tcore_util_is_empty(enc_short_ons, enc_short_ons_len) == TRUE){
		err("empty record");
		return FALSE;	// this is empty record
	}

	/*CPHS specification shows current EF include 10 bytes */
	tcore_check_return_value_assert(enc_short_ons_len != 0, FALSE);

	if (enc_short_ons_len > TEL_SIM_CPHS_OPERATOR_NAME_SHORT_FORM_LEN_MAX)
		enc_short_ons_len = TEL_SIM_CPHS_OPERATOR_NAME_SHORT_FORM_LEN_MAX;

	length = tcore_util_get_string(dec_short_ons, enc_short_ons, enc_short_ons_len);
	dec_short_ons[length] = '\0';
	dbg( "Operator short Name is (%s) &  length (%d)", dec_short_ons, length);
	return TRUE;
}

gboolean tcore_sim_decode_opl(unsigned char *enc_opl, int enc_opl_len, TelSimOpl *dec_opl)
{
	unsigned char packet_in_digit[3 * 2 + 1];

	tcore_check_return_value_assert(enc_opl != NULL, FALSE);
	tcore_check_return_value_assert(enc_opl_len != 0, FALSE);
	tcore_check_return_value_assert(dec_opl != NULL, FALSE);

	if (tcore_util_is_empty(enc_opl, enc_opl_len) == TRUE) {
		err("empty record");
		return FALSE;	// this is empty record
	}
	tcore_util_convert_bcd_to_digit((char*) packet_in_digit, (char*) &enc_opl[0], 3);
	dbg( "AFTER tcore_util_convert_bcd_to_digit 4th[0x%x]", packet_in_digit[3]);

	// get MCC
	memcpy(&dec_opl->plmn, &(packet_in_digit[0]), 3);
	// get MNC
	if (packet_in_digit[3] == 0x00){
		memcpy(&(dec_opl->plmn[3]), &(packet_in_digit[3 + 1]), 2);
		dec_opl->plmn[5] = '\0';
	} else{
		memcpy(&(dec_opl->plmn[3]), &(packet_in_digit[3]), 3);
		dec_opl->plmn[6] = '\0';
	}

	dbg( " PLMN Code[%s]", dec_opl->plmn);
	dec_opl->lac_from = (*(enc_opl+3) << 8) | (*(enc_opl+4) & 0x00ff);
	dbg( " Start value of the LAC range[%x]",	dec_opl->lac_from);
	dec_opl->lac_to = (*(enc_opl+5) << 8) | (*(enc_opl+6) & 0x00ff);
	dbg( " End value of the LAC range[%x]", dec_opl->lac_to);
	dec_opl->rec_identifier = enc_opl[7];
	dbg( " PNN Record identifier[%x]", dec_opl->rec_identifier);
	return TRUE;
}

gboolean tcore_sim_decode_pnn(unsigned char* enc_pnn, int enc_pnn_len, TelSimPnn *dec_pnn)
{
	int full_name_len = 0, short_name_len = 0;
	int cvt_leng = 0, short_name_base = 0;
	unsigned char *full_name_str, *short_name_str;

	tcore_check_return_value_assert(enc_pnn != NULL, FALSE);
	tcore_check_return_value_assert(enc_pnn_len != 0, FALSE);
	tcore_check_return_value_assert(dec_pnn != NULL, FALSE);

	if (tcore_util_is_empty(enc_pnn, enc_pnn_len) == TRUE) {
		err("empty record");
		return FALSE;	// this is empty record
	}

	/*Full name for network IEI(Information Element Identifier),0x43*/
	if (enc_pnn[0] == 0x43) {
		dbg( " Full name of network IEI exist");
		//f_name_part includes information byte.
		full_name_len =  (int)enc_pnn[1] - 1;

		/* 3rd byte information element(according to TS 24.008 for Network Name)
		 8 :ext1
		 7 6 5 : coding scheme
		 4 : Add CI
		 3 2 1 : number of spare bits in last octet

		 Coding Scheme (octet 3, bits 5-7)
		 0	0	0		Cell Broadcast data coding scheme, GSM default alphabet, language unspecified, defined in 3GPP TS 23.038 [8b]
		 0	0	1		UCS2 (16 bit) [72]
		 0	1	0		to	reserved
		 1	1	1		to	reserved
		 */
		if ((enc_pnn[2] & 0x70) >> 4 == 0) {
			dbg( "DCS:GSM7");
			// In case of GSM7, 35byte packing data will be converted 40 bytes unpacking string.
			if (full_name_len > (TEL_SIM_NW_FULL_NAME_LEN_MAX * 7) / 8)
				full_name_len = (TEL_SIM_NW_FULL_NAME_LEN_MAX * 7) / 8;

			full_name_str = (unsigned char *) tcore_util_unpack_gsm7bit(enc_pnn + 3, full_name_len);
			memcpy(&dec_pnn->full_name, full_name_str, TEL_SIM_NW_FULL_NAME_LEN_MAX+1);
		} else if ((enc_pnn[2] & 0x70) >> 4 == 1) {
			dbg( "DCS:UCS2");
			/* current telephony supports 40 bytes network name string */
			if (full_name_len > TEL_SIM_NW_FULL_NAME_LEN_MAX)
				full_name_len = TEL_SIM_NW_FULL_NAME_LEN_MAX;

			tcore_util_convert_str_to_utf8((guchar *) (dec_pnn->full_name), (gushort *) &cvt_leng, TEL_UTIL_ALPHABET_FORMAT_UCS2, enc_pnn + 3, full_name_len);
		} else {
			dbg( "DCS:unknown");
			return FALSE;
		}
		dbg( " Full name of network contents[%s]", dec_pnn->full_name);

		short_name_base = (int)enc_pnn[1] +2;
		dbg( " short name base byte [0x%02x]", short_name_base);

		/*Short Name for network IEI(Information Element Identifier), 0x45*/
		if (enc_pnn[short_name_base] == 0x45) {
			dbg( " Short name of network IEI exist");
			//s_name_part includes information byte.
			short_name_len = enc_pnn[short_name_base +1] -1;

			if ((enc_pnn[short_name_base + 2] & 0x70) >> 4 == 0) {
				dbg( "DCS:GSM7");
				// In case of GSM7, 35byte packing data will be converted 40 bytes unpacking string.
				if (short_name_len > (TEL_SIM_NW_FULL_NAME_LEN_MAX * 7) / 8)
					short_name_len = (TEL_SIM_NW_FULL_NAME_LEN_MAX * 7) / 8;

				short_name_str = (unsigned char *) tcore_util_unpack_gsm7bit(enc_pnn + short_name_base + 3, short_name_len);
				memcpy(&dec_pnn->short_name, short_name_str, TEL_SIM_NW_FULL_NAME_LEN_MAX+1);
			} else if ((enc_pnn[short_name_base +2] & 0x70) >> 4 == 1) {
				dbg( "DCS:UCS2");
				if (short_name_len > TEL_SIM_NW_FULL_NAME_LEN_MAX)
					short_name_len = TEL_SIM_NW_FULL_NAME_LEN_MAX;

				tcore_util_convert_str_to_utf8((guchar *) (dec_pnn->short_name), (gushort*) &cvt_leng, TEL_UTIL_ALPHABET_FORMAT_UCS2, enc_pnn + short_name_base+ 3, short_name_len);
			} else {
				dbg( "DCS:unknown");
				return FALSE;
			}
			dbg( " Short name of network contents[%s]", dec_pnn->short_name);
		}
		return TRUE;
	}
	return FALSE;
}

gboolean tcore_sim_get_status(CoreObject *co, TelSimCardStatus *status)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*status = po->sim_status;

	return TRUE;
}

gboolean tcore_sim_set_status(CoreObject *co, TelSimCardStatus status)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->sim_status = status;

	return TRUE;
}

gboolean tcore_sim_get_identification(CoreObject *co, TelSimCardChangeStatus *change_status)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*change_status = po->change_status;

	return TRUE;
}

gboolean tcore_sim_set_identification(CoreObject *co, TelSimCardChangeStatus change_status)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->change_status = change_status;

	return TRUE;
}

gboolean tcore_sim_get_type(CoreObject *co, TelSimCardType *type)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*type = po->type;

	return TRUE;
}

gboolean tcore_sim_set_type(CoreObject *co, TelSimCardType type)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->type = type;

	return TRUE;
}

gboolean tcore_sim_get_imsi(CoreObject *co, TelSimImsiInfo **imsi)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*imsi = (TelSimImsiInfo *)tcore_memdup((gconstpointer)&po->imsi, sizeof(TelSimImsiInfo));

	return TRUE;
}

gboolean tcore_sim_set_imsi(CoreObject *co, TelSimImsiInfo *imsi)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	memcpy(&po->imsi, imsi, sizeof(TelSimImsiInfo));

	return TRUE;
}

gboolean tcore_sim_get_service_table(CoreObject *co, TelSimServiceTable **svct)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*svct = (TelSimServiceTable *)tcore_memdup((gconstpointer)&po->svct, sizeof(TelSimServiceTable));

	return TRUE;
}

gboolean tcore_sim_set_service_table(CoreObject *co, TelSimServiceTable *svct)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	memcpy(&po->svct, svct, sizeof(TelSimServiceTable));

	return TRUE;
}

gboolean tcore_sim_get_cphs_status(CoreObject *co, gboolean *is_supported)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	*is_supported = po->is_cphs;

	return TRUE;
}

gboolean tcore_sim_set_cphs_status(CoreObject *co, gboolean is_supported)
{
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->is_cphs = is_supported;

	return TRUE;
}

gboolean tcore_sim_get_spn(CoreObject *co, gchar **spn)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(spn != NULL, FALSE);

	*spn = tcore_strdup(po->spn);

	return TRUE;
}

gboolean tcore_sim_set_spn(CoreObject *co, gchar *spn)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	tcore_free(po->spn);
	po->spn = tcore_strdup(spn);

	return TRUE;
}

gboolean tcore_sim_get_disp_condition(CoreObject *co,
		TelSimSpnDispCondition *disp_condition)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(disp_condition != NULL, FALSE);

	*disp_condition = po->disp_condition;

	return TRUE;
}

gboolean tcore_sim_set_disp_condition(CoreObject *co,
		TelSimSpnDispCondition disp_condition)
{
	PrivateObject *po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->disp_condition = disp_condition;

	return TRUE;
}

gboolean tcore_sim_link_userdata(CoreObject *co, void *userdata)
{
	PrivateObject *po = NULL;
	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	po->userdata = userdata;

	return TRUE;
}

void* tcore_sim_ref_userdata(CoreObject *co)
{
	PrivateObject *po = NULL;
	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, NULL);
	tcore_check_return_value_assert(po->userdata != NULL, NULL);

	return po->userdata;
}

static void tcore_sim_initialize_context(CoreObject *co)
{
	TcoreSimOps *tmp_ops = NULL;
	PrivateObject *po = NULL;

	po = tcore_object_ref_object(co);
	tcore_check_return_assert(po != NULL);
	tmp_ops = po->ops;
	memset(po, 0x00, sizeof(PrivateObject));
	po->ops = tmp_ops;
	po->sim_status = TEL_SIM_STATUS_UNKNOWN;
}

void tcore_sim_override_ops(CoreObject *co, TcoreSimOps *sim_ops)
{
	PrivateObject *po = NULL;
	po = (PrivateObject *)tcore_object_ref_object(co);

	tcore_check_return_assert(po != NULL);
	tcore_check_return_assert(po->ops != NULL);
	tcore_check_return_assert(sim_ops != NULL);

	if (sim_ops->get_ecc)
		po->ops->get_ecc = sim_ops->get_ecc;

	if (sim_ops->get_iccid)
		po->ops->get_iccid = sim_ops->get_iccid;

	if (sim_ops->get_language)
		po->ops->get_language = sim_ops->get_language;

	if (sim_ops->set_language)
		po->ops->set_language = sim_ops->set_language;

	if (sim_ops->get_callforwarding_info)
		po->ops->get_callforwarding_info = sim_ops->get_callforwarding_info;

	if (sim_ops->get_messagewaiting_info)
		po->ops->get_messagewaiting_info = sim_ops->get_messagewaiting_info;

	if (sim_ops->set_messagewaiting_info)
		po->ops->set_messagewaiting_info = sim_ops->set_messagewaiting_info;

	if (sim_ops->get_mailbox_info)
		po->ops->get_mailbox_info = sim_ops->get_mailbox_info;

	if (sim_ops->set_mailbox_info)
		po->ops->set_mailbox_info = sim_ops->set_mailbox_info;

	if (sim_ops->get_msisdn)
		po->ops->get_msisdn = sim_ops->get_msisdn;

	if (sim_ops->get_spn)
		po->ops->get_spn = sim_ops->get_spn;

	if (sim_ops->get_cphs_netname)
		po->ops->get_cphs_netname = sim_ops->get_cphs_netname;

	if (sim_ops->get_sp_display_info)
		po->ops->get_sp_display_info = sim_ops->get_sp_display_info;

	if (sim_ops->req_authentication)
		po->ops->req_authentication = sim_ops->req_authentication;

	if (sim_ops->verify_pins)
		po->ops->verify_pins = sim_ops->verify_pins;

	if (sim_ops->verify_puks)
		po->ops->verify_puks = sim_ops->verify_puks;

	if (sim_ops->change_pins)
		po->ops->change_pins = sim_ops->change_pins;

	if (sim_ops->disable_facility)
		po->ops->disable_facility = sim_ops->disable_facility;

	if (sim_ops->enable_facility)
		po->ops->enable_facility = sim_ops->enable_facility;

	if (sim_ops->get_facility)
		po->ops->get_facility = sim_ops->get_facility;

	if (sim_ops->get_lock_info)
		po->ops->get_lock_info = sim_ops->get_lock_info;

	if (sim_ops->req_apdu)
		po->ops->req_apdu = sim_ops->req_apdu;

	if (sim_ops->req_atr)
		po->ops->req_atr = sim_ops->req_atr;
}

gboolean tcore_sim_set_ops(CoreObject *co, TcoreSimOps *ops)
{
	PrivateObject *po;
	tcore_check_return_value(co != NULL, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	if (po->ops != NULL) {
		tcore_free(po->ops);
		po->ops = NULL;
	}

	if (ops != NULL)
		po->ops = tcore_memdup((gconstpointer)ops, sizeof(TcoreSimOps));

	return TRUE;
}

CoreObject *tcore_sim_new(TcorePlugin *p,
			TcoreSimOps *ops, TcoreHal *hal)
{
	CoreObject *co = NULL;
	PrivateObject *po = NULL;

	tcore_check_return_value_assert(p != NULL, NULL);

	co = tcore_object_new(p, hal);
	tcore_check_return_value_assert(co != NULL, NULL);

	po = tcore_malloc0(sizeof(PrivateObject));
	po->ops = tcore_memdup(ops, sizeof(TcoreSimOps));

	tcore_object_set_type(co, CORE_OBJECT_TYPE_SIM);
	tcore_object_link_object(co, po);
	tcore_object_set_free_hook(co, _po_free_hook);
	tcore_object_set_clone_hook(co, _po_clone_hook);
	tcore_object_set_dispatcher(co, _dispatcher);

	tcore_sim_initialize_context(co);

	return co;
}

void tcore_sim_free(CoreObject *co)
{
	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_SIM);
	tcore_object_free(co);
}
