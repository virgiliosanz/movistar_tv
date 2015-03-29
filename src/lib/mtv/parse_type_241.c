#include <mtv/all.h>


// --------------------------------
// tva_parse
typedef enum {
	TYPE_241_PARSER_STATE_START,
	TYPE_241_PARSER_STATE_SCHEDULE_EVENT,
	TYPE_241_PARSER_STATE_TITLE,
	TYPE_241_PARSER_STATE_START_TIME,
	TYPE_241_PARSER_STATE_UNKNOWN,
} type_241_parser_state_e;

#define PARSER_TYPE_241_MAX_DEPTH (10)

struct type_241_parser_context_s {
	int                     depth;
	type_241_parser_state_e states[PARSER_TYPE_241_MAX_DEPTH];
	char                   *channel_id;
	list_s                 *programmes;
	struct epg_programme        *programme;
	sbuf_s                 *buffer;
};
typedef struct type_241_parser_context_s type_241_parser_context_s;

static xmlEntityPtr
_get_entity(type_241_parser_context_s *ctx, const xmlChar *name)
{
	warn_if(!ctx, "ctx is NULL");
	return xmlGetPredefinedEntity(name);
}

static void
_start_document(type_241_parser_context_s *ctx)
{
	warn_if(!ctx, "ctx is NULL");
	trace("%s", "Start Document");
	sbuf_reset(ctx->buffer);
	ctx->depth= 0;
	ctx->states[ctx->depth] = TYPE_241_PARSER_STATE_START;
}

static void
_end_document(type_241_parser_context_s *ctx)
{
	warn_if(!ctx, "ctx is NULL");
	trace("%s", "End Document");
	ctx->depth= 0;
	sbuf_delete(ctx->buffer);
	ctx->states[ctx->depth] = TYPE_241_PARSER_STATE_UNKNOWN;
}

static void
_start_element(type_241_parser_context_s *ctx, const xmlChar *xn, const xmlChar **attrs)
{
	error_if(NULL == ctx, error, "Param Error");
	error_if(NULL == xn, error, "Param Error");

	const xmlChar **_attrs; _attrs = attrs; // Avoid not using warning

	ctx->depth++;

	const char *name = (const char *)xn;
	sbuf_reset(ctx->buffer);

	if (0 == strcasecmp("ScheduleEvent", name)) {
		ctx->states[ctx->depth] = TYPE_241_PARSER_STATE_SCHEDULE_EVENT;
		ctx->programme = epg_programme_alloc();
		ctx->programme->channel = strdup(ctx->channel_id);
	}
	else if (0 ==  strcasecmp("Title", name)) {
		ctx->states[ctx->depth] = TYPE_241_PARSER_STATE_TITLE;
	}
	else if (0 ==  strcasecmp("PublishedStartTime", name)) {
		ctx->states[ctx->depth] = TYPE_241_PARSER_STATE_START_TIME;
	}
	else {
		ctx->states[ctx->depth] = TYPE_241_PARSER_STATE_UNKNOWN;
	}

error:
	return;

}

static void
_characters(type_241_parser_context_s *ctx, const xmlChar *ch, int len)
{
	sbuf_appendbytes(ctx->buffer, (char *)ch, len);
}

static void
_end_element(type_241_parser_context_s *ctx, const xmlChar *name)
{
	warn_if(!name, "Name is NULL");

	// TODO: Cambiar state a un array en relación al depth
	switch (ctx->states[ctx->depth]) {
		case TYPE_241_PARSER_STATE_TITLE:
			ctx->programme->title = sbuf_detach(ctx->buffer);
			break;

		case TYPE_241_PARSER_STATE_START_TIME:
			mtv_parse_datetime(
				&ctx->programme->start, sbuf_ptr(ctx->buffer));
			mtv_parse_datetime(
				&ctx->programme->date, sbuf_ptr(ctx->buffer));
			break;
		case TYPE_241_PARSER_STATE_SCHEDULE_EVENT:
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
_error(type_241_parser_context_s *ctx, const char *msg, ...)
{
	warn_if(!ctx, "ctx is NULL");

	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}

static void
_warning(type_241_parser_context_s *ctx, const char *msg, ...)
{
	warn_if(!ctx, "ctx is NULL");

	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}

static void
_ctx_free(type_241_parser_context_s * ctx)
{
	if (ctx->programmes) epg_programme_list_free(ctx->programmes);
	if (ctx->buffer)     sbuf_delete(ctx->buffer);
	if (ctx->channel_id) free(ctx->channel_id);
	if (ctx->programme)  epg_programme_free(ctx->programme);
	if (ctx)             free(ctx);
}


static type_241_parser_context_s *
_ctx_alloc()
{
	type_241_parser_context_s *ctx;
	ctx = (type_241_parser_context_s *) malloc(sizeof(type_241_parser_context_s));
	error_if(ctx == NULL, error, "Error Allocating Memory");

	ctx->programmes = list_create();
	error_if(ctx->programmes == NULL, error, "Error Creating List");

	ctx->programme = NULL;
	ctx->depth = 0;
	ctx->states[ctx->depth] = TYPE_241_PARSER_STATE_UNKNOWN;
	ctx->buffer = sbuf_new();

	return ctx;

error:
	if (ctx)             _ctx_free(ctx);
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
mtv_parse_file_type_241(const char *xml, const char *channel_id)
{
	type_241_parser_context_s *ctx = _ctx_alloc();
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
		(getEntitySAXFunc)     _get_entity,  /* getEntity */
		0,  /* entityDecl */
		0,  /* notationDecl */
		0,  /* attributeDecl */
		0,  /* elementDecl */
		0,  /* unparsedEntityDecl */
		0,  /* setDocumentLocator */
		(startDocumentSAXFunc) _start_document, /* startDocument */
		(endDocumentSAXFunc)   _end_document,   /* endDocument */
		(startElementSAXFunc)  _start_element,  /* startElement */
		(endElementSAXFunc)    _end_element,	   /* endElement */
		0,  /* reference */
		(charactersSAXFunc)    _characters,     /* characters */
		0,  /* ignorableWhitespace */
		0,  /* processingInstruction */
		0,  /* comment */
		(warningSAXFunc)       _warning,        /* warning */
		(errorSAXFunc)         _error,          /* error */
		(fatalErrorSAXFunc)    _error,          /* fatalError */
		0,  /* fatalError //: unused error() get all the errors */
		0,  /* getParameterEntity */
		0,  /* cdataBlock */
		0,  /* externalSubset */
		0,  /* initialized */
		0,  /* private */
		0,  /* startElementNs */
		0,  /* endElementNs */
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
	if (ctx) _ctx_free(ctx);
	xmlCleanupParser();
	xmlMemoryDump();

	return NULL;
}

