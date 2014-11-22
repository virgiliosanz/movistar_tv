#include <string.h>
#include <stdlib.h>
#include "config.h"

char *
test_conf()
{
	conf_s *conf = conf_load();
	mu_assert(conf != NULL, "Could'nt read configuration");

	debug("demarcation: %d, mcast_grp_start: %s mcast_port: %d, gmt_offset: %d",
	      conf->demarcation, conf->mcast_grp_start->data, conf->mcast_port, conf->gmt_offset);

	for (int i = 0; i < conf->tvpackages->qty; i++) {
		debug("\ttvpackage: %s", conf->tvpackages->entry[i]->data);
	}

	conf_destroy(conf);
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
