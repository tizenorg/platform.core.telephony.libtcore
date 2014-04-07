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

#ifndef __UTIL_H__
#define __UTIL_H__

#include <glib-object.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Allocates 'n_bytes' bytes of memory and initialized to 0's to allocated memory.
 * If 'n_bytes' is 0 it returns NULL(in this case caller has to perfom sanity check for NULL),
 * else it aborts the program if unable to allocate requested memory.
 */
#define tcore_malloc0(n_bytes) g_malloc0(n_bytes)
/*
 * same as tcore_malloc0 but it does not initialized to 0's to allocated memory
 */
#define tcore_malloc(n_bytes) g_malloc(n_bytes)
/*
 * Attempts to allocate n_bytes and initialized to 0's to allocated memory.
 * It returns NULL if unable to allocate memory,unlike 'tcore_malloc0' which aborts on failure.
 */
#define tcore_try_malloc0(n_bytes) g_try_malloc0(n_bytes)

#define tcore_try_malloc(n_bytes) g_try_malloc(n_bytes)

#define tcore_memdup(mem, byte_size) g_memdup(mem, byte_size)
#define tcore_free(p) g_free(p)
#define tcore_strdup(str) g_strdup(str)

/* If Condition fails it assert if G_DISABLE_ASSERT is enabled, else it will return 'ret_val' */
#define tcore_check_return_value_assert(cond, ret_val) \
	do { \
		if(!(cond)) { \
			err("*** Condition (%s) Fails ***", #cond); \
			g_assert(cond); \
			return ret_val; \
		} \
	} while (0)

/* Same as 'tcore_assert_with_return_value' except return value 'ret_val' */
#define tcore_check_return_assert(cond) \
	do { \
		if(!(cond)) { \
			err("*** Condition (%s) Fails ***", #cond); \
			g_assert(cond); \
			return ; \
		} \
	} while (0)

/* If condition fails it retuns 'ret_val' without asserting program  */
#define tcore_check_return_value(cond, ret_val) \
	do { \
		if(!(cond)) { \
			err("*** Condition (%s) Fails ***", #cond); \
			return ret_val; \
		} \
	} while (0)

#define tcore_check_return(cond) \
	do { \
		if(!(cond)) { \
			err("*** Condition (%s) Fails ***", #cond); \
			return ; \
		} \
	} while (0)

#define bitsize(type) (sizeof(type) * 8)

#define copymask(type) ((0xffffffff) >> (32 - bitsize(type)))

#define MASK(width, offset, data) \
	(((width) == bitsize(data)) ? (data) :	 \
	 ((((copymask(data) << (bitsize(data) - ((width) % bitsize(data)))) & copymask(data)) >> (offset)) & (data))) \


#define MASK_AND_SHIFT(width, offset, shift, data)	\
	((((signed) (shift)) < 0) ?		  \
	 MASK((width), (offset), (data)) << -(shift) :	\
	 MASK((width), (offset), (data)) >> (((signed) (shift)))) \

#define TEL_UTIL_ENCODED_SCA_LEN_MAX	12

typedef enum {
	TEL_UTIL_ALPHABET_FORMAT_SMS_DEFAULT = 0x00, /**< Alphabet type -SMS default format */
	TEL_UTIL_ALPHABET_FORMAT_8BIT_DATA = 0x01, /**< Alphabet type -8 bit format data */
	TEL_UTIL_ALPHABET_FORMAT_UCS2 = 0x02, /**< Alphabet type -UCS2 format */
	TEL_UTIL_ALPHABET_FORMAT_RESERVED = 0x03 /**< Alphabet type -format reserved */
} TelUtilAlphabetFormat;

typedef union {
	guint i;
	guchar s[4];
} TcoreIp4Type;

/*
 * Conversion APIs
 */

/* BCD --> */
gchar *tcore_util_convert_bcd_to_ascii(const gchar *bcd_str, guint bcd_str_len,
	guint max_len);
void tcore_util_convert_digit_to_bcd(char* bcd_code, char* digits, guint digit_len);
guint tcore_util_convert_bcd_to_digit(char* digit, char* bcd_code, guint bcd_len);

/* String --> */
gboolean tcore_util_convert_str_to_utf8(guchar *dest, gushort *dest_len,
	TelUtilAlphabetFormat dcs, const guchar *src, gushort src_len);

/* UTF8 --> */
gboolean tcore_util_convert_utf8_to_gsm(guchar *dest, guint *dest_len,
	guchar* src, guint src_len);
gboolean	 tcore_util_convert_utf8_to_ucs2(guchar *dest, guint *dest_len,
	guchar *src, guint src_len);

/* Packing/Unpacking */
guchar *tcore_util_pack_gsm7bit(const guchar *src, guint src_len);
guchar *tcore_util_unpack_gsm7bit(const guchar *src, guint src_len);

/*
 * Utility APIs
 */
gchar *tcore_util_get_version(void);
void tcore_util_hex_dump(gchar *pad, int size, const void *data);

void tcore_util_swap_byte_order(gushort *dest,
	const gushort* src, guint src_len);

long tcore_util_encode_hex(const guchar *src,
	long num_bytes, gchar *buf);

guint tcore_util_encode_pdu(const TelSmsSca *sca,
	const guchar *tpdu, guint tpdu_len, gchar *pdu);
guint tcore_util_encode_sca(const TelSmsSca *sca,
	gchar encoded_sca[TEL_UTIL_ENCODED_SCA_LEN_MAX]);

gchar tcore_util_hexchar_to_int(gchar c);
gboolean tcore_util_hexstring_to_bytes(gchar *hex_str, gchar **bytes, guint *bytes_len);
guint tcore_util_get_string(unsigned char* alpha_id, unsigned char* record, guint alpha_id_max_len);
void tcore_util_set_string(unsigned char* record, unsigned char* alpha_id, guint alpha_id_max_len);
gboolean tcore_util_is_empty(unsigned char* in, guint in_length);
guint tcore_util_get_valid_bcd_byte(unsigned char* bcd_data, guint bcd_max_len);

gchar *tcore_util_get_string_by_ip4type(TcoreIp4Type ip);
gboolean tcore_util_decode_sms_parameters(unsigned char *incoming,
	unsigned int length, TelSmsParamsInfo *get_params);

TelUtilAlphabetFormat tcore_util_get_cbs_coding_scheme(guchar encode);

TelReturn tcore_util_netif(const char *name, gboolean enabled);

TelReturn tcore_util_netif_set(const char *name, const char *ipaddr,
		const char *gateway, const char *netmask);

gboolean tcore_util_byte_to_hex(const char *byte_pdu, char *hex_pdu, int num_bytes);
char tcore_util_convert_byte_hexchar(char val);
char tcore_util_unpackb(const char *src, int pos, int len);

gboolean tcore_util_encode_sms_parameters(TelSmsParamsInfo *set_params,
	gchar *set_params_data, guint length);

#if 0	/* To be UNBLOCKED on requirement basis */
typedef enum {
	TCORE_UTIL_MARSHAL_DATA_CHAR_TYPE = G_TYPE_CHAR,
	TCORE_UTIL_MARSHAL_DATA_BOOLEAN_TYPE = G_TYPE_BOOLEAN,
	TCORE_UTIL_MARSHAL_DATA_INT_TYPE = G_TYPE_INT,
	TCORE_UTIL_MARSHAL_DATA_DOUBLE_TYPE = G_TYPE_DOUBLE,
	TCORE_UTIL_MARSHAL_DATA_STRING_TYPE = G_TYPE_STRING,
	TCORE_UTIL_MARSHAL_DATA_OBJECT_TYPE = G_TYPE_BOXED,
	TCORE_UTIL_MARSHAL_DATA_STRING_MAX = 0xFF,
} TcoreUtilMarshalData;

typedef enum {
	TCORE_DCS_TYPE_NONE = 0xff,
	TCORE_DCS_TYPE_7_BIT = 0x00,
	TCORE_DCS_TYPE_8_BIT = 0x04,
	TCORE_DCS_TYPE_UCS2 = 0x08,
	TCORE_DCS_TYPE_UNSPECIFIED = 0x0F,
} TcoreUtilDcs;

TelReturn tcore_util_netif(const char *name, gboolean enabled);
TelReturn tcore_util_netif_set(const char *name,
	const char *ipaddr, const char *gateway, const char *netmask);

GHashTable *tcore_util_marshal_create();
void tcore_util_marshal_destory(GHashTable *ht);

GHashTable *tcore_util_marshal_deserialize_string(const gchar *serialized_string);
gchar *tcore_util_marshal_serialize(GHashTable *ht);

gboolean tcore_util_marshal_add_data(GHashTable *ht, const gchar *key,
	const void *data, TcoreUtilMarshalData type);
gboolean tcore_util_marshal_get_data(GHashTable *ht, const gchar *key,
	void **data, TcoreUtilMarshalData type);

gint tcore_util_marshal_get_int(GHashTable *ht, const gchar *key);
gchar *tcore_util_marshal_get_string(GHashTable *ht, const gchar *key);
GHashTable *tcore_util_marshal_get_object(GHashTable *ht, const gchar *key);

TcoreUtilDcs tcore_util_get_cbs_coding_scheme(guchar encode);

guchar *tcore_util_decode_hex(const char *src, int len);
#endif	/* #if 0 */

#ifdef __cplusplus
}
#endif

#endif	/* __UTIL_H__ */
