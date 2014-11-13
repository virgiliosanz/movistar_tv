#include <yajl/yajl_parse.h>
#include <yajl/yajl_tree.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>
#include "core/dbg.h"
#include "core/net.h"
#include "core/xmltv.h"

#include "movistartv/mtv.h"

static const char *client_profile_url = "http://172.26.22.23:2001/appserver/mvtv.do?action=getClientProfile";
static const char *platform_profile_url = "http://172.26.22.23:2001/appserver/mvtv.do?action=getPlatformProfile";

static void _mtv_parse_platform_json(mtv_conf_t *cnf, const char *json);
static void _mtv_parse_client_json(mtv_conf_t *cnf, const char *json);

static void _mtv_start_element_tva(void * ctx,
    const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI,
    int nb_namespaces, const xmlChar **namespaces,
    int nb_attributes, int nb_defaulted, const xmlChar **attributes);
static void _mtv_characters_tva(void *ctx, const xmlChar *ch, int len);
static void _mtv_end_element_tva(void *ctx,
    const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI);
static void _mtv_error_tva(void *ctx, const char *msg, ... );
static void _mtv_warning_tva(void *ctx, const char *msg, ...);


static mtv_conf_t *_mtv_conf_alloc()
{
    mtv_conf_t *cnf = (mtv_conf_t *) malloc(sizeof(mtv_conf_t));
    check_mem(cnf);

    cnf->mcast_grp_start = bfromcstr("");
    cnf->tvpackages = list_create();

    return cnf;

error:
    if (cnf) mtv_conf_destroy(cnf);
    return NULL;
}

void mtv_conf_destroy(mtv_conf_t *cnf)
{
   bdestroy(cnf->mcast_grp_start);
   list_foreach(cnf->tvpackages, first, next, cur) {
       bdestroy((bstring) cur);
   }
   list_clear_destroy(cnf->tvpackages);
   free(cnf);
}

mtv_conf_t *mtv_load_conf()
{
    char *json = NULL;
    mtv_conf_t *cnf = _mtv_conf_alloc();

    json = net_http_get(client_profile_url);
    check(json, "Error getting %s", client_profile_url);
    _mtv_parse_client_json(cnf, json);
    free(json);

    json = net_http_get(platform_profile_url);
    check(json, "Error getting %s", platform_profile_url);
    _mtv_parse_platform_json(cnf, json);
    free(json);

    return cnf;

error:
    if (json) free(json);
    if (cnf) mtv_conf_destroy(cnf);
    return NULL;
}



xmltv_t *mtv_parse_tva(const char *s)
{

    xmltv_t *xml = NULL;
    xml = xmltv_alloc();
    check_mem(xml);


    LIBXML_TEST_VERSION

    xmlSAXHandler handler;
    memset(&handler, 0, sizeof(handler));
    // Using xmlSAXVersion( &hanlder, 2 ) generate crash as it sets plenty of other pointers...
    handler.initialized = XML_SAX2_MAGIC;  // so we do this to force parsing as SAX2.
    handler.startElementNs = _mtv_start_element_tva;
    handler.characters = _mtv_characters_tva;
    handler.endElementNs = _mtv_end_element_tva;
    handler.warning = _mtv_error_tva;
    handler.error = _mtv_error_tva;

    int result = xmlSAXUserParseMemory(&handler, &xml, s, strlen(s));
    check(0 == result, "Error while parse doc - %d.", result);
    xmlCleanupParser();
    xmlMemoryDump();


    return xml;

error:
    if (xml) xmltv_free(xml);
    xmlCleanupParser();
    xmlMemoryDump();
    return NULL;
}

static void _mtv_start_element_tva(void * ctx,
    const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI,
    int nb_namespaces, const xmlChar **namespaces,
    int nb_attributes, int nb_defaulted, const xmlChar **attributes)
{
    xmltv_t *xml = (xmltv_t *) ctx;

    debug("name = '%s' prefix = '%s' uri = (%p)'%s'", localname, prefix, URI, URI);
    for (int i= 0; i< nb_namespaces; ++i) {
        const xmlChar *prefix = namespaces[i*2];
        const xmlChar *nsURI = namespaces[i*2+1];
        debug("  namespace: name='%s' uri=(%p)'%s'", prefix, nsURI, nsURI);
    }


    int j = 0;
    for (int i = 0; i < nb_attributes; ++i, j += 5 ) {
        const xmlChar *localname = attributes[j];
        const xmlChar *prefix = attributes[j+1];
        const xmlChar *nsURI = attributes[j+2];
        const xmlChar *valueBegin = attributes[j+3];
        const xmlChar *valueEnd = attributes[j+4];
        //std::string value( (const char *)valueBegin, (const char *)valueEnd );
        bstring s = bfromcstr("");
        debug("  %sattribute: localname='%s', prefix='%s', uri=(%p)'%s', value='%s'",
                i >= (nb_attributes - nb_defaulted) ? "defaulted " : "",
                localname, prefix, nsURI, nsURI, s->data);
   }
}

static void _mtv_characters_tva(void *ctx, const xmlChar *ch, int len)
{
    debug("Content: %s", (char *)ch);
}

static void _mtv_end_element_tva(void *ctx,
    const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI)
{
    xmltv_t *xml = (xmltv_t *)ctx;
    debug("name = '%s' prefix = '%s' uri = '%s'", localname, prefix, URI);
}

static void _mtv_error_tva(void *ctx, const char *msg, ... )
{
    xmltv_t *xml = (xmltv_t *)ctx;

    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
}

static void _mtv_warning_tva(void *ctx, const char *msg, ...)
{
    xmltv_t *xml = (xmltv_t *)ctx;

    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
}


// Private functions
static void _mtv_parse_platform_json(mtv_conf_t *cnf, const char *json)
{
    const char *path[] = {"resultData", "dvbConfig", "dvbEntryPoint", (const char *) 0};

    yajl_val node; yajl_val v;
    char err_buf[1024];
    bstring s;
    struct bstrList *list = NULL;

    node = yajl_tree_parse(json, err_buf, sizeof(err_buf));
    check(NULL != node, "Error Parsing json: %s", err_buf);

    v = yajl_tree_get(node, path, yajl_t_string);
    check(v != NULL, "Error reading multicast conf");
    s = bfromcstr(YAJL_GET_STRING(v));
    debug("node: %s/%s/%s = %s\n", path[0], path[1], path[2], s->data);
    list = bsplit(s, ':');
    cnf->mcast_grp_start =  bstrcpy(list->entry[0]);
    cnf->mcast_port = atoi((const char *)list->entry[1]->data);

    bstrListDestroy(list);
    yajl_tree_free(node);
    return;

error:
    if (node) yajl_tree_free(node);
    if (list) bstrListDestroy(list);
}

static void _mtv_parse_client_json(mtv_conf_t *cnf, const char *json)
{
    const char *path_demarcation[] = {"resultData", "demarcation", (const char *) 0};
    const char *path_tvPackages[] = {"resultData", "tvPackages", (const char *) 0};

    yajl_val node; yajl_val v;
    char err_buf[1024];
    bstring s = bfromcstr("");
    struct bstrList *list = NULL;

    node = yajl_tree_parse(json, err_buf, sizeof(err_buf));
    check(node, "Error Parsing json: %s", err_buf);

    v = yajl_tree_get(node, path_tvPackages, yajl_t_string);
    check(v != NULL, "Error reading tvPackages (client json)");
    s = bfromcstr(YAJL_GET_STRING(v));
    debug("node: %s/%s = %s\n", path_tvPackages[0], path_tvPackages[1], s->data);
    list = bsplit(s, '|');
    for (int i = 0; i < list->qty; i++) {
        list_push(cnf->tvpackages, list->entry[i]->data);
    }

    v = yajl_tree_get(node, path_demarcation, yajl_t_any);
    check(v != NULL, "Error reading demarcation (client json)");
    cnf->demarcation = YAJL_GET_INTEGER(v);
    debug("node: %s/%s = %d\n",
          path_demarcation[0], path_demarcation[1], cnf->demarcation);

    bstrListDestroy(list);
    bdestroy(s);
    yajl_tree_free(node);
    return;

error:
    if (node) yajl_tree_free(node);
    if (list) bstrListDestroy(list);
    if (s) bdestroy(s);
}

