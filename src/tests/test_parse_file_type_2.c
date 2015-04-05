#include "config.h"

#define XML_TEST_FILE "misc/xmls/239.0.2.154/2_0.xml"

char *
test_parse()
{
	char   *xml      = NULL;
	list_s *channels = NULL;

	xml = read_file(XML_TEST_FILE);
	mu_assert(NULL != xml, "Error reading file: %s: %s", XML_TEST_FILE, strerror(errno));

	channels = mtv_parse_file_type_2(xml);
	mu_assert(NULL != channels, "Error parsing file");

	list_foreach(channels, first, next, cur) {
		epg_debug_channel((struct epg_channel *)cur->value);
	}

	return NULL;
}


char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_parse);
	return NULL;
}

RUN_TESTS(all_tests);

