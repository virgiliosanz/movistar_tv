#include <mtv/all.h>
#include "xmltv_dtd.h"

static void _channels_to_xml(xmlNodePtr root, list_s * channels);
static void _programmes_to_xml(xmlNodePtr root, list_s * programmes);

char *
epg_to_xmltv(const struct epg *epg)
{
    xmlDocPtr  doc  = NULL;
    xmlDtdPtr  dtd  = NULL;
    xmlNodePtr root = NULL;
    xmlChar   *s    = NULL;

    error_if(epg == NULL, error, "Param Error");

    LIBXML_TEST_VERSION;

    doc = xmlNewDoc((const xmlChar *)"1.0");
    error_if(doc == NULL, error, "Error Creating xmlDoc");

    root = xmlNewDocNode(doc, NULL, BAD_CAST "tv", NULL);
    error_if(root == NULL, error, "Error creating root node");

    dtd = xmlCreateIntSubset(doc, BAD_CAST "tv", NULL, BAD_CAST "epg.dtd");
    error_if(dtd == NULL, error, "Error adding DTD to xmlDoc");

    xmlDocSetRootElement(doc, root);
    xmlNewProp(root,
           BAD_CAST "generator-info-name",
           BAD_CAST "tvz_epg - https://github.com/virgiliosanz/movistar_sv");

    trace("%s", "Adding programmes");
    _programmes_to_xml(root, epg->programmes);

    trace("%s", "Adding channels");
    _channels_to_xml(root, epg->channels);

    trace("calling xmlDocDumpMemory chan: %zu prog: %zu",
        list_count(epg->channels), list_count(epg->programmes));

    int size;
    xmlDocDumpMemoryEnc(doc, &s, &size, "UTF-8");
    xmlCleanupParser();

    return (char *)s;

 error:
    xmlCleanupParser();
    if (s) free(s);

    return NULL;
}


void _gen_m3u_simpletv(void *value, void *state);
void _gen_m3u_tvheadend(void *value, void *state);

char *
epg_to_m3u(const list_s *channels, enum epg_m3u_format format)
{
    sbuf_s *buffer = sbuf_new();
    error_if(buffer == NULL, error, "Error Allocating sbuf_s");

    error_if(channels == NULL, error, "Params Error");
    trace("There are %zu channels", list_count(channels));

    list_apply_with_state_cb func =
        (format == epg_m3u_format_simpletv) ?
        _gen_m3u_simpletv : _gen_m3u_tvheadend;

    list_walk_with_state((list_s *)channels, func, (void *)buffer);

    char *s = sbuf_detach(buffer);
    sbuf_delete(buffer);
    return s;

error:
    if (buffer)
        sbuf_delete(buffer);
    return NULL;
}

/** Programmes **/
struct epg_programme *
epg_programme_alloc()
{
    struct epg_programme *prog = (struct epg_programme *) malloc(sizeof(struct epg_programme));
    error_if(prog == NULL, error, "Error Allocating Memory");

    prog->channel      = NULL;
    prog->title        = NULL;
    prog->desc         = NULL;
    prog->director     = NULL;
    prog->country      = NULL;
    prog->episode_num  = NULL;
    prog->aspect       = NULL;
    prog->rating_value = NULL;
    prog->rating_icon  = NULL;
    prog->star_rating  = NULL;

    prog->actors = list_create();

    return prog;

 error:
    if (prog)
        epg_programme_free(prog);
    return NULL;
}

void
epg_programme_free(struct epg_programme *prog)
{
    if (!prog)
        return;

    if (prog->channel)      free(prog->channel);
    if (prog->title)        free(prog->title);
    if (prog->desc)         free(prog->desc);
    if (prog->director)     free(prog->director);
    if (prog->country)      free(prog->country);
    if (prog->episode_num)  free(prog->episode_num);
    if (prog->aspect)       free(prog->aspect);
    if (prog->rating_value) free(prog->rating_value);
    if (prog->rating_icon)  free(prog->rating_icon);
    if (prog->star_rating)  free(prog->star_rating);

    list_foreach(prog->actors, first, next, cur) free(cur->value);
    list_destroy(prog->actors);
}

/** Channels **/
struct epg_channel *
epg_channel_alloc()
{
    struct epg_channel *chan = (struct epg_channel *) malloc(sizeof(struct epg_channel));
    error_if(chan == NULL, error, "Error Allocating Memory");

    chan->id           = NULL;
    chan->display_name = NULL;
    chan->short_name   = NULL;
    chan->url          = NULL;
    chan->icon         = NULL;
    chan->ip           = NULL;
    chan->port         = 0;
    chan->tags         = NULL;
    chan->order        = 0;

    return chan;

 error:
    if (chan)
        epg_channel_free(chan);
    return NULL;
}

void
epg_channel_free(struct epg_channel *chan)
{
    if (!chan)
        return;

    if (chan->id)           free(chan->id);
    if (chan->display_name) free(chan->display_name);
    if (chan->short_name)   free(chan->short_name);
    if (chan->ip)           free(chan->ip);
    if (chan->tags)         free(chan->tags);
    free(chan);
}

int
epg_channel_compare_by_order(const void *l, const void *r)
{
    struct epg_channel *cl = (struct epg_channel *)l;
    struct epg_channel *cr = (struct epg_channel *)r;

    return (cl->order - cr->order);
}

/** epg **/
struct epg *
epg_alloc()
{
    struct epg *epg = (struct epg *) malloc(sizeof(struct epg));
    error_if(epg == NULL, error, "Error Allocating Memory");

    epg->channels = list_create();
    error_if(epg->channels == NULL, error, "Cannot create list");

    epg->programmes = list_create();
    error_if(epg->programmes == NULL, error, "Cannot create list");

    return epg;

 error:
    if (epg)
        epg_free(epg);
    return NULL;
}

void
epg_programme_list_free(list_s *programmes)
{
    error_if(programmes == NULL, error, "Param Error");
    list_foreach(programmes, first, next, cur) {
        epg_programme_free((struct epg_programme *) cur->value);
    }
    list_destroy(programmes);

 error:
    return;
}

void
epg_channel_list_free(list_s *channels)
{
    error_if(channels == NULL, error, "Param error");

    list_foreach(channels, first, next, cur) {
        epg_channel_free((struct epg_channel *) cur->value);
    }
    list_destroy(channels);

 error:
    return;

}

void
epg_free(struct epg *epg)
{
    if (!epg)
        return;

    epg_programme_list_free(epg->programmes);
    epg_channel_list_free(epg->channels);
    free(epg);
}

void
epg_add_channel(struct epg *epg, struct epg_channel *channel)
{
    error_if(epg == NULL, error, "Param Error");
    error_if(channel == NULL, error, "Param Error");

    list_push(epg->channels, (void *)channel);

 error:
    return;
}

void
epg_add_programme(struct epg *epg, struct epg_programme *programme)
{
    error_if(epg == NULL, error, "Param Error");
    error_if(programme == NULL, error, "Param Error");

    list_push(epg->programmes, (void *)programme);

 error:
    return;
}

static void
_actors_to_xml(xmlNodePtr root, list_s *actors)
{
    error_if(root == NULL, error, "Param Error");
    error_if(actors == NULL, error, "Param Error");
    char *s = NULL;
    list_foreach(actors, first, next, cur) {
        s = (char *) cur->value;
//		trace("Adding actor: %s", s)
        xmlNewChild(root, NULL, BAD_CAST "actor", BAD_CAST s);
    }

 error:
    return;
}

int
epg_programme_compare_by_date(const void *l, const void *r)
{
    struct epg_programme *pl = (struct epg_programme *)l;
    struct epg_programme *pr = (struct epg_programme *)r;

    return (mktime(&pl->start) - mktime(&pr->start));
}


void
_programmes_to_xml(xmlNodePtr root, list_s *programmes)
{
    error_if(NULL == root, error, "Param Error");
    error_if(NULL == programmes, error, "Param Error");
    error_if(0 >= programmes->count, error, "Programs list empty");

    xmlNodePtr node;
    xmlNodePtr subnode;
    char start[EPG_START_FMT_SIZE];
    char progdate[EPG_DATE_FMT_SIZE];
    struct epg_programme *prog;
    size_t n_programmes = 0;

    list_foreach(programmes, first, next, cur) {
        prog = (struct epg_programme *) cur->value;

//		epg_debug_programme(prog);

        if (!prog->title || !prog->channel) {
            warn("No channel (%s) or title (%s)!!",
                prog->channel, prog->title);
            continue;
        }

//		trace("Adding channel (%s): %s", prog->title, prog->channel);
        node = xmlNewChild(root, NULL, BAD_CAST "programme", NULL);
        xmlNewProp(node, BAD_CAST "channel", BAD_CAST prog->channel);

        strftime(start, EPG_START_FMT_SIZE, EPG_START_FMT, &prog->start);
//		trace("Adding start time: %s", start);
        xmlNewProp(node, BAD_CAST "start", BAD_CAST start);

//		trace("Adding title: %s", prog->title);
        subnode = xmlNewChild(node, NULL,
                          BAD_CAST "title",
                      BAD_CAST prog->title);
        xmlNewProp(subnode, BAD_CAST "lang", BAD_CAST "es");

        if (prog->desc) {
//			trace("Adding desc: '%s'", prog->desc);
            subnode = xmlNewChild(node,
                                  NULL,
                                  BAD_CAST "desc",
                          BAD_CAST prog->desc);
            xmlNewProp(subnode, BAD_CAST "lang", BAD_CAST "es");
        }


        subnode = xmlNewChild(node, NULL, BAD_CAST "credits", NULL);
        if (prog->director) {
//			trace("Adding director: %s", prog->director);
            xmlNewChild(subnode,
                        NULL,
                        BAD_CAST "director",
                    BAD_CAST prog->director);
        }
//		trace("Addind %d actors", prog->actors->count);
        if (prog->actors->count > 0) {
            _actors_to_xml(subnode, prog->actors);
        }

        strftime(progdate,
            EPG_DATE_FMT_SIZE, EPG_DATE_FMT, &prog->date);
//		trace("Adding progdate: %s", progdate);
        subnode = xmlNewChild(node,
            NULL, BAD_CAST "date", BAD_CAST progdate);

        if (prog->country) {
//			trace("Adding country: %s", prog->country);
            subnode = xmlNewChild(node,
                                  NULL,
                                  BAD_CAST "country",
                              BAD_CAST prog->country);
        }

        if (prog->episode_num) {
//			trace("Adding episode num: %s", prog->episode_num);
            subnode = xmlNewChild(node,
                                  NULL,
                                  BAD_CAST "episode-num",
                          BAD_CAST prog->episode_num);
            xmlNewProp(subnode,
                       BAD_CAST "system",
                       BAD_CAST "epg_ns");
        }

        if (prog->aspect) {
//			trace("Adding aspect: %s", prog->aspect);
            subnode = xmlNewChild(node,
                                  NULL,
                                  BAD_CAST "video",
                                  NULL);
            xmlNewChild(subnode,
                        NULL,
                        BAD_CAST "aspect",
                    BAD_CAST prog->aspect);
        }

        if (prog->rating_value) {
//			trace("Adding rating_value: %s", prog->rating_value);
            subnode = xmlNewChild(node,
                                  NULL,
                                  BAD_CAST "rating",
                                  NULL);
            xmlNewProp(subnode,
                       BAD_CAST "system",
                       BAD_CAST "MPAA");
            xmlNewChild(subnode,
                        NULL,
                        BAD_CAST "value",
                    BAD_CAST prog->rating_value);
        }

        if (prog->rating_icon) {
//			trace("Adding rating_icon: %s", prog->rating_icon);
            subnode = xmlNewChild(subnode,
                                  NULL,
                                  BAD_CAST "icon",
                                  NULL);
            xmlNewProp(subnode,
                       BAD_CAST "src",
                       BAD_CAST prog->rating_icon);
        }

        if (prog->star_rating) {
//			trace("Adding star_rating: %s", prog->star_rating);
            subnode = xmlNewChild(node,
                                  NULL,
                                  BAD_CAST "star-rating",
                                  NULL);
            xmlNewChild(subnode,
                        NULL,
                        BAD_CAST "value",
                    BAD_CAST prog->star_rating);
        }
        n_programmes++;

    }
    trace("%zu programmes added to root xml", n_programmes);

error:
    return;
}

void
_channels_to_xml(xmlNodePtr root, list_s *channels)
{
    error_if(root == NULL, error, "Param Error");
    error_if(NULL == channels, error, "Param Error");
    error_if(0 >= channels->count, error, "Channel list empty");

    xmlNodePtr node;
    struct epg_channel *chan;
    list_foreach(channels, first, next, cur) {
        chan = (struct epg_channel *) cur->value;

        node = xmlNewChild(root, NULL, BAD_CAST "channel", NULL);
        xmlNewProp(node, BAD_CAST "id", BAD_CAST chan->id);

        node = xmlNewChild(node,
                           NULL,
                           BAD_CAST "display-name",
                           BAD_CAST chan->display_name);
        xmlNewProp(node, BAD_CAST "lang", BAD_CAST "es");
    }

 error:
    return;
}


const char *epg_dtd_file = "xmltv.dtd";

/**
 * Validate epg document in bstring format against dtd
 */
int
epg_validate(const char *xml)
{
    if (!xml)
        return -1;

    int rc;

    xmlValidCtxtPtr ctx = xmlNewValidCtxt();
    error_if(ctx == NULL, error, "Error creating validation context");

    xmlDocPtr doc = xmlParseMemory((char *)xml, strlen(xml));
    error_if(doc == NULL, error, "Error creating Parser");

    /* Validate against in memory dtd */
    xmlParserInputBufferPtr buf = xmlParserInputBufferCreateMem(
        xmltv_dtd, strlen(xmltv_dtd), XML_CHAR_ENCODING_NONE);
    xmlDtdPtr dtd = xmlIOParseDTD(NULL, buf, XML_CHAR_ENCODING_NONE);
    xmlFreeParserInputBuffer(buf);
/*
    xmlDtdPtr dtd = xmlParseDTD(NULL, BAD_CAST epg_dtd_file);
*/
    rc = xmlValidateDtd(ctx, doc, dtd);
    trace("Resultado de validación: %d", rc);

    xmlCleanupParser();

    return rc;

 error:
    xmlCleanupParser();
    return -1;

}


void
_gen_m3u_simpletv(void *value, void *state)
{
    struct epg_channel *chan = (struct epg_channel *)value;
    sbuf_s        *buf  = (sbuf_s *)state;

//	trace("Adding channel: %s", chan->display_name);

    sbuf_appendf(
        buf,
        "#EXTM3U\n#EXTINF:-1 tvg-id=%s tvg-logo=%s, %s\nrtp://@%s:%d\n",
        chan->id, chan->icon, chan->display_name,
        chan->ip, chan->port);
}


void
_gen_m3u_tvheadend(void *value, void *state)
{
    struct epg_channel *chan = (struct epg_channel *)value;
    sbuf_s        *buf  = (sbuf_s *)state;

//	trace("Adding channel: %s", chan->display_name);

    sbuf_appendf(
        buf,
        "#EXTM3U\n#EXTINF:-1 tvg-id=%s tvg-logo=%s, %s\nrtp://@%s:%d\n",
        chan->id, chan->icon, chan->display_name,
        chan->ip, chan->port);
}

