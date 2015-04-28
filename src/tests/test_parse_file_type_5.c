#include "config.h"

#define XML_TEST_FILE "misc/data/239.0.2.154/5_0.xml"
#define TVPACKAGES "UTX00|UTX2E"

char *
test_parse()
{
	char   *xml      = NULL;
	list_s *channels = NULL;
	struct mtv_channel_order *chann;

	xml = read_file(XML_TEST_FILE);
	mu_assert(NULL != xml, "Error reading file: %s: %s", XML_TEST_FILE, strerror(errno));
	trace("%s readed", XML_TEST_FILE);

	channels = mtv_parse_file_type_5(xml, TVPACKAGES);
	mu_assert(NULL != channels, "Error parsing file");
	trace("Hay %zu channels in %s", list_count(channels), XML_TEST_FILE);

	list_foreach(channels, first, next, cur) {
		chann = (struct mtv_channel_order *)cur->value;
		trace("%s -> %u", chann->id, chann->order);
	}

	trace("Borrando lista de canales");
	mtv_channel_order_free_list(channels);
	mu_assert(NULL != channels, "Channels should be null after freed");

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

