#include <mtv/all.h>
#include <unistd.h>
#include <getopt.h>

// ---------------------------------------------
// Configuration
struct _conf_s {
        short            n_days_to_get;
        char            *output_path;
        char            *images_path;
        char            *tmp_path;
        epg_m3u_format_e m3u;

	// json conf
	tva_conf_s *tva_conf;
};
typedef struct _conf_s conf_s;

static conf_s *
_conf_alloc()
{
	conf_s *cnf = (conf_s *)malloc(sizeof(conf_s));
	error_if(cnf == NULL, error);

	//TODO: Meter valores por defecto

	return(cnf);

error:
	if (cnf)
		free(cnf);

	return(NULL);
}

static bool
_conf_getopt(int argc, char *argv[])
{
	// TODO: Parse command line
	//
	return false;
}


// -------
static void
_configure_clean(conf_s *cnf)
{
	tva_conf_destroy(cnf->tva_conf);
        free(cnf->output_path);
        free(cnf->images_path);
        free(cnf->tmp_path);
        free(cnf);
}

static conf_s *
_configure(int argc, char *argv[])
{
	conf_s *cnf = _conf_alloc();
	error_if(cnf == NULL, error);

	bool ok = _conf_getopt(argc, argv);
	error_if (ok != true, error);

	cnf->tva_conf = tva_conf_load();
	error_if(cnf->tva_conf == NULL, error);


	return cnf;

 error:
	if (cnf) _configure_clean(cnf);
	return NULL;

}


// ---------------------------------------------
// Process

// ---------------------------------------------
// Main
//
int
main(int argc, char *argv[])
{
	conf_s *cnf = _configure(argc, argv);
	error_if(cnf == NULL, error);

	// Leer canales
	// Crear un thread por channel y día.
	// recorrer canales y días.

	_configure_clean(cnf);
	return EXIT_SUCCESS;

error:
	if (cnf) _configure_clean(cnf);
	return EXIT_FAILURE;
}

