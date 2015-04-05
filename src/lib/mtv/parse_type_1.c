#include <mtv/all.h>

struct type_1_parser_context_s {
	bool          in_demarcation;
	char         *domain;
	struct mcast *mcast;

};
typedef struct type_1_parser_context_s type_1_parser_context_s;

#define _TYPE_1_PARSER_DOMAIN_FMT "DEM_%d.imagenio.es"

static xmlEntityPtr
_get_entity(type_1_parser_context_s *ctx, const xmlChar *name)
{
	warn_if(!ctx, "ctx is NULL");
	return xmlGetPredefinedEntity(name);
}

static void
_start_element(type_1_parser_context_s *ctx,
	       const xmlChar           *xn,
	       const xmlChar          **attrs)
{
	error_if(NULL == ctx, error, "Param Error");
	error_if(NULL == xn, error, "Param Error");

	const char *name = (const char *)xn;

	if (0 == strcasecmp("ServiceProvider", name)) {
//		trace("DomainName: %s Version: %s", attrs[1], attrs[3]);
		if (0 == strcasecmp(ctx->domain, (const char *)attrs[1])) {
			ctx->in_demarcation = true;
		}

	}
	else if (0 ==  strcasecmp("Push", name)) {
//		trace("Address: %s Port: %s", attrs[1], attrs[3]);
		if (ctx->in_demarcation) {
			mcast_open(ctx->mcast,
			           (const char *)attrs[1],
			           atoi((const char *)attrs[3]));
			ctx->in_demarcation = false;
		}
	}

error:
	return;

}

static void
_error(type_1_parser_context_s *ctx, const char *msg, ...)
{
	warn_if(!ctx, "ctx is NULL");

	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}

static void
_warning(type_1_parser_context_s *ctx, const char *msg, ...)
{
	warn_if(!ctx, "ctx is NULL");

	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}

/*
<ServiceDiscovery xmlns="urn:dvb:ipisdns:2006" xmlns:tva="urn:tva:metadata:2005" xmlns:mpeg7="urn:tva:mpeg7:2005" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="urn:dvb:ipisdns:2006 C:\DVB_IPI\sdns.xsd">
<ServiceProviderDiscovery Version="192">
<ServiceProvider DomainName="DEM_1.imagenio.es" Version="229">
<Offering>
	<Push Address="239.0.2.140" Port="3937"></Push>
</Offering>
</ServiceProvider>
<ServiceProvider DomainName="DEM_4.imagenio.es" Version="227">
<Offering>
	<Push Address="239.0.2.142" Port="3937"></Push>
</Offering>
</ServiceProvider>
..
...
*/
struct mcast *
mtv_parse_file_type_1(const char *xml, const int demarcation)
{
	type_1_parser_context_s *ctx = NULL;

	error_if(NULL == xml, error, "Param Error");

	ctx = malloc(sizeof(type_1_parser_context_s));
	error_if(NULL == ctx, error, "Error Allocating Memory");

	ctx->mcast = mcast_alloc();
	error_if(NULL == ctx->mcast, error, "Error Allocating Memory");

	asprintf(&ctx->domain, _TYPE_1_PARSER_DOMAIN_FMT, demarcation);
	ctx->in_demarcation = false;

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
		0,  /* endElement */
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

	struct mcast *mcast = ctx->mcast;

	free(ctx->domain);
	free(ctx);

//	xmlCleanupParser();
//	xmlMemoryDump();


	return mcast;

error:

	if (ctx->mcast)  mcast_free(ctx->mcast);
	if (ctx->domain) free(ctx->domain);
	if (ctx)         free(ctx);

	xmlCleanupParser();
	xmlMemoryDump();

	return NULL;
}

