#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "dbg.h"
#include "net.h"

struct _buffer_s
{
    char  *ptr;
    size_t size;
};

static size_t _curl_write_memory_callback(void *ptr, size_t size, size_t nmemb, void *data)
{
    struct _buffer_s *b = (struct _buffer_s *) data;
    size_t ptr_size = size * nmemb;

    b->ptr = (char *)realloc(b->ptr, b->size + ptr_size + 1);
    check_mem(b->ptr);
    memcpy(&(b->ptr[b->size]), ptr, ptr_size);
    b->size += ptr_size;
    b->ptr[b->size] = '\0';
    return ptr_size;

error:
    if (b->ptr) free(b->ptr);
    return -1;
}


char *net_http_get(const char *url)
{
    CURL *curl = NULL;
    struct _buffer_s buffer;
    buffer.ptr = malloc(1);  /* will be grown as needed by the realloc above */
    buffer.size = 0;

    check(NULL != url, "Url is undefined");
    debug("Loading %s ...", url);

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    check(curl != NULL, "Cannot initialize curl_easy_init");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_memory_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
#endif

    CURLcode res = curl_easy_perform(curl);
    check(res == CURLE_OK, "Curl couln't get url");
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    debug("... leidos %zu bytes", buffer.size);

    return buffer.ptr;

error:
    if (curl) curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (buffer.ptr) free(buffer.ptr);
    return NULL;
}

