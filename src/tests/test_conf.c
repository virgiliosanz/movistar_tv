#include "config.h"

char *
test_conf()
{
	struct tva_conf *cnf = tva_conf_load();
	mu_assert(cnf != NULL, "Could'nt read configuration");
	tva_conf_debug(cnf);

	mu_assert(cnf->demarcation > 0, "Error reading demarcation");
	mu_assert(cnf->mcast_port > 0, "Error reading port");
	mu_assert(cnf->mcast_grp_start != NULL, "Error reading multicast gropu ip");

	tva_conf_destroy(cnf);

	return NULL;
}

char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_conf);
	return NULL;
}

RUN_TESTS(all_tests);

