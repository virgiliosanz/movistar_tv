#include "core/dbg.h"
#include "core/net.h"
#include "movistartv/mtv.h"

static const char *client_profile_url = "http://172.26.22.23:2001/appserver/mvtv.do?action=getClientProfile";
static const char *platform_profile_url = "http://172.26.22.23:2001/appserver/mvtv.do?action=getPlatformProfile";


static mtv_conf_t *_mtv_conf_create()
{
    mtv_conf_t *cnf = (mtv_conf_t *) malloc(sizeof(mtv_conf_t));
    check_mem(cnf);

    cnf->cast_grp_start = bfromcstr("");
    cnf->tvpackages = list_create();

    return cnf;

error:
    if (cnf) mtv_conf_destroy(cnf);
    return NULL;
}

void mtv_conf_destroy(mtv_conf_t *cnf)
{
   bdestroy(cnf->cast_grp_start);
   list_clear_destroy(cnf->tvpackages);
   free(cnf);
}


mtv_conf_t *mtv_load_conf()
{
    mtv_conf_t *cnf = _mtv_conf_create();

    bstring client_profile = NULL;
    bstring platform_profile = NULL;

    client_profile = net_http_get(client_profile_url);
    check(NULL != client_profile, "Error de acceso a: %s", client_profile_url);


    platform_profile = net_http_get(platform_profile_url);
    check(NULL != platform_profile, "Error de acceso  a: %s", platform_profile_url);

    return cnf;

error:
    if (client_profile) free(client_profile);
    if (platform_profile) free(platform_profile);
    if (cnf) mtv_conf_destroy(cnf);
    return NULL;
}

