#include "core/minunit.h"
#include <string.h>
#include <stdlib.h>
#include "movistartv/mtv.h"

char *test_load_conf()
{

    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_load_conf);
    return NULL;
}

RUN_TESTS(all_tests)

