#include <pcre.h>
#include <regex.h>
#include "core/minunit.h"
#include <assert.h>
#include "core/list.h"
/*
char *test_pcre(const char *txt, const char *regex)
{
    pcre *re = NULL;
    pcre_extra *re_extra = NULL;
    const char *re_error = NULL;
    int result;
    static size_t vec_len = 30;
    int vec[vec_len];
    int re_offset;
    const char *match = NULL;

    debug("Trying to match: \"%s\" in \"%s\"", regex, txt);
    re = pcre_compile(regex, 0, &re_error, &re_offset, NULL);
    debug("Compile result is %s", re ? "OK" : "KO");
    mu_assert(re, "Error in regex");

    re_extra = pcre_study(re, 0, &re_error);
    mu_assert(NULL != re_extra, "Error in study");
    result = pcre_exec(re, re_extra, txt, strlen(txt), 0, 0 , vec, vec_len);

    result = pcre_exec(re, NULL, txt, strlen(txt), 0, 0 , vec, vec_len);
    debug("result = %d in regex: \"%s\" for \"%s\"", result, regex, txt);
    mu_assert(0 <= result, "Error executing regex");
    pcre_get_substring(txt, vec, result, 0, &(match));
    mu_assert(match, "No hubo resultados")

    pcre_free_substring(match);
    pcre_free(re);
    pcre_free(re_extra);

    return NULL;
}
*/

char *test_pcre(const char *txt, const char *regex)
{
    regex_t re;
    int reti;
    char msgbuf[100];

    debug("Attempting to match \"%s\" on \"%s\"", regex, txt);
    reti = regcomp(&re, regex, 0);
    mu_assert(!reti, "Could not compile regex");

    /* Execute regular expression */
    reti = regexec(&re, txt, 0, NULL, 0);
    mu_assert(!reti, "No Match!!");

    regerror(reti, &re, msgbuf, sizeof(msgbuf));
    debug("Regex match?: %s\n", msgbuf);

    regfree(&re);

    return NULL;

}

char *test_regex()
{
    char *result;

    result = test_pcre("abc", "^a[[:alnum:]]");
    mu_assert(!result, "No Match");

    result = test_pcre("bananas nefertiti", "\\(ba\\(na\\)*s \\|nefer\\(ti\\)* \\)");
    mu_assert(!result, "No Match");

    result = test_pcre("Un texto", "\\(.\\)*");
    mu_assert(!result, "No Match");


    result = test_pcre("Un texto", "Un [[:alnum]])");
    mu_assert(!result, "No Match");

    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_regex);

    return NULL;
}

RUN_TESTS(all_tests)
