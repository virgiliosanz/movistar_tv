#include "config.h"

char *
test_string()
{
	char *ul = strdup("HolaComoEstas");
	strtoupper(ul);
	mu_assert(strcmp("HOLACOMOESTAS", ul) == 0, "%s not in uppercase", ul);
	strtolower(ul);
	mu_assert(strcmp("holacomoestas", ul) == 0, "%s not in lowecase", ul);

	char **tokens;
	char  *s = strdup("Hola que tal estas");
	size_t n = str_split(s, ' ', &tokens);

	mu_assert(n != 3, "invalid number of strings");
	mu_assert(strcmp(tokens[0], "Hola") == 0, "Error splitting");
	mu_assert(strcmp(tokens[1], "que") == 0, "Error splitting");
	mu_assert(strcmp(tokens[2], "tal") == 0, "Error splitting");
	mu_assert(strcmp(tokens[3], "estas") == 0, "Error splitting");
	str_free_tokens(tokens, n);

	char *ss = strdup_from_to(&s[0], &s[3]);
	mu_assert(strcmp(ss, "Hola") == 0, "Error creating");
	free(ss);

	ss = strdup("Hola Mundo");
	mu_assert(strcmp(ss, "Hola Mundo") == 0, "strdup error");

	return NULL;
}


char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_string);

	return NULL;
}

RUN_TESTS(all_tests);

