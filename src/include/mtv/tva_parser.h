#ifndef __tva_parser_h__
#define __tva_parser_h__

#include <mtv/all.h>

list_s *tva_parse(const char *xml, const char *channel_id);

void tva_parse_datetime(struct tm *dt, const char *s);

#endif
