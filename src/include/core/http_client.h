#ifndef __http_h__
#define __http_h__

#include <core/all.h>

struct _http_request_s {
	url_s         *url;
	char          *method;
	char          *version;
	char          *server;
	int            port;
	list_s        *headers;
};
typedef struct _http_request_s http_request_s;

#define http_request_debug(r) \
trace("m: %s v: %s s: %s p: %d", (r)->method, (r)->version, (r)->server, (r)->port); \
url_debug((r)->url)

struct _http_response_s {
	int     response_code;
	char   *body;
	list_s *headers;
};
typedef struct _http_response_s http_response_s;

/*
 * Simplified methods
 */
char *http_get(const char *url);

/*
 * Generic methods for better control
 */
http_request_s *http_request_create(const char *url);
http_response_s *http_do_request(const http_request_s *req);

void http_request_add_auth(http_request_s *req, const char *user, const char *pass);
void http_request_set_method(http_request_s *req, const char *method);
void http_request_set_version(http_request_s *req, const char *version);
void http_request_set_server(http_request_s *req, const char *server);
void http_request_set_port(http_request_s *req, const int port);
void http_request_add_header(http_request_s *req, const char *name, const char *value);
void http_request_free(http_request_s *req);

char *http_response_detach(http_response_s *res);
void http_response_free(http_response_s *res);

#define HTTP_DEFAULT_METHOD "GET"
#define HTTP_DEFAULT_VERSION "1.0"
#define HTTP_TIMEOUT (5)

struct _http_header_s {
	char *name;
	char *value;
};
typedef struct _http_header_s http_header_s;
void http_header_free(http_header_s *h);
void http_header_set_name(http_header_s *h, const char *name);
void http_header_set_value(http_header_s *h, const char *value);
void http_header_set(http_header_s *h, const char *name, const char *value);
http_header_s *http_header_alloc();

#endif

