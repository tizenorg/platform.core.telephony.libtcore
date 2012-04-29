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

#include "tcore.h"
#include "util.h"


static gboolean _tcore_util_marshal_create_gvalue(GValue *value,
		const void *data, enum tcore_util_marshal_data_type type)
{
	switch (type) {
		case TCORE_UTIL_MARSHAL_DATA_CHAR_TYPE:
			g_value_init(value, type);
			g_value_set_char(value, *((gchar *) data));
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
		enum tcore_util_marshal_data_type type)
{
	switch (type) {
		case TCORE_UTIL_MARSHAL_DATA_CHAR_TYPE:
			*dest = g_new0(gchar, 1);
			*((gchar *) *dest) = g_value_get_char(src);
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
	unsigned int gtype = 0;
	GHashTable *ht = NULL;

	gtype = ((GValue *) value)->g_type;
	if (gtype == G_TYPE_HASH_TABLE) {
		ht = g_value_get_boxed(value);
		g_hash_table_destroy(ht);
	}

	g_value_unset((GValue *) value);
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
	if (fd < 0) {
		return TCORE_RETURN_FAILURE;
	}

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
	if (fd < 0) {
		return TCORE_RETURN_FAILURE;
	}

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

TReturn tcore_util_netif_set(const char *name, const char *ipaddr,
		const char *gateway, const char *netmask)
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
	if (fd < 0) {
		return TCORE_RETURN_FAILURE;
	}

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

char *tcore_util_get_string_by_ip4type(union tcore_ip4_type ip)
{
	char buf[16]; /* 'nnn'*4 + '.'*3 + '\0' */

	snprintf(buf, 16, "%d.%d.%d.%d", ip.s[0], ip.s[1], ip.s[2], ip.s[3]);

	return strdup(buf);
}

enum tcore_dcs_type tcore_util_get_cbs_coding_scheme(unsigned char encode)
{
	enum tcore_dcs_type dcs = TCORE_DCS_TYPE_NONE;

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
			else if ((encode & 0x04) == 0x01)
				dcs = TCORE_DCS_TYPE_8_BIT;
			break;
	}

	return dcs;
}

char *tcore_util_unpack_gsm7bit(const char *src, unsigned int src_len)
{
	char *dest;
	int i = 0;
	unsigned int pos = 0;
	unsigned char shift = 0;
	int outlen = 0;

	if (!src || src_len == 0) {
		return NULL;
	}

	outlen = (src_len * 8) / 7;

	dest = calloc(outlen + 1, 1);
	if (!dest)
		return NULL;

	for (i = 0; pos < src_len; i++, pos++) {
		dest[i] = (src[pos] << shift) & 0x7F;

		if (pos != 0) {
			/* except the first byte, a character contains some bits from the previous byte.*/
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

	/*If a character is '\r'(13), change it to space(32) */
	for (i = 0; i < outlen; i++)
		if (dest[i] == '\r')
			dest[i] = ' ';

	dest[outlen] = '\0';

	return dest;
}

char *tcore_util_pack_gsm7bit(const char* src, unsigned int src_len)
{
	char *dest;
	unsigned int i = 0;
	unsigned int pos = 0, shift = 0;
	unsigned int outlen = 0;

	if (!src || src_len == 0) {
		return NULL;
	}

	outlen = ((src_len * 7) / 8) + 1;

	dest = calloc(outlen + 1, 1);
	if (!dest)
		return NULL;

	for (pos = 0, i = 0; i < src_len; pos++, i++) {
		if ( pos >= outlen ) {
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

	tuple = g_strsplit((gchar *) serialized_string, "/", 0);

	while (strlen(tuple[index]) > 3) {
		int tmp = 0;
		guchar *content = NULL;
		gchar **inner_tuple = NULL;
		GValue *src = g_new0(GValue, 1);
		GValue *dest = g_new0(GValue, 1);
		unsigned int type = 0;

		inner_tuple = g_strsplit(tuple[index], ":", 0);
		type = atoi(inner_tuple[1]);
		content = g_base64_decode(inner_tuple[2], (gsize *)&tmp);

		g_value_init(src, G_TYPE_STRING);
		g_value_init(dest, type);

		g_value_set_string(src, (const gchar *)content);
		_tcore_util_marshal_convert_str_to_type(src, dest, type);
		g_hash_table_insert(ht, g_strdup(inner_tuple[0]), dest);

		g_free(content);
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
		unsigned int gtype = 0;
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
		g_free(tmp);

		g_string_append_printf(gstring_tmp, "%s:%d:%s/", (gchar *)key,
				gtype, encoded_d);
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

	rv = tcore_util_marshal_get_data(ht, key, (void **) &tmp,
			TCORE_UTIL_MARSHAL_DATA_INT_TYPE);
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
