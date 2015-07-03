#include "config.h"

char *
test_case_transformations()
{
    char *s = strdup("HolaComoEstas");

    strtoupper(s);
    mu_assert(strcmp("HOLACOMOESTAS", s) == 0, "%s not in uppercase", s);

    strtolower(s);
    mu_assert(strcmp("holacomoestas", s) == 0, "%s not in lowecase", s);

    return NULL;
}

char *
test_split()
{
    char **tokens;
    char  *s = "Hola que tal estas";
    size_t n = str_split(s, ' ', &tokens);

    mu_assert(n != 3, "invalid number of strings");
    mu_assert(strcmp(tokens[0], "Hola") == 0, "Error splitting");
    mu_assert(strcmp(tokens[1], "que") == 0, "Error splitting");
    mu_assert(strcmp(tokens[2], "tal") == 0, "Error splitting");
    mu_assert(strcmp(tokens[3], "estas") == 0, "Error splitting");
    str_free_tokens(tokens, n);

    return NULL;
}

char *
test_duplication()
{
    char  *s = "Hola que tal estas";
    char *ss = NULL;

    ss = strdup_from_to(&s[0], &s[3]);
    mu_assert(strcmp(ss, "Hola") == 0, "'%s' != 'Hola'", ss);
    free(ss);

    ss = strdup_from_to(&s[5], &s[7]);
    mu_assert(strcmp(ss, "que") == 0, "'%s' != 'que'", ss);
    free(ss);

    ss = strdup_from_to(&s[5], NULL);
    mu_assert(ss == NULL, "'%s' != NULL", ss);
    free(ss);

    return NULL;
}

#define CHECK_TRIMMED(test_func, in, out, good) do { \
    out = test_func(in); \
    mu_assert(0 == strcmp(out, good), "%s('%s') - '%s' != '%s'", #test_func, in, out, good) \
} while (0);


char *
test_trim()
{
    char *s = strdup("Hola");
    char *trimmed = NULL;

    CHECK_TRIMMED(ltrim, s, trimmed, s);
    CHECK_TRIMMED(rtrim, s, trimmed, s);
    CHECK_TRIMMED(trim, s, trimmed, s);
    free(s);

    s = strdup("    Hola    ");
    CHECK_TRIMMED(ltrim, s, trimmed, "Hola    ");
    CHECK_TRIMMED(rtrim, s, trimmed, "    Hola");
    CHECK_TRIMMED(trim, s, trimmed, "Hola");
    free (s);

    return NULL;
}

char *
all_tests()
{
    mu_suite_start();

    mu_run_test(test_case_transformations);
    mu_run_test(test_split);
    mu_run_test(test_duplication);
    mu_run_test(test_trim);

    return NULL;
}

RUN_TESTS(all_tests);

