#include <mtv/all.h>

static void _channels_to_xml(xmlNodePtr root, list_s * channels);
static void _actors_to_xml(xmlNodePtr node, list_s * actors);
static void _programmes_to_xml(xmlNodePtr root, list_s * programmes);

const char *xmltv_dtd_file = "xmltv.dtd";

char *
xmltv_to_xml(xmltv_s *xmltv)
{
	check(xmltv != NULL, "xmltv is NULL");

	xmlDocPtr doc = NULL;
	xmlDtdPtr dtd = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr cur = NULL;

	LIBXML_TEST_VERSION;

	doc = xmlNewDoc((const xmlChar *)"1.0"); check_mem(doc);
	root = xmlNewDocNode(doc, NULL, BAD_CAST "tv", NULL); check_mem(root);
	dtd = xmlCreateIntSubset(doc, BAD_CAST "tv", NULL, BAD_CAST "xmltv.dtd"); check_mem(dtd);
	xmlDocSetRootElement(doc, root);
	xmlNewProp(root,
		   BAD_CAST "generator-info-name",
		   BAD_CAST "tvz_epg - https://github.com/virgiliosanz/movistar_sv");


	debug("Addind programmes");
	_programmes_to_xml(root, xmltv->programmes);
	debug("Addind channels");
	_channels_to_xml(root, xmltv->channels);

	debug("calling xmlDocDumpMemory chan: %d prog: %d",
		xmltv->channels->count, xmltv->channels->count);

	xmlChar *s;
	int size;
	xmlDocDumpMemoryEnc(doc, &s, &size, "UTF-8");
	debug("Error generando XML");

	xmlCleanupParser();

	return s;

 error:
	xmlCleanupParser();
	if (s) free(s);

	return NULL;
}

char *
xmltv_channel_to_m3u(xmltv_channel_s *chan)
{
	check(chan != NULL, "Chan is null");
	debug("Adding m3u: %d - %s", chan->order, chan->display_name->data);

	bstring b = bformat("#EXTM3U\n#EXINF:-1,%d - %s\n#EXTTV:%s;es;%s;%s\nrtp://@%s:%d\n",
			    chan->order, chan->display_name->data,
			    chan->tags->data, chan->id->data, chan->icon->data,
			    chan->ip->data, chan->port);

	return bdata(b);

 error:
	return NULL;

}

char *
xmltv_channel_list_to_m3u(list_s *l)
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

	return bdata(b);

 error:
	if (b)
		bdestroy(b);
	if (chan)
		bdestroy(chan);
	return NULL;

}

char *
xmltv_channel_to_m3usimple(xmltv_channel_s *chan)
{
	check(chan != NULL, "Chan is null");
	debug("Adding m3usimple: %d - %s", chan->order, chan->display_name->data);

	bstring b = bformat("#EXTM3U\n#EXTINF:-1 tvg-id=%s tvg-logo=%s, %s\nrtp://@%s:%d\n",
			    chan->short_name->data, chan->icon->data, chan->display_name->data,
			    chan->ip->data, chan->port);
	return bdata(b);

 error:
	return NULL;

}

char *
xmltv_channel_list_to_m3usimple(list_s *l)
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

	return bdata(b);
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

	prog->channel = NULL;
	prog->title = NULL;
	prog->desc = NULL;
	prog->director = NULL;
	prog->country = NULL;
	prog->episode_num = NULL;
	prog->aspect = NULL;
	prog->rating_value = NULL;
	prog->rating_icon = NULL;
	prog->star_rating = NULL;

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

	list_foreach(prog->actors, first, next, cur) bdestroy((bstring) cur->value);
	list_destroy(prog->actors);
}

/** Channels **/
xmltv_channel_s *
xmltv_channel_alloc()
{
	xmltv_channel_s *chan = (xmltv_channel_s *) malloc(sizeof(xmltv_channel_s));
	check_mem(chan);

	chan->id = NULL;
	chan->display_name = NULL;
	chan->short_name = NULL;
	chan->url = NULL;
	chan->icon = NULL;
	chan->ip = NULL;
	chan->port = 0;
	chan->tags = NULL;
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

	if (chan->id)           free(chan->id);
	if (chan->display_name) free(chan->display_name);
	if (chan->short_name)   free(chan->short_name);
	if (chan->ip)           free(chan->ip);
	if (chan->tags)         free(chan->tags);
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
xmltv_add_channel(xmltv_s *xmltv, xmltv_channel_s *channel)
{
	check(xmltv != NULL, "xmltv is NULL");
	check(channel != NULL, "channel is NULL");

	list_push(xmltv->channels, (void *)channel);

 error:
	return;
}

void
xmltv_add_programme(xmltv_s *xmltv, xmltv_programme_s *programme)
{
	check(xmltv != NULL, "xmltv is NULL");
	check(programme != NULL, "programme is NULL");

	list_push(xmltv->programmes, (void *)programme);

 error:
	return;
}

static void
_actors_to_xml(xmlNodePtr node, list_s *actors)
{
	check(node != NULL, "Node is NULL");
	check(actors != NULL, "Actors is NULL");
	bstring s;
	list_foreach(actors, first, next, cur) {
		s = (char *) cur->value;
		xmlNewChild(node, NULL, BAD_CAST "actor", BAD_CAST s);
	}

 error:
	return;
}

void
_programmes_to_xml(xmlNodePtr root, list_s *programmes)
{
	check(root != NULL, "Node is NULL");
	check(NULL != programmes, "Programmes is NULL");
	check(0 < programmes->count, "Programmes list is empty");

	xmlNodePtr node, subnode;
	char start[XMLTV_START_FMT_SIZE];
	char progdate[XMLTV_DATE_FMT_SIZE];
	xmltv_programme_s *prog;
	size_t n_programmes = 0;

	list_foreach(programmes, first, next, cur) {
		prog = (xmltv_programme_s *) cur->value;

		debug_programme(prog);

		if (!prog->title) {
			continue;
		}

		if (prog->channel) {
			node = xmlNewChild(root,
					   NULL,
					   BAD_CAST "programme",
					   NULL);
			xmlNewProp(node,
				   BAD_CAST "channel",
				   BAD_CAST prog->channel);
		}

		strftime(start,
		         XMLTV_START_FMT_SIZE,
		         XMLTV_START_FMT,
		         &prog->start);
		xmlNewProp(node,
		           BAD_CAST "start",
		           BAD_CAST start);

		if (prog->title) {
			subnode = xmlNewChild(node,
			                      NULL,
			                      BAD_CAST "title",
				              BAD_CAST prog->title);
			xmlNewProp(subnode,
			           BAD_CAST "lang",
			           BAD_CAST "es");
		}

		if (prog->desc) {
			subnode = xmlNewChild(node,
			                      NULL,
			                      BAD_CAST "desc",
					      BAD_CAST prog->desc);
			xmlNewProp(subnode,
			           BAD_CAST "lang",
			           BAD_CAST "es");
		}

		subnode = xmlNewChild(node,
		                      NULL,
		                      BAD_CAST "credits",
		                      NULL);
		if (prog->director)) {
			xmlNewChild(subnode,
			            NULL,
			            BAD_CAST "director",
				    BAD_CAST prog->director);
		}
		if (prog->actors->count > 0) {
			_actors_to_xml(subnode, prog->actors);
		}

		strftime(progdate,
		         XMLTV_START_FMT_SIZE,
		         XMLTV_START_FMT,
		         &prog->date);
		subnode = xmlNewChild(node,
		                      NULL,
		                      BAD_CAST "date",
		                      BAD_CAST progdate);

		if (prog->country) {
			subnode = xmlNewChild(node,
			                      NULL,
			                      BAD_CAST "country",
				              BAD_CAST prog->country);
		}

		if (prog->episode_num) {
			subnode = xmlNewChild(node,
			                      NULL,
			                      BAD_CAST "episode-num",
					      BAD_CAST prog->episode_num);
			xmlNewProp(subnode,
			           BAD_CAST "system",
			           BAD_CAST "xmltv_ns");
		}

		if (prog->aspect) {
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
			subnode = xmlNewChild(subnode,
			                      NULL,
			                      BAD_CAST "icon",
			                      NULL);
			xmlNewProp(subnode,
			           BAD_CAST "src",
			           BAD_CAST prog->rating_icon);
		}

		if (prog->star_rating) {
			subnode = xmlNewChild(node,
			                      NULL,
			                      BAD_CAST "star-rating",
			                      NULL);
			xmlNewChild(subnode,
			            NULL,
			            BAD_CAST "value",
				    BAD_CAST prog->star_rating);
		}
		debug_programme(prog);

		n_programmes++;
	}
	debug("%u programmes added to root xml", n_programmes);

error:
	return;
}

void
_channels_to_xml(xmlNodePtr root, list_s *channels)
{
	check(root != NULL, "Node is NULL");
	check(NULL != channels, "Channels is NULL");
	check(0 < channels->count, "Channels list is empty");

	xmlNodePtr node;
	xmltv_channel_s *chan;
	list_foreach(channels, first, next, cur) {
		chan = (xmltv_channel_s *) cur->value;

		node = xmlNewChild(root, NULL, BAD_CAST "channel", NULL);
		xmlNewProp(node, BAD_CAST "id", BAD_CAST chan->id);

		node = xmlNewChild(node,
		                   NULL,
		                   BAD_CAST "display-name",
		                   chan->display_name);
		xmlNewProp(node, BAD_CAST "lang", BAD_CAST "es");
	}

 error:
	return;
}


/**
 * Validate xmltv document in bstring format against dtd
 */
int
xmltv_validate(cont char *xml)
{
	if (!xml)
		return -1;

	int rc;

	xmlValidCtxtPtr ctx = xmlNewValidCtxt();
	check_mem(ctx);

	xmlDocPtr doc = xmlParseMemory((char *)xml, strlen(xml));
	check_mem(doc);

	/* Validate against in memory dtd
	   xmlParserInputBufferPtr buf = xmlParserInputBufferCreateMem(
	   xmltv_dtd, strlen(xmltv_dtd), XML_CHAR_ENCODING_NONE);
	   xmlDtdPtr dtd = xmlIOParseDTD(NULL, buf, XML_CHAR_ENCODING_NONE);
	   xmlFreeParserInputBuffer(buf);
	 */

	xmlDtdPtr dtd = xmlParseDTD(NULL, BAD_CAST xmltv_dtd_file);

	rc = xmlValidateDtd(ctx, doc, dtd);
	debug("Resultado de validación: %d", rc);

	xmlCleanupParser();

	return rc;

 error:
	xmlCleanupParser();
	return -1;

}
