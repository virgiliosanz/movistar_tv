#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/xmlstring.h>
#include <libxml2/libxml/valid.h>
#include <libxml2/libxml/xmlmemory.h>
#include <time.h>

#include "bstrlib/bstrlib.h"
#include "core/dbg.h"
#include "core/xmltv.h"
#include "core/xmltv_dtd.h"

static void _channels_to_xml(xmlNodePtr root, const list_t *channels);
static void _free_programmes(xmltv_t *xmltv);
static void _free_channels(xmltv_t *xmltv);
static void _actors_to_xml(xmlNodePtr node, const list_t *actors);
static void _programmes_to_xml(xmlNodePtr root, const list_t *programmes);

bstring xmltv_to_xml(const xmltv_t *xmltv)
{
    xmlDocPtr  doc = NULL;
    xmlDtdPtr dtd = NULL;
    xmlNodePtr root = NULL;
    xmlNodePtr cur = NULL;

    LIBXML_TEST_VERSION;

    doc  = xmlNewDoc((const xmlChar *)"1.0"); check_mem(doc);
    root = xmlNewDocNode(doc, NULL, (const xmlChar *)"tv", NULL); check_mem(root);
    dtd  = xmlCreateIntSubset(doc, BAD_CAST "tv", NULL, BAD_CAST "xmltv.dtd"); check_mem(dtd);
    xmlDocSetRootElement(doc, root);

    xmlNewProp(root,
            BAD_CAST "generator-info-name",
            BAD_CAST "movistar_tv - https://github.com/virgiliosanz/movistar_tv");

    _channels_to_xml(root, xmltv->channels);
    _programmes_to_xml(root, xmltv->programmes);

    xmlChar *s; int size;
    /* xmlDocDumpMemory(doc, &s, &size); */
    xmlDocDumpMemoryEnc(doc, &s, &size, "UTF-8");

    xmlCleanupParser();

    bstring b = bfromcstr((char *)s);
    free(s);

    return b;

error:
    xmlCleanupParser();

    return NULL;
}

bstring xmltv_channel_to_m3u(const xmltv_channel_t *chan)
{
    debug("Adding m3u: %d - %s", chan->order, chan->display_name->data);
    bstring b = bformat("#EXTM3U\n#EXINF:-1,%d - %s\n#EXTTV:%s;es;%s;%s\nrtp://@%s:%d\n",
            chan->order, chan->display_name->data,
            chan->tags->data, chan->id->data, chan->icon->data,
            chan->ip->data, chan->port);

    return b;

}

bstring xmltv_channel_to_m3usimple(const xmltv_channel_t *chan)
{
    debug("Adding m3usimple: %d - %s", chan->order, chan->display_name->data);
    bstring b = bformat("#EXTM3U\n#EXTINF:-1 tvg-id=%s tvg-logo=%s, %s\nrtp://@%s:%d\n",
            chan->short_name->data, chan->icon->data, chan->display_name->data,
            chan->ip->data, chan->port);
    return b;
}


/** Programmes **/
xmltv_programme_t *xmltv_programme_alloc()
{
    xmltv_programme_t *prog = (xmltv_programme_t *)malloc(sizeof(xmltv_programme_t));
    check_mem(prog);

    prog->channel = bfromcstr("");
    prog->title = bfromcstr("");
    prog->desc = bfromcstr("");
    prog->director = bfromcstr("");
    prog->country = bfromcstr("");
    prog->episode_num = bfromcstr("");
    prog->aspect = bfromcstr("");
    prog->rating_value = bfromcstr("");
    prog->rating_icon = bfromcstr("");
    prog->star_rating = bfromcstr("");

    prog->actors = list_create();

    return prog;

error:
    if (prog) xmltv_programme_free(prog);
    return NULL;
}

void xmltv_programme_free(xmltv_programme_t* prog)
{
    if (!prog) return;

    bdestroy(prog->channel);
    bdestroy(prog->title);
    bdestroy(prog->desc);
    bdestroy(prog->director);
    bdestroy(prog->country);
    bdestroy(prog->episode_num);
    bdestroy(prog->aspect);
    bdestroy(prog->rating_value);
    bdestroy(prog->rating_icon);
    bdestroy(prog->star_rating);

    list_foreach(prog->actors, first, next, cur) bdestroy((bstring)cur->value);
    list_destroy(prog->actors);
}

/** Channels **/
xmltv_channel_t *xmltv_channel_alloc()
{
    xmltv_channel_t *chan = (xmltv_channel_t *)malloc(sizeof(xmltv_channel_t));
    check_mem(chan);

    chan->id = bfromcstr("");
    chan->display_name = bfromcstr("");
    chan->short_name = bfromcstr("");
    chan->url = bfromcstr("");
    chan->icon = bfromcstr("");
    chan->ip = bfromcstr("");
    chan->port = 0;
    chan->tags = bfromcstr("");
    chan->order = 0;

    return chan;

error:
    if (chan) xmltv_channel_free(chan);
    return NULL;
}

void xmltv_channel_free(xmltv_channel_t* chan)
{
    if (!chan) return;
    bdestroy(chan->id);
    bdestroy(chan->display_name);
    bdestroy(chan->short_name);
    bdestroy(chan->ip);
    bdestroy(chan->tags);
    free(chan);
}

/** xmltv **/
xmltv_t *xmltv_alloc()
{
    xmltv_t *xmltv = (xmltv_t *)malloc(sizeof(xmltv_t));
    check_mem(xmltv);

    xmltv->channels = list_create();
    check_mem(xmltv->channels);

    xmltv->programmes = list_create();
    check_mem(xmltv->programmes);

    return xmltv;

error:
    if(xmltv) xmltv_free(xmltv);
    return NULL;
}

static void _free_programmes(xmltv_t *xmltv)
{
    list_foreach(xmltv->programmes, first, next, cur) {
        xmltv_programme_free((xmltv_programme_t *)cur->value);
    }
    list_destroy(xmltv->programmes);
}

static void _free_channels(xmltv_t *xmltv)
{
    list_foreach(xmltv->channels, first, next, cur) {
        xmltv_channel_free((xmltv_channel_t *)cur->value);
    }
    list_destroy(xmltv->channels);
}

void xmltv_free(xmltv_t *xmltv)
{
    _free_programmes(xmltv);
    _free_channels(xmltv);
}

void xmltv_add_channel(xmltv_t *xmltv, const xmltv_channel_t *channel)
{
    list_push(xmltv->channels, (void *)channel);
}

void xmltv_add_programme(xmltv_t *xmltv, const xmltv_programme_t *programme)
{
    list_push(xmltv->programmes, (void *)programme);
}

static void _actors_to_xml(xmlNodePtr node, const list_t *actors)
{
    bstring s;

    list_foreach(actors, first, next, cur) {
        s = (bstring)cur->value;
        xmlNewChild(node, NULL, BAD_CAST "actor", BAD_CAST s->data);
    }
}

static void _programmes_to_xml(xmlNodePtr root, const list_t *programmes)
{
    xmlNodePtr node, subnode;
    char start[XMLTV_START_FMT_SIZE];
    char date[XMLTV_DATE_FMT_SIZE];
    xmltv_programme_t *prog;

    list_foreach(programmes, first, next, cur) {
        prog = (xmltv_programme_t *)cur->value;

        debug("Adding program: %s (%s)", prog->title->data, prog->channel->data);

        node = xmlNewChild(root, NULL, BAD_CAST "programme", NULL);
        xmlNewProp(node, BAD_CAST "channel", BAD_CAST prog->channel->data);
        strftime(start, XMLTV_START_FMT_SIZE, XMLTV_START_FMT, &prog->start);
        xmlNewProp(node, BAD_CAST "start", BAD_CAST start);

        subnode = xmlNewChild(node, NULL, BAD_CAST "title", BAD_CAST prog->title->data);
        xmlNewProp(subnode, BAD_CAST "lang", BAD_CAST "es");

        subnode = xmlNewChild(node, NULL, BAD_CAST "desc", BAD_CAST prog->desc->data);
        xmlNewProp(subnode, BAD_CAST "lang", BAD_CAST "es");

        subnode = xmlNewChild(node, NULL, BAD_CAST "credits", NULL);
        xmlNewChild(subnode, NULL, BAD_CAST "director", prog->director->data);
        _actors_to_xml(subnode, prog->actors);

        strftime(date, XMLTV_START_FMT_SIZE, XMLTV_START_FMT, &prog->date);
        subnode = xmlNewChild(node, NULL, BAD_CAST "date", BAD_CAST date);

        subnode = xmlNewChild(node, NULL, BAD_CAST "country", BAD_CAST prog->country->data);

        subnode = xmlNewChild(node, NULL, BAD_CAST "episode-num", BAD_CAST prog->episode_num->data);
        xmlNewProp(subnode, BAD_CAST "system", BAD_CAST "xmltv_ns");

        subnode = xmlNewChild(node, NULL, BAD_CAST "video", NULL);
        xmlNewChild(subnode, NULL, BAD_CAST "aspect", prog->aspect->data);

        subnode = xmlNewChild(node, NULL, BAD_CAST "rating", NULL);
        xmlNewProp(subnode, BAD_CAST "system", BAD_CAST "MPAA");
        xmlNewChild(subnode, NULL, BAD_CAST "value", prog->rating_value->data);
        subnode = xmlNewChild(subnode, NULL, BAD_CAST "icon", NULL);
        xmlNewProp(subnode, BAD_CAST "src", BAD_CAST prog->rating_icon->data);

        subnode = xmlNewChild(node, NULL, BAD_CAST "start-rating", NULL);
        xmlNewChild(subnode, NULL, BAD_CAST "value", prog->star_rating->data);
    }
}

static void _channels_to_xml(xmlNodePtr root, const list_t *channels)
{
    xmlNodePtr node;
    xmltv_channel_t *chan;
    list_foreach(channels, first, next, cur) {
        chan = (xmltv_channel_t *)cur->value;

        node = xmlNewChild(root, NULL, BAD_CAST "channel", NULL);
        xmlNewProp(node, BAD_CAST "id", BAD_CAST chan->id->data);

        xmlNewChild(node, NULL, BAD_CAST "display-name", chan->display_name->data);
        xmlNewProp(node, BAD_CAST "lang", BAD_CAST "es");


    }
}



int xmltv_validate(const char *xml)
{
    int res;

    xmlValidCtxtPtr ctxt = xmlNewValidCtxt();
    xmlDocPtr doc = xmlParseMemory(xml, strlen(xml));
    xmlDtdPtr dtd = xmlParseDTD(NULL, BAD_CAST xmltv_dtd);

    res = xmlValidateDtd(ctxt, doc, dtd);
    return res;
}
