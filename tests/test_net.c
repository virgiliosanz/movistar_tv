#include <check.h>
#include <string.h>
#include <stdlib.h>
#include "movistartv/net.h"
#include "movistartv/dbg.h"

START_TEST(test_net_http_get)
{
    char *body = net_http_get("http://www.google.com");
    ck_assert(strlen(body) > 0);

    ck_assert_msg(strstr(body, "<html") != NULL, "body:\n%s", body);
    ck_assert_msg(strstr(body, "</html>") != NULL, "body:\n%s", body);
    ck_assert_msg(strstr(body, "<body") != NULL, "body:\n%s", body);
    ck_assert_msg(strstr(body, "</body>") != NULL, "body:\n%s", body);

    free(body);
}
END_TEST

Suite *net_suite(void)
{
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_net_http_get);

    Suite *s = suite_create("Net");
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
   Suite *s = net_suite();
   SRunner *sr = srunner_create(s);
   srunner_run_all(sr, CK_NORMAL);
   size_t number_failed = srunner_ntests_failed(sr);
   srunner_free(sr);
   return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

