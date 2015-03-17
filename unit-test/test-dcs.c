
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>

#include <tcore.h>
#include <util.h>

#if 0
static void dump_hex(const char *msg, unsigned char *data, int data_len)
{
	int i;

	printf("%s", msg);

	for (i = 0; i < data_len; i++) {
		printf("%02X ", data[i]);
	};

	printf("\n");
}
#endif

static void test_7bit(void)
{
	unsigned char *dest;
	unsigned char packed[] = { 0xE8, 0x32, 0x9B, 0xFD, 0x06, 0x00 };

	/* unpack packed-data('hello') */
	dest = tcore_util_unpack_gsm7bit(packed, 0x05);
	g_assert_cmpstr((char *)dest, ==, "hello");
	free(dest);

	/* pack 'hello' */
	dest = tcore_util_pack_gsm7bit((unsigned char *)"hello", strlen("hello"));

	if(!dest){
		g_assert(memcmp(packed, dest, strlen((char *)dest)) == 0);
		free(dest);
	}
}

static void test_hex(void)
{
	char *hex = (char *)"1A2B3C4D5E6F";
	unsigned char expected[] = { 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F };
	unsigned char *dest;

	dest = tcore_util_decode_hex(hex, strlen(hex));
	g_assert(memcmp(dest, expected, strlen((char *)dest)) == 0);
	free(dest);
}

static void test_ucs(void)
{
}

int main(int argc, char **argv)
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/dcs/hex", test_hex);
	g_test_add_func("/dcs/7bit", test_7bit);
	g_test_add_func("/dcs/ucs", test_ucs);

	return g_test_run();
}
