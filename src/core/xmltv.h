#ifndef __xmltv_h__
#define __xmltv_h__

#include <time.h>
#include "bstrlib/bstrlib.h"
#include "core/list.h"

struct _xmltv_channel_s {
	bstring id;
	bstring display_name;
	bstring short_name;
	bstring icon;
	bstring url;
	bstring ip;
	unsigned short port;
	bstring tags;
	unsigned short order;
};
typedef struct _xmltv_channel_s xmltv_channel_s;

#define XMLTV_START_FMT "%Y%m%d%H%M%z"
#define XMLTV_START_FMT_SIZE 18

#define XMLTV_DATE_FMT "%Y%m"
#define XMLTV_DATE_FMT_SIZE 7

struct _xmltv_programme_s {
	struct tm start;
	struct tm date;
	bstring channel;
	bstring title;
	bstring desc;
	bstring director;
	list_s *actors;
	bstring country;
	bstring episode_num;	/* system="xmltv_ns" */
	bstring aspect;
	bstring rating_value;	/* system="MPAA" */
	bstring rating_icon;
	bstring star_rating;
};
typedef struct _xmltv_programme_s xmltv_programme_s;

struct _xmltv_s {
	list_s *channels;
	list_s *programmes;
};
typedef struct _xmltv_s xmltv_s;

xmltv_programme_s *xmltv_programme_alloc();
void xmltv_programme_free(xmltv_programme_s * programme);
void xmltv_programme_list_free(list_s * programmes);

xmltv_channel_s *xmltv_channel_alloc();
void xmltv_channel_free(xmltv_channel_s * channel);
void xmltv_channel_list_free(list_s * channels);

xmltv_s *xmltv_alloc();
void xmltv_free(xmltv_s * xmltv);
void xmltv_add_channel(xmltv_s * xmltv, const xmltv_channel_s * channel);
void xmltv_add_programme(xmltv_s * xmltv, const xmltv_programme_s * programme);

int xmltv_validate(const bstring xml);

bstring xmltv_so_xml(const xmltv_s * xmltv);
bstring xmltv_channel_so_m3u(const xmltv_channel_s * chan);
bstring xmltv_channel_list_so_m3u(const list_s * l);
bstring xmltv_channel_so_m3usimple(const xmltv_channel_s * chan);
bstring xmltv_channel_list_so_m3usimple(const list_s * l);

#endif
