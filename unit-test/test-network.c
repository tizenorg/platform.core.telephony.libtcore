
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>

#include <tcore.h>
#include <plugin.h>
#include <co_network.h>


static void test_operator(void)
{
	TcorePlugin *p;
	CoreObject *o;
	struct tcore_network_operator_info noi;
	struct tcore_network_operator_info *tmp_noi;
	TReturn ret;

	p = tcore_plugin_new(NULL, NULL, NULL, NULL);
	g_assert(p);

	o = tcore_network_new(p, "test_network", NULL, NULL);
	g_assert(o);

	/* Add 2 items */
	snprintf(noi.mcc, 4, "123");
	snprintf(noi.mnc, 4, "01");
	snprintf(noi.name, 41, "Hello");
	ret = tcore_network_operator_info_add(o, &noi);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	snprintf(noi.mcc, 4, "123");
	snprintf(noi.mnc, 4, "02");
	snprintf(noi.name, 41, "World");
	ret = tcore_network_operator_info_add(o, &noi);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* Check items */
	tmp_noi = tcore_network_operator_info_find(o, "123", "01");
	g_assert(tmp_noi);
	g_assert_cmpstr(tmp_noi->name, ==, "Hello");

	tmp_noi = tcore_network_operator_info_find(o, "123", "02");
	g_assert(tmp_noi);
	g_assert_cmpstr(tmp_noi->name, ==, "World");

	/* Replace item */
	snprintf(noi.mcc, 4, "123");
	snprintf(noi.mnc, 4, "02");
	snprintf(noi.name, 41, "Update");
	ret = tcore_network_operator_info_add(o, &noi);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* Check items */
	tmp_noi = tcore_network_operator_info_find(o, "123", "01");
	g_assert(tmp_noi);
	g_assert_cmpstr(tmp_noi->name, ==, "Hello");

	tmp_noi = tcore_network_operator_info_find(o, "123", "02");
	g_assert(tmp_noi);
	g_assert_cmpstr(tmp_noi->name, ==, "Update");

	tcore_network_free(o);
	tcore_plugin_free(p);
}

int main(int argc, char **argv)
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/network/operator", test_operator);

	return g_test_run();
}
