#ifndef __epg_h__
#define __epg_h__

#include <mtv/all.h>

struct epg_channel {
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

#define EPG_START_FMT "%Y%m%d%H%M%z"
#define EPG_START_FMT_SIZE 18

#define EPG_DATE_FMT "%Y%m"
#define EPG_DATE_FMT_SIZE 7

struct epg_programme {
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

struct epg {
	list_s *channels;
	list_s *programmes;
};

struct epg_programme *epg_programme_alloc();
void                  epg_programme_free(struct epg_programme * programme);
void                  epg_programme_list_free(list_s * programmes);
int                   epg_programme_compare_by_date(const void *l, const void *r);
// TODO: Implement comparison by channel order and datetime
#define epg_programme_sort(programmes) \
	list_merge_sort(programmes, epg_programme_compare_by_date)
#define epg_debug_programme(prog) trace("Programme -> tit: '%s', chan: '%s' dir: '%s' country: '%s' start: '%02d/%02d/%04d %02d:%02d:%02d' desc: '%s'", \
	(prog)->title, (prog)->channel, \
	(prog)->director, (prog)->country, \
	(prog)->start.tm_mday, (prog)->start.tm_mon + 1, (prog)->start.tm_year + 1900, \
	(prog)->start.tm_hour, (prog)->start.tm_min, (prog)->start.tm_sec, \
	(prog)->desc);

struct epg_channel *epg_channel_alloc();
void                epg_channel_free(struct epg_channel * channel);
void                epg_channel_list_free(list_s * channels);
int                 epg_channel_compare_by_order(const void *l, const void *r);
#define epg_channel_sort(channels) \
	list_merge_sort(channels, epg_channel_compare_by_order)
#define epg_debug_channel(chan) trace("Channel -> id: '%s', name: '%s', short: '%s' icon: '%s' url: '%s' ip: '%s' port: '%d'", \
	(chan)->id, (chan)->display_name, \
	(chan)->short_name, (chan)->icon, \
	(chan)->url, (chan)->ip, (chan)->port);

struct epg *epg_alloc();
void        epg_free(struct epg * epg);
void        epg_add_channel(struct epg *epg, struct epg_channel *channel);
void        epg_add_programme(struct epg *epg, struct epg_programme *programme);
int         epg_validate(const char *xml);

enum epg_m3u_format {
	epg_m3u_format_simpletv,
	epg_m3u_format_tvheadend,
	epg_m3u_format_unknown
};

char *epg_to_xmltv(const struct epg *epg);
char *epg_to_m3u(const list_s *channels, enum epg_m3u_format format);

#endif
