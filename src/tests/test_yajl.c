#include "config.h"

const char *json_str = "{  \
\"resultCode\":0, \
\"resultText\":\"Operation successful\", \
\"resultData\": { \
	\"catchUp\":0, \
	\"genre\":\"SERIES\", \
	\"endtime\":1408926960000, \
	\"begintime\":1408925100000, \
	\"productionDate\":[\"1969\"], \
	\"ageRatingID\":\"2\", \
	\"countries\":[\"Suecia\"], \
	\"version\":[\"0\"], \
	\"duration\":1860, \
	\"title\":\"Pippi Calzaslargas: Pippi va de compras\", \
	\"originalLongTitle\":[\"Pippi Langstrump\"], \
	\"description\":\"Pippi es un personaje entrañable. Pecosa y peinada con dos rojizas coletas, es capaz de levantar vacas o caballos, hacer volar por los aires a policías y ganar a los más temibles pillos callejeros. Su vida es maravillosa y su ruinosa casa, la envidia de cualquier niño.\", \
	\"originalTitle\":[\"Pippi Langstrump\"], \
	\"longTitle\":[\"Pippi Calzaslargas: Pippi va de compras\"], \
	\"subgenre\":\"COMEDIA\", \
	\"mainActors\":[\"Inger Nilsson,Maria Persson,Pär Sundberg\"], \
	\"startOver\":0, \
	\"seriesID\":\" \
\"}, \
\"hashCode\":\"f4b4f0e20b4926878a8d9625bcb35ed8\" }";


char *
test_parsing()
{
	yajl_val node, v;
	char     err_buf[1024];
	char    *s;

	node = yajl_tree_parse(json_str, err_buf, sizeof(err_buf));
	mu_assert(NULL != node, "Error Parsing json");

	// desc
	const char *desc_path[] = {"resultData", "description", NULL};
	v = yajl_tree_get(node, desc_path, yajl_t_string);
	mu_assert(!YAJL_IS_NULL(v), "Cannot find description");
	s = YAJL_GET_STRING(v);
	trace("resultData->description: %s", s);
	free(s);


	// directors
	const char *directors_path[] = {"resultData", "directors", NULL};
	v = yajl_tree_get(node, directors_path, yajl_t_string);
	mu_assert(NULL == v, "there is no directors");
	// actors
	const char *actors_path[] = {"resultData", "mainActors", NULL};
	v = yajl_tree_get(node, actors_path, yajl_t_array);
	mu_assert(!YAJL_IS_NULL(v), "Cannot find mainActors");
	trace("main Actors is an Array of %zu values", v->u.array.len);
	for (int i=0; i < v->u.array.len; i++) {
		s = YAJL_GET_STRING(v->u.array.values[i]);
		trace("Actor %d: %s", i, s);
	}


	// date
	trace("Leyendo date");
	const char *date_path[] = { "resultData", "productionDate", (const char *)0 };
	v = yajl_tree_get(node, date_path, yajl_t_array);
	mu_assert(!YAJL_IS_NULL(v), "Cannot find productionDate");
	mu_assert(YAJL_IS_ARRAY(v), "productionDate is not an array");
	mu_assert(v->u.array.len == 1, "productionDate should be 1");
	int year = atoi(YAJL_GET_STRING(v->u.array.values[0]));
	mu_assert(year == 1969, "Year is not 1969 -> %d", year);

	// country
	trace("Leyendo country");
	const char *countries_path[] = { "resultData", "countries", (const char *)0 };
	v = yajl_tree_get(node, countries_path, yajl_t_array);
	mu_assert(!YAJL_IS_NULL(v), "Cannot find countries");
	mu_assert(YAJL_IS_ARRAY(v), "countries is not an array");
	trace("main Countries is an Array of %zu values", v->u.array.len);
	for (int i=0; i < v->u.array.len; i++) {
		s = YAJL_GET_STRING(v->u.array.values[i]);
		trace("Country %d: %s", i, s);
	}

	return NULL;
}

char *
all_tests()
{
	mu_suite_start();
	mu_run_test(test_parsing);
	return NULL;
}

RUN_TESTS(all_tests)

