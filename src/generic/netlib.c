#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <curl/curl.h>
#include "netlib.h"

/** Local helper funcions */
static size_t _curl_write_callback(char *buffer, size_t size, size_t nmemb, void *data)
{
    char **ptr =  (char**)data;
    *ptr = strndup(buffer, (size_t)(size *nmemb));
    return size;
}

char *http_get(const char *url)
{
    assert(NULL != url);

    CURL *curl = curl_easy_init();
    assert(curl != NULL);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_callback);

    char *body = NULL;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);

#ifdef NDEBUG
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
#endif

    CURLcode res = curl_easy_perform(curl);
    assert(res != CURLE_OK);
    curl_easy_cleanup(curl);

    return body;
}

