#include "config.h"

bool
mcast_cb(const mcast_file_s *file, void *ctx)
{
	mu_assert(NULL != file, "file is NULL!!");
	mu_assert(NULL != ctx,  "ctx is NULL!!")

	size_t *n = (size_t *)ctx;
	(*n) ++;

	char *fname;
	asprintf(&fname, "%u_%u.xml", file->type, file->id);
	trace("File number: %lu -> Saved in %s (%u bytes == %lu)",
	      *n, fname, file->size, strlen(file->data));

	FILE *f = fopen(fname, "w");
	fputs(file->data, f);
	fclose(f);

	free(fname);

	return true;
}

char *
test_multicast()
{
	mcast_s *mcast = NULL;
	size_t n       = 0;

	mcast = mcast_alloc();
	mu_assert(NULL != mcast, "mcast is NULL!!");

	// De aquí se lee cual es el grupo de nuestro area (sale del json de conf)
	mcast_open(mcast, "239.0.2.129", 3937);
	mcast_proccess_files(mcast, mcast_cb, &n);
	mcast_close(mcast);

	// Aquí viene toda la conf de canales (3 ficheros)
	mcast_open(mcast, "239.0.2.154", 3937);
	mcast_proccess_files(mcast, mcast_cb, &n);
	mcast_close(mcast);

	// Programación de mañana?
	mcast_open(mcast, "239.0.2.131", 3937);
	mcast_proccess_files(mcast, mcast_cb, &n);
	mcast_close(mcast);

	mcast_free(mcast);
	return NULL;
}

char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_multicast);
	return NULL;
}

RUN_TESTS(all_tests)

