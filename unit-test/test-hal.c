
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>

#include <tcore.h>
#include <hal.h>

int g_flag;


static void on_recv_callback(TcoreHal *h, unsigned int data_len, const void *data, void *user_data)
{
	g_assert(data_len == 3);
	g_assert_cmpstr((char *)data, ==, "123");

	g_flag++;
}

static void on_recv_callback2(TcoreHal *h, unsigned int data_len, const void *data, void *user_data)
{
	g_assert(data_len == 3);
	g_assert_cmpstr((char *)data, ==, "123");

	g_flag++;
}

static void test_callback(void)
{
	TcoreHal *h;

	h = tcore_hal_new(NULL, "test_hal", NULL, TCORE_HAL_MODE_CUSTOM);
	g_assert(h);

	tcore_hal_add_recv_callback(h, on_recv_callback, NULL);

	g_flag = 0;
	tcore_hal_emit_recv_callback(h, 3, "123");
	g_assert(g_flag == 1);

	tcore_hal_remove_recv_callback(h, on_recv_callback);

	g_flag = 0;
	tcore_hal_emit_recv_callback(h, 3, "123");
	g_assert(g_flag == 0);

	tcore_hal_free(h);
}

static void test_callback2(void)
{
	TcoreHal *h;

	h = tcore_hal_new(NULL, "test_hal", NULL, TCORE_HAL_MODE_CUSTOM);
	g_assert(h);

	tcore_hal_add_recv_callback(h, on_recv_callback, NULL);
	tcore_hal_add_recv_callback(h, on_recv_callback2, NULL);

	g_flag = 0;
	tcore_hal_emit_recv_callback(h, 3, "123");
	g_assert(g_flag == 2);

	tcore_hal_remove_recv_callback(h, on_recv_callback);

	g_flag = 0;
	tcore_hal_emit_recv_callback(h, 3, "123");
	g_assert(g_flag == 1);

	/* remove already removed callback */
	tcore_hal_remove_recv_callback(h, on_recv_callback);

	g_flag = 0;
	tcore_hal_emit_recv_callback(h, 3, "123");
	g_assert(g_flag == 1);

	tcore_hal_remove_recv_callback(h, on_recv_callback2);

	g_flag = 0;
	tcore_hal_emit_recv_callback(h, 3, "123");
	g_assert(g_flag == 0);

	tcore_hal_free(h);
}

int main(int argc, char **argv)
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/hal/callback", test_callback);
	g_test_add_func("/hal/callback2", test_callback2);

	return g_test_run();
}
