#include "config.h"

char *
test_conf()
{
	mtv_conf_s *cnf = mtv_conf_load();
	mu_assert(cnf != NULL, "Could'nt read configuration");
	mtv_conf_debug(cnf);

	mtv_conf_destroy(cnf);
	mu_assert(conf != NULL, "Did not destroy memory correctly");

	return NULL;
}

char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_conf);
	return NULL;
}

RUN_TESTS(all_tests)
