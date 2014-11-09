#include "core/minunit.h"
#include "movistartv/xmltv.h"

char *test_xmltv_add_programme()
{
    return NULL;
}

char *test_xmltv_add_channel()
{
    return NULL;
}

char *test_xmltv_to_xml()
{
    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_xmltv_add_channel);
    mu_run_test(test_xmltv_add_programme);
    mu_run_test(test_xmltv_to_xml);

    return NULL;
}

RUN_TESTS(all_tests)

