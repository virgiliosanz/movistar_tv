#include <mtv/all.h>

enum type_2_parser_state {
	TYPE_2_PARSER_STATE_START,
	TYPE_2_PARSER_STATE_VALID_PACKAGE,
	TYPE_2_PARSER_STATE_SHORT_NAME,
	TYPE_2_PARSER_STATE_UNKNOWN,
};

struct type_5_parser_context {
	bool                      in_valid_package;
	char                     *tvpackages;
	list_s                   *channels;
	struct mtv_channel_order *current_channel;
	sbuf_s                   *buffer;

};

static void
_ctx_free(struct type_5_parser_context *ctx)
{
	if (!ctx) return;

	if (ctx->channels) list_free(ctx->channels);
	if (ctx->buffer) sbuf_free(ctx->buffer);
	free(ctx);
}

static struct mtv_channel_order *
_channel_order_alloc()
{
	struct mtv_channel_order *mco;
	mco = malloc(sizeof(struct mtv_channel_order));
	error_if(NULL == mco, error, "malloc error: %s", strerror(errno));

	return mco;

error:
	if (mco) free(mco);
	return NULL;
}

static struct type_5_parser_context *
_ctx_alloc()
{
	struct type_5_parser_context *ctx;
	ctx = malloc(sizeof(struct type_5_parser_context));
	error_if(NULL == ctx, error, "malloc error: %s", strerror(errno));

	ctx->current_channel = _channel_order_alloc();
	error_if(NULL == ctx->current_channel, error, "malloc error: %s", strerror(errno));

	return ctx;

error:
	if (ctx) _ctx_free(ctx);
	return NULL;
}

static xmlEntityPtr
_get_entity(struct type_5_parser_context *ctx, const xmlChar *name)
{
	warn_if(!ctx, "ctx is NULL");
	return xmlGetPredefinedEntity(name);
}

static void
_start_element(
	struct type_5_parser_context *ctx,
	const xmlChar                *xn,
	const xmlChar               **_attrs)
{
	error_if(NULL == ctx, error, "Param Error");
	error_if(NULL == xn, error, "Param Error");

	const char *name   = (const char *)xn;
	const char **attrs = (const char **)_attrs;

	if (0 == strcasecmp("PackageName", name)) {
		if (NULL != strcasestr(ctx->tvpackages, name)) {
			ctx->in_valid_package = true;
		} else {
			ctx->in_valid_package = false;
		}

	} else if (0 ==  strcasecmp("TextualID", name)) {
		if (ctx->in_valid_package) {
			ctx->current_channel->id = strdup(attrs[1]);
		}
	}

error:
	return;

}

static void
_end_element(struct type_5_parser_context *ctx, const xmlChar *xn)
{
	error_if(NULL == ctx, error, "Param Error");
	error_if(NULL == xn, error, "Param Error");

	const char *name   = (const char *)xn;

	if (0 == strcasecmp("LogicalChannelNumber", name)) {
		ctx->current_channel->order = (unsigned) atoi(sbuf_ptr(ctx->buffer));
		list_push(ctx->channels, ctx->current_channel);
		ctx->current_channel = _channel_order_alloc();
		sbuf_reset(ctx->buffer);
	}

error:
	return;
}

static void
_error(struct type_5_parser_context *ctx, const char *msg, ...)
{
	warn_if(!ctx, "ctx is NULL");

	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}

static void
_warning(struct type_5_parser_context *ctx, const char *msg, ...)
{
	warn_if(!ctx, "ctx is NULL");

	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}

/*
<ServiceDiscovery xmlns="urn:dvb:ipisdns:2006">
  <PackageDiscovery DomainName="imagenio.es" Version="87">
    <Package Id="1">
      <PackageName Language="ENG">UTX00</PackageName>
      <Service>
        <TextualID ServiceName="597"/>
        <LogicalChannelNumber>0</LogicalChannelNumber>
      </Service>
      <Service>
        <TextualID ServiceName="1"/>
        <LogicalChannelNumber>1</LogicalChannelNumber>
      </Service>
      <Service>
....
....
*/
list_s *
mtv_parse_file_type_5(const char *xml, const char *tvpackages)
{
	struct type_5_parser_context *ctx = NULL;

	error_if(NULL == xml, error, "Param Error: empty XML!");
	error_if(NULL == tvpackages, error, "Param Error: empty tvpackages");

	ctx = _ctx_alloc();
	error_if(NULL == ctx, error, "Error Allocating Memory: %s", strerror(errno));

	ctx->in_valid_package = false;
	ctx->tvpackages = (char *)tvpackages;

	LIBXML_TEST_VERSION;

	xmlSAXHandler handler = {
		0,  /* internalSubset */
		0,  /* isStandalone */
		0,  /* hasInternalSubset */
		0,  /* hasExternalSubset */
		0,  /* resolveEntity */
		(getEntitySAXFunc)    _get_entity, /* getEntity */
		0,  /* entityDecl */
		0,  /* notationDecl */
		0,  /* attributeDecl */
		0,  /* elementDecl */
		0,  /* unparsedEntityDecl */
		0,  /* setDocumentLocator */
		0,  /* startDocument */
		0,  /* endDocument */
		(startElementSAXFunc) _start_element, /* startElement */
		(endElementSAXFunc) _end_element, /* startElement */
		0,  /* reference */
		0,  /* characters */
		0,  /* ignorableWhitespace */
		0,  /* processingInstruction */
		0,  /* comment */
		(warningSAXFunc)      _warning,       /* warning */
		(errorSAXFunc)        _error,         /* error */
		(fatalErrorSAXFunc)   _error,         /* fatalError */
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

	error_if (!ctx->channels, error, "Error reading channels order");
	error_if (list_count(ctx->channels) == 0, error, "Error reading channels order");

	list_s *channels = ctx->channels;
	ctx->channels = NULL;

	free(ctx);


//	xmlCleanupParser();
//	xmlMemoryDump();

	return channels;

error:

	if (ctx) _ctx_free(ctx);

	xmlCleanupParser();
	xmlMemoryDump();

	return NULL;
}

