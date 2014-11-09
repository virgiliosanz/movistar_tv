#include "core/dbg.h"
#include "movistartv/xmltv.h"

static xmltv_error_t xmltv_programme_create(xmltv_programme_t *programme)
{
    return XMLTV_NOT_IMPLEMENTED;
}
static void xmltv_programme_destroy(xmltv_programme_t* programme)
{
    return;
}

static bstring xmltv_programme_to_xml(const xmltv_programme_t *programme)
{
    return NULL;
}

static xmltv_error_t xmltv_channel_create(xmltv_channel_t *channel)
{
    return XMLTV_NOT_IMPLEMENTED;
}

static void xmltv_channel_destroy(xmltv_channel_t* channel)
{
    return;
}

static bstring xmltv_channel_to_xml(const xmltv_channel_t *channel)
{
    return NULL;
}

xmltv_error_t xmltv_create(xmltv_t *xmltv)
{
    return XMLTV_NOT_IMPLEMENTED;
}

xmltv_error_t xmltv_add_channel(xmltv_t *xmltv, const xmltv_channel_t *channel)
{
    return XMLTV_NOT_IMPLEMENTED;
}

xmltv_error_t xmltv_add_programme(xmltv_t *xmltv, const xmltv_programme_t *programme)
{
    return XMLTV_NOT_IMPLEMENTED;
}

bstring xmltv_to_xml(const xmltv_t *xmltv)
{
    return NULL;
}

