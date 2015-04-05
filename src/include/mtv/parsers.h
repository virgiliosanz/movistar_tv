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
#define EPG_INFO_URL_FMT "http://www-60.svc.imagenio.telefonica.net:2001/appserver/mvtv.do?action=getEpgInfo&extInfoID=%s&tvWholesaler=1"
void mtv_programme_info(struct epg_programme *prog, char *crid);

/*
 * Parse file type 1 from multicast.
 *
 * It looks for demarcation and returns a mcast socket
 */
struct mcast *mtv_parse_file_type_1(const char *xml, const int demarcation);

/*
 * Parse a TVAMain XML document (type 241)
 *
 * params:
 *      xml:        string in type 241 xml format
 *      channel_id: string with channel id for this XML EPG info.
 * Returns: list of epg_programmes
 */
list_s *mtv_parse_file_type_241(const char *xml, const char *channel_id);

/*
 * Parse a BroadcastDiscovery XML document (type 1)
 *
 * Returns: list of epg_channels
 */
list_s *mtv_parse_file_type_2(const char *xml);

/*
 * Parses a ServiceDiscovery XML Doument (type 5)
 *
 * Params:
 *      xml:        string in type 5 xml format
 *      tvpackages: string with user packages (got from configuration - tva_conf)
 *
 *
 * Returns a list of channels Ids and it's logical order (struct mtv_channel_order)
 */
struct mtv_channel_order {
	char    *id;
	unsigned order;
};
list_s *mtv_parse_file_type_5(const char *xml, const char *tvpackages);

#endif

