#include <mtv/all.h>

#define PARSER_TYPE_2_MAX_DEPTH (20)
struct type_2_parser_context {
	list_s                  *channels;
	struct epg_channel      *channel;
	int                      depth;
	sbuf_s                  *buffer;
};

///////// Static functions /////////
static void
_ctx_free(struct type_2_parser_context *ctx)
{
	if (!ctx) return;
	if (ctx->channels) list_free(ctx->channels);
	if (ctx->channel) epg_channel_free(ctx->channel);
	if (ctx->buffer) sbuf_free(ctx->buffer);

	free(ctx);
}

static struct type_2_parser_context *
_ctx_alloc() {
	struct type_2_parser_context *ctx;
	ctx = malloc(sizeof(struct type_2_parser_context));
	error_if(NULL == ctx, error, "Error Allocating Memory");

	ctx->channels = list_create();
	error_if(NULL == ctx->channels, error, "Error Allocating Memory")

	ctx->buffer = sbuf_new();
	error_if(NULL == ctx->buffer, error, "Error Allocating Memory")

	ctx->depth = 0;

	return ctx;

error:
	if (ctx) _ctx_free(ctx);
	return NULL;
}

static xmlEntityPtr
_get_entity(struct type_2_parser_context *ctx, const xmlChar *name)
{
	warn_if(!ctx, "ctx is NULL");
	return xmlGetPredefinedEntity(name);
}

static void
_start_document(struct type_2_parser_context *ctx)
{
	warn_if(!ctx, "ctx is NULL");
	trace("%s", "Start Document");
	sbuf_reset(ctx->buffer);
	ctx->depth= 0;
}

static void
_end_document(struct type_2_parser_context *ctx)
{
	warn_if(!ctx, "ctx is NULL");
	trace("%s", "End Document");
	ctx->depth= 0;
}

static void
_start_element(struct type_2_parser_context *ctx,
	       const xmlChar           *xn,
	       const xmlChar          **_attrs)
{
	error_if(NULL == ctx, error, "Param Error");
	error_if(NULL == xn, error, "Param Error");

	const char **attrs = (const char **) _attrs;
	const char *name = (const char *)xn;

//	trace("%s", name);
	if (0 == strcasecmp("SingleService", name)) {
		ctx->channel = epg_channel_alloc();
		error_if(ctx->channel == NULL, error, "Memmory error %s", strerror(errno));

	} else if (0 == strcasecmp("IPMulticastAddress", name)) {
//		trace("<IPMulticastAddress %s=%s %s=%s/>", attrs[0], attrs[1], attrs[2], attrs[3]);

		ctx->channel->port = atoi(attrs[1]);
		ctx->channel->ip   = strdup(attrs[3]);

	} else if (0 ==  strcasecmp("TextualIdentifier", name)) {
//		trace("<textualIdentifier %s=%s %s=%s/>", attrs[0], attrs[1], attrs[2], attrs[3]);

		if (attrs[1] != NULL) ctx->channel->id   = strdup(attrs[1]);
		if (attrs[3] != NULL) ctx->channel->icon = strdup(attrs[3]);
	}

error:
	ctx->depth ++;
	return;

}

static void
_characters(struct type_2_parser_context *ctx, const xmlChar *ch, int len)
{
	sbuf_appendbytes(ctx->buffer, (const char *)ch, len);
}

static void
_end_element(struct type_2_parser_context *ctx, const xmlChar *_name)
{
	char *name = (char *)_name;

//	trace("%s", name);
	warn_if(!name, "Name is NULL");
	if (0 == strcasecmp(name, "SingleService")) {
		list_push(ctx->channels, ctx->channel);
		ctx->channel = epg_channel_alloc();
	} else if (0 == strcasecmp(name, "Name")) {
		ctx->channel->display_name = trim(sbuf_detach(ctx->buffer));
	} else if (0 == strcasecmp(name, "ShortName")) {
		ctx->channel->short_name = trim(sbuf_detach(ctx->buffer));
	}

	sbuf_reset(ctx->buffer);
	ctx->depth--;
}

static void
_error(struct type_2_parser_context *ctx, const char *msg, ...)
{
	warn_if(!ctx, "Name is NULL");

	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}

static void
_warning(struct type_2_parser_context *ctx, const char *msg, ...)
{
	warn_if(!ctx, "Name is NULL");

	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}

///////// API /////////
list_s *
mtv_parse_file_type_2(const char *xml)
{
	struct type_2_parser_context *ctx = NULL;

	error_if(NULL == xml, error, "Param Error");

	ctx = _ctx_alloc();
	error_if(NULL == ctx, error, "Error Allocating Context");


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
		0,  /* fatalError */
		0,  /* getParameterEntity */
		0,  /* cdataBlock */
		0,  /* externalSubset */
		0,  /* initialized */
		0,  /* private */
		0,  /* startElementNs */
		0,  /* endElementNs */
	};

	int result = xmlSAXUserParseMemory(&handler, ctx, (char *)xml, strlen(xml));
	error_if(0 != result, error, "Error Parsing xml: \n%s\n", xml);
	error_if(ctx->channels == NULL, error, "list of channels is NULL!!");
	error_if(list_count(ctx->channels) == 0, error, "Empty list of channels");

	list_s *channels = ctx->channels;

	ctx->channels = NULL;
	_ctx_free(ctx);

//	xmlCleanupParser();
//	xmlMemoryDump();

	return channels;

error:
	if (ctx) _ctx_free(ctx);
	xmlCleanupParser();
	xmlMemoryDump();

	return NULL;
}

