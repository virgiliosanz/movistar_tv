#ifndef __xmltv_h__
#define __xmltv_h__

#include <mtv/all.h>

struct _xmltv_channel_s {
	char          *id;
	char          *display_name;
	char          *short_name;
	char          *icon;
	char          *url;
	char          *ip;
	char          *tags;
	unsigned short port;
	unsigned short order;
};
typedef struct _xmltv_channel_s xmltv_channel_s;

#define XMLTV_START_FMT "%Y%m%d%H%M%z"
#define XMLTV_START_FMT_SIZE 18

#define XMLTV_DATE_FMT "%Y%m"
#define XMLTV_DATE_FMT_SIZE 7

struct _xmltv_programme_s {
	struct  tm start;
	struct  tm date;
	char   *channel;
	char   *title;
	char   *desc;
	char   *director;
	list_s *actors;
	char   *country;
	char   *episode_num;	/* system="xmltv_ns" */
	char   *aspect;
	char   *rating_value;	/* system="MPAA" */
	char   *rating_icon;
	char   *star_rating;
};
typedef struct _xmltv_programme_s xmltv_programme_s;

struct _xmltv_s {
	list_s *channels;
	list_s *programmes;
};
typedef struct _xmltv_s xmltv_s;

xmltv_programme_s *xmltv_programme_alloc();
void               xmltv_programme_free(xmltv_programme_s * programme);
void               xmltv_programme_list_free(list_s * programmes);
#define debug_programme(prog) trace("Programme -> tit: '%s', chan: '%s' dir: '%s' country: '%s' start: '%02d/%02d/%04d %02d:%02d:%02d'\n'%s'", \
	(prog)->title, (prog)->channel, \
	(prog)->director, (prog)->country, \
	(prog)->start.tm_mday, (prog)->start.tm_mon + 1, (prog)->start.tm_year + 1900, \
	(prog)->start.tm_hour, (prog)->start.tm_min, (prog)->start.tm_sec, \
	(prog)->desc);

xmltv_channel_s *xmltv_channel_alloc();
void             xmltv_channel_free(xmltv_channel_s * channel);
void             xmltv_channel_list_free(list_s * channels);
#define debug_channel(chan) trace("Channel -> id: '%s', name: '%s', short: '%s' icon: '%s' url: '%s' ip: '%s' port: '%d'", \
	(chan)->id, (chan)->display_name, \
	(chan)->short_name, (chan)->icon, \
	(chan)->url, (chan)->ip, (chan)->port);

xmltv_s *xmltv_alloc();
void     xmltv_free(xmltv_s * xmltv);
void     xmltv_add_channel(xmltv_s * xmltv, xmltv_channel_s * channel);
void     xmltv_add_programme(xmltv_s * xmltv, xmltv_programme_s * programme);
int      xmltv_validate(const char *xml);

enum _mtv_m3u_format_e {
	mtv_m3u_format_simpletv,
	mtv_m3u_format_tvheaded,
	mtv_m3u_format_unknown
};
typedef enum _mtv_m3u_format_e mtv_m3u_format_e;

char *xmltv_to_xml(xmltv_s * xmltv);
char *xmltv_channel_to_m3u(xmltv_channel_s * chan);
char *xmltv_channel_list_to_m3u(list_s * l);
char *xmltv_channel_to_m3usimple(xmltv_channel_s * chan);
char *xmltv_channel_list_to_m3usimple(list_s * l);

#endif
