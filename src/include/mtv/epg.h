#ifndef __epg_h__
#define __epg_h__

#include <mtv/all.h>

struct _epg_channel_s {
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
typedef struct _epg_channel_s epg_channel_s;

#define EPG_START_FMT "%Y%m%d%H%M%z"
#define EPG_START_FMT_SIZE 18

#define EPG_DATE_FMT "%Y%m"
#define EPG_DATE_FMT_SIZE 7

struct _epg_programme_s {
	struct tm start;
	struct tm date;
	char     *channel;
	char     *title;
	char     *desc;
	char     *director;
	list_s   *actors;
	char     *country;
	char     *episode_num;	/* system="epg_ns" */
	char     *aspect;
	char     *rating_value;	/* system="MPAA" */
	char     *rating_icon;
	char     *star_rating;
};
typedef struct _epg_programme_s epg_programme_s;

struct _epg_s {
	list_s *channels;
	list_s *programmes;
};
typedef struct _epg_s epg_s;

epg_programme_s *epg_programme_alloc();
void             epg_programme_free(epg_programme_s * programme);
void             epg_programme_list_free(list_s * programmes);
int              epg_programme_compare_by_date(const void *l, const void *r);
#define epg_debug_programme(prog) trace("Programme -> tit: '%s', chan: '%s' dir: '%s' country: '%s' start: '%02d/%02d/%04d %02d:%02d:%02d'\n'%s'", \
	(prog)->title, (prog)->channel, \
	(prog)->director, (prog)->country, \
	(prog)->start.tm_mday, (prog)->start.tm_mon + 1, (prog)->start.tm_year + 1900, \
	(prog)->start.tm_hour, (prog)->start.tm_min, (prog)->start.tm_sec, \
	(prog)->desc);

epg_channel_s *epg_channel_alloc();
void           epg_channel_free(epg_channel_s * channel);
void           epg_channel_list_free(list_s * channels);
int            epg_channel_compare_by_order(const void *l, const void *r);
#define epg_debug_channel(chan) trace("Channel -> id: '%s', name: '%s', short: '%s' icon: '%s' url: '%s' ip: '%s' port: '%d'", \
	(chan)->id, (chan)->display_name, \
	(chan)->short_name, (chan)->icon, \
	(chan)->url, (chan)->ip, (chan)->port);

epg_s *epg_alloc();
void   epg_free(epg_s * epg);
void   epg_add_channel(epg_s * epg, epg_channel_s * channel);
void   epg_add_programme(epg_s * epg, epg_programme_s * programme);
int    epg_validate(const char *xml);

enum _epg_m3u_format_e {
	epg_m3u_format_simpletv,
	epg_m3u_format_tvheaded,
	epg_m3u_format_unknown
};
typedef enum _epg_m3u_format_e epg_m3u_format_e;

char *epg_to_xmltv(epg_s *epg);
char *epg_to_m3u(epg_s *epg, epg_m3u_format_e format);

#endif
