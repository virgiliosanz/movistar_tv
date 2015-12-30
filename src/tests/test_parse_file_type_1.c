#include "config.h"

const char* fname = "misc/data/239.0.2.129/1_0.xml";

char* test_parse()
{
	struct mcast* mcast;

	char* doc = read_file(fname);
	mu_assert(doc, "Error reading %s: %s", fname, strerror(errno));

	mcast = mtv_parse_file_type_1(doc, 13);
	mu_assert(mcast != NULL, "Error reading demarcation");
	mcast_debug(mcast);
	mcast_free(mcast);

	mcast = mtv_parse_file_type_1(doc, 19);
	mu_assert(mcast != NULL, "Error reading demarcation");
	mcast_debug(mcast);
	mcast_free(mcast);

	return NULL;
}

char* all_tests()
{
	mu_suite_start();
	mu_run_test(test_parse);

	return NULL;
}

RUN_TESTS(all_tests)
