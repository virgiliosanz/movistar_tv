#include <mtv/all.h>

static const char *client_profile_url =
    "http://172.26.22.23:2001/appserver/mvtv.do?action=getClientProfile";
static const char *platform_profile_url =
    "http://172.26.22.23:2001/appserver/mvtv.do?action=getPlatformProfile";

/*
 * Helper functions for parsing json conf
 */
static void
_conf_parse_platform_json(tva_conf_s *cnf, const char *json)
{
	yajl_val node     = NULL;
	yajl_val v        = NULL;
	size_t   n_tokens = 0;
	char   **tokens   = NULL;
	char    *s        = NULL;
	char     err_buf[1024];

	error_if(cnf == NULL, error);
	error_if(json == NULL, error);

	trace("json:\n%s", json);

	node = yajl_tree_parse(json, err_buf, sizeof(err_buf));
	error_if(NULL == node, error);

	const char *path[] = {"resultData", "dvbConfig", "dvbEntryPoint", NULL};
	v = yajl_tree_get(node, path, yajl_t_string);
	error_if(v == NULL, error);

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
_conf_parse_client_json(tva_conf_s *cnf, const char *json)
{
	const char *path_demarcation[] = {"resultData", "demarcation", NULL};
	const char *path_tvPackages[] = {"resultData", "tvPackages", NULL};

	yajl_val node = NULL;
	yajl_val v    = NULL;
	char    *s    = NULL;
	char     err_buf[1024];

	error_if(cnf == NULL, error);
	error_if(json == NULL, error);
	trace("json:\n%s", json);

	node = yajl_tree_parse((char *)json, err_buf, sizeof(err_buf));
	error_if(node == NULL, error);

	v = yajl_tree_get(node, path_tvPackages, yajl_t_string);
	error_if(v == NULL, error);
	s = YAJL_GET_STRING(v);
	trace("node: %s/%s = %s\n", path_tvPackages[0], path_tvPackages[1], s);
	cnf->tvpackages = strdup(s);

	v = yajl_tree_get(node, path_demarcation, yajl_t_any);
	error_if(v == NULL, error);
	cnf->demarcation = YAJL_GET_INTEGER(v);
	trace("node: %s/%s = %d\n",
		path_demarcation[0], path_demarcation[1], cnf->demarcation);
	if (s) free(s);

	return;

error:
	if (s)    free(s);
}

static tva_conf_s *
_conf_alloc()
{
	tva_conf_s *cnf = (tva_conf_s *) malloc(sizeof(tva_conf_s));
	error_if(cnf == NULL, error);

	cnf->mcast_grp_start = NULL;
	cnf->tvpackages = NULL;

	return cnf;

error:
	if (cnf) tva_conf_destroy(cnf);
	return NULL;
}

void
tva_conf_destroy(tva_conf_s *cnf)
{
	if (!cnf) return;

	if (cnf->mcast_grp_start) free(cnf->mcast_grp_start);
	if (cnf->tvpackages)      free(cnf->tvpackages);

	free(cnf);
}

tva_conf_s *
tva_conf_load()
{
	char *json = NULL;
	tva_conf_s *cnf = _conf_alloc();

	trace("Reading client profile json");
	json = http_get(client_profile_url);
	error_if(json == NULL, error);
	_conf_parse_client_json(cnf, json);

	trace("Reading platform profile json");
	json = http_get(platform_profile_url);
	error_if(json == NULL, error);
	_conf_parse_platform_json(cnf, json);

	return cnf;

 error:
	if (json) free(json);
	if (cnf)  tva_conf_destroy(cnf);
	return NULL;
}

