#include "config.h"

char *
test_parse()
{

	return "Not Implemented";
}


char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_parse);
	return NULL;
}

RUN_TESTS(all_tests);

