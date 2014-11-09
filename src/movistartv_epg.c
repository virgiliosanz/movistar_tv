#include <stdio.h>
#include <stdlib.h>
#include "core/dbg.h"
#include "movistartv/mtv.h"

int main(int argc, char* argv[])
{
    mtv_conf_t *cnf = mtv_load_conf();
    check_mem(cnf);
    debug("Leida la configuración correctamente");

    mtv_conf_destroy(cnf);

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}

