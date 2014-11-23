#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/xmlstring.h>
#include <libxml2/libxml/valid.h>
#include <libxml2/libxml/xmlmemory.h>
#include <time.h>
#include "all_includes.h"

static void _channels_to_xml(xmlNodePtr root, const list_s * channels);
static void _actors_to_xml(xmlNodePtr node, const list_s * actors);
static void _programmes_to_xml(xmlNodePtr root, const list_s * programmes);

const char *xmltv_dtd_file = "xmltv.dtd";

bstring
xmltv_to_xml(const xmltv_s *xmltv)
{
	check(xmltv != NULL, "xmltv is NULL");

	xmlDocPtr doc = NULL;
	xmlDtdPtr dtd = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr cur = NULL;

	LIBXML_TEST_VERSION;

	doc = xmlNewDoc((const xmlChar *)"1.0");
	check_mem(doc);
	root = xmlNewDocNode(doc, NULL, (const xmlChar *)"tv", NULL);
	check_mem(root);
	dtd = xmlCreateIntSubset(doc, BAD_CAST "tv", NULL, BAD_CAST "xmltv.dtd");
	check_mem(dtd);
	xmlDocSetRootElement(doc, root);

	xmlNewProp(root,
		   BAD_CAST "generator-info-name",
		   BAD_CAST "tvz_epg - https://github.com/virgiliosanz/movistar_sv");

	_channels_to_xml(root, xmltv->channels);
	_programmes_to_xml(root, xmltv->programmes);

	xmlChar *s;
	int size;
	xmlDocDumpMemoryEnc(doc, &s, &size, "UTF-8");

	xmlCleanupParser();

	bstring b = bfromcstr((char *)s);
	free(s);

	return b;

 error:
	xmlCleanupParser();
	if (s)
		free(s);

	return NULL;
}

bstring
xmltv_channel_to_m3u(const xmltv_channel_s *chan)
{
	check(chan != NULL, "Chan is null");
	debug("Adding m3u: %d - %s", chan->order, chan->display_name->data);

	bstring b = bformat("#EXTM3U\n#EXINF:-1,%d - %s\n#EXTTV:%s;es;%s;%s\nrtp://@%s:%d\n",
			    chan->order, chan->display_name->data,
			    chan->tags->data, chan->id->data, chan->icon->data,
			    chan->ip->data, chan->port);

	return b;
 error:
	return NULL;

}

bstring
xmltv_channel_list_to_m3u(const list_s *l)
{
	check(l != NULL, "Channels list is NULL/Empty");

	bstring b = bfromcstr("");
	bstring chan = NULL;
	xmltv_channel_s *channel = NULL;

	list_foreach(l, first, next, cur) {
		channel = (xmltv_channel_s *) cur->value;
		debug("Adding m3u: %d - %s", channel->order, channel->display_name->data);
		chan = xmltv_channel_to_m3u(channel);
		bconcat(b, chan);
		bdestroy(chan);
	}

	return b;
 error:
	if (b)
		bdestroy(b);
	if (chan)
		bdestroy(chan);
	return NULL;

}

bstring
xmltv_channel_to_m3usimple(const xmltv_channel_s *chan)
{
	check(chan != NULL, "Chan is null");
	debug("Adding m3usimple: %d - %s", chan->order, chan->display_name->data);

	bstring b = bformat("#EXTM3U\n#EXTINF:-1 tvg-id=%s tvg-logo=%s, %s\nrtp://@%s:%d\n",
			    chan->short_name->data, chan->icon->data, chan->display_name->data,
			    chan->ip->data, chan->port);
	return b;

 error:
	return NULL;

}

bstring
xmltv_channel_list_to_m3usimple(const list_s *l)
{

	check(l != NULL, "Channels list is NULL/Empty");

	bstring b = bfromcstr("");
	bstring chan = NULL;
	xmltv_channel_s *channel = NULL;

	list_foreach(l, first, next, cur) {
		channel = (xmltv_channel_s *) cur->value;
		debug("Adding m3u: %d - %s", channel->order, channel->display_name->data);
		chan = xmltv_channel_to_m3usimple(channel);
		bconcat(b, chan);
		bdestroy(chan);
	}

	return b;
 error:
	if (b)
		bdestroy(b);
	if (chan)
		bdestroy(chan);
	return NULL;
}

/** Programmes **/
xmltv_programme_s *
xmltv_programme_alloc()
{
	xmltv_programme_s *prog = (xmltv_programme_s *) malloc(sizeof(xmltv_programme_s));
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
	if (prog)
		xmltv_programme_free(prog);
	return NULL;
}

void
xmltv_programme_free(xmltv_programme_s *prog)
{
	if (!prog)
		return;

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

	list_foreach(prog->actors, first, next, cur) bdestroy((bstring) cur->value);
	list_destroy(prog->actors);
}

/** Channels **/
xmltv_channel_s *
xmltv_channel_alloc()
{
	xmltv_channel_s *chan = (xmltv_channel_s *) malloc(sizeof(xmltv_channel_s));
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
	if (chan)
		xmltv_channel_free(chan);
	return NULL;
}

void
xmltv_channel_free(xmltv_channel_s *chan)
{
	if (!chan)
		return;

	bdestroy(chan->id);
	bdestroy(chan->display_name);
	bdestroy(chan->short_name);
	bdestroy(chan->ip);
	bdestroy(chan->tags);
	free(chan);
}

/** xmltv **/
xmltv_s *
xmltv_alloc()
{
	xmltv_s *xmltv = (xmltv_s *) malloc(sizeof(xmltv_s));
	check_mem(xmltv);

	xmltv->channels = list_create();
	check_mem(xmltv->channels);

	xmltv->programmes = list_create();
	check_mem(xmltv->programmes);

	return xmltv;

 error:
	if (xmltv)
		xmltv_free(xmltv);
	return NULL;
}

void
xmltv_programme_list_free(list_s *programmes)
{
	check(programmes != NULL, "Channels is NULL");
	list_foreach(programmes, first, next, cur) {
		xmltv_programme_free((xmltv_programme_s *) cur->value);
	}
	list_destroy(programmes);

 error:
	return;
}

void
xmltv_channel_list_free(list_s *channels)
{
	check(channels != NULL, "Channels is NULL");

	list_foreach(channels, first, next, cur) {
		xmltv_channel_free((xmltv_channel_s *) cur->value);
	}
	list_destroy(channels);

 error:
	return;

}

void
xmltv_free(xmltv_s *xmltv)
{
	if (!xmltv)
		return;

	xmltv_programme_list_free(xmltv->programmes);
	xmltv_channel_list_free(xmltv->channels);
	free(xmltv);
}

void
xmltv_add_channel(xmltv_s *xmltv, const xmltv_channel_s *channel)
{
	check(xmltv != NULL, "xmltv is NULL");
	check(channel != NULL, "channel is NULL");

	list_push(xmltv->channels, (void *)channel);

 error:
	return;
}

void
xmltv_add_programme(xmltv_s *xmltv, const xmltv_programme_s *programme)
{
	check(xmltv != NULL, "xmltv is NULL");
	check(programme != NULL, "programme is NULL");

	list_push(xmltv->programmes, (void *)programme);

 error:
	return;
}

static void
_actors_to_xml(xmlNodePtr node, const list_s *actors)
{
	check(node != NULL, "Node is NULL");
	check(actors != NULL, "Actors is NULL");
	bstring s;
	list_foreach(actors, first, next, cur) {
		s = (bstring) cur->value;
		xmlNewChild(node, NULL, BAD_CAST "actor", BAD_CAST s->data);
	}

 error:
	return;
}

void
_programmes_to_xml(xmlNodePtr root, const list_s *programmes)
{
	check(root != NULL, "Node is NULL");
	check(NULL != programmes, "Programmes is NULL");

	xmlNodePtr node, subnode;
	char start[XMLTV_START_FMT_SIZE];
	char progdate[XMLTV_DATE_FMT_SIZE];
	xmltv_programme_s *prog;
	size_t n_programmes = 0;

	list_foreach(programmes, first, next, cur) {
		prog = (xmltv_programme_s *) cur->value;

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

		strftime(progdate, XMLTV_START_FMT_SIZE, XMLTV_START_FMT, &prog->date);
		subnode = xmlNewChild(node, NULL, BAD_CAST "date", BAD_CAST progdate);

		subnode = xmlNewChild(node, NULL, BAD_CAST "country", BAD_CAST prog->country->data);

		subnode =
		    xmlNewChild(node, NULL, BAD_CAST "episode-num",
				BAD_CAST prog->episode_num->data);
		xmlNewProp(subnode, BAD_CAST "system", BAD_CAST "xmltv_ns");

		subnode = xmlNewChild(node, NULL, BAD_CAST "video", NULL);
		xmlNewChild(subnode, NULL, BAD_CAST "aspect", prog->aspect->data);

		subnode = xmlNewChild(node, NULL, BAD_CAST "rating", NULL);
		xmlNewProp(subnode, BAD_CAST "system", BAD_CAST "MPAA");
		xmlNewChild(subnode, NULL, BAD_CAST "value", prog->rating_value->data);
		subnode = xmlNewChild(subnode, NULL, BAD_CAST "icon", NULL);
		xmlNewProp(subnode, BAD_CAST "src", BAD_CAST prog->rating_icon->data);

		subnode = xmlNewChild(node, NULL, BAD_CAST "star-rating", NULL);
		xmlNewChild(subnode, NULL, BAD_CAST "value", prog->star_rating->data);

		n_programmes++;
		debug("Programme added -> %zx", n_programmes);
	}

 error:
	return;
}

void
_channels_to_xml(xmlNodePtr root, const list_s *channels)
{
	check(root != NULL, "Node is NULL");
	check(NULL != channels, "Programmes is NULL");

	xmlNodePtr node;
	xmltv_channel_s *chan;
	list_foreach(channels, first, next, cur) {
		chan = (xmltv_channel_s *) cur->value;

		node = xmlNewChild(root, NULL, BAD_CAST "channel", NULL);
		xmlNewProp(node, BAD_CAST "id", BAD_CAST chan->id->data);

		node = xmlNewChild(node, NULL, BAD_CAST "display-name", chan->display_name->data);
		xmlNewProp(node, BAD_CAST "lang", BAD_CAST "es");
	}

 error:
	return;
}


/**
 * Validate xmltv document in bstring format against dtd
 */
int
xmltv_validate(const bstring xml)
{
	if (!xml)
		return -1;

	int rc;

	xmlValidCtxtPtr ctx = xmlNewValidCtxt();
	check_mem(ctx);

	xmlDocPtr doc = xmlParseMemory((char *)xml->data, blength(xml));
	check_mem(doc);

	/* Validate against in memory dtd
	   xmlParserInputBufferPtr buf = xmlParserInputBufferCreateMem(
	   xmltv_dtd, strlen(xmltv_dtd), XML_CHAR_ENCODING_NONE);
	   xmlDtdPtr dtd = xmlIOParseDTD(NULL, buf, XML_CHAR_ENCODING_NONE);
	   xmlFreeParserInputBuffer(buf);
	 */

	xmlDtdPtr dtd = xmlParseDTD(NULL, BAD_CAST xmltv_dtd_file);	/* parse the DTD */

	rc = xmlValidateDtd(ctx, doc, dtd);
	debug("Resultado de validación: %d", rc);

	xmlCleanupParser();

	return rc;

 error:
	xmlCleanupParser();
	return -1;

}
