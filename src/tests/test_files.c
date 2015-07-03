#include "config.h"

char *
test_readfile()
{
	char *contents;
	contents = read_file(__FILE__);
	mu_assert(NULL != contents, "contents is null: %s", contents);

	return NULL;
}

#define GOOD_DIR "./esto/es/un/directorio/largo/"
#define BAD_DIR "/esto/es/un/directorio/largo/"
char *
test_mkpath()
{
	trace("Creating path: %s %x", GOOD_DIR, 0777);
	int ret = mkpath(strdup(GOOD_DIR), 0777);
	mu_assert(0 == ret, "Error creating directory tree...");

	ret = mkpath(strdup(BAD_DIR), 0777);
	mu_assert(-1 == ret, "I cannot create that dir, so it should have failed");

	return NULL;
}


char *
test_rmrf()
{
	int ret = rmrf(strdup("esto"));
	trace("Return val: %d", ret);

	return NULL;
}

char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_readfile);
	mu_run_test(test_mkpath);
	mu_run_test(test_rmrf);
	return NULL;
}

RUN_TESTS(all_tests);
