#include <mtv/all.h>

int
main(int argc, char *argv[])
{
	mtv_conf_s *cnf = mtv_conf_load();
	check_mem(cnf);

	mtv_conf_debug(cnf);

	conf_destroy(cnf);

	return EXIT_SUCCESS;

 error:
	return EXIT_FAILURE;
}
