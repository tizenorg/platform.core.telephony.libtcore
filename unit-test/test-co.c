
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>

#include <tcore.h>
#include <core_object.h>

static int _count = 0;

static gboolean on_event1(CoreObject *o, const void *event_info, void *user_data)
{
	_count++;

	g_assert(o);
	g_assert(event_info);
	g_assert_cmpstr(event_info, ==, "ok");

	return TRUE;
}

static gboolean on_event2(CoreObject *o, const void *event_info, void *user_data)
{
	_count++;

	g_assert(o);
	g_assert(event_info);
	g_assert_cmpstr(event_info, ==, "ok");

	return FALSE;
}


static void test_co_new(void)
{
	CoreObject *co;

	co = tcore_object_new(NULL, "test", NULL);
	g_assert(co);

	tcore_object_free(co);
}

static void test_callback_renew(void)
{
	CoreObject *co;
	TReturn ret;

	co = tcore_object_new(NULL, "test", NULL);
	g_assert(co);

	/* lifetime: unlimit */
	_count = 0;
	ret = tcore_object_add_callback(co, "event1", on_event1, NULL);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	ret = tcore_object_emit_callback(co, "event1", "ok");
	g_assert(ret == TCORE_RETURN_SUCCESS);

	ret = tcore_object_emit_callback(co, "event1", "ok");
	g_assert(ret == TCORE_RETURN_SUCCESS);

	g_assert(_count == 2);
}

static void test_callback_once(void)
{
	CoreObject *co;
	TReturn ret;

	co = tcore_object_new(NULL, "test", NULL);
	g_assert(co);

	/* lifetime: one time callback + unlimit callback */
	_count = 0;
	ret = tcore_object_add_callback(co, "event2", on_event2, NULL);
	g_assert(ret == TCORE_RETURN_SUCCESS);
	ret = tcore_object_add_callback(co, "event1", on_event1, NULL);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	ret = tcore_object_emit_callback(co, "event2", "ok");
	g_assert(ret == TCORE_RETURN_SUCCESS);

	ret = tcore_object_emit_callback(co, "event2", "ok");
	g_assert(ret == TCORE_RETURN_SUCCESS);

	g_assert(_count == 1);

	ret = tcore_object_emit_callback(co, "event1", "ok");
	g_assert(ret == TCORE_RETURN_SUCCESS);

	g_assert(_count == 2);

	tcore_object_free(co);
}

static gboolean on_prop_changed(CoreObject *co, const void *event_info,
		void *user_data)
{
	int *mode = user_data;
	GSList *key = (GSList *)event_info;
	const char *data;

	switch (*mode) {
	case 1:
		g_assert(g_slist_length(key) == 1);
		g_assert_cmpstr(key->data, ==, "qqq");
		data = tcore_object_ref_property(co, "qqq");
		g_assert_cmpstr(data, ==, "1234");
	break;

	case 2:
		g_assert(g_slist_length(key) == 1);
		g_assert_cmpstr(key->data, ==, "qqq");
		data = tcore_object_ref_property(co, "qqq");
		g_assert(data == NULL);
	break;

	case 3:
		g_assert(g_slist_length(key) == 1);
		g_assert_cmpstr(key->data, ==, "www");
		data = tcore_object_ref_property(co, "www");
		g_assert_cmpstr(data, ==, "heap");
	break;

	case 4:
		g_assert(g_slist_length(key) == 1);
		g_assert_cmpstr(key->data, ==, "www");
		data = tcore_object_ref_property(co, "www");
		g_assert(data == NULL);
	break;

	case 5:
		g_assert(g_slist_length(key) == 1);
		g_assert_cmpstr(key->data, ==, "key");
		data = tcore_object_ref_property(co, "key");
		g_assert_cmpstr(data, ==, "1");
	break;

	case 6:
		g_assert(g_slist_length(key) == 1);
		g_assert_cmpstr(key->data, ==, "key");
		data = tcore_object_ref_property(co, "key");
		g_assert_cmpstr(data, ==, "2");
	break;

	case 7:
		g_assert(FALSE);
	break;

	case 8:
		g_assert(g_slist_length(key) == 1);
		g_assert_cmpstr(key->data, ==, "key");
		data = tcore_object_ref_property(co, "key");
		g_assert_cmpstr(data, ==, "2");
	break;

	case 9:
		g_assert(g_slist_length(key) == 2);
		g_assert_cmpstr(key->data, ==, "foo");
		g_assert_cmpstr(key->next->data, ==, "bar");
		data = tcore_object_ref_property(co, "foo");
		g_assert_cmpstr(data, ==, "1");
		data = tcore_object_ref_property(co, "bar");
		g_assert_cmpstr(data, ==, "2");
	break;

	case 10:
		g_assert(g_slist_length(key) == 1);
		g_assert_cmpstr(key->data, ==, "foo");
		data = tcore_object_ref_property(co, "foo");
		g_assert(data == NULL);
	break;

	case 11:
		g_assert(g_slist_length(key) == 2);
		g_assert(g_slist_find_custom(key, "foo",
					(GCompareFunc)g_strcmp0) != NULL);
		g_assert(CORE_OBJECT_KEY_FIND(key, "bar") != NULL);
		data = tcore_object_ref_property(co, "foo");
		g_assert_cmpstr(data, ==, "1");
		data = tcore_object_ref_property(co, "bar");
		g_assert(data == NULL);
	break;

	default:
		g_assert(FALSE);
	break;
	}

	/* Set flag to callback invocation success */
	*mode = 0;

	return TRUE;
}

static void test_property(void)
{
	CoreObject *co;
	const char *data;
	char *test;
	GHashTable *raw;
	int mode = 0;
	GSList *tmp_list;

	co = tcore_object_new(NULL, "test", NULL);
	g_assert(co);

	tcore_object_add_callback(co, CORE_OBJECT_EVENT_PROPERTY_CHANGED,
		on_prop_changed, &mode);

	raw = tcore_object_ref_property_hash(co);
	g_assert(raw);

	/* Case: Basic property set */
	mode = 1;
	tcore_object_set_property(co, "qqq", "1234");
	g_assert(g_hash_table_size(raw) == 1);
	g_assert(mode == 0);

	data = tcore_object_ref_property(co, "qqq");
	g_assert_cmpstr(data, ==, "1234");

	/* Case: Basic property remove */
	mode = 2;
	tcore_object_set_property(co, "qqq", NULL);
	g_assert(g_hash_table_size(raw) == 0);
	g_assert(mode == 0);

	data = tcore_object_ref_property(co, "qqq");
	g_assert(data == NULL);

	/* Case: Malloc property set */
	mode = 3;
	test = strdup("heap");
	tcore_object_set_property(co, "www", test);
	g_assert(g_hash_table_size(raw) == 1);
	g_assert(mode == 0);
	free(test);

	data = tcore_object_ref_property(co, "www");
	g_assert_cmpstr(data, ==, "heap");

	/* Case: Malloc property remove */
	mode = 4;
	tcore_object_set_property(co, "www", NULL);
	g_assert(g_hash_table_size(raw) == 0);
	g_assert(mode == 0);

	data = tcore_object_ref_property(co, "www");
	g_assert(data == NULL);

	/* Case: Same key & Different value set */
	mode = 5;
	tcore_object_set_property(co, "key", "1");
	g_assert(g_hash_table_size(raw) == 1);
	g_assert(mode == 0);

	mode = 6;
	tcore_object_set_property(co, "key", "2");
	g_assert(g_hash_table_size(raw) == 1);
	g_assert(mode == 0);

	/* Case: Same key & Same value set => No callback invocation */
	mode = 7;
	tcore_object_set_property(co, "key", "2");
	g_assert(g_hash_table_size(raw) == 1);
	g_assert(mode == 7);

	/* Case: Same key & Same value set & force event invocation */
	mode = 8;
	tcore_object_set_property(co, "key", "2");
	g_assert(g_hash_table_size(raw) == 1);

	tmp_list = g_slist_append(NULL, (char *)"key");
	tcore_object_emit_callback(co, CORE_OBJECT_EVENT_PROPERTY_CHANGED, tmp_list);
	g_assert(mode == 0);
	g_slist_free(tmp_list);

	/* Case: Multiple property set */
	mode = 9;
	tcore_object_set_property(co, "foo", "1", "bar", "2");
	g_assert(g_hash_table_size(raw) == 3);
	g_assert(mode == 0);

	/* Case: Set key without value => same as key with NULL: remove key */
	mode = 10;
	tcore_object_set_property(co, "foo");
	g_assert(g_hash_table_size(raw) == 2);
	g_assert(mode == 0);

	/* Case: Multiple property set with unset (foo set, bar unset) */
	mode = 11;
	tcore_object_set_property(co, "foo", "1", "bar");
	g_assert(g_hash_table_size(raw) == 2);
	g_assert(mode == 0);

	tcore_object_free(co);
}

int main(int argc, char **argv)
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/co/new", test_co_new);
	g_test_add_func("/co/callback_renew", test_callback_renew);
	g_test_add_func("/co/callback_once", test_callback_once);
	g_test_add_func("/co/property", test_property);

	return g_test_run();
}
