#ifndef __xmltv_h__
#define __xmltv_h__

#include <time.h>
#include "movistartv/xmltv_dtd.h"
#include "bstrlib/bstrlib.h"
#include "core/list.h"

struct xmltv_channel_s {
    bstring id;
    bstring display_name;
};
typedef struct xmltv_channel_s xmltv_channel_t;

struct xmltv_programme_s {
    struct tm start;
    struct tm_date;
    bstring channel;
    bstring title;
    bstring desc;
    bstring director;
    list_t  *actors;
    bstring country;
    bstring episode_num; /* system="xmltv_ns" */
    bstring aspect;
    bstring rating_value; /* system="MPAA" */
};
typedef struct xmltv_programme_s xmltv_programme_t;

struct xmltv_s {
    list_t *channels;
    list_t *programmes;
};
typedef struct xmltv_s xmltv_t;

xmltv_programme_t *xmltv_programme_alloc();
void xmltv_programme_free(xmltv_programme_t* programme);

xmltv_channel_t *xmltv_channel_alloc();
void xmltv_channel_free(xmltv_channel_t* channel);

xmltv_t *xmltv_alloc();
void xmltv_free(xmltv_t *xmltv);
void xmltv_add_channel(xmltv_t *xmltv, const xmltv_channel_t *channel);
void xmltv_add_programme(xmltv_t *xmltv, const xmltv_programme_t *programme);
bstring xmltv_to_xml(const xmltv_t *xmltv);
char *xmltv_validate_dtd(const char *xml);

#endif

