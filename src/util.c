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
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <glib.h>
#include <glib-object.h>
#include <iconv.h>

#include "tcore.h"
#include "util.h"

#define TEXT_STRING_LEN_MAX			512

/* Unicode table lengths */
#define GSM_UNICODE2_TABLE_LEN_MAX		9
#define GSM_UNICODE_TABLE_LEN_MAX		42

#define convert_to_hex(in, out)	(in <= 9) ? \
	(out = '0' + in) : (out = 'A' + in - 10)

			/* EF-SMSP digit length */

#define MAX_GSM_SMS_PARAM_RECORD_SIZE		156		/* Maximum number of bytes SMSP Record size (Y + 28), y : 0 ~ 128 */

#define SMS_SMSP_PARAMS_MAX_LEN	28
#define SMS_SMSP_ALPHA_ID_LEN_MAX	128	/* EF-SMSP alpha id length */
#define SMS_SMSP_DEST_ADDRESS_LEN	12	/* EF-SMSP Destination Address length */
#define SMS_SMSP_ADDRESS_LEN	20	/* EF-SMSP digit length */

#define SMS_DEFAULT_SMSP_WITHOUT_ALPHAID	28

#define SMS_SMSP_SCA_ADDRESS	0x02
#define SMS_SMSP_VP 0x10

#define SMS_SCA_ADDR_OFFSET 12
#define SMS_VP_OFFSET 27

#define MAX_GSM_SMS_PARAM_RECORD_SIZE	 156

typedef struct {
	gchar gsm;
	gushort unicode;
} TelUtilGsmUnicodeTable;

/* GSM Unicode Table */
const TelUtilGsmUnicodeTable gsm_unicode_table[] = {
	{ 0x00, 0x0040 }, { 0x01, 0x00A3 }, { 0x02, 0x0024 }, { 0x03, 0x00A5 },
	{ 0x04, 0x00E8 }, { 0x05, 0x00E9 }, { 0x06, 0x00F9 }, { 0x07, 0x00EC },
	{ 0x08, 0x00F2 }, { 0x09, 0x00E7 }, { 0x0B, 0x00D8 }, { 0x0C, 0x00F8 },
	{ 0x0E, 0x00C5 }, { 0x0F, 0x00E5 }, { 0x10, 0x0394 }, { 0x11, 0x005F },
	{ 0x12, 0x03A6 }, { 0x13, 0x0393 }, { 0x14, 0x039B }, { 0x15, 0x03A9 },
	{ 0x16, 0x03A0 }, { 0x17, 0x03A8 }, { 0x18, 0x03A3 }, { 0x19, 0x0398 },
	{ 0x1A, 0x039E }, { 0x1C, 0x00C6 }, { 0x1D, 0x00E6 }, { 0x1E, 0x00DF },
	{ 0x1F, 0x00C9 }, { 0x24, 0x00A4 }, { 0x40, 0x00A1 }, { 0x5B, 0x00C4 },
	{ 0x5C, 0x00D6 }, { 0x5D, 0x00D1 }, { 0x5E, 0x00DC }, { 0x5F, 0x00A7 },
	{ 0x60, 0x00BF }, { 0x7B, 0x00E4 }, { 0x7C, 0x00F6 }, { 0x7D, 0x00F1 },
	{ 0x7E, 0x00FC }, { 0x7F, 0x00E0 },
};

/* GSM Unicode2 Table */
const TelUtilGsmUnicodeTable gsm_unicode2_table[] = {
	{ 0x14, 0x005E }, { 0x28, 0x007B }, { 0x29, 0x007D }, { 0x2F, 0x005C },
	{ 0x3C, 0x005B }, { 0x3D, 0x007E }, { 0x3E, 0x005D }, { 0x40, 0x007C },
	{ 0x65, 0x20AC }
};



/* Internal APIs */
static gboolean __find_gsm_code_exception_table(gushort src)
{
	if ((src >= 0x0020 && src <= 0x0023)
			|| (src >= 0x0025 && src <= 0x003F)
			|| (src >= 0x0041 && src <= 0x005A)
			|| (src >= 0x0061 && src <= 0x007A)
			|| src == 0x000A || src == 0x000D)
		return TRUE;

	return FALSE;
}

static int __get_gsm_code_size(gushort *src, guint src_len)
{
	gboolean in_table = FALSE;
	gboolean in_sec_table = FALSE;
	gint gsm_len = 0;
	guint i;

	if (NULL == src) {
		dbg("INPUT PARAM was NULL");
		return -1;
	}

	for (; src_len > 0 && src; src_len--) {
		if (__find_gsm_code_exception_table(*src) == TRUE) {
			src++;
			gsm_len++;
			continue;
		}

		in_table = FALSE;
		for (i = 0; i < GSM_UNICODE_TABLE_LEN_MAX; i++) {
			if (*src == gsm_unicode_table[i].unicode) {
				src++;
				in_table = TRUE;
				gsm_len++;
				break;
			}
		}

		if (in_table == FALSE) {
			in_sec_table = FALSE;
			for (i = 0; i < GSM_UNICODE2_TABLE_LEN_MAX; i++) {/* second table */
				if (*src == gsm_unicode2_table[i].unicode) {
					src++;
					in_table = TRUE;
					in_sec_table = TRUE;
					gsm_len += 2;
					break;
				}
			}

			if (in_sec_table == FALSE) {/* second*/
				if (__find_gsm_code_exception_table(*src) == FALSE) {
					dbg("GSM Char[%d], gsm_len[%d]", *src, gsm_len);
					return -1;
				}
				src++;
				gsm_len++;
			}
		}
	}

	return gsm_len;
}

static char *__acitoa(int n, char* str, int b)
{
	guint i = 0;

	do {
		str[i++] = "0123456789ABCDEF"[n % b];
	} while ((n /= b) > 0);

	str = (char *)g_strreverse((gchar *)str);
	str[i] = '\0';

	return str;
}

static gint __convert_ucs2_to_gsm(guchar *dest,
	gushort *src, guint src_len)
{
	guchar* rear = NULL;
	gushort* p;
	guchar temp;
	gboolean in_table = FALSE;
	gboolean in_sec_table = FALSE;
	gint gsm_code_len = 0;
	guint i;

	if ((!dest) || (!src) || (0x00 == src_len)) {
		dbg("Warning: Wrong Input");
		return -1;
	}

	rear = dest;
	p = src;

	for (; src_len > 0 && p; src_len--) {
		in_table = FALSE;
		for (i = 0; i < GSM_UNICODE_TABLE_LEN_MAX; i++) { /* is in table  */
			if (*p == gsm_unicode_table[i].unicode) {
				temp = (guchar) (gsm_unicode_table[i].gsm);
				*rear = temp;
				rear++;
				p++;
				in_table = TRUE;
				gsm_code_len++;
				break;
			}
		}

		if (in_table == FALSE) {
			in_sec_table = FALSE;
			for (i = 0; i < GSM_UNICODE2_TABLE_LEN_MAX; i++) { /* second table*/
				if (*p == gsm_unicode2_table[i].unicode) {
					*rear = 0x1B;
					rear++;
					temp = (guchar) (gsm_unicode2_table[i].gsm);
					*rear = temp;
					rear++;
					p++;
					in_table = TRUE;
					in_sec_table = TRUE;
					gsm_code_len += 2;
					break;
				}
			}

			if (in_sec_table == FALSE) { /* second */
				if (__find_gsm_code_exception_table(*p) == FALSE)
					return -1;

				temp = (guchar) (*p); /* isn't in table. but it's just able to be converted to GSM (0x00?? -> 0x??)*/
				*rear = temp;
				rear++;
				p++;
				gsm_code_len++;
			}
		}
	}

	src = p;
	return gsm_code_len;
}

static gboolean __convert_unicode_to_gsm(guchar *dest, guint dest_len,
	gushort *src, guint src_len)
{
	gchar *gsm_str;
	int gsm_code_len = 0;

	if ((NULL == dest) || (NULL == src)) {
		dbg("INPUT PARAM was NULL");
		return FALSE;
	}

	if (src_len == 0)
		return FALSE;

	/* Finding the GSM code size */
	gsm_code_len = __get_gsm_code_size(src, src_len);
	if (0 >= gsm_code_len) {
		dbg("Warning: Error[%d] while finding the GSM Code Size", gsm_code_len);
		return FALSE;
	}

	if (dest_len < (guint)gsm_code_len) {
		if (dest_len == sizeof(void *)) {
			dbg("Out buffer (%s) size seems to be small", dest);
		} else {
			dbg("Buffer (%s) size is too small - length [%d]"
				"gsm_code_len(%d)", dest, dest_len, gsm_code_len);
		}
		return FALSE;
	}

	gsm_str = tcore_malloc0(gsm_code_len);

	/* Conversion: UCS2 --> GSM */
	gsm_code_len = __convert_ucs2_to_gsm((guchar*)gsm_str, src, src_len);
	if (gsm_code_len == -1) {
		tcore_free(gsm_str);
		return FALSE;
	}
	memcpy((char*)dest, (char*)gsm_str, gsm_code_len);

	tcore_free(gsm_str);
	return TRUE;
}

static gint __convert_gsm_to_ucs2(gushort *dest,
	guchar *src, guint src_len)
{
	gushort *org;
	guint i;

	org = dest;

	for (i = 0 ; i < src_len ; i++) {
		guint table_index = 0;
		gboolean b_tabled = FALSE;

		/*
		 * if the first byte is 0x1B, it is the escape character.
		 * The byte value shoulbe be changed to unicode.
		 */
		if (*src == 0x1B) {
			src++;
			i++;//move to next byte

			for (table_index = 0;
					table_index < GSM_UNICODE2_TABLE_LEN_MAX;
					table_index++) {
				if (*src == gsm_unicode2_table[table_index].gsm) {
					*dest = gsm_unicode2_table[table_index].unicode;
					b_tabled = TRUE;
					break;
				}
			}

			/* If matched data is not in table, it should be changed to NULL; */
			if (!b_tabled)
				*dest = 0x0020;
		}
		else {
			for (table_index = 0;
					table_index < GSM_UNICODE_TABLE_LEN_MAX;
					table_index++) {
				if (*src == gsm_unicode_table[table_index].gsm) {
					*dest = gsm_unicode_table[table_index].unicode;
					b_tabled = TRUE;
					break;
				}
			}

			/* If matched data is not in table, it is using original value; */
			if (!b_tabled)
				*dest = *src;
		}

		/* Move to next position */
		src++;
		dest++;
	}

	dbg("cvt sr(%s), the size of data (%d) ", org, dest - org);
	return (dest - org);
}

static gchar *__convert_ucs_to_utf8(guchar *src, guint src_len)
{
	gchar* utf_str = NULL;
	iconv_t cd = NULL;
	size_t ileft = 0;
	size_t oleft = 0;

	gchar *pIn = NULL;
	gchar *in_buf = NULL;
	gchar *out_buf = NULL;

	gint ret = 0;

	if (!src) {
		err("src is null");
		return NULL;
	}

	ileft = src_len * 2;//over allocate as utf-8 may occupy 3 bytes
	oleft = src_len * 3;//over allocate as utf-8 may occupy 3 bytes

	pIn = in_buf = (gchar *)tcore_malloc0(ileft + 2);
	memcpy(in_buf, src, ileft);
	in_buf[ileft] = '\0';

	utf_str = out_buf = (gchar *)tcore_malloc0(oleft + 1);

	cd = iconv_open("UTF-8", "UCS-2");
	ret = iconv(cd, (char**) &in_buf, &ileft, &out_buf, &oleft);

	utf_str[src_len * 2 - ileft] = '\0';

	iconv_close(cd);
	tcore_free(pIn);

	return utf_str;
}

static gint __convert_utf8_to_unicode(gushort *dest,
	guchar *src, guint src_len)
{
	gushort *org;
	guchar hi = 0;
	guchar mid = 0;
	guchar low = 0;

	if ((NULL == dest) || (NULL == src)) {
		dbg("INPUT PARAM NULL");
		return -1;
	}
	org = dest;

	while (src_len > 0 && (*src != '\0')) {
		if (*src < 0x80) {
			*dest = (*src & 0x7F);
			dest++;
			src++;
			src_len--;
		} else if (((0xC0 <= *src) && (*src < 0xE0)) && (*(src + 1) >= 0x80)) {
			hi = *src & 0x1F;
			low = *(src+1) & 0x3F;
			*dest = (hi << 6) | low;
			dest++;
			src += 2;
			src_len -= 2;
		} else if ((*src >= 0xE0) && (*(src + 1) >= 0x80) && (*(src + 2) >= 0x80)) {
			hi = *src & 0x0F;
			mid = *(src+1) & 0x3F;
			low = *(src+2) & 0x3F;
			*dest = (hi << 12) | (mid << 6) | low;
			dest++;
			src += 3;
			src_len -= 3;
		} else {
			*dest = (*src & 0x7F);
			dest++;
			src++;
			src_len--;
			err("UTF-8 range incorrect!!!");
		}
	}

	*dest = 0;
	return (dest - org);
}

static gboolean __convert_gsm_to_unicode(gushort *dest, guint dest_len,
	guchar *src, guint src_len)
{
	guint i;
	gint tmp_len;

	if (!dest || !src) {
		dbg("dest(%p) or src(%p) is null",dest, src);
		return FALSE;
	}

	if (!src_len) {
		dest[0] = '\0';
		return TRUE;
	}

	dbg("source string (%s) len(%d)", src, src_len);

	for (i = 0; i < src_len; i++)
		if (src[i] == 0x1B)
			src_len--;

	dbg("strlen excluding escape character (%d)", src_len);

	/* Conversion: GSM --> UCS2 */
	tmp_len = __convert_gsm_to_ucs2(dest, src, src_len);
	if (tmp_len == -1)
		return FALSE;

	dest[tmp_len] = '\0';

	return TRUE;
}

static gboolean __convert_ucs2_to_utf8(gchar *dest, gushort *dest_len,
	gchar *src, gushort src_len)
{
	gchar *p_o = NULL;
	size_t in_len = (size_t)src_len;
	size_t out_len = (size_t)src_len*3;

	iconv_t cd = iconv_open("UTF-8", "UCS2");
	if (cd == (iconv_t) (-1)) {
		perror("iconv_open");
		return FALSE;
	}

	p_o = dest;

	dbg("Expected input bytes: [%d] output bytes: [%d]", in_len, out_len);

	if (iconv(cd, &src, &in_len, &p_o, &out_len) == (size_t)(-1)) {
		dbg("failed to iconv errno:%d", errno);
	} else {
		dbg("remained input bytes:%d processed bytes:%d",
			in_len, in_len*3 - out_len);
		dest[in_len*3 - out_len] = '\0';
	}
	*dest_len = in_len*3 - out_len;

	dbg("Output - Length: [%d] Data: [%s]", *dest_len, dest);
	iconv_close(cd);

	return TRUE;
}

static void __convert_gsm_to_utf8(guchar *dest, gushort *dest_len, guchar *src, guint src_len)
{
	int tmp_len = 0;
	char *target_tmp = NULL;
	guchar *raw_unicode = NULL;
	gushort tmp_dest[TEXT_STRING_LEN_MAX];

	memset(tmp_dest, 0x0, TEXT_STRING_LEN_MAX);

	__convert_gsm_to_unicode(tmp_dest, TEXT_STRING_LEN_MAX, src, src_len);
	while (tmp_dest[tmp_len] != '\0')
		tmp_len++;
	tmp_len++; // add null character

	tmp_len = tmp_len*2; //for byte align

	raw_unicode = (guchar*)tcore_malloc0(tmp_len);
	memcpy(raw_unicode, (guchar*)tmp_dest, tmp_len);

	*dest_len = tmp_len;
	target_tmp = __convert_ucs_to_utf8(raw_unicode, tmp_len);
	if (!target_tmp) {
		dbg("str is NULL");
		tcore_free(raw_unicode);
		return;
	}
	memcpy(dest, target_tmp, strlen((const char*)target_tmp));
	dbg("final utf8 str (%s), length (%d)", dest, tmp_len);

	tcore_free(raw_unicode);
	tcore_free(target_tmp);
}

static void __convert_alpha_field_ucs2_to_utf8(guchar *out, gushort *out_len, guchar *in, gushort in_len)
{
	//input string UNSIGNED CHAR *IN should be encoded with BIG-ENDIAN
	switch (in[0]) {
		case 0x80: {
			guchar num = in_len/2;	//number of characters
			int i = 0;
			int data_loc = 1;	//starting location of data
			gushort* in_buf = NULL;
			dbg("[UCS2] prefix case:[0x80]");
			in_buf = (gushort*)malloc(num * sizeof(gushort));
			for (i=0; i<num; i++,data_loc++)	{
				in_buf[i] = ((gushort)in[data_loc]<<8) + (gushort)in[data_loc+1];
				data_loc++;
				dbg("[UCS2]in_buf[%d]=0x%04x", i, in_buf[i]);
			}
			__convert_ucs2_to_utf8((char*)out, out_len, (char*) in_buf, num*2);
			if (in_buf!=NULL)	free(in_buf);
		} break;

		case 0x81: {
			guchar num = in[1];	//number of characters
			gushort base = (gushort) in[2] << 7;	//base pointer for UCS2 type
			int i = 0;
			int data_loc = 3;	//starting location of data
			gushort* in_buf = NULL;
			dbg("[UCS2] prefix case:[0x81]");
			in_buf = (gushort*)malloc(num * sizeof(gushort));
			for (i=0; i<num; i++,data_loc++)	{
				if (in[data_loc]<0x80) {	// if the MSB is zero (0x80 => 1000b), then remaining 7 bits are GSM default character.
					__convert_gsm_to_ucs2(&in_buf[i], (guchar *)&in[data_loc], 1);
					dbg("[UCS2]in_buf[%d]=0x%04x", i, in_buf[i]);
				} else {	// if the MSB is 1 then the remaining 7 bits are offset value added to Base Pointer which the result defines the UCS2 character.
					in_buf[i] = base + ((gushort)(in[data_loc]) & 0x7F);
					dbg("[UCS2]in_buf[%d]=0x%04x", i, in_buf[i]);
				}
			}
			__convert_ucs2_to_utf8((char*)out, out_len, (char*) in_buf, num*2);
			if (in_buf!=NULL)	free(in_buf);

		} break;

		case 0x82: {
			guchar num = in[1];	//number of characters
			gushort base = ((gushort) in[2] << 8) | (gushort) in[3];	//base pointer for UCS2 type
			int i = 0;
			int data_loc = 4;	//starting location of data
			gushort* in_buf = NULL;
			dbg("[UCS2] prefix case:[0x82]");
			in_buf = (gushort*)malloc(num * sizeof(gushort));
			for (i=0; i<num; i++,data_loc++)	{
				if (in[data_loc]<0x80) {
					__convert_gsm_to_ucs2(&in_buf[i], (guchar *)&in[data_loc], (guint)1);
					dbg("[UCS2]in_buf[%d]=0x%04x", i, in_buf[i]);
				} else {
					in_buf[i] = base + ((gushort)(in[data_loc]) & 0x7F);
					dbg("[UCS2]in_buf[%d]=0x%04x", i, in_buf[i]);
				}
			}
			__convert_ucs2_to_utf8((char*)out, out_len, (char*) in_buf, num*2);
			if (in_buf!=NULL)	free(in_buf);
		} break;

		default: {
			guchar num = in_len/2;	//number of characters
			int i = 0;
			int data_loc = 0;	//starting location of data
			gushort* in_buf = NULL;
			dbg("[UCS2] non-prefix case.");
			in_buf = (gushort*)malloc(num * sizeof(gushort));
			for (i=0; i<num; i++,data_loc++)	{
				in_buf[i] = ((gushort)in[data_loc]<<8) + (gushort)in[data_loc+1];
				data_loc++;
				dbg("[UCS2]in_buf[%d]=0x%04x", i, in_buf[i]);
			}
			__convert_ucs2_to_utf8((char*)out, out_len, (char*) in_buf, num*2);
			if (in_buf!=NULL)	free(in_buf);
		} break;
	}
}

/*
 * Conversion APIs
 */

/* BCD --> */
gchar *tcore_util_convert_bcd_to_ascii(const gchar *bcd_str, guint bcd_str_len,
	guint max_len)
{
	guint index, len = 0;
	gchar l_bcd, h_bcd = 0x0F;
	gchar *ascii_str;

	tcore_check_return_value(bcd_str != NULL, NULL);

	/* validate max length */
	if (bcd_str_len*2 > max_len) {
		err("PARSER - number length exceeds the maximum limit");
		return NULL;
	}

	ascii_str = tcore_malloc0(((bcd_str_len*2)*sizeof(char)) + 1);

	for (index = 0 ; index < bcd_str_len ; index++) {
		l_bcd = bcd_str[index] & 0x0F;
		h_bcd = (bcd_str[index] & 0xF0) >> 0x04;

		/* Lower nibble */
		switch (l_bcd) {
		case 0x0A:
			ascii_str[len++] = '*';
		break;

		case 0x0B:
			ascii_str[len++] = '#';
		break;

		case 0x0C:
			ascii_str[len++] = 'p';	/* Pause */
		break;

		case 0x0D:
			ascii_str[len++] = '?';	/* Wild Card character */
		break;

		case 0x0E:	/* Ignore, RFU */
		case 0x0F:	/* Ignore in l_bcd */
		break;

		default:
			ascii_str[len++] = l_bcd+'0';	/* Digits 0~9 */
		break;
		}

		/* Higher nibble */
		switch (h_bcd) {
		case 0x0A:
			ascii_str[len++] = '*';
		break;

		case 0x0B:
			ascii_str[len++] = '#';
		break;

		case 0x0C:
			ascii_str[len++] = 'p';	/* Pause */
		break;

		case 0x0D:
			ascii_str[len++] = '?';	/* Wild Card character */
		break;

		case 0x0E: //ignore, RFU
		case 0x0F:
			ascii_str[len] = '\0';	/* Null termination */
		break;

		default:
			ascii_str[len++] = h_bcd+'0';	/* Digits 0~9 */
		break;
		}
	}

	/* Append '\0' */
	if (h_bcd != 0x0F)
		ascii_str[len] = '\0';

	dbg("ASCII string: [%s] length: [%d]", ascii_str, len);
	return ascii_str;
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
 * @param[out]	bcd_code - BCD output
 * @param[in]		digits - Digit input
 * @param[in]		digit_len - digit length
 * @Interface		Synchronous.
 * @remark
 * @Refer
 */
void tcore_util_convert_digit_to_bcd(char* bcd_code, char* digits, guint digit_len)
{
	guint i, j, digit;
	unsigned char higher, lower;

	// 0123456789 -> 1032547698
	for (i = 0, j = 0; i < digit_len; i = i + 2, j++) {
		if (digits[i] == '*')
			digit = 0x0A;
		else if (digits[i] == '#')
			digit = 0x0B;
		else if (toupper((int) digits[i]) == 'P')
			digit = 0x0C;
		else if ((digits[i]) == '?')
			digit = 0x0D;
		else
			digit = (guint) digits[i];

		lower = digit & 0x0F;

		if (digit_len != i + 1) {
			if (digits[i + 1] == '*')
				digit = 0x0A;
			else if (digits[i + 1] == '#')
				digit = 0x0B;
			else if (toupper((int) digits[i + 1]) == 'P')
				digit = 0x0C;
			else if (digits[i + 1] == '?')
				digit = 0x0D;
			else
				digit = (guint) digits[i + 1];
			higher = digit & 0x0F;
		} else {
			higher = 0xFF;
		}
		bcd_code[j] = (higher << 4) | lower;
	}
}

/**
 * This function is used to Convert BCD to Digit (BCD to Digit)
 *
 * @return		None
 * @param[out]	digit - Digit output output
 * @param[in]		bcd_code - BCD Input
 * @param[in]		bcd_len - BCD length
 * @Interface		Synchronous.
 * @remark
 * @Refer
 */
guint tcore_util_convert_bcd_to_digit(char *digit, char *bcd_code, guint bcd_len)
{
	guint i, h, l;
	char c[2] = {0, };
	unsigned char higher, lower;

	// 0123456789 <- 1032547698
	memset((void*)digit, 0, bcd_len * 2);

	for (i = 0; i < bcd_len; i++) {
		higher = (bcd_code[i] >> 4) & 0x0F; // get high nibble
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
			h = (guint)higher;
			__acitoa(h, c, 16);
			higher = (char)toupper(*c);
		}

		lower = bcd_code[i] & 0x0F; // get low nibble
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
			l = (guint) lower;
			__acitoa(l, c, 16);
			lower = (char)toupper(*c);
		}

		digit[i * 2] = lower;
		digit[i * 2 + 1] = higher;
	}

	return (bcd_len * 2);
}

/* String --> */
gboolean tcore_util_convert_str_to_utf8(guchar *dest, gushort *dest_len,
	TelUtilAlphabetFormat dcs, const guchar *src, gushort src_len)
{
	tcore_check_return_value(src != NULL, FALSE);
	tcore_check_return_value(src_len != 0, FALSE);
	tcore_check_return_value(dest != NULL, FALSE);
	tcore_check_return_value(dest_len != 0, FALSE);

	switch (dcs) {
	case TEL_UTIL_ALPHABET_FORMAT_SMS_DEFAULT: {
		guchar *unpacked_str = NULL;
		dbg("FORMAT_SMS_DEFAULT");

		/* Convert: GSM to UTF-8 */
		unpacked_str = (guchar*)tcore_util_unpack_gsm7bit((const guchar *)src, src_len);
		if (unpacked_str == NULL) {
			err("Failed to unpack...");
			return FALSE;
		}

		/* Convert: GSM to UTF-8 */
		__convert_gsm_to_utf8(dest, dest_len, unpacked_str, strlen((const char*)unpacked_str));

		tcore_free(unpacked_str);
	}
	break;

	case TEL_UTIL_ALPHABET_FORMAT_8BIT_DATA: {	//GSM7bit with bit 8 set to 0
		int src_buf_len = 0;
		guchar *src_buf;
		dbg("8BIT_DATA");

		src_buf = (guchar*)tcore_memdup(src, src_len);

		/* Get string length */
		/* 0xFF is the end of string */
		while (src[src_buf_len] != 0xFF && src_buf_len < src_len)
			src_buf_len++;

		/* Last space character must be deleted */
		while (src[src_buf_len - 1] == 0x20 && src_buf_len > 0)
			src_buf_len--;

		dbg("src_buf_len: [%d]", src_buf_len);

		/* Convert: GSM to UTF-8 */
		__convert_gsm_to_utf8(dest, dest_len, src_buf, src_buf_len);

		tcore_free(src_buf);
	}
	break;

	case TEL_UTIL_ALPHABET_FORMAT_UCS2: {
		guchar *src_buf;
		dbg("UCS2");

		src_buf = (guchar*)tcore_memdup(src, src_len);

		/* Convert: UCS2 to UTF-8 */
		__convert_alpha_field_ucs2_to_utf8(dest, dest_len, src_buf, src_len);

		tcore_free(src_buf);
	}
	break;

	default: {
		dbg("Unhandled format: [0x%02x]", dcs);
		return FALSE;
	}
	}

	return TRUE;
}

/* UTF8 --> */
gboolean tcore_util_convert_utf8_to_gsm(guchar *dest, guint *dest_len,
	guchar* src, guint src_len)
{
	gushort *unicode = NULL;
	gint gsm_code_len = 0;
	gint unicode_len;

	if (src == NULL || src_len == 0) {
		dbg("WARNING: Invalid Parameter");
		return FALSE;
	}

	unicode = (gushort *)tcore_malloc0((src_len + 1) * sizeof(gushort));

	/* Converting from UTF8 --> UNICODE */
	unicode_len = __convert_utf8_to_unicode(unicode, src, src_len);
	dbg("Unicode length: [%d]", unicode_len);
	if (unicode_len == -1) {
		dbg("UTF8 --> UNICODE conversion fails...");

		tcore_free(unicode);
		return FALSE;
	}

	/* Finding the GSM code size */
	gsm_code_len = __get_gsm_code_size(unicode, (guint)unicode_len);
	dbg("GSM code length: [%d]", gsm_code_len);
	if (gsm_code_len == -1) {
		dbg("SM-DATA is not in GSM7BIT Character Set");

		tcore_free(unicode);
		return FALSE;
	}

	/*Converting from UNICODE --> GSM CODE */
	if (__convert_unicode_to_gsm(dest, (guint)gsm_code_len,
			unicode, (guint)unicode_len) == FALSE) {
		dbg("UNICODE --> GSM conversion fails...");

		tcore_free(unicode);
		return FALSE;
	}
	*dest_len = gsm_code_len;

	tcore_free(unicode);
	return TRUE;
}

gboolean tcore_util_convert_utf8_to_ucs2(guchar *dest, guint *dest_len,
	guchar *src, guint src_len)
{
	gsize byte_converted = 0;
	gsize byte_read = 0;
	gchar* str_converted = NULL;
	GError *error = NULL;
	char tmp_char;
	guint i;

	if (dest == NULL || dest_len == NULL || src == NULL) {
		dbg("Invalid Input Parameter");
		return FALSE;
	}

	/*Converting from UTF8 => UCS-2 using the g_convert*/
	str_converted = (gchar *)g_convert((gchar *)src, (gsize)src_len,
				(gchar*) "UCS-2", (gchar*)"UTF8",
				(gsize*)&byte_read, (gsize*)&byte_converted,
				&error);
	if (str_converted == NULL) {
		err("g_convert failed...");
		if (error != NULL) {
			err("UTF8 --> UCS2 conversion failed "
				"- Error code: [%d]", error->code);
			g_error_free(error);
		}
		return FALSE;
	}

	dbg("src_len[%u], byte_read[%u], byte_converted[%u]",
		src_len, byte_read, byte_converted);
	*dest_len = (guint)byte_converted;

	if (byte_converted % 2 != 0) {
		dbg("String length is wrong!");
	} else {
		for (i = 0; i < (guint)byte_converted; i++) {
			if (i % 2 == 0) {
				tmp_char = str_converted[i];
				str_converted[i] = str_converted[i + 1];
				str_converted[i + 1] = tmp_char;
			}
		}
	}

	memcpy((guchar*)dest, (guchar*)str_converted, byte_converted);

	tcore_free(str_converted);
	return TRUE;
}

/* Packing/Unpacking */
guchar *tcore_util_pack_gsm7bit(const guchar *src, guint src_len)
{
	guchar *dest;
	guint i = 0;
	guint pos = 0, shift = 0;
	guint outlen = 0;

	if (!src || src_len == 0) {
		return NULL;
	}

	outlen = ((src_len * 7) / 8) + 1;

	dest = calloc(outlen + 1, 1);
	if (!dest)
		return NULL;

	for (pos = 0, i = 0; i < src_len; pos++, i++) {
		if (pos >= outlen) {
			free(dest);
			return NULL;
		}

		/* pack the low bits */
		dest[pos] = src[i] >> shift;

		if (i + 1 < src_len) {
			/* pack the high bits using the low bits of the next character */
			dest[pos] |= src[i + 1] << (7 - shift);

			shift++;

			if (shift == 7) {
				shift = 0;
				i++;
			}
		}
		else {
			if (shift == 6)
				dest[pos] |= 0x1a;
		}
	}

	return dest;
}

guchar *tcore_util_unpack_gsm7bit(const guchar *src, guint src_len)
{
	guchar *dest;
	guint pos = 0;
	guchar shift = 0;
	guint outlen = 0;
	guint i = 0;

	tcore_check_return_value(src != NULL, NULL);
	tcore_check_return_value(src_len != 0, NULL);

	outlen = (src_len * 8) / 7;

	dest = tcore_malloc0(outlen + 1);

	for (i = 0; pos < src_len; i++, pos++) {
		dest[i] = (src[pos] << shift) & 0x7F;

		if (pos != 0) {
			/*
			 * Except the first byte, a character contains
			 * some bits from the previous byte.
			 */
			dest[i] |= src[pos - 1] >> (8 - shift);
		}

		shift++;
		if (shift == 7) {
			shift = 0;

			/* a possible extra complete character is available */
			i++;
			dest[i] = src[pos] >> 1;
		}
	}

	/* If a character is '\r' (13), change it to space(32) */
	for (i = 0; i < outlen; i++)
		if (dest[i] == '\r')
			dest[i] = ' ';

	dest[outlen] = '\0';
	dbg("Converted string: [%s] length: [%d]", dest, outlen);

	return dest;
}

/*
 * Utility APIs
 */
gboolean tcore_util_encode_sms_parameters(TelSmsParamsInfo *set_params,
		gchar *set_params_data, guint smsp_record_length)
{
	gint sca_length, param_offset = 0;
	guchar param_indicator;
	guint pid_index = 0;

	/* SMSP Record Length = 28 + Y bytes
	  * 	(Y = alphaid size)
	  * Order of EF-SMSP file
	  * 	Alpha-Identifier
	  *	Parameter Indicators
	  * 	TP-Destination Address
	  * 	TS-Service Centre Address
	  * 	TP-Protocol Identifier
	  * 	TP-Data Coding Scheme
	  * 	TP-Validity Period
	  */

	memset(set_params_data, 0xFF, smsp_record_length);

	if (smsp_record_length >= SMS_DEFAULT_SMSP_WITHOUT_ALPHAID)
		pid_index = smsp_record_length;

	param_indicator = set_params_data[pid_index];

	/* SCA*/
	sca_length = (unsigned)strlen(set_params->sca.number);
	if (sca_length) {
		if (sca_length <= SMS_SMSP_ADDRESS_LEN) {
			dbg("Valid SCA Present");
			param_offset = SMS_SCA_ADDR_OFFSET;

			/* SCA Length */
			set_params_data[pid_index + (param_offset)] =(sca_length + 1);
			/* SCA Type */
			set_params_data[pid_index + (++param_offset)] = ((set_params->sca.ton << 4) |set_params->sca.npi) | 0x80;
			/* Copy SCA */
			memcpy(&(set_params_data[pid_index + (++param_offset)]), &(set_params->sca.number), sca_length + 1);
		} else {
			err("Invalid SCA Length.");
			return FALSE;
		}
	} else {
		dbg("SCA is NULL");
	}
	/*
	  * Validity Period
	  * 	Always send ValidityPeriod to CP
	  * 	becase period could be 0 day.
	  * 	Section: 3GPP TS 23.040 v6.5.0, Section 9.2.3.12.1
	  */
	param_offset = SMS_VP_OFFSET;

	set_params_data[pid_index + (param_offset)] = set_params->vp;

	return TRUE;
}

gboolean tcore_util_decode_sms_parameters(unsigned char *incoming,
	unsigned int length, TelSmsParamsInfo *get_params)
{
	gint i = 0, alpha_id_len = 0, sca_length = 0;
	unsigned char param_indicator;
	guint sca_offset = 0;

	/*
	 * Record length: 28+Y bytes
	 *	1 to Y - Alpha-Identifier
	 */
	alpha_id_len = length - SMS_SMSP_PARAMS_MAX_LEN;
	dbg("Alpha ID length: [%d]", alpha_id_len);
	if (alpha_id_len > 0) {
		if (alpha_id_len > SMS_SMSP_ALPHA_ID_LEN_MAX)
			alpha_id_len = SMS_SMSP_ALPHA_ID_LEN_MAX;

		/* Iterate the loop to get alpha ID */
		for (i = 0; i < alpha_id_len; i++) {
			if (0xff == incoming[i]) {
				dbg("Alpha ID present");
				break;
			}
		}
	}
	else {
		alpha_id_len = 0;
	}

	/* Param Indicators */
	param_indicator = incoming[alpha_id_len];
	dbg("Parameter Indicators: [%02x]", param_indicator);

	/*
	 * Service Centre Address
	 *
	 *	1 to Y Alpha-Identifier - alpha_id_len
	 *	Y+1 Parameter Indicators - 1 byte
	 *	Y+2 to Y+13 TP-Destination Address - SMS_SMSP_DEST_ADDRESS_LEN
	 *	Y+14 to Y+25 TS-Service Centre Address <-- SCA
	 */
	sca_offset = alpha_id_len + 1 + SMS_SCA_ADDR_OFFSET;
	if ((0 == (param_indicator & SMS_SMSP_SCA_ADDRESS))
			&& (0xFF != (int)incoming[sca_offset])) {
		dbg("SCA Address Present");

		if ((incoming[sca_offset] > 0) && (incoming[sca_offset] <= 12)) {
			gchar *decoded_sca;

			sca_length = (int)(incoming[sca_offset] - 1);
			decoded_sca = tcore_util_convert_bcd_to_ascii(&incoming[sca_offset+2],
				sca_length, sca_length*2);
			dbg("Decoded SCA: [%s]", decoded_sca);
			g_strlcpy(get_params->sca.number,
				decoded_sca, strlen(decoded_sca)+1);
			tcore_free(decoded_sca);

			get_params->sca.ton = incoming[++sca_offset] & 0x0f;
			get_params->sca.npi = (incoming[sca_offset] & 0x70) >> 4;
		} else {
			sca_length = 0;
			dbg("SCA Length is 0");
		}
	}else {
		dbg(" SCA Address NOT Present");
		sca_length = 0;
		get_params->sca.number[sca_length] = '\0';
	}
	dbg("SCA - TON: [%d] NPI: [%d] Number: [%s]",
		get_params->sca.ton, get_params->sca.npi, get_params->sca.number);

	/* Validity Period */
	if(((param_indicator & SMS_SMSP_VP) == 0)
			&& (alpha_id_len + SMS_VP_OFFSET) < MAX_GSM_SMS_PARAM_RECORD_SIZE) {
		get_params->vp = incoming[alpha_id_len + SMS_VP_OFFSET];
		dbg("Validity period: [%d]", get_params->vp);
	}

	return TRUE;
}

char tcore_util_unpackb(const char *src, int pos, int len)
{
	char result = 0;
	int rshift = 0;

	src += pos / 8;
	pos %= 8;

	rshift = MAX(8 - (pos + len), 0);

	if (rshift > 0) {
		result = MASK_AND_SHIFT(len, pos, rshift, (unsigned char)*src);
	} else {
		result = MASK(8 - pos, pos, (unsigned char)*src);
		src++;
		len -= 8 - pos;

		if (len > 0) result = (result << len) | (*src >> (8 - len));   // if any bits left
	}

	return result;
}

char tcore_util_convert_byte_hexchar(char val)
{
	char hex_char;

	if (val <= 9) {
		hex_char = (char) (val + '0');
	} else if (val >= 10 && val <= 15) {
		hex_char = (char) (val - 10 + 'A');
	} else {
		hex_char = '0';
	}

	return (hex_char);
}

gboolean tcore_util_byte_to_hex(const char *byte_pdu, char *hex_pdu, int num_bytes)
{
	int i;
	char nibble;
	int buf_pos = 0;

	for (i = 0; i < num_bytes * 2; i++) {
		nibble = tcore_util_unpackb(byte_pdu, buf_pos, 4);
		buf_pos += 4;
		hex_pdu[i] = tcore_util_convert_byte_hexchar(nibble);
	}

	return TRUE;
}

void tcore_util_swap_byte_order(gushort* dest,
	const gushort* src, guint src_len)
{
	guint i = 0;

	for (i = 0; i < src_len; i++)
		dest[i] = (src[i] << 8) + (src[i] >> 8);
}

gchar *tcore_util_get_version(void)
{
	return tcore_strdup(TCORE_VERSION);
}

long tcore_util_encode_hex(const guchar *src,
	long num_bytes, gchar *buf)
{
	long i, j;

	if (num_bytes <= 0)
	return -1;

	for (i = 0, j = 0; i < num_bytes; i++, j++) {
		convert_to_hex(((src[i] >> 4) & 0xf), buf[j++]);
		convert_to_hex((src[i] & 0xf), buf[j]);
	}

	buf[j] = '\0';

	return j;
}

void tcore_util_hex_dump(char *pad, int size, const void *data)
{
	char buf[255] = {0, };
	char hex[4] = {0, };
	int i;
	unsigned char *p;

	if (size <= 0) {
		msg("%sno data", pad);
		return;
	}

	p = (unsigned char *) data;

	snprintf(buf, 255, "%s%04X: ", pad, 0);
	for (i = 0; i < size; i++) {
		snprintf(hex, 4, "%02X ", p[i]);
		strcat(buf, hex);

		if ((i + 1) % 8 == 0) {
			if ((i + 1) % 16 == 0) {
				msg("%s", buf);
				memset(buf, 0, 255);
				snprintf(buf, 255, "%s%04X: ", pad, i + 1);
			} else {
				strcat(buf, "  ");
			}
		}
	}

	msg("%s", buf);
}

guint tcore_util_encode_sca(const TelSmsSca *sca,
	gchar encoded_sca[TEL_UTIL_ENCODED_SCA_LEN_MAX])
{
	gchar *sca_number;
	guint sca_len, encoded_sca_len = 0;
	gchar lower_nibble, higher_nibble;
	guint i = 0, j;

	if (sca == NULL || strlen(sca->number) == 0)
		return encoded_sca_len;

	sca_number = (gchar *)&sca->number[0];
	sca_len = strlen(sca_number);
	if (sca_number[0] == '+') {	/* Remove '+' */
		sca_number = (gchar *)&sca->number[1];
		sca_len--;
	}

	/*
	 * Encoded SCA Length:
	 * number of packed BCD bytes + 1 byte (for TON/NPI byte)
	 */
	if (sca_len % 2)    /* Odd: +1 for padding 0xF */
		encoded_sca_len = (sca_len/2 + 1) + 1;
	else                /* Even */
		encoded_sca_len = (sca_len/2) + 1;

	encoded_sca[0] = encoded_sca_len;

	/*
	 * TON/NPI
	 *
	 *   7   6  5  4  3  2  1  0  - Bits
	 * | 1 |  TON  |     NPI    |
	 */
	encoded_sca[1] = (gchar)(0x80 | ((sca->ton & 0x7) << 4) | (sca->npi & 0x0F));

	/* Encoded SCA */
	for (i = 0, j = 0; i < sca_len; i+=2, j++) {
		/* Lower nibble */
		lower_nibble = (sca_number[i] - '0') & 0x0F;

		/* Higher nibble - In case of Odd SCA length, pad with 0xF */
		if (sca_len != (i+1))
			higher_nibble = (sca_number[i+1] - '0') & 0x0F;
		else
			higher_nibble = 0xFF;

		encoded_sca[j+2] = (higher_nibble << 4) | lower_nibble;
	}

	return encoded_sca_len;
}

guint tcore_util_encode_pdu(const TelSmsSca *sca,
	const guchar *tpdu, guint tpdu_len, gchar *pdu)
{
	guint sca_len = 0;

	pdu[0] = 0;

	/* Encode SCA */
	sca_len = tcore_util_encode_sca(sca, &pdu[0]);

	/*
	 * Copy TPDU
	 * +1 is for sca_len field
	 */
	memcpy(pdu + sca_len + 1, tpdu, tpdu_len);

	/* Encoded SCA length (sca_len + 1) + TPDU length */
	return ((sca_len + 1) + tpdu_len);
}

gchar tcore_util_hexchar_to_int(gchar c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	else if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	else {
		err("Invalid charater!!");
		return -1;
	}
}

gboolean tcore_util_hexstring_to_bytes(gchar *hex_str,
	gchar **bytes, guint *bytes_len)
{
	gchar *byte_str;
	guint hex_str_len;
	guint i;

	if (hex_str == NULL)
		return FALSE;

	hex_str_len = strlen(hex_str);

	byte_str = tcore_try_malloc0((hex_str_len / 2) + 1);

	dbg("Convert String to Binary!!!");
	for (i = 0; i < hex_str_len; i += 2) {
		byte_str[i / 2] = (gchar)((tcore_util_hexchar_to_int(hex_str[i]) << 4)
				| tcore_util_hexchar_to_int(hex_str[i + 1]));
		msg("		[%02x]", byte_str[i / 2]);
	}

	*bytes_len = (hex_str_len / 2);
	*bytes = byte_str;

	return TRUE;
}

/**
 * This function is used to get(decode) string name
 *
 * @return		length of string
 * @param[out]	alpha_id - Alpha string
 * @param[in]		record - Input raw data
 * @param[in]		alpha_id_max_len - Max size of alpha id array
 * @Interface		Synchronous.
 * @remark
 * @Refer
 */
guint tcore_util_get_string(unsigned char* alpha_id, unsigned char* record, guint alpha_id_max_len)
{
	guint i, alpha_id_len = 0;

	memset((void*) alpha_id, 0, alpha_id_max_len);

	if (record[0] == 0xFF)
		return alpha_id_len;

	for (i = 0; i < alpha_id_max_len; i++) {
		if (record[0] <= 0x7F && record[i] == 0xFF)
			break;

		alpha_id[i] = record[i];
		alpha_id_len++;
	}
	return alpha_id_len;
}

/**
 * This function is used to set(encode) string name
 *
 * @return		length of string
 * @param[in]		alpha_id - Alpha string input
 * @param[out]	record - output raw data
 * @param[in]		alpha_id_max_len - Max size of alpha id array
 * @Interface		Synchronous.
 * @remark
 * @Refer
 */
void tcore_util_set_string(unsigned char* record, unsigned char* alpha_id, guint alpha_id_max_len)
{
	guint i;

	tcore_check_return_assert(record != NULL);
	memset((void*) record, 0xFF, alpha_id_max_len);

	for (i = 0; i < alpha_id_max_len; i++)
		record[i] = alpha_id[i];
}

gboolean tcore_util_is_empty(unsigned char* in, guint in_length)
{
	guint i;

	for (i = 0; i < in_length; i++) {
		if (in[i] != 0xFF)
			return FALSE;
	}
	dbg("current index has empty data");
	return TRUE;		// this is empty record.
}

/**
 * This function is used to get BCD length
 *
 * @return		length of string
 * @param[in]		bcd_data - BCD Input data
 * @param[in]		bcd_max_len - BCD Max data Length
 * @Interface		Synchronous.
 * @remark
 * @Refer
 */
guint tcore_util_get_valid_bcd_byte(unsigned char* bcd_data, guint bcd_max_len)
{
	guint i, bcd_length = 0;

	for (i = 0; i < bcd_max_len; i++) {
		if (bcd_data[i] == 0xFF)
			break;

		bcd_length++;
	}
	return bcd_length;
}

TelUtilAlphabetFormat tcore_util_get_cbs_coding_scheme(guchar encode)
{
	TelUtilAlphabetFormat alphabet_format = TEL_UTIL_ALPHABET_FORMAT_SMS_DEFAULT;

	switch (encode & 0xf0)
	{
		case 0x00:
		case 0x20:
		case 0x30:
			alphabet_format = TEL_UTIL_ALPHABET_FORMAT_RESERVED;
			break;

		case 0x10:
			if ((encode & 0x0f) == 0x00)
				alphabet_format = TEL_UTIL_ALPHABET_FORMAT_RESERVED;
			else if ((encode & 0x0f) == 0x01)
				alphabet_format = TEL_UTIL_ALPHABET_FORMAT_8BIT_DATA;
			else
				alphabet_format = TEL_UTIL_ALPHABET_FORMAT_SMS_DEFAULT;
			break;

		case 0x40:
		case 0x50:
		case 0x60:
		case 0x70: // 01xx
			if ((encode & 0x0c) == 0x00)
				alphabet_format = TEL_UTIL_ALPHABET_FORMAT_RESERVED;
			else if ((encode & 0x0c) == 0x04)
				alphabet_format = TEL_UTIL_ALPHABET_FORMAT_8BIT_DATA;
			else if ((encode & 0x0c) == 0x08)
				alphabet_format = TEL_UTIL_ALPHABET_FORMAT_UCS2;
			else if ((encode & 0x0c) == 0x0c)
				alphabet_format = TEL_UTIL_ALPHABET_FORMAT_SMS_DEFAULT;
			break;

		case 0x90: // 1001
			if ((encode & 0x0c) == 0x00)
				alphabet_format = TEL_UTIL_ALPHABET_FORMAT_RESERVED;
			else if ((encode & 0x0c) == 0x04)
				alphabet_format = TEL_UTIL_ALPHABET_FORMAT_8BIT_DATA;
			else if ((encode & 0x0c) == 0x08)
				alphabet_format = TEL_UTIL_ALPHABET_FORMAT_UCS2;
			else if ((encode & 0x0c) == 0x0c)
				alphabet_format = TEL_UTIL_ALPHABET_FORMAT_SMS_DEFAULT;
			break;

		case 0x80: // 1000
		case 0xA0:
		case 0xB0:
		case 0xC0:
		case 0xD0: // 1010 .. 1101
		case 0xE0: // 0x1110
			break;

		case 0xF0:
			if ((encode & 0x04) == 0x00)
				alphabet_format = TEL_UTIL_ALPHABET_FORMAT_RESERVED;
			else if ((encode & 0x04) == 0x04)
				alphabet_format = TEL_UTIL_ALPHABET_FORMAT_8BIT_DATA;
			break;
	}

	return alphabet_format;
}

gchar *tcore_util_get_string_by_ip4type(TcoreIp4Type ip)
{
	gchar *ip_addr_str = NULL;

	ip_addr_str = g_strdup_printf("%d.%d.%d.%d", ip.s[0], ip.s[1], ip.s[2], ip.s[3]);
	dbg("IP Address: [%s]", ip_addr_str);

	return ip_addr_str;
}

TelReturn tcore_util_netif (const char *name, gboolean enabled)
{
	gint ret;
	gint fd;
	struct ifreq ifr;

	if (!name)
		return TEL_RETURN_INVALID_PARAMETER;

	if (strlen(name) > IFNAMSIZ)
		return TEL_RETURN_INVALID_PARAMETER;

	if (enabled == TRUE)
		fd = socket(AF_INET, SOCK_DGRAM, 0);
	else
		fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

	if (fd < 0) {
		return TEL_RETURN_FAILURE;
	}

	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';

	ret = ioctl(fd, SIOCGIFFLAGS, &ifr);
	if (ret < 0) {
		close(fd);
		return TEL_RETURN_FAILURE;
	}

	if (enabled == TRUE)
		ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
	else
		ifr.ifr_flags &= ~(IFF_UP | IFF_RUNNING);

	ret = ioctl(fd, SIOCSIFFLAGS, &ifr);
	if (ret < 0) {
		close(fd);
		return TEL_RETURN_FAILURE;
	}

	close(fd);
	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_util_netif_set(const char *name, const char *ipaddr,
		const char *gateway, const char *netmask)
{
	gint ret;
	gint fd;
	struct ifreq ifr;
	struct sockaddr_in sai;

	if (!name)
		return TEL_RETURN_INVALID_PARAMETER;

	if (strlen(name) > IFNAMSIZ)
		return TEL_RETURN_INVALID_PARAMETER;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		return TEL_RETURN_FAILURE;
	}

	memset(&sai, 0, sizeof(struct sockaddr_in));
	sai.sin_family = AF_INET;
	sai.sin_port = 0;
	memset(&ifr, 0, sizeof(struct ifreq));

	if (ipaddr) {
		dbg("ip = [%s]", ipaddr);
		if (!inet_aton(ipaddr, &sai.sin_addr)) {
			close(fd);
			return TEL_RETURN_FAILURE;
		}

		strncpy(ifr.ifr_name, name, IFNAMSIZ);
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		memcpy(&ifr.ifr_addr, &sai, sizeof(sai));

		ret = ioctl(fd, SIOCSIFADDR, &ifr);
		if (ret < 0) {
			close(fd);
			return TEL_RETURN_FAILURE;
		}
	}

	if (gateway) {
		dbg("gateway = [%s]", gateway);
		if (!inet_aton(gateway, &sai.sin_addr)) {
			close(fd);
			return TEL_RETURN_FAILURE;
		}

		strncpy(ifr.ifr_name, name, IFNAMSIZ);
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		memcpy(&ifr.ifr_dstaddr, &sai, sizeof(sai));

		ret = ioctl(fd, SIOCSIFDSTADDR, &ifr);
		if (ret < 0) {
			close(fd);
			return TEL_RETURN_FAILURE;
		}
	}

	if (netmask) {
		dbg("netmask = [%s]", netmask);
		if (!inet_aton(netmask, &sai.sin_addr)) {
			close(fd);
			return TEL_RETURN_FAILURE;
		}

		strncpy(ifr.ifr_name, name, IFNAMSIZ);
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		memcpy(&ifr.ifr_netmask, &sai, sizeof(sai));

		ret = ioctl(fd, SIOCSIFNETMASK, &ifr);
		if (ret < 0) {
			close(fd);
			return TEL_RETURN_FAILURE;
		}
	}

	close(fd);
	return TEL_RETURN_SUCCESS;
}

#if 0	/* To be UNBLOCKED on requirement basis */

static gboolean		__find_gsm_code_exception_table(gushort src);
static int 		__get_gsm_code_size(gushort* src, int src_len);
static gboolean		__convert_gsm_to_unicode(gushort *dest, int dest_len, guchar *src, guint src_len);
static int			__convert_gsm_to_ucs2(gushort* dest, guchar* src, guint src_len);
static void		__convert_gsm_to_utf8(guchar *dest, gushort *dest_len,	guchar *src, guint src_len);
static gboolean		__convert_unicode_to_gsm(guchar* dest, int dest_len, gushort* src, int src_len);
static char*		__convert_ucs_to_utf8(guchar *src, int src_len);
static int			__convert_ucs2_to_gsm(guchar* dest, gushort* src, guint src_len);
static int			__convert_ucs2_to_utf8(char *out, gushort *out_len, char *in, gushort in_len);
static void		__convert_alpha_field_ucs2_to_utf8(guchar *out, gushort *out_len, guchar *in, gushort in_len);
static int			__convert_utf8_to_unicode(gushort* dest, guchar* src, guint src_len);


static gboolean _tcore_util_marshal_create_gvalue(GValue *value,
		const void *data, TcoreUtilMarshalData type)
{
	switch (type) {
		case TCORE_UTIL_MARSHAL_DATA_CHAR_TYPE:
			g_value_init(value, type);
			g_value_set_schar(value, *((gchar *) data));
			break;

		case TCORE_UTIL_MARSHAL_DATA_BOOLEAN_TYPE:
			g_value_init(value, type);
			g_value_set_boolean(value, *((gboolean *) data));
			break;

		case TCORE_UTIL_MARSHAL_DATA_INT_TYPE:
			g_value_init(value, type);
			g_value_set_int(value, *((gint *) data));
			break;

		case TCORE_UTIL_MARSHAL_DATA_DOUBLE_TYPE:
			g_value_init(value, type);
			g_value_set_double(value, *((gdouble *) data));
			break;

		case TCORE_UTIL_MARSHAL_DATA_STRING_TYPE:
			g_value_init(value, type);
			g_value_set_string(value, (gchar *) data);
			break;

		case TCORE_UTIL_MARSHAL_DATA_OBJECT_TYPE:
			g_value_init(value, G_TYPE_HASH_TABLE);
			g_value_set_boxed(value, (gpointer) data);
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
}


static gboolean _tcore_util_return_value(GValue *src, void **dest,
		TcoreUtilMarshalData type)
{
	switch (type) {
		case TCORE_UTIL_MARSHAL_DATA_CHAR_TYPE:
			*dest = g_new0(gchar, 1);
			*((gchar *) *dest) = g_value_get_schar(src);
			break;

		case TCORE_UTIL_MARSHAL_DATA_BOOLEAN_TYPE:
			*dest = g_new0(gboolean, 1);
			*((gboolean *) *dest) = g_value_get_boolean(src);
			break;

		case TCORE_UTIL_MARSHAL_DATA_INT_TYPE:
			*dest = g_new0(gint, 1);
			*((gint *) *dest) = g_value_get_int(src);
			break;

		case TCORE_UTIL_MARSHAL_DATA_DOUBLE_TYPE:
			*dest = g_new0(gdouble, 1);
			*((gdouble *) *dest) = g_value_get_double(src);
			break;

		case TCORE_UTIL_MARSHAL_DATA_STRING_TYPE:
			*dest = g_value_dup_string(src);
			break;

		case TCORE_UTIL_MARSHAL_DATA_OBJECT_TYPE:
			*dest = g_value_dup_boxed(src);
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
}


static void _tcore_util_marshal_remove_value(gpointer value)
{
	guint gtype = 0;
	GHashTable *ht = NULL;

	gtype = ((GValue *) value)->g_type;
	if (gtype == G_TYPE_HASH_TABLE) {
		ht = g_value_get_boxed(value);
		g_hash_table_destroy(ht);
	}

	g_value_unset((GValue *) value);
	return;
}

static gboolean _tcore_util_marshal__convert_str_to_type(GValue *src,
		GValue *dest, guint dest_type)
{
	if (dest_type == G_TYPE_HASH_TABLE)
		dest_type = G_TYPE_BOXED;

	switch (dest_type) {
		case G_TYPE_INT: {
			gint64 tmp = 0;
			tmp = g_ascii_strtoll(g_value_get_string(src), NULL, 10);
			g_value_set_int(dest, tmp);
		}
			break;
		case G_TYPE_BOOLEAN: {
			gboolean tmp = FALSE;
			tmp = g_ascii_strncasecmp(g_value_get_string(src), "TRUE", 4) == 0
					? TRUE : FALSE;
			g_value_set_boolean(dest, tmp);
		}
			break;
		case G_TYPE_STRING: {
			const gchar* tmp = NULL;
			tmp = g_value_get_string(src);
			g_value_set_string(dest, tmp);
		}
			break;
		case G_TYPE_DOUBLE: {
			gdouble tmp = 0;
			tmp = g_ascii_strtod(g_value_get_string(src), NULL);
			g_value_set_double(dest, tmp);
		}
			break;
		case G_TYPE_BOXED: {
			GHashTable* tmp;
			tmp = tcore_util_marshal_deserialize_string(g_value_get_string(src));
			g_value_set_boxed(dest, tmp);
		}
			break;
		default: {
			return FALSE;
		}
			break;
	}

	return TRUE;
}

TcoreUtilDcs tcore_util_get_cbs_coding_scheme(guchar encode)
{
	TcoreUtilDcs dcs = TCORE_DCS_TYPE_NONE;

	switch (encode & 0xf0)
	{
		case 0x00:
		case 0x20:
		case 0x30:
			dcs = TCORE_DCS_TYPE_7_BIT;
			break;

		case 0x10:
			if ((encode & 0x0f) == 0x00)
				dcs = TCORE_DCS_TYPE_7_BIT;
			else if ((encode & 0x0f) == 0x01)
				dcs = TCORE_DCS_TYPE_8_BIT; //should be re-defined
			else
				dcs = TCORE_DCS_TYPE_UNSPECIFIED;
			break;

		case 0x40:
		case 0x50:
		case 0x60:
		case 0x70: // 01xx
			if ((encode & 0x0c) == 0x00)
				dcs = TCORE_DCS_TYPE_7_BIT;
			else if ((encode & 0x0c) == 0x04)
				dcs = TCORE_DCS_TYPE_8_BIT;
			else if ((encode & 0x0c) == 0x08)
				dcs = TCORE_DCS_TYPE_UCS2;
			else if ((encode & 0x0c) == 0x0c)
				dcs = TCORE_DCS_TYPE_UNSPECIFIED;
			break;

		case 0x90: // 1001
			if ((encode & 0x0c) == 0x00)
				dcs = TCORE_DCS_TYPE_7_BIT;
			else if ((encode & 0x0c) == 0x04)
				dcs = TCORE_DCS_TYPE_8_BIT;
			else if ((encode & 0x0c) == 0x08)
				dcs = TCORE_DCS_TYPE_UCS2;
			else if ((encode & 0x0c) == 0x0c)
				dcs = TCORE_DCS_TYPE_UNSPECIFIED;
			break;

		case 0x80: // 1000
		case 0xA0:
		case 0xB0:
		case 0xC0:
		case 0xD0: // 1010 .. 1101
		case 0xE0: // 0x1110
			break;

		case 0xF0:
			if ((encode & 0x04) == 0x00)
				dcs = TCORE_DCS_TYPE_7_BIT;
			else if ((encode & 0x04) == 0x04)
				dcs = TCORE_DCS_TYPE_8_BIT;
			break;
	}

	return dcs;
}

#define CONVERT_HEXCHAR_TO_INT(h, i) if ((h) >= '0' && (h) <= '9') (i) = (h) - '0'; \
	else if ((h) >= 'A' && (h) <= 'F') (i) = (h) - 'A' + 10; \
	else if ((h) >= 'a' && (h) <= 'f') (i) = (h) - 'a' + 10; \
	else (i) = 0;


guchar *tcore_util_decode_hex(const char *src, int len)
{
	guchar *buf;
	int i = 0;
	int j = 0;
	int out_len = 0;
	int value1 = 0;
	int value2 = 0;

	if (!src)
		return NULL;

	if (len == -1) {
		out_len = strlen(src) / 2 + 1;
	}
	else {
		out_len = len;
	}

	buf = calloc(out_len, 1);
	if (!buf)
		return NULL;

	for (; j < out_len; i+= 2, j++) {
		CONVERT_HEXCHAR_TO_INT(src[i], value1);
		CONVERT_HEXCHAR_TO_INT(src[i+1], value2);

		buf[j] = (value1 << 4) + value2;
	}

	return buf;
}

GHashTable *tcore_util_marshal_create()
{
	GHashTable *ht = NULL;

	ht = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
			_tcore_util_marshal_remove_value);

	return ht;
}

void tcore_util_marshal_destory(GHashTable *ht)
{
	if (!ht)
		return;

	g_hash_table_destroy(ht);
}


GHashTable *tcore_util_marshal_deserialize_string(const gchar *serialized_string)
{
	int index = 0;
	gchar **tuple = NULL;
	GHashTable *ht = NULL;

	ht = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
			_tcore_util_marshal_remove_value);

	if (strlen(serialized_string) == 0) {
		return ht;
	}

	tuple = g_strsplit((gchar *) serialized_string, " ", 0);

	while (strlen(tuple[index]) > 3) {
		int tmp = 0;
		guchar *content = NULL;
		gchar **inner_tuple = NULL;
		GValue *src = g_new0(GValue, 1);
		GValue *dest = g_new0(GValue, 1);
		guint type = 0;

		inner_tuple = g_strsplit(tuple[index], ":", 0);
		type = atoi(inner_tuple[1]);
		content = g_base64_decode(inner_tuple[2], (gsize *)&tmp);

		g_value_init(src, G_TYPE_STRING);
		g_value_init(dest, type);

		g_value_set_string(src, (const gchar *)content);
		_tcore_util_marshal__convert_str_to_type(src, dest, type);
		g_hash_table_insert(ht, tcore_strdup(inner_tuple[0]), dest);

		tcore_free(content);
		g_strfreev(inner_tuple);
		index++;
	}

	g_strfreev(tuple);
	return ht;
}

gchar *tcore_util_marshal_serialize(GHashTable *ht)
{
	gchar *rv_str = NULL;
	GHashTableIter iter;
	gpointer key, value;
	GString *gstring_tmp = NULL;

	gstring_tmp = g_string_new(NULL);
	g_hash_table_iter_init(&iter, ht);
	while (g_hash_table_iter_next(&iter, &key, &value) == TRUE) {
		guint gtype = 0;
		gchar *tmp = NULL, *encoded_d = NULL;
		GValue gval = {	0,{ { 0 } } };

		g_value_init(&gval, G_TYPE_STRING);

		gtype = ((GValue *) value)->g_type;
		if (gtype != G_TYPE_HASH_TABLE) {
			g_value_transform((GValue *) value, &gval);
			tmp = g_value_dup_string(&gval);
		}
		else {
			GHashTable *sub_ht;
			sub_ht = g_value_get_boxed((GValue *) value);
			tmp = tcore_util_marshal_serialize(sub_ht);
		}

		encoded_d = g_base64_encode((guchar *)tmp, (gsize)strlen(tmp));
		tcore_free(tmp);

		g_string_append_printf(gstring_tmp, "%s:%d:%s ", (gchar *)key,
				gtype, encoded_d);
		tcore_free((gpointer)encoded_d);
		g_value_unset(&gval);
	}

	rv_str = tcore_strdup(gstring_tmp->str);
	g_string_free(gstring_tmp, TRUE);

	return rv_str;
}

gboolean tcore_util_marshal_add_data(GHashTable *ht, const gchar *key,
		const void *data, TcoreUtilMarshalData type)
{
	gboolean rv = FALSE;
	GValue *value;

	if (!ht || !key || !data)
		return FALSE;

	if (type >= 0xff)
		return FALSE;

	value = g_new0(GValue, 1);

	rv = _tcore_util_marshal_create_gvalue(value, data, type);

	if (!rv)
		return FALSE;

	g_hash_table_insert(ht, tcore_strdup(key), value);

	return TRUE;
}

gboolean tcore_util_marshal_get_data(GHashTable *ht, const gchar *key,
		void **data, TcoreUtilMarshalData type)
{
	gboolean rv = FALSE;
	gpointer value;

	if (!ht || !key || !data)
		return FALSE;

	value = g_hash_table_lookup(ht, key);

	rv = _tcore_util_return_value((GValue *) value, data, type);
	if (!rv)
		return FALSE;

	return TRUE;
}

gint tcore_util_marshal_get_int(GHashTable *ht, const gchar *key)
{
	gboolean rv = FALSE;
	gint rvalue, *tmp = NULL;

	if (!ht || !key)
		return 0;

	rv = tcore_util_marshal_get_data(ht, key, (void **) &tmp,
			TCORE_UTIL_MARSHAL_DATA_INT_TYPE);
	if (!rv)
		return 0;

	if (!tmp)
		return 0;

	rvalue = *tmp;
	tcore_free(tmp);

	return rvalue;
}

gchar *tcore_util_marshal_get_string(GHashTable *ht, const gchar *key)
{
	gboolean rv = FALSE;
	gchar *rvalue = NULL;

	if (!ht || !key)
		return 0;

	rv = tcore_util_marshal_get_data(ht, key, (void **) &rvalue,
			TCORE_UTIL_MARSHAL_DATA_STRING_TYPE);
	if (!rv)
		return NULL;

	return rvalue;
}

GHashTable *tcore_util_marshal_get_object(GHashTable *ht, const gchar *key)
{
	gboolean rv = FALSE;
	GHashTable *rvalue = NULL;

	if (!ht || !key)
		return 0;

	rv = tcore_util_marshal_get_data(ht, key, (void **) &rvalue,
			TCORE_UTIL_MARSHAL_DATA_OBJECT_TYPE);
	if (!rv)
		return NULL;

	return rvalue;
}
#endif	/* #if 0 */

