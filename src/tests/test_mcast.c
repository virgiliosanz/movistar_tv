#include "config.h"
#include <sys/stat.h>

struct context {
	size_t n;
	char path[PATH_MAX];
};

bool
mcast_cb(const struct mcast_file *file, void *_ctx)
{
	mu_assert(NULL != file, "file is NULL!!");
	mu_assert(NULL != _ctx,  "context is NULL!!");

	struct context *ctx = (struct context *)_ctx;

	trace("Creando directorio: %s (%zu)", ctx->path, ctx->n);
	mkpath(ctx->path, 0777);

	char fname[PATH_MAX];
	snprintf(fname, PATH_MAX, "%s/%u_%u.xml", ctx->path, file->type, file->id);
	trace("File number: %lu -> Saved in %s (%u bytes == %lu)",
	      ctx->n, fname, file->size, strlen(file->data));

	FILE *f = fopen(fname, "w");
	fputs(file->data, f);
	fclose(f);

	ctx->n ++;

	return true;
}

char *
test_multicast()
{
	struct mcast  *mcast = NULL;
	struct context ctx = {0, {0}};

	mcast = mcast_alloc();
	mu_assert(NULL != mcast, "mcast is NULL!!");

	char *hosts[] = {
		// De aquí se lee cual es el grupo de nuestro area (sale del json de conf)
		"239.0.2.129"

		/* Channels info */
		,"239.0.2.154"
		,"239.0.2.155"

		/* EPG 7 days */
		,"239.0.2.130"
		,"239.0.2.131"
		,"239.0.2.132"
//		,"239.0.2.133"
//		,"239.0.2.134"
//		,"239.0.2.135"
//		,"239.0.2.136"
//		,"239.0.2.137"
		,NULL
	};

	char *host;
	for (int i = 0; hosts[i]; i++) {
		host = hosts[i];
		snprintf(ctx.path, PATH_MAX, "misc/data/%s/", host);
		trace("CTX -> %s / %zu (%d)", ctx.path, ctx.n, PATH_MAX);
		mcast_open(mcast, host, 3937);
		mcast_proccess_files(mcast, mcast_cb, &ctx);
		mcast_close(mcast);
	}

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

