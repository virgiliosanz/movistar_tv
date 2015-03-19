#include <mtv/all.h>

#define DOMAIN_LEN (20)
struct demarcation_context_s {
	bool     in_demarcation;
	char     domain[DOMAIN_LEN];
	mcast_s *mcast;

};
typedef struct demarcation_context_s demarcation_context_s;

static xmlEntityPtr
demarcation_get_entity(demarcation_context_s *ctx, const xmlChar *name)
{
	return xmlGetPredefinedEntity(name);
}

static void
demarcation_start_element(demarcation_context_s *ctx,
		          const xmlChar         *xn,
		          const xmlChar        **attrs)
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
demarcation_error(demarcation_context_s *ctx, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}

static void
demarcation_warning(demarcation_context_s *ctx, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
}

mcast_s *
demarcation_get_mcast_group_from_xml(const char *xml, const int demarcation)
{
	demarcation_context_s *ctx = NULL;

	error_if(NULL == xml, error, "Param Error");

	ctx = malloc(sizeof(demarcation_context_s));
	error_if(NULL == ctx, error, "Error Allocating Memory");

	ctx->mcast = mcast_alloc();
	error_if(NULL == ctx->mcast, error, "Error Allocating Memory");

	snprintf(ctx->domain, DOMAIN_LEN, "DEM_%d.imagenio.es", demarcation);
	ctx->in_demarcation = false;

	LIBXML_TEST_VERSION;

	xmlSAXHandler handler = {
		0,  /* internalSubset */
		0,  /* isStandalone */
		0,  /* hasInternalSubset */
		0,  /* hasExternalSubset */
		0,  /* resolveEntity */
		(getEntitySAXFunc)    demarcation_get_entity, /* getEntity */
		0,  /* entityDecl */
		0,  /* notationDecl */
		0,  /* attributeDecl */
		0,  /* elementDecl */
		0,  /* unparsedEntityDecl */
		0,  /* setDocumentLocator */
		0,  /* startDocument */
		0,  /* endDocument */
		(startElementSAXFunc) demarcation_start_element, /* startElement */
		0,  /* endElement */
		0,  /* reference */
		0,  /* characters */
		0,  /* ignorableWhitespace */
		0,  /* processingInstruction */
		0,  /* comment */
		(warningSAXFunc)      demarcation_warning,       /* warning */
		(errorSAXFunc)        demarcation_error,         /* error */
		(fatalErrorSAXFunc)   demarcation_error,         /* fatalError */
	};

	int result = xmlSAXUserParseMemory(&handler, ctx, (char *)xml, strlen(xml));
	error_if(0 != result, error, "Error Parsing xml: \n%s\n", xml);

	mcast_s *mcast = ctx->mcast;
	free(ctx);

//	xmlCleanupParser();
//	xmlMemoryDump();


	return mcast;

error:

	if (ctx->mcast) mcast_free(ctx->mcast);
	if (ctx) free(ctx);
	xmlCleanupParser();
	xmlMemoryDump();

	return NULL;
}

