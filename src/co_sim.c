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
#include <ctype.h>
#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "queue.h"
#include "user_request.h"
#include "core_object.h"
#include "co_sim.h"

struct private_object_data {
	struct tcore_sim_operations *ops;

	enum tel_sim_type type; /**< Provides the SIM card type*/
	enum tel_sim_status sim_status; /**< Provides the card status*/
	struct tel_sim_imsi imsi; /**< Provides IMSI information*/
	gboolean b_sim_changed; /**< Provides SIM card Identification- 0:no changed, 1:changed*/
	gboolean b_cphs; /**< Whether current SIM is for CPHS or not*/
	struct tel_sim_service_table svct; /**< (U)SIM Service Table information*/
	void *userdata; /**< free use data*/
};


static TReturn _dispatcher(CoreObject *o, UserRequest *ur)
{
	enum tcore_request_command command;
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_SIM, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po || !po->ops)
		return TCORE_RETURN_ENOSYS;

	command = tcore_user_request_get_command(ur);

	switch (command) {
		case TREQ_SIM_VERIFY_PINS:
			if (!po->ops->verify_pins)
				return TCORE_RETURN_ENOSYS;

			return po->ops->verify_pins(o, ur);
			break;

		case TREQ_SIM_VERIFY_PUKS:
			if (!po->ops->verify_puks)
				return TCORE_RETURN_ENOSYS;

			return po->ops->verify_puks(o, ur);
			break;

		case TREQ_SIM_CHANGE_PINS:
			if (!po->ops->change_pins)
				return TCORE_RETURN_ENOSYS;

			return po->ops->change_pins(o, ur);
			break;

		case TREQ_SIM_GET_FACILITY_STATUS:
			if (!po->ops->get_facility_status)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_facility_status(o, ur);
			break;

		case TREQ_SIM_DISABLE_FACILITY:
			if (!po->ops->disable_facility)
				return TCORE_RETURN_ENOSYS;

			return po->ops->disable_facility(o, ur);
			break;

		case TREQ_SIM_ENABLE_FACILITY:
			if (!po->ops->enable_facility)
				return TCORE_RETURN_ENOSYS;

			return po->ops->enable_facility(o, ur);
			break;

		case TREQ_SIM_GET_LOCK_INFO:
			if (!po->ops->get_lock_info)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_lock_info(o, ur);
			break;

		case TREQ_SIM_TRANSMIT_APDU:
			if (!po->ops->transmit_apdu)
				return TCORE_RETURN_ENOSYS;

			return po->ops->transmit_apdu(o, ur);
			break;

		case TREQ_SIM_GET_ATR:
			if (!po->ops->get_atr)
				return TCORE_RETURN_ENOSYS;

			return po->ops->get_atr(o, ur);
			break;

		case TREQ_SIM_SET_LANGUAGE:
		case TREQ_SIM_SET_CALLFORWARDING:
		case TREQ_SIM_SET_MESSAGEWAITING:
		case TREQ_SIM_SET_MAILBOX:
			if (!po->ops->update_file)
				return TCORE_RETURN_ENOSYS;

			return po->ops->update_file(o, ur);
			break;

		case TREQ_SIM_GET_ECC:
		case TREQ_SIM_GET_LANGUAGE:
		case TREQ_SIM_GET_ICCID:
		case TREQ_SIM_GET_MAILBOX:
		case TREQ_SIM_GET_CALLFORWARDING:
		case TREQ_SIM_GET_MESSAGEWAITING:
		case TREQ_SIM_GET_CPHS_INFO:
		case TREQ_SIM_GET_MSISDN:
		case TREQ_SIM_GET_SPN:
		case TREQ_SIM_GET_SPDI:
		case TREQ_SIM_GET_OPL:
		case TREQ_SIM_GET_PNN:
		case TREQ_SIM_GET_CPHS_NETNAME:
		case TREQ_SIM_GET_OPLMNWACT:
			if (!po->ops->read_file)
				return TCORE_RETURN_ENOSYS;

			return po->ops->read_file(o, ur);
			break;

		case TREQ_SIM_REQ_AUTHENTICATION:
			if (!po->ops->req_authentication)
				return TCORE_RETURN_ENOSYS;

			return po->ops->req_authentication(o, ur);
			break;

		default:
			break;
	}

	return TCORE_RETURN_SUCCESS;
}

static void _clone_hook(CoreObject *src, CoreObject *dest)
{
	struct private_object_data *src_po = NULL;
	struct private_object_data *dest_po = NULL;

	if (!src || !dest)
		return;

	dest_po = calloc(sizeof(struct private_object_data), 1);
	if (!dest_po) {
		tcore_object_link_object(dest, NULL);
		return;
	}

	src_po = tcore_object_ref_object(src);
	dest_po->ops = src_po->ops;

	tcore_object_link_object(dest, dest_po);
}

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SIM);

	po = tcore_object_ref_object(o);
	if (po) {
		free(po);
		tcore_object_link_object(o, NULL);
	}
}

static void _reverse(char* p_in, int length)
{
	int i, j = length - 1;
	for (i = 0; i < j; i++) {
		int t = p_in[i];
		p_in[i] = p_in[j];
		p_in[j--] = t;
	}
}

static char* _acitoa(int n, char* str, int b)
{
	int i = 0;

	do {
		str[i++] = "0123456789ABCDEF"[n % b];
	} while ((n /= b) > 0);

	_reverse(str, i);

	str[i] = '\0';

	return str;
}

/*******************************************************************************
 Convert Digit to BCD (BCD to Digit)

 bcd		<--->	digit
 0xa				0x2a '*'
 0xb				0x23 '#'
 0xc				0x70 'P'
 0xd				'?'
 0xf				0
 1032547698		0123456789

 ********************************************************************************/
/**
 * This function is used to Convert Digit to BCD (Digit to BCD)
 *
 * @return		None
 * @param[out]	bcdCode - BCD output
 * @param[in]		digits - Digit input
 * @param[in]		digitLen - digit length
 * @Interface		Synchronous.
 * @remark
 * @Refer
 */
static void _digit_to_bcd(char* bcdCode, char* digits, int digitLen)
{
	int i, j, digit;
	unsigned char higher, lower;

	// 0123456789 -> 1032547698
	for (i = 0, j = 0; i < digitLen; i = i + 2, j++) {
		if (digits[i] == '*')
			digit = 0x0A;
		else if (digits[i] == '#')
			digit = 0x0B;
		else if (toupper((int) digits[i]) == 'P')
			digit = 0x0C;
		else if ((digits[i]) == '?')
			digit = 0x0D;
		else
			digit = (int) digits[i];

		lower = digit & 0x0F;

		if (digitLen != i + 1) {
			if (digits[i + 1] == '*')
				digit = 0x0A;
			else if (digits[i + 1] == '#')
				digit = 0x0B;
			else if (toupper((int) digits[i + 1]) == 'P')
				digit = 0x0C;
			else if (digits[i + 1] == '?')
				digit = 0x0D;
			else
				digit = (int) digits[i + 1];
			higher = digit & 0x0F;
		} else {
			higher = 0xFF;
		}
		bcdCode[j] = (higher << 4) | lower;
	}
}

/**
 * This function is used to Convert BCD to Digit (BCD to Digit)
 *
 * @return		None
 * @param[out]	digit - Digit output output
 * @param[in]		bcdCode - BCD Input
 * @param[in]		bcdLen - BCD length
 * @Interface		Synchronous.
 * @remark
 * @Refer
 */
static unsigned long _bcd_to_digit(char* digit, char* bcdCode, int bcdLen)
{
	int i, h, l;
	char c[2];
	unsigned char higher, lower;
	unsigned long digitLen = 0;

	// 0123456789 <- 1032547698
	memset((void*) digit, 0, bcdLen * 2);

	for (i = 0; i < bcdLen; i++) {
		higher = (bcdCode[i] >> 4) & 0x0F; // get high nibble

		if (higher == 0x0A)
			higher = '*'; // =0x2A
		else if (higher == 0x0B)
			higher = '#'; // =0x23
		else if (higher == 0x0C)
			higher = 'P'; // =0x70, DTMF Control digit
		else if (higher == 0x0D)
			higher = '?';
		else if (higher == 0x0F)
			higher = 0;
		else {
			h = (int) higher;
			_acitoa(h, c, 16);
			higher = (char) toupper(*c);
		}

		lower = bcdCode[i] & 0x0F; // get low nibble

		if (lower == 0x0A)
			lower = '*';
		else if (lower == 0x0B)
			lower = '#';
		else if (lower == 0x0C)
			lower = 'P'; //DTMF Control digit
		else if (lower == 0x0D)
			lower = '?';
		else if (lower == 0x0F)
			lower = 0;
		else {
			l = (int) lower;
			_acitoa(l, c, 16);
			lower = (char) toupper(*c);
		}
		digit[i * 2] = lower;
		digit[i * 2 + 1] = higher;
	}

	digitLen = (unsigned long) strlen(digit);
	return digitLen;
}

/**
 * This function is used to get(decode) string name
 *
 * @return		length of string
 * @param[out]	palpha_id - Alpha string
 * @param[in]		pRecord - Input raw data
 * @param[in]		alphaIDMaxLen - Max size of alpha id array
 * @Interface		Synchronous.
 * @remark
 * @Refer
 */
static unsigned long _get_string(unsigned char* palpha_id, unsigned char* pRecord,	unsigned long alphaIDMaxLen)
{
	unsigned long i, alphaIDLen = 0;
	unsigned char *pAlphaID = (unsigned char*) palpha_id;

	memset((void*) pAlphaID, 0, alphaIDMaxLen);

	if (pRecord[0] == 0xFF)
		return alphaIDLen;

	for (i = 0; i < alphaIDMaxLen; i++) {
		if (pRecord[0] <= 0x7F && pRecord[i] == 0xFF)
			break;

		pAlphaID[i] = pRecord[i];
		alphaIDLen++;
	}
	return alphaIDLen;
}

/**
 * This function is used to set(encode) string name
 *
 * @return		length of string
 * @param[in]		palpha_id - Alpha string input
 * @param[out]	pRecord - output raw data
 * @param[in]		alphaIDMaxLen - Max size of alpha id array
 * @Interface		Synchronous.
 * @remark
 * @Refer
 */
static void _set_string(unsigned char* pRecord, unsigned char* palpha_id, unsigned long alphaIDMaxLen)
{
	unsigned long i;
	unsigned char* pAlphaID = (unsigned char*) palpha_id;

	memset((void*) pRecord, 0xFF, alphaIDMaxLen);

	for (i = 0; i < alphaIDMaxLen; i++)
		pRecord[i] = pAlphaID[i];
}

static gboolean _is_empty(unsigned char* p_in, int in_length)
{
	int i;
	for (i = 0; i < in_length; i++) {
		if (p_in[i] != 0xFF)
			return FALSE;
	}
	dbg("current index has empty data");
	return TRUE;		// this is empty record.
}

/**
 * This function is used to get BCD length
 *
 * @return		length of string
 * @param[in]		pBcdData - BCD Input data
 * @param[in]		bcdMaxLen - BCD Max data Length
 * @Interface		Synchronous.
 * @remark
 * @Refer
 */
static int _get_valid_bcd_byte(unsigned char* pBcdData, int bcdMaxLen)
{
	int i, bcd_length = 0;

	for (i = 0; i < bcdMaxLen; i++) {
		if (pBcdData[i] == 0xFF)
			break;

		bcd_length++;
	}
	return bcd_length;
}

/**
 * This function is used to get unpacked 8bit Format from GSM 7bit packed string.
 *
 * @return          the length of unpacked characters	.
 * @param[out]	out_string	Specifies the unpacked output string
 * @param[in]		in_string	Contains the input string to be unpacked
 * @param[in]		in_string_len	Contains the input string length
 * @remark
 */
static int _unpack_7bit28bit(unsigned char* pInString, unsigned int inStringLen, unsigned char * pOutString)
{
	int i = 0;
	unsigned int pos = 0;
	unsigned short shift = 0;
	int outlen = 0;
	outlen = (short int) ((inStringLen * 8) / 7);

	for (i = 0; pos < inStringLen; i++, pos++) {
		pOutString[i] = (pInString[pos] << shift) & 0x7F;

		if (pos != 0) {
			/* except the first byte, a character contains some bits from the previous byte.*/
			pOutString[i] |= pInString[pos - 1] >> (8 - shift);
		}

		shift++;

		if (shift == 7) {
			shift = 0;

			/* a possible extra complete character is available */
			i++;
			pOutString[i] = pInString[pos] >> 1;
		}
	}

	/*If a character is '\r'(13), change it to space(32) */
	for (i = 0; i < outlen; i++)
		if (pOutString[i] == 13) pOutString[i] = 32;

	pOutString[outlen] = '\0';

	dbg( "unpack is done with outlen[%d] and array index[%d], out string[%s]", outlen, i, pOutString);
	return (unsigned int) (i);
}

static int _ucs2_to_utf8(int in_length, unsigned char *in_data, int *out_length,	unsigned char *out_data)
{
	int i, j;
	i = j = 0;
	if (in_length == 0 || in_data == NULL || out_data == NULL) {
		dbg( "Unicode Decode Failed as text length is 0");
		return FALSE;
	}

	if (0 != (in_length % 2)) {
		dbg( " ##### Unicode decoding failed due to invalid text length [%d]",	in_length);
		return FALSE;
	}

	for (i = 0; i < in_length; i++) {
		out_data[i] = in_data[j];
		j = j + 2;
	}
	*out_length = (in_length / 2);
	out_data[i] = '\0';
	return TRUE;
}

/**
 * According to ETSI 102.221 ( 3GPP specification refers it ), EF-ICCID is coded by BCD, left justified and padded with 'F'.
 * This EF is mandatory and byte length is fixed with 10 bytes. So actual maximum length of ICCID is 20 digits.
 */
gboolean tcore_sim_decode_iccid(struct tel_sim_iccid *p_out, unsigned char *p_in, int in_length)
{
	int bcd_byte = 0;
	int char_length = 0;

	if (p_in == NULL || p_out == NULL)
		return FALSE;

	if (in_length == 0 || in_length > 10)
		return FALSE;

	memset((void*) p_out->iccid, 0, SIM_ICCID_LEN_MAX+1);

	bcd_byte = _get_valid_bcd_byte(p_in, in_length);
	dbg( "ICCID valid bcd byte is[%d]", bcd_byte);

	char_length = _bcd_to_digit(p_out->iccid, (char*) p_in, bcd_byte);
	dbg( "ICCID string length is[%d]", char_length);

	*(p_out->iccid+char_length) = '\0';

	return TRUE;
}

/**
 * This function is used to decode EFLP (2G)
 */
gboolean tcore_sim_decode_lp(struct tel_sim_language *p_out, unsigned char *p_in, int in_length)
{
	int i = 0;

	memset((void*) p_out, 0xFF, sizeof(struct tel_sim_language));
	p_out->language_count = 0;

	if (in_length == 0)
		return FALSE;
	/*
	 * Description of problem: language decoding was not correctly done if we used 7layers's test SIM
	 * Patch Description : The tested SIM  at 7layers has 3 language codes like [ff][ff][01].
	 In this case we could not decode 3rd language code.
	 So, the below 2 lines checking the UNSPECIFIED language are commented.
	 */

	if (in_length > SIM_LANG_CNT_MAX)
		in_length = SIM_LANG_CNT_MAX;

	for (i = 0; i < in_length; i++) {
		/*
		 * Description of problem: Language decoding was not correctly done if we used some test SIM
		 * Patch Description : Test SIM at some place has 3 language codes like  [ff][ff][01].
		 * In this case we could not decode 3rd language code.
		 * So, the below 2 lines checking the UNSPECIFIED language are commented.
		 */
		if (p_in[i] == 0xFF)
			continue;

		p_out->language[p_out->language_count] = (enum tel_sim_language_type) p_in[i];
		dbg( "p_out->language[%d]=[%d] ", i, p_out->language[p_out->language_count]);
		p_out->language_count++;
	}
	dbg( "in_length %d, lang_cnt %d ", in_length, p_out->language_count);
	return TRUE;
}

/**
 * This function is used to encode EFLP (2G)
 */
char* tcore_sim_encode_lp( int *out_length, struct tel_sim_language *p_in)
{
    int i = 0;
    char *tmp_out = NULL;

    if ( out_length == NULL || p_in == NULL ){
        dbg("out_length or p_in is null");
        return NULL;
    }

    tmp_out = (char*)malloc(p_in->language_count);
    memset((void*) tmp_out, 0xff, p_in->language_count);

    for (i = 0; i < p_in->language_count; i++)
        tmp_out[i] = p_in->language[i];

    *out_length = i;
    return tmp_out;
}

/**
 * This function is used to decode LI (3G)
 */
gboolean tcore_sim_decode_li(enum tel_sim_file_id file_id, struct tel_sim_language *p_out, unsigned char *p_in, int in_length)
{
	int i;
	unsigned short defaultLi;
	unsigned char tempLi[3] = { 0, 0, 0 };

	memset((void*) p_out, 0xFF, sizeof(struct tel_sim_language));
	p_out->language_count  = 0;

	if (in_length == 0)
		return FALSE;

	/*
	 * Description of problem: language decoding was not correctly done if we used 7layers's test SIM
	 * Patch Description : TS31.102 If the EFLI has the value 'FFFF' in its highest priority position,
	 then the preferred language selection shall be the language preference in the EFPL
	 */
	if (/*TODO g_sim.CardType == SIM_TYPE_USIM && */(file_id == SIM_EF_USIM_LI || file_id == SIM_EF_LP)) {
		defaultLi = p_in[0];
		defaultLi = ((defaultLi << 8) & 0xFF00) + p_in[1];

		if (defaultLi == 0xFFFF)	// 1st language is default.
			return FALSE;
	}

	if (in_length > SIM_LANG_CNT_MAX)
		in_length = SIM_LANG_CNT_MAX;

	for (i = 0; i < in_length; i++) {
		tempLi[0] = p_in[i++];
		tempLi[1] = p_in[i];
/*
		 Description of problem: language decoding was not correctly done if we used 7layers's test SIM
		 Patch Description : The tested SIM at specific test lab has 3 language codes like [ff][ff][ff][ff][64][65].
		 In this case we could not decode 3rd language code.
		 So, the below 2 lines checking the UNSPECIFIED language are commented.
*/
		if (tempLi[0] == 0xFF || tempLi[1] == 0xFF)  //this is always 2 bytes
			continue;

		p_out->language[p_out->language_count] = SIM_LANG_UNSPECIFIED;

		if (tempLi[0] == 'e') {
			switch (tempLi[1]) {
				case 'n':
					p_out->language[p_out->language_count] = SIM_LANG_ENGLISH;
					break;
				case 's':
					p_out->language[p_out->language_count] = SIM_LANG_SPANISH;
					break;
				case 'l':
					p_out->language[p_out->language_count] = SIM_LANG_GREEK;
					break;
			}
		} else if (tempLi[0] == 'd') {
			switch (tempLi[1]) {
				case 'e':
					p_out->language[p_out->language_count] = SIM_LANG_GERMAN;
					break;

				case 'a':
					p_out->language[p_out->language_count] = SIM_LANG_DANISH;
					break;
			}
		} else if (tempLi[0] == 'i' && tempLi[1] == 't') {
			p_out->language[p_out->language_count] = SIM_LANG_ITALIAN;
		} else if (tempLi[0] == 'f' && tempLi[1] == 'r') {
			p_out->language[p_out->language_count] = SIM_LANG_FRENCH;
		} else if (tempLi[0] == 'n' && tempLi[1] == 'l') {
			p_out->language[p_out->language_count] = SIM_LANG_DUTCH;
		} else if (tempLi[0] == 's' && tempLi[1] == 'v') {
			p_out->language[p_out->language_count] = SIM_LANG_SWEDISH;
		} else if (tempLi[0] == 'f' && tempLi[1] == 'i') {
			p_out->language[p_out->language_count] = SIM_LANG_FINNISH;
		} else if (tempLi[0] == 'n' && tempLi[1] == 'o') {
			p_out->language[p_out->language_count] = SIM_LANG_NORWEGIAN;
		} else if (tempLi[0] == 't' && tempLi[1] == 'r') {
			p_out->language[p_out->language_count] = SIM_LANG_TURKISH;
		} else if (tempLi[0] == 'h' && tempLi[1] == 'u') {
			p_out->language[p_out->language_count] = SIM_LANG_HUNGARIAN;
		} else if (tempLi[0] == 'p') {
			switch (tempLi[1]) {
				case 'l':
					p_out->language[p_out->language_count] = SIM_LANG_POLISH;
					break;
				case 't':
					p_out->language[p_out->language_count] = SIM_LANG_PORTUGUESE;
					break;
			}
		} else if (tempLi[0] == 'k' && tempLi[1] == 'o') {
			p_out->language[p_out->language_count] = SIM_LANG_KOREAN;
		} else if (tempLi[0] == 'z' && tempLi[1] == 'h') {
			p_out->language[p_out->language_count] = SIM_LANG_CHINESE;
		} else if (tempLi[0] == 'r' && tempLi[1] == 'u') {
			p_out->language[p_out->language_count] = SIM_LANG_RUSSIAN;
		} else if (tempLi[0] == 'j' && tempLi[1] == 'a') {
			p_out->language[p_out->language_count] = SIM_LANG_JAPANESE;
		}

		dbg( "count %d & Codes %d ", p_out->language_count, p_out->language[p_out->language_count]);
		p_out->language_count++;
	}

	if (p_out->language_count == 0) {
		dbg( "p_out->language_count = %d ", p_out->language_count);
		return FALSE;
	}
	else {
		return TRUE;
	}
}

/**
 * This function is used to encode EFLI (3G)
 */
char* tcore_sim_encode_li( int *out_length, struct tel_sim_language *p_in)
{
	int i = 0;
    char *tmp_out = NULL;
	char *LanguageCode[] = { "de", "en", "it", "fr", "es", "nl", "sv", "da", "pt", "fi", "no", "el",
													"tr", "hu", "pl", "ko", "zh", "ru", "ja" };

    if ( out_length == NULL || p_in == NULL ){
        dbg("out_length or p_in is null");
        return NULL;
    }

    tmp_out = (char*)malloc((p_in->language_count) *2);
    memset((void*) tmp_out, 0xff, (p_in->language_count)*2);

	for (i = 0; i < p_in->language_count; i++) {
		if (p_in->language[i] < SIM_LANG_UNSPECIFIED) {
			strncpy((char *) &tmp_out[i * 2], LanguageCode[p_in->language[i]], 2);
			dbg( "sim_encode_li: p_out[%s]:[%x][%x]", tmp_out, tmp_out[i*2], tmp_out[(i*2)+1]);
		}
	}
	*out_length = i*2;
	return tmp_out;
}

gboolean tcore_sim_decode_imsi(struct tel_sim_imsi *p_out, unsigned char *p_in, int in_length)
{
	int i,j=0;
	char imsi_raw[16];

	dbg( "Func Entrance");

	if ((NULL == p_out) || (NULL == p_in))
		return FALSE;

	/*
		According to 3GPP specification, the length of raw IMSI data is 9 bytes.
		first byte is length of IMSI
		second byte byte has parity nibble, so second byte has one digit of IMSI. other byte has two digit of IMSI
	*/
	if ((in_length == 0) || (in_length == 0xff) || (4 > in_length) || (9 <in_length)) {
		dbg("No valid IMSI present to convert - length:[%x]",	in_length);
		return FALSE;
	}
	dbg("imsi length[%d], input data length[%d]", p_in[0], in_length);

	/* Decode IMSI value from nibbles */
	for (i = 1; i < in_length; i++) {
		if (i == 1) { /* first byte, ignore lower nibble */
			imsi_raw[j++] = ((p_in[i] & 0xF0) >> 4) + '0';
		} else if (i == p_in[0]+1) { /* last byte */
			imsi_raw[j++] = (p_in[i] & 0x0F)+ '0';
			if (p_in[0]%2) /* count the last one if odd digits */
				imsi_raw[j++] = ((p_in[i] & 0xF0) >> 4) + '0';
		} else {
			imsi_raw[j++] = (p_in[i]  & 0x0F) + '0';
			imsi_raw[j++] = ((p_in[i] & 0xF0) >> 4) + '0';
		}
	}
	/* Terminate string */
	imsi_raw[j] = '\0';
	dbg("imsi_raw[%s], size[%d]", imsi_raw, strlen(imsi_raw));

	memcpy(p_out->plmn, imsi_raw, 5);
	p_out->plmn[5] = '\0';
	memcpy(p_out->msin, imsi_raw+5, strlen(imsi_raw)-5);
	p_out->msin[strlen(imsi_raw)-5] = '\0';

	dbg("p_out->plmn[%s], p_out->msin[%s]", p_out->plmn, p_out->msin);

	return TRUE;
}

gboolean tcore_sim_decode_sst(struct tel_sim_sst *p_sst, unsigned char *p_in, int in_length)
{
	unsigned char sstByte, rast, mask = 0;
	char simServiceID = 1;	// set "CHV1 disable function"
	int i, svc_count;
	char *p_index;

	memset((void*)p_sst, 0, sizeof(struct tel_sim_sst));

	if (in_length == 0 || in_length > SIM_SST_LEN_MAX)
		return FALSE;

	// get count of SIM service id. one byte has four service status.
	svc_count = in_length * 4;

	/*3GPP 51.011 SST shows 56 kinds of service types. current tel_sim_sst has also 56 elements*/
	if (svc_count > SIM_SST_SERVICE_CNT_MAX)
		svc_count = SIM_SST_SERVICE_CNT_MAX;

	p_index = (char*)p_sst;

	for (i = 0; i < svc_count; i++) {
		sstByte = p_in[(simServiceID - 1) / 4];
		rast = simServiceID - 4 * (simServiceID / 4);

		switch (rast) {
			case 1:
				mask = 0x02;
				break;
			case 2:
				mask = 0x08;
				break;
			case 3:
				mask = 0x20;
				break;
			case 0:
				mask = 0x80;
				break;
		}

		if (sstByte & mask)
			*p_index = 1;
		else
			*p_index = 0;

		p_index += sizeof(char);
		simServiceID++;	// next service id
	}
	return TRUE;
}

gboolean tcore_sim_decode_spn(struct tel_sim_spn *p_spn, unsigned char *p_in, int in_length)
{
	int i;

	if (in_length == 0)
		return FALSE;

	p_spn->display_condition = p_in[0];
	dbg( "The display condition is [%d]", p_spn->display_condition);

	for (i = 1; i < SIM_SPN_LEN_MAX + 1; i++) {
		if (p_in[i] == 0xFF)
			break; /* loop break*/

		p_spn->spn[i - 1] = p_in[i];
		dbg( "EF-SPN name[%d][%c]", i, p_in[i]);
	}
	p_spn->spn[i-1] = '\0';

	return TRUE;
}

gboolean tcore_sim_decode_spdi(struct tel_sim_spdi *p_spdi, unsigned char *p_in, int in_length)
{
	int i, Src_plmn_start_len, total_data_len;

	if (in_length == 0)
		return FALSE;

	if(p_in[0] == 0xff){
		dbg("file is exist but there is no valid records");
		p_spdi->plmn_count = 0;
		memset(p_spdi->list, 0x00, sizeof(unsigned char)*7*SIM_SPDI_PLMN_MAX);
		return TRUE;
	}

	//Display info tag('A3')
	if (p_in[0] == 0xA3) {
		total_data_len = p_in[1];
		 //PLMN list tag('80')
		if (p_in[2] == 0x80) {
			p_spdi->plmn_count = p_in[3] / 3;

			/*
			 *  plmn tag 1byte
			 *  length 1byte
			 *  each plmn entry 3byte
			 */
			if (p_spdi->plmn_count > SIM_SPDI_PLMN_MAX)
				p_spdi->plmn_count =	SIM_SPDI_PLMN_MAX;

			Src_plmn_start_len = 4;

			dbg( "p_spdi->num_of_plmn_entries[%d]", p_spdi->plmn_count);

			for (i = 0; i < p_spdi->plmn_count; i++) {
				unsigned char packetInDigit[3 * 2 + 1];
				_bcd_to_digit((char*) packetInDigit, (char*) &p_in[Src_plmn_start_len], 3);
				// get MCC (mobile country code)
				memcpy(p_spdi->list[i].plmn, &(packetInDigit[0]), 6);
				p_spdi->list[i].plmn[6] = '\0';
				dbg( "SPDI PLMN[%d][%s]", i, p_spdi->list[i].plmn);

				Src_plmn_start_len = Src_plmn_start_len + 3;
			}
			return TRUE;
		}
		dbg("Current EF-SPDI has invalid data");
		return FALSE;
	}
	dbg("Current EF-SPDI has invalid data");
	return FALSE;
}

gboolean tcore_sim_decode_msisdn(struct tel_sim_msisdn *p_msisdn, unsigned char *p_in, int in_length)
{
	int X = 0;	// alpha id max length
	int alpha_id_length = 0;
	int value_length = 0;
	int bcd_byte = 0;	// dialing number max length

	memset((void*) p_msisdn, 0, sizeof(struct tel_sim_msisdn));

	if (in_length == 0)
		return FALSE;

	if (_is_empty(p_in, in_length) == TRUE) {
		memset(p_msisdn, 0, sizeof(struct tel_sim_msisdn));
		return FALSE;
	}

	X = in_length - 14;	// get alpha id max length

	if (X != 0) {
		alpha_id_length = X;
		dbg("alpha_id_length[%d]", alpha_id_length);
		if(alpha_id_length > SIM_XDN_ALPHA_ID_LEN_MAX)
			alpha_id_length = SIM_XDN_ALPHA_ID_LEN_MAX;

		value_length =  _get_string((unsigned char *)p_msisdn->name, p_in, alpha_id_length);
		p_msisdn->name[value_length] = '\0';
	}

	// get dialing number length
	// p_in[X] is BCD length of dialing number length plus TON/NPI 1 bytes.
	// Convert to digit length and subtract TON/NPI length.
	if (p_in[X] != 0xFF) {
		dbg( "Dialing number Length %d, BCD length 0x%x ",  (p_in[X] - 1) * 2, p_in[X]);

		// get TON and NPI
		p_msisdn->ton = (p_in[X + 1] >> 4) & 0x07;

		// get actual dialing number length
		bcd_byte = _get_valid_bcd_byte(&p_in[X + 2], SIM_XDN_NUMBER_LEN_MAX / 2);
		dbg( "bcd_byte[%x]", bcd_byte);

		// get dialing number/SSC string
		value_length = _bcd_to_digit((char*) p_msisdn->num, (char*) &p_in[X + 2], bcd_byte); // actual dialing number length in BCD.
		p_msisdn->num[value_length] = '\0';
		dbg( "p_msisdn->num[%s]", p_msisdn->num);
	}
	return TRUE;
}

gboolean tcore_sim_decode_xdn(struct tel_sim_dialing_number *p_xdn, unsigned char *p_in, int in_length)
{
	int X;	// alpha id max length
	int bcd_byte;	// dialing number max length

	memset((void*) p_xdn, 0, sizeof(struct tel_sim_dialing_number));

	if (in_length == 0)
		return FALSE;

	if (_is_empty(p_in, in_length) == TRUE) {
		return FALSE;	// this is empty record
	}

	X = in_length - 14;	// get alpha id max length

	if (X != 0) {
		_get_string((unsigned char *)p_xdn->alpha_id, p_in, X);
		p_xdn->alpha_id_max_len = X;
	}

	// get dialing number length
	// p_in[X] is BCD length of dialing number length plus TON/NPI 1 bytes.
	// Convert to digit length and subtract TON/NPI length.
	if (p_in[X] != 0xFF) {
		dbg( "Dialing number Length %d, BCD length 0x%x ",	(p_in[X] - 1) * 2, p_in[X]);

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
			 bcd_byte = _get_valid_bcd_byte (&p_in[X+2], TAPI_SIM_XDN_DIALING_NUMBER_LEN/2);
			 by using the 20/2; so don`t return false.

			if (p_in[X] == 0x00)
				p_xdn->num_max_len = 0;
			else
				p_xdn->num_max_len = SIM_XDN_NUMBER_LEN_MAX;
		}*/

		// get TON and NPI
		p_xdn->ton = (p_in[X + 1] >> 4) & 0x07;
		p_xdn->npi = p_in[X + 1] & 0x0F;

		// get actual dialing number length
		bcd_byte = _get_valid_bcd_byte(&p_in[X + 2], SIM_XDN_NUMBER_LEN_MAX / 2);
		dbg( "bcd_byte[%x]", bcd_byte);

		// get dialing number/SSC string
		_bcd_to_digit((char*) p_xdn->num, (char*) &p_in[X + 2], bcd_byte); // actual dialing number length in BCD.
		dbg( "p_xdn->DiallingNum[%s]", p_xdn->num);
		// get Capability/Configuration id
		p_xdn->cc_id = p_in[X + 12];
		// get Extension1 id
		p_xdn->ext1_id = p_in[X + 13];
	}
	return TRUE;
}

char* tcore_sim_encode_xdn(int out_length, struct tel_sim_dialing_number *p_xdn)
{
	int alpha_id_space =0, digit_len =0, str_len = 0;
	char bcdCode[SIM_XDN_NUMBER_LEN_MAX / 2];
	char * p_out = NULL;
	p_out = calloc(out_length,1);
	memset((void*) p_out, 0xFF, out_length);

	// get alpha id max length
	alpha_id_space = out_length - 14;

	// alpha id is too big
	str_len = strlen(p_xdn->alpha_id);
	if (alpha_id_space < str_len) {
		dbg("SIM space for alpha_id is [%d] but input alpha_id length is [%d]. so we will use [%d] byte",
				alpha_id_space, str_len, alpha_id_space);
		str_len = alpha_id_space;
	}

	digit_len = strlen(p_xdn->num);
	// this is digit length
	if ( digit_len > SIM_XDN_NUMBER_LEN_MAX) {
		dbg("SIM space for number is [%d] but input number length is [%d]. so we will use [%d] byte",
				SIM_XDN_NUMBER_LEN_MAX, digit_len, SIM_XDN_NUMBER_LEN_MAX);
		digit_len = SIM_XDN_NUMBER_LEN_MAX;
	}

	_set_string((unsigned char *)p_out, (unsigned char *)p_xdn->alpha_id, str_len);

	// set length of BCD number/SSC contents
	// p_xdn->diallingnumLen is maximum digit length. = 20 bytes.
	// convert to BCD length and add 1 byte.
	p_out[alpha_id_space] = ( (digit_len + 1) / 2 ) + 1;

	// set TON and NPI
	p_out[alpha_id_space + 1] = 0x80;
	p_out[alpha_id_space + 1] |= (p_xdn->ton & 0x07) << 4;
	p_out[alpha_id_space + 1] |= p_xdn->npi & 0x0F;

	// set dialing number/SSC string
	memset((void*) bcdCode, 0xFF, SIM_XDN_NUMBER_LEN_MAX / 2);

	_digit_to_bcd((char*) bcdCode, (char*) p_xdn->num, digit_len);

	memcpy((void*) &p_out[alpha_id_space + 2], bcdCode, SIM_XDN_NUMBER_LEN_MAX / 2);

	// set Capability/Configuration Identifier
	p_out[alpha_id_space + 12] = (unsigned char) p_xdn->cc_id;
	// set extension1 record Identifier
	p_out[alpha_id_space + 13] = (unsigned char) p_xdn->ext1_id;

	return p_out;
}

gboolean tcore_sim_decode_ecc(struct tel_sim_ecc_list *p_ecc, unsigned char *p_in, int in_length)
{
	int bcd_byte;	// dialing number max length
	int i;
	int valid_ecc_length;
	memset((void*)p_ecc, 0x00, sizeof(struct tel_sim_ecc_list));

	if(in_length%3 != 0) {
		dbg("error - invalid data length");
		return FALSE;
	}

	for(i=0; i < in_length/3; i++){
		//get the BCD length of the ECC
		bcd_byte = _get_valid_bcd_byte((unsigned char*) p_in+(i*3), 3);
		if(bcd_byte != 0) {
			valid_ecc_length = _bcd_to_digit(p_ecc->ecc[p_ecc->ecc_count].ecc_num, (char *)p_in+(i*3), bcd_byte);
			p_ecc->ecc[p_ecc->ecc_count].ecc_num[valid_ecc_length] = '\0';
			p_ecc->ecc_count++;
		}
	}
	return TRUE;
}

gboolean tcore_sim_decode_ust(struct tel_sim_ust *p_ust, unsigned char *p_in, int in_length)
{
	int i, j;
	char mask;
	char *p_index;
	memset((void*) p_ust, 0, sizeof(struct tel_sim_ust));
	p_index = (char*)p_ust;

/*	UST service is described to 74(1 byte includes 8 service status) in 31.102 r7.
	current sim_ust_s has 64 services. so in_length should be under 8 byte. */
	if (in_length > SIM_UST_BYTE_LEN_MAX)
		in_length = SIM_UST_BYTE_LEN_MAX;

	for (i = 0; i < in_length; i++) {
		mask = 0x01;	// reset mast to check first bit

		for (j = 0; j < 8; j++) {
			if (p_in[i] & mask) {
				*p_index = 1;
			}
			p_index += sizeof(char);
			mask = mask << 1;
		}
	}
	return TRUE;
}

gboolean tcore_sim_decode_est(struct tel_sim_est *p_est, unsigned char *p_in, int in_length)
{
	memset((void*) p_est, 0, sizeof(struct tel_sim_est));

	if (*p_in & 0x01)
		p_est->bFdnEnabled = TRUE;
	if (*p_in & 0x02)
		p_est->bBdnEnabled = TRUE;
	if (*p_in & 0x04)
		p_est->bAclEnabled = TRUE;

	return TRUE;
}

gboolean tcore_sim_decode_uecc(struct tel_sim_ecc *p_ecc, unsigned char* p_in, int in_length)
{
	int bcd_byte;	// dialing number max length
	unsigned char eccServiceCategory;

	if (_is_empty(p_in, in_length) == TRUE) {
		memset(p_ecc, 0, sizeof(struct tel_sim_ecc));
		return FALSE;
	}

	//get the BCD length of the ECC
	bcd_byte = _get_valid_bcd_byte(&p_in[0], SIM_ECC_BYTE_LEN_MAX);

	//get the ECC codes in digits and the length as well
	_bcd_to_digit((char*) p_ecc->ecc_num, (char*) &p_in[0], bcd_byte);

	//get the alpha identifier of ECC (
	_get_string((unsigned char*) p_ecc->ecc_string, (unsigned char*) &p_in[3], in_length - 3);

	eccServiceCategory = p_in[in_length - 1] & 0x1F;	 // Check for the first 5 bits

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
	switch (eccServiceCategory) {
		case 0x01:
			p_ecc->ecc_category = SIM_ECC_POLICE;
			break;
		case 0x02:
			p_ecc->ecc_category = SIM_ECC_AMBULANCE;
			break;
		case 0x04:
			p_ecc->ecc_category = SIM_ECC_FIREBRIGADE;
			break;
		case 0x08:
			p_ecc->ecc_category = SIM_ECC_MARAINEGUARD;
			break;
		case 0x10:
			p_ecc->ecc_category = SIM_ECC_MOUTAINRESCUE;
			break;
		default:
			p_ecc->ecc_category = SIM_ECC_SPARE;
			break;
	}
	return TRUE;
}

gboolean tcore_sim_decode_gid( struct tel_sim_gid *p_gid, unsigned char* p_in, int in_length)
{
	int i;

	memset((void*) p_gid, 0, sizeof(struct tel_sim_gid ));

	if (in_length == 0)
		return FALSE;

/*regarding 31.102, EF-GID data byte is not defined. currently 10.*/
	if (in_length >= SIM_GROUP_IDENTIFIER_LEN_MAX)
		in_length = SIM_GROUP_IDENTIFIER_LEN_MAX;

	for (i = 0; i < in_length; i++) {
		if (p_in[i] == 0xFF)
			break;

		p_gid->szGroupIdentifier[i] = p_in[i];
		p_gid->GroupIdentifierLen++;
	}
	return TRUE;
}

gboolean tcore_sim_decode_mbi(struct tel_sim_mbi *p_mbi, unsigned char *p_in, int in_length)
{
	/* EF-MBI is defined 4 mandatory, 1 optional byte in 31.102 */
	if (in_length == 0 || in_length > SIM_MAIL_BOX_IDENTIFIER_LEN_MAX)
		return FALSE;

	if ( _is_empty(p_in, in_length) == TRUE) {
		return FALSE; // this is empty record
	}

	p_mbi->voice_index = p_in[0];
	p_mbi->fax_index = p_in[1];
	p_mbi->email_index = p_in[2];
	p_mbi->other_index = p_in[3];

	// 5th byte is optional
	if (in_length == 5)
		p_mbi->video_index = p_in[4];

	return TRUE;
}

gboolean tcore_sim_encode_mbi(char *p_out, int out_length, struct tel_sim_mbi *p_mbi)
{
	p_out[0] = p_mbi->voice_index;
	p_out[1] = p_mbi->fax_index;
	p_out[2] = p_mbi->email_index;
	p_out[3] = p_mbi->other_index;

	if (out_length == 5)
		p_out[4] = p_mbi->video_index;

	return TRUE;
}

gboolean tcore_sim_decode_cff(struct tel_sim_cphs_cf *p_cff, unsigned char *p_in, int in_length)
{
	if (in_length == 0)
		return FALSE;

	dbg( "flag(0)=%x, packetlen=%d ", (unsigned int)p_in[0], in_length);
	dbg( "flag(1)=%x", p_in[1]);

	if ((p_in[0] & 0x0F) == 0x0A) {
		p_cff->b_line1 = TRUE;
	}
	if ((p_in[0] & 0xF0) == 0xA0) {
		p_cff->b_line2 = TRUE;
	}

	if (in_length > 1) {
		if ((p_in[1] & 0x0F) == 0x0A) {
			p_cff->b_fax = TRUE;
		}
		if ((p_in[1] & 0xF0) == 0xA0) {
			p_cff->b_data = TRUE;
		}
	}

	dbg("Line1 = %d, line2 = %d, Fax = %d, Data = %d ",
			p_cff->b_line1,
			p_cff->b_line2,
			p_cff->b_fax,
			p_cff->b_data);
	return TRUE;
}

char* tcore_sim_encode_cff(const struct tel_sim_cphs_cf *cff)
{
	int i, j = 0;
	char *p_out = NULL;
	unsigned char* pTemp = (unsigned char*) cff;
	unsigned char present = 0x0A;
	unsigned char absent = 0x05;

	p_out =  calloc(SIM_CPHS_CALL_FORWARDING_LEN_MAX+1, 1);

	for (i = 0; i < SIM_CPHS_CALL_FORWARDING_LEN_MAX; i++) {
		present = 0x0A;
		absent = 0x05;

		for (j = 0; j < 2; j++) {
			if (*pTemp) {
				p_out[i] = p_out[i] | present;
			} else {
				p_out[i] = p_out[i] | absent;
			}
			pTemp += sizeof(gboolean);
			present = present << 4;
			absent = absent << 4;
		}
	}
	p_out[SIM_CPHS_CALL_FORWARDING_LEN_MAX] = '\0';
	return p_out;
}

gboolean tcore_sim_decode_csp(struct tel_sim_cphs_csp *p_csp, unsigned char *p_in, int in_length)
{
	int i, j, k = 0;
	unsigned char byteSignificance = 0x00;
	unsigned char mask = 0x80;

	if (in_length == 0)
		return FALSE;

	memset((void*) p_csp, 0, sizeof(struct tel_sim_cphs_csp));

/* current telephony supports 22 byte cphs-csp data. 18 byte is mandatory, the other is optional. */
	for (i = 0, j = 0; i < SIM_CPHS_CSP_LEN_MAX || j < SIM_CPHS_CSP_LEN_MAX; i++, j++) {
		p_csp->ServiceProfileEntry[j].CustomerServiceGroup = (enum tel_sim_cphs_csp_group) p_in[i];
		byteSignificance = p_in[++i];
		mask = 0x80;

		switch (p_csp->ServiceProfileEntry[j].CustomerServiceGroup) {
			case 0x01:
				for (k = 0; k < 5; k++) {
					switch (byteSignificance & mask) {
						case 0x80:
							p_csp->ServiceProfileEntry[j].u.CallOffering.bCallForwardingUnconditional = TRUE;
							break;
						case 0x40:
							p_csp->ServiceProfileEntry[j].u.CallOffering.bCallForwardingOnUserBusy = TRUE;
							break;
						case 0x20:
							p_csp->ServiceProfileEntry[j].u.CallOffering.bCallForwardingOnNoReply = TRUE;
							break;
						case 0x10:
							p_csp->ServiceProfileEntry[j].u.CallOffering.bCallForwardingOnUserNotReachable = TRUE;
							break;
						case 0x08:
							p_csp->ServiceProfileEntry[j].u.CallOffering.bCallTransfer = TRUE;
							break;
						default:
							break;
					}
					mask = mask >> 1;
				}
				break;

			case 0x02:
				for (k = 0; k < 5; k++) {
					switch (byteSignificance & mask) {
						case 0x80:
							p_csp->ServiceProfileEntry[j].u.CallRestriction.bBarringOfAllOutgoingCalls = TRUE;
							break;
						case 0x40:
							p_csp->ServiceProfileEntry[j].u.CallRestriction.bBarringOfOutgoingInternationalCalls = TRUE;
							break;
						case 0x20:
							p_csp->ServiceProfileEntry[j].u.CallRestriction.bBarringOfOutgoingInternationalCallsExceptHplmn = TRUE;
							break;
						case 0x10:
							p_csp->ServiceProfileEntry[j].u.CallRestriction.bBarringOfAllIncomingCallsRoamingOutsideHplmn = TRUE;
							break;
						case 0x08:
							p_csp->ServiceProfileEntry[j].u.CallRestriction.bBarringOfIncomingCallsWhenRoaming = TRUE;
							break;
						default:
							break;
					}
					mask = mask >> 1;
				}
				break;

			case 0x03:
				for (k = 0; k < 5; k++) {
					switch (byteSignificance & mask) {
						case 0x80:
							p_csp->ServiceProfileEntry[j].u.OtherSuppServices.bMultiPartyService = TRUE;
							break;
						case 0x40:
							p_csp->ServiceProfileEntry[j].u.OtherSuppServices.bClosedUserGroup = TRUE;
							break;
						case 0x20:
							p_csp->ServiceProfileEntry[j].u.OtherSuppServices.bAdviceOfCharge = TRUE;
							break;
						case 0x10:
							p_csp->ServiceProfileEntry[j].u.OtherSuppServices.bPreferentialClosedUserGroup = TRUE;
							break;
						case 0x08:
							p_csp->ServiceProfileEntry[j].u.OtherSuppServices.bClosedUserGroupOutgoingAccess = TRUE;
							break;
						default:
							break;
					}
					mask = mask >> 1;
				}
				break;

			case 0x04:
				for (k = 0; k < 4; k++) {
					switch (byteSignificance & mask) {
						case 0x80:
							p_csp->ServiceProfileEntry[j].u.CallComplete.bCallHold = TRUE;
							break;
						case 0x40:
							p_csp->ServiceProfileEntry[j].u.CallComplete.bCallWaiting = TRUE;
							break;
						case 0x20:
							p_csp->ServiceProfileEntry[j].u.CallComplete.bCompletionOfCallToBusySubscriber = TRUE;
							break;
						case 0x10:
							p_csp->ServiceProfileEntry[j].u.CallComplete.bUserUserSignalling = TRUE;
							break;
						default:
							break;
					}
					mask = mask >> 1;
				}
				break;

			case 0x05:
				for (k = 0; k < 7; k++) {
					switch (byteSignificance & mask) {
						case 0x80:
							p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageMobileTerminated = TRUE;
							break;
						case 0x40:
							p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageMobileOriginated = TRUE;
							break;
						case 0x20:
							p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageCellBroadcast = TRUE;
							break;
						case 0x10:
							p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageReplyPath = TRUE;
							break;
						case 0x08:
							p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageDeliveryConf = TRUE;
							break;
						case 0x04:
							p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageProtocolIdentifier = TRUE;
							break;
						case 0x02:
							p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageValidityPeriod = TRUE;
							break;
						default:
							break;
					}
					mask = mask >> 1;
				}
				break;

			case 0x06:
				for (k = 0; k < 1; k++) {
					switch (byteSignificance & mask) {
						case 0x80:
							p_csp->ServiceProfileEntry[j].u.CphsTeleservices.bAlternativeLineService =  TRUE;
							break;
						default:
							break;
					}
					mask = mask >> 1;
				}
				break;

			case 0x07:
				for (k = 0; k < 1; k++) {
					switch (byteSignificance & mask) {
						case 0x80:
							p_csp->ServiceProfileEntry[j].u.CphsFeatures.bStringServiceTable = TRUE;
							break;
						default:
							break;
					}
					mask = mask >> 1;
				}
				break;

			case 0x08:
				for (k = 0; k < 8; k++) {
					switch (byteSignificance & mask) {
						case 0x80:
							p_csp->ServiceProfileEntry[j].u.NumberIdentifiers.bCallingLineIdentificationPresent = TRUE;
							break;
						case 0x20:
							p_csp->ServiceProfileEntry[j].u.NumberIdentifiers.bConnectedLineIdentificationRestrict = TRUE;
							break;
						case 0x10:
							p_csp->ServiceProfileEntry[j].u.NumberIdentifiers.bConnectedLineIdentificationPresent = TRUE;
							break;
						case 0x08:
							p_csp->ServiceProfileEntry[j].u.NumberIdentifiers.bMaliciousCallIdentifier = TRUE;
							break;
						case 0x02:
							p_csp->ServiceProfileEntry[j].u.NumberIdentifiers.bCallingLineIdentificationSend = TRUE;
							break;
						case 0x01:
							p_csp->ServiceProfileEntry[j].u.NumberIdentifiers.bCallingLineIdentificationBlock = TRUE;
							break;
						default:
							break;
					}
					mask = mask >> 1;
				}
				break;

			case 0x09:
				for (k = 0; k < 6; k++) {
					switch (byteSignificance & mask) {
						case 0x80:
							p_csp->ServiceProfileEntry[j].u.PhaseServices.bMenuForGprs = TRUE;
							break;
						case 0x40:
							p_csp->ServiceProfileEntry[j].u.PhaseServices.bMenuForHighSpeedCsd = TRUE;
							break;
						case 0x20:
							p_csp->ServiceProfileEntry[j].u.PhaseServices.bMenuForVoiceGroupCall = TRUE;
							break;
						case 0x10:
							p_csp->ServiceProfileEntry[j].u.PhaseServices.bMenuForVoiceBroadcastService = TRUE;
							break;
						case 0x08:
							p_csp->ServiceProfileEntry[j].u.PhaseServices.bMenuForMultipleSubscriberProfile = TRUE;
							break;
						case 0x04:
							p_csp->ServiceProfileEntry[j].u.PhaseServices.bMenuForMultipleBand = TRUE;
							break;
						default:
							break;
					}
					mask = mask >> 1;
				}
				break;

			case 0xC0:
				for (k = 0; k < 8; k++) {
					switch (byteSignificance & mask) {
						case 0x80:
							p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForManualSelection = TRUE;
							break;
						case 0x40:
							p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForVoiceMail = TRUE;
							break;
						case 0x20:
							p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForMoSmsAndPaging = TRUE;
							break;
						case 0x10:
							p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForMoSmsWithEmialType = TRUE;
							break;
						case 0x08:
							p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForFaxCalls = TRUE;
							break;
						case 0x04:
							p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForDataCalls = TRUE;
							break;
						case 0x01:
							p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForChangeLanguage = TRUE;
							break;
						default:
							break;
					}
					mask = mask >> 1;
				}
				break;

			case 0xD5:
				for (k = 0; k < 8; k++) {
					switch (byteSignificance & mask) {
						case 0x80:
						case 0x40:
						case 0x20:
						case 0x10:
						case 0x08:
						case 0x04:
						case 0x02:
						case 0x01:
							p_csp->ServiceProfileEntry[j].u.InformationNumbers.bInformationNumbers = TRUE;
							break;
						default:
							break;
					}
					mask = mask >> 1;
				}
				break;

			default:
				break;
		}
	}
	return TRUE;
}

gboolean tcore_sim_encode_csp(unsigned char *p_out, int out_length, struct tel_sim_cphs_csp *p_csp)
{
	unsigned char i, j = 0;

	if (out_length == 0)
		return FALSE;

	memset((void*) p_out, 0xFF, out_length);

/* current telephony supports 22 byte cphs-csp data. 18 byte is mandatory, the other is optional.*/
	for (i = 0, j = 0; i < 22 || j < 22; i++, j++) {
		p_out[i] = (unsigned char) p_csp->ServiceProfileEntry[j].CustomerServiceGroup;
		switch (p_out[i]) {
			case 0x01:
				p_out[++i] =	(((unsigned char) p_csp->ServiceProfileEntry[j].u.CallOffering.bCallForwardingUnconditional) << 7)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.CallOffering.bCallForwardingOnUserBusy) << 6)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.CallOffering.bCallForwardingOnNoReply) << 5)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.CallOffering.bCallForwardingOnUserNotReachable) << 4)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.CallOffering.bCallTransfer) << 3);
				break;

			case 0x02:
				p_out[++i] =	(((unsigned char) p_csp->ServiceProfileEntry[j].u.CallRestriction.bBarringOfAllOutgoingCalls) << 7)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.CallRestriction.bBarringOfOutgoingInternationalCalls) << 6)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.CallRestriction.bBarringOfOutgoingInternationalCallsExceptHplmn) << 5)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.CallRestriction.bBarringOfAllIncomingCallsRoamingOutsideHplmn) << 4)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.CallRestriction.bBarringOfIncomingCallsWhenRoaming) << 3);
				break;

			case 0x03:
				p_out[++i] =	(((unsigned char) p_csp->ServiceProfileEntry[j].u.OtherSuppServices.bMultiPartyService) << 7)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.OtherSuppServices.bClosedUserGroup) << 6)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.OtherSuppServices.bAdviceOfCharge) << 5)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.OtherSuppServices.bPreferentialClosedUserGroup) << 4)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.OtherSuppServices.bClosedUserGroupOutgoingAccess) << 3);
				break;

			case 0x04:
				p_out[++i] = (((unsigned char) p_csp->ServiceProfileEntry[j].u.CallComplete.bCallHold) << 7)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.CallComplete.bCallWaiting) << 6)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.CallComplete.bCompletionOfCallToBusySubscriber) << 5)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.CallComplete.bUserUserSignalling) << 4);
				break;

			case 0x05:
				p_out[++i] = (((unsigned char) p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageMobileTerminated) << 7)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageMobileOriginated) << 6)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageCellBroadcast) << 5)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageReplyPath) << 4)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageDeliveryConf) << 3)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageProtocolIdentifier) << 2)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.Teleservices.bShortMessageValidityPeriod) << 1);
				break;

			case 0x06:
				p_out[++i] = (((unsigned char) p_csp->ServiceProfileEntry[j].u.CphsTeleservices.bAlternativeLineService) << 7);
				break;

			case 0x07:
				p_out[++i] = (((unsigned char) p_csp->ServiceProfileEntry[j].u.CphsFeatures.bStringServiceTable) << 7);
				break;

			case 0x08:
				p_out[++i] = (((unsigned char) p_csp->ServiceProfileEntry[j].u.NumberIdentifiers.bCallingLineIdentificationPresent) << 7)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.NumberIdentifiers.bConnectedLineIdentificationRestrict) << 5)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.NumberIdentifiers.bConnectedLineIdentificationPresent) << 4)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.NumberIdentifiers.bMaliciousCallIdentifier) << 3)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.NumberIdentifiers.bCallingLineIdentificationSend) << 1)
								+ ((unsigned char) p_csp->ServiceProfileEntry[j].u.NumberIdentifiers.bCallingLineIdentificationBlock);
				break;

			case 0x09:
				p_out[++i] =	(((unsigned char) p_csp->ServiceProfileEntry[j].u.PhaseServices.bMenuForGprs) << 7)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.PhaseServices.bMenuForHighSpeedCsd) << 6)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.PhaseServices.bMenuForVoiceGroupCall) << 5)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.PhaseServices.bMenuForVoiceBroadcastService) << 4)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.PhaseServices.bMenuForMultipleSubscriberProfile) << 3)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.PhaseServices.bMenuForMultipleBand) << 2);
				break;

			case 0xC0:
				p_out[++i] =	(((unsigned char) p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForManualSelection) << 7)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForVoiceMail) << 6)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForMoSmsAndPaging) << 5)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForMoSmsWithEmialType) << 4)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForFaxCalls) << 3)
								+ (((unsigned char) p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForDataCalls) << 2)
								+ ((unsigned char) p_csp->ServiceProfileEntry[j].u.ValueAddedServices.bRestrictMenuForChangeLanguage);
				break;

			case 0xD5:
				if (p_csp->ServiceProfileEntry[j].u.InformationNumbers.bInformationNumbers)
					p_out[++i] = 0xFF;
				else
					p_out[++i] = 0x00;
				break;

			default:
				break;
		}
	}
	return TRUE;
}

gboolean tcore_sim_decode_mwis(struct tel_sim_mw *pMwis, unsigned char *p_in, int in_length)
{
	int i;
	unsigned char type = 0;
	unsigned char mask = 0x01;

	if (in_length == 0)
		return FALSE;

	memset((void*) pMwis, 0, sizeof(struct tel_sim_mw));

	type = p_in[0];  //0x07

	if (type) {
		for (i = 0; i < 5; i++) {
			switch (type & mask) {
				case 0x01:
					pMwis->indicator_status = pMwis->indicator_status | SIM_MWIS_VOICE;
					break;
				case 0x02:
					pMwis->indicator_status = pMwis->indicator_status | SIM_MWIS_FAX;
					break;
				case 0x04:
					pMwis->indicator_status = pMwis->indicator_status | SIM_MWIS_EMAIL;
					break;
				case 0x08:
					pMwis->indicator_status = pMwis->indicator_status | SIM_MWIS_OTHER;
					break;
				case 0x10:
					pMwis->indicator_status = pMwis->indicator_status | SIM_MWIS_VIDEO;
					break;
				default:
					pMwis->indicator_status = pMwis->indicator_status | SIM_MWIS_NONE;
					break;
			}
			mask = mask << 1;
		}

		pMwis->voice_count = p_in[1];
		pMwis->fax_count = p_in[2];
		pMwis->email_count = p_in[3];
		pMwis->other_count = p_in[4];

		if (in_length == 6)
			pMwis->video_count = p_in[5];
	}
	return TRUE;
}

char* tcore_sim_encode_mwis( int *out_length, const struct tel_sim_mw *pMwis)
{
	char *p_out = NULL;

	if (out_length == 0)
		return FALSE;

	p_out = calloc(6,1);

	p_out[0] = (unsigned char) pMwis->indicator_status;
	p_out[1] = pMwis->voice_count;
	p_out[2] = pMwis->fax_count;
	p_out[3] = pMwis->email_count;
	p_out[4] = pMwis->other_count;
	p_out[5] = pMwis->video_count;

	*out_length = 6;
	return p_out;
}

gboolean tcore_sim_decode_vmwf(struct tel_sim_cphs_mw *p_vmwf,	unsigned char* p_in, unsigned long in_length)
{
	int i, j = 0;
	unsigned char* pTemp = (unsigned char*) p_vmwf;
	unsigned char mask = 0x0F;
	unsigned char voiceMsgFlg = 0;

	if (in_length == 0){
		dbg("fail - input length is zero");
		return FALSE;
	}

/*	current telephony supports 2 byte cphs-vmwf data*/
	for (i = 0; i < SIM_CPHS_VMWF_LEN_MAX; i++) {
		voiceMsgFlg = p_in[i];
		for (j = 0; j < 2; j++) {
			if ((voiceMsgFlg & mask) == 0x0A) {
				*pTemp = 1;  //TRUE
			} else if ((voiceMsgFlg & mask) == 0x05) {
				*pTemp = 0;  // FALSE
			}
			pTemp += sizeof(gboolean);
			voiceMsgFlg = voiceMsgFlg >> 4;
		}
	}
	return TRUE;
}

char* tcore_sim_encode_vmwf(int *out_length, const struct tel_sim_cphs_mw *p_vmwf)
{
	int i, j = 0;
	char *p_out = NULL;
	unsigned char* pTemp = (unsigned char*) p_vmwf;
	unsigned char present = 0x0A;
	unsigned char absent = 0x05;

	if (out_length == 0)
		return NULL;

	p_out = calloc(2,1);

	for (i = 0; i < 2; i++) {
		present = 0x0A;
		absent = 0x05;

		p_out[i] = 0x00;

		for (j = 0; j < 2; j++) {
			if (*pTemp)
				p_out[i] = p_out[i] | present;  //TRUE
			else
				p_out[i] = p_out[i] | absent;  //TRUE

			pTemp += sizeof(gboolean);
			present = present << 4;
			absent = absent << 4;
		}
	}
	*out_length = 2;
	return p_out;
}

gboolean tcore_sim_decode_ons(unsigned char* p_out,unsigned  char* p_in, int in_length)
{
	int length;
	memset((void*) p_out, 0, SIM_CPHS_OPERATOR_NAME_LEN_MAX+1);

	if (in_length == 0)
		return FALSE;

	if (_is_empty(p_in, in_length) == TRUE) {
		return FALSE;
	}

	/* current telephony supports 25 byte cphs-operator name string.*/
	if (in_length >= SIM_CPHS_OPERATOR_NAME_LEN_MAX)
		in_length = SIM_CPHS_OPERATOR_NAME_LEN_MAX;

	length = _get_string(p_out, p_in, in_length);
	p_out[length] = '\0';
	dbg( "Operator Name is (%s) & Length (%d) ",	p_out, length);

	return TRUE;
}

gboolean tcore_sim_decode_cfis(struct tel_sim_cfis *p_cfis, unsigned char *p_in, int in_length)
{
	int bcd_byte;	// dialing number max length
	int digit_len;
	int i = 0;
	if (in_length == 0)
		return FALSE;

	if (_is_empty(p_in, in_length) == TRUE) {
		dbg("empty record. all data is set 0xff");
		return TRUE;	// this is empty record
	}

	p_cfis->msp_num = p_in[i++];
	p_cfis->cfu_status = p_in[i++];

	// get TON and NPI
	p_cfis->ton = (p_in[++i] >> 4) & 0x07;
	p_cfis->npi = p_in[i++] & 0x0F;

	// get actual dialing number length
	/* current telephony supports 20 byte dialing number format. */
	bcd_byte = _get_valid_bcd_byte(&p_in[i], SIM_XDN_NUMBER_LEN_MAX / 2);

	// get dialing number/SSC string
	digit_len = _bcd_to_digit((char*) p_cfis->cfu_num, (char*) &p_in[i], bcd_byte); // actual dialing number length in BCD.
	dbg( "Dialing number Length[%d]", digit_len);

	i = i + SIM_XDN_NUMBER_LEN_MAX / 2;

	// get Capability/Configuration id
	p_cfis->cc2_id = p_in[i++];

	// get Extension1 id
	p_cfis->ext7_id = p_in[i];

	dbg( "MspNumber 0x%x", p_cfis->msp_num);
	dbg( "Status 0x%x", p_cfis->cfu_status);
	dbg( "TypeOfNumber %d", p_cfis->ton);
	dbg( "NumberingPlanIdent %d", p_cfis->npi);
	dbg( "Dialing number[%s]", p_cfis->cfu_num);

	return TRUE;
}

char* tcore_sim_encode_cfis(int *out_length, const struct tel_sim_cfis *p_cfis)
{
	char *encoded_o = NULL;
	char bcd[10];

	encoded_o = calloc(16, 1); // EF-CFIS record length is 16
	memset(bcd, 0xff, 10);

	/*
	 Bytes	Description							M/O		Length
	 1		MSP number							M		1 byte
	 2		CFU indicator status					M		1 byte
	 3		Length of BCD number					M		1 byte
	 4		TON and NPI							M		1 byte
	 5 to 14	Dialing Number						M		10 bytes. unused byte should be set with 'F'
	 15		Capability/Configuration2 Record Identifier	M		1 byte
	 16		Extension 7 Record Identifier				M		1 byte
	 */
	encoded_o[0] = p_cfis->msp_num;
	encoded_o[1] = p_cfis->cfu_status;

	encoded_o[2] = (strlen(p_cfis->cfu_num) +1) /2;

	// set TON and NPI
	encoded_o[3] = 0x80;
	encoded_o[3] |= (p_cfis->ton & 0x07) << 4;
	encoded_o[3] |= p_cfis->npi & 0x0F;

	_digit_to_bcd(bcd, (char*)&p_cfis->cfu_num, strlen(p_cfis->cfu_num));
	memcpy(&encoded_o[4], bcd, 10);

	encoded_o[14] = p_cfis->cc2_id;
	encoded_o[15] = p_cfis->ext7_id;

	*out_length = 16;
	return encoded_o;
}

gboolean tcore_sim_decode_dynamic_flag(struct tel_sim_cphs_dflag *p_df, unsigned char *p_in, int in_length)
{
	if (in_length == 0)
		return FALSE;

	memset((void*) p_df, 0, sizeof(struct tel_sim_cphs_dflag));

	switch (p_in[0] & 0x01) {
		case 0x00:
			p_df->DynamicFlags = SIM_DYNAMIC_FLAGS_LINE2;
			break;

		case 0x01:
			p_df->DynamicFlags = SIM_DYNAMIC_FLAGS_LINE1;
			break;

		default:
			break;
	}
	return TRUE;
}

gboolean tcore_sim_decode_dynamic2_flag(struct tel_sim_cphs_dflag2 *p_d2f, unsigned char *p_in, int in_length)
{
	if (in_length == 0)
		return FALSE;

	memset((void*) p_d2f, 0, sizeof(struct tel_sim_cphs_dflag2));

	switch (p_in[0] & 0x01) {
		case 0x00:
			p_d2f->Dynamic2Flag = SIM_PIN2_ACCESSIBLE_FLAGS_UNLOCKED;
			break;
		case 0x01:
			p_d2f->Dynamic2Flag = SIM_PIN2_ACCESSIBLE_FLAGS_LOCKED;
			break;
		default:
			break;
	}
	return TRUE;
}

gboolean tcore_sim_encode_dynamic_flag(char *p_out, int out_length, struct tel_sim_cphs_dflag *p_df)
{
	if (out_length == 0 || out_length > 1)
		return FALSE;

	memset((void*) p_out, 0xFF, out_length);
	p_out[0] = p_df->DynamicFlags;
	return TRUE;
}

gboolean tcore_sim_encode_dynamic2_flag(char *p_out, int out_length, struct tel_sim_cphs_dflag2 *p_d2f)
{
	if (out_length == 0 || out_length > 1)
		return FALSE;

	memset((void*) p_out, 0xFF, out_length);
	p_out[0] = p_d2f->Dynamic2Flag;
	return TRUE;
}

gboolean tcore_sim_decode_cphs_info(struct tel_sim_cphs_info *pCphsInfo, unsigned char *p_in, int in_length)
{
	int i, j = 0;
	unsigned char mask = 0x03;
	unsigned char* pTemp = (unsigned char*) &pCphsInfo->CphsServiceTable;

	memset((void*) pCphsInfo, 0, sizeof(struct tel_sim_cphs_info));

	if (in_length == 0)
		return FALSE;

	 /*CPHS info EF has 3 bytes data.*/
	if (in_length >= SIM_CPHS_INFO_LEN_MAX)
		in_length = SIM_CPHS_INFO_LEN_MAX;

	switch (p_in[0]) {
		case 0x01:
			pCphsInfo->CphsPhase = SIM_CPHS_PHASE1;
			break;
		case 0x02:
			pCphsInfo->CphsPhase = SIM_CPHS_PHASE2;
			break;
		default:
			pCphsInfo->CphsPhase = SIM_CPHS_PHASE_RFU;
			break;
	}

	dbg( "Cphs Phase %d \n", pCphsInfo->CphsPhase);

	for (i = 1; i < in_length; i++)	{ //CPHS SST is only 2 bytes
		mask = 0x03;	// reset mast to check first bit
		for (j = 0; j < 4; j++) {
			if (p_in[i] & mask) {
				*pTemp = 1;
			}
			dbg( "Byte (%d), service (%d) ", i+1, *pTemp);
			pTemp += sizeof(gboolean);
			mask = mask << 2;
		}
	}
	return TRUE;
}

gboolean tcore_sim_decode_short_ons(unsigned char *p_out, unsigned char *p_in, int in_length)
{
	int length;

	memset(p_out, 0x00, SIM_CPHS_OPERATOR_NAME_SHORT_FORM_LEN_MAX+1);

	if (_is_empty(p_in, in_length) == TRUE){
		return FALSE;	// this is empty record
	}

	/*CPHS specification shows current EF include 10 bytes */
	if (in_length == 0)
		return FALSE;

	if(in_length > SIM_CPHS_OPERATOR_NAME_SHORT_FORM_LEN_MAX)
		in_length = SIM_CPHS_OPERATOR_NAME_SHORT_FORM_LEN_MAX;

	length = _get_string( p_out, p_in, in_length );
	p_out[length] = '\0';
	dbg( "Operator short Name is (%s) &  length (%d)", p_out, length);
	return TRUE;
}

gboolean tcore_sim_decode_information_number(struct tel_sim_cphs_info_number *p_info, unsigned char* p_in, int in_length)
{
	int i;

	if (in_length == 0)
		return FALSE;

	for (i = 0; i < in_length; i++) {
		dbg( " \t0x%04X.", p_in[i]);
	}

	if (_is_empty(p_in, in_length) == TRUE) {
		return FALSE;	// this is empty record
	}

	p_info->AlphaIdLength = p_in[0];

	if (p_in[1] & 0x0F)
		p_info->IndexLevelIndicator =	(enum tel_sim_cphs_index_level) (p_in[1] & 0x0F);

	if (p_in[1] & 0x10)
		p_info->PremiumServiceIndicator = TRUE;

	if (p_in[1] & 0x20)
		p_info->PremiumServiceIndicator = TRUE;

	_get_string(p_info->Alpha_id, &p_in[2],	p_info->AlphaIdLength);

	p_info->DiallingnumLength = p_in[2 + p_info->AlphaIdLength] * 2;
	p_info->TypeOfNumber = (p_in[3 + p_info->AlphaIdLength] >> 4) & 0x07;
	p_info->NumberingPlanIdentity = p_in[3 + p_info->AlphaIdLength]	& 0x0F;

	// get dialing number/SSC string
	_bcd_to_digit((char*) p_info->DiallingnumLength, (char*) &p_in[4 + p_info->AlphaIdLength],	p_info->DiallingnumLength / 2); // actual dialing number length in BCD.
	// get Extension1 id
	p_info->Ext1RecordId = p_in[4 + p_info->AlphaIdLength + p_info->DiallingnumLength / 2];

	return TRUE;
}

gboolean tcore_sim_decode_opl(struct tel_sim_opl *p_opl, unsigned char *p_in, int in_length)
{
	unsigned char packetInDigit[3 * 2 + 1];

	if (_is_empty(p_in, in_length) == TRUE) {
		memset(p_opl, 0x00, sizeof(struct tel_sim_opl));
		return FALSE;	// this is empty record
	}
	_bcd_to_digit((char*) packetInDigit, (char*) &p_in[0], 3);
	dbg( "AFTER _bcd_to_digit 4th[0x%x]", packetInDigit[3]);

	// get MCC
	memcpy(&p_opl->plmn, &(packetInDigit[0]), 3);
	// get MNC
	if (packetInDigit[3] == 0x00){
		memcpy(&(p_opl->plmn[3]), &(packetInDigit[3 + 1]), 2);
		p_opl->plmn[5] = '\0';
	} else{
		memcpy(&(p_opl->plmn[3]), &(packetInDigit[3]), 3);
		p_opl->plmn[6] = '\0';
	}

	dbg( " PLMN Code[%s]", p_opl->plmn);
	p_opl->lac_from = (*(p_in+3) << 8) | (*(p_in+4) & 0x00ff);
	dbg( " Start value of the LAC range[%x]",	p_opl->lac_from);
	p_opl->lac_to = (*(p_in+5) << 8) | (*(p_in+6) & 0x00ff);
	dbg( " End value of the LAC range[%x]", p_opl->lac_to);
	p_opl->rec_identifier = p_in[7];
	dbg( " PNN Record identifier[%x]", p_opl->rec_identifier);
	return TRUE;
}

gboolean tcore_sim_decode_pnn(struct tel_sim_pnn *p_pnn, unsigned char* p_in, int in_length)
{
	int f_name_len = 0, s_name_len = 0;
	int cvt_leng = 0, s_name_base = 0;

	if (_is_empty(p_in, in_length) == TRUE) {
		memset(p_pnn, 0x00, sizeof(struct tel_sim_pnn));
		return FALSE;	// this is empty record
	}

	/*Full name for network IEI(Information Element Identifier),0x43*/
	if (p_in[0] == 0x43) {
		dbg( " Full name of network IEI exist");
		//f_name_part includes information byte.
		f_name_len =  (int)p_in[1] - 1;

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
		if ((p_in[2] & 0x70) >> 4 == 0) {
			dbg( "DCS:GSM7");
			// In case of GSM7, 35byte packing data will be converted 40 bytes unpacking string.
			if (f_name_len > (SIM_NW_FULL_NAME_LEN_MAX * 7) / 8)
				f_name_len = (SIM_NW_FULL_NAME_LEN_MAX * 7) / 8;

			_unpack_7bit28bit(p_in + 3, f_name_len, (unsigned char *) (p_pnn->full_name));
		} else if ((p_in[2] & 0x70) >> 4 == 1) {
			dbg( "DCS:UCS2");
			/* current telephony supports 40 bytes network name string */
			if (f_name_len > SIM_NW_FULL_NAME_LEN_MAX)
				f_name_len = SIM_NW_FULL_NAME_LEN_MAX;

			_ucs2_to_utf8(f_name_len, p_in + 3, (int*) &cvt_leng, (unsigned char *) (p_pnn->full_name));
		} else {
			dbg( "DCS:unknown");
			return FALSE;
		}
		dbg( " Full name of network contents[%s]", p_pnn->full_name);

		s_name_base = (int)p_in[1] +2;
		dbg( " short name base byte [0x%02x]", s_name_base);

		/*Short Name for network IEI(Information Element Identifier), 0x45*/
		if (p_in[s_name_base] == 0x45) {
			dbg( " Short name of network IEI exist");
			//s_name_part includes information byte.
			s_name_len = p_in[s_name_base +1] -1;

			if ((p_in[s_name_base + 2] & 0x70) >> 4 == 0) {
				dbg( "DCS:GSM7");
				// In case of GSM7, 35byte packing data will be converted 40 bytes unpacking string.
				if (s_name_len > (SIM_NW_FULL_NAME_LEN_MAX * 7) / 8)
					s_name_len = (SIM_NW_FULL_NAME_LEN_MAX * 7) / 8;

				_unpack_7bit28bit(p_in + s_name_base + 3, s_name_len, (unsigned char *) (p_pnn->short_name));
			} else if ((p_in[s_name_base +2] & 0x70) >> 4 == 1) {
				dbg( "DCS:UCS2");
				if (s_name_len > SIM_NW_FULL_NAME_LEN_MAX)
					s_name_len = SIM_NW_FULL_NAME_LEN_MAX;

				_ucs2_to_utf8(s_name_len, p_in + s_name_base+ 3, (int*) &cvt_leng, (unsigned char *) (p_pnn->short_name));
			} else {
				dbg( "DCS:unknown");
				return FALSE;
			}
			dbg( " Short name of network contents[%s]", p_pnn->short_name);
		}
		return TRUE;
	}
	return FALSE;
}

gboolean tcore_sim_decode_oplmnwact(struct tel_sim_oplmnwact_list *p_list, unsigned char *p_in, int in_length)
{
	unsigned long m = 0;
//unsigned long k, j, m = 0;
//	unsigned char mask = 0x00;

	//current raw data can include invalid OPLMN data(ex: ff ff ff 00 00). so we can`t decide the number of OPLMN records directly.
	int rawOplmnWactCount = 0;
	int i = 0;

	memset((void*) p_list, 0, sizeof(struct tel_sim_oplmnwact_list));

	rawOplmnWactCount = in_length / 5;

	dbg( "rawOplmnWactCount[%d]", rawOplmnWactCount);

	for (i = 0; i < rawOplmnWactCount; i++) {
		unsigned char packetInDigit[3 * 2 + 1];

		//Regarding current IPC data, even if there`s no OPLMN value, IPC data is sending with 'ff ff ff 00 00'. so we should check for validation.
		if (p_in[m] == 0xff) {
			p_list->opwa_count = m / 5;
			dbg("OPLMN(MCC+MNC) value is not found at p_in[m]=[%lu].So OPLMN count is [%d]", m, p_list->opwa_count);
			return TRUE;
		}

		_bcd_to_digit((char*) packetInDigit, (char*) &p_in[m], 3);
		dbg( "AFTER _bcd_to_digit 4th[0x%x]", packetInDigit[3]);

		// get MCC
		memcpy(&p_list->opwa[i].plmn, &(packetInDigit[0]), 3);
		// get MNC
		if (packetInDigit[3] == 0x00){
			memcpy(&(p_list->opwa[i].plmn[3]), &(packetInDigit[3 + 1]), 2);
			p_list->opwa[i].plmn[5] = '\0';
		} else{
			memcpy(&(p_list->opwa[i].plmn[3]), &(packetInDigit[3]), 3);
			p_list->opwa[i].plmn[6] = '\0';
		}
		dbg( "[%d] OPLMN PLMN Code[%s]", i, p_list->opwa[i].plmn);

		if(p_in[m+3] & 0x80)
			p_list->opwa[i].b_umts = 1;

		if(p_in[m+4] & 0x80)
			p_list->opwa[i].b_gsm = 1;

		m = m + 5;
	}

	p_list->opwa_count = rawOplmnWactCount;
	dbg( "OPLMN count is p_list->opwa_count[%d]", p_list->opwa_count);

	return TRUE;
}

enum tel_sim_status tcore_sim_get_status(CoreObject *o)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return -1;
	}
	return po->sim_status;
}

gboolean tcore_sim_set_status(CoreObject *o, enum tel_sim_status status)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}
	po->sim_status = status;

	return TRUE;
}

gboolean tcore_sim_get_identification(CoreObject *o)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return -1;
	}
	return po->b_sim_changed;
}

gboolean tcore_sim_set_identification(CoreObject *o, gboolean b_changed)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}
	po->b_sim_changed = b_changed;

	return TRUE;
}

enum tel_sim_type tcore_sim_get_type(CoreObject *o)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return -1;
	}
	return po->type;
}

gboolean tcore_sim_set_type(CoreObject *o, enum tel_sim_type type)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}
	po->type = type;
	return TRUE;
}

struct tel_sim_imsi* tcore_sim_get_imsi(CoreObject *o)
{
	struct tel_sim_imsi *tmp_imsi;
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return NULL;
	}
	tmp_imsi =  calloc(sizeof(struct tel_sim_imsi), 1);
	memcpy(tmp_imsi, &po->imsi, sizeof(struct tel_sim_imsi));
	return tmp_imsi;
}

gboolean tcore_sim_set_imsi(CoreObject *o, struct tel_sim_imsi *imsi)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}
	memcpy(&po->imsi, imsi, sizeof(struct tel_sim_imsi));
	return TRUE;
}

struct tel_sim_service_table* tcore_sim_get_service_table(CoreObject *o)
{
	struct tel_sim_service_table *tmp_svct;
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return NULL;
	}
	tmp_svct =  calloc(sizeof(struct tel_sim_service_table), 1);
	memcpy(tmp_svct, &po->svct, sizeof(struct tel_sim_service_table));
	return tmp_svct;
}

gboolean tcore_sim_set_service_table(CoreObject *o, struct tel_sim_service_table *svct)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}
	memcpy(&po->svct, svct, sizeof(struct tel_sim_service_table));
	return TRUE;
}

gboolean tcore_sim_get_cphs_status(CoreObject *o){
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}
	return po->b_cphs;
}

gboolean tcore_sim_set_cphs_status(CoreObject *o, gboolean b_support){
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}
	po->b_cphs = b_support;
	return TRUE;
}

gboolean tcore_sim_link_userdata(CoreObject *o, void *userdata)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}
	po->userdata = userdata;
	return TRUE;
}

void* tcore_sim_ref_userdata(CoreObject *o)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po || !po->userdata) {
		dbg("po access fail");
		return NULL;
	}
	return po->userdata;
}

static void tcore_sim_initialize_context(CoreObject *o)
{
	struct tcore_sim_operations *tmp_ops = NULL;
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return;
	}

	tmp_ops = po->ops;
	memset(po, 0x00, sizeof(struct private_object_data));
	po->ops = tmp_ops;
	po->sim_status = SIM_STATUS_UNKNOWN;
}

CoreObject *tcore_sim_new(TcorePlugin *p, const char *name,
		struct tcore_sim_operations *ops, TcoreHal *hal)
{
	CoreObject *o = NULL;
	struct private_object_data *po = NULL;

	if (!p)
		return NULL;

	o = tcore_object_new(p, name, hal);
	if (!o)
		return NULL;

	po = calloc(sizeof(struct private_object_data), 1);
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	po->ops = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_SIM);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_clone_hook(o, _clone_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	tcore_sim_initialize_context(o);

	return o;
}

void tcore_sim_free(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SIM);

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	free(po);
	tcore_object_free(o);
}
