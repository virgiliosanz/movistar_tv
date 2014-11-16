#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "core/dbg.h"
#include "core/net.h"
#include "bstrlib/bstrlib.h"

static size_t _curl_write_memory_callback(void *ptr, size_t size, size_t nmemb, bstring buffer)
{
    const char *c = (const char *)ptr;
    debug("Data: %d Ptr: %zu", blength(buffer), strlen(c));
    // debug("%s", c);
    bcatcstr(buffer, c);

    return size * nmemb;
}

bstring net_http_get(const bstring url)
{
    CURL *curl = NULL;
    bstring buffer = bfromcstr("");

    check(NULL != url, "Url is undefined");
    debug("Loading %s ...", url->data);

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    check(curl != NULL, "Cannot initialize curl_easy_init");

    curl_easy_setopt(curl, CURLOPT_URL, url->data);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_memory_callback);
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

    debug("... %d bytes read.", blength(buffer));

    return buffer;

error:
    if (curl) curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (buffer) bdestroy(buffer);
    return NULL;
}

