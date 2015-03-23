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

	// json conf
	tva_conf_s *tva_conf;
};
typedef struct _conf_s conf_s;

static conf_s *
_conf_alloc()
{
	conf_s *cnf = (conf_s *)malloc(sizeof(conf_s));
	error_if(cnf == NULL, error, "Error Allocating Memory");

	cnf->n_days_to_get = MTV_N_DAYS_TO_GET;
	cnf->output_path   = strdup(MTV_OUTPUT_PATH);
	cnf->images_path   = strdup(MTV_IMAGES_PATH);
	cnf->tmp_path      = strdup(MTV_TMP_PATH);

	return(cnf);

error:
	if (cnf) free(cnf);

	return NULL;
}

static void
_conf_help(int argc, char *argv[])
{
	char *msg = "TODO, Escribir ayuda!\n"
"%s -n <days to get> -o <output path> -i <image path> -t <tmp path>\n\n";

	printf(msg, argv[0]);
}

static bool
_conf_getopt(conf_s *cnf, int argc, char *argv[])
{
	int c;
	int n;

	while ((c = getopt(argc, argv, "hn:o:i:t:v")) != -1) {
		switch(c) {
		case 'h':
			goto help;
		case '?':
			goto help;

		case 'n':
			n = atoi(optarg);
			if (1 > n) goto help;
			cnf->n_days_to_get = n;
			break;
		case 'o':
			cnf->output_path = strdup(optarg);
			break;
		case 'i':
			cnf->images_path = strdup(optarg);
			break;
		case 't':
			cnf->tmp_path = strdup(optarg);
			break;
		case 'v':
			debug_set_level(debug_level_all);
		}

	}
	return true;

help:
	_conf_help(argc, argv);
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
	debug_set_level(debug_level_warn);

	conf_s *cnf = _conf_alloc();
	error_if(cnf == NULL, error, "Error Allocating Conf.");

	bool ok = _conf_getopt(cnf, argc, argv);
	error_if (ok != true, error, "Error reading command opts");

	activity("Loading Movistar configuration...");
	cnf->tva_conf = tva_conf_load();
	error_if(cnf->tva_conf == NULL, error, "Erro reading conf");
	tva_conf_debug(cnf->tva_conf);

	activity("demarcation: %d, "
	    "mcast_grp_start: %s mcast_port: %d, "
	    "gmt_offset: %d tvpackages: %s",
	    cnf->tva_conf->demarcation,
	    cnf->tva_conf->mcast_grp_start, cnf->tva_conf->mcast_port,
	    cnf->tva_conf->gmt_offset, cnf->tva_conf->tvpackages);

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
	error_if(cnf == NULL, error, "Error reading configuration");

	setlocale(LC_ALL, "spanish");

	// Leer canales
	// Crear un thread por channel y día.
	// recorrer canales y días.

	_configure_clean(cnf);
	return EXIT_SUCCESS;

error:
	if (cnf) _configure_clean(cnf);
	return EXIT_FAILURE;
}

