#include <string.h>
#include <stdlib.h>
#include "config.h"

char *
test_url(const char *url)
{
	trace("---- Testing: %s (%zu)", url, strlen(url));

	char *body = http_get(url);
//	trace("Body:\n%s\n", body);
	mu_assert(body != NULL, "Error leyendo %s", url);
	mu_assert(strlen(body) > 0, "Tamaño de la respuesta es 0!");
	mu_assert(strstr(body, "<html"), "No encuentro <html en: %s", body);
	mu_assert(strstr(body, "</html>"), "No encuentro </html %s", body);
	mu_assert(strstr(body, "<body"), "No encuentro <body %s", body);
	mu_assert(strstr(body, "</body>"), "No encuentro </body> %s", body);

	return NULL;
}
char *
test_http_get()
{

	char *urls[] = {
		"http://www.cocinario.es",
		"http://www.mediaset.es/presentadores-celebran-navidadMEDIASET_4_1908435010.html",
		"http://economia.elpais.com/economia/2014/12/15/actualidad/1418657657_039392.html",
		"http://91.216.63.241/economia/2014/12/11/actualidad/1418297517_574627.html",
		/*
		"http://172.26.22.23:2001/appserver/mvtv.do?action=getClientProfile",
		"http://172.26.22.23:2001/appserver/mvtv.do?action=getPlatformProfile",
		*/
		NULL
	};

	char **url = urls;
	char *rc;
	while (*url) {
		rc = test_url(*url);
		error_if(rc != NULL, error);
		url++;

	}
	return NULL;

error:
	return rc;
}


char *
all_tests()
{
	mu_suite_start();
	mu_run_test(test_http_get);
	return NULL;
}

RUN_TESTS(all_tests)

