#include <stdio.h>
#include <stdlib.h>
#include "core/dbg.h"
#include "movistartv/conf.h"

int main(int argc, char* argv[])
{
    conf_t *cnf = conf_load();
    check_mem(cnf);
    debug("Leida la configuración correctamente");

    conf_destroy(cnf);

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}

