#include <mtv/all.h>

static const char *client_profile_url =
    "http://172.26.22.23:2001/appserver/mvtv.do?action=getClientProfile";
static const char *platform_profile_url =
    "http://172.26.22.23:2001/appserver/mvtv.do?action=getPlatformProfile";

/*
 * Helper functions for parsing json conf
 */
static void
_conf_parse_platform_json(struct tva_conf *cnf, const char *json)
{
	yajl_val node     = NULL;
	yajl_val v        = NULL;
	size_t   n_tokens = 0;
	char   **tokens   = NULL;
	char    *s        = NULL;
	char     err_buf[1024];

	error_if(cnf == NULL, error, "Param Error");
	error_if(json == NULL, error, "Param Error");

	trace("json:\n%s", json);

	node = yajl_tree_parse(json, err_buf, sizeof(err_buf));
	error_if(NULL == node, error, "Error Creating Json Parser");

	const char *path[] = {"resultData", "dvbConfig", "dvbEntryPoint", NULL};
	v = yajl_tree_get(node, path, yajl_t_string);
	error_if(v == NULL, error, "Cannot find dbvEntryPoint");

	s = YAJL_GET_STRING(v);
	trace("node: %s/%s/%s = %s\n", path[0], path[1], path[2], s);
	n_tokens = str_split(s, ':', &tokens);
	cnf->mcast_grp_start = strdup(tokens[0]);
	cnf->mcast_port      = atoi(tokens[1]);
	trace("mcast group IP: %s port: %d",
	      cnf->mcast_grp_start, cnf->mcast_port);

	if (s) free(s);
	str_free_tokens(tokens, n_tokens);

	return;

error:
	if (tokens) str_free_tokens(tokens, n_tokens);
	if (s)      free(s);
}

static void
_conf_parse_client_json(struct tva_conf *cnf, const char *json)
{
	const char *path_demarcation[] = {"resultData", "demarcation", NULL};
	const char *path_tvPackages[] = {"resultData", "tvPackages", NULL};

	yajl_val node = NULL;
	yajl_val v    = NULL;
	char    *s    = NULL;
	char     err_buf[1024];

	error_if(cnf == NULL, error, "Param Error");
	error_if(json == NULL, error, "Param Error");
	trace("json:\n%s", json);

	node = yajl_tree_parse((char *)json, err_buf, sizeof(err_buf));
	error_if(node == NULL, error, "Error Parsing json: %s", err_buf);

	v = yajl_tree_get(node, path_tvPackages, yajl_t_string);
	error_if(v == NULL, error, "Error getting tvPackages");
	s = YAJL_GET_STRING(v);
	trace("node: %s/%s = %s\n", path_tvPackages[0], path_tvPackages[1], s);
	cnf->tvpackages = strdup(s);

	v = yajl_tree_get(node, path_demarcation, yajl_t_any);
	error_if(v == NULL, error, "Error geting demarcation");
	cnf->demarcation = YAJL_GET_INTEGER(v);
	trace("node: %s/%s = %d\n",
		path_demarcation[0], path_demarcation[1], cnf->demarcation);
	if (s) free(s);

	return;

error:
	if (s) free(s);
}

static struct tva_conf *
_conf_alloc()
{
	struct tva_conf *cnf = (struct tva_conf *) malloc(sizeof(struct tva_conf));
	error_if(cnf == NULL, error, "Error Allocating Memory");

	cnf->mcast_grp_start = NULL;
	cnf->tvpackages = NULL;

	return cnf;

error:
	if (cnf) tva_conf_destroy(cnf);
	return NULL;
}

void
tva_conf_destroy(struct tva_conf *cnf)
{
	if (!cnf) return;

	if (cnf->mcast_grp_start) free(cnf->mcast_grp_start);
	if (cnf->tvpackages)      free(cnf->tvpackages);
	free(cnf);
}

struct tva_conf *
tva_conf_load()
{
	char *json = NULL;
	struct tva_conf *cnf = _conf_alloc();

	trace("Reading client profile json");
	json = http_get(client_profile_url);
	error_if(json == NULL, error, "Error getting profile json");
	_conf_parse_client_json(cnf, json);

	trace("Reading platform profile json");
	json = http_get(platform_profile_url);
	error_if(json == NULL, error, "Error getting platform json");
	_conf_parse_platform_json(cnf, json);

	return cnf;

 error:
	if (json) free(json);
	if (cnf)  tva_conf_destroy(cnf);
	return NULL;
}

