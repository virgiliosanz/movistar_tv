#ifndef __mtv_h__
#define __mtv_h__

#include <stdbool.h>
#include "core/list.h"
#include "core/xmltv.h"
#include "bstrlib/bstrlib.h"

struct _conf_s {
	int demarcation;
	struct bstrList *tvpackages;
	bstring mcast_grp_start;
	int mcast_port;
	int gmt_offset;
};
typedef struct _conf_s conf_s;

void conf_destroy(conf_s * cnf);
conf_s *conf_load();

#endif
