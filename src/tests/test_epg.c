#include "config.h"

struct epg *epg;
list_s *channels;
list_s *programmes;

char *
test_create_channels()
{
	struct epg_channel *chan;

	channels = list_create();
	mu_assert(channels != NULL, "Error building channels")

	chan = epg_channel_alloc();
	mu_assert(chan != NULL, "Error building channel")
	chan->id = strdup("La1");
	chan->display_name = strdup("La 1");
	chan->short_name = strdup("Quitar este y dejar Id");
	chan->icon = strdup("La1.jpg");
	chan->url = strdup("http://www.rtve.es/la1");
	chan->ip = strdup("127.0.0.1");
	chan->port = 5327;
	chan->tags = strdup("entetenimiento, noticias");
	chan->order = 3;
	list_push(channels, chan);

	chan = epg_channel_alloc();
	mu_assert(chan != NULL, "Error building channel")
	chan->id = strdup("La2");
	chan->display_name = strdup("La 2");
	chan->short_name = strdup("Quitar este y dejar Id");
	chan->icon = strdup("La2.jpg");
	chan->url = strdup("http://www.rtve.es/la2");
	chan->ip = strdup("127.0.0.2");
	chan->port = 5327;
	chan->tags = strdup("entetenimiento, noticias");
	chan->order = 2;
	list_push(channels, chan);

	chan = epg_channel_alloc();
	mu_assert(chan != NULL, "Error building channel")
	chan->id = strdup("tmad");
	chan->display_name = strdup("Telemadrid");
	chan->short_name = strdup("Quitar este y dejar Id");
	chan->icon = strdup("telemadrid.jpg");
	chan->url = strdup("http://www.telemadrid.es");
	chan->ip = strdup("127.0.0.3");
	chan->port = 5327;
	chan->tags = strdup("entetenimiento, noticias");
	chan->order = 1;
	list_push(channels, chan);

	mu_assert(2 != channels->count, "Invalid number of channels");

	epg_channel_sort(channels);


	return NULL;
}

char *
test_create_programmes()
{
	programmes = list_create();
	struct epg_programme *prog;

	prog = epg_programme_alloc();
	mu_assert(prog != NULL, "Error building Programme")
	prog->start.tm_year = 2014 - 1900;
	prog->start.tm_mon = 11 - 1;
	prog->start.tm_mday = 10;
	prog->start.tm_hour = 15;
	prog->start.tm_min = 0;
	prog->date.tm_year = 2014 - 1900;
	prog->date.tm_mon = 11 - 1;
	prog->channel = strdup("La1");
	prog->title = strdup("Telediario");
	prog->desc = strdup("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam pharetra finibus neque vitae euismod. Nulla in nunc sit amet velit maximus hendrerit. Nunc molestie ligula ac eros luctus hendrerit. Praesent ac interdum purus. Nulla nec tortor ante. Aliquam efficitur, tortor ac condimentum finibus, dui urna eleifend ante, vel sagittis massa quam et arcu. Vivamus non efficitur neque. Fusce molestie tempus ante, sit amet pulvinar erat ornare ut. Proin eleifend fringilla tristique. Maecenas bibendum est erat, porttitor blandit ante consectetur ac. Vivamus aliquam laoreet bibendum. Ut vitae sagittis velit. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Vestibulum sollicitudin a elit a dignissim.\n Praesent scelerisque at dolor id semper. Interdum et malesuada fames ac ante ipsum primis in faucibus. In a vehicula ex, eget ultricies urna. Fusce lorem quam, lacinia vel ligula vitae, sagittis commodo tortor. Nullam a nibh non nisl tincidunt pretium. Morbi semper cursus dictum. Proin quis odio ex. Integer eu arcu est. Fusce eu tincidunt sapien. Donec vulputate, ipsum sit amet vestibulum maximus, ante quam laoreet risus, a fermentum dolor magna a purus. Fusce augue ex, tincidunt sit amet erat eget, tempus finibus urna. Sed porttitor orci ac nunc pretium, nec suscipit enim feugiat. Ut velit nisl, laoreet sit amet eros eu, malesuada faucibus tellus. Nulla luctus sed lacus sit amet pharetra.");
	prog->director = strdup("Julio Somoano");
	list_push(prog->actors, strdup("Diego Losada"));
	list_push(prog->actors, strdup("Ana Ibañez"));
	prog->country = strdup("España");
	prog->episode_num = strdup("Matinal");
	prog->aspect = strdup("16:9");
	prog->rating_value = strdup("PG");
	prog->rating_icon = strdup("http://img.irtve.es/css/rtve.commons/rtve.header.footer/i/logoRTVEes.png");
	prog->star_rating = strdup("3/3");
	list_push(programmes, prog);

	prog = epg_programme_alloc();
	mu_assert(prog != NULL, "Error building Programme")
	    prog->start.tm_year = 2014 - 1900;
	prog->start.tm_mon = 11 - 1;
	prog->start.tm_mday = 10;
	prog->start.tm_hour = 21;
	prog->start.tm_min = 30;
	prog->date.tm_year = 2014 - 1900;
	prog->date.tm_mon = 11 - 1;
	prog->channel = strdup("La1");
	prog->title = strdup("Telediario");
	prog->desc = strdup("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam pharetra finibus neque vitae euismod. Nulla in nunc sit amet velit maximus hendrerit. Nunc molestie ligula ac eros luctus hendrerit. Praesent ac interdum purus. Nulla nec tortor ante. Aliquam efficitur, tortor ac condimentum finibus, dui urna eleifend ante, vel sagittis massa quam et arcu. Vivamus non efficitur neque. Fusce molestie tempus ante, sit amet pulvinar erat ornare ut. Proin eleifend fringilla tristique. Maecenas bibendum est erat, porttitor blandit ante consectetur ac. Vivamus aliquam laoreet bibendum. Ut vitae sagittis velit. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Vestibulum sollicitudin a elit a dignissim.\n Praesent scelerisque at dolor id semper. Interdum et malesuada fames ac ante ipsum primis in faucibus. In a vehicula ex, eget ultricies urna. Fusce lorem quam, lacinia vel ligula vitae, sagittis commodo tortor. Nullam a nibh non nisl tincidunt pretium. Morbi semper cursus dictum. Proin quis odio ex. Integer eu arcu est. Fusce eu tincidunt sapien. Donec vulputate, ipsum sit amet vestibulum maximus, ante quam laoreet risus, a fermentum dolor magna a purus. Fusce augue ex, tincidunt sit amet erat eget, tempus finibus urna. Sed porttitor orci ac nunc pretium, nec suscipit enim feugiat. Ut velit nisl, laoreet sit amet eros eu, malesuada faucibus tellus. Nulla luctus sed lacus sit amet pharetra.");
	prog->director = strdup("Julio Somoano");
	list_push(prog->actors, strdup("Arsenio Cañada"));
	list_push(prog->actors, strdup("Marc Martín"));
	prog->country = strdup("España");
	prog->episode_num = strdup("Tarde");
	prog->aspect = strdup("16:9");
	prog->rating_value = strdup("PG");
	prog->rating_icon = strdup("http://img.irtve.es/css/rtve.commons/rtve.header.footer/i/logoRTVEes.png");
	prog->star_rating = strdup("1/3");
	list_push(programmes, prog);

	prog = epg_programme_alloc();
	mu_assert(prog != NULL, "Error building Programme")
	prog->start.tm_year = 2014 - 1900;
	prog->start.tm_mon = 11 - 1;
	prog->start.tm_mday = 10;
	prog->start.tm_hour = 20;
	prog->start.tm_min = 30;
	prog->date.tm_year = 2014 - 1900;
	prog->date.tm_mon = 11 - 1;
	prog->channel = strdup("tmad");
	prog->title = strdup("Telediario de Telemadrid");
	prog->desc = strdup("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam pharetra finibus neque vitae euismod. Nulla in nunc sit amet velit maximus hendrerit. Nunc molestie ligula ac eros luctus hendrerit. Praesent ac interdum purus. Nulla nec tortor ante. Aliquam efficitur, tortor ac condimentum finibus, dui urna eleifend ante, vel sagittis massa quam et arcu. Vivamus non efficitur neque. Fusce molestie tempus ante, sit amet pulvinar erat ornare ut. Proin eleifend fringilla tristique. Maecenas bibendum est erat, porttitor blandit ante consectetur ac. Vivamus aliquam laoreet bibendum. Ut vitae sagittis velit. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Vestibulum sollicitudin a elit a dignissim.\n Praesent scelerisque at dolor id semper. Interdum et malesuada fames ac ante ipsum primis in faucibus. In a vehicula ex, eget ultricies urna. Fusce lorem quam, lacinia vel ligula vitae, sagittis commodo tortor. Nullam a nibh non nisl tincidunt pretium. Morbi semper cursus dictum. Proin quis odio ex. Integer eu arcu est. Fusce eu tincidunt sapien. Donec vulputate, ipsum sit amet vestibulum maximus, ante quam laoreet risus, a fermentum dolor magna a purus. Fusce augue ex, tincidunt sit amet erat eget, tempus finibus urna. Sed porttitor orci ac nunc pretium, nec suscipit enim feugiat. Ut velit nisl, laoreet sit amet eros eu, malesuada faucibus tellus. Nulla luctus sed lacus sit amet pharetra.");
	prog->director = strdup("Agustín de Grado");
	list_push(prog->actors, strdup("Rocio Delgado"));
	list_push(prog->actors, strdup("Javier Gálvez"));
	prog->country = strdup("España");
	prog->episode_num = strdup("Tarde");
	prog->aspect = strdup("16:9");
	prog->rating_value = strdup("PG");
	prog->rating_icon = strdup("http://www.telemadrid.es/sites/default/files/logo.gif");
	prog->star_rating = strdup("3/3");
	list_push(programmes, prog);


	epg_programme_sort(programmes);

	return NULL;
}

char *
_add_channels()
{
	struct epg_channel *chan;
	list_foreach(channels, first, next, cur) {
		chan = (struct epg_channel *) cur->value;
		trace("Adding chan: %s (%s)", chan->display_name, chan->id);
		epg_add_channel(epg, chan);
	}

	return NULL;
}

char *
_add_programmes()
{
	struct epg_programme *prog;
	list_foreach(programmes, first, next, cur) {
		prog = (struct epg_programme *) cur->value;
		trace("Adding prog: %s (%s)", prog->title, prog->channel);
		epg_add_programme(epg, prog);
	}
	return NULL;
}

char *
test_create_epg()
{
	epg = epg_alloc();
	mu_assert(NULL != epg, "Error building epg");

	trace("Adding Channels");
	_add_channels();
	trace("Channels added");

	trace("Adding Programmes");
	_add_programmes();
	trace("Programmes added");

	return NULL;
}

char *
test_create_xml_and_validate()
{
	char *xml;

	trace("Generating xml from struct epg");
	xml = epg_to_xmltv(epg);

	trace("XML Generated from struct epg");
	mu_assert(xml != NULL, "Error creating xml");

	trace("XMLTV:\n%s", xml);
/*
	int res = epg_validate(xml);
	mu_assert(res == 1, "Invalid XML")
*/
	trace("No validating against DTD");
	return NULL;
}

char *
test_create_m3us()
{
	char *m3u;

	m3u = epg_to_m3u(epg->channels, epg_m3u_format_simpletv);
	mu_assert(m3u != NULL, "Error generating m3u simple");
	trace("SimpleTV M3U: %s", m3u);
	free(m3u);

	m3u = epg_to_m3u(epg->channels, epg_m3u_format_tvheadend);
	mu_assert(m3u != NULL, "Error generating m3u tvheadend");
	trace("TVHeaded M3U: %s", m3u);
	free(m3u);

	return NULL;
}

char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_create_channels);
	mu_run_test(test_create_programmes);
	mu_run_test(test_create_epg);
	mu_run_test(test_create_xml_and_validate);
	mu_run_test(test_create_m3us);


	// TODO: Preparar el test para xmltv real y probar la generación de m3u

	return NULL;
}

RUN_TESTS(all_tests)
