#include "config.h"

char *
test_multicast()
{
	mcast_s *mcast = NULL;
	mcast = mcast_alloc();
	mu_assert(NULL != mcast, "mcast is NULL!!");

	mcast_open(mcast, "239.0.2.129", 3937);

	mcast_close(mcast);

	return NULL;
}

char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_multicast);
	return NULL;
}

RUN_TESTS(all_tests)

