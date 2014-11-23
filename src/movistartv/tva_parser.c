#define _XOPEN_SOURCE
#include <strings.h>
#include <time.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>
#include <yajl/yajl_parse.h>
#include <yajl/yajl_tree.h>

#include "all_includes.h"


/**
 * Converts from string in format: YYYY-MM-DDTHH:MM:SS.000Z
 * to struct tm
 */
void
tva_parse_datetime(struct tm *dt, const char *date_time)
{
	int microsec;
	int rc;

	memset((void *)dt, 0, sizeof(struct tm));
/*
	rc = sscanf(date_time, "%4d-%2d-%2dT%2d:%2d:%2d.%dZ",
		    &dt->tm_year, &dt->tm_mon, &dt->tm_mday,
		    &dt->tm_hour, &dt->tm_min, &dt->tm_sec,
		    &microsec);

	check(rc == EOF, "Error parsing date time: >%s<", date_time);
*/
	strptime(date_time, "%FT%T%z", dt);

//	mktime(dt);

	return;

error:
	memset((void *)dt, 0, sizeof(struct tm));
	return;

}


typedef enum {
	TVA_STATE_START,
	TVA_STATE_SCHEDULE_EVENT,
	TVA_STATE_TITLE,
	TVA_STATE_START_TIME,
	TVA_STATE_UNKNOWN,
} parser_state;

struct tva_context_s {
	int depth;
	parser_state state;
	bstring channel_id;
	list_s *programmes;
	xmltv_programme_s *programme;
	bstring chars;
};
typedef struct tva_context_s tva_context_s;

static xmlEntityPtr
tva_get_entity(tva_context_s *ctx, const xmlChar *name)
{
	return xmlGetPredefinedEntity(name);
}

static void
tva_start_document(tva_context_s *ctx)
{
	debug("Start Document");
	ctx->state = TVA_STATE_START;
	bassigncstr(ctx->chars, "");
	ctx->depth= 0;
}

static void
tva_end_document(tva_context_s *ctx)
{
	debug("End Document");
	ctx->state = TVA_STATE_UNKNOWN;
	ctx->depth= 0;
	bdestroy(ctx->chars);
}

/*

{
   "resultCode":0,
   "resultText":"Operation successful",
   "resultData": {
      "catchUp":0,
      "genre":"SERIES",
      "endtime":1408926960000,
      "begintime":1408925100000,
      "productionDate":["1969"],
      "ageRatingID":"2",
      "countries":["Suecia"],
      "version":["0"],
      "duration":1860,
      "title":"Pippi Calzaslargas: Pippi va de compras",
      "originalLongTitle":["Pippi Langstrump"],
      "description":"Pippi es un personaje entrañable. Pecosa y peinada con dos rojizas coletas, es capaz de levantar vacas o caballos, hacer volar por los aires a policías y ganar a los más temibles pillos callejeros. Su vida es maravillosa y su ruinosa casa, la envidia de cualquier niño.",
      "originalTitle":["Pippi Langstrump"],
      "longTitle":["Pippi Calzaslargas: Pippi va de compras"],
      "subgenre":"COMEDIA",
      "mainActors":["Inger Nilsson,Maria Persson,Pär Sundberg"],
      "startOver":0,
      "seriesID":"
   "},
   "hashCode":"f4b4f0e20b4926878a8d9625bcb35ed8"
}

*/

// TODO: Parsear el json y añdirlo al programme

static void
_tva_get_info(tva_context_s *ctx, const char *crid)
{
	debug("Getting Advance info for: %s", crid);

	bstring b = bfromcstr(crid);
	struct bstrList *l = bsplit(b, '/');
	bdestroy(b);

	const char *url_fmt = "http://www-60.svc.imagenio.telefonica.net:2001/appserver/mvtv.do?action=getEpgInfo&extInfoID=%s&tvWholesaler=1";
	bstring url = bformat(url_fmt, bdata(l->entry[l->qty - 1]));
	bstrListDestroy(l);

	bstring json = net_http_get(url);
	bdestroy(url);
	check_mem(json);
	debug("json:\n%s", bdata(json));


	yajl_val node, v, array;
	char err_buf[1024];

	node = yajl_tree_parse((char *)json->data, err_buf, sizeof(err_buf));
	check(NULL != node, "Error Parsing json: %s", err_buf);
	bdestroy(json);
	debug("json parseado");

	bstring s;
	// desc
	debug("Leyendo desc");
	const char *desc_path[] = { "resultData", "description", (const char *)0 };
	v = yajl_tree_get(node, desc_path, yajl_t_string);
	if (v) {
		s = bfromcstr(YAJL_GET_STRING(v));
		bassign(ctx->programme->desc, s);
		bdestroy(s);
	}
	else {
		debug("description not found");
	}

	// actors
	debug("Leyendo actors");
	const char *actors_path[] = { "resultData", "mainActors", (const char *)0 };
	v = yajl_tree_get(node, actors_path, yajl_t_string);
	if (v) {
		s = bfromcstr(YAJL_GET_STRING(v));
		debug("node: actors = %s\n", s->data);
		l = bsplit(s, ',');
		bdestroy(s);
		for (int i = 0; i < l->qty; i++) {
			list_push(ctx->programme->actors, bstrcpy(l->entry[i]));
		}
		bstrListDestroy(l);
	}
	else {
		debug("actors not found");
	}

	// directors
	debug("Leyendo directors");
	const char *directors_path[] = { "resultData", "directors", (const char *)0 };
	v = yajl_tree_get(node, directors_path, yajl_t_string);
	if (v) {
		s = bfromcstr(YAJL_GET_STRING(v));
		debug("node: directors = %s\n", bdata(s));
		bassign(ctx->programme->director, s);

	}
	else {
		debug("directors not found");
	}
/*
	// date
	debug("Leyendo date");
	const char *date_path[] = { "resultData", "productionDate", (const char *)0 };
	v = yajl_tree_get(node, date_path, yajl_t_array);
	if (v) {
		array = (yajl_val)YAJL_GET_ARRAY(v);
		ctx->programme->date.tm_year = YAJL_GET_INTEGER(array->u.array.values[0]);
	}
	else {
		debug("productionDate not found");
	}

	// country
	debug("Leyendo country");
	const char *countries_path[] = { "resultData", "countries", (const char *)0 };
	v = yajl_tree_get(node, countries_path, yajl_t_array);
	if (v) {
		array = (yajl_val)YAJL_GET_ARRAY(v);
		bassigncstr(ctx->programme->country,
			    YAJL_GET_STRING(array->u.array.values[0]));

	}
	else {
		debug("countries not found");
	}

	// start
	// We could get time from here too but I'll keep the socket one

*/
error:
	return;

}

static void
tva_start_element(tva_context_s *ctx, const xmlChar *xname, const xmlChar **attrs)
{

	ctx->depth++;
	//debug("\tStart Element (%d): %s", ctx->depth, xname);

	const char *name = (const char *)xname;
	bassigncstr(ctx->chars, "");

	if (0 ==  strcasecmp("ScheduleEvent", name)) {
		ctx->state = TVA_STATE_SCHEDULE_EVENT;
		ctx->programme = xmltv_programme_alloc();
		bassign(ctx->programme->channel, ctx->channel_id);
	}
	else if (0 ==  strcasecmp("Title", name)) {
		ctx->state = TVA_STATE_TITLE;
	}
	else if (0 ==  strcasecmp("PublishedStartTime", name)) {
		ctx->state = TVA_STATE_START_TIME;
	}
	else if (0 ==  strcasecmp("Program", name)) {
		_tva_get_info(ctx, (const char *)attrs[1]);
	}

}

static void
tva_characters(tva_context_s *ctx, const xmlChar *ch, int len)
{
	bcatblk(ctx->chars, ch, len);
}

static void
tva_end_element(tva_context_s *ctx, const xmlChar *name)
{
	//debug("\tEnd Element: %s", name);
	ctx->depth--;

	switch (ctx->state) {
		case TVA_STATE_TITLE:
			bassign(ctx->programme->title, ctx->chars);
			break;

		case TVA_STATE_START_TIME:
			tva_parse_datetime(&ctx->programme->start,
					   bdata(ctx->chars));
			tva_parse_datetime(&ctx->programme->date,
					   bdata(ctx->chars));
			break;
		default:
			if (0 ==  strcasecmp("ScheduleEvent", (const char *)name)) {
				list_push(ctx->programmes, ctx->programme);
			}

			break;

	}
}

static void
tva_error(tva_context_s *ctx, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}

static void
tva_warning(tva_context_s *ctx, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}

static tva_context_s *
tva_ctx_alloc()
{
	tva_context_s *ctx;
	ctx = (tva_context_s *) malloc(sizeof(tva_context_s));
	check_mem(ctx);

	ctx->programmes = list_create();
	check_mem(ctx->programmes);

	ctx->programme = NULL;
	ctx->depth = 0;
	ctx->state = TVA_STATE_UNKNOWN;
	ctx->chars = bfromcstr("");
	ctx->channel_id = bfromcstr("");

	return ctx;

 error:
	if (ctx->programmes)
		xmltv_programme_list_free(ctx->programmes);
	if (ctx->chars)
		bdestroy(ctx->chars);
	if (ctx)
		free(ctx);
	return NULL;
}

static void
tva_ctx_free(tva_context_s * ctx)
{
	if (ctx->programmes)
		xmltv_programme_list_free(ctx->programmes);
	if (ctx->chars)
		bdestroy(ctx->chars);
	if (ctx)
		free(ctx);
}

/**
 * Parse a TVAMain XML document & return the list of programmes
<TVAMain xmlns="urn:tva:metadata:2007" xmlns:mpeg7="urn:tva:mpeg7:2005" xml:lang="ENG">
    <ProgramDescription>
        <ProgramLocationTable>
            <Schedule serviceIDRef="1019.imagenio.es" Version="24">
                <ScheduleEvent>
                    <Program crid="crid://imagenio.es/0/38815303/38815303"/>
                    <InstanceDescription>
                        <Title>Pippi Calzaslargas: Pippi va de compras</Title>
                        <Genre href="urn:miviewtv:cs:GenreCS:2007:112.116">
                            <Name>COMEDIA</Name>
                        </Genre>
                        <ParentalGuidance>
                            <mpeg7:ParentalRating href="urn:dvb:metadata:cs:ParentalGuidanceCS:2007:2">
                                <mpeg7:Name>Suitable for all audiences</mpeg7:Name>
                            </mpeg7:ParentalRating>
                        </ParentalGuidance>
                    </InstanceDescription>
                    <PublishedStartTime>2014-08-25T00:05:00.000Z</PublishedStartTime>
                    <PublishedDuration>PT31M</PublishedDuration>
                    <ImmediateViewing value="false"/>
                    <UserActionList>
                        <mpeg7:ActionType>
                        <mpeg7:Name>FastForward</mpeg7:Name>
                        </mpeg7:ActionType>
                    </UserActionList>
                    <EventStartTime>2014-08-25T00:05:00.000Z</EventStartTime>
                    <EventDuration>PT31M</EventDuration>
                </ScheduleEvent>

                ....
                ....

            </Schedule>
        </ProgramLocationTable>
    </ProgramDescription>
</TVAMain>
*/
list_s *
tva_parse(const bstring xml, const bstring channel_id)
{
	tva_context_s *ctx = tva_ctx_alloc();
	check_mem(ctx);
	bassign(ctx->channel_id, channel_id);

	LIBXML_TEST_VERSION xmlSAXHandler handler = {
		0,		        /* internalSubset */
		0,		        /* isStandalone */
		0,		        /* hasInternalSubset */
		0,		        /* hasExternalSubset */
		0,		        /* resolveEntity */
		(getEntitySAXFunc)tva_get_entity,	        /* getEntity */
		0,		        /* entityDecl */
		0,		        /* notationDecl */
		0,		        /* attributeDecl */
		0,		        /* elementDecl */
		0,		        /* unparsedEntityDecl */
		0,		        /* setDocumentLocator */
		(startDocumentSAXFunc)tva_start_document,	/* startDocument */
		(endDocumentSAXFunc)tva_end_document,	/* endDocument */
		(startElementSAXFunc)tva_start_element,	/* startElement */
		(endElementSAXFunc)tva_end_element,	/* endElement */
		0,		        /* reference */
		(charactersSAXFunc)tva_characters,	        /* characters */
		0,		        /* ignorableWhitespace */
		0,		        /* processingInstruction */
		0,  	                /* comment */
		(warningSAXFunc)tva_warning,	        /* warning */
		(errorSAXFunc)tva_error,	        /* error */
		(fatalErrorSAXFunc)tva_error,	        /* fatalError */
	};

	int result = xmlSAXUserParseMemory(&handler,
					   ctx,
					   (char *)bdata(xml),
					   blength(xml));

	check(0 == result, "Error while parse doc - %d.", result);
	xmlCleanupParser();
	xmlMemoryDump();

	list_s *programmes = ctx->programmes;
	xmltv_programme_free(ctx->programme);
	free(ctx);
	return programmes;

 error:
	if (ctx)
		tva_ctx_free(ctx);
	xmlCleanupParser();
	xmlMemoryDump();
	return NULL;
}
