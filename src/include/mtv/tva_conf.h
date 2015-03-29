#ifndef __tva_conf_h__
#define __tva_conf_h__

#include <mtv/epg.h>

struct tva_conf {
	int   demarcation;
	char *tvpackages;
	char *mcast_grp_start;
	int   mcast_port;
	int   gmt_offset;
};

#define tva_conf_debug(cnf) \
	trace("demarcation: %d, mcast_grp_start: %s mcast_port: %d, " \
	      "gmt_offset: %d tvpackages: %s", \
	      cnf->demarcation, cnf->mcast_grp_start, cnf->mcast_port, \
	      cnf->gmt_offset, cnf->tvpackages);


void             tva_conf_destroy(struct tva_conf *cnf);
struct tva_conf *tva_conf_load();

#endif
