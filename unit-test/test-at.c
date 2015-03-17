
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>

#include <tcore.h>
#include <at.h>

static gboolean on_cmti(TcoreAT *at, const GSList *lines, void *user_data)
{
	int *checked = user_data;

	*checked = 1;

	printf("data = [%s]\n", (char *)lines->data);

	return FALSE;
}

static gboolean on_cmt(TcoreAT *at, const GSList *lines, void *user_data)
{
	int *checked = user_data;

	if (g_slist_length((GSList *)lines) != 2) {
		printf("invalid message\n");
		return TRUE;
	}

	*checked = 2;

	printf("data1 = [%s]\n", (char *)lines->data);
	printf("data2 = [%s]\n", (char *)lines->next->data);

	return TRUE;
}

static void test_noti(void)
{
	TcoreAT *at;
	TReturn ret;
	int checked = 0;
	char *msg = NULL;

	at = tcore_at_new(NULL);
	g_assert(at);

	ret = tcore_at_add_notification(at, "+CMTI:", FALSE, on_cmti, &checked);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	ret = tcore_at_add_notification(at, "+CMT:", TRUE, on_cmt, &checked);
	g_assert(ret == TCORE_RETURN_SUCCESS);


	/* Non type PDU message */
	msg = (char *)"+CMTI: \"SM\", 1\r\n";
	ret = tcore_at_process(at, strlen(msg), msg);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* callback check */
	g_assert(checked == 1);


	/* Completed PDU message */
	msg = (char *)"+CMT: 19\r\n038121F3100481214301112170137192410404F17B590E\r\n";
	ret = tcore_at_process(at, strlen(msg), msg);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* callback check */
	g_assert(checked == 2);


	/* Split PDU message */
	checked = 0;
	msg = (char *)"+CMT: 19\r\n";
	ret = tcore_at_process(at, strlen(msg), msg);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* callback check */
	g_assert(checked == 0);

	msg = (char *)"038121F310";
	ret = tcore_at_process(at, strlen(msg), msg);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* callback check */
	g_assert(checked == 0);

	msg = (char *)"0481214301112170137192410404F17B590E\r\n";
	ret = tcore_at_process(at, strlen(msg), msg);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	/* callback check */
	g_assert(checked == 2);

}

static void test_buf_write(void)
{
#define BUF_SIZE 1024
	TcoreAT *at;
	TReturn ret;
	char *msg_1 = (char *)"ATZ";
	char msg_2[BUF_SIZE + 1];

	at = tcore_at_new(NULL);
	g_assert(at);

	ret = tcore_at_buf_write(at, strlen(msg_1), msg_1);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	memset(msg_2, '1', BUF_SIZE);
	msg_2[BUF_SIZE] = '\0';
	ret = tcore_at_buf_write(at, strlen(msg_2), msg_2);
	g_assert(ret == TCORE_RETURN_SUCCESS);

	tcore_at_free (at);
}

static void test_tok(void)
{
	GSList *tokens;
	char *buf;

	/* test unsolicited message */
	tokens = tcore_at_tok_new("CSQ: 31, 99");
	g_assert(tokens);

	g_assert_cmpstr((char *)g_slist_nth_data(tokens, 0), ==, "31");
	g_assert_cmpstr(tcore_at_tok_nth(tokens, 1), ==, "99");
	g_assert(g_slist_nth_data(tokens, 2) == NULL);

	tcore_at_tok_free(tokens);

	/* test sub token (list type) */
	tokens = tcore_at_tok_new("(2, \"T-Meego\", \"TTAU\", \"23401\", 2)");
	g_assert(tokens);

	g_assert_cmpstr((char *)g_slist_nth_data(tokens, 0), ==, "2");
	g_assert_cmpstr(tcore_at_tok_nth(tokens, 1), ==, "\"T-Meego\"");
	g_assert_cmpstr(tcore_at_tok_nth(tokens, 2), ==, "\"TTAU\"");
	g_assert_cmpstr(tcore_at_tok_nth(tokens, 3), ==, "\"23401\"");
	g_assert_cmpstr(tcore_at_tok_nth(tokens, 4), ==, "2");

	/* test extract */
	buf = tcore_at_tok_extract(tcore_at_tok_nth(tokens, 2));
	g_assert(buf);
	g_assert_cmpstr(buf, ==, "TTAU");
	free(buf);

	tcore_at_tok_free(tokens);

	/* test extract */
	buf = tcore_at_tok_extract("(haha)");
	g_assert(buf);
	g_assert_cmpstr(buf, ==, "haha");
	free(buf);

	/* test extract */
	buf = tcore_at_tok_extract("(a)");
	g_assert(buf);
	g_assert_cmpstr(buf, ==, "a");
	free(buf);

	/* test extract */
	buf = tcore_at_tok_extract("()");
	g_assert(buf);
	g_assert_cmpstr(buf, ==, "");
	free(buf);

	/* test extract */
	buf = tcore_at_tok_extract("(");
	g_assert(buf);
	g_assert_cmpstr(buf, ==, "(");
	free(buf);

	/* test extract */
	buf = tcore_at_tok_extract(")");
	g_assert(buf);
	g_assert_cmpstr(buf, ==, ")");
	free(buf);

	/* test extract */
	buf = tcore_at_tok_extract("");
	g_assert(buf);
	g_assert_cmpstr(buf, ==, "");
	free(buf);
}

static void test_pdu_resp(void)
{
	TcoreAT *at;
	TcoreATRequest *req;
	TReturn ret;
	char *msg;

	at = tcore_at_new(NULL);
	g_assert(at);

	req = tcore_at_request_new("AT+CMGR=1", "+CMGR", TCORE_AT_PDU);
	g_assert(req);

	ret = tcore_at_set_request(at, req, TRUE);

	msg = (char *)"+CMGR: 1,,105\r\n12345678901234567890\r\nOK\r\n";
	ret = tcore_at_process(at, strlen(msg), msg);
	g_assert(ret == TRUE);

	tcore_at_free (at);
}

int main(int argc, char **argv)
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/at/buf_write", test_buf_write);
	g_test_add_func("/at/tok", test_tok);
	g_test_add_func("/at/noti", test_noti);
	g_test_add_func("/at/pdu_resp", test_pdu_resp);

	return g_test_run();
}
