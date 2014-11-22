#include <string.h>
#include <stdlib.h>
#include "config.h"

char *
test_net_http_get()
{
	bstring body = net_http_get(bfromcstr("http://www.google.com"));
	mu_assert(body != NULL, "Error leyendo google.com")
	    mu_assert(blength(body) > 0, "Tamaño de la respuesta es 0!");
	mu_assert(binstr(body, 0, bfromcstr("<html")) != BSTR_ERR, "No encuentro <html");
	mu_assert(binstr(body, 0, bfromcstr("</html>")) != BSTR_ERR, "No encuentro <html");
	mu_assert(binstr(body, 0, bfromcstr("<body")) != BSTR_ERR, "No encuentro <html");
	mu_assert(binstr(body, 0, bfromcstr("</body>")) != BSTR_ERR, "No encuentro <html");

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
