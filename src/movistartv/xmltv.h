#ifndef __xmltv_h__
#define __xmltv_h__

#include <time.h>
#include "bstrlib/bstrlib.h"
#include "core/list.h"

enum xmltv_error_e {
    XMLTV_OK = 0,
    XMLTV_UNKNOWN_ERROR,
    XMLTV_ALLOC_ERROR,

    XMLTV_NOT_IMPLEMENTED,
    XMLTV_LAST_ERROR,
};
typedef enum xmltv_error_e xmltv_error_t;

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
    bstring ating_value; /* system="MPAA" */
};
typedef struct xmltv_programme_s xmltv_programme_t;

struct xmltv_s {
    list_t *channels;
    list_t *programmes;
};
typedef struct xmltv_s xmltv_t;


xmltv_error_t xmltv_create(xmltv_t *xmltv);
xmltv_error_t xmltv_add_channel(xmltv_t *xmltv, const xmltv_channel_t *channel);
xmltv_error_t xmltv_add_programme(xmltv_t *xmltv, const xmltv_programme_t *programme);
bstring xmltv_to_xml(const xmltv_t *xmltv);

#endif
