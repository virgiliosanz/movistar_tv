#include <pcre.h>
#include <regex.h>
#include "config.h"

char *
test_posix(const char *txt, const char *regex)
{
	regex_t re;
	int reti;
	char msgbuf[100];

	debug("Matchng \"%s\" on \"%s\"", regex, txt);
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

char *
test_regex_posix()
{
	char *result;

	result = test_posix("abc", "^a[[:alnum:]]");
	mu_assert(!result, "No Match");

	result = test_posix("bananas nefertiti", "\\(ba\\(na\\)*s \\|nefer\\(ti\\)* \\)");
	mu_assert(!result, "No Match");

	result = test_posix("Un texto", "\\(.\\)*");
	mu_assert(!result, "No Match");

	result = test_posix("Un texto", "Un [[:alnum]])");
	mu_assert(!result, "No Match");

	return NULL;
}

char *
test_pcre(const char *txt, const char *regex)
{
	pcre *re = NULL;
	pcre_extra *re_extra = NULL;
	const char *re_error = NULL;
	int result;
	static size_t vec_len = 30;
	int vec[vec_len];
	int re_offset;
	const char *match = NULL;

	debug("Attempting to match: \"%s\" in \"%s\"", regex, txt);
	re = pcre_compile(regex, 0, &re_error, &re_offset, NULL);
	debug("Compile result is %s", re ? "OK" : "KO");
	mu_assert(re, "Error in regex");
/*
    re_extra = pcre_study(re, 0, &re_error);
    mu_assert(NULL != re_extra, "Error in study");
    result = pcre_exec(re, re_extra, txt, strlen(txt), 0, 0 , vec, vec_len);
*/
	result = pcre_exec(re, NULL, txt, strlen(txt), 0, 0, vec, vec_len);
	debug("result = %d", result);
	mu_assert(0 <= result, "Error executing regex");
	for (int i = 0; i < result; i++) {
		pcre_get_substring(txt, vec, result, i, &(match));
		debug("Match(%2d/%2d): (%2d,%2d): '%s'",
		      i, result - 1, vec[i * 2], vec[i * 2 + 1], match);
	}

	pcre_free_substring(match);
	pcre_free(re);
	pcre_free(re_extra);

	return NULL;
}

char *
test_regex_pcre()
{
	char *result;

	result = test_pcre("Un texto", "(.*) texto");
	mu_assert(!result, "No Match");
	debug("------");

	result = test_pcre("abc", "^a(.*)$");
	mu_assert(!result, "No Match");
	debug("------");

	result = test_pcre("bananas nefertiti", "nan.* nefer");
	mu_assert(!result, "No Match");
	debug("------");

	return NULL;
}

char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_regex_pcre);

	return NULL;
}

RUN_TESTS(all_tests)
