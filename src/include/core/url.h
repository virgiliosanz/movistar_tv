#ifndef __url_h__
#define __url_h__

/*_
 * Copyright 2010 Scyphus Solutions Co. Ltd.  All rights reserved.
 *
 * Authors:
 *      Hirochika Asai
 * Modified by:
 *      Virgilio Sanz
 */

struct _url_s {
    char *scheme;               /* mandatory */
    char *host;                 /* mandatory */
    char *port;                 /* optional */
    char *path;                 /* optional */
    char *query;                /* optional */
    char *fragment;             /* optional */
    char *username;             /* optional */
    char *password;             /* optional */
};
typedef struct _url_s url_s;

url_s *url_parse(const char *url);
void url_free(url_s *url);

#define url_debug(url) trace("(url) s: %s h: %s p: %s p: %s q: %s f: %s u: %s p: %s", \
	url->scheme, url->host, url->port, url->path, url->query, \
	url->fragment, url->username, url->password) \

#endif

