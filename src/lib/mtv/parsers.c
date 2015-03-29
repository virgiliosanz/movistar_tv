#include <mtv/all.h>

void
mtv_parse_datetime(struct tm *dt, const char *date_time)
{
	memset((void *)dt, 0, sizeof(struct tm));
	strptime(date_time, "%FT%T%z", dt);
	return;
}

/*

   {
   "resultCode":0,
   "resultText":"Operation successful",
   "resultData": {
   "catchUp":0,
   "genre":"SERIES",
   "endtime":1408926960000,
   "begintime":1408925100000,
   "productionDate":["1969"],
   "ageRatingID":"2",
   "countries":["Suecia"],
   "version":["0"],
   "duration":1860,
   "title":"Pippi Calzaslargas: Pippi va de compras",
   "originalLongTitle":["Pippi Langstrump"],
   "description":"Pippi es un personaje entrañable. Pecosa y peinada con dos rojizas coletas, es capaz de levantar vacas o caballos, hacer volar por los aires a policías y ganar a los más temibles pillos callejeros. Su vida es maravillosa y su ruinosa casa, la envidia de cualquier niño.",
   "originalTitle":["Pippi Langstrump"],
   "longTitle":["Pippi Calzaslargas: Pippi va de compras"],
   "subgenre":"COMEDIA",
   "mainActors":["Inger Nilsson,Maria Persson,Pär Sundberg"],
   "startOver":0,
   "seriesID":"
   "},
   "hashCode":"f4b4f0e20b4926878a8d9625bcb35ed8"
   }

*/
// TODO: Parsear el json y añdirlo al programme
void
mtv_complete_programme_info(struct epg_programme *prog, char *crid)
{
	// debug("Getting Advance info for: %s", crid);
	char       *s        = NULL;
	char      **tokens   = NULL;
	size_t      n_tokens = 0;
	const char *url_fmt  = "http://www-60.svc.imagenio.telefonica.net:2001/appserver/mvtv.do?action=getEpgInfo&extInfoID=%s&tvWholesaler=1";
	sbuf_s     *url      = sbuf_new();

	n_tokens = str_split(crid, '/', &tokens);
	sbuf_printf(url, url_fmt, tokens[n_tokens - 1]);
	str_free_tokens(tokens, n_tokens);

	char *json = http_get(sbuf_ptr(url));
	error_if(json == NULL, error, "Error getting EpgInfo");
	trace("json (%zu):\n%s", strlen(json), json);
	sbuf_delete(url);

	yajl_val node, v;
	char err_buf[1024];

	node = yajl_tree_parse(json, err_buf, sizeof(err_buf));
	error_if(NULL == node, error, "Error Parsing json");
	free(json);

	// desc
	const char *desc_path[] = {"resultData", "description", NULL};
	v = yajl_tree_get(node, desc_path, yajl_t_string);
	if (v) {
		s = YAJL_GET_STRING(v);
		prog->desc = strdup(s);
		free(s);
	}

	// actors
	const char *actors_path[] = {"resultData", "mainActors", NULL};
	v = yajl_tree_get(node, actors_path, yajl_t_string);
	if (v) {
		s = YAJL_GET_STRING(v);
		n_tokens = str_split(s, ',', &tokens);
		for (size_t i = 0; i < n_tokens; i++) {
			list_push(prog->actors, strdup(tokens[i]));
		}
		free(s);
		str_free_tokens(tokens, n_tokens);
	}

	// directors
	const char *directors_path[] = {"resultData", "directors", NULL};
	v = yajl_tree_get(node, directors_path, yajl_t_string);
	if (v) {
		s = YAJL_GET_STRING(v);
		prog->director = strdup(s);
		free(s);
	}
/*
	// date
	trace("Leyendo date");
	const char *date_path[] = { "resultData", "productionDate", (const char *)0 };
	v = yajl_tree_get(node, date_path, yajl_t_array);
	if (v) {
		array = (yajl_val)YAJL_GET_ARRAY(v);
		prog->date.tm_year = YAJL_GET_INTEGER(array->u.array.values[0]);
	}
	else {
		debug("productionDate not found");
	}

	// country
	debug("Leyendo country");
	const char *countries_path[] = { "resultData", "countries", (const char *)0 };
	v = yajl_tree_get(node, countries_path, yajl_t_array);
	if (v) {
		array = (yajl_val)YAJL_GET_ARRAY(v);
		bassigncstr(prog->country,
			    	YAJL_GET_STRING(array->u.array.values[0]));

	}
	else {
		debug("countries not found");
	}
*/
	// start
	// We could get time from here too but I'll keep the socket one

	return;

error:
	//	if (url) sbuf_delete(url);
	//	if (tokens) str_free_tokens(tokens, n_tokens);
	//	if (json) free(json);
	return;

}

