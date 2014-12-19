#include <core/all.h>

http_request_s *
http_request_create(const char *url)
{
	http_request_s *r = malloc(sizeof(http_request_s));
	error_if (NULL == r, error);

	r->url = url_parse(url);
	error_if (NULL == r->url, error);

	r->method  = strdup(HTTP_DEFAULT_METHOD);
	r->version = strdup(HTTP_DEFAULT_VERSION);
	r->server  = strdup(r->url->host);

	if (r->url->port) {
		r->port =  atoi(r->url->port);
	} else if (strcasecmp(r->url->scheme, "http") == 0) {
		r->port = 80;

	} else if (strcasecmp(r->url->scheme, "https") == 0) {
		r->port = 443;
	} else {
		warn("Don't know the port number for %s", r->url->scheme);
		goto error;
	}

	r->headers = list_create();

	http_request_debug(r);


	return r;

error:
	if (r) http_request_free(r);
	return NULL;
}


void
http_request_add_auth(http_request_s *r, const char *user, const char *pass)
{
	error_if(NULL == r, error);
	error_if(NULL == r->url, error);
	error_if(NULL == user, error);
	error_if(NULL == pass, error);

	if (r->url->username) free(r->url->username);
	r->url->username = strdup(user);

	if (r->url->password) free(r->url->password);
	r->url->password = strdup(pass);

error:
	return;
}

void
http_request_set_method(http_request_s *r, const char *method)
{
	error_if(NULL == r, error);
	error_if(NULL == method, error);
	if (r->method) free(r->method);
	r->method = strdup(method);

	strtoupper(r->method);

error:
	return;
}

void
http_request_set_version(http_request_s *r, const char *version)
{
	error_if(NULL == r, error);
	error_if(NULL == version, error);
	if (r->version) free(r->version);
	r->version = strdup(version);

error:
	return;
}

void
http_request_set_server(http_request_s *r, const char *server)
{
	error_if(NULL == r, error);
	error_if(NULL == server, error);
	if (r->server) free(r->server);
	r->server = strdup(server);

error:
	return;
}

void
http_request_set_port(http_request_s *r, const int port)
{
	error_if(NULL == r, error);
	r->port = port;
error:
	return;
}

static void
http_header_debug(http_header_s *h)
{
	trace("Header -> %s: %s", h->name, h->value);
}

void
http_header_free(http_header_s *h)
{
	if (!h) {
		warn("Header vacia");
		return;
	}

	if (h->name) free(h->name);
	if (h->value) free(h->value);
	free(h);
}

void
http_header_set_name(http_header_s *h, const char *name)
{
	error_if(h == NULL, error);
	if (h->name) free(h->name);
	h->name = strdup(name);
	return;
error:
	return;

}

void
http_header_set_value(http_header_s *h, const char *value)
{
	error_if(h == NULL, error);
	if (h->value) free(h->value);
	h->value = strdup(value);
	return;
error:
	return;

}

void
http_header_set(http_header_s *h, const char *name, const char *value)
{
	error_if(h == NULL, error);
	error_if(name == NULL, error);
	error_if(value  == NULL, error);

	http_header_set_name(h, name);
	http_header_set_value(h, value);

error:
	return;
}

http_header_s *
http_header_alloc()
{
	http_header_s *h = NULL;
	h = malloc(sizeof(http_header_s));
	error_if (NULL == h, error);

	h->name = NULL;
	h->value = NULL;

	return h;

error:
	if (h) http_header_free(h);
	return NULL;
}

void
http_request_add_header(http_request_s *r, const char *name, const char *value)
{
	http_header_s *h = NULL;

	error_if(NULL == r, error);
	error_if(NULL == name, error);
	error_if(NULL == value, error);

	h = http_header_alloc();
	http_header_set(h, name, value);

	// TODO: Check if header exists. If already exits concat with ', '
	list_push(r->headers, h);

error:
	return;
}

static void
_add_headers_to_sbuf(http_header_s *h, sbuf_s *buf)
{
	sbuf_appendf(buf, "%s:%s\r\n", h->name, h->value);
}


static char *
_request_to_str(const http_request_s *r)
{
	sbuf_s *s   = NULL;
	char   *str = NULL;

	http_request_debug(r);
	error_if(NULL == r, error);

	s = sbuf_new();

	char *path = r->url->path != NULL ? r->url->path : "";

	sbuf_printf(s, "%s /%s", r->method, path);
	if (r->url->query)
		sbuf_appendf(s, "?%s", r->url->query);
	sbuf_appendf(s, " HTTP/%s\r\n", r->version);
	sbuf_appendf(s, "Host: %s\r\n", r->url->host);
	list_walk_with_state(
		r->headers, (list_apply_with_state) _add_headers_to_sbuf, s);
	sbuf_appendstr(s, "\r\n");

	str = sbuf_detach(s);
	sbuf_delete(s);

	return str;

error:
	if (s) sbuf_delete(s);
	if (str) free(str);

	return NULL;
}
/*
static int
nonblock(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	error_if(flags >= 0, error)

	int rc = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	error_if(rc == 0, error);
	return 0;

error:
	return -1;
}
*/

static char *
_do_http_request(const char *host, const int port, const http_request_s *r)
{
	static const size_t BUFFER_SIZE     = 1024;
	char               *buffer[BUFFER_SIZE];
	sbuf_s             *s               = sbuf_new();

	int                 sock            = 0;
	int                 rc              = 0;
	struct sockaddr_in  addr;
	struct hostent     *ip              = NULL;

	trace("Resolving host: %s (%d)", host, port);
	ip = gethostbyname(host);
	error_if(NULL == ip, error);

	int i = 0;
	while (ip->h_addr_list[i] != NULL ) {
       		trace( "%s ", inet_ntoa(*(struct in_addr*)(ip->h_addr_list[i])));
		i++;
	}

	memset(buffer, 0, sizeof(buffer));
	sock = socket(AF_INET, SOCK_STREAM, 0);
	error_if(sock < 0, error);

	addr.sin_family = AF_INET;
	addr.sin_port   = htons(port);
	addr.sin_addr   = *((struct in_addr *)ip->h_addr);

	rc = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0) {
		error("Cannot connect to %s at port number %d", host, port)
		goto error;
	}

	char *str = _request_to_str(r);
	trace("Sending Request:\n------\n%s\n-----", str);
	write(sock, str, strlen(str));
	free(str);

	while((rc = read(sock, buffer, BUFFER_SIZE - 1)) != 0) {
		sbuf_appendbytes(s, (const char *)buffer, rc);
	}

	shutdown(sock, SHUT_RDWR);
	close(sock);

	trace("%d bytes readed", sbuf_len(s));

	char *response = sbuf_detach(s);
	sbuf_delete(s);

	return response;

error:
	if (s) sbuf_delete(s);
	shutdown(sock, SHUT_RDWR);
	close(sock);
	return NULL;
}

/*
static http_request_s *
_do_https_request(socket, req)
{

}
*/

static http_response_s *
_response_alloc()
{
	http_response_s *r = malloc(sizeof(http_response_s));
	error_if(r == NULL, error);
	r->headers = list_create();
	return r;

error:
	if (r) http_response_free(r);
	return NULL;
}

/*
 * Parsing Reponse (end of line is \r\n):
 *
 * HTTP/1.0 200 Ok
 * Cache-Control: private
 * Content-Type: text/html; charset=UTF-8
 * Location: http://www.google.es/?gfe_rd=cr&ei=fAGHVLHhEvKs8we33YDgAQ
 * Content-Length: 258
 * Date: Tue, 09 Dec 2014 14:04:44 GMT
 * Server: GFE/2.0
 * Alternate-Protocol: 80:quic,p=0.02
 *
 * <HTML>.....</HTML>
 */
http_response_s *
_parse_response(char *str)
{
	const size_t    HEADER_MAX = 1024;
	http_response_s *r         = _response_alloc();
	http_header_s       *header    = NULL;
	sbuf_s          *s         = sbuf_new();
	char             b[HEADER_MAX];

	error_if(r == NULL, error);
	error_if(str == NULL, error);

	strncpy(b, &str[9], 3);
	b[3] = 0;
	r->response_code = atoi(b);
	trace("Response code: %d", r->response_code);

	if (r->response_code != 200) {
		return (r);
	}

	// move to next line
	size_t     cnt            = 0;
	size_t     last_pos       = 0;

	// Move to second line
	for (cnt = 9; str[cnt] != '\n'; cnt ++) {}

	// Parse rest of headers and body
	cnt ++;
	last_pos = cnt;
	while (true) {
		if ('\r' == str[cnt]) {
			str[cnt] = '\0';
			cnt += 2;
			http_header_set_value(header, &str[last_pos]);
			list_push(r->headers, header);
			header = NULL;
			last_pos = cnt ;

			// Si \r\n\r\n terminan las cabeceras y empieza el body
			if ('\r' == str[cnt]) {
				cnt += 2; // Nos saltamos el \r\n
				r->body = strdup(&str[cnt]);
				break;
			}

		} else if (':' == str[cnt]){
			header = http_header_alloc();
			str[cnt] = '\0';
			http_header_set_name(header, &str[last_pos]);
			cnt ++; // Saltamos el :
			last_pos = cnt;
		}
		cnt++;
	}

	return r;

error:
	if (r) http_response_free(r);
	if (s) sbuf_delete(s);
	if (header) http_header_free(header);
	return NULL;
}

http_response_s *
http_do_request(const http_request_s *req)
{
	char            *res_str = NULL;
	http_response_s *res     = NULL;

	error_if(NULL == req, error);

	if (strcmp("GET", req->method) != 0) {
		error("%s", "Only GET method is supported");
		goto error;
	}

	if (strcasecmp(req->url->scheme, "http" ) == 0) {
		res_str = _do_http_request(req->server, req->port, req);
	}
	/*
	else if (strcasecmp(req->url->scheme, "https" ) == 0) {
		req_str = _do_https_request(socket, req);

	}
	*/
	else {
		error("%s schema is not supported", req->url->scheme);
		goto error;
	}

	res = _parse_response(res_str);
	error_if(NULL == res, error);
	free(res_str);

	return res;

error:
	if (res_str) free(res_str);
	if (res) http_response_free(res);
	return NULL;
}

void
http_request_free(http_request_s *r)
{
	error_if(NULL == r, error);
	if (r->url) url_free(r->url);
	if (r->version) free(r->version);
	if (r->server) free(r->server);
	if (r->headers) {
		list_walk(r->headers, (list_apply)http_header_free);
		list_destroy(r->headers);
	}

error:
	return;
}


char *
http_response_detach(http_response_s *r)
{
	error_if(NULL == r, error);

	char *body = r->body;
	r->body = NULL;
	return body;

error:
	return NULL;
}

void
http_response_free(http_response_s *r)
{
	error_if(NULL == r, error);
	if (r->body) free(r->body);
	if (r->headers) {
		list_walk(r->headers, (list_apply) http_header_free);
		list_destroy(r->headers);
	}
	free(r);
error:
	return;
}


char *
http_get(const char *url)
{
	http_request_s  *req = NULL;
	http_response_s *res = NULL;

	req = http_request_create(url);
	error_if(req == NULL, error);

	res = http_do_request(req);
	error_if(res == NULL, error);

	if (res->response_code != 200) {
		error("Response code not 200 -> %d", res->response_code);
		goto error;
	}

	// Debug headers
	trace("There are %d headers", list_count(res->headers));
	list_walk(res->headers, (list_apply) http_header_debug);

	char *body = http_response_detach(res);
	http_request_free(req);
	http_response_free(res);

	trace("Tamaño del body: %zu", strlen(body));


	return body;

error:
	if (req) http_request_free(req);
	if (res) http_response_free(res);
	return NULL;

}
