#include <stdlib.h>
#include "movistartv/dbg.h"
#include "movistartv/net.h"
#include "movistartv/mtv.h"

static const char *client_profile_url = "http://172.26.22.23:2001/appserver/mvtv.do?action=getClientProfile";
static const char *platform_profile_url = "http://172.26.22.23:2001/appserver/mvtv.do?action=getPlatformProfile";

mtv_error_t mtv_load_conf(mtv_conf_t *conf)
{
    char *client_profile = NULL;
    char *platform_profile = NULL;

    client_profile = net_http_get(client_profile_url);
    check(NULL != client_profile, "Error de acceso a: %s", client_profile_url);


    platform_profile = net_http_get(platform_profile_url);
    check(NULL != platform_profile, "Error de acceso  a: %s", platform_profile_url);

error:
    if (client_profile) free(client_profile);
    if (platform_profile) free(platform_profile);
    return MTV_NETWORK_ERROR;
}

