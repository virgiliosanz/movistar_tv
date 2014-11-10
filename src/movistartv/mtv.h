#ifndef __mtv_h__
#define __mtv_h__

#include <stdbool.h>
#include "core/list.h"
#include "bstrlib/bstrlib.h"


struct mtv_conf_s {
    unsigned short demarcation;
    list_t        *tvpackages;
    bstring        mcast_grp_start;
    unsigned short mcast_port;
};
typedef struct mtv_conf_s mtv_conf_t;

void mtv_conf_destroy(mtv_conf_t *cnf);
mtv_conf_t *mtv_load_conf();
void mtv_parse_client_json(mtv_conf_t *cnf, const char *json);
void mtv_parse_platform_json(mtv_conf_t *cnf, const char *json);

#endif

