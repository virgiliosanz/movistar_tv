#include "config.h"

xmltv_s *xmltv;
list_s *channels;
list_s *programmes;

char *
test_create_channels()
{
	xmltv_channel_s *chan;

	channels = list_create();
	mu_assert(channels != NULL, "Error building channels")

	    chan = xmltv_channel_alloc();
	mu_assert(chan != NULL, "Error building channel")
	    chan->id = bfromcstr("La1");
	chan->display_name = bfromcstr("La 1");
	chan->short_name = bfromcstr("Quitar este y dejar Id");
	chan->icon = bfromcstr("La1.jpg");
	chan->url = bfromcstr("http://www.rtve.es/la1");
	chan->ip = bfromcstr("127.0.0.1");
	chan->port = 5327;
	chan->tags = bfromcstr("entetenimiento, noticias");
	chan->order = 3;
	list_push(channels, chan);

	chan = xmltv_channel_alloc();
	mu_assert(chan != NULL, "Error building channel")
	    chan->id = bfromcstr("La2");
	chan->display_name = bfromcstr("La 2");
	chan->short_name = bfromcstr("Quitar este y dejar Id");
	chan->icon = bfromcstr("La2.jpg");
	chan->url = bfromcstr("http://www.rtve.es/la2");
	chan->ip = bfromcstr("127.0.0.2");
	chan->port = 5327;
	chan->tags = bfromcstr("entetenimiento, noticias");
	chan->order = 2;
	list_push(channels, chan);

	chan = xmltv_channel_alloc();
	mu_assert(chan != NULL, "Error building channel")
	    chan->id = bfromcstr("tmad");
	chan->display_name = bfromcstr("Telemadrid");
	chan->short_name = bfromcstr("Quitar este y dejar Id");
	chan->icon = bfromcstr("telemadrid.jpg");
	chan->url = bfromcstr("http://www.telemadrid.es");
	chan->ip = bfromcstr("127.0.0.3");
	chan->port = 5327;
	chan->tags = bfromcstr("entetenimiento, noticias");
	chan->order = 1;
	list_push(channels, chan);

	mu_assert(2 != channels->count, "Invalid number of channels")

	return NULL;
}

char *
test_create_programmes()
{
	programmes = list_create();
	xmltv_programme_s *prog;

	prog = xmltv_programme_alloc();
	mu_assert(prog != NULL, "Error building Programme")
	    prog->start.tm_year = 2014 - 1900;
	prog->start.tm_mon = 11 - 1;
	prog->start.tm_mday = 10;
	prog->start.tm_hour = 15;
	prog->start.tm_min = 0;
	prog->date.tm_year = 2014 - 1900;
	prog->date.tm_mon = 11 - 1;
	prog->channel = bfromcstr("La1");
	prog->title = bfromcstr("Telediario");
	prog->desc =
	    bfromcstr
	    ("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam pharetra finibus neque vitae euismod. Nulla in nunc sit amet velit maximus hendrerit. Nunc molestie ligula ac eros luctus hendrerit. Praesent ac interdum purus. Nulla nec tortor ante. Aliquam efficitur, tortor ac condimentum finibus, dui urna eleifend ante, vel sagittis massa quam et arcu. Vivamus non efficitur neque. Fusce molestie tempus ante, sit amet pulvinar erat ornare ut. Proin eleifend fringilla tristique. Maecenas bibendum est erat, porttitor blandit ante consectetur ac. Vivamus aliquam laoreet bibendum. Ut vitae sagittis velit. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Vestibulum sollicitudin a elit a dignissim.\n Praesent scelerisque at dolor id semper. Interdum et malesuada fames ac ante ipsum primis in faucibus. In a vehicula ex, eget ultricies urna. Fusce lorem quam, lacinia vel ligula vitae, sagittis commodo tortor. Nullam a nibh non nisl tincidunt pretium. Morbi semper cursus dictum. Proin quis odio ex. Integer eu arcu est. Fusce eu tincidunt sapien. Donec vulputate, ipsum sit amet vestibulum maximus, ante quam laoreet risus, a fermentum dolor magna a purus. Fusce augue ex, tincidunt sit amet erat eget, tempus finibus urna. Sed porttitor orci ac nunc pretium, nec suscipit enim feugiat. Ut velit nisl, laoreet sit amet eros eu, malesuada faucibus tellus. Nulla luctus sed lacus sit amet pharetra.");
	prog->director = bfromcstr("Julio Somoano");
	list_push(prog->actors, bfromcstr("Diego Losada"));
	list_push(prog->actors, bfromcstr("Ana Ibañez"));
	prog->country = bfromcstr("España");
	prog->episode_num = bfromcstr("Matinal");
	prog->aspect = bfromcstr("16:9");
	prog->rating_value = bfromcstr("PG");
	prog->rating_icon =
	    bfromcstr("http://img.irtve.es/css/rtve.commons/rtve.header.footer/i/logoRTVEes.png");
	prog->star_rating = bfromcstr("3/3");
	list_push(programmes, prog);

	prog = xmltv_programme_alloc();
	mu_assert(prog != NULL, "Error building Programme")
	    prog->start.tm_year = 2014 - 1900;
	prog->start.tm_mon = 11 - 1;
	prog->start.tm_mday = 10;
	prog->start.tm_hour = 21;
	prog->start.tm_min = 30;
	prog->date.tm_year = 2014 - 1900;
	prog->date.tm_mon = 11 - 1;
	prog->channel = bfromcstr("La1");
	prog->title = bfromcstr("Telediario");
	prog->desc =
	    bfromcstr
	    ("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam pharetra finibus neque vitae euismod. Nulla in nunc sit amet velit maximus hendrerit. Nunc molestie ligula ac eros luctus hendrerit. Praesent ac interdum purus. Nulla nec tortor ante. Aliquam efficitur, tortor ac condimentum finibus, dui urna eleifend ante, vel sagittis massa quam et arcu. Vivamus non efficitur neque. Fusce molestie tempus ante, sit amet pulvinar erat ornare ut. Proin eleifend fringilla tristique. Maecenas bibendum est erat, porttitor blandit ante consectetur ac. Vivamus aliquam laoreet bibendum. Ut vitae sagittis velit. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Vestibulum sollicitudin a elit a dignissim.\n Praesent scelerisque at dolor id semper. Interdum et malesuada fames ac ante ipsum primis in faucibus. In a vehicula ex, eget ultricies urna. Fusce lorem quam, lacinia vel ligula vitae, sagittis commodo tortor. Nullam a nibh non nisl tincidunt pretium. Morbi semper cursus dictum. Proin quis odio ex. Integer eu arcu est. Fusce eu tincidunt sapien. Donec vulputate, ipsum sit amet vestibulum maximus, ante quam laoreet risus, a fermentum dolor magna a purus. Fusce augue ex, tincidunt sit amet erat eget, tempus finibus urna. Sed porttitor orci ac nunc pretium, nec suscipit enim feugiat. Ut velit nisl, laoreet sit amet eros eu, malesuada faucibus tellus. Nulla luctus sed lacus sit amet pharetra.");
	prog->director = bfromcstr("Julio Somoano");
	list_push(prog->actors, bfromcstr("Arsenio Cañada"));
	list_push(prog->actors, bfromcstr("Marc Martín"));
	prog->country = bfromcstr("España");
	prog->episode_num = bfromcstr("Tarde");
	prog->aspect = bfromcstr("16:9");
	prog->rating_value = bfromcstr("PG");
	prog->rating_icon =
	    bfromcstr("http://img.irtve.es/css/rtve.commons/rtve.header.footer/i/logoRTVEes.png");
	prog->star_rating = bfromcstr("1/3");
	list_push(programmes, prog);

	prog = xmltv_programme_alloc();
	mu_assert(prog != NULL, "Error building Programme")
	    prog->start.tm_year = 2014 - 1900;
	prog->start.tm_mon = 11 - 1;
	prog->start.tm_mday = 10;
	prog->start.tm_hour = 20;
	prog->start.tm_min = 30;
	prog->date.tm_year = 2014 - 1900;
	prog->date.tm_mon = 11 - 1;
	prog->channel = bfromcstr("tmad");
	prog->title = bfromcstr("Telediario de Telemadrid");
	prog->desc =
	    bfromcstr
	    ("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam pharetra finibus neque vitae euismod. Nulla in nunc sit amet velit maximus hendrerit. Nunc molestie ligula ac eros luctus hendrerit. Praesent ac interdum purus. Nulla nec tortor ante. Aliquam efficitur, tortor ac condimentum finibus, dui urna eleifend ante, vel sagittis massa quam et arcu. Vivamus non efficitur neque. Fusce molestie tempus ante, sit amet pulvinar erat ornare ut. Proin eleifend fringilla tristique. Maecenas bibendum est erat, porttitor blandit ante consectetur ac. Vivamus aliquam laoreet bibendum. Ut vitae sagittis velit. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Vestibulum sollicitudin a elit a dignissim.\n Praesent scelerisque at dolor id semper. Interdum et malesuada fames ac ante ipsum primis in faucibus. In a vehicula ex, eget ultricies urna. Fusce lorem quam, lacinia vel ligula vitae, sagittis commodo tortor. Nullam a nibh non nisl tincidunt pretium. Morbi semper cursus dictum. Proin quis odio ex. Integer eu arcu est. Fusce eu tincidunt sapien. Donec vulputate, ipsum sit amet vestibulum maximus, ante quam laoreet risus, a fermentum dolor magna a purus. Fusce augue ex, tincidunt sit amet erat eget, tempus finibus urna. Sed porttitor orci ac nunc pretium, nec suscipit enim feugiat. Ut velit nisl, laoreet sit amet eros eu, malesuada faucibus tellus. Nulla luctus sed lacus sit amet pharetra.");
	prog->director = bfromcstr("Agustín de Grado");
	list_push(prog->actors, bfromcstr("Rocio Delgado"));
	list_push(prog->actors, bfromcstr("Javier Gálvez"));
	prog->country = bfromcstr("España");
	prog->episode_num = bfromcstr("Tarde");
	prog->aspect = bfromcstr("16:9");
	prog->rating_value = bfromcstr("PG");
	prog->rating_icon = bfromcstr("http://www.telemadrid.es/sites/default/files/logo.gif");
	prog->star_rating = bfromcstr("3/3");
	list_push(programmes, prog);

	return NULL;
}

char *
_add_channels()
{
	xmltv_channel_s *chan;
	list_foreach(channels, first, next, cur) {
		chan = (xmltv_channel_s *) cur->value;
		debug("Adding chan: %s (%s)", chan->display_name->data, chan->id->data);
		xmltv_add_channel(xmltv, chan);
	}

	return NULL;
}

char *
_add_programmes()
{
	xmltv_programme_s *prog;
	list_foreach(programmes, first, next, cur) {
		prog = (xmltv_programme_s *) cur->value;
		debug("Adding prog: %s (%s)", prog->title->data, prog->channel->data);
		xmltv_add_programme(xmltv, prog);
	}
	return NULL;
}

char *
test_create_xmltv()
{
	xmltv = xmltv_alloc();
	mu_assert(NULL != xmltv, "Error building xmltv");

	debug("Adding Channels");
	_add_channels();
	debug("Channels added");

	debug("Adding Programmes");
	_add_programmes();
	debug("Programmes added");

	return NULL;
}

char *
test_create_xml_and_validate()
{
	bstring xml;

	debug("Generating xml from struct xmltv");
	xml = xmltv_so_xml(xmltv);

	debug("XML Generated from struct xmltv");
	mu_assert(xml != NULL, "Error creating xml");

	debug("XMLTV:\n%s", xml->data);

	int res = xmltv_validate(xml);
	mu_assert(res == 1, "Invalid XML")

	    return NULL;
}

char *
test_create_m3u()
{
	debug("Generating list m3u");
	bstring s = xmltv_channel_list_so_m3u(channels);
	debug("m3u:\n%s", s->data);
	bdestroy(s);

	return NULL;
}

char *
test_create_m3usimple()
{
	debug("Generating m3usimple");
	bstring b = NULL;
	list_foreach(channels, first, next, cur) {
		b = xmltv_channel_so_m3usimple((xmltv_channel_s *) cur->value);
	}

	return NULL;
}

char *
test_create_list_m3usimple()
{
	debug("Generating list m3usimple");
	bstring s = xmltv_channel_list_so_m3usimple(channels);
	debug("m3u - simpleiptv:\n%s", s->data);
	bdestroy(s);

	return NULL;
}

char *
test_create_list_m3u()
{
	debug("Generating list m3u");
	bstring s = xmltv_channel_list_so_m3u(channels);
	debug("m3u:\n%s", s->data);
	bdestroy(s);

	return NULL;
}

char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_create_channels);
	mu_run_test(test_create_programmes);
	mu_run_test(test_create_xmltv);
	mu_run_test(test_create_xml_and_validate);
	mu_run_test(test_create_m3u);
	mu_run_test(test_create_m3usimple)
	    mu_run_test(test_create_list_m3u);
	mu_run_test(test_create_list_m3usimple);

	return NULL;
}

RUN_TESTS(all_tests)
