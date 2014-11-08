#include <check.h>
#include <string.h>
#include <stdlib.h>
#include "movistartv/xmltv.h"
#include "movistartv/dbg.h"

START_TEST(test_xmltv_add_programme)
{
    ck_assert(1);
}
END_TEST

START_TEST(test_xmltv_add_channel)
{
    ck_assert(1);
}
END_TEST

START_TEST(test_xmltv_to_xml)
{
    ck_assert(1);
}
END_TEST

Suite *net_suite(void)
{
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_xmltv_add_channel);
    tcase_add_test(tc_core, test_xmltv_add_programme);
    tcase_add_test(tc_core, test_xmltv_to_xml);

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

