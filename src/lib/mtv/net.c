#include <mtv/all.h>

static size_t
_curl_callback(void *ptr, size_t size, size_t nmemb, sbuf_s *buffer)
{
	const char *c = (const char *)ptr;
	debug("Data: %d Ptr: %zu", sbuf_len(buffer), strlen(c));
	// debug("%s", c);
	sbuf_appendstr(buffer, c);

	return size * nmemb;
}

char *
mtv_http_get(const char *url)
{
	CURL       *curl   = NULL;
	sbuf_s *buffer = sbuf_new();

	check(NULL != url, "Url is undefined");
	debug("Loading %s ...", url);

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	check(curl != NULL, "Cannot initialize curl_easy_init");

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

#ifndef NDEBUG
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
#else
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

	CURLcode res = curl_easy_perform(curl);
	check(res == CURLE_OK, "Curl couln't get url");
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	debug("... %d bytes read.", sbuf_len(buffer));

	char *rv = sbuf_detach(buffer);
	sbuf_delete(buffer);

	return rv;

 error:
	if (curl) curl_easy_cleanup(curl);
	curl_global_cleanup();
	if (buffer) sbuf_delete(buffer);

	return NULL;
}

