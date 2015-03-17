
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include <tcore.h>
#include <queue.h>
#include <hal.h>
#include <user_request.h>

static int g_flag;

static TReturn always_send_ok(TcoreHal *hal, unsigned int data_len, void *data)
{
	return TCORE_RETURN_SUCCESS;
}

static struct tcore_hal_operations hops = {
	.send = always_send_ok,
};

static void test_queue_hal(void)
{
	TcoreHal *h;
	TcorePending *pending1;
	TcorePending *pending2;
	TcorePending *pending3;
	TcorePending *pending = NULL;
	TReturn ret;

	h = tcore_hal_new(NULL, "test_hal", &hops, TCORE_HAL_MODE_CUSTOM);
	g_assert(h);

	ret = tcore_hal_set_power_state(h, TRUE);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* first item */
	pending1 = tcore_pending_new(NULL, 1);
	g_assert(pending1);
	tcore_pending_set_priority(pending1, TCORE_PENDING_PRIORITY_IMMEDIATELY);

	/* second item */
	pending2 = tcore_pending_new(NULL, 1);
	g_assert(pending2);
	tcore_pending_set_auto_free_status_after_sent(pending2, TRUE);

	/* third item */
	pending3 = tcore_pending_new(NULL, 1);
	g_assert(pending3);
	tcore_pending_set_priority(pending3, TCORE_PENDING_PRIORITY_IMMEDIATELY);

	/* send pending1 */
	ret = tcore_hal_send_request(h, pending1);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* send pending2 -> queue */
	ret = tcore_hal_send_request(h, pending2);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* remove pending1 */
	ret = tcore_hal_dispatch_response_data(h, 1, 0, NULL);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* check next pending */
	pending = tcore_queue_ref_pending_by_id(tcore_hal_ref_queue(h), 1);
	g_assert(pending == pending2);

	/* force send (because g_main_loop not work in unit test) */
	tcore_hal_send_force(h);

	/* check next pending (pending2 is auto free) */
	pending = tcore_queue_ref_pending_by_id(tcore_hal_ref_queue(h), 1);
	g_assert(pending == NULL);

	/* send pending3 */
	ret = tcore_hal_send_request(h, pending3);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	pending = tcore_queue_ref_pending_by_id(tcore_hal_ref_queue(h), 1);
	g_assert(pending == pending3);

	/* remove pending3 */
	ret = tcore_hal_dispatch_response_data(h, 1, 0, NULL);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* check next pending (no more pending) */
	pending = tcore_queue_ref_pending_by_id(tcore_hal_ref_queue(h), 1);
	g_assert(pending == NULL);

	tcore_hal_free(h);
}

static void test_queue_push_priority(void)
{
	TcoreQueue *q;
	TcorePending *pending1;
	TcorePending *pending2;
	TcorePending *pending3;
	TcorePending *pending = NULL;
	TReturn ret;

	q = tcore_queue_new(NULL);
	g_assert(q);

	/* first item */
	pending1 = tcore_pending_new(NULL, 1);
	g_assert(pending1);
	tcore_pending_set_priority(pending1, TCORE_PENDING_PRIORITY_IMMEDIATELY);

	ret = tcore_queue_push(q, pending1);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* second item */
	pending2 = tcore_pending_new(NULL, 1);
	g_assert(pending2);

	ret = tcore_queue_push(q, pending2);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* third item, but push to second item position */
	pending3 = tcore_pending_new(NULL, 1);
	g_assert(pending3);
	tcore_pending_set_priority(pending3, TCORE_PENDING_PRIORITY_IMMEDIATELY);

	ret = tcore_queue_push(q, pending3);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* pop test (order by priority) */
	pending = tcore_queue_pop_by_id(q, 1);
	g_assert(pending == pending1);

	pending = tcore_queue_pop_by_id(q, 1);
	g_assert(pending == pending3);

	pending = tcore_queue_pop_by_id(q, 1);
	g_assert(pending == pending2);

	tcore_queue_free(q);
}

static void test_queue_push(void)
{
	TcoreQueue *q;
	TcorePending *pending1;
	TcorePending *pending2;
	TcorePending *pending3;
	TcorePending *pending = NULL;
	TReturn ret;

	q = tcore_queue_new(NULL);
	g_assert(q);

	/* first item */
	pending1 = tcore_pending_new(NULL, 1);
	g_assert(pending1);

	ret = tcore_queue_push(q, pending1);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* second item */
	pending2 = tcore_pending_new(NULL, 1);
	g_assert(pending2);

	ret = tcore_queue_push(q, pending2);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* third item */
	pending3 = tcore_pending_new(NULL, 1);
	g_assert(pending3);

	ret = tcore_queue_push(q, pending3);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* pop test (order by push sequence (same priority)) */
	pending = tcore_queue_pop_by_id(q, 1);
	g_assert(pending == pending1);

	pending = tcore_queue_pop_by_id(q, 1);
	g_assert(pending == pending2);

	pending = tcore_queue_pop_by_id(q, 1);
	g_assert(pending == pending3);

	tcore_queue_free(q);
}

static void on_resp(TcorePending *pending, int data_len, const void *data, void *user_data)
{
	printf("on_resp !!\n");
	g_flag++;
}

static void test_queue_push_cancel(void)
{
	TcoreHal *h;
	TcorePending *pending1;
	TcorePending *pending2;
	TcorePending *pending3;
	TcorePending *p;
	TReturn ret;
	UserRequest *ur1;
	UserRequest *ur2;

	h = tcore_hal_new(NULL, "test_hal", &hops, TCORE_HAL_MODE_CUSTOM);
	g_assert(h);

	ret = tcore_hal_set_power_state(h, TRUE);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* first item */
	ur1 = tcore_user_request_new(NULL, NULL);
	g_assert(ur1);

	tcore_user_request_set_command(ur1, TREQ_NETWORK_SEARCH);

	pending1 = tcore_pending_new(NULL, 1);
	g_assert(pending1);

	tcore_pending_set_response_callback(pending1, on_resp, NULL);
	tcore_pending_link_user_request(pending1, ur1);

	ret = tcore_hal_send_request(h, pending1);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* force update send state */
	tcore_pending_emit_send_callback(pending1, TRUE);

	/* second item */
	ur2 = tcore_user_request_new(NULL, NULL);
	g_assert(ur2);

	tcore_user_request_set_command(ur2, TREQ_NETWORK_SEARCH);

	pending2 = tcore_pending_new(NULL, 1);
	g_assert(pending2);

	tcore_pending_set_response_callback(pending2, on_resp, NULL);
	tcore_pending_link_user_request(pending2, ur2);

	ret = tcore_hal_send_request(h, pending2);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* third item */
	pending3 = tcore_pending_new(NULL, 1);
	g_assert(pending3);

	ret = tcore_hal_send_request(h, pending3);
	g_assert(ret == TCORE_RETURN_SUCCESS);


	/* search */
	p = tcore_queue_search_by_command(tcore_hal_ref_queue(h), TREQ_NETWORK_SEARCH, TRUE);
	g_assert(p == pending1);

	p = tcore_queue_search_by_command(tcore_hal_ref_queue(h), TREQ_NETWORK_SEARCH, FALSE);
	g_assert(p == pending2);

	/* cancel */
	g_flag = 0;
	ret = tcore_queue_cancel_pending_by_command(tcore_hal_ref_queue(h), TREQ_NETWORK_SEARCH);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	g_assert(g_flag == 2);

}

static void test_queue_new(void)
{
	TcoreQueue *q;

	q = tcore_queue_new(NULL);
	g_assert(q);

	tcore_queue_free(q);
}

int main(int argc, char **argv)
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/queue/new", test_queue_new);
	g_test_add_func("/queue/push", test_queue_push);
	g_test_add_func("/queue/push_priority", test_queue_push_priority);
	g_test_add_func("/queue/push_cancel", test_queue_push_cancel);
	g_test_add_func("/queue/hal", test_queue_hal);

	return g_test_run();
}
