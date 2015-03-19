#include <mtv/all.h>

/**
 * Converts from string in format: YYYY-MM-DDTHH:MM:SS.000Z
 * to struct tm
 */
void
tva_parse_datetime(struct tm *dt, const char *date_time)
{
	memset((void *)dt, 0, sizeof(struct tm));
	strptime(date_time, "%FT%T%z", dt);
	return;
}


// --------------------------------
// tva_parse
typedef enum {
	TVA_STATE_START,
	TVA_STATE_SCHEDULE_EVENT,
	TVA_STATE_TITLE,
	TVA_STATE_START_TIME,
	TVA_STATE_UNKNOWN,
} tva_parser_state;

#define TVA_MAX_DEPTH (10)

struct tva_context_s {
	int              depth;
	tva_parser_state states[TVA_MAX_DEPTH];
	char            *channel_id;
	list_s          *programmes;
	epg_programme_s *programme;
	sbuf_s          *buffer;
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
	trace("%s", "Start Document");
	sbuf_reset(ctx->buffer);
	ctx->depth= 0;
	ctx->states[ctx->depth] = TVA_STATE_START;
}

static void
tva_end_document(tva_context_s *ctx)
{
	trace("%s", "End Document");
	ctx->depth= 0;
	sbuf_delete(ctx->buffer);
	ctx->states[ctx->depth] = TVA_STATE_UNKNOWN;
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

void
tva_complete_programme_info(epg_programme_s *prog, char *crid)
{
	// debug("Getting Advance info for: %s", crid);
	char       *s        = NULL;
	char      **tokens   = NULL;
	size_t      n_tokens = 0;
	const char *url_fmt  = "http://www-60.svc.imagenio.telefonica.net:2001/appserver/mvtv.do?action=getEpgInfo&extInfoID=%s&tvWholesaler=1";
	sbuf_s     *url      = sbuf_new();

	n_tokens = str_split(crid, '/', &tokens);
	sbuf_printf(url, url_fmt, tokens[n_tokens - 1]);
	str_free_tokens(tokens, n_tokens);

	char *json = http_get(sbuf_ptr(url));
	error_if(json == NULL, error, "Error getting EpgInfo");
	trace("json (%zu):\n%s", strlen(json), json);
	sbuf_delete(url);

	yajl_val node, v;
	char err_buf[1024];

	node = yajl_tree_parse(json, err_buf, sizeof(err_buf));
	error_if(NULL == node, error, "Error Parsing json");
	free(json);

	// desc
	const char *desc_path[] = {"resultData", "description", NULL};
	v = yajl_tree_get(node, desc_path, yajl_t_string);
	if (v) {
		s = YAJL_GET_STRING(v);
		prog->desc = strdup(s);
		free(s);
	}

	// actors
	const char *actors_path[] = {"resultData", "mainActors", NULL};
	v = yajl_tree_get(node, actors_path, yajl_t_string);
	if (v) {
		s = YAJL_GET_STRING(v);
		n_tokens = str_split(s, ',', &tokens);
		for (size_t i = 0; i < n_tokens; i++) {
			list_push(prog->actors, strdup(tokens[i]));
		}
		free(s);
		str_free_tokens(tokens, n_tokens);
	}

	// directors
	const char *directors_path[] = {"resultData", "directors", NULL};
	v = yajl_tree_get(node, directors_path, yajl_t_string);
	if (v) {
		s = YAJL_GET_STRING(v);
		prog->director = strdup(s);
		free(s);
	}
/*
	// date
	trace("Leyendo date");
	const char *date_path[] = { "resultData", "productionDate", (const char *)0 };
	v = yajl_tree_get(node, date_path, yajl_t_array);
	if (v) {
		array = (yajl_val)YAJL_GET_ARRAY(v);
		prog->date.tm_year = YAJL_GET_INTEGER(array->u.array.values[0]);
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
		bassigncstr(prog->country,
			    	YAJL_GET_STRING(array->u.array.values[0]));

	}
	else {
		debug("countries not found");
	}
*/
	// start
	// We could get time from here too but I'll keep the socket one

	return;

error:
	//	if (url) sbuf_delete(url);
	//	if (tokens) str_free_tokens(tokens, n_tokens);
	//	if (json) free(json);
	return;

}

static void
tva_start_element(tva_context_s *ctx, const xmlChar *xn, const xmlChar **attrs)
{
	error_if(NULL == ctx, error, "Param Error");
	error_if(NULL == xn, error, "Param Error");

	ctx->depth++;

	const char *name = (const char *)xn;
	sbuf_reset(ctx->buffer);

	if (0 == strcasecmp("ScheduleEvent", name)) {
		ctx->states[ctx->depth] = TVA_STATE_SCHEDULE_EVENT;
		ctx->programme = epg_programme_alloc();
		ctx->programme->channel = strdup(ctx->channel_id);
	}
	else if (0 ==  strcasecmp("Title", name)) {
		ctx->states[ctx->depth] = TVA_STATE_TITLE;
	}
	else if (0 ==  strcasecmp("PublishedStartTime", name)) {
		ctx->states[ctx->depth] = TVA_STATE_START_TIME;
	}
	else {
		ctx->states[ctx->depth] = TVA_STATE_UNKNOWN;
	}

error:
	return;

}

static void
tva_characters(tva_context_s *ctx, const xmlChar *ch, int len)
{
	sbuf_appendbytes(ctx->buffer, (char *)ch, len);
}

static void
tva_end_element(tva_context_s *ctx, const xmlChar *name)
{
	// TODO: Cambiar state a un array en relación al depth
	switch (ctx->states[ctx->depth]) {
		case TVA_STATE_TITLE:
			ctx->programme->title = sbuf_detach(ctx->buffer);
			break;

		case TVA_STATE_START_TIME:
			tva_parse_datetime(
				&ctx->programme->start, sbuf_ptr(ctx->buffer));
			tva_parse_datetime(
				&ctx->programme->date, sbuf_ptr(ctx->buffer));
			break;
		case TVA_STATE_SCHEDULE_EVENT:
		        if (ctx->programme->title) {
				list_push(ctx->programmes, ctx->programme);
				epg_debug_programme(ctx->programme);
			}
			break;
		default:
//			trace("Don't know what to do with: %s (%s)", name, sbuf_ptr(ctx->buffer));
			break;

	}

	ctx->depth--;
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

static void
tva_ctx_free(tva_context_s * ctx)
{
	if (ctx->programmes) epg_programme_list_free(ctx->programmes);
	if (ctx->buffer)     sbuf_delete(ctx->buffer);
	if (ctx->channel_id) free(ctx->channel_id);
	if (ctx->programme)  epg_programme_free(ctx->programme);
	if (ctx)             free(ctx);
}


static tva_context_s *
tva_ctx_alloc()
{
	tva_context_s *ctx;
	ctx = (tva_context_s *) malloc(sizeof(tva_context_s));
	error_if(ctx == NULL, error, "Error Allocating Memory");

	ctx->programmes = list_create();
	error_if(ctx->programmes == NULL, error, "Error Creating List");

	ctx->programme = NULL;
	ctx->depth = 0;
	ctx->states[ctx->depth] = TVA_STATE_UNKNOWN;
	ctx->buffer = sbuf_new();

	return ctx;

error:
	if (ctx)             tva_ctx_free(ctx);
	return NULL;
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
tva_parse(const char *xml, const char *channel_id)
{
	tva_context_s *ctx = tva_ctx_alloc();
	error_if(ctx == NULL, error, "Error creating context");
	error_if(NULL == xml, error, "Param Error");
	error_if(NULL == channel_id, error, "Param Error");

	ctx->channel_id = strdup(channel_id);

	LIBXML_TEST_VERSION;

	xmlSAXHandler handler = {
		0,  /* internalSubset */
		0,  /* isStandalone */
		0,  /* hasInternalSubset */
		0,  /* hasExternalSubset */
		0,  /* resolveEntity */
		(getEntitySAXFunc)     tva_get_entity,  /* getEntity */
		0,  /* entityDecl */
		0,  /* notationDecl */
		0,  /* attributeDecl */
		0,  /* elementDecl */
		0,  /* unparsedEntityDecl */
		0,  /* setDocumentLocator */
		(startDocumentSAXFunc) tva_start_document, /* startDocument */
		(endDocumentSAXFunc)   tva_end_document,   /* endDocument */
		(startElementSAXFunc)  tva_start_element,  /* startElement */
		(endElementSAXFunc)    tva_end_element,	   /* endElement */
		0,  /* reference */
		(charactersSAXFunc)    tva_characters,     /* characters */
		0,  /* ignorableWhitespace */
		0,  /* processingInstruction */
		0,  /* comment */
		(warningSAXFunc)       tva_warning,        /* warning */
		(errorSAXFunc)         tva_error,          /* error */
		(fatalErrorSAXFunc)    tva_error,          /* fatalError */
	};

	int result = xmlSAXUserParseMemory(
		&handler, ctx, (char *)xml, strlen(xml));
	error_if(0 != result, error, "Error creating SAX Parser");

	xmlCleanupParser();
	xmlMemoryDump();

	list_s *programmes = ctx->programmes;
	free(ctx);
	return programmes;

error:
	if (ctx) tva_ctx_free(ctx);
	xmlCleanupParser();
	xmlMemoryDump();

	return NULL;
}

