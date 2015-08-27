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


#define tabGsmUniMax2	9
#define tabGsmUniMax	42
#define TAB_SPACE	"  "

gboolean tcore_debug = TRUE;

static gboolean _find_gsm_code_exception_table(unsigned short src);
static int _get_gsm_code_size(unsigned short *src, int src_len);
static gboolean _convert_gsm_to_unicode(unsigned short *dest, int dest_len, unsigned char *src, unsigned int src_len);
static int _convert_gsm_to_ucs2(unsigned short *dest, unsigned char *src, unsigned int src_len);
static void _convert_gsm_to_utf8(unsigned char *dest, unsigned short *dest_len, unsigned char *src, unsigned int src_len);
static gboolean _convert_unicode_to_gsm(unsigned char *dest, int dest_len, unsigned short *src, int src_len);
static char *_convert_ucs_to_utf8(unsigned char *src, int src_len);
static int _convert_ucs2_to_gsm(unsigned char *dest, unsigned short *src, unsigned int src_len);
static void _convert_alpha_field_ucs2_to_utf8(unsigned char *out, unsigned short *out_len, unsigned char *in, unsigned short in_len);
static int _convert_utf8_to_unicode(unsigned short *dest, unsigned char *src, unsigned int src_len);

typedef struct {
	char gsm;
	unsigned short unicode;
} GsmUniTable;

const GsmUniTable gsm_unicode2_table[] = {
	{0x14, 0x005E}, {0x28, 0x007B}, {0x29, 0x007D}, {0x2F, 0x005C},
	{0x3C, 0x005B}, {0x3D, 0x007E}, {0x3E, 0x005D}, {0x40, 0x007C},
	{0x65, 0x20AC}
};

const GsmUniTable gsm_unicode_table[] = {
	{0x00, 0x0040}, {0x01, 0x00A3}, {0x02, 0x0024}, {0x03, 0x00A5},
	{0x04, 0x00E8}, {0x05, 0x00E9}, {0x06, 0x00F9}, {0x07, 0x00EC}, {0x08, 0x00F2},
	{0x09, 0x00E7}, {0x0B, 0x00D8}, {0x0C, 0x00F8}, {0x0E, 0x00C5}, {0x0F, 0x00E5},
	{0x10, 0x0394}, {0x11, 0x005F}, {0x12, 0x03A6}, {0x13, 0x0393}, {0x14, 0x039B},
	{0x15, 0x03A9}, {0x16, 0x03A0}, {0x17, 0x03A8}, {0x18, 0x03A3}, {0x19, 0x0398},
	{0x1A, 0x039E}, {0x1C, 0x00C6}, {0x1D, 0x00E6}, {0x1E, 0x00DF}, {0x1F, 0x00C9},
	{0x24, 0x00A4}, {0x40, 0x00A1}, {0x5B, 0x00C4}, {0x5C, 0x00D6}, {0x5D, 0x00D1},
	{0x5E, 0x00DC}, {0x5F, 0x00A7}, {0x60, 0x00BF}, {0x7B, 0x00E4}, {0x7C, 0x00F6},
	{0x7D, 0x00F1}, {0x7E, 0x00FC}, {0x7F, 0x00E0},
};

static gboolean _find_gsm_code_exception_table(unsigned short src)
{
	if ((src >= 0x0020 && src <= 0x0023)
			|| (src >= 0x0025 && src <= 0x003F)
			|| (src >= 0x0041 && src <= 0x005A)
			|| (src >= 0x0061 && src <= 0x007A)
			|| src == 0x000A || src == 0x000D)
		return TRUE;

	return FALSE;
}

static int _get_gsm_code_size(unsigned short *src, int src_len)
{
	gboolean in_table = FALSE;
	gboolean in_sec_table = FALSE;
	int i, gsm_len = 0;

	if (NULL == src) {
		dbg("INPUT PARAM was NULL");
		return -1;
	}

	for (; src_len > 0 && src; src_len--) {
		if (_find_gsm_code_exception_table(*src) == TRUE) {
			src++;
			gsm_len++;

			continue;
		}

		in_table = FALSE;
		for (i = 0; i < tabGsmUniMax; i++) {
			if (*src == gsm_unicode_table[i].unicode) {
				src++;
				in_table = TRUE;
				gsm_len++;

				break;
			}
		}

		if (in_table == FALSE) {
			in_sec_table = FALSE;
			for (i = 0; i < tabGsmUniMax2; i++) {/* second table */
				if (*src == gsm_unicode2_table[i].unicode) {
					src++;
					in_sec_table = TRUE;
					gsm_len += 2;

					break;
				}
			}

			if (in_sec_table == FALSE) {/* second*/
				if (_find_gsm_code_exception_table(*src) == FALSE) {
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

static gboolean _convert_gsm_to_unicode(unsigned short *dest, int dest_len, unsigned char *src, unsigned int src_len)
{
	int count, tmp_len;

	if (!dest || !src) {
		dbg("dest(%p) or src(%p) is null", dest, src);
		return FALSE;
	}

	if (!src_len) {
		dest[0] = '\0';
		return TRUE;
	}

	dbg("source string (%s) len(%d)", src, src_len);

	for (count = 0; count < (int)src_len; count++) {
		if (src[count] == 0x1B)
			src_len--;
	}

	dbg("strlen excluding escape character (%d)", src_len);

	tmp_len = _convert_gsm_to_ucs2(dest, src, src_len);
	dest[tmp_len] = '\0';

	return TRUE;
}

static int _convert_gsm_to_ucs2(unsigned short *dest, unsigned char *src, unsigned int src_len)
{
	int count;
	unsigned short *org;

	org = dest;

	for (count = 0; count < (int)src_len; count++) {
		int table_index = 0;
		gboolean b_tabled = FALSE;

		/*
		 * if the first byte is 0x1B, it is the escape character.
		 * The byte value shoulbe be changed to unicode.
		 */
		if (*src == 0x1B) {
			src++; count++; /* move to next byte */
			for (table_index = 0; table_index < tabGsmUniMax2; table_index++) {
				if (*src == gsm_unicode2_table[table_index].gsm) {
					*dest = gsm_unicode2_table[table_index].unicode;
					b_tabled = TRUE;
					break;
				}
			}

			/* if matched data is not in table, it should be changed to NULL; */
			if (!b_tabled)
				*dest = 0x0020;
		} else {
			for (table_index = 0; table_index < tabGsmUniMax; table_index++) {
				if (*src == gsm_unicode_table[table_index].gsm) {
					*dest = gsm_unicode_table[table_index].unicode;
					b_tabled = TRUE;
					break;
				}
			}

			/* if matched data is not in table, it is using original value; */
			if (!b_tabled)
				*dest = *src;
		}

		/* move to next position */
		src++; dest++;
	}

	dbg("cvt sr(%s), the size of data (%d) ", org, dest - org);
	return (dest - org);
}

static void _convert_gsm_to_utf8(unsigned char *dest, unsigned short *dest_len, unsigned char *src, unsigned int src_len)
{
	unsigned short tmp_len = 0;
	char *target_tmp = NULL;
	unsigned char *raw_unicode = NULL;
	unsigned short tmp_dest[SAT_TEXT_STRING_LEN_MAX];

	memset(tmp_dest, 0 , SAT_TEXT_STRING_LEN_MAX);

	_convert_gsm_to_unicode(tmp_dest, SAT_TEXT_STRING_LEN_MAX, src, src_len);
	while (tmp_dest[tmp_len] != '\0')
		tmp_len++;
	tmp_len++; /* add null character */

	tmp_len = tmp_len * 2; /* for byte align */
	raw_unicode = (unsigned char *)malloc(tmp_len);
	if (!raw_unicode) {
		*dest_len = 0;
		return;
	}

	memset(raw_unicode, 0, tmp_len);

	memcpy(raw_unicode, (unsigned char *)tmp_dest, tmp_len);

	target_tmp = _convert_ucs_to_utf8(raw_unicode, tmp_len);
	if (!target_tmp) {
		dbg("str is NULL");
		g_free(raw_unicode);
		return;
	}

	*dest_len = strlen((const char *)target_tmp);
	dbg("utf8 (%s), len(%d)", (const char *)target_tmp, strlen((const char *)target_tmp));
	memcpy(dest, target_tmp, strlen((const char *)target_tmp));
	dbg("final utf8 str (%s), length (%d)", dest, tmp_len);

	g_free(raw_unicode);
	g_free(target_tmp);
	return;
}

static gboolean _convert_unicode_to_gsm(unsigned char *dest, int dest_len, unsigned short *src, int src_len)
{
	char *tmp_str;
	int gc_len = 0;

	if ((NULL == dest) || (NULL == src)) {
		dbg("INPUT PARAM was NULL");
		return FALSE;
	}

	if (src_len == 0)
		return FALSE;

	gc_len = _get_gsm_code_size(src, src_len);
	if (0 >= gc_len) {
		dbg("Warning: Error[%d] while finding the GSM Code Size", gc_len);
		return FALSE;
	}

	if (dest_len < gc_len) {
		if (dest_len == sizeof(void *))
			dbg("Out buffer size seems to be small (%s)", dest);
		else
			dbg("Buffer size is too small (%s): dest_len(%d), gc_len(%d)", dest, dest_len, gc_len);

		return FALSE;
	}

	tmp_str = calloc(1, (unsigned short) gc_len);
	if (tmp_str == NULL) {
		dbg("Memory Allocation Failed!");
		return FALSE;
	}

	gc_len = _convert_ucs2_to_gsm((unsigned char *) tmp_str, src, src_len);
	if (gc_len != -1) {
		memcpy((char *) dest, (char *) tmp_str, gc_len);
		free(tmp_str);
		return TRUE;
	}

	free(tmp_str);
	return FALSE;
}

static char *_convert_ucs_to_utf8(unsigned char *src, int src_len)
{
	char *utf_str = NULL;
	iconv_t cd = NULL;
	size_t ileft = 0;
	size_t oleft = 0;

	char *pIn = NULL;
	char *in_buf = NULL;
	char *out_buf = NULL;

	if (!src) {
		dbg("src is null");
		return NULL;
	}

	ileft = src_len * 2; /* over allocate as utf-8 may occupy 3 bytes */
	oleft = src_len * 3; /* over allocate as utf-8 may occupy 3 bytes */
	pIn = in_buf = (char *) malloc(ileft + 2);
	if (in_buf == NULL) {
		dbg("in_buf allocation failed");
		return NULL;
	}

	utf_str = out_buf = (char *) malloc(oleft + 1);
	if (utf_str == NULL) {
		dbg("in_buf allocation failed");
		free(in_buf);
		return NULL;
	}

	memset(in_buf, 0x00, ileft + 2);
	memset(out_buf, 0x00, oleft + 1);
	memcpy(in_buf, src, ileft);

	in_buf[ileft] = '\0';

	cd = iconv_open("UTF-8", "UCS-2");

	if (iconv(cd, (char **) &in_buf, &ileft, &out_buf, &oleft) == (size_t)(-1))
		dbg("failed to iconv errno:%d", errno);
	else
		utf_str[src_len * 2 - ileft] = '\0';

	iconv_close(cd);
	free(pIn);

	return utf_str;
}

static int _convert_ucs2_to_gsm(unsigned char *dest, unsigned short *src, unsigned int src_len)
{
	unsigned char *rear = NULL;
	unsigned short *p;
	unsigned char temp;
	gboolean in_table = FALSE;
	gboolean in_sec_table = FALSE;
	int i, gc_len = 0;

	if ((!dest) || (!src) || (0x00 == src_len)) {
		dbg("Warning: Wrong Input");
		return -1;
	}

	rear = dest;
	p = src;

	for (; src_len > 0 && p; src_len--) {
		in_table = FALSE;
		for (i = 0; i < tabGsmUniMax; i++) {/* is in table  */
			if (*p == gsm_unicode_table[i].unicode) {
				temp = (unsigned char) (gsm_unicode_table[i].gsm);
				*rear = temp;
				rear++;
				p++;
				in_table = TRUE;
				gc_len++;

				break;
			}
		}

		if (in_table == FALSE) {
			in_sec_table = FALSE;
			for (i = 0; i < tabGsmUniMax2; i++) { /* second table*/
				if (*p == gsm_unicode2_table[i].unicode) {
					*rear = 0x1B;
					rear++;
					temp = (unsigned char) (gsm_unicode2_table[i].gsm);
					*rear = temp;
					rear++;
					p++;
					in_sec_table = TRUE;
					gc_len += 2;

					break;
				}
			}

			if (in_sec_table == FALSE) {/* second */
				if (_find_gsm_code_exception_table(*p) == FALSE)
					return -1;

				temp = (unsigned char) (*p); /* isn't in table. but it's just able to be converted to GSM (0x00?? -> 0x??)*/
				*rear = temp;
				rear++;
				p++;
				gc_len++;
			}
		}
	}

	src = p;
	return gc_len;
}

static unsigned char _convert_hexchar_to_int(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	else if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	else {
		dbg("invalid charater!!");
		return -1;
	}
}

char *tcore_util_convert_hexstring_to_bytes(char *s)
{
	char *ret;
	int i;
	int sz;

	if (s == NULL)
		return NULL;

	sz = strlen(s);

	ret = g_try_malloc0((sz / 2) + 1);
	if (ret == NULL)
		return NULL;

	dbg("Convert String to Binary!!");

	for (i = 0; i < sz; i += 2)
		ret[i / 2] = (char)((_convert_hexchar_to_int(s[i]) << 4) | _convert_hexchar_to_int(s[i + 1]));

	return ret;
}

int tcore_util_convert_ucs2_to_utf8(char *out, unsigned short *out_len,
	char *in, unsigned short in_len)
{
	/* input string "char *in" should be BIG-ENDIAN format. */
	gsize byte_converted = 0;
	gsize byte_read = 0;
	gchar *str_converted = NULL;

	if (NULL == out || NULL == out_len || NULL == in) {
		dbg("Invalid Input Parameter");
		return 0;
	}

try_again:
	str_converted = (gchar *)g_convert((const gchar *)in, (gssize)in_len,
			"UTF8", "UCS-2BE", &byte_read, &byte_converted, NULL);

	dbg("read:[%d] converted:[%d] out:[%s]", byte_read, byte_converted, str_converted);

	*out_len = byte_converted;
	if (str_converted) {
		memcpy(out, str_converted, byte_converted);
		g_free(str_converted);
	} else {
		warn("Cannot get converted data");
		if (byte_read > 0) {
			in_len = byte_read;
			goto try_again;
		}
	}
	return 0;
}

static void _convert_alpha_field_ucs2_to_utf8(unsigned char *out, unsigned short *out_len,
	unsigned char *in, unsigned short in_len)
{
	/* input string "unsigned char *in" should be BIG-ENDIAN format. */
	int i = 0;

	switch (in[0]) {
	case 0x80: {
		dbg("[UCS2] prefix case:[0x80]");
		tcore_util_hex_dump(" [UCS2] ", in_len, in);
		tcore_util_convert_ucs2_to_utf8((char *)out, out_len, (char *)in+1, in_len-1);
	}
	break;

	case 0x81: {
		unsigned char num = in[1];	/* number of characters */
		unsigned short base = (unsigned short) in[2] << 7; /* base pointer for UCS2 type */
		int data_loc = 3;	/* starting location of data */
		unsigned short *in_buf = NULL;
		dbg("[UCS2] prefix case:[0x81]");
		in_buf = (unsigned short *)malloc(num * sizeof(unsigned short));
		if (in_buf == NULL) {
			dbg("in_buf malloc failed.");
			return;
		}

		for (i = 0; i < num; i++, data_loc++)	{
			if (in[data_loc] < 0x80) {
				/*
				 * if the MSB is zero (0x80 => 1000b), then remaining 7 bits
				 * are GSM default character.
				 */
				_convert_gsm_to_ucs2(&in_buf[i], &in[data_loc], 1);
				dbg("[UCS2]in_buf[%d]=0x%04x", i, in_buf[i]);
			} else {
				/*
				 * if the MSB is 1 then the remaining 7 bits are offset value
				 * added to Base Pointer which the result defines the UCS2 character.
				 */
				in_buf[i] = base + ((unsigned short)(in[data_loc]) & 0x7F);
				dbg("[UCS2]in_buf[%d]=0x%04x", i, in_buf[i]);
			}
		}
		{
			unsigned char *dest = NULL;
			dest = (unsigned char *)malloc(num*2);
			if (dest == NULL) {
				dbg("dest malloc failed.");
				free(in_buf);
				return;
			}

			tcore_util_swap_byte_order(dest, (unsigned char *)in_buf, num*2);
			tcore_util_convert_ucs2_to_utf8((char *)out, out_len, (char *) dest, num*2);

			free(in_buf);
			free(dest);
		}
	}
	break;

	case 0x82: {
		unsigned char num = in[1];	/* number of characters */
		unsigned short base = ((unsigned short) in[2] << 8) | (unsigned short) in[3]; /* base pointer for UCS2 type */
		int data_loc = 4;	/* starting location of data */
		unsigned short *in_buf = NULL;
		dbg("[UCS2] prefix case:[0x82]");
		in_buf = (unsigned short *)malloc(num * sizeof(unsigned short));
		if (in_buf == NULL) {
			dbg("in_buf malloc failed.");
			return;
		}

		for (i = 0; i < num; i++, data_loc++)	{
			if (in[data_loc] < 0x80) {
				_convert_gsm_to_ucs2(&in_buf[i], &in[data_loc], (unsigned int)1);
				dbg("[UCS2]in_buf[%d]=0x%04x", i, in_buf[i]);
			} else {
				in_buf[i] = base + ((unsigned short)(in[data_loc]) & 0x7F);
				dbg("[UCS2]in_buf[%d]=0x%04x", i, in_buf[i]);
			}
		}
		{
			unsigned char *dest = NULL;
			dest = (unsigned char *)malloc(num*2);
			if (dest == NULL) {
				dbg("dest malloc failed.");
				free(in_buf);
				return;
			}

			tcore_util_swap_byte_order(dest, (unsigned char *)in_buf, num*2);
			tcore_util_convert_ucs2_to_utf8((char *)out, out_len, (char *) dest, num*2);

			free(in_buf);
			free(dest);
		}
	}
	break;

	default: {
		dbg("[UCS2] non-prefix case.");
		tcore_util_hex_dump(" [UCS2] ", in_len, in);
		tcore_util_convert_ucs2_to_utf8((char *)out, out_len, (char *)in, in_len);
	}
	break;
	}
}

static int _convert_utf8_to_unicode(unsigned short *dest, unsigned char *src, unsigned int src_len)
{
	unsigned short *org = NULL;
	unsigned char hi = 0;
	unsigned char mid = 0;
	unsigned char low = 0;

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
			dbg("utf8 incorrect range");
		}
	}

	*dest = 0;
	return (dest - org);
}

static char _convert_gsm7bit_extension(char c)
{
	switch (c) {
	case 0x0A:
	case 0x1B:
		return ' ';

	case 0x14:
		return '^';

	case 0x28:
		return '{';

	case 0x29:
		return '}';

	case 0x2F:
		return '\\';

	case 0x3C:
		return '[';

	case 0x3D:
		return '~';

	case 0x3E:
		return ']';

	case 0x40:
		return '|';

	/*
	case 0x65:
	   This code represents the EURO currency symbol.
	   The code value is that used for the character ‘e’.
	   Therefore a receiving entity which is incapable of displaying
	   the EURO currency symbol will display the character ‘e’ instead. GSM 03.38
	*/
	case 0x65:
		return 'e';

	default:
		dbg("this is not extension character : (0x%x)", c);
	break;
	}

	return c;
}

gboolean tcore_util_convert_utf8_to_gsm(unsigned char *dest, int *dest_len,
	unsigned char *src, int src_len)
{
	unsigned short *uc = NULL;
	int gc_len = 0;
	int uc_len = 0;

	if (src == NULL || src_len == 0) {
		dbg("WARNING: Invalid Parameter");
		return FALSE;
	}

	uc = (unsigned short *)calloc(src_len + 1, sizeof(unsigned short));
	if (!uc) {
		err("Memory allocation failed");
		return FALSE;
	}

	/* Converting from UTF8 => UNICODE */
	uc_len = _convert_utf8_to_unicode(uc, src, src_len);
	dbg("uc_len:[%d]", uc_len);
	if (uc_len == -1) {
		err("_convert_utf8_to_unicode returns false!");
		free(uc);
		return FALSE;
	}

	/*Finding the GSMCode Size*/
	gc_len = _get_gsm_code_size(uc, uc_len);
	dbg("gc_len:[%d]", gc_len);
	if (gc_len == -1) {
		err("SM- DATA is not in GSM7BIT Character Set & Error:[%d]", gc_len);
		free(uc);
		return FALSE;
	}

	*dest_len = gc_len;

	/* Converting from UNICODE ==> GSM CODE */
	if (_convert_unicode_to_gsm((unsigned char *) dest, *dest_len, uc, uc_len) == FALSE) {
		err("_convert_unicode_to_gsm Failed");
		*dest_len = 0x00;
		free(uc);
		return FALSE;
	}

	if (uc)
		free(uc);

	return TRUE;
}

gboolean tcore_util_convert_utf8_to_ucs2(char **dest, int *dest_len,
	unsigned char *src, int src_len)
{
	gsize byte_converted = 0;
	gsize byte_read = 0;
	gchar *str_converted = NULL;

	if (NULL == src || NULL == dest || NULL == dest_len) {
		dbg("Invalid Input Parameter");
		return FALSE;
	}

try_again:
	/* Converting from UTF8 => UCS-2 BIG-ENDIAN FORMAT using the g_convert */
	str_converted = (gchar *)g_convert((const gchar *)src, (gssize)src_len,
			"UCS-2BE", "UTF8", &byte_read, &byte_converted, NULL);

	dbg("byte_read: [%d] byte_converted: [%d]", byte_read, byte_converted);
	if (str_converted) {
		*dest = str_converted;
		*dest_len = byte_converted;
	} else {
		warn("Cannot get converted string");
		if (byte_read > 0) {
			src_len = byte_read;
			goto try_again;
		}
	}

	return TRUE;
}

gboolean tcore_util_convert_string_to_utf8(unsigned char *dest, unsigned short *dest_len,
	enum alphabet_format dcs, const unsigned char *src, unsigned short src_len)
{
	dbg("dcs=[0x%02x], src=[%s], src_len=[%d]", dcs, src, src_len);

	if (src == NULL || src_len == 0) {
		err("src is NULL or src_len is 0");
		return FALSE;
	}

	switch (dcs) {
	case ALPHABET_FORMAT_SMS_DEFAULT: {
		unsigned char *tmp_dest_str = NULL;
		tmp_dest_str = (unsigned char *)tcore_util_unpack_gsm7bit((const unsigned char *)src, src_len);

		if (!tmp_dest_str) {
			err("temp_dest_str is NULL");
			return FALSE;
		}
		_convert_gsm_to_utf8(dest, dest_len, tmp_dest_str, strlen((const char *)tmp_dest_str));

		if (tmp_dest_str)
			free(tmp_dest_str);
	}
	break;

	case ALPHABET_FORMAT_8BIT_DATA: { /* GSM7bit with bit 8 set to 0 */
		int tmp_str_len = 0;
		unsigned char *src_buf = NULL;

		src_buf = (unsigned char *)malloc(src_len);
		if (src_buf == NULL) {
			dbg("src_buf malloc failed.");
			return FALSE;
		}

		memset(src_buf, 0, src_len);
		memcpy(src_buf, src, src_len);

		/*
		 * Get string length
		 * '0xFF' is the end of string
		 */
		while (src[tmp_str_len] != 0xFF && tmp_str_len < src_len)
			tmp_str_len++;

		/* last space character must be deleted */
		while (src[tmp_str_len - 1] == 0x20 && tmp_str_len > 0)
			tmp_str_len--;

		dbg("tmp_str_len[%d]", tmp_str_len);

		_convert_gsm_to_utf8(dest, dest_len, src_buf, tmp_str_len);
		if (src_buf != NULL)
			free(src_buf);
	}
	break;

	case ALPHABET_FORMAT_UCS2: {
		unsigned char *src_buf = NULL;

		src_buf = (unsigned char *)malloc(src_len);
		if (src_buf == NULL) {
			dbg("src_buf malloc failed.");
			return FALSE;
		}

		memset(src_buf, 0, src_len);
		memcpy(src_buf, src, src_len);

		_convert_alpha_field_ucs2_to_utf8(dest, dest_len, src_buf, src_len);
		if (src_buf != NULL)
			free(src_buf);
	}
	break;

	default: {
		dbg("not handled alpha format[0x%02x]", dcs);
		return FALSE;
	}
	}

	return TRUE;
}

gboolean tcore_util_convert_ascii_to_utf8(unsigned char **dest, int *dest_len,
	unsigned char *src, int src_len)
{
	int i = 0, j = 0, tmp_len = 0;
	unsigned char *tmp = 0;

	if ((!dest) || (!src) || (!dest_len))
		return FALSE;

	if (!src_len)
		src_len = strlen((char *)src);

	tmp_len = (src_len * 2);
	tmp = g_new0(unsigned char, tmp_len);

	for (i = 0, j = 0; i < src_len; i++, j++) {
		if (src[i] <= 0x7F)  {
			tmp[j] = src[i];
		} else if ((src[i] >= 0x80) && (src[i] <= 0xBF)) {
			tmp[j] = 0xC2;
			tmp[++j] = src[i];
		} else { /* ((src[i] >= 0xC0) && (src[i] <= 0xFF)) */
			tmp[j] = 0xC3;
			tmp[++j] = (src[i] - 0x40);
		}
	}

	*dest_len = (j+1);

	*dest = g_new0(unsigned char, *dest_len);
	memcpy(*dest, tmp, j);

	g_free(tmp);

	return TRUE;
}

void tcore_util_swap_byte_order(unsigned char *dest,
	const unsigned char *src, int src_len)
{
	int i = 0;
	for (i = 0; i+1 < src_len; i = i+2) {
		dest[i] = src[i+1];
		dest[i+1] = src[i];
	}

	if (src_len%2 == 1)
		dest[i-1] = src[i-1];

	dbg("swap completed.");
}

static gboolean _tcore_util_marshal_create_gvalue(GValue *value,
	const void *data, enum tcore_util_marshal_data_type type)
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
	}

	return TRUE;
}


static gboolean _tcore_util_return_value(GValue *src, void **dest,
	enum tcore_util_marshal_data_type type)
{
	switch (type) {
	case TCORE_UTIL_MARSHAL_DATA_CHAR_TYPE:
		*dest = g_new0(gchar, 1);
		*((gchar *)*dest) = g_value_get_schar(src);
	break;

	case TCORE_UTIL_MARSHAL_DATA_BOOLEAN_TYPE:
		*dest = g_new0(gboolean, 1);
		*((gboolean *)*dest) = g_value_get_boolean(src);
	break;

	case TCORE_UTIL_MARSHAL_DATA_INT_TYPE:
		*dest = g_new0(gint, 1);
		*((gint *)*dest) = g_value_get_int(src);
	break;

	case TCORE_UTIL_MARSHAL_DATA_DOUBLE_TYPE:
		*dest = g_new0(gdouble, 1);
		*((gdouble *)*dest) = g_value_get_double(src);
	break;

	case TCORE_UTIL_MARSHAL_DATA_STRING_TYPE:
		if (src)
			*dest = g_value_dup_string(src);
	break;

	case TCORE_UTIL_MARSHAL_DATA_OBJECT_TYPE:
		*dest = g_value_dup_boxed(src);
	break;

	default:
		return FALSE;
	}

	return TRUE;
}


static void _tcore_util_marshal_remove_value(gpointer value)
{
	unsigned int gtype = 0;
	GHashTable *ht = NULL;

	gtype = ((GValue *) value)->g_type;
	if (gtype == G_TYPE_HASH_TABLE) {
		ht = g_value_get_boxed(value);
		g_hash_table_destroy(ht);
	}

	g_value_unset((GValue *)value);
	g_free(value);

	return;
}

static gboolean _tcore_util_marshal_convert_str_to_type(GValue *src,
	GValue *dest, unsigned int dest_type)
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
		const gchar *tmp = NULL;
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
		GHashTable *tmp;
		tmp = tcore_util_marshal_deserialize_string(g_value_get_string(src));
		g_value_set_boxed(dest, tmp);
	}
	break;

	default: {
		return FALSE;
	}
	}

	return TRUE;
}

TReturn tcore_util_netif_up(const char *name)
{
	int ret;
	int fd;
	struct ifreq ifr;

	if (!name)
		return TCORE_RETURN_EINVAL;

	if (strlen(name) > IFNAMSIZ)
		return TCORE_RETURN_EINVAL;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
		return TCORE_RETURN_FAILURE;

	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';

	ret = ioctl(fd, SIOCGIFFLAGS, &ifr);
	if (ret < 0) {
		close(fd);
		return TCORE_RETURN_FAILURE;
	}

	ifr.ifr_flags |= IFF_UP | IFF_RUNNING;

	ret = ioctl(fd, SIOCSIFFLAGS, &ifr);
	if (ret < 0) {
		close(fd);
		return TCORE_RETURN_FAILURE;
	}

	close(fd);

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_util_netif_down(const char *name)
{
	int ret;
	int fd;
	struct ifreq ifr;

	if (!name)
		return TCORE_RETURN_EINVAL;

	if (strlen(name) > IFNAMSIZ)
		return TCORE_RETURN_EINVAL;

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (fd < 0)
		return TCORE_RETURN_FAILURE;

	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';


	ret = ioctl(fd, SIOCGIFFLAGS, &ifr);
	if (ret < 0) {
		close(fd);
		return TCORE_RETURN_FAILURE;
	}

	ifr.ifr_flags &= ~(IFF_UP | IFF_RUNNING);

	ret = ioctl(fd, SIOCSIFFLAGS, &ifr);
	if (ret < 0) {
		close(fd);
		return TCORE_RETURN_FAILURE;
	}

	close(fd);
	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_util_netif_set(const char *name,
	const char *ipaddr, const char *gateway, const char *netmask)
{
	int ret;
	int fd;
	struct ifreq ifr;
	struct sockaddr_in sai;

	if (!name)
		return TCORE_RETURN_EINVAL;

	if (strlen(name) > IFNAMSIZ)
		return TCORE_RETURN_EINVAL;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
		return TCORE_RETURN_FAILURE;

	memset(&sai, 0, sizeof(struct sockaddr_in));
	sai.sin_family = AF_INET;
	sai.sin_port = 0;
	memset(&ifr, 0, sizeof(struct ifreq));

	if (ipaddr) {
		dbg("ip = [%s]", ipaddr);
		if (!inet_aton(ipaddr, &sai.sin_addr)) {
			close(fd);
			return TCORE_RETURN_FAILURE;
		}

		strncpy(ifr.ifr_name, name, IFNAMSIZ);
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		memcpy(&ifr.ifr_addr, &sai, sizeof(sai));

		ret = ioctl(fd, SIOCSIFADDR, &ifr);
		if (ret < 0) {
			close(fd);
			return TCORE_RETURN_FAILURE;
		}
	}

	if (gateway) {
		dbg("gateway = [%s]", gateway);
		if (!inet_aton(gateway, &sai.sin_addr)) {
			close(fd);
			return TCORE_RETURN_FAILURE;
		}

		strncpy(ifr.ifr_name, name, IFNAMSIZ);
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		memcpy(&ifr.ifr_dstaddr, &sai, sizeof(sai));

		ret = ioctl(fd, SIOCSIFDSTADDR, &ifr);
		if (ret < 0) {
			close(fd);
			return TCORE_RETURN_FAILURE;
		}
	}

	if (netmask) {
		dbg("netmask = [%s]", netmask);
		if (!inet_aton(netmask, &sai.sin_addr)) {
			close(fd);
			return TCORE_RETURN_FAILURE;
		}

		strncpy(ifr.ifr_name, name, IFNAMSIZ);
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		memcpy(&ifr.ifr_netmask, &sai, sizeof(sai));

		ret = ioctl(fd, SIOCSIFNETMASK, &ifr);
		if (ret < 0) {
			close(fd);
			return TCORE_RETURN_FAILURE;
		}
	}

	close(fd);
	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_util_netif_set_arp(const char *name, gboolean disable)
{
	int ret;
	int fd;
	struct ifreq ifr;

	if (!name)
		return TCORE_RETURN_EINVAL;

	if (strlen(name) > IFNAMSIZ)
		return TCORE_RETURN_EINVAL;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
		return TCORE_RETURN_FAILURE;

	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';

	ret = ioctl(fd, SIOCGIFFLAGS, &ifr);
	if (ret < 0) {
		close(fd);
		return TCORE_RETURN_FAILURE;
	}

	if (disable)
		ifr.ifr_flags |= IFF_NOARP;
	else
		ifr.ifr_flags &= ~IFF_NOARP;

	ret = ioctl(fd, SIOCSIFFLAGS, &ifr);
	if (ret < 0) {
		close(fd);
		return TCORE_RETURN_FAILURE;
	}

	close(fd);
	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_util_reset_ipv4_socket(const char *name, const char *ipaddr)
{
	int ret;
	int fd;
	struct ifreq ifr;
	struct sockaddr_in sai;

	if (!name)
		return TCORE_RETURN_EINVAL;

	if (strlen(name) > IFNAMSIZ)
		return TCORE_RETURN_EINVAL;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
		return TCORE_RETURN_FAILURE;

	memset(&sai, 0, sizeof(struct sockaddr_in));
	sai.sin_family = AF_INET;
	sai.sin_port = 0;
	if (ipaddr) {
		if (!inet_aton(ipaddr, &sai.sin_addr)) {
			dbg("fail to inet_aton()");
			close(fd);
			return TCORE_RETURN_FAILURE;
		}
	}

	memset(&ifr, 0, sizeof(struct ifreq));
	memcpy(&ifr.ifr_addr, &sai, sizeof(sai));
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';

	if (!ipaddr) {
		ret = ioctl(fd, SIOCGIFADDR, &ifr);
		if (ret < 0) {
			dbg("fail to ioctl[SIOCGIFADDR]!!!");
			close(fd);
			return TCORE_RETURN_FAILURE;
		}
	}

	/* SIOCKILLADDR is initially introduced in Android OS. */
#ifndef SIOCKILLADDR
#define SIOCKILLADDR    0x8939
#endif
	ret = ioctl(fd, SIOCKILLADDR, &ifr);
	if (ret < 0) {
		dbg("fail to ioctl[SIOCKILLADDR]!!!");
		close(fd);
		return TCORE_RETURN_FAILURE;
	}
	dbg("devname: [%s], ipaddr: [%s]", name, ipaddr);

	close(fd);
	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_util_netif_set_mtu(const char *name, unsigned int mtu)
{
	int ret;
	int fd;
	struct ifreq ifr;

	if (!name)
		return TCORE_RETURN_EINVAL;

	if (strlen(name) > IFNAMSIZ)
		return TCORE_RETURN_EINVAL;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
		return TCORE_RETURN_FAILURE;

	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_data = GUINT_TO_POINTER(mtu);

	ret = ioctl(fd, SIOCSIFMTU, &ifr);
	if (ret < 0) {
		dbg("fail to ioctl[SIOCSIFMTU]!!!");
		close(fd);
		return TCORE_RETURN_FAILURE;
	}

	close(fd);
	return TCORE_RETURN_SUCCESS;
}

char *tcore_util_get_string_by_ip4type(union tcore_ip4_type ip)
{
	char buf[16]; /* 'nnn'*4 + '.'*3 + '\0' */

	snprintf(buf, 16, "%d.%d.%d.%d", ip.s[0], ip.s[1], ip.s[2], ip.s[3]);

	return strdup(buf);
}

char *tcore_util_get_string_by_ip6type(unsigned char ipv6_addr_type[IPV6_ADDR_LEN])
{
	char buf[INET6_ADDRSTRLEN] = {0,};

	if (inet_ntop(AF_INET6, ipv6_addr_type, buf, INET6_ADDRSTRLEN) == NULL)
		return NULL;

	return g_strdup(buf);
}

enum tcore_dcs_type tcore_util_get_cbs_coding_scheme(unsigned char encode)
{
	enum tcore_dcs_type dcs = TCORE_DCS_TYPE_NONE;

	switch (encode & 0xf0) {
	case 0x00:
	case 0x20:
	case 0x30:
		dcs = TCORE_DCS_TYPE_7_BIT;
	break;

	case 0x10:
		if ((encode & 0x0f) == 0x00)
			dcs = TCORE_DCS_TYPE_7_BIT;
		else if ((encode & 0x0f) == 0x01)
			dcs = TCORE_DCS_TYPE_8_BIT; /* should be re-defined */
		else
			dcs = TCORE_DCS_TYPE_UNSPECIFIED;
	break;

	case 0x40:
	case 0x50:
	case 0x60:
	case 0x70: /* 01xx */
		if ((encode & 0x0c) == 0x00)
			dcs = TCORE_DCS_TYPE_7_BIT;
		else if ((encode & 0x0c) == 0x04)
			dcs = TCORE_DCS_TYPE_8_BIT;
		else if ((encode & 0x0c) == 0x08)
			dcs = TCORE_DCS_TYPE_UCS2;
		else if ((encode & 0x0c) == 0x0c)
			dcs = TCORE_DCS_TYPE_UNSPECIFIED;
	break;

	case 0x90: /* 1001 */
		if ((encode & 0x0c) == 0x00)
			dcs = TCORE_DCS_TYPE_7_BIT;
		else if ((encode & 0x0c) == 0x04)
			dcs = TCORE_DCS_TYPE_8_BIT;
		else if ((encode & 0x0c) == 0x08)
			dcs = TCORE_DCS_TYPE_UCS2;
		else if ((encode & 0x0c) == 0x0c)
			dcs = TCORE_DCS_TYPE_UNSPECIFIED;
	break;

	case 0x80: /* 1000 */
	case 0xA0:
	case 0xB0:
	case 0xC0:
	case 0xD0: /* 1010 .. 1101 */
	case 0xE0: /* 0x1110 */
	break;

	case 0xF0:
		if ((encode & 0x04) == 0x00)
			dcs = TCORE_DCS_TYPE_7_BIT;
		else if ((encode & 0x04) == 0x04)
			dcs = TCORE_DCS_TYPE_8_BIT;
	break;

	default:
		err("invalid encoding type");
	break;
	}

	return dcs;
}

unsigned char *tcore_util_decode_hex(const char *src, int len)
{
	unsigned char *buf;
	int i = 0;
	int j = 0;
	int out_len = 0;
	int value1 = 0;
	int value2 = 0;

	if (!src)
		return NULL;

	if (len == -1)
		out_len = strlen(src) / 2 + 1;
	else
		out_len = len;

	buf = calloc(1, out_len);
	if (!buf)
		return NULL;

	for (; j < out_len; i += 2, j++) {
		CONVERT_HEXCHAR_TO_INT(src[i], value1);
		CONVERT_HEXCHAR_TO_INT(src[i+1], value2);

		buf[j] = (value1 << 4) + value2;
	}

	return buf;
}

void tcore_util_hex_dump(const char *pad, int size, const void *data)
{
	char buf[255] = {0,};
	char hex[4] = {0,};
	int i = 0;
	unsigned char *p = NULL;

	if (0 >= size) {
		msg("%sno data", pad);
		return;
	}

	p = (unsigned char *)data;

	g_snprintf(buf, 255, "%s%04X: ", pad, 0);
	for (i = 0; i < size; i++) {
		g_snprintf(hex, 4, "%02X ", p[i]);
		memcpy(buf+strlen(buf), hex, 4);

		if ((i + 1) % 8 == 0) {
			if ((i + 1) % 16 == 0) {
				dbg("%s", buf);
				memset(buf, 0, 255);
				snprintf(buf, 255, "%s%04X: ", pad, i + 1);
			} else
				strncat(buf, TAB_SPACE, strlen(TAB_SPACE));
		}
	}

	dbg("%s", buf);
}

unsigned char *tcore_util_unpack_gsm7bit(const unsigned char *src, unsigned int src_len)
{
	unsigned char *dest;
	int i = 0, j = 0;
	unsigned int pos = 0;
	unsigned char shift = 0;
	int outlen = 0;

	if (!src || src_len == 0)
		return NULL;

	outlen = (src_len * 8) / 7;

	dest = calloc(1, outlen + 1);
	if (!dest)
		return NULL;

	for (i = 0; pos < src_len; i++, pos++) {
		dest[i] = (src[pos] << shift) & 0x7F;

		if (pos != 0) {
			/*
			 * except the first byte, a character contains some bits
			 * from the previous byte.
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

	/* If a character is '\r'(13), change it to space(32) */
	for (i = 0, j = 0; i < outlen; i++, j++) {
		if (dest[i] == '\r')
			dest[j] = ' ';
		else if (dest[i] == 0x1B)  /* If a charater is 0x1B (escape), next charater is changed on gsm 7bit extension table */
			dest[j] = _convert_gsm7bit_extension(dest[++i]);
		else
			dest[j] = dest[i];
	}

	outlen -= (i - j);

	dest[outlen] = '\0';

	return dest;
}

unsigned char *tcore_util_pack_gsm7bit(const unsigned char *src, unsigned int src_len)
{
	unsigned char *dest;
	unsigned int i = 0;
	unsigned int pos = 0, shift = 0;
	unsigned int outlen = 0;

	if (!src || src_len == 0)
		return NULL;

	outlen = ((src_len * 7) / 8) + 1;

	dest = calloc(1, outlen + 1);
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
		} else {
			if (shift == 6)
				dest[pos] |= 0x1a;
		}
	}

	return dest;
}

char *tcore_util_convert_bcd_str_2_ascii_str(const char *src, int src_len)
{
	int count = 0;
	char *dest = NULL;

	if (!src)
		return NULL;

	dest = malloc((src_len+1)*sizeof(char));
	if (!dest)
		return NULL;

	memset(dest, 0, (src_len+1)*sizeof(char));

	for (count = 0; count < src_len; count++) {
		switch (src[count]) {
		case 0x0A:
			dest[count] = '*';
		break;

		case 0x0B:
			dest[count] = '#';
		break;

		case 0x0C:
			dest[count] = 'p'; /* Pause */
		break;

		case 0x0D:
			dest[count] = '?'; /* Wild Card character */
		break;

		case 0x0E: /* ignore, RFU */
		case 0x0F:
			dest[count] = '\0'; /* Null termination */
		break;

		default:
			dest[count] = src[count]+'0'; /* digits 0~9 */
		break;
		}
	}

	dest[count] = '\0';
	dbg("PARSER - number(%s) len(%d)", dest, strlen(dest));

	return dest;
}

char *tcore_util_convert_bcd2ascii(const char *src, int src_len, int max_len)
{
	int count = 0, len = 0;
	char l_bcd = 0x00, h_bcd = 0x0F;
	char *dest = NULL;

	if (!src)
		return NULL;

	if (src_len*2 > max_len) {
		err("PARSER - number length exceeds the max");
		return NULL;
	}

	dest = malloc((src_len*2)*sizeof(char)+1);
	if (!dest)
		return NULL;

	memset(dest, 0, (src_len*2)*sizeof(char)+1);

	for (count = 0; count < src_len; count++) {
		l_bcd = src[count] & 0x0F;
		h_bcd = (src[count] & 0xF0) >> 0x04;

		switch (l_bcd) {
		case 0x0A:
			dest[len++] = '*';
		break;

		case 0x0B:
			dest[len++] = '#';
		break;

		case 0x0C:
			dest[len++] = 'p'; /* Pause */
		break;

		case 0x0D:
			dest[len++] = '?'; /* Wild Card character */
		break;

		case 0x0E: /* ignore, RFU */
		case 0x0F: /* ignore in l_bcd */
		break;

		default:
			dest[len++] = l_bcd+'0'; /* digits 0~9 */
		break;
		} /* l_lbcd switch */

		switch (h_bcd) {
		case 0x0A:
			dest[len++] = '*';
		break;

		case 0x0B:
			dest[len++] = '#';
		break;

		case 0x0C:
			dest[len++] = 'p'; /* Pause */
		break;

		case 0x0D:
			dest[len++] = '?'; /* Wild Card character */
		break;

		case 0x0E: /* ignore, RFU */
		case 0x0F:
			dest[len] = '\0'; /* Null termination */
		break;

		default:
			dest[len++] = h_bcd+'0'; /*digits 0~9 */
		break;
		} /* h_bcd switch */
	}

	if (h_bcd != 0x0F)
		dest[len] = '\0';

	dbg("PARSER - number(%s) len(%d)", dest, len);
	return dest;
}

char *tcore_util_convert_digit2ascii(const char *src, int src_len)
{
	int count = 0;
	char *dest = NULL;

	if (!src)
		return NULL;

	dest = malloc((src_len+1)*sizeof(char));
	if (!dest)
		return NULL;

	memset(dest, 0, (src_len+1)*sizeof(char));

	for (count = 0; count < src_len; count++)
		dest[count] = src[count] + '0';
	dest[count] = '\0';

	dbg("PARSER - number(%s) len(%d)", dest, strlen(dest));
	return dest;
}

GHashTable *tcore_util_marshal_create()
{
	GHashTable *ht = NULL;

	ht = g_hash_table_new_full(g_str_hash, g_str_equal,
		g_free, _tcore_util_marshal_remove_value);

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
	int count = 0;
	gchar **tuple = NULL;
	GHashTable *ht = NULL;
	GValue src = G_VALUE_INIT;

	if (!serialized_string)
		return NULL;

	ht = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
			_tcore_util_marshal_remove_value);

	if (strlen(serialized_string) == 0)
		return ht;

	g_value_init(&src, G_TYPE_STRING);

	tuple = g_strsplit(serialized_string, " ", 0);

	while (strlen(tuple[count]) > 3) {
		int tmp = 0;
		guchar *content = NULL;
		gchar **inner_tuple = NULL;
		GValue *dest = g_new0(GValue, 1);
		unsigned int type = 0;

		inner_tuple = g_strsplit(tuple[count], ":", 0);
		type = atoi(inner_tuple[1]);
		content = g_base64_decode(inner_tuple[2], (gsize *)&tmp);

		g_value_init(dest, type);

		g_value_set_string(&src, (const gchar *)content);
		_tcore_util_marshal_convert_str_to_type(&src, dest, type);
		g_hash_table_insert(ht, g_strdup(inner_tuple[0]), dest);

		g_value_reset(&src);
		g_free(content);
		g_strfreev(inner_tuple);
		count++;
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
		unsigned int gtype = 0;
		gchar *tmp = NULL, *encoded_d = NULL;
		GValue gval = {0, {{0} } };

		g_value_init(&gval, G_TYPE_STRING);

		gtype = ((GValue *) value)->g_type;
		if (gtype != G_TYPE_HASH_TABLE) {
			g_value_transform((GValue *) value, &gval);
			tmp = g_value_dup_string(&gval);
		} else {
			GHashTable *sub_ht;
			sub_ht = g_value_get_boxed((GValue *) value);
			tmp = tcore_util_marshal_serialize(sub_ht);
		}

		encoded_d = g_base64_encode((guchar *)tmp, (gsize)strlen(tmp));
		g_free(tmp);

		g_string_append_printf(gstring_tmp, "%s:%d:%s ",
			(gchar *)key, gtype, encoded_d);
		g_free((gpointer)encoded_d);
		g_value_unset(&gval);
	}

	rv_str = g_strdup(gstring_tmp->str);
	g_string_free(gstring_tmp, TRUE);

	return rv_str;
}

gboolean tcore_util_marshal_add_data(GHashTable *ht, const gchar *key,
		const void *data, enum tcore_util_marshal_data_type type)
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

	g_hash_table_insert(ht, g_strdup(key), value);

	return TRUE;
}

gboolean tcore_util_marshal_get_data(GHashTable *ht, const gchar *key,
		void **data, enum tcore_util_marshal_data_type type)
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

	rv = tcore_util_marshal_get_data(ht, key,
		(void **)&tmp, TCORE_UTIL_MARSHAL_DATA_INT_TYPE);
	if (!rv)
		return 0;

	if (!tmp)
		return 0;

	rvalue = *tmp;
	g_free(tmp);

	return rvalue;
}

gchar *tcore_util_marshal_get_string(GHashTable *ht, const gchar *key)
{
	gboolean rv = FALSE;
	gchar *rvalue = NULL;

	if (!ht || !key)
		return 0;

	rv = tcore_util_marshal_get_data(ht, key,
		(void **)&rvalue, TCORE_UTIL_MARSHAL_DATA_STRING_TYPE);
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

	rv = tcore_util_marshal_get_data(ht, key,
		(void **)&rvalue, TCORE_UTIL_MARSHAL_DATA_OBJECT_TYPE);
	if (!rv)
		return NULL;

	return rvalue;
}

char *tcore_util_get_version(void)
{
	return strdup(TCORE_VERSION);
}

void tcore_util_set_log(gboolean enable)
{
	tcore_debug = enable;
}

gboolean tcore_util_is_country_NA(char *plmn)
{
	if (!plmn)
		return FALSE;

	if (!strncmp(plmn, "310", 3)
			|| !strncmp(plmn, "311", 3)
			|| !strncmp(plmn, "312", 3)
			|| !strncmp(plmn, "313", 3)
			|| !strncmp(plmn, "314", 3)
			|| !strncmp(plmn, "315", 3)
			|| !strncmp(plmn, "316", 3)
			|| !strncmp(plmn, "302", 3))
		return TRUE;

	return FALSE;
}
