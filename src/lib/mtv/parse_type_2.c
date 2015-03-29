#include <mtv/all.h>

///////// Data /////////
typedef enum {
	TYPE_2_PARSER_STATE_START,
	TYPE_2_PARSER_STATE_NAME,
	TYPE_2_PARSER_STATE_SHORT_NAME,

	TYPE_2_PARSER_STATE_UNKNOWN,
} type_2_parser_state_e;

#define PARSER_TYPE_2_MAX_DEPTH (10)
struct type_2_parser_context_s {
	list_s               *channels;
	struct epg_channel        *channel;
	int                   depth;
	type_2_parser_state_e states[PARSER_TYPE_2_MAX_DEPTH];
	sbuf_s               *buffer;
};
typedef struct type_2_parser_context_s type_2_parser_context_s;

///////// Static functions /////////
static void
_ctx_free(type_2_parser_context_s *ctx)
{
	if (!ctx) return;
	if (ctx->channels) list_free(ctx->channels);
	if (ctx->channel) epg_channel_free(ctx->channel);
	if (ctx->buffer) sbuf_free(ctx->buffer);

	free(ctx);
}

static type_2_parser_context_s *
_ctx_alloc() {
	type_2_parser_context_s *ctx;
	ctx = malloc(sizeof(type_2_parser_context_s));
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
_get_entity(type_2_parser_context_s *ctx, const xmlChar *name)
{
	warn_if(!ctx, "ctx is NULL");
	return xmlGetPredefinedEntity(name);
}

static void
_start_document(type_2_parser_context_s *ctx)
{
	warn_if(!ctx, "ctx is NULL");
	trace("%s", "Start Document");
	sbuf_reset(ctx->buffer);
	ctx->depth= 0;
	ctx->states[ctx->depth] = TYPE_2_PARSER_STATE_START;
}

static void
_end_document(type_2_parser_context_s *ctx)
{
	warn_if(!ctx, "ctx is NULL");
	trace("%s", "End Document");
	ctx->depth= 0;
	ctx->states[ctx->depth] = TYPE_2_PARSER_STATE_UNKNOWN;
}

static void
_start_element(type_2_parser_context_s *ctx,
	       const xmlChar           *xn,
	       const xmlChar          **attrs)
{
	const xmlChar **_attrs; _attrs = attrs; // Avoid not using warning

	error_if(NULL == ctx, error, "Param Error");
	error_if(NULL == xn, error, "Param Error");

	const char *name = (const char *)xn;

	if (0 == strcasecmp("ServiceProvider", name)) {
	} else if (0 ==  strcasecmp("Push", name)) {

	}

error:
	return;

}

static void
_characters(type_2_parser_context_s *ctx, const xmlChar *ch, int len)
{
	sbuf_appendbytes(ctx->buffer, (char *)ch, len);
}

static void
_end_element(type_2_parser_context_s *ctx, const xmlChar *name)
{
	warn_if(!name, "Name is NULL");
	switch (ctx->states[ctx->depth]) {
		case TYPE_2_PARSER_STATE_NAME:
		break;

		default:
//			trace("Don't know what to do with: %s (%s)", name, sbuf_ptr(ctx->buffer));
		break;

	}

	ctx->depth--;
}

static void
_error(type_2_parser_context_s *ctx, const char *msg, ...)
{
	warn_if(!ctx, "Name is NULL");

	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}

static void
_warning(type_2_parser_context_s *ctx, const char *msg, ...)
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
	type_2_parser_context_s *ctx = NULL;

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
		0,  /* fatalError //: unused error() get all the errors */
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

	_ctx_free(ctx);

//	xmlCleanupParser();
//	xmlMemoryDump();


	return NULL;

error:
	if (ctx) _ctx_free(ctx);
	xmlCleanupParser();
	xmlMemoryDump();

	return NULL;
}

