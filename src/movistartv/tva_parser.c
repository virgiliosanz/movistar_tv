#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>
#include "core/dbg.h"
#include "core/xmltv.h"
#include "movistartv/tva_parser.h"

typedef enum {
    STATE_START,
    STATE_UNKNOWN,
} parser_state;

struct tva_context_s {
    int depth;
    parser_state state;
    list_t *programmes;
    xmltv_programme_t *programme;
};
typedef struct tva_context_s tva_context_t;

static xmlEntityPtr tva_get_entity(tva_context_t *ctx, const xmlChar *name)
{
    return xmlGetPredefinedEntity(name);
}

static void tva_start_document(tva_context_t *ctx)
{
    debug("Start Document");
}
static void tva_end_document(tva_context_t *ctx)
{
    debug("End Document");
}

static void tva_start_element(
        tva_context_t *ctx, const xmlChar *name, const xmlChar **attrs)
{
    ctx->depth ++;
    debug("\tStart Element (%d): %s", ctx->depth, name);

    int i_name = 0;
    int i_value = 1;
    while (attrs && attrs[i_name]) {
        debug("\t\t%s: %s -> %s", name, attrs[i_name], attrs[i_value]);
        i_name ++; i_value ++;
    }
}

static void tva_characters(tva_context_t *ctx, const xmlChar *ch, int len)
{
//    extern int bcatblk (bstring b, const void * s, int len);
//    bstring s = blk2bstr((char *)ch, len);

    bstring s = bfromcstr("");
    bassignblk(s, ch, len);

    debug("\tCharacters... %s", s->data);
}

static void tva_end_element(tva_context_t *ctx, const xmlChar *name )
{
    debug("\tEnd Element: %s", name);
    ctx->depth --;
}

static void tva_error(tva_context_t *ctx, const char *msg, ... )
{
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
}

static void tva_warning(tva_context_t *ctx, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
}

static tva_context_t *tva_ctx_alloc()
{
    tva_context_t *ctx;
    ctx = (tva_context_t *)malloc(sizeof(tva_context_t));
    check_mem(ctx);

    ctx->programmes = list_create();
    check_mem(ctx->programmes);

    ctx->programme = NULL;
    ctx->depth = 0;
    ctx->state = STATE_UNKNOWN;

    return ctx;

error:
    if (ctx->programmes) xmltv_programme_list_free(ctx->programmes);
    if (ctx) free(ctx);
    return NULL;
}

static void tva_ctx_free(tva_context_t *ctx)
{
    if (ctx->programmes) xmltv_programme_list_free(ctx->programmes);
    if (ctx) free(ctx);
}

/**
 * Parse a TVAMain XML document & return the list of programmes
 */
list_t *tva_parse(const bstring s)
{
    tva_context_t *ctx = tva_ctx_alloc();
    check_mem(ctx);

    LIBXML_TEST_VERSION

    xmlSAXHandler handler = {
        0, /* internalSubset */
        0, /* isStandalone */
        0, /* hasInternalSubset */
        0, /* hasExternalSubset */
        0, /* resolveEntity */
        (getEntitySAXFunc)tva_get_entity, /* getEntity */
        0, /* entityDecl */
        0, /* notationDecl */
        0, /* attributeDecl */
        0, /* elementDecl */
        0, /* unparsedEntityDecl */
        0, /* setDocumentLocator */
        (startDocumentSAXFunc)tva_start_document, /* startDocument */
        (endDocumentSAXFunc)tva_end_document, /* endDocument */
        (startElementSAXFunc)tva_start_element, /* startElement */
        (endElementSAXFunc)tva_end_element, /* endElement */
        0, /* reference */
        (charactersSAXFunc)tva_characters, /* characters */
        0, /* ignorableWhitespace */
        0, /* processingInstruction */
        (commentSAXFunc)0, /* comment */
        (warningSAXFunc)tva_warning, /* warning */
        (errorSAXFunc)tva_error, /* error */
        (fatalErrorSAXFunc)tva_error, /* fatalError */
    };

    int result = xmlSAXUserParseMemory(&handler, ctx, (char *)s->data, blength(s));
    check(0 == result, "Error while parse doc - %d.", result);
    xmlCleanupParser();
    xmlMemoryDump();

    list_t *programmes = ctx->programmes;
    xmltv_programme_free(ctx->programme);
    free(ctx);
    return programmes;

error:
    if (ctx) tva_ctx_free(ctx);
    xmlCleanupParser();
    xmlMemoryDump();
    return NULL;
}

