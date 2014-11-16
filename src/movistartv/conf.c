#include <yajl/yajl_parse.h>
#include <yajl/yajl_tree.h>
#include "core/dbg.h"
#include "core/net.h"
#include "movistartv/conf.h"

static const char *client_profile_url = "http://172.26.22.23:2001/appserver/mvtv.do?action=getClientProfile";
static const char *platform_profile_url = "http://172.26.22.23:2001/appserver/mvtv.do?action=getPlatformProfile";

/*
 * Helper functions for parsing json conf
 */
static void _conf_parse_platform_json(conf_t *cnf, const bstring json)
{
    const char *path[] = {"resultData", "dvbConfig", "dvbEntryPoint", (const char *) 0};

    yajl_val node; yajl_val v;
    char err_buf[1024];
    bstring s;
    struct bstrList *list = NULL;

    node = yajl_tree_parse((char *)json->data, err_buf, sizeof(err_buf));
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

static void _conf_parse_client_json(conf_t *cnf, const bstring json)
{
    const char *path_demarcation[] = {"resultData", "demarcation", (const char *) 0};
    const char *path_tvPackages[] = {"resultData", "tvPackages", (const char *) 0};

    yajl_val node; yajl_val v;
    char err_buf[1024];
    bstring s = bfromcstr("");

    node = yajl_tree_parse((char *)json->data, err_buf, sizeof(err_buf));
    check(node, "Error Parsing json: %s", err_buf);

    v = yajl_tree_get(node, path_tvPackages, yajl_t_string);
    check(v != NULL, "Error reading tvPackages (client json)");
    s = bfromcstr(YAJL_GET_STRING(v));
    debug("node: %s/%s = %s\n", path_tvPackages[0], path_tvPackages[1], s->data);
    cnf->tvpackages = bsplit(s, '|');

    v = yajl_tree_get(node, path_demarcation, yajl_t_any);
    check(v != NULL, "Error reading demarcation (client json)");
    cnf->demarcation = YAJL_GET_INTEGER(v);
    debug("node: %s/%s = %d\n",
          path_demarcation[0], path_demarcation[1], cnf->demarcation);

    bdestroy(s);
    yajl_tree_free(node);
    return;

error:
    if (node) yajl_tree_free(node);
    if (cnf->tvpackages) bstrListDestroy(cnf->tvpackages);
    if (s) bdestroy(s);
}


static conf_t *_conf_alloc()
{
    conf_t *cnf = (conf_t *) malloc(sizeof(conf_t));
    check_mem(cnf);

    cnf->mcast_grp_start = bfromcstr("");
    cnf->tvpackages = NULL;

    return cnf;

error:
    if (cnf) conf_destroy(cnf);
    return NULL;
}

void conf_destroy(conf_t *cnf)
{
   bdestroy(cnf->mcast_grp_start);
   bstrListDestroy(cnf->tvpackages);
   free(cnf);
}

conf_t *conf_load()
{
    bstring json = NULL;
    conf_t *cnf = _conf_alloc();

    json = net_http_get(bfromcstr(client_profile_url));
    check(json, "Error getting %s", client_profile_url);
    _conf_parse_client_json(cnf, json);
    bdestroy(json);

    json = net_http_get(bfromcstr(platform_profile_url));
    check(json, "Error getting %s", platform_profile_url);
    _conf_parse_platform_json(cnf, json);
    bdestroy(json);

    return cnf;

error:
    if (json) bdestroy(json);
    if (cnf) conf_destroy(cnf);
    return NULL;
}
