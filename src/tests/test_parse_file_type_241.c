#include "config.h"

const size_t _MAX_SIZE_FOR_DATE = 201;

char *
test_parse_datetime()
{
	struct tm dt;
	const char *s = "2014-11-22T19:45:24.000Z";
	const char *fmt = "%Y-%m-%dT%H:%M:%S.%z";

	mtv_parse_datetime(&dt, s);

	trace("%s -> %d/%d/%d %d:%d:%d (%d, %d)\n", s,
		dt.tm_mday, dt.tm_mon + 1, dt.tm_year + 1900,
		dt.tm_hour, dt.tm_min, dt.tm_sec,
		dt.tm_wday + 1, dt.tm_isdst);

	char str[_MAX_SIZE_FOR_DATE];
	strftime(str, _MAX_SIZE_FOR_DATE, fmt, &dt);

	struct tm dt2;
	mtv_parse_datetime(&dt2, str);
	trace("%s -> %d/%d/%d %d:%d:%d (%d, %d)\n", str,
		dt2.tm_mday, dt2.tm_mon + 1, dt2.tm_year + 1900,
		dt2.tm_hour, dt2.tm_min, dt2.tm_sec,
		dt2.tm_wday + 1, dt2.tm_isdst);

	mu_assert(dt.tm_year == dt2.tm_year, "Different year");
	mu_assert(dt.tm_mon == dt2.tm_mon, "Different mon");
	mu_assert(dt.tm_mday == dt2.tm_mday, "Different mday");
	mu_assert(dt.tm_hour == dt2.tm_hour, "Different hour");
	mu_assert(dt.tm_min == dt2.tm_min, "Different min");
	mu_assert(dt.tm_sec == dt2.tm_sec, "Different sec");


	return NULL;
}

const char *fname = "misc/data/239.0.2.130/241_848.xml";
char *
test_parse()
{
	char *doc = read_file(fname);
	mu_assert(doc, "Error reading %s: %s", fname, strerror(errno));

	list_s *programmes = mtv_parse_file_type_241(doc, "La1");
	mu_assert(programmes != NULL, "Error getting list of programmes");

	struct epg *epg = epg_alloc();
	list_destroy(epg->programmes);
	epg->programmes = programmes;
	list_bubble_sort(epg->programmes, epg_programme_compare_by_date);

	struct epg_channel *un_chan = epg_channel_alloc();
	un_chan->id = "1";
	un_chan->display_name = "RTVE - La1";
	un_chan->short_name = "La1";
	epg_add_channel(epg, un_chan);
	trace("Generating epg for %zu channels and %zu programmes",
		list_count(epg->channels), list_count(epg->programmes));

	char *s = epg_to_xmltv(epg);
	trace("XMLTV:\n%s\n", s);

	return NULL;
}

char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_parse);
	mu_run_test(test_parse_datetime);

	return NULL;
}

RUN_TESTS(all_tests)
