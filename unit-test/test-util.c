
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>

#include <tcore.h>
#include <util.h>

static void test_marshal(void)
{
	GHashTable *list;
	int value_int;

	list = tcore_util_marshal_create ();
	g_assert(list);

	value_int = 1;
	tcore_util_marshal_add_data (list, "key1", &value_int, TCORE_UTIL_MARSHAL_DATA_INT_TYPE);
	g_assert(tcore_util_marshal_get_int (list, "key1") == value_int);

	value_int = 2;
	tcore_util_marshal_add_data (list, "key2", &value_int, TCORE_UTIL_MARSHAL_DATA_INT_TYPE);
	g_assert(tcore_util_marshal_get_int (list, "key2") == value_int);

	tcore_util_marshal_destory (list);
}

static void test_marshal_serialize(void)
{
	GHashTable *list;
	GHashTable *item;
	GHashTable *tmp;
	gchar *serialized;
	int i;
	char buf[255];
	int value_int;

	list = tcore_util_marshal_create ();
	g_assert(list);

	value_int = 1;
	tcore_util_marshal_add_data (list, "key1", &value_int, TCORE_UTIL_MARSHAL_DATA_INT_TYPE);
	g_assert(tcore_util_marshal_get_int (list, "key1") == value_int);

	value_int = 2;
	tcore_util_marshal_add_data (list, "key2", &value_int, TCORE_UTIL_MARSHAL_DATA_INT_TYPE);
	g_assert(tcore_util_marshal_get_int (list, "key2") == value_int);

	item = tcore_util_marshal_create ();
	g_assert (item);
	tcore_util_marshal_add_data (list, "key_object", item, TCORE_UTIL_MARSHAL_DATA_OBJECT_TYPE);

	for (i = 0; i < 3; i++) {
		item = tcore_util_marshal_create ();
		g_assert (item);

		value_int = i * 10;
		snprintf (buf, 255, "sub-%d", i);
		tcore_util_marshal_add_data (item, buf, &value_int, TCORE_UTIL_MARSHAL_DATA_INT_TYPE);
		g_assert(tcore_util_marshal_get_int (item, buf) == value_int);

		tcore_util_marshal_add_data (list, buf, item, TCORE_UTIL_MARSHAL_DATA_OBJECT_TYPE);
	}

	serialized = tcore_util_marshal_serialize (list);
	g_assert (serialized);
	tcore_util_marshal_destory (list);

	tmp = tcore_util_marshal_deserialize_string (serialized);
	g_assert (tmp);

	free (serialized);

	g_assert(tcore_util_marshal_get_int (tmp, "key1") == 1);
	g_assert(tcore_util_marshal_get_int (tmp, "key2") == 2);

	tcore_util_marshal_destory (tmp);
}

int main(int argc, char **argv)
{
	g_test_init(&argc, &argv, NULL);

#if !GLIB_CHECK_VERSION(2,35,0)
	g_type_init();
#endif

	g_test_add_func("/util/marshal", test_marshal);
	g_test_add_func("/util/marshal_serialize", test_marshal_serialize);

	return g_test_run();
}
