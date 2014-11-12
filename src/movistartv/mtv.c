#include <yajl/yajl_parse.h>
#include <yajl/yajl_tree.h>
#include "core/dbg.h"
#include "core/net.h"
#include "movistartv/mtv.h"

static const char *client_profile_url = "http://172.26.22.23:2001/appserver/mvtv.do?action=getClientProfile";
static const char *platform_profile_url = "http://172.26.22.23:2001/appserver/mvtv.do?action=getPlatformProfile";


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

void mtv_parse_platform_json(mtv_conf_t *cnf, const char *json)
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

void mtv_parse_client_json(mtv_conf_t *cnf, const char *json)
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


mtv_conf_t *mtv_load_conf()
{
    char *json = NULL;
    mtv_conf_t *cnf = _mtv_conf_alloc();

    json = net_http_get(client_profile_url);
    check(json, "Error getting %s", client_profile_url);
    mtv_parse_client_json(cnf, json);
    free(json);

    json = net_http_get(platform_profile_url);
    check(json, "Error getting %s", platform_profile_url);
    mtv_parse_platform_json(cnf, json);
    free(json);

    return cnf;

error:
    if (json) free(json);
    if (cnf) mtv_conf_destroy(cnf);
    return NULL;
}


/*
static void _on_start_element(
    void *ctx,
    const xmlChar *localname,
    const xmlChar *prefix,
    const xmlChar *URI,
    int nb_namespaces,
    const xmlChar **namespaces,
    int nb_attributes,
    int nb_defaulted,
    const xmlChar **attributes
)
{
    printf("<%s>\n", localname);
}

static void _on_end_element(
    void* ctx,
    const xmlChar* localname,
    const xmlChar* prefix,
    const xmlChar* URI
)
{
    printf("</%s>\n", localname);
}

static void _on_characters(void *ctx, const xmlChar *ch, int len)
{
    char chars[len + 1];
    strncpy(chars, (const char *)ch, len);
    chars[len] = (char)NULL;
    printf("[%s]\n", chars);
}

char *xml_parse(const char *xml, xmltv_t *xmltv)
{
    xmlSAXHandler SAXHander;

    memset(&SAXHander, 0, sizeof(xmlSAXHandler));

    SAXHander.initialized = XML_SAX2_MAGIC;
    SAXHander.startElementNs = OnStartElement;
    SAXHander.endElementNs = OnEndElement;
    SAXHander.characters = OnCharacters;

    xmlParserCtxtPtr ctxt = xmlCreatePushParserCtxt(
        &SAXHander, NULL, xml, res, NULL
    );

    while ((res = fread(chars, 1, sizeof(chars), f)) > 0) {
        check(xmlParseChunk(ctxt, chars, res, 0));
    }

    xmlFreeParserCtxt(ctxt);
    xmlCleanupParser();

    return NULL;

error:
    xmlParserError(ctxt, "xmlParseChunk");
    xmlFreeParserCtxt(ctxt);
    xmlCleanupParser();
    return NULL;
}
*/

