#ifndef __http_h__
#define __http_h__

#include <core/all.h>

struct http_request {
	struct url *url;
	char       *method;
	char       *version;
	char       *server;
	int         port;
	list_s     *headers;
};

#define http_request_debug(r) \
trace("m: %s v: %s s: %s p: %d", (r)->method, (r)->version, (r)->server, (r)->port); \
url_debug((r)->url)

struct http_response {
	int     response_code;
	char   *body;
	list_s *headers;
};

/*
 * Simplified methods
 */
char *http_get(const char *url);

/*
 * Generic methods for better control
 */
struct http_request  *http_request_create(const char *url);
struct http_response *http_do_request(const struct http_request *req);

void http_request_add_auth(struct http_request *req, const char *user, const char *pass);
void http_request_set_method(struct http_request *req, const char *method);
void http_request_set_version(struct http_request *req, const char *version);
void http_request_set_server(struct http_request *req, const char *server);
void http_request_set_port(struct http_request *req, const int port);
void http_request_add_header(struct http_request *req, const char *name, const char *value);
void http_request_free(struct http_request *req);

char *http_response_detach(struct http_response *res);
void  http_response_free(struct http_response *res);

#define HTTP_DEFAULT_METHOD "GET"
#define HTTP_DEFAULT_VERSION "1.0"
#define HTTP_TIMEOUT (5)

struct http_header {
	char *name;
	char *value;
};
void http_header_free(struct http_header *h);
void http_header_set_name(struct http_header *h, const char *name);
void http_header_set_value(struct http_header *h, const char *value);
void http_header_set(struct http_header *h, const char *name, const char *value);
struct http_header *http_header_alloc();

#endif

