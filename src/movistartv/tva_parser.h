#ifndef __mtv_h__
#define __mtv_h__

#include <time.h>
#include "core/list.h"
#include "bstrlib/bstrlib.h"

list_s *tva_parse(const bstring xml, const bstring channel_id);
void tva_parse_datetime(struct tm *dt, const bstring s);

#endif
