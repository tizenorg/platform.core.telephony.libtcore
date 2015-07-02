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

#define CONVERT_HEXCHAR_TO_INT(h, i) do { \
	if ((h) >= '0' && (h) <= '9') \
		(i) = (h) - '0'; \
	else if ((h) >= 'A' && (h) <= 'F') \
		(i) = (h) - 'A' + 10; \
	else if ((h) >= 'a' && (h) <= 'f') \
		(i) = (h) - 'a' + 10; \
	else \
		(i) = 0; \
} while (0)

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

TReturn tcore_util_netif_up(const char *name);
TReturn tcore_util_netif_down(const char *name);
TReturn tcore_util_netif_set(const char *name,
	const char *ipaddr, const char *gateway, const char *netmask);
TReturn tcore_util_netif_set_arp(const char *name, gboolean disable);
TReturn tcore_util_reset_ipv4_socket(const char *name, const char *ipaddr);
TReturn tcore_util_netif_set_mtu(const char *name, unsigned int mtu);

char *tcore_util_get_string_by_ip4type(union tcore_ip4_type ip);
char *tcore_util_get_string_by_ip6type(unsigned char ipv6_addr_type[IPV6_ADDR_LEN]);

GHashTable *tcore_util_marshal_create();
void tcore_util_marshal_destory(GHashTable *ht);

GHashTable *tcore_util_marshal_deserialize_string(const gchar *serialized_string);
gchar *tcore_util_marshal_serialize(GHashTable *ht);

gboolean tcore_util_marshal_add_data(GHashTable *ht, const gchar *key,
	const void *data, enum tcore_util_marshal_data_type type);
gboolean tcore_util_marshal_get_data(GHashTable *ht, const gchar *key,
	void **data, enum tcore_util_marshal_data_type type);

gint tcore_util_marshal_get_int(GHashTable *ht, const gchar *key);
gchar *tcore_util_marshal_get_string(GHashTable *ht, const gchar *key);
GHashTable *tcore_util_marshal_get_object(GHashTable *ht, const gchar *key);

enum tcore_dcs_type tcore_util_get_cbs_coding_scheme(unsigned char encode);

unsigned char *tcore_util_decode_hex(const char *src, int len);
void tcore_util_hex_dump(const char *pad, int size, const void *data);

unsigned char *tcore_util_unpack_gsm7bit(const unsigned char *src, unsigned int src_len);
unsigned char *tcore_util_pack_gsm7bit(const unsigned char *src, unsigned int src_len);
char *tcore_util_convert_bcd_str_2_ascii_str(const char* src, int src_len);
char *tcore_util_convert_bcd2ascii(const char *src, int src_len, int max_len);
char *tcore_util_convert_digit2ascii(const char* src, int src_len);
gboolean tcore_util_convert_utf8_to_gsm(unsigned char *dest, int *dest_len, unsigned char* src, int src_len);
gboolean tcore_util_convert_utf8_to_ucs2(char **dest, int *dest_len, unsigned char *src, int src_len);
gboolean tcore_util_convert_string_to_utf8(unsigned char *dest, unsigned short *dest_len,
	enum alphabet_format dcs, const unsigned char *src, unsigned short src_len);
gboolean tcore_util_convert_ascii_to_utf8(unsigned char **dest, int *dest_len, unsigned char *src, int src_len);
int tcore_util_convert_ucs2_to_utf8(char *out, unsigned short *out_len, char *in, unsigned short in_len);
char *tcore_util_convert_hexstring_to_bytes(char *s);

void tcore_util_swap_byte_order(unsigned char* dest, const unsigned char* src, int src_len);

char *tcore_util_get_version(void);
void tcore_util_set_log(gboolean enable);
gboolean tcore_util_is_country_NA(char *plmn);

__END_DECLS

#endif /* __TCORE_UTIL_H__ */
