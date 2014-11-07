#include "generic/dbg.h"
#include <stdlib.h>
#include <check.h>

int test_sentinel(int code)
{
    char *temp = malloc(100);
    check_mem(temp);

    switch(code) {
        case 1:
            log_info("It worked.");
            break;
        default:
            sentinel("I shouldn't run.");
    }

    free(temp);
    return 0;

error:
    if(temp) free(temp);
    return -1;
}

int test_check_mem()
{
    char *test = NULL;
    check_mem(test);

    free(test);
    return 1;

error:
    return -1;
}

int test_check_debug()
{
    int i = 0;
    check_debug(i != 0, "Oops, I was 0.");

    return 0;
error:
    return -1;
}

START_TEST(test_debug)
{
    // notice you don't need the \n
    debug("I have Brown Hair.");

    // passing in arguments like printf
    debug("I am %d years old.", 37);
}
END_TEST

START_TEST(test_log_err)
{
    log_err("I believe everything is broken.");
    log_err("There are %d problems in %s.", 0, "space");
}
END_TEST

START_TEST(test_log_warn)
{
    log_warn("You can safely ignore this.");
    log_warn("Maybe consider looking at: %s.", "/etc/passwd");
}
END_TEST

START_TEST(test_log_info)
{
    log_info("Well I did something mundane.");
    log_info("It happened %f times today.", 1.3f);
}
END_TEST


START_TEST(test_silly_things)
{
    check(test_sentinel(1) == 0, "test_sentinel failed.");
    check(test_sentinel(100) == -1, "test_sentinel failed.");
    check(test_check_mem() == -1, "test_check_mem failed.");
    check(test_check_debug() == -1, "test_check_debug failed.");

error:
    return;
}
END_TEST

int main(int argc, char *argv[])
{
    Suite *s = suite_create("Test DBG Library for logging");
    TCase *tc_core = tcase_create("tests");
    tcase_add_test(tc_core, test_debug);
    tcase_add_test(tc_core, test_log_err);
    tcase_add_test(tc_core, test_log_warn);
    tcase_add_test(tc_core, test_log_info);
    tcase_add_test(tc_core, test_silly_things);
    suite_add_tcase(s, tc_core);

    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    int failed = srunner_ntests_failed (sr);

error:
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
