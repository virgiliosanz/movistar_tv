#include <string.h>
#include <stdlib.h>
#include "config.h"

char *
test_net_http_get()
{
	char *body = mtv_http_get(bfromcstr("http://www.google.com"));
	mu_assert(body != NULL, "Error leyendo google.com")
	    mu_assert(strlen(body) > 0, "Tamaño de la respuesta es 0!");
	mu_assert(!strstr(body, "<html"), "No encuentro <html");
	mu_assert(!strstr(body, "</html>"), "No encuentro <html");
	mu_assert(!strstr(body, "<body"), "No encuentro <html");
	mu_assert(!strstr(body, "</body>"), "No encuentro <html");

	return NULL;
}

char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_net_http_get);
	return NULL;
}

RUN_TESTS(all_tests)
