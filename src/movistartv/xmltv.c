#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include "core/dbg.h"
#include "movistartv/xmltv.h"


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

    list_foreach(prog->actors, first, next, cur) bdestroy((bstring)cur);
    list_destroy(prog->actors);
}

/** Channels **/
xmltv_channel_t *xmltv_channel_alloc()
{
    xmltv_channel_t *chan = (xmltv_channel_t *)malloc(sizeof(xmltv_channel_t));
    check_mem(chan);

    chan->id = bfromcstr("");
    chan->display_name = bfromcstr("");

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
    list_foreach(xmltv->programmes, first, next, cur) xmltv_programme_free((xmltv_programme_t *)cur);
    list_destroy(xmltv->programmes);
}

static void _free_channels(xmltv_t *xmltv)
{
    list_foreach(xmltv->channels, first, next, cur) xmltv_channel_free((xmltv_channel_t *)cur);
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


static void _programme_to_xml(const xmltv_programme_t *prog, xmlNodePtr node)
{

}

static void _channel_to_xml(const xmltv_channel_t *chan, xmlNodePtr node)
{

}


bstring xmltv_to_xml(const xmltv_t *xmltv)
{
    bstring xml = bfromcstr(""); check_mem(xml);
    xmlDocPtr doc = xmlNewDoc((const xmlChar *)"1.0"); check_mem(doc);
    xmlNodePtr root = xmlNewDocNode(doc, NULL, (const xmlChar *)"tv", NULL); check_mem(root);
    xmlNodePtr cur;





    xmlFreeDoc(doc); xmlFreeNode(root); xmlFreeNode(cur);

    return xml;

error:

    if (xml) bdestroy(xml);
    if (doc) xmlFreeDoc(doc);
    if (root) xmlFreeNode(root);
    if (cur) xmlFreeNode(cur);

    return NULL;
}

char *xmltv_validate_dtd(const char *xml)
{
    xmlDtdPtr dtd;
    xmlDocPtr doc;

    return NULL;

}

