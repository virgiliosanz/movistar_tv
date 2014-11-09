#include "core/minunit.h"
#include <assert.h>
#include "bstrlib/bstrlib.h"

char *test_to_learn()
{
    bstring s = bfromcstr("HOLA");
    mu_assert(bstricmp(s, bfromcstr("hola")) == 0, "Es igual a cero");
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_to_learn);

    return NULL;

}

RUN_TESTS(all_tests)

