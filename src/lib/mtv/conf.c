#include <mtv/all.h>

static const char *client_profile_url =
    "http://172.26.22.23:2001/appserver/mvtv.do?action=getClientProfile";
static const char *platform_profile_url =
    "http://172.26.22.23:2001/appserver/mvtv.do?action=getPlatformProfile";

/*
 * Helper functions for parsing json conf
 */
static void
_conf_parse_platform_json(mtv_conf_s *cnf, const char *json)
{
	yajl_val node     = NULL;
	yajl_val v        = NULL;
	size_t   n_tokens = 0;
	char   **tokens   = NULL;
	char    *s        = NULL;
	char     err_buf[1024];

	ok_or_goto(cnf != NULL, error);
	ok_or_goto(json != NULL, error);
	trace("json:\n%s", json);

	node = yajl_tree_parse(json, err_buf, sizeof(err_buf));
	ok_or_goto(NULL != node, error);

	const char *path[] = {"resultData", "dvbConfig", "dvbEntryPoint", NULL};
	v = yajl_tree_get(node, path, yajl_t_string);
	ok_or_goto(v != NULL, error);

	s = YAJL_GET_STRING(v);
	trace("node: %s/%s/%s = %s\n", path[0], path[1], path[2], s);
	n_tokens = str_split(s, ':', &tokens);
	free(s);

	cnf->mcast_grp_start = strdup(tokens[0]);
	cnf->mcast_port      = atoi(tokens[1]);

	str_free_tokens(tokens, n_tokens);
	yajl_tree_free(node);
	return;

error:
	if (node)
		yajl_tree_free(node);
	if (tokens)
		str_free_tokens(tokens, n_tokens);
	if (s)
		free(s);
}

static void
_conf_parse_client_json(mtv_conf_s *cnf, const char *json)
{
	const char *path_demarcation[] = {"resultData", "demarcation", NULL};
	const char *path_tvPackages[] = {"resultData", "tvPackages", NULL};

	yajl_val node = NULL;
	yajl_val v    = NULL;
	char    *s    = NULL;
	char     err_buf[1024];

	ok_or_goto(cnf != NULL, error);
	ok_or_goto(json != NULL, error);
	trace("json:\n%s", json);

	node = yajl_tree_parse((char *)json, err_buf, sizeof(err_buf));
	ok_or_goto(node, error);

	v = yajl_tree_get(node, path_tvPackages, yajl_t_string);
	ok_or_goto(v != NULL, error);
	s = YAJL_GET_STRING(v);
	trace("node: %s/%s = %s\n", path_tvPackages[0], path_tvPackages[1], s);
	cnf->tvpackages = strdup(s);

	v = yajl_tree_get(node, path_demarcation, yajl_t_any);
	ok_or_goto(v != NULL, error);
	cnf->demarcation = YAJL_GET_INTEGER(v);
	trace("node: %s/%s = %d\n",
		path_demarcation[0], path_demarcation[1], cnf->demarcation);

	free(s);
	yajl_tree_free(node);
	return;

error:
	if (node)
		yajl_tree_free(node);
	if (s)
		free(s);
}

static mtv_conf_s *
_conf_alloc()
{
	mtv_conf_s *cnf = (mtv_conf_s *) malloc(sizeof(mtv_conf_s));
	ok_or_goto(cnf != NULL, error);

	cnf->mcast_grp_start = NULL;
	cnf->tvpackages = NULL;

	return cnf;

error:
	if (cnf)
		mtv_conf_destroy(cnf);
	return NULL;
}

void
mtv_conf_destroy(mtv_conf_s *cnf)
{
	if (!cnf)
		return;

	if (cnf->mcast_grp_start)
		free(cnf->mcast_grp_start);
	if (cnf->tvpackages)
		free(cnf->tvpackages);

	free(cnf);
}

mtv_conf_s *
mtv_conf_load()
{
	char *json = NULL;
	mtv_conf_s *cnf = _conf_alloc();

	json = mtv_http_get(client_profile_url);
	ok_or_goto(json != NULL, error);
	_conf_parse_client_json(cnf, json);
	free(json);

	json = mtv_http_get(platform_profile_url);
	ok_or_goto(json != NULL, error);
	_conf_parse_platform_json(cnf, json);
	free(json);

	return cnf;

 error:
	if (json)
		free(json);
	if (cnf)
		mtv_conf_destroy(cnf);
	return NULL;
}

