#ifndef __mtv_conf_h__
#define __mtv_conf_h__

#include <mtv/xmltv.h>

struct _mtv_conf_s {
	// Excution configuration
        short            n_days_to_get;
        char            *output_path;
        char            *images_path;
        char            *tmp_path;
        mtv_m3u_format_e m3u;

	// json conf
	int              demarcation;
	char            *tvpackages;
	char            *mcast_grp_start;
	int              mcast_port;
	int              gmt_offset;
};
typedef struct _mtv_conf_s mtv_conf_s;

#define mtv_conf_debug(cnf) \
	debug("days: %d paths output: %s images: %s tmp: %s m3u format: %s" \
	      "demarcation: %d, mcast_grp_start: %s mcast_port: %d, gmt_offset: %dtvpackages: %s", \
	      cnf-_>n_days_to_get, \
	      cnf->output_path, cnf->images_path, cnf->tmp_path, \
	      cnf->m3u == mtv_m3u_format_tvheaded ? "TVHeaded" : "SimpleTV", \
	      cnf->demarcation, cnf->mcast_grp_start, cnf->mcast_port, \
	      cnf->gmt_offset, cnf->tvpackages);


void mtv_conf_destroy(mtv_conf_s *cnf);
mtv_conf_s *mtv_conf_load();

#endif
