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
#include "internal/tcore_types.h"
#include "plugin.h"
#include "queue.h"
#include "user_request.h"
#include "core_object.h"
#include "co_sim.h"

#define SIM_FILE_TYPE_TAG			0x07
#define SIM_FCP_TEMPLATE_TAG			0x62
#define SIM_FILE_DESCRIPTOR_TAG		0x82
#define SIM_FILE_IDENTIFIER_TAG		0x83
#define SIM_PROPRIETARY_INFORMATION_TAG	0xA5
#define SIM_LIFE_CYCLE_STATUS_TAG		0x8A
#define SIM_FILE_SIZE_TAG			0x80
#define SIM_TOTAL_FILE_SIZE_TAG		0x81
#define SIM_SHORT_FILE_IDENTIFIER_TAG		0x88

/* GSM file types */
#define SIM_FTYPE_RFU				0x0
#define SIM_FTYPE_MF				0x1
#define SIM_FTYPE_DF				0x2
#define SIM_FTYPE_EF				0x4

struct private_object_data {
	struct tcore_sim_operations *ops[TCORE_OPS_TYPE_MAX];

	unsigned char app_list;
	enum tel_sim_type type; /**< Provides the SIM card type*/
	enum tel_sim_status sim_status; /**< Provides the card status*/
	struct tel_sim_imsi imsi; /**< Provides IMSI information*/
	gboolean b_sim_changed; /**< Provides SIM card Identification- 0:no changed, 1:changed*/
	gboolean b_cphs; /**< Whether current SIM is for CPHS or not*/
	struct tel_sim_service_table *svct; /**< (U)SIM Service Table information*/
	struct tel_sim_ecc_list *ecc_list;
	struct tel_sim_msisdn_list *msisdn_list;
	struct tel_sim_spn *spn;
	struct tel_sim_cphs_netname *cphs_netname;
	struct tel_sim_iccid *iccid;
	struct tel_sim_cphs_csp *csp;
	struct tel_sim_ist *ist;
	void *userdata; /**< free use data*/
};

/*
 * Below table is created by referring two sites:
 *
 * 1) ITU "Mobile Network Code (MNC) for the international
 *   identification plan for mobile terminals and mobile users"
 *   which is available as an annex to the ITU operational bulletin
 *   available here: http://www.itu.int/itu-t/bulletin/annex.html
 *
 * 2) The ISO 3166 country codes list, available here:
 *    http://www.iso.org/iso/en/prods-services/iso3166ma/02iso-3166-code-lists/index.html
 *
 */
static const char *mcc_having_3digits_mnc[] = {
	"302",	/* Canada */
	"310", "311", "312", "313", "314", "315", "316",	/* United States of America */
	/* "334",*/	/* Mexico (uses 3-digits MNC only partially.) */
	"338",	/* Jamaica */
	"342",	/* Barbados */
	"344",	/* Antigua and Barbuda */
	"346",	/* Cayman Islands */
	"348",	/* British Virgin Islands */
	"365",	/* Anguilla */
	"708",	/* Honduras (Republic of) */
	/* "722", */	/* Argentine Republic (uses 3-digits MNC only partially.) */
	"732",	/* Colombia (Republic of) */
};

static const char *plmn_having_3digits_mnc[] = {
	/* Mexico */
	"334020",	/* Telcel */
	"334050",	/* Iusacell/Unefon */
	/* India */
	"405025", "405026", "405027", "405028", "405029", "405030", "405031", "405032",
	"405033", "405034", "405035", "405036", "405037", "405038", "405039", "405040",
	"405041", "405042", "405043", "405044", "405045", "405046", "405047", "405750",
	"405751", "405752", "405753", "405754", "405755", "405756", "405799", "405800",
	"405801", "405802", "405803", "405804", "405805", "405806", "405807", "405808",
	"405809", "405810", "405811", "405812", "405813", "405814", "405815", "405816",
	"405817", "405818", "405819", "405820", "405821", "405822", "405823", "405824",
	"405825", "405826", "405827", "405828", "405829", "405830", "405831", "405832",
	"405833", "405834", "405835", "405836", "405837", "405838", "405839", "405840",
	"405841", "405842", "405843", "405844", "405845", "405846", "405847", "405848",
	"405849", "405850", "405851", "405852", "405853", "405875", "405876", "405877",
	"405878", "405879", "405880", "405881", "405882", "405883", "405884", "405885",
	"405886", "405908", "405909", "405910", "405911", "405912", "405913", "405914",
	"405915", "405916", "405917", "405918", "405919", "405920", "405921", "405922",
	"405923", "405924", "405925", "405926", "405927", "405928", "405929", "405930",
	"405931", "405932",
	/* Malaysia */
	"502142", "502143", "502145", "502146", "502147", "502148",
	/* Argentina */
	"722070",	/* Movistar AR */
	"722310",	/* Claro AR */
	"722341",	/* Personal AR */
};

gboolean tcore_sim_check_plmn_having_3digits_mnc(char *plmn)
{
	int num = 0;
	int i = 0;
	char *mcc = NULL;

	mcc = g_strndup(plmn, 3 + 1);
	if (mcc) {
		mcc[3] = '\0';
		num = G_N_ELEMENTS(mcc_having_3digits_mnc);
		for (i = 0; i < num; i++) {
			if (strcmp((const char *)mcc, mcc_having_3digits_mnc[i]) == 0) {
				g_free(mcc);
				return TRUE;
			}
		}
		g_free(mcc);
	}

	num = G_N_ELEMENTS(plmn_having_3digits_mnc);
	for (i = 0; i < num; i++)
		if (strcmp((const char *)plmn, plmn_having_3digits_mnc[i]) == 0)
			return TRUE;

	return FALSE;
}

static TReturn _dispatcher(CoreObject *o, UserRequest *ur, enum tcore_ops_type ops_type)
{
	enum tcore_request_command command;
	struct private_object_data *po = tcore_object_ref_object(o);
	struct tcore_sim_operations *ops = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_SIM, TCORE_RETURN_EINVAL);
	CORE_OBJECT_VALIDATE_OPS_RETURN_VAL(ops_type, TCORE_RETURN_EINVAL);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_EINVAL);
	tcore_check_null_ret_err("ur", ur, TCORE_RETURN_EINVAL);

	ops = po->ops[ops_type];
	tcore_check_null_ret_err("ops", ops, TCORE_RETURN_FAILURE);

	command = tcore_user_request_get_command(ur);

	switch (command) {
	case TREQ_SIM_VERIFY_PINS:
		tcore_check_null_ret_err("ops->verify_pins",
			ops->verify_pins, TCORE_RETURN_ENOSYS);

		return ops->verify_pins(o, ur);

	case TREQ_SIM_VERIFY_PUKS:
		tcore_check_null_ret_err("ops->verify_puks",
			ops->verify_puks, TCORE_RETURN_ENOSYS);

		return ops->verify_puks(o, ur);

	case TREQ_SIM_CHANGE_PINS:
		tcore_check_null_ret_err("ops->change_pins",
			ops->change_pins, TCORE_RETURN_ENOSYS);

		return ops->change_pins(o, ur);

	case TREQ_SIM_GET_FACILITY_STATUS:
		tcore_check_null_ret_err("ops->get_facility_status",
			ops->get_facility_status, TCORE_RETURN_ENOSYS);

		return ops->get_facility_status(o, ur);

	case TREQ_SIM_DISABLE_FACILITY:
		tcore_check_null_ret_err("ops->disable_facility",
			ops->disable_facility, TCORE_RETURN_ENOSYS);

		return ops->disable_facility(o, ur);

	case TREQ_SIM_ENABLE_FACILITY:
		tcore_check_null_ret_err("ops->enable_facility",
			ops->enable_facility, TCORE_RETURN_ENOSYS);

		return ops->enable_facility(o, ur);

	case TREQ_SIM_GET_LOCK_INFO:
		tcore_check_null_ret_err("ops->get_lock_info",
			ops->get_lock_info, TCORE_RETURN_ENOSYS);

		return ops->get_lock_info(o, ur);

	case TREQ_SIM_TRANSMIT_APDU:
		tcore_check_null_ret_err("ops->transmit_apdu",
			ops->transmit_apdu, TCORE_RETURN_ENOSYS);

		return ops->transmit_apdu(o, ur);

	case TREQ_SIM_GET_ATR:
		tcore_check_null_ret_err("ops->get_atr",
			ops->get_atr, TCORE_RETURN_ENOSYS);

		return ops->get_atr(o, ur);

	case TREQ_SIM_SET_LANGUAGE:
	case TREQ_SIM_SET_CALLFORWARDING:
	case TREQ_SIM_SET_MESSAGEWAITING:
	case TREQ_SIM_SET_MAILBOX:
#if defined TIZEN_GLOBALCONFIG_ENABLE_CSP
	case TREQ_SIM_SET_CPHS_CSP_INFO:
#endif
		tcore_check_null_ret_err("ops->update_file",
			ops->update_file, TCORE_RETURN_ENOSYS);

		return ops->update_file(o, ur);

	case TREQ_SIM_GET_ECC:
	case TREQ_SIM_GET_LANGUAGE:
	case TREQ_SIM_GET_ICCID:
	case TREQ_SIM_GET_MAILBOX:
	case TREQ_SIM_GET_CALLFORWARDING:
	case TREQ_SIM_GET_MESSAGEWAITING:
	case TREQ_SIM_GET_CPHS_INFO:
	case TREQ_SIM_GET_SERVICE_TABLE:
	case TREQ_SIM_GET_MSISDN:
	case TREQ_SIM_GET_SPN:
	case TREQ_SIM_GET_SPDI:
	case TREQ_SIM_GET_OPL:
	case TREQ_SIM_GET_PNN:
	case TREQ_SIM_GET_CPHS_NETNAME:
	case TREQ_SIM_GET_OPLMNWACT:
	case TREQ_SIM_GET_ICON:
	case TREQ_SIM_GET_IMPI:
	case TREQ_SIM_GET_IMPU:
	case TREQ_SIM_GET_GID:
	case TREQ_SIM_GET_DOMAIN:
	case TREQ_SIM_GET_PCSCF:
	case TREQ_SIM_GET_ISIM_SERVICE_TABLE:
		tcore_check_null_ret_err("ops->read_file",
			ops->read_file, TCORE_RETURN_ENOSYS);

		return ops->read_file(o, ur);

	case TREQ_SIM_REQ_AUTHENTICATION:
		tcore_check_null_ret_err("ops->req_authentication",
			ops->req_authentication, TCORE_RETURN_ENOSYS);

		return ops->req_authentication(o, ur);

	case TREQ_SIM_SET_POWERSTATE:
		tcore_check_null_ret_err("ops->set_powerstate",
			ops->set_powerstate, TCORE_RETURN_ENOSYS);

		return ops->set_powerstate(o, ur);

	case TREQ_SIM_SET_PROVISIONING:
		tcore_check_null_ret_err("ops->set_provisioning",
			ops->set_provisioning, TCORE_RETURN_ENOSYS);

		return ops->set_provisioning(o, ur);

	default:
		warn("unhandled request command[%d]", command);
	break;
	}

	return TCORE_RETURN_SUCCESS;
}

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SIM);

	po = tcore_object_ref_object(o);
	if (po) {
		if (po->svct)
			free(po->svct);

		if (po->ecc_list)
			free(po->ecc_list);

		if (po->msisdn_list)
			free(po->msisdn_list);

		if (po->spn)
			free(po->spn);

		if (po->cphs_netname)
			free(po->cphs_netname);

		if (po->iccid)
			free(po->iccid);

		if (po->csp)
			free(po->csp);

		if (po->ist)
			free(po->ist);

		free(po);
		tcore_object_link_object(o, NULL);
	}
}

static void _reverse(char *p_in, int length)
{
	int i, j = length - 1;

	for (i = 0; i < j; i++) {
		int t = p_in[i];
		p_in[i] = p_in[j];
		p_in[j--] = t;
	}
}

static char *_acitoa(int n, char *str, int b)
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
static void _digit_to_bcd(char *bcdCode, char *digits, int digitLen)
{
	int i, j, digit;
	unsigned char higher, lower;

	/* 0123456789 -> 1032547698 */
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
		} else
			higher = 0xFF;

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
static unsigned long _bcd_to_digit(char *digit, char *bcdCode, int bcdLen)
{
	int i, h, l;
	char c[2];
	unsigned char higher, lower;
	unsigned long digitLen = 0;

	/* 0123456789 <- 1032547698 */
	memset((void *)digit, 0, bcdLen * 2);

	for (i = 0; i < bcdLen; i++) {
		higher = (bcdCode[i] >> 4) & 0x0F; /* get high nibble */

		if (higher == 0x0A)
			higher = '*'; /* =0x2A */
		else if (higher == 0x0B)
			higher = '#'; /* =0x23 */
		else if (higher == 0x0C)
			higher = 'P'; /* =0x70, DTMF Control digit */
		else if (higher == 0x0D)
			higher = '?';
		else if (higher == 0x0F)
			higher = 0;
		else {
			h = (int) higher;
			_acitoa(h, c, 16);
			higher = (char) toupper(*c);
		}

		lower = bcdCode[i] & 0x0F; /* get low nibble */

		if (lower == 0x0A)
			lower = '*';
		else if (lower == 0x0B)
			lower = '#';
		else if (lower == 0x0C)
			lower = 'P'; /* DTMF Control digit */
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

	digitLen = (unsigned long)strlen(digit);
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
static unsigned long _get_string(unsigned char *palpha_id, unsigned char *pRecord, unsigned long alphaIDMaxLen)
{
	unsigned long i, alphaIDLen = 0;
	unsigned char *pAlphaID = (unsigned char *) palpha_id;

	memset((void *)pAlphaID, 0, alphaIDMaxLen);

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
static void _set_string(unsigned char *pRecord, unsigned char *palpha_id, unsigned long alphaIDMaxLen)
{
	unsigned long i;
	unsigned char *pAlphaID = (unsigned char *) palpha_id;

	memset((void *) pRecord, 0xFF, alphaIDMaxLen);

	for (i = 0; i < alphaIDMaxLen; i++)
		pRecord[i] = pAlphaID[i];
}

static gboolean _is_empty(unsigned char *p_in, int in_length)
{
	int i;
	for (i = 0; i < in_length; i++) {
		if (p_in[i] != 0xFF)
			return FALSE;
	}

	dbg("current index has empty data");
	return TRUE; /* this is empty record */
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
static int _get_valid_bcd_byte(unsigned char *pBcdData, int bcdMaxLen)
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
static int _unpack_7bit28bit(unsigned char *pInString, unsigned int inStringLen, unsigned char *pOutString)
{
	int i = 0;
	unsigned int pos = 0;
	unsigned short shift = 0;
	int outlen = 0;
	outlen = (short int)((inStringLen * 8) / 7);

	for (i = 0; pos < inStringLen; i++, pos++) {
		pOutString[i] = (pInString[pos] << shift) & 0x7F;

		if (pos != 0) {
			/* except the first byte, a character contains some bits from the previous byte. */
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

	/* If a character is '\r'(13), change it to space(32) */
	for (i = 0; i < outlen; i++)
		if (pOutString[i] == 13) pOutString[i] = 32;

	pOutString[outlen] = '\0';

	dbg("unpack is done with outlen[%d] and array index[%d], out string[%s]", outlen, i, pOutString);
	return (unsigned int)(i);
}

static int _ucs2_to_utf8(int in_length, unsigned char *in_data, int *out_length, unsigned char *out_data)
{
	int i, j;

	if (in_length == 0 || in_data == NULL || out_data == NULL) {
		dbg("Unicode Decode Failed as text length is 0");
		return FALSE;
	}

	if (0 != (in_length % 2)) {
		dbg(" ##### Unicode decoding failed due to invalid text length [%d]",
			in_length);
		return FALSE;
	}

	for (i = 0, j = 0; i < in_length; i++) {
		out_data[i] = in_data[j];
		j = j + 2;
	}

	*out_length = (in_length / 2);
	out_data[i] = '\0';
	return TRUE;
}

static int _decode_cdma_imsi_util(char *imsi, unsigned short *mcc, unsigned char *mnc,
	unsigned long *min1, unsigned short *min2)
{
	unsigned short country_code = *mcc;
	unsigned char nw_code = *mnc;
	unsigned long imsi_min1 = *min1;
	unsigned short imsi_min2 = *min2;
	unsigned short second_three = 0;
	unsigned char thousands = 0;
	unsigned short last_three = 0;
	unsigned char min_to_digit[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
	unsigned char bcd_to_num[] = {0xFF, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	int p_digit = 0;
	int digit = 0;

	/* Convert Country code of IMSI */
	if (country_code <= 999) {
		digit = min_to_digit[country_code/100];
		imsi[p_digit++] = digit;
		country_code %= 100;

		digit = min_to_digit[country_code / 10];
		imsi[p_digit++] = digit;

		digit = min_to_digit[country_code % 10];
		imsi[p_digit++] = digit;

	} else {
		err("Invalid Country code");
		return -1;
	}

	/* Convert Network code of IMSI */
	if (nw_code <= 99) {
		digit = min_to_digit[nw_code / 10];
		imsi[p_digit++] = digit;

		digit = min_to_digit[nw_code % 10];
		imsi[p_digit++] = digit;
	} else {
		err("Invalid Network code");
		return -1;
	}

	/* Convert First Three Digits of IMSI */
	if (imsi_min2 <= 999) {
		digit = min_to_digit[imsi_min2 / 100];
		imsi[p_digit++] = digit;
		imsi_min2 %= 100;

		digit = min_to_digit[imsi_min2 / 10];
		imsi[p_digit++] = digit;

		digit = min_to_digit[imsi_min2 % 10];
		imsi[p_digit++] = digit;
	} else {
		err("Invalid IMSI_MIN2 code");
		return -1;
	}

	/* Convert Last Seven digits of IMSI */
	second_three = (imsi_min1 & 0x00FFC000) >> 14;
	thousands = (imsi_min1 & 0x00003C00) >> 10;
	last_three = (imsi_min1 & 0x000003FF) >> 0;

	thousands = bcd_to_num[thousands];
	if ((thousands != 0xFF) && (second_three <= 999) && (last_three <= 999)) {
		digit = min_to_digit[second_three / 100];
		imsi[p_digit++] = digit;

		second_three %= 100;

		digit = min_to_digit[second_three / 10];
		imsi[p_digit++] = digit;

		digit = min_to_digit[second_three % 10];
		imsi[p_digit++] = digit;

		imsi[p_digit++] = thousands;

		digit = min_to_digit[last_three / 100];
		imsi[p_digit++] = digit;

		last_three %= 100;

		digit = min_to_digit[last_three / 10];
		imsi[p_digit++] = digit;

		digit = min_to_digit[last_three % 10];
		imsi[p_digit++] = digit;
	} else {
		err("Invalid IMSI_MIN1 code");
		return -1;
	}

	return p_digit;
}

/**
 * This function is used to decode 3 bytes of plmn string encoded as TS 24.008 to 6 bytes char string.
 *
 * @return          void	.
 * @param[out]		out	decoded output string (must be 6 + 1 bytes)
 * @param[in]		in	encoded input string (must be 3 bytes)
 * @remark
 */
static void _decode_plmn(const unsigned char *in, unsigned char *out)
{
	unsigned char temp[6 + 1] = {0, };
	int i;
	unsigned char higher, lower;

	for (i = 0; i < 3; i++) {
		higher = (in[i] >> 4) & 0x0F;	/* get high nibble */
		if (higher < 0x0A) {
			/* if it is a number */
			temp[i*2] = higher + 0x30;
		} else if (higher == 0x0D) {
			/* 0x0D (BCD digit) is regarded as wild character by TS 24.008 */
			temp[i*2] = 'D';
		} else
			temp[i*2] = 0x00;

		lower = in[i] & 0x0F;	/* get low nibble */
		if (lower < 0x0A) {
			/* if it is a number */
			temp[i*2 + 1] = lower + 0x30;
		} else if (lower == 0x0D) {
			/* 0x0D (BCD digit) is regarded as wild character by TS 24.008 */
			temp[i*2 + 1] = 'D';
		} else
			temp[i*2 + 1] = 0x00;
	}

	/* according to PLMN digits order by TS 24.008 */
	/* MCC */
	out[0] = temp[1];
	out[1] = temp[0];
	out[2] = temp[3];
	/* MNC */
	out[3] = temp[5];
	out[4] = temp[4];
	out[5] = temp[2];

	out[6] = '\0';

	/*
	 * [14.08.21]
	 * As NA Operators requested,
	 * for NA operators (MCC 310 ~ 316),
	 * if 6th digit of MNC is 'F' then should be regarded as '0'.
	 */
	if (out[5] == 0x00
			&& strncmp((const char *)out, "31", 2) == 0
			&& ('0' <= out[2] && out[2] <= '6')) {
		out[5] = '0';
	}
}

static inline unsigned short int _swap_bytes16(unsigned short int x)
{
	return ((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8);
}

/**
 * According to ETSI 102.221 (3GPP specification refers it), EF-ICCID is coded by BCD, left justified and padded with 'F'.
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

	memset((void *)p_out->iccid, 0, SIM_ICCID_LEN_MAX+1);

	bcd_byte = _get_valid_bcd_byte(p_in, in_length);
	char_length = _bcd_to_digit(p_out->iccid, (char *) p_in, bcd_byte);
	dbg("bcd byte:[%d] string length:[%d]", bcd_byte, char_length);

	*(p_out->iccid+char_length) = '\0';

	return TRUE;
}

/**
 * This function is used to decode EFLP (2G)
 */
gboolean tcore_sim_decode_lp(struct tel_sim_language *p_out, unsigned char *p_in, int in_length)
{
	int i = 0;

	memset((void *) p_out, 0xFF, sizeof(struct tel_sim_language));
	p_out->language_count = 0;

	if (in_length == 0)
		return FALSE;

	/*
	 * Description of problem: language decoding was not correctly done if we
	 * used 7layers's test SIM
	 *
	 * Patch Description : The tested SIM  at 7layers has 3 language codes like [ff][ff][01].
	 * In this case we could not decode 3rd language code.
	 * So, the below 2 lines checking the UNSPECIFIED language are commented.
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
		dbg("p_out->language[%d]=[%d] ", i, p_out->language[p_out->language_count]);
		p_out->language_count++;
	}

	dbg("in_length %d, lang_cnt %d ", in_length, p_out->language_count);
	return TRUE;
}

/**
 * This function is used to encode EFLP (2G)
 */
char *tcore_sim_encode_lp(int *out_length, struct tel_sim_language *p_in)
{
	int i = 0;
	char *tmp_out = NULL;

	if (out_length == NULL || p_in == NULL) {
		dbg("out_length or p_in is null");
		return NULL;
	}

	tmp_out = (char *)malloc(p_in->language_count);
	if (!tmp_out)
		return NULL;

	memset((void *) tmp_out, 0xff, p_in->language_count);

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

	memset((void *)p_out, 0xFF, sizeof(struct tel_sim_language));
	p_out->language_count  = 0;

	if (in_length == 0)
		return FALSE;

	/*
	 * Description of problem: language decoding was not correctly done if we
	 * used 7layers's test SIM
	 *
	 * Patch Description : TS31.102 If the EFLI has the value 'FFFF' in its highest priority position,
	 * then the preferred language selection shall be the language preference in the EFPL
	 */
	if (/*TODO g_sim.CardType == SIM_TYPE_USIM && */(file_id == SIM_EF_USIM_LI || file_id == SIM_EF_LP)) {
		defaultLi = p_in[0];
		defaultLi = ((defaultLi << 8) & 0xFF00) + p_in[1];

		if (defaultLi == 0xFFFF)	/* 1st language is default. */
			return FALSE;
	}

	if (in_length > SIM_LANG_CNT_MAX)
		in_length = SIM_LANG_CNT_MAX;

	for (i = 0; i < in_length; i++) {
		tempLi[0] = p_in[i++];
		tempLi[1] = p_in[i];

		/*
		 * Description of problem: language decoding was not correctly
		 * done if we used 7layers's test SIM
		 *
		 * Patch Description : The tested SIM at specific test lab has
		 * 3 language codes like [ff][ff][ff][ff][64][65].
		 * In this case we could not decode 3rd language code.
		 * So, the below 2 lines checking the UNSPECIFIED language are commented.
		 */
		if (tempLi[0] == 0xFF || tempLi[1] == 0xFF)  /* this is always 2 bytes */
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

			default:
				warn("invalid language");
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

			default:
				warn("invalid language");
			break;
			}
		} else if (tempLi[0] == 'i' && tempLi[1] == 't')
			p_out->language[p_out->language_count] = SIM_LANG_ITALIAN;
		else if (tempLi[0] == 'f' && tempLi[1] == 'r')
			p_out->language[p_out->language_count] = SIM_LANG_FRENCH;
		else if (tempLi[0] == 'n' && tempLi[1] == 'l')
			p_out->language[p_out->language_count] = SIM_LANG_DUTCH;
		else if (tempLi[0] == 's' && tempLi[1] == 'v')
			p_out->language[p_out->language_count] = SIM_LANG_SWEDISH;
		else if (tempLi[0] == 'f' && tempLi[1] == 'i')
			p_out->language[p_out->language_count] = SIM_LANG_FINNISH;
		else if (tempLi[0] == 'n' && tempLi[1] == 'o')
			p_out->language[p_out->language_count] = SIM_LANG_NORWEGIAN;
		else if (tempLi[0] == 't' && tempLi[1] == 'r')
			p_out->language[p_out->language_count] = SIM_LANG_TURKISH;
		else if (tempLi[0] == 'h' && tempLi[1] == 'u')
			p_out->language[p_out->language_count] = SIM_LANG_HUNGARIAN;
		else if (tempLi[0] == 'p') {
			switch (tempLi[1]) {
			case 'l':
				p_out->language[p_out->language_count] = SIM_LANG_POLISH;
			break;

			case 't':
				p_out->language[p_out->language_count] = SIM_LANG_PORTUGUESE;
			break;

			default:
				warn("invalid language");
			break;
			}
		} else if (tempLi[0] == 'k' && tempLi[1] == 'o')
			p_out->language[p_out->language_count] = SIM_LANG_KOREAN;
		else if (tempLi[0] == 'z' && tempLi[1] == 'h')
			p_out->language[p_out->language_count] = SIM_LANG_CHINESE;
		else if (tempLi[0] == 'r' && tempLi[1] == 'u')
			p_out->language[p_out->language_count] = SIM_LANG_RUSSIAN;
		else if (tempLi[0] == 'j' && tempLi[1] == 'a')
			p_out->language[p_out->language_count] = SIM_LANG_JAPANESE;

		dbg("count %d & Codes %d ", p_out->language_count, p_out->language[p_out->language_count]);
		p_out->language_count++;
	}

	if (p_out->language_count == 0) {
		dbg("p_out->language_count = %d ", p_out->language_count);
		return FALSE;
	} else {
		return TRUE;
	}
}

/**
 * This function is used to encode EFLI (3G)
 */
char *tcore_sim_encode_li(int *out_length, struct tel_sim_language *p_in)
{
	int i = 0;
	char *tmp_out = NULL;
	const char *LanguageCode[] = {
		"de", "en", "it", "fr",
		"es", "nl", "sv", "da",
		"pt", "fi", "no", "el",
		"tr", "hu", "pl", "ko",
		"zh", "ru", "ja"};

	if (out_length == NULL || p_in == NULL) {
		dbg("out_length or p_in is null");
		return NULL;
	}

	tmp_out = (char *)malloc((p_in->language_count) * 2);
	if (!tmp_out)
		return NULL;

	memset((void *)tmp_out, 0xff, (p_in->language_count) * 2);

	for (i = 0; i < p_in->language_count; i++) {
		if (p_in->language[i] < SIM_LANG_UNSPECIFIED) {
			strncpy((char *) &tmp_out[i * 2], LanguageCode[p_in->language[i]], 2);
			dbg("sim_encode_li: p_out[%s]:[%x][%x]", tmp_out, tmp_out[i*2], tmp_out[(i*2)+1]);
		}
	}

	*out_length = i*2;
	return tmp_out;
}

gboolean tcore_sim_decode_imsi(struct tel_sim_imsi *p_out, unsigned char *p_in, int in_length)
{
	int i = 0, j = 0;
	char imsi_raw[16];
	char *plmn = NULL;
	int plmn_digits = 5;

	if ((NULL == p_out) || (NULL == p_in))
		return FALSE;

	/*
	 * According to 3GPP specification, the length of raw IMSI data is 9 bytes.
	 * first byte is length of IMSI
	 * second byte byte has parity nibble, so second byte has one digit of IMSI.
	 * other byte has two digit of IMSI
	 */
	if ((in_length == 0) || (in_length == 0xff) || (4 > in_length) || (9 < in_length)) {
		dbg("No valid IMSI present to convert - length:[%x]", in_length);
		return FALSE;
	}

	/* Decode IMSI value from nibbles */
	for (i = 1; i < in_length; i++) {
		if (i == 1) { /* first byte, ignore lower nibble */
			imsi_raw[j++] = ((p_in[i] & 0xF0) >> 4) + '0';
		} else if (i == p_in[0] + 1) { /* last byte */
			imsi_raw[j++] = (p_in[i] & 0x0F) + '0';
			if (p_in[0] % 2) /* count the last one if odd digits */
				imsi_raw[j++] = ((p_in[i] & 0xF0) >> 4) + '0';
		} else {
			imsi_raw[j++] = (p_in[i]  & 0x0F) + '0';
			imsi_raw[j++] = ((p_in[i] & 0xF0) >> 4) + '0';
		}
	}

	/* Determine # of PLMN digits (5 or 6) */
	plmn = g_strndup(imsi_raw, 6 + 1);
	if (plmn) {
		plmn[6] = '\0';
		if (tcore_sim_check_plmn_having_3digits_mnc(plmn))
			plmn_digits = 6;
		g_free(plmn);
	}

	/* Terminate string */
	imsi_raw[j] = '\0';
	memcpy(p_out->plmn, imsi_raw, plmn_digits);
	p_out->plmn[plmn_digits] = '\0';
	memcpy(p_out->msin, imsi_raw + plmn_digits, strlen(imsi_raw) - plmn_digits);
	p_out->msin[strlen(imsi_raw) - plmn_digits] = '\0';

	dbg("plmn in imsi = [%s]", p_out->plmn);

	return TRUE;
}

gboolean tcore_sim_decode_cdma_imsi(struct tel_sim_imsi *p_out, unsigned char *p_in, int in_length)
{
	char imsi_raw[16] = {0, };
	int digits = 0;
	unsigned short mcc;
	unsigned char mnc;
	unsigned long min1;
	unsigned short min2;
	char *plmn = NULL;
	int plmn_digits = 5;

	if ((NULL == p_out) || (NULL == p_in))
		return FALSE;

	/*
	 * According to 3GPP2 specification, the length of raw IMSI data is 10 bytes.
	 *	byte		Description
	 *-----------------------------------------------------------------
	 *	1		Class assignment of IMSI_M
	 *	2-3		IMSI_M_S2 : MSIN2
	 *	4-6		IMSI_M_S1 : MSIN1
	 *	7		IMSI_M_11_12 : MNC
	 *	8		IMSI_M_ADDR_NUM : No of IMSI_M address digits.
	 *	9-10		MCC_M : MCC
	 */
	if ((in_length == 0) || (in_length == 0xff) || (4 > in_length) || (10 < in_length)) {
		dbg("No valid IMSI present to convert - length:[%x]", in_length);
		return FALSE;
	}

	/* Decode IMSI value from nibbles */
	mcc = (p_in[9] << 8) | p_in[8];
	mnc = p_in[6];
	min1 = (p_in[5] << 16) | (p_in[4] << 8) | (p_in[3]);
	min2 = (p_in[2] << 8) | p_in[1];

	digits = _decode_cdma_imsi_util(imsi_raw, &mcc, &mnc, &min1, &min2);

	if (digits < 0)
		return FALSE;

	/* Determine # of PLMN digits (5 or 6) */
	plmn = g_strndup(imsi_raw, 6 + 1);
	if (plmn) {
		plmn[6] = '\0';
		if (tcore_sim_check_plmn_having_3digits_mnc(plmn))
			plmn_digits = 6;

		g_free(plmn);
	}

	/* Terminate string */
	imsi_raw[digits] = '\0';
	memcpy(p_out->plmn, imsi_raw, plmn_digits);
	p_out->plmn[plmn_digits] = '\0';
	memcpy(p_out->msin, imsi_raw + plmn_digits, strlen(imsi_raw) - plmn_digits);
	p_out->msin[strlen(imsi_raw) - plmn_digits] = '\0';

	dbg("plmn in imsi = [%s]", p_out->plmn);

	return TRUE;
}

gboolean tcore_sim_decode_sst(struct tel_sim_sst *p_sst, unsigned char *p_in, int in_length)
{
	unsigned char sstByte, rast, mask = 0;
	char simServiceID = 1;	/* set "CHV1 disable function" */
	int i, svc_count;
	char *p_index;

	memset((void *)p_sst, 0, sizeof(struct tel_sim_sst));

	if (in_length == 0) {
		err("invalid length. return FALSE.");
		return FALSE;
	}

	/* get count of SIM service id. one byte has four service status. */
	svc_count = in_length * 4;

	/*
	 * 3GPP 51.011 SST shows 56 kinds of service types.
	 * current tel_sim_sst has also 56 elements
	 */
	if (svc_count > SIM_SST_SERVICE_CNT_MAX) {
		warn("out of range[%d]. cut off the tail.", svc_count);
		svc_count = SIM_SST_SERVICE_CNT_MAX;
	}

	p_index = (char *)p_sst;

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

		default:
			warn("invalid rast");
		break;
		}

		if (sstByte & mask)
			*p_index = 1;
		else
			*p_index = 0;

		p_index += sizeof(char);
		simServiceID++;	/* next service id */
	}

	return TRUE;
}

gboolean tcore_sim_decode_cdma_st(struct tel_sim_cst *p_cdma_st, unsigned char *p_in, int in_length)
{
	unsigned char sstByte, rast, mask = 0;
	char simServiceID = 1;	/* set "CHV1 disable function" */
	int i, svc_count;
	char *p_index;

	memset((void *)p_cdma_st, 0, sizeof(struct tel_sim_cst));

	if (in_length == 0 || in_length > SIM_CDMA_ST_SERVICE_LEN_MAX)
		return FALSE;

	/* get count of SIM service id. one byte has four service status. */
	svc_count = in_length * 4;

	/*
	 * CDMA_ST service is described to 47(1 byte includes 4 service status) in C.S0023 3.4.18.
	 * Current tel_sim_cst.serivce.cdma_service has 47 services. so in_length should be under 12 byte.
	 */
	if (svc_count > SIM_CDMA_ST_SERVICE_CNT_MAX)
		svc_count = SIM_CDMA_ST_SERVICE_CNT_MAX;

	p_cdma_st->cdma_svc_table = SIM_CDMA_SVC_TABLE;

	p_index = (char *)p_cdma_st->service.cdma_service;

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

		default:
			warn("invalid rast");
		break;
		}

		if (sstByte & mask)
			*p_index = 1;
		else
			*p_index = 0;

		p_index += sizeof(char);
		simServiceID++;	/* next service id */
	}

	return TRUE;
}

gboolean tcore_sim_decode_csim_st(struct tel_sim_cst *p_csim_st, unsigned char *p_in, int in_length)
{
	int i, j;
	char mask;
	char *p_index;
	memset((void *) p_csim_st, 0, sizeof(struct tel_sim_cst));

	p_csim_st->cdma_svc_table = SIM_CSIM_SVC_TABLE;
	p_index = (char *)p_csim_st->service.csim_service;

	/*
	 * CSIM_ST service is described to 41(1 byte includes 8 service status) in C.S0065 5.2.18.
	 * Current tel_sim_cst.serivce.csim_service has 41 services. so in_length should be under 6 byte.
	 */
	if (in_length > SIM_CSIM_ST_SERVICE_LEN_MAX)
		in_length = SIM_CSIM_ST_SERVICE_LEN_MAX;

	for (i = 0; i < in_length; i++) {
		mask = 0x01;	/* reset mask to check first bit */

		for (j = 0; j < 8; j++) {
			if (p_in[i] & mask)
				*p_index = 1;

			p_index += sizeof(char);
			mask = mask << 1;
		}
	}

	return TRUE;
}

gboolean tcore_sim_decode_spn(struct tel_sim_spn *p_spn, unsigned char *p_in, int in_length)
{
	int i;

	if (in_length == 0)
		return FALSE;

	p_spn->display_condition = p_in[0] & 0x3;

	for (i = 1; i < SIM_SPN_LEN_MAX + 1; i++) {
		if (p_in[i] == 0xFF)
			break; /* loop break*/

		p_spn->spn[i - 1] = p_in[i];
	}

	p_spn->spn[i-1] = '\0';

	dbg("spn:[%s] display condition : [%d]", p_spn->spn, p_spn->display_condition);

	return TRUE;
}

gboolean tcore_sim_decode_cdma_spn(struct tel_sim_spn *p_spn, unsigned char *p_in, int in_length)
{
	int i = 0;

	if (in_length == 0)
		return FALSE;

	p_spn->display_condition = p_in[0] & 0x1;

	/*
	 * Note : Character Encoding (1 byte) and Language Indicator (1 byte)
	 * are ignored, will be added later if required by Application
	 */
	for (i = 3; i < SIM_CDMA_SPN_LEN_MAX + 1; i++) {
		if (p_in[i] == 0xFF)
			break; /* loop break*/

		p_spn->spn[i - 3] = p_in[i];
	}
	p_spn->spn[i - 3] = '\0';

	dbg("spn:[%s] display condition : [%d]", p_spn->spn, p_spn->display_condition);

	return TRUE;
}

gboolean tcore_sim_decode_spdi(struct tel_sim_spdi *p_spdi, unsigned char *p_in, int in_length)
{
	int i, Src_plmn_start_len, total_data_len;

	if (in_length == 0)
		return FALSE;

	if (p_in[0] == 0xff) {
		dbg("file is exist but there is no valid records");

		p_spdi->plmn_count = 0;
		memset(p_spdi->list, 0x00, sizeof(unsigned char)*7*SIM_SPDI_PLMN_MAX);

		return TRUE;
	}

	/* Display info tag('A3') */
	if (p_in[0] == 0xA3) {
		total_data_len = p_in[1];
		dbg("total_data_len=[%d]", total_data_len);

		 /* PLMN list tag('80') */
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

			dbg("p_spdi->num_of_plmn_entries[%d]", p_spdi->plmn_count);

			for (i = 0; i < p_spdi->plmn_count; i++) {
				_decode_plmn(&p_in[Src_plmn_start_len], p_spdi->list[i].plmn);
				dbg("SPDI PLMN[%d][%s]", i, p_spdi->list[i].plmn);

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
	int X = 0;	/* alpha id max length */
	int alpha_id_length = 0;
	int value_length = 0;
	int bcd_byte = 0;	/* dialing number max length */

	memset((void *) p_msisdn, 0, sizeof(struct tel_sim_msisdn));

	if (in_length < 14) {
		err("invalid in_length[%d]", in_length);
		return FALSE;
	}

	if (_is_empty(p_in, in_length) == TRUE) {
		memset(p_msisdn, 0, sizeof(struct tel_sim_msisdn));
		return FALSE;
	}

	X = in_length - 14;	/* get alpha id max length */

	if (X != 0) {
		alpha_id_length = X;
		dbg("alpha_id_length[%d]", alpha_id_length);
		if (alpha_id_length > SIM_XDN_ALPHA_ID_LEN_MAX)
			alpha_id_length = SIM_XDN_ALPHA_ID_LEN_MAX;

		value_length =  _get_string((unsigned char *)p_msisdn->name, p_in, alpha_id_length);
		p_msisdn->name[value_length] = '\0';
	}

	/* get dialing number length */
	/* p_in[X] is BCD length of dialing number length plus TON/NPI 1 bytes. */
	/* Convert to digit length and subtract TON/NPI length. */
	if (p_in[X] != 0xFF) {
		dbg("Dialing number Length %d, BCD length 0x%x ",  (p_in[X] - 1) * 2, p_in[X]);

		/* get TON and NPI */
		p_msisdn->ton = (p_in[X + 1] >> 4) & 0x07;

		/* get actual dialing number length */
		bcd_byte = _get_valid_bcd_byte(&p_in[X + 2], SIM_XDN_NUMBER_LEN_MAX / 2);
		dbg("bcd_byte[%x]", bcd_byte);

		/* get dialing number/SSC string */
		value_length = _bcd_to_digit((char *)p_msisdn->num, (char *) &p_in[X + 2], bcd_byte); /* actual dialing number length in BCD. */
		p_msisdn->num[value_length] = '\0';
		p_msisdn->next_record = p_in[X+13];
		dbg("p_msisdn->num[%s]", p_msisdn->num);
	}

	return TRUE;
}

gboolean tcore_sim_decode_mdn(struct tel_sim_msisdn *p_msisdn, unsigned char *p_in, int in_length)
{
	int value_length = 0;
	int bcd_byte = 0;	/* dialing number max length */

	memset((void *)p_msisdn, 0, sizeof(struct tel_sim_msisdn));

	if (in_length == 0)
		return FALSE;

	if (_is_empty(p_in, in_length) == TRUE)
		return FALSE;

	/* Note : Alpha identifier is not present in EF-MDN file. */
	if (p_in[0] != 0xFF) {
		dbg("Dialing number Length %d, BCD length 0x%x ",  (p_in[0] - 1) * 2, p_in[0]);

		/* get TON and NPI */
		p_msisdn->ton = (p_in[9] >> 4) & 0x07;

		/* get actual dialing number length */
		bcd_byte = _get_valid_bcd_byte(&p_in[1], 8);
		dbg("bcd_byte[%x]", bcd_byte);

		/* get dialing number/SSC string */
		value_length = _bcd_to_digit((char *) p_msisdn->num, (char *) &p_in[1], bcd_byte); /* actual dialing number length in BCD. */
		p_msisdn->num[value_length] = '\0';

		/*p_msisdn->next_record = p_in[];*/ /* Need to check with next_record field */
		dbg("p_msisdn->num[%s]", p_msisdn->num);
	}

	return TRUE;
}

gboolean tcore_sim_decode_xdn(struct tel_sim_dialing_number *p_xdn, unsigned char *p_in, int in_length)
{
	int X;	/* alpha id max length */
	int bcd_byte;	/* dialing number max length */

	memset((void *)p_xdn, 0, sizeof(struct tel_sim_dialing_number));

	/* Min Length 14 is Spec Defined File ID MBDN -6FC7 & CPHS MAILBOX 6F17 */
	if (in_length == 0 || in_length < 14)
		return FALSE;

	if (_is_empty(p_in, in_length) == TRUE)
		return FALSE;	/* this is empty record */

	X = in_length - 14; /* get alpha id max length */

	if (X > 0) {
		p_xdn->alpha_id_max_len = X > SIM_XDN_ALPHA_ID_LEN_MAX ? SIM_XDN_ALPHA_ID_LEN_MAX : X;
		_get_string((unsigned char *)p_xdn->alpha_id, p_in, p_xdn->alpha_id_max_len);
	}

	/* get dialing number length */
	/* p_in[X] is BCD length of dialing number length plus TON/NPI 1 bytes. */
	/* Convert to digit length and subtract TON/NPI length. */
	if (p_in[X] != 0xFF) {
		dbg("Dialing number Length %d, BCD length 0x%x ",
			(p_in[X] - 1) * 2, p_in[X]);

		/*
		if (p_xdn->num_max_len > SIM_XDN_NUMBER_LEN_MAX) {

			 this may be broken record.
			 p_xdn->b_used = FALSE;
			 memset((void *)p_xdn, 0, sizeof(tapi_sim_dialing_number_info_t));
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
		}
		*/

		/* get TON and NPI */
		p_xdn->ton = (p_in[X + 1] >> 4) & 0x07;
		p_xdn->npi = p_in[X + 1] & 0x0F;

		/* get actual dialing number length */
		bcd_byte = _get_valid_bcd_byte(&p_in[X + 2], SIM_XDN_NUMBER_LEN_MAX / 2);
		dbg("bcd_byte[%x]", bcd_byte);

		/* get dialing number/SSC string */
		_bcd_to_digit((char *) p_xdn->num, (char *) &p_in[X + 2], bcd_byte); /* actual dialing number length in BCD. */
		dbg("p_xdn->DiallingNum[%s]", p_xdn->num);

		/* get Capability/Configuration id */
		p_xdn->cc_id = p_in[X + 12];

		/* get Extension1 id */
		p_xdn->ext1_id = p_in[X + 13];
	}
	return TRUE;
}

char *tcore_sim_encode_xdn(int in_length, struct tel_sim_dialing_number *p_xdn)
{
	int alpha_id_space = 0, digit_len = 0, str_len = 0;
	char bcdCode[SIM_XDN_NUMBER_LEN_MAX / 2];
	char *p_out = NULL;

	if (in_length < 14) {
		dbg("in_length[%d] should be greater than or equal to 14.", in_length);
		return NULL;
	}

	p_out = calloc(1, in_length + 1);
	if (!p_out)
		return NULL;

	memset((void *)p_out, 0xFF, in_length);

	/* get alpha id max length */
	alpha_id_space = in_length - 14;

	/* alpha id is too big */
	str_len = strlen(p_xdn->alpha_id);
	if (alpha_id_space < str_len) {
		dbg("SIM space for alpha_id is [%d] but input alpha_id length is [%d]. so we will use [%d] byte",
				alpha_id_space, str_len, alpha_id_space);
		str_len = alpha_id_space;
	}

	digit_len = strlen(p_xdn->num);
	/* this is digit length */
	if (digit_len > SIM_XDN_NUMBER_LEN_MAX) {
		dbg("SIM space for number is [%d] but input number length is [%d]. so we will use [%d] byte",
			SIM_XDN_NUMBER_LEN_MAX, digit_len, SIM_XDN_NUMBER_LEN_MAX);
		digit_len = SIM_XDN_NUMBER_LEN_MAX;
	}

	_set_string((unsigned char *)p_out, (unsigned char *)p_xdn->alpha_id, str_len);

	/* set length of BCD number/SSC contents */
	/* p_xdn->diallingnumLen is maximum digit length. = 20 bytes. */
	/* convert to BCD length and add 1 byte. */
	p_out[alpha_id_space] = ((digit_len + 1) / 2) + 1;

	/* set TON and NPI */
	p_out[alpha_id_space + 1] = 0x80;
	p_out[alpha_id_space + 1] |= (p_xdn->ton & 0x07) << 4;
	p_out[alpha_id_space + 1] |= p_xdn->npi & 0x0F;

	/* set dialing number/SSC string */
	memset((void *) bcdCode, 0xFF, SIM_XDN_NUMBER_LEN_MAX / 2);

	_digit_to_bcd((char *)bcdCode, (char *)p_xdn->num, digit_len);

	memcpy((void *)&p_out[alpha_id_space + 2], bcdCode, SIM_XDN_NUMBER_LEN_MAX / 2);

	/* set Capability/Configuration Identifier */
	if (p_xdn->cc_id == 0x00)
		p_out[alpha_id_space + 12] = 0xff;
	else
		p_out[alpha_id_space + 12] = (unsigned char)p_xdn->cc_id;

	/* set extension1 record Identifier */
	if (p_xdn->ext1_id == 0x00)
		p_out[alpha_id_space + 13] = 0xff;
	else
		p_out[alpha_id_space + 13] = (unsigned char)p_xdn->ext1_id;

	return p_out;
}

gboolean tcore_sim_decode_ecc(struct tel_sim_ecc_list *p_ecc, unsigned char *p_in, int in_length)
{
	int bcd_byte;	/* dialing number max length */
	int i;
	int valid_ecc_length;

	memset((void *)p_ecc, 0x00, sizeof(struct tel_sim_ecc_list));

	if (in_length % 3 != 0) {
		dbg("error - invalid data length");
		return FALSE;
	}

	for (i = 0; i < in_length / 3; i++) {
		/* get the BCD length of the ECC */
		bcd_byte = _get_valid_bcd_byte((unsigned char *) p_in+(i*3), 3);
		if (bcd_byte != 0) {
			valid_ecc_length = _bcd_to_digit(p_ecc->ecc[p_ecc->ecc_count].ecc_num, (char *)p_in+(i*3), bcd_byte);
			p_ecc->ecc[p_ecc->ecc_count].ecc_num[valid_ecc_length] = '\0';
			p_ecc->ecc_count++;
		}
	}

	return TRUE;
}

gboolean tcore_sim_decode_ext(struct tel_sim_ext *p_ext, unsigned char *p_in, int in_length)
{
	int bcd_byte;	/* dialing number max length */
	gboolean res = FALSE;

	memset((void *)p_ext, 0x00, sizeof(struct tel_sim_ext));

	if (*p_in & 0x01) {
		dbg("Record type - Called Party Subaddress - NOT SUPPORTED");
	} else if (*p_in & 0x02) {
		dbg("Record type - Additional data");
		bcd_byte = _get_valid_bcd_byte(&p_in[2], SIM_XDN_NUMBER_LEN_MAX / 2);
		p_ext->ext_len = _bcd_to_digit((char *) p_ext->ext, (char *) &p_in[2], bcd_byte); /* actual dialing number length in BCD. */
		p_ext->next_record = p_in[12];
		dbg("Dialing number Length[%d]", p_ext->ext_len);
		res = TRUE;
	} else {
		dbg("Record type - Invalid");
	}

	return res;
}

gboolean tcore_sim_decode_ust(struct tel_sim_ust *p_ust, unsigned char *p_in, int in_length)
{
	int i, j;
	char mask;
	char *p_index;

	memset((void *)p_ust, 0, sizeof(struct tel_sim_ust));
	p_index = (char *)p_ust;

	if (in_length == 0) {
		err("invalid length. return FALSE.");
		return FALSE;
	}

	/*
	 * UST service is described to 74(1 byte includes 8 service status) in 31.102 r7.
	 * current sim_ust_s has 64 services. so in_length should be under 8 byte.
	 */
	if (in_length > (SIM_UST_SERVICE_CNT_MAX / 8)) {
		warn("out of range[%d]. cut off the tail.", in_length);
		in_length = SIM_SST_SERVICE_CNT_MAX / 8;
	}

	for (i = 0; i < in_length; i++) {
		mask = 0x01;	/* reset mast to check first bit */

		for (j = 0; j < 8; j++) {
			if (p_in[i] & mask)
				*p_index = 1;

			p_index += sizeof(char);
			mask = mask << 1;
		}
	}

	return TRUE;
}

gboolean tcore_sim_decode_ist(struct tel_sim_ist *p_ist, unsigned char *p_in, int in_length)
{
	int i, j;
	char mask;
	char *p_index;

	memset((void *)p_ist, 0, sizeof(struct tel_sim_ist));
	p_index = (char *)p_ist;

	/*
	 * IST service is described to 11(1 byte includes 8 service status) in 31.103 release12.
	 * current tel_sim_ist_service has 11 services. so in_length should be under 2 byte.
	 */
	if (in_length > SIM_IST_BYTE_LEN_MAX)
		in_length = SIM_IST_BYTE_LEN_MAX;

	for (i = 0; i < in_length; i++) {
		mask = 0x01; /* reset mast to check first bit */

		for (j = 0; j < 8; j++) {
			if (p_in[i] & mask)
				*p_index = 1;

			p_index += sizeof(char);
			mask = mask << 1;
		}
	}

	return TRUE;
}

gboolean tcore_sim_decode_est(struct tel_sim_est *p_est, unsigned char *p_in, int in_length)
{
	memset((void *)p_est, 0, sizeof(struct tel_sim_est));

	if (*p_in & 0x01)
		p_est->bFdnEnabled = TRUE;

	if (*p_in & 0x02)
		p_est->bBdnEnabled = TRUE;

	if (*p_in & 0x04)
		p_est->bAclEnabled = TRUE;

	return TRUE;
}

gboolean tcore_sim_decode_uecc(struct tel_sim_ecc *p_ecc, unsigned char *p_in, int in_length)
{
	int bcd_byte;	/* dialing number max length */
	unsigned char eccServiceCategory;

	if (_is_empty(p_in, in_length) == TRUE) {
		memset(p_ecc, 0, sizeof(struct tel_sim_ecc));
		return FALSE;
	}

	/* get the BCD length of the ECC */
	bcd_byte = _get_valid_bcd_byte(&p_in[0], SIM_ECC_BYTE_LEN_MAX);

	/* get the ECC codes in digits and the length as well */
	_bcd_to_digit((char *) p_ecc->ecc_num, (char *)&p_in[0], bcd_byte);

	/* get the alpha identifier of ECC */
	_get_string((unsigned char *)p_ecc->ecc_string, (unsigned char *)&p_in[3],
		(in_length - 3 > SIM_ECC_STRING_LEN_MAX) ? SIM_ECC_STRING_LEN_MAX : in_length - 3);

	eccServiceCategory = p_in[in_length - 1];

	/*
	 * Assign the service category
	 * 3GPP TS24.008 Emergency Service Category Value.
	 * Bit 8, 7, 6 are spare, 5~1 bit is used.
	 * The meaning of the Emergency Category Value is derived from the following settings
	 * (see 3GPP TS 22.101 clause 10):
	 * Bit 1	Police 0x01
	 * Bit 2	Ambulance 0x02
	 * Bit 3	Fire Brigade 0x04
	 * Bit 4	Marine Guard 0x08
	 * Bit 5	Mountain Rescue 0x10
	 * Bit 6	manually initiated eCall
	 * Bit 7	automatically initiated eCall
	 * Bit 8	is spare and set to "0"
	 */

	if (eccServiceCategory == 0xFF) /* if category vaule is unused (unchecked) then just return 0xff */
		p_ecc->ecc_category = eccServiceCategory;
	else
		p_ecc->ecc_category = eccServiceCategory & 0x1F; /* Check for the first 5 bits */

	return TRUE;
}

gboolean tcore_sim_decode_gid(struct tel_sim_gid *p_gid, unsigned char *p_in, int in_length)
{
	int i;

	memset((void *) p_gid, 0, sizeof(struct tel_sim_gid));

	if (in_length == 0)
		return FALSE;

	/* regarding 31.102, EF-GID data byte is not defined. currently 10. */
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

	if (_is_empty(p_in, in_length) == TRUE)
		return FALSE; /* this is empty record */

	p_mbi->voice_index = p_in[0];
	p_mbi->fax_index = p_in[1];
	p_mbi->email_index = p_in[2];
	p_mbi->other_index = p_in[3];

	/* 5th byte is optional */
	if (in_length == 5)
		p_mbi->video_index = p_in[4];

	return TRUE;
}

char *tcore_sim_encode_mbi(const struct tel_sim_mbi *p_mbi, int in_length)
{
	char *p_out = NULL;

	if (in_length < 4) {
		dbg("in_length[%d] should be greater than or equal to 4.", in_length);
		return NULL;
	}

	p_out = calloc(1, in_length);
	if (!p_out)
		return NULL;

	p_out[0] = p_mbi->voice_index;
	p_out[1] = p_mbi->fax_index;
	p_out[2] = p_mbi->email_index;
	p_out[3] = p_mbi->other_index;

	if (in_length == 5)
		p_out[4] = p_mbi->video_index;

	return p_out;
}

gboolean tcore_sim_decode_cff(struct tel_sim_cphs_cf *p_cff, unsigned char *p_in, int in_length)
{
	if (in_length == 0)
		return FALSE;

	dbg("flag(0)=%x, packetlen=%d ", (unsigned int)p_in[0], in_length);
	dbg("flag(1)=%x", p_in[1]);

	if ((p_in[0] & 0x0F) == 0x0A)
		p_cff->b_line1 = TRUE;

	if ((p_in[0] & 0xF0) == 0xA0)
		p_cff->b_line2 = TRUE;

	if (in_length > 1) {
		if ((p_in[1] & 0x0F) == 0x0A)
			p_cff->b_fax = TRUE;

		if ((p_in[1] & 0xF0) == 0xA0)
			p_cff->b_data = TRUE;
	}

	dbg("Line1 = %d, line2 = %d, Fax = %d, Data = %d ",
			p_cff->b_line1,
			p_cff->b_line2,
			p_cff->b_fax,
			p_cff->b_data);
	return TRUE;
}

char *tcore_sim_encode_cff(const struct tel_sim_cphs_cf *cff, int in_length)
{
	int i, j = 0;
	char *p_out = NULL;
	unsigned char *pTemp = (unsigned char *) cff;
	unsigned char present = 0x0A;
	unsigned char absent = 0x05;

	if (in_length < SIM_CPHS_CALL_FORWARDING_LEN_MIN) {
		err("in_length[%d] is smaller than SIM_CPHS_CALL_FORWARDING_LEN_MIN[%d]",
			in_length, SIM_CPHS_CALL_FORWARDING_LEN_MIN);
		return NULL;
	}

	p_out =  calloc(1, SIM_CPHS_CALL_FORWARDING_LEN_MIN+1);
	if (!p_out)
		return NULL;

	for (i = 0; i < SIM_CPHS_CALL_FORWARDING_LEN_MIN; i++) {
		present = 0x0A;
		absent = 0x05;

		for (j = 0; j < 2; j++) {
			if (*pTemp)
				p_out[i] = p_out[i] | present;
			else
				p_out[i] = p_out[i] | absent;

			pTemp += sizeof(gboolean);
			present = present << 4;
			absent = absent << 4;
		}
	}

	p_out[SIM_CPHS_CALL_FORWARDING_LEN_MIN] = '\0';
	return p_out;
}

gboolean tcore_sim_decode_csp(struct tel_sim_cphs_csp *p_csp, unsigned char *p_in, int in_length)
{
	int i, j, k = 0;
	unsigned char byteSignificance = 0x00;
	unsigned char mask = 0x80;

	if (in_length == 0)
		return FALSE;

	memset((void *) p_csp, 0, sizeof(struct tel_sim_cphs_csp));

	/* current telephony supports 22 byte cphs-csp data. 18 byte is mandatory, the other is optional. */
	for (i = 0, j = 0; i < SIM_CPHS_CSP_LEN_MAX && j < SIM_CPHS_CSP_ENTRY_CNT_MAX; i++, j++) {
		p_csp->service_profile_entry[j].customer_service_group = (enum tel_sim_cphs_csp_group) p_in[i];
		byteSignificance = p_in[++i];
		mask = 0x80;

		switch (p_csp->service_profile_entry[j].customer_service_group) {
		case 0x01:
			for (k = 0; k < 5; k++) {
				switch (byteSignificance & mask) {
				case 0x80:
					p_csp->service_profile_entry[j].service.call_offering.b_call_forwarding_unconditional = TRUE;
				break;

				case 0x40:
					p_csp->service_profile_entry[j].service.call_offering.b_call_forwarding_on_user_busy = TRUE;
				break;

				case 0x20:
					p_csp->service_profile_entry[j].service.call_offering.b_call_forwarding_on_no_reply = TRUE;
				break;

				case 0x10:
					p_csp->service_profile_entry[j].service.call_offering.b_call_forwarding_on_user_not_reachable = TRUE;
				break;

				case 0x08:
					p_csp->service_profile_entry[j].service.call_offering.b_call_transfer = TRUE;
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
					p_csp->service_profile_entry[j].service.call_restriction.b_barring_of_all_outgoing_calls = TRUE;
				break;

				case 0x40:
					p_csp->service_profile_entry[j].service.call_restriction.b_barring_of_outgoing_international_calls = TRUE;
				break;

				case 0x20:
					p_csp->service_profile_entry[j].service.call_restriction.b_barring_of_outgoing_international_calls_except_hplmn = TRUE;
				break;

				case 0x10:
					p_csp->service_profile_entry[j].service.call_restriction.b_barring_of_all_incoming_calls_roaming_outside_hplmn = TRUE;
				break;

				case 0x08:
					p_csp->service_profile_entry[j].service.call_restriction.b_barring_of_incoming_calls_when_roaming = TRUE;
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
					p_csp->service_profile_entry[j].service.other_supp_services.b_multi_party_service = TRUE;
				break;

				case 0x40:
					p_csp->service_profile_entry[j].service.other_supp_services.b_closed_user_group = TRUE;
				break;

				case 0x20:
					p_csp->service_profile_entry[j].service.other_supp_services.b_advice_of_charge = TRUE;
				break;

				case 0x10:
					p_csp->service_profile_entry[j].service.other_supp_services.b_preferential_closed_user_group = TRUE;
				break;

				case 0x08:
					p_csp->service_profile_entry[j].service.other_supp_services.b_closed_user_group_outgoing_access = TRUE;
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
					p_csp->service_profile_entry[j].service.call_complete.b_call_hold = TRUE;
				break;

				case 0x40:
					p_csp->service_profile_entry[j].service.call_complete.b_call_waiting = TRUE;
				break;

				case 0x20:
					p_csp->service_profile_entry[j].service.call_complete.b_completion_of_call_to_busy_subscriber = TRUE;
				break;

				case 0x10:
					p_csp->service_profile_entry[j].service.call_complete.b_user_user_signalling = TRUE;
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
					p_csp->service_profile_entry[j].service.teleservices.b_short_message_mobile_terminated = TRUE;
				break;

				case 0x40:
					p_csp->service_profile_entry[j].service.teleservices.b_short_message_mobile_originated = TRUE;
				break;

				case 0x20:
					p_csp->service_profile_entry[j].service.teleservices.b_short_message_cell_broadcast = TRUE;
				break;

				case 0x10:
					p_csp->service_profile_entry[j].service.teleservices.b_short_message_reply_path = TRUE;
				break;

				case 0x08:
					p_csp->service_profile_entry[j].service.teleservices.b_short_message_delivery_conf = TRUE;
				break;

				case 0x04:
					p_csp->service_profile_entry[j].service.teleservices.b_short_message_protocol_identifier = TRUE;
				break;

				case 0x02:
					p_csp->service_profile_entry[j].service.teleservices.b_short_message_validity_period = TRUE;
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
					p_csp->service_profile_entry[j].service.cphs_teleservices.b_alternative_line_service =  TRUE;
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
					p_csp->service_profile_entry[j].service.cphs_features.b_string_service_table = TRUE;
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
					p_csp->service_profile_entry[j].service.number_identifiers.b_calling_line_identification_present = TRUE;
				break;

				case 0x20:
					p_csp->service_profile_entry[j].service.number_identifiers.b_connected_line_identification_restrict = TRUE;
				break;

				case 0x10:
					p_csp->service_profile_entry[j].service.number_identifiers.b_connected_line_identification_present = TRUE;
				break;

				case 0x08:
					p_csp->service_profile_entry[j].service.number_identifiers.b_malicious_call_identifier = TRUE;
				break;

				case 0x02:
					p_csp->service_profile_entry[j].service.number_identifiers.b_calling_line_identification_send = TRUE;
				break;

				case 0x01:
					p_csp->service_profile_entry[j].service.number_identifiers.b_calling_line_identification_block = TRUE;
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
					p_csp->service_profile_entry[j].service.phase_services.b_menu_for_gprs = TRUE;
				break;

				case 0x40:
					p_csp->service_profile_entry[j].service.phase_services.b_menu_for_high_speed_csd = TRUE;
				break;

				case 0x20:
					p_csp->service_profile_entry[j].service.phase_services.b_menu_for_voice_group_call = TRUE;
				break;

				case 0x10:
					p_csp->service_profile_entry[j].service.phase_services.b_menu_for_voice_broadcast_service = TRUE;
				break;

				case 0x08:
					p_csp->service_profile_entry[j].service.phase_services.b_menu_for_multiple_subscriber_profile = TRUE;
				break;

				case 0x04:
					p_csp->service_profile_entry[j].service.phase_services.b_menu_for_multiple_band = TRUE;
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
					p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_manual_selection = TRUE;
				break;

				case 0x40:
					p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_voice_mail = TRUE;
				break;

				case 0x20:
					p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_mo_sms_and_paging = TRUE;
				break;

				case 0x10:
					p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_mo_sms_with_emial_type = TRUE;
				break;

				case 0x08:
					p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_fax_calls = TRUE;
				break;

				case 0x04:
					p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_data_calls = TRUE;
				break;

				case 0x01:
					p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_change_language = TRUE;
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
					p_csp->service_profile_entry[j].service.information_numbers.b_information_numbers = TRUE;
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

	memset((void *) p_out, 0xFF, out_length);

	/*
	 * current telephony supports 22 byte cphs-csp data.
	 * 18 byte is mandatory, the other is optional.
	 */
	for (i = 0, j = 0;
			i < SIM_CPHS_CSP_LEN_MAX && j < SIM_CPHS_CSP_ENTRY_CNT_MAX;
			i++, j++) {
		p_out[i] = (unsigned char) p_csp->service_profile_entry[j].customer_service_group;
		switch (p_out[i]) {
		case 0x01:
			p_out[++i] = (((unsigned char) p_csp->service_profile_entry[j].service.call_offering.b_call_forwarding_unconditional) << 7)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.call_offering.b_call_forwarding_on_user_busy) << 6)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.call_offering.b_call_forwarding_on_no_reply) << 5)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.call_offering.b_call_forwarding_on_user_not_reachable) << 4)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.call_offering.b_call_transfer) << 3);
		break;

		case 0x02:
			p_out[++i] = (((unsigned char) p_csp->service_profile_entry[j].service.call_restriction.b_barring_of_all_outgoing_calls) << 7)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.call_restriction.b_barring_of_outgoing_international_calls) << 6)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.call_restriction.b_barring_of_outgoing_international_calls_except_hplmn) << 5)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.call_restriction.b_barring_of_all_incoming_calls_roaming_outside_hplmn) << 4)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.call_restriction.b_barring_of_incoming_calls_when_roaming) << 3);
		break;

		case 0x03:
			p_out[++i] = (((unsigned char) p_csp->service_profile_entry[j].service.other_supp_services.b_multi_party_service) << 7)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.other_supp_services.b_closed_user_group) << 6)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.other_supp_services.b_advice_of_charge) << 5)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.other_supp_services.b_preferential_closed_user_group) << 4)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.other_supp_services.b_closed_user_group_outgoing_access) << 3);
		break;

		case 0x04:
			p_out[++i] = (((unsigned char) p_csp->service_profile_entry[j].service.call_complete.b_call_hold) << 7)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.call_complete.b_call_waiting) << 6)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.call_complete.b_completion_of_call_to_busy_subscriber) << 5)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.call_complete.b_user_user_signalling) << 4);
		break;

		case 0x05:
			p_out[++i] = (((unsigned char) p_csp->service_profile_entry[j].service.teleservices.b_short_message_mobile_terminated) << 7)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.teleservices.b_short_message_mobile_originated) << 6)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.teleservices.b_short_message_cell_broadcast) << 5)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.teleservices.b_short_message_reply_path) << 4)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.teleservices.b_short_message_delivery_conf) << 3)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.teleservices.b_short_message_protocol_identifier) << 2)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.teleservices.b_short_message_validity_period) << 1);
		break;

		case 0x06:
			p_out[++i] = (((unsigned char) p_csp->service_profile_entry[j].service.cphs_teleservices.b_alternative_line_service) << 7);
		break;

		case 0x07:
			p_out[++i] = (((unsigned char) p_csp->service_profile_entry[j].service.cphs_features.b_string_service_table) << 7);
		break;

		case 0x08:
			p_out[++i] = (((unsigned char) p_csp->service_profile_entry[j].service.number_identifiers.b_calling_line_identification_present) << 7)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.number_identifiers.b_connected_line_identification_restrict) << 5)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.number_identifiers.b_connected_line_identification_present) << 4)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.number_identifiers.b_malicious_call_identifier) << 3)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.number_identifiers.b_calling_line_identification_send) << 1)
					+ ((unsigned char) p_csp->service_profile_entry[j].service.number_identifiers.b_calling_line_identification_block);
		break;

		case 0x09:
			p_out[++i] = (((unsigned char) p_csp->service_profile_entry[j].service.phase_services.b_menu_for_gprs) << 7)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.phase_services.b_menu_for_high_speed_csd) << 6)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.phase_services.b_menu_for_voice_group_call) << 5)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.phase_services.b_menu_for_voice_broadcast_service) << 4)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.phase_services.b_menu_for_multiple_subscriber_profile) << 3)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.phase_services.b_menu_for_multiple_band) << 2);
		break;

		case 0xC0:
			p_out[++i] = (((unsigned char) p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_manual_selection) << 7)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_voice_mail) << 6)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_mo_sms_and_paging) << 5)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_mo_sms_with_emial_type) << 4)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_fax_calls) << 3)
					+ (((unsigned char) p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_data_calls) << 2)
					+ ((unsigned char) p_csp->service_profile_entry[j].service.value_added_services.b_restrict_menu_for_change_language);
		break;

		case 0xD5:
			if (p_csp->service_profile_entry[j].service.information_numbers.b_information_numbers)
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

	memset((void *) pMwis, 0, sizeof(struct tel_sim_mw));

	type = p_in[0];  /* 0x07 */

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

char *tcore_sim_encode_mwis(int *out_length, const struct tel_sim_mw *pMwis, int in_length)
{
	char *p_out = NULL;
	int i = 0;
	int encoded_size = 0;

	if (out_length == 0)
		return NULL;

	/*
	 * by 3GPP spec (31.102),
	 * EF-MWIS record length should be <= 6. (5 or 6)
	 */
	if (in_length > 6)
		encoded_size = 6;
	else
		encoded_size = in_length;

	p_out = calloc(1, encoded_size);
	if (!p_out)
		return NULL;

	for (i = 0; i < encoded_size; i++) {
		switch (i) {
		case 0:
			p_out[0] = (unsigned char) pMwis->indicator_status;
		break;

		case 1:
			p_out[1] = pMwis->voice_count;
		break;

		case 2:
			p_out[2] = pMwis->fax_count;
		break;

		case 3:
			p_out[3] = pMwis->email_count;
		break;

		case 4:
			p_out[4] = pMwis->other_count;
		break;

		case 5:
			p_out[5] = pMwis->video_count;
		break;

		default:
		break;
		}
	}

	*out_length = encoded_size;
	return p_out;
}

gboolean tcore_sim_decode_vmwf(struct tel_sim_cphs_mw *p_vmwf, unsigned char *p_in, unsigned long in_length)
{
	int i, j = 0;
	unsigned char *pTemp = (unsigned char *) p_vmwf;
	unsigned char mask = 0x0F;
	unsigned char voiceMsgFlg = 0;

	if (in_length == 0) {
		dbg("fail - input length is zero");
		return FALSE;
	}

	/* current telephony supports 2 byte cphs-vmwf data */
	for (i = 0; i < SIM_CPHS_VMWF_LEN_MAX; i++) {
		voiceMsgFlg = p_in[i];
		for (j = 0; j < 2; j++) {
			if ((voiceMsgFlg & mask) == 0x0A)
				*pTemp = 1;  /* TRUE */
			else if ((voiceMsgFlg & mask) == 0x05)
				*pTemp = 0;  /* FALSE */

			pTemp += sizeof(gboolean);
			voiceMsgFlg = voiceMsgFlg >> 4;
		}
	}
	return TRUE;
}

char *tcore_sim_encode_vmwf(int *out_length, const struct tel_sim_cphs_mw *p_vmwf, int in_length)
{
	int i, j = 0;
	char *p_out = NULL;
	unsigned char *pTemp = (unsigned char *) p_vmwf;
	unsigned char present = 0x0A;
	unsigned char absent = 0x05;

	if (out_length == 0)
		return NULL;

	p_out = calloc(1, in_length);
	if (!p_out)
		return NULL;

	for (i = 0; i < in_length; i++) {
		present = 0x0A;
		absent = 0x05;

		p_out[i] = 0x00;

		for (j = 0; j < 2; j++) {
			if (*pTemp)
				p_out[i] = p_out[i] | present;  /* TRUE */
			else
				p_out[i] = p_out[i] | absent;  /* TRUE */

			pTemp += sizeof(gboolean);
			present = present << 4;
			absent = absent << 4;
		}
	}

	*out_length = in_length;
	return p_out;
}

gboolean tcore_sim_decode_ons(unsigned char *p_out, unsigned  char *p_in, int in_length)
{
	int length;
	memset((void *) p_out, 0, SIM_CPHS_OPERATOR_NAME_LEN_MAX+1);

	if (in_length == 0)
		return FALSE;

	if (_is_empty(p_in, in_length) == TRUE)
		return FALSE;

	/* current telephony supports 25 byte cphs-operator name string. */
	if (in_length >= SIM_CPHS_OPERATOR_NAME_LEN_MAX)
		in_length = SIM_CPHS_OPERATOR_NAME_LEN_MAX;

	length = _get_string(p_out, p_in, in_length);
	p_out[length] = '\0';
	dbg("Operator Name is (%s) & Length (%d) ", p_out, length);

	return TRUE;
}

gboolean tcore_sim_decode_cfis(struct tel_sim_cfis *p_cfis, unsigned char *p_in, int in_length)
{
	int bcd_byte;	/* dialing number max length */
	int digit_len;
	int i = 0;
	if (in_length == 0)
		return FALSE;

	if (_is_empty(p_in, in_length) == TRUE) {
		dbg("empty record. all data is set 0xff");
		return TRUE;	/* this is empty record */
	}

	p_cfis->msp_num = p_in[i++];
	p_cfis->cfu_status = p_in[i++];

	/* get TON and NPI */
	p_cfis->ton = (p_in[++i] >> 4) & 0x07;
	p_cfis->npi = p_in[i++] & 0x0F;

	/* get actual dialing number length */
	/* current telephony supports 20 byte dialing number format. */
	bcd_byte = _get_valid_bcd_byte(&p_in[i], SIM_XDN_NUMBER_LEN_MAX / 2);

	/* get dialing number/SSC string */
	digit_len = _bcd_to_digit((char *) p_cfis->cfu_num, (char *) &p_in[i], bcd_byte); /* actual dialing number length in BCD. */
	dbg("Dialing number Length[%d]", digit_len);

	i = i + SIM_XDN_NUMBER_LEN_MAX / 2;

	/* get Capability/Configuration id */
	p_cfis->cc2_id = p_in[i++];

	/* get Extension1 id */
	p_cfis->ext7_id = p_in[i];

	dbg("MspNumber 0x%x", p_cfis->msp_num);
	dbg("Status 0x%x", p_cfis->cfu_status);
	dbg("TypeOfNumber %d", p_cfis->ton);
	dbg("NumberingPlanIdent %d", p_cfis->npi);
	dbg("Dialing number[%s]", p_cfis->cfu_num);

	return TRUE;
}

gboolean tcore_sim_decode_img(struct tel_sim_img *p_out, unsigned char *p_in, int in_length)
{
	int i = 1;

	dbg("Func Entrance");

	if ((NULL == p_out) || (NULL == p_in))
		return FALSE;

	if ((in_length == 0) || (in_length == 0xff) || (10 > in_length)) {
		dbg("No valid IMG data present - length:[%x]", in_length);
		return FALSE;
	}

	if (_is_empty(p_in, in_length) == TRUE) {
		dbg("empty record. all data is set 0xff");
		return FALSE;	/* this is empty record */
	}

	/* We are trying to decode only the 1st valid image data property and ignoring other for time being */
	p_out->width = p_in[i++];
	p_out->height = p_in[i++];
	p_out->ics = p_in[i++];
	p_out->iidf_fileid =  (*(p_in+4) << 8) | (*(p_in+5) & 0x00ff); /*index is 4 and 5 because the 1st byte is number of actual image instance */
	p_out->offset =  (*(p_in+6) << 8) | (*(p_in+7) & 0x00ff);
	p_out->length =  (*(p_in+8) << 8) | (*(p_in+9) & 0x00ff);

	dbg("p_out->width[%d], p_out->height[%d],  p_out->ics[%d],  p_out->offset[%d], p_out->length[%d], p_out->iidf_fileid[0x%02x]",
		p_out->width, p_out->height,  p_out->ics, p_out->offset, p_out->length, p_out->iidf_fileid);

	return TRUE;
}

gboolean tcore_sim_decode_isim_impi(struct tel_sim_impi *p_out, unsigned char *p_in, int in_length)
{
	unsigned char tag = 0, len = 0;

	if (p_in == NULL || p_out == NULL || in_length < 2)
		return FALSE;

	tag = p_in[0];
	if (tag != 0x80) {
		err("tag[0x%x] should be 0x80. return FALSE.", tag);
		return FALSE;
	}

	len = p_in[1];

	if (in_length < len + 2)
		err("invalid length. in_length[%d] < TLV_len[%d] + 2", in_length, len);

	p_out->impi = g_memdup(&p_in[2], len);

	return TRUE;
}

gboolean tcore_sim_decode_isim_domain(struct tel_sim_domain *p_out, unsigned char *p_in, int in_length)
{
	unsigned char tag = 0, len = 0;

	if (p_in == NULL || p_out == NULL || in_length < 2)
		return FALSE;

	tag = p_in[0];
	if (tag != 0x80) {
		err("tag[0x%x] should be 0x80. return FALSE.", tag);
		return FALSE;
	}

	len = p_in[1];

	if (in_length < len + 2)
		err("invalid length. in_length[%d] < TLV_len[%d] + 2", in_length, len);

	p_out->domain = g_memdup(&p_in[2], len);

	return TRUE;
}

gboolean tcore_sim_decode_isim_impu(struct tel_sim_impu *p_out, unsigned char *p_in, int in_length)
{
	unsigned char tag = 0, len = 0;

	if (p_in == NULL || p_out == NULL || in_length < 2)
		return FALSE;

	tag = p_in[0];
	if (tag != 0x80) {
		err("tag[0x%x] should be 0x80. return FALSE.", tag);
		return FALSE;
	}

	len = p_in[1];

	if (in_length < len + 2)
		err("invalid length. in_length[%d] < TLV_len[%d] + 2", in_length, len);

	p_out->impu = g_memdup(&p_in[2], len);

	return TRUE;
}

gboolean tcore_sim_decode_isim_pcscf(struct tel_sim_pcscf *p_out, unsigned char *p_in, int in_length)
{
	unsigned char tag = 0, len = 0;

	if (p_in == NULL || p_out == NULL || in_length < 2)
		return FALSE;

	tag = p_in[0];
	if (tag != 0x80) {
		err("tag[0x%x] should be 0x80. return FALSE.", tag);
		return FALSE;
	}

	/* P-CSCF Address length */
	len = p_in[1];
	if (len <= 0) {
		err("invalid address length : len : %d", len);
		return FALSE;
	}

	if (in_length < len + 2) {
		err("invalid length. in_length[%d] < TLV_len[%d] + 2", in_length, len);
		return FALSE;
	}

	/* P-CSCF Address type */
	if (p_in[2] == 0x00)
		p_out->type = SIM_PCSCF_TYPE_FQDN;
	else if (p_in[2] == 0x01)
		p_out->type = SIM_PCSCF_TYPE_IPV4;
	else if (p_in[2] == 0x02)
		p_out->type = SIM_PCSCF_TYPE_IPV6;

	/*
	 * If P-CSCF Address type is "00" (FQDN), then address shall be
	 * encoded to an octet string according to UTF-8 encoding rules.
	 * So, no need to convert it to UTF-8.
	 */
	if (p_out->type == SIM_PCSCF_TYPE_FQDN) {
		p_out->pcscf = g_memdup(&p_in[3], len - 1);
	} else {
		unsigned char buf[255] = {0, };
		unsigned short buf_len;
		gboolean ret = FALSE;

		ret = tcore_util_convert_string_to_utf8(buf, &buf_len, ALPHABET_FORMAT_SMS_DEFAULT,
						(const unsigned char *)&p_in[3], len - 1);
		if (ret)
			p_out->pcscf = g_memdup(buf, buf_len);
	}

	return TRUE;
}

char *tcore_sim_encode_cfis(int *out_length, const struct tel_sim_cfis *p_cfis)
{
	char *encoded_o = NULL;
	char bcd[10];

	encoded_o = calloc(1, 16); /* EF-CFIS record length is 16 */
	if (!encoded_o)
		return NULL;

	memset(bcd, 0xff, 10);

	/*
	* Bytes		Description				M/O	Length
	*----------------------------------------------------------------------------------------
	*    1		MSP number				M	1 byte
	*    2		CFU indicator status			M	1 byte
	*    3		Length of BCD number			M	1 byte
	*    4		TON and NPI				M	1 byte
	* 5 to 14		Dialing Number				M	10 bytes. unused byte should be set with 'F'
	*   15		Capability/Configuration2 Record Identifier	M	1 byte
	*   16		Extension 7 Record Identifier			M	1 byte
	*/
	encoded_o[0] = p_cfis->msp_num;
	encoded_o[1] = p_cfis->cfu_status;

	encoded_o[2] = (strlen(p_cfis->cfu_num) + 1)  / 2;

	/* set TON and NPI */
	encoded_o[3] = 0x80;
	encoded_o[3] |= (p_cfis->ton & 0x07) << 4;
	encoded_o[3] |= p_cfis->npi & 0x0F;

	_digit_to_bcd(bcd, (char *)&p_cfis->cfu_num, strlen(p_cfis->cfu_num));
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

	memset((void *)p_df, 0, sizeof(struct tel_sim_cphs_dflag));

	switch (p_in[0] & 0x01) {
	case 0x00:
		p_df->DynamicFlags = SIM_DYNAMIC_FLAGS_LINE2;
	break;

	case 0x01:
		p_df->DynamicFlags = SIM_DYNAMIC_FLAGS_LINE1;
	break;

	default:
		warn("invalid input");
	break;
	}

	return TRUE;
}

gboolean tcore_sim_decode_dynamic2_flag(struct tel_sim_cphs_dflag2 *p_d2f, unsigned char *p_in, int in_length)
{
	if (in_length == 0)
		return FALSE;

	memset((void *) p_d2f, 0, sizeof(struct tel_sim_cphs_dflag2));

	switch (p_in[0] & 0x01) {
	case 0x00:
		p_d2f->Dynamic2Flag = SIM_PIN2_ACCESSIBLE_FLAGS_UNLOCKED;
	break;

	case 0x01:
		p_d2f->Dynamic2Flag = SIM_PIN2_ACCESSIBLE_FLAGS_LOCKED;
	break;

	default:
		warn("invalid input");
	break;
	}

	return TRUE;
}

gboolean tcore_sim_encode_dynamic_flag(char *p_out, int out_length, struct tel_sim_cphs_dflag *p_df)
{
	if (out_length == 0 || out_length > 1)
		return FALSE;

	memset((void *)p_out, 0xFF, out_length);
	p_out[0] = p_df->DynamicFlags;

	return TRUE;
}

gboolean tcore_sim_encode_dynamic2_flag(char *p_out, int out_length, struct tel_sim_cphs_dflag2 *p_d2f)
{
	if (out_length == 0 || out_length > 1)
		return FALSE;

	memset((void *)p_out, 0xFF, out_length);
	p_out[0] = p_d2f->Dynamic2Flag;

	return TRUE;
}

gboolean tcore_sim_decode_cphs_info(struct tel_sim_cphs_info *pCphsInfo, unsigned char *p_in, int in_length)
{
	int i, j = 0;
	unsigned char mask = 0x03;
	unsigned char *pTemp = (unsigned char *) &pCphsInfo->CphsServiceTable;

	memset((void *)pCphsInfo, 0, sizeof(struct tel_sim_cphs_info));

	if (in_length == 0)
		return FALSE;

	 /* CPHS info EF has 3 bytes data. */
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

	dbg("Cphs Phase %d \n", pCphsInfo->CphsPhase);

	for (i = 1; i < in_length; i++) { /* CPHS SST is only 2 bytes */
		mask = 0x03;	/* reset mast to check first bit */
		for (j = 0; j < 4; j++) {
			if (p_in[i] & mask)
				*pTemp = 1;

			dbg("Byte (%d), service (%d) ", i + 1, *pTemp);
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

	if (_is_empty(p_in, in_length) == TRUE)
		return FALSE;	/* this is empty record */

	/* CPHS specification shows current EF include 10 bytes */
	if (in_length == 0)
		return FALSE;

	if (in_length > SIM_CPHS_OPERATOR_NAME_SHORT_FORM_LEN_MAX)
		in_length = SIM_CPHS_OPERATOR_NAME_SHORT_FORM_LEN_MAX;

	length = _get_string(p_out, p_in, in_length);
	p_out[length] = '\0';
	dbg("Operator short Name is (%s) &  length (%d)", p_out, length);

	return TRUE;
}

gboolean tcore_sim_decode_information_number(struct tel_sim_cphs_info_number *p_info, unsigned char *p_in, int in_length)
{
	int i;

	if (in_length == 0)
		return FALSE;

	for (i = 0; i < in_length; i++)
		dbg(" \t0x%04X.", p_in[i]);

	if (_is_empty(p_in, in_length) == TRUE)
		return FALSE;	/* this is empty record */

	p_info->AlphaIdLength = p_in[0];

	if (p_in[1] & 0x0F)
		p_info->IndexLevelIndicator = (enum tel_sim_cphs_index_level)(p_in[1] & 0x0F);

	if (p_in[1] & 0x10)
		p_info->PremiumServiceIndicator = TRUE;

	if (p_in[1] & 0x20)
		p_info->PremiumServiceIndicator = TRUE;

	_get_string(p_info->Alpha_id, &p_in[2], p_info->AlphaIdLength);

	p_info->DiallingnumLength = p_in[2 + p_info->AlphaIdLength] * 2;
	p_info->TypeOfNumber = (p_in[3 + p_info->AlphaIdLength] >> 4) & 0x07;
	p_info->NumberingPlanIdentity = p_in[3 + p_info->AlphaIdLength] & 0x0F;

	/* get dialing number/SSC string */
	_bcd_to_digit((char *)p_info->DiallingnumLength,
		(char *)&p_in[4 + p_info->AlphaIdLength],
		p_info->DiallingnumLength / 2); /* actual dialing number length in BCD. */

	/* get Extension1 id */
	p_info->Ext1RecordId = p_in[4 + p_info->AlphaIdLength + p_info->DiallingnumLength / 2];

	return TRUE;
}

gboolean tcore_sim_decode_opl(struct tel_sim_opl *p_opl, unsigned char *p_in, int in_length)
{
	if (_is_empty(p_in, in_length) == TRUE) {
		memset(p_opl, 0x00, sizeof(struct tel_sim_opl));
		return FALSE; /* this is empty record */
	}

	_decode_plmn(p_in, p_opl->plmn);
	dbg(" PLMN Code[%s]", p_opl->plmn);

	p_opl->lac_from = (*(p_in+3) << 8) | (*(p_in+4) & 0x00ff);
	dbg(" Start value of the LAC range[%x]", p_opl->lac_from);

	p_opl->lac_to = (*(p_in+5) << 8) | (*(p_in+6) & 0x00ff);
	dbg(" End value of the LAC range[%x]", p_opl->lac_to);

	p_opl->rec_identifier = p_in[7];
	dbg(" PNN Record identifier[%x]", p_opl->rec_identifier);

	return TRUE;
}

gboolean tcore_sim_decode_pnn(struct tel_sim_pnn *p_pnn, unsigned char *p_in, int in_length)
{
	int f_name_len = 0, s_name_len = 0;
	int cvt_leng = 0, s_name_base = 0;

	if (_is_empty(p_in, in_length) == TRUE) {
		memset(p_pnn, 0x00, sizeof(struct tel_sim_pnn));
		return FALSE;	/* this is empty record */
	}

	/* Full name for network IEI(Information Element Identifier), 0x43 */
	if (p_in[0] == 0x43) {
		dbg(" Full name of network IEI exist");
		/* f_name_part includes information byte. */
		f_name_len =  (int)p_in[1] - 1;

		/*
		 * 3rd byte information element(according to TS 24.008 for Network Name)
		 * 8 :ext1
		 * 7 6 5 : coding scheme
		 * 4 : Add CI
		 * 3 2 1 : number of spare bits in last octet
		 *
		 * Coding Scheme (octet 3, bits 5-7)
		 * 0	0	0		Cell Broadcast data coding scheme, GSM default alphabet, language unspecified, defined in 3GPP TS 23.038 [8b]
		 * 0	0	1		UCS2 (16 bit) [72]
		 * 0	1	0		to	reserved
		 * 1	1	1		to	reserved
		 */
		if ((p_in[2] & 0x70) >> 4 == 0) {
			dbg("DCS:GSM7");

			/*
			 * In case of GSM7, 35byte packing data will be
			 * converted 40 bytes unpacking string.
			 */
			if (f_name_len > (SIM_NW_FULL_NAME_LEN_MAX * 7) / 8)
				f_name_len = (SIM_NW_FULL_NAME_LEN_MAX * 7) / 8;

			_unpack_7bit28bit(p_in + 3,
				f_name_len, (unsigned char *)(p_pnn->full_name));
		} else if ((p_in[2] & 0x70) >> 4 == 1) {
			dbg("DCS:UCS2");

			/* current telephony supports 40 bytes network name string */
			if (f_name_len > SIM_NW_FULL_NAME_LEN_MAX)
				f_name_len = SIM_NW_FULL_NAME_LEN_MAX;

			_ucs2_to_utf8(f_name_len, p_in + 3,
				(int *)&cvt_leng, (unsigned char *)(p_pnn->full_name));
		} else {
			dbg("DCS:unknown");

			return FALSE;
		}

		dbg(" Full name of network contents[%s]", p_pnn->full_name);

		s_name_base = (int)p_in[1] + 2;
		dbg(" short name base byte [0x%02x]", s_name_base);

		/* Short Name for network IEI(Information Element Identifier), 0x45 */
		if (p_in[s_name_base] == 0x45) {
			dbg(" Short name of network IEI exist");

			/* s_name_part includes information byte. */
			s_name_len = p_in[s_name_base + 1] - 1;

			if ((p_in[s_name_base + 2] & 0x70) >> 4 == 0) {
				dbg("DCS:GSM7");

				/*
				 * In case of GSM7, 35byte packing data
				 * will be converted 40 bytes unpacking string.
				 */
				if (s_name_len > (SIM_NW_FULL_NAME_LEN_MAX * 7) / 8)
					s_name_len = (SIM_NW_FULL_NAME_LEN_MAX * 7) / 8;

				_unpack_7bit28bit(p_in + s_name_base + 3,
					s_name_len, (unsigned char *)(p_pnn->short_name));
			} else if ((p_in[s_name_base + 2] & 0x70) >> 4 == 1) {
				dbg("DCS:UCS2");

				if (s_name_len > SIM_NW_FULL_NAME_LEN_MAX)
					s_name_len = SIM_NW_FULL_NAME_LEN_MAX;

				_ucs2_to_utf8(s_name_len, p_in + s_name_base + 3,
					(int *)&cvt_leng, (unsigned char *)(p_pnn->short_name));
			} else {
				dbg("DCS:unknown");

				return FALSE;
			}

			dbg(" Short name of network contents[%s]", p_pnn->short_name);
		}

		return TRUE;
	}

	return FALSE;
}

gboolean tcore_sim_decode_oplmnwact(struct tel_sim_oplmnwact_list *p_list, unsigned char *p_in, int in_length)
{
	unsigned long m = 0;

	/*
	 * current raw data can include invalid OPLMN data(ex: ff ff ff 00 00).
	 * so we can`t decide the number of OPLMN records directly.
	 */
	int rawOplmnWactCount = 0;
	int i = 0;

	memset((void *)p_list, 0, sizeof(struct tel_sim_oplmnwact_list));

	rawOplmnWactCount = in_length / 5;

	dbg("rawOplmnWactCount[%d]", rawOplmnWactCount);

	for (i = 0; i < rawOplmnWactCount; i++) {
		/*
		 * Regarding current IPC data, even if there`s no OPLMN value,
		 * IPC data is sending with 'ff ff ff 00 00'. so we should check for validation.
		 */
		if (p_in[m] == 0xff) {
			p_list->opwa_count = m / 5;
			dbg("OPLMN(MCC+MNC) value is not found at p_in[m]=[%lu].So OPLMN count is [%d]",
				m, p_list->opwa_count);
			return TRUE;
		}

		_decode_plmn(&p_in[m], p_list->opwa[i].plmn);
		dbg("[%d] OPLMN PLMN Code[%s]", i, p_list->opwa[i].plmn);

		if (p_in[m+3] & 0x80)
			p_list->opwa[i].b_umts = 1;

		if (p_in[m+4] & 0x80)
			p_list->opwa[i].b_gsm = 1;

		m = m + 5;
	}

	p_list->opwa_count = rawOplmnWactCount;
	dbg("OPLMN count is p_list->opwa_count[%d]", p_list->opwa_count);

	return TRUE;
}

gboolean tcore_sim_decode_ef_info(struct tcore_sim_ef_info *p_ef_info, CoreObject *o, char *p_in, int in_length)
{
	unsigned short arr_file_id = 0;
	char *record_data = NULL;
	unsigned char file_type_tag = SIM_FILE_TYPE_TAG;
	unsigned char *ptr_data;
	unsigned char file_id_len = 0;
	gboolean ret = FALSE;

	if (!p_ef_info) {
		err("output parameter is null");
		return ret;
	}

	if (_is_empty((unsigned char *)p_in, in_length) == TRUE) {
		memset(p_ef_info, 0x00, sizeof(struct tcore_sim_ef_info));
		return ret;	/* Empty record */
	}

	record_data = tcore_util_convert_hexstring_to_bytes(p_in);
	if (!record_data) {
		err("tcore_util_convert_hexstring_to_bytes Failed!!");
		return ret;
	}
	tcore_util_hex_dump("	", strlen(p_in) / 2, record_data);

	ptr_data = (unsigned char *)record_data;
	switch (tcore_sim_get_type(o)) {
	case SIM_TYPE_USIM: {
		/*
		 * ETSI TS 102 221 v7.9.0
		 * - Response Data
		 * '62'	FCP template tag
		 * - Response for an EF
		 * '82'	M	File Descriptor
		 * '83'	M	File Identifier
		 * 'A5'	O	Proprietary information
		 * '8A'	M	Life Cycle Status Integer
		 * '8B', '8C' or 'AB' C1	Security attributes
		 * '80'	M	File size
		 * '81'	O	Total file size
		 * '88'	O	Short File Identifier (SFI)
		 */

		/* FCP template tag - File Control Parameters tag*/
		if (*ptr_data == SIM_FCP_TEMPLATE_TAG) {
			/* parse complete FCP tag. increment to next byte*/
			ptr_data++;

			dbg("tag_len: %02x", *ptr_data++);
			/* FCP file descriptor - file type, accessibility, DF, ADF etc*/
			if (*ptr_data == SIM_FILE_DESCRIPTOR_TAG) {
				/* increment to next byte */
				ptr_data++;

				/* 2 or 5 value*/
				ptr_data++;

				 /* consider only last 3 bits*/
				file_type_tag = file_type_tag & (*ptr_data);
				dbg("File Type Tag: %02x", file_type_tag);

				switch (file_type_tag) {
				case SIM_FTYPE_TRANSPARENT:
					dbg("FileType: [Transparent file type]");
					p_ef_info->file_type = SIM_FTYPE_TRANSPARENT;

					/* increment to next byte */
					ptr_data++;

					/* increment to next byte */
					ptr_data++;
				break;

				case SIM_FTYPE_LINEAR_FIXED:
					dbg("FileType: [Linear fixed file type]");
					/* increment to next byte */
					ptr_data++;

					/* data coding byte - value 21 */
					ptr_data++;

					/* 2bytes */
					memcpy(&p_ef_info->record_length, ptr_data, 2);

					/* swap bytes */
					p_ef_info->record_length = _swap_bytes16(p_ef_info->record_length);
					ptr_data = ptr_data + 2;
					p_ef_info->number_of_records = *ptr_data++;

					/* Data lossy conversation from enum (int) to unsigned char */
					p_ef_info->file_type = SIM_FTYPE_LINEAR_FIXED;
				break;

				case SIM_FTYPE_CYCLIC:
					dbg("FileType: [Cyclic fixed file type]");
					/* increment to next byte */
					ptr_data++;

					/* data coding byte - value 21 */
					ptr_data++;

					/* 2bytes */
					memcpy(&p_ef_info->record_length, ptr_data, 2);

					/* swap bytes */
					p_ef_info->record_length = _swap_bytes16(p_ef_info->record_length);
					ptr_data = ptr_data + 2;
					p_ef_info->number_of_records = *ptr_data++;
					p_ef_info->file_type = SIM_FTYPE_CYCLIC;

					break;

				default:
					err("Unhandled File Type [0x%x]", *ptr_data);
				break;
				}
			} else {
				err("INVALID FCP received[0x%x] - Debug!", *ptr_data);
				goto EXIT;
			}

			/* File identifier - file id */ /* 0x84, 0x85, 0x86 etc are currently ignored and not handled */
			if (*ptr_data == SIM_FILE_IDENTIFIER_TAG) {
				/* increment to next byte */
				ptr_data++;
				file_id_len = *ptr_data++;
				dbg("File ID length: %02x", file_id_len);

				memcpy(&p_ef_info->file_id, ptr_data, file_id_len);
				dbg("File ID: %x", p_ef_info->file_id);

				/* swap bytes */
				p_ef_info->file_id = _swap_bytes16(p_ef_info->file_id);

				ptr_data = ptr_data + 2;
			} else {
				err("INVALID FCP received[0x%x] - Debug!", *ptr_data);
				goto EXIT;
			}

			/* proprietary information */
			if (*ptr_data == SIM_PROPRIETARY_INFORMATION_TAG) {
				unsigned short prop_len;
				/* increment to next byte */
				ptr_data++;

				/* length */
				prop_len = *ptr_data;
				dbg("prop_len: %02x", prop_len);

				/* skip data */
				ptr_data = ptr_data + prop_len + 1;
			} else {
				err("INVALID FCP received[0x%x] - Debug!", *ptr_data);
			}

			/* life cycle status integer [8A][length:0x01][status]*/
			/*
			 * status info b8~b1
			 * 00000000 : No information given
			 * 00000001 : creation state
			 * 00000011 : initialization state
			 * 000001-1 : operation state -activated
			 * 000001-0 : operation state -deactivated
			 * 000011-- : Termination state
			 * b8~b5 !=0, b4~b1=X : Proprietary
			 * Any other value : RFU
			 */
			if (*ptr_data == SIM_LIFE_CYCLE_STATUS_TAG) {
				/* increment to next byte */
				ptr_data++;

				/* length - value 1 */
				ptr_data++;

				switch (*ptr_data) {
				case 0x04:
				case 0x06:
					dbg("Operation state - Deactivated");
					ptr_data++;
				break;

				case 0x05:
				case 0x07:
					dbg("Operation state - Activated");
					ptr_data++;
				break;

				default:
					dbg("DEBUG! LIFE CYCLE STATUS =[0x%x]", *ptr_data);
					ptr_data++;
				break;
				}
			}

			/* related to security attributes : currently not handled*/
			if (*ptr_data == 0x86 || *ptr_data == 0x8B || *ptr_data == 0x8C || *ptr_data == 0xAB) {
				/* increment to next byte */
				ptr_data++;

				/* if tag length is 3 */
				if (*ptr_data == 0x03) {
					/* increment to next byte */
					ptr_data++;

					/* EFARR file id */
					memcpy(&arr_file_id, ptr_data, 2);

					/* swap byes */
					arr_file_id = _swap_bytes16(arr_file_id);
					ptr_data = ptr_data + 2;
					ptr_data++;
				} else {
					/* if tag length is not 3 */
					/* ignoring bytes	*/
					dbg("Useless security attributes, so jump to next tag");
					ptr_data = ptr_data + (*ptr_data + 1);
				}
			} else {
				err("INVALID FCP received[0x%x] - Debug!", *ptr_data);
				goto EXIT;
			}

			/* file size excluding structural info*/
			if (*ptr_data == SIM_FILE_SIZE_TAG) {
				/*
				 * for EF file size is body of file and for Linear or cyclic it is
				 * number of recXsizeof(one record)
				 */
				/* increment to next byte */
				ptr_data++;

				/* length is 1 byte - value is 2 bytes or more */
				ptr_data++;
				memcpy(&p_ef_info->file_size, ptr_data, 2);

				/* swap bytes */
				p_ef_info->file_size = _swap_bytes16(p_ef_info->file_size);
				ptr_data = ptr_data + 2;
			} else {
				err("INVALID FCP received[0x%x] - Debug!", *ptr_data);
				goto EXIT;
			}

			/* total file size including structural info*/
			if (*ptr_data == SIM_TOTAL_FILE_SIZE_TAG) {
				/* increment to next byte */
				ptr_data++;
				/* ignored bytes */
				ptr_data = ptr_data + 3;
			} else {
				err("INVALID FCP received[0x%x] - Debug!", *ptr_data);
				/* 0x81 is optional tag?? check out! so do not return -1 from here! */
			}

			/*short file identifier ignored*/
			if (*ptr_data == SIM_SHORT_FILE_IDENTIFIER_TAG) {
				dbg("0x88: Do Nothing");
				/* DO NOTHING */
			}
		} else {
			err("INVALID FCP received[0x%x] - Debug!", *ptr_data);
			goto EXIT;
		}
	}
	break;
	case SIM_TYPE_GSM: {
		/*Ignore RFU byte1 and byte2 */
		ptr_data++;
		ptr_data++;

		/* file size */
		memcpy(&p_ef_info->file_size, ptr_data, 2);

		/* swap bytes */
		p_ef_info->file_size = _swap_bytes16(p_ef_info->file_size);

		/* parsed file size */
		ptr_data = ptr_data + 2;

		/* file id */
		memcpy(&p_ef_info->file_id, ptr_data, 2);
		p_ef_info->file_id = _swap_bytes16(p_ef_info->file_id);
		ptr_data = ptr_data + 2;

		/* save file type - transparent, linear fixed or cyclic */
		file_type_tag = (*(ptr_data + 7));

		switch (*ptr_data) {
		case SIM_FTYPE_RFU:
			/* RFU file type */
			dbg("RFU file type- not handled - Debug!");
		break;

		case SIM_FTYPE_MF:
			/* MF file type */
			dbg("MF file type - not handled - Debug!");
		break;

		case SIM_FTYPE_DF:
			/* DF file type */
			dbg("DF file type - not handled - Debug!");
		break;

		case SIM_FTYPE_EF:
			/* EF file type */
			dbg("EF file type tag[%d] ", file_type_tag);
			/* increment to next byte */
			ptr_data++;

			if (file_type_tag == 0x00 || file_type_tag == 0x01) {
				/* increament to next byte as this byte is RFU */
				ptr_data++;
				p_ef_info->file_type = (file_type_tag == 0x00) ? SIM_FTYPE_TRANSPARENT : SIM_FTYPE_LINEAR_FIXED;
			} else {
				/* increment to next byte */
				ptr_data++;

				/* For a cyclic EF all bits except bit 7 are RFU; b7=1 indicates that */
				/* the INCREASE command is allowed on the selected cyclic file. */
				p_ef_info->file_type = SIM_FTYPE_CYCLIC;
			}

			/* bytes 9 to 11 give SIM file access conditions */
			ptr_data++;

			/* byte 10 has one nibble that is RF U and another for INCREASE which is not used currently */
			ptr_data++;

			/* byte 11 is invalidate and rehabilate nibbles */
			ptr_data++;

			/* byte 12 - file status */
			ptr_data++;

			/* byte 13 - GSM specific data */
			ptr_data++;

			/* byte 14 - structure of EF - transparent or linear or cyclic , already saved above */
			ptr_data++;

			/* byte 15 - length of record for linear and cyclic , for transparent it is set to 0x00. */
			p_ef_info->record_length = *ptr_data;
			if (p_ef_info->record_length != 0)
				p_ef_info->number_of_records = (p_ef_info->file_size / p_ef_info->record_length);
		break;

		default:
			err("Unhandled file type[0x%x]", *ptr_data);
		break;
		}
	}
	break;
	default:
		err("Unknown SIM type [%d]", tcore_sim_get_type(o));
	break;
	}

	ret = TRUE;

EXIT:
	g_free(record_data);
	return ret;
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

struct tel_sim_imsi *tcore_sim_get_imsi(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return NULL;
	}

	return g_memdup(&po->imsi, sizeof(struct tel_sim_imsi));
}

gboolean tcore_sim_set_imsi(CoreObject *o, const struct tel_sim_imsi *imsi)
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

struct tel_sim_msisdn_list *tcore_sim_get_msisdn_list(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return NULL;
	} else if (!po->msisdn_list) {
		dbg("po->msisdn_list is NULL");
		return NULL;
	}

	return g_memdup(po->msisdn_list, sizeof(struct tel_sim_msisdn_list));
}

gboolean tcore_sim_set_msisdn_list(CoreObject *o, const struct tel_sim_msisdn_list *msisdn_list)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	 if (po->msisdn_list) {
		if (msisdn_list)
			memcpy(po->msisdn_list, msisdn_list, sizeof(struct tel_sim_msisdn_list));
		else
			memset(po->msisdn_list, 0x0, sizeof(struct tel_sim_msisdn_list));
	} else {
		if (msisdn_list)
			po->msisdn_list = g_memdup(msisdn_list, sizeof(struct tel_sim_msisdn_list));
		else
			po->msisdn_list = g_malloc0(sizeof(struct tel_sim_msisdn_list));
	}

	return TRUE;
}

enum tcore_return tcore_sim_delete_msisdn_list(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return TCORE_RETURN_EINVAL;
	}

	if (po->msisdn_list) {
		free(po->msisdn_list);
		po->msisdn_list = NULL;
	}

	return TCORE_RETURN_SUCCESS;
}

struct tel_sim_service_table *tcore_sim_get_service_table(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return NULL;
	} else if (!po->svct) {
		dbg("po->svct is NULL.");
		return NULL;
	}

	return g_memdup(po->svct, sizeof(struct tel_sim_service_table));
}

gboolean tcore_sim_set_service_table(CoreObject *o, const struct tel_sim_service_table *svct)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	 if (po->svct) {
		if (svct)
			memcpy(po->svct, svct, sizeof(struct tel_sim_service_table));
		else
			memset(po->svct, 0x0, sizeof(struct tel_sim_service_table));
	} else {
		if (svct)
			po->svct = g_memdup(svct, sizeof(struct tel_sim_service_table));
		else
			po->svct = g_malloc0(sizeof(struct tel_sim_service_table));
	}

	return TRUE;
}

enum tcore_return tcore_sim_delete_service_table(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return TCORE_RETURN_EINVAL;
	}

	if (po->svct) {
		free(po->svct);
		po->svct = NULL;
	}

	return TCORE_RETURN_SUCCESS;
}

gboolean tcore_sim_get_cphs_status(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	return po->b_cphs;
}

gboolean tcore_sim_set_cphs_status(CoreObject *o, gboolean b_support)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	po->b_cphs = b_support;

	return TRUE;
}

struct tel_sim_cphs_csp *tcore_sim_get_csp(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return NULL;
	} else if (!po->csp) {
		dbg("po->csp is NULL");
		return NULL;
	}

	return g_memdup(po->csp, sizeof(struct tel_sim_cphs_csp));
}

gboolean tcore_sim_set_csp(CoreObject *o, const struct tel_sim_cphs_csp *csp)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	 if (po->csp) {
		if (csp)
			memcpy(po->csp, csp, sizeof(struct tel_sim_cphs_csp));
		else
			memset(po->csp, 0x0, sizeof(struct tel_sim_cphs_csp));
	} else {
		if (csp)
			po->csp = g_memdup(csp, sizeof(struct tel_sim_cphs_csp));
		else
			po->csp = g_malloc0(sizeof(struct tel_sim_cphs_csp));
	}

	return TRUE;
}

gboolean tcore_sim_delete_csp(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	if (po->csp) {
		free(po->csp);
		po->csp = NULL;
	}

	return TRUE;
}

struct tel_sim_ecc_list *tcore_sim_get_ecc_list(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return NULL;
	} else if (!po->ecc_list) {
		dbg("po->ecc_list is NULL");
		return NULL;
	}

	return g_memdup(po->ecc_list, sizeof(struct tel_sim_ecc_list));
}

gboolean tcore_sim_set_ecc_list(CoreObject *o, const struct tel_sim_ecc_list *ecc_list)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	 if (po->ecc_list) {
		if (ecc_list)
			memcpy(po->ecc_list, ecc_list, sizeof(struct tel_sim_ecc_list));
		else
			memset(po->ecc_list, 0x0, sizeof(struct tel_sim_ecc_list));
	} else {
		if (ecc_list)
			po->ecc_list = g_memdup(ecc_list, sizeof(struct tel_sim_ecc_list));
		else
			po->ecc_list = g_malloc0(sizeof(struct tel_sim_ecc_list));
	}

	return TRUE;
}

enum tcore_return tcore_sim_delete_ecc_list(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return TCORE_RETURN_EINVAL;
	}

	if (po->ecc_list) {
		free(po->ecc_list);
		po->ecc_list = NULL;
	}

	return TCORE_RETURN_SUCCESS;
}

struct tel_sim_spn *tcore_sim_get_spn(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return NULL;
	} else if (!po->spn) {
		dbg("po->spn is NULL");
		return NULL;
	}

	return g_memdup(po->spn, sizeof(struct tel_sim_spn));
}

gboolean tcore_sim_set_spn(CoreObject *o, const struct tel_sim_spn *spn)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	 if (po->spn) {
		if (spn)
			memcpy(po->spn, spn, sizeof(struct tel_sim_spn));
		else
			memset(po->spn, 0x0, sizeof(struct tel_sim_spn));
	} else {
		if (spn)
			po->spn = g_memdup(spn, sizeof(struct tel_sim_spn));
		else
			po->spn = g_malloc0(sizeof(struct tel_sim_spn));
	}

	return TRUE;
}

enum tcore_return tcore_sim_delete_spn(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return TCORE_RETURN_EINVAL;
	}

	if (po->spn) {
		free(po->spn);
		po->spn = NULL;
	}

	return TCORE_RETURN_SUCCESS;
}

struct tel_sim_cphs_netname *tcore_sim_get_cphs_netname(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return NULL;
	} else if (!po->cphs_netname) {
		dbg("po->cphs_netname is NULL");
		return NULL;
	}

	return g_memdup(po->cphs_netname, sizeof(struct tel_sim_cphs_netname));
}

gboolean tcore_sim_set_cphs_netname(CoreObject *o, const struct tel_sim_cphs_netname *cphs_netname)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	 if (po->cphs_netname) {
		if (cphs_netname)
			memcpy(po->cphs_netname, cphs_netname, sizeof(struct tel_sim_cphs_netname));
		else
			memset(po->cphs_netname, 0x0, sizeof(struct tel_sim_cphs_netname));
	} else {
		if (cphs_netname)
			po->cphs_netname = g_memdup(cphs_netname, sizeof(struct tel_sim_cphs_netname));
		else
			po->cphs_netname = g_malloc0(sizeof(struct tel_sim_cphs_netname));
	}

	return TRUE;
}

enum tcore_return tcore_sim_delete_cphs_netname(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return TCORE_RETURN_EINVAL;
	}

	if (po->cphs_netname) {
		free(po->cphs_netname);
		po->cphs_netname = NULL;
	}

	return TCORE_RETURN_SUCCESS;
}

struct tel_sim_iccid *tcore_sim_get_iccid(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return NULL;
	} else if (!po->iccid) {
		dbg("po->iccid is NULL");
		return NULL;
	}

	return g_memdup(po->iccid, sizeof(struct tel_sim_iccid));
}

gboolean tcore_sim_set_iccid(CoreObject *o, const struct tel_sim_iccid *iccid)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	 if (po->iccid) {
		if (iccid)
			memcpy(po->iccid, iccid, sizeof(struct tel_sim_iccid));
		else
			memset(po->iccid, 0x0, sizeof(struct tel_sim_iccid));
	} else {
		if (iccid)
			po->iccid = g_memdup(iccid, sizeof(struct tel_sim_iccid));
		else
			po->iccid = g_malloc0(sizeof(struct tel_sim_iccid));
	}

	return TRUE;
}

enum tcore_return tcore_sim_delete_iccid(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return TCORE_RETURN_EINVAL;
	}

	if (po->iccid) {
		free(po->iccid);
		po->iccid = NULL;
	}

	return TCORE_RETURN_SUCCESS;
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

void *tcore_sim_ref_userdata(CoreObject *o)
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

	/* set ops to default type when core object is created. */
	tmp_ops = po->ops[TCORE_OPS_TYPE_CP];

	memset(po, 0x00, sizeof(struct private_object_data));
	po->ops[TCORE_OPS_TYPE_CP] = tmp_ops;
	po->sim_status = SIM_STATUS_UNKNOWN;
}

unsigned char tcore_sim_get_app_list(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		err("po access fail");
		return 0;
	}

	return po->app_list;
}

gboolean tcore_sim_set_app_list(CoreObject *o, unsigned char app_list)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		err("po access fail");
		return FALSE;
	}

	po->app_list = app_list;

	return TRUE;
}

struct tel_sim_ist *tcore_sim_get_isim_service_table(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		err("po access fail");
		return NULL;
	}

	if (!po->ist) {
		err("po->ist is NULL");
		return NULL;
	}

	return g_memdup(po->ist, sizeof(struct tel_sim_ist));
}

gboolean tcore_sim_set_isim_service_table(CoreObject *o, struct tel_sim_ist *ist)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po) {
		err("po access fail");
		return FALSE;
	}

	if (!ist) {
		err("ist is NULL");
		return FALSE;
	}

	if (po->ist)
		memcpy(po->ist, ist, sizeof(struct tel_sim_ist));
	else
		po->ist = g_memdup(ist, sizeof(struct tel_sim_ist));

	return TRUE;
}

enum tcore_return tcore_sim_delete_isim_service_table(CoreObject *o)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		err("po access fail");
		return TCORE_RETURN_EINVAL;
	}

	if (po->ist) {
		free(po->ist);
		po->ist = NULL;
	}

	return TCORE_RETURN_SUCCESS;
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

	po = calloc(1, sizeof(struct private_object_data));
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	/* set ops to default type when core object is created. */
	po->ops[TCORE_OPS_TYPE_CP] = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_SIM);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	tcore_sim_initialize_context(o);

	return o;
}

void tcore_sim_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SIM);
	tcore_object_free(o);
}

void tcore_sim_set_ops(CoreObject *o, struct tcore_sim_operations *ops, enum tcore_ops_type ops_type)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SIM);
	CORE_OBJECT_VALIDATE_OPS_RETURN(ops_type);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		err("po is NULL");
		return;
	}

	po->ops[ops_type] = ops;
}
