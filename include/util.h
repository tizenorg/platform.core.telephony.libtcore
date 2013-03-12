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

#ifndef __TCORE_UTIL_H__
#define __TCORE_UTIL_H__

#include <glib-object.h>

__BEGIN_DECLS


enum tcore_util_marshal_data_type {
	TCORE_UTIL_MARSHAL_DATA_CHAR_TYPE = G_TYPE_CHAR,
	TCORE_UTIL_MARSHAL_DATA_BOOLEAN_TYPE = G_TYPE_BOOLEAN,
	TCORE_UTIL_MARSHAL_DATA_INT_TYPE = G_TYPE_INT,
	TCORE_UTIL_MARSHAL_DATA_DOUBLE_TYPE = G_TYPE_DOUBLE,
	TCORE_UTIL_MARSHAL_DATA_STRING_TYPE = G_TYPE_STRING,
	TCORE_UTIL_MARSHAL_DATA_OBJECT_TYPE = G_TYPE_BOXED,
	TCORE_UTIL_MARSHAL_DATA_STRING_MAX = 0xFF,
};


union tcore_ip4_type {
	uint32_t i;
	unsigned char s[4];
};

enum tcore_dcs_type {
	TCORE_DCS_TYPE_NONE = 0xff,
	TCORE_DCS_TYPE_7_BIT = 0x00,
	TCORE_DCS_TYPE_8_BIT = 0x04,
	TCORE_DCS_TYPE_UCS2 = 0x08,
	TCORE_DCS_TYPE_UNSPECIFIED = 0x0F,
};

TReturn     tcore_util_netif_up(const char *name);
TReturn     tcore_util_netif_down(const char *name);
TReturn     tcore_util_netif_set(const char *name, const char *ipaddr,
                const char *gateway, const char *netmask);

char*       tcore_util_get_string_by_ip4type(union tcore_ip4_type ip);

GHashTable* tcore_util_marshal_create();
void        tcore_util_marshal_destory(GHashTable *ht);

GHashTable* tcore_util_marshal_deserialize_string(const gchar *serialized_string);
gchar*      tcore_util_marshal_serialize(GHashTable *ht);

gboolean    tcore_util_marshal_add_data(GHashTable *ht, const gchar *key,
                const void *data, enum tcore_util_marshal_data_type type);
gboolean    tcore_util_marshal_get_data(GHashTable *ht, const gchar *key,
                void **data, enum tcore_util_marshal_data_type type);

gint        tcore_util_marshal_get_int(GHashTable *ht, const gchar *key);
gchar*      tcore_util_marshal_get_string(GHashTable *ht, const gchar *key);
GHashTable* tcore_util_marshal_get_object(GHashTable *ht, const gchar *key);

enum tcore_dcs_type
            tcore_util_get_cbs_coding_scheme(unsigned char encode);

unsigned char* tcore_util_decode_hex(const char *src, int len);
long tcore_util_encode_hex(const unsigned char *src, long num_bytes,
				char *buf);

int tcore_util_pdu_encode(const unsigned char *sca, const unsigned char *tpdu,
				int tpdu_len, char *pdu);

unsigned char*		tcore_util_unpack_gsm7bit(const unsigned char *src, unsigned int src_len);
unsigned char*		tcore_util_pack_gsm7bit(const unsigned char *src, unsigned int src_len);
char*				tcore_util_convert_bcd2ascii(const char *src, int src_len, int max_len);
gboolean			tcore_util_convert_utf8_to_gsm(unsigned char *dest, int *dest_len, unsigned char* src, int src_len);
gboolean			tcore_util_convert_utf8_to_ucs2(unsigned char* dest, int* dest_len,	unsigned char* src, int src_len);
gboolean			tcore_util_convert_string_to_utf8(unsigned char *dest, unsigned short *dest_len,
						enum alphabet_format dcs, const unsigned char *src, unsigned short src_len);
void				tcore_util_swap_byte_order(unsigned short* dest, const unsigned short* src, int src_len);

char*          tcore_util_get_version(void);

__END_DECLS

#endif
