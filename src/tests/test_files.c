#include "config.h"

char *
test_readfile()
{

	return "Not Implemented";
}

char *
test_mkpath()
{
	return "Not Implemented";
}


char *
test_rmpath()
{
	return "Not Implemented";
}

char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_readfile);
	return NULL;
}

RUN_TESTS(all_tests);

