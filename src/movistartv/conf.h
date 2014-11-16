#ifndef __mtv_h__
#define __mtv_h__

#include <stdbool.h>
#include "core/list.h"
#include "core/xmltv.h"
#include "bstrlib/bstrlib.h"


struct conf_s {
    int              demarcation;
    struct bstrList *tvpackages;
    bstring          mcast_grp_start;
    int              mcast_port;
    int              gmt_offset;
};
typedef struct conf_s conf_t;

void conf_destroy(conf_t *cnf);
conf_t *conf_load();

#endif

