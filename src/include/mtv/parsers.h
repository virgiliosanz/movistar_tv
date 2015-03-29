#ifndef __parsers_h__
#define __parsers_h__

#include <mtv/all.h>

/*
 * Converts from string in format: YYYY-MM-DDTHH:MM:SS.000Z
 * to struct tm
 */
void mtv_parse_datetime(struct tm *dt, const char *date_time);

/*
 * Get additional information for a program from url:
 *
 * "http://www-60.svc.imagenio.telefonica.net:2001/appserver/mvtv.do?action=getEpgInfo&extInfoID=%s&tvWholesaler=1";
 */
void mtv_programme_info(struct epg_programme *prog, char *crid);

/*
 * Parse file type 1 from multicast.
 *
 * It looks for demarcation and returns a mcast socket
 */
struct mcast *mtv_parse_file_type_1(const char *xml, const int demarcation);

/*
 * Parse a TVAMain XML document (type 241) & return the list of programmes
 */
list_s *mtv_parse_file_type_241(const char *xml, const char *channel_id);

/*
 * Parse a BroadcastDiscovery XML document (type 1) & return the list of channels
 */
list_s *mtv_parse_file_type_2(const char *xml);


#endif

