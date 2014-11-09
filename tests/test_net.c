#include "core/minunit.h"
#include <string.h>
#include <stdlib.h>
#include "core/net.h"

char *test_net_http_get()
{
    bstring body = net_http_get("http://www.google.com");
    mu_assert(blength(body) > 0, "Tamaño de la respuesta es 0!");

    const char *cbody = bstr2cstr(body, ' ');
    mu_assert(strlen(cbody) > 0, "Tamaño de la respuesta es 0!");

    mu_assert(strstr(cbody, "<html") != NULL, "No encuentro <html");
    mu_assert(strstr(cbody, "</html>") != NULL, "No encuentro <html");
    mu_assert(strstr(cbody, "<body") != NULL, "No encuentro <html");
    mu_assert(strstr(cbody, "</body>") != NULL, "No encuentro <html");


    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_net_http_get);
    return NULL;
}

RUN_TESTS(all_tests)
