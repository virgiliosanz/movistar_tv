#ifndef __xmltv_h__
#define __xmltv_h__

struct xmltv_channel_s {
    char *name;
};
typedef struct xmltv_channel_s xmltv_channel_t;

struct xmltv_programme_s {
    char *name;
};
typedef struct xmltv_programme_s xmltv_programme_t;

struct xmltv_s {
    xmltv_channel_t *channels;
    xmltv_programme_t *programmes;
};
typedef struct xmltv_s xmltv_t;

#endif

enum xmltv_error_e {
    XMLTV_OK = 0,
    XMLTV_UNKNOWN_ERROR,

    XMLTV_NOT_IMPLEMENTED,
    XMLTV_LAST_ERROR,
};
typedef enum xmltv_error_e xmltv_error_t;

xmltv_error_t xmltv_create(xmltv_t *xmltv);
xmltv_error_t xmltv_add_channel(xmltv_t *xmltv, const xmltv_channel_t *channel);
xmltv_error_t xmltv_add_programme(xmltv_t *xmltv, const xmltv_programme_t *programme);
char *xmltv_to_xml(const xmltv_t *xmltv);
