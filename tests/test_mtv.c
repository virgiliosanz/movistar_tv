#include "core/minunit.h"
#include <string.h>
#include <stdlib.h>
#include "movistartv/mtv.h"

mtv_conf_t *conf;

char *test_load_conf()
{
    conf = mtv_load_conf();
    mu_assert(conf, "Error Loading conf");
    debug("Demarcation: %d - %s:%d",
          conf->demarcation, conf->mcast_grp_start->data, conf->mcast_port);
    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_load_conf);
    return NULL;
}

RUN_TESTS(all_tests)

