#include "config.h"

char *
test_url()
{
	struct {
		char  *str_url;
		url_s url;
	} urls[] = {
		{"http://www.midominio.com/", {
			.scheme   = "http",
			.host     = "www.midominio.com",
			.port     = NULL,
			.path     = NULL,
			.query    = NULL,
			.fragment = NULL,
			.username = NULL,
			.password = NULL}
		},
		{"http://www.midominio.com:8000", {
			.scheme   = "http",
			.host     = "www.midominio.com",
			.port     = "8000",
			.path     = NULL,
			.query    = NULL,
			.fragment = NULL,
			.username = NULL,
			.password = NULL}
		},
		{"http://www.midominio.com/index.html", {
			.scheme   = "http",
			.host     = "www.midominio.com",
			.port     = NULL,
			.path     = "index.html",
			.query    = NULL,
			.fragment = NULL,
			.username = NULL,
			.password = NULL}
		},
		{"http://www.midominio.com/index.html?uno=1&dos=2", {
			.scheme   = "http",
			.host     = "www.midominio.com",
			.port     = NULL,
			.path     = "index.html",
			.query    = "uno=1&dos=2",
			.fragment = NULL,
			.username = NULL,
			.password = NULL}
		},
		{"http://www.midominio.com/?uno=1&dos=2", {
			.scheme   = "http",
			.host     = "www.midominio.com",
			.port     = NULL,
			.path     = NULL,
			.query    = "uno=1&dos=2",
			.fragment = NULL,
			.username = NULL,
			.password = NULL}
		},
		{"http://www.midominio.com/?uno=1&dos=2#frag", {
			.scheme   = "http",
			.host     = "www.midominio.com",
			.port     = NULL,
			.path     = NULL,
			.query    = "uno=1&dos=2",
			.fragment = "frag",
			.username = NULL,
			.password = NULL}
		},
		{"http://user@www.midominio.com/?uno=1&dos=2#frag", {
			.scheme   = "http",
			.host     = "www.midominio.com",
			.port     = NULL,
			.path     = NULL,
			.query    = "uno=1&dos=2",
			.fragment = "frag",
			.username = "user",
			.password = NULL}
		},
		{"http://user:pass@www.midominio.com/?uno=1&dos=2#frag", {
			.scheme   = "http",
			.host     = "www.midominio.com",
			.port     = NULL,
			.path     = NULL,
			.query    = "uno=1&dos=2",
			.fragment = "frag",
			.username = "user",
			.password = "pass"}
		},
		{"https://user:pass@www.midominio.com/?uno=1&dos=2#frag", {
			.scheme   = "https",
			.host     = "www.midominio.com",
			.port     = NULL,
			.path     = NULL,
			.query    = "uno=1&dos=2",
			.fragment = "frag",
			.username = "user",
			.password = "pass"}
		},
		{"https://user:pass@www.midominio.com:8080/home.html?uno=1&dos=2#frag", {
			.scheme   = "https",
			.host     = "www.midominio.com",
			.port     = "8080",
			.path     = "home.html",
			.query    = "uno=1&dos=2",
			.fragment = "frag",
			.username = "user",
			.password = "pass"}
		},
		{"http://172.26.22.23:2001/appserver/mvtv.do?action=getClientProfile", {
			.scheme   = "http",
			.host     = "172.26.22.23",
			.port     = "2001",
			.path     = "appserver/mvtv.do",
			.query    = "action=getClientProfile",
			.fragment = NULL,
			.username = NULL,
			.password = NULL}
		},
		{"http://172.26.22.23:2001/appserver/mvtv.do?action=getPlatformProfile", {
			.scheme   = "http",
			.host     = "172.26.22.23",
			.port     = "2001",
			.path     = "appserver/mvtv.do",
			.query    = "action=getPlatformProfile",
			.fragment = NULL,
			.username = NULL,
			.password = NULL}
		},
		{NULL, {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}}
	};


	url_s *url = NULL;
	for (size_t i = 0; urls[i].str_url != NULL; i++) {
		trace("\n\nParsing URL: %s", urls[i].str_url);
		url = url_parse(urls[i].str_url);
		mu_assert(url != NULL, "%s parses to NULL", urls[i].str_url);

		trace("%s -> %s %s %s %s %s %s %s %s ->  %s %s %s %s %s %s %s %s", urls[i].str_url,
			urls[i].url.scheme, urls[i].url.host, urls[i].url.port,
			urls[i].url.path, urls[i].url.query, urls[i].url.fragment,
			urls[i].url.username, urls[i].url.password,

			url->scheme, url->host, url->port,
			url->path, url->query, url->fragment,
			url->username, url->password);

		if (urls[i].url.scheme != NULL)
 			mu_assert(strcmp(urls[i].url.scheme, url->scheme) == 0, "scheme don't match: %s = %s", urls[i].url.scheme, url->scheme);
		if (urls[i].url.host != NULL)
			mu_assert(strcmp(urls[i].url.host, url->host) == 0, "host don't match: %s = %s", urls[i].url.host, url->host);
		if (urls[i].url.port != NULL)
			mu_assert(strcmp(urls[i].url.port, url->port) == 0, "port don't match: %s = %s", urls[i].url.port, url->port);
		if (urls[i].url.path != NULL)
			mu_assert(strcmp(urls[i].url.path, url->path) == 0, "path don't match: %s = %s", urls[i].url.path, url->path);
		if (urls[i].url.query != NULL)
			mu_assert(strcmp(urls[i].url.query, url->query) == 0, "query don't match: %s = %s", urls[i].url.query, url->query);
		if (urls[i].url.fragment != NULL)
			mu_assert(strcmp(urls[i].url.fragment, url->fragment) == 0, "fragment don't match: %s = %s", urls[i].url.fragment, url->fragment);
		if (urls[i].url.username != NULL)
			mu_assert(strcmp(urls[i].url.username, url->username) == 0, "username don't match: %s = %s", urls[i].url.username, url->username);
		if (urls[i].url.password != NULL)
			mu_assert(strcmp(urls[i].url.password, url->password) == 0, "password don't match: %s = %s", urls[i].url.password, url->password);
		url_free(url);
	}


	return NULL;
}


char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_url);

	return NULL;
}

RUN_TESTS(all_tests);

