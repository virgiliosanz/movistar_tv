#define _XOPEN_SOURCE
#include <strings.h>
#include <time.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>

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


/*
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

typedef enum {
	TVA_STATE_START,
	TVA_STATE_END,
	TVA_STATE_SCHEDULE_EVENT,
	TVA_STATE_TITLE,
	TVA_STATE_GENRE,
	TVA_STATE_GENRE_NAME,
	TVA_STATE_PARENTAL_GUIDANCE,
	TVA_STATE_START_TIME,
	TVA_STATE_DURATION,
	TVA_STATE_EVENT_START,
	TVA_STATE_EVENT_DURATION,
	TVA_STATE_UNKNOWN,
} parser_state;

struct tva_context_s {
	int depth;
	parser_state state;
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
	ctx->state = TVA_STATE_END;
	ctx->depth= 0;
	bdestroy(ctx->chars);
}

static void
tva_start_element(tva_context_s *ctx, const xmlChar *xname, const xmlChar **attrs)
{

	ctx->depth++;
	debug("\tStart Element (%d): %s", ctx->depth, xname);

	const char *name = (const char *)xname;
	bassigncstr(ctx->chars, "");

	if (0 ==  strcasecmp("ScheduleEvent", name)) {
		ctx->state = TVA_STATE_SCHEDULE_EVENT;
		ctx->programme = xmltv_programme_alloc();
	}
	else if (0 ==  strcasecmp("Title", name)) {
		ctx->state = TVA_STATE_TITLE;
	}
	else if (0 ==  strcasecmp("PublishedStartTime", name)) {
		ctx->state = TVA_STATE_EVENT_START;
	}
	else if (0 ==  strcasecmp("EventStartTime", name)) {
		ctx->state = TVA_STATE_EVENT_DURATION;
	}
	else if (0 ==  strcasecmp("EventDuration", name)) {
		ctx->state = TVA_STATE_DURATION;
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
	switch (ctx->state) {
		case TVA_STATE_TITLE:
			bassign(ctx->programme->title, ctx->chars);
			break;

		case TVA_STATE_EVENT_START:

			break;
		default:
			break;

	}
	debug("\tEnd Element: %s", name);
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
 */
list_s *
tva_parse(const bstring s)
{
	tva_context_s *ctx = tva_ctx_alloc();
	check_mem(ctx);

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
					   (char *)s->data,
					   blength(s));

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
