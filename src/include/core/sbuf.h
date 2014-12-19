#ifndef __sbuf_h__
#define __sbuf_h__

#include <core/all.h>

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
    #define GNUC_PRINTF_CHECK(fmt_idx, arg_idx) __attribute__((format (printf, fmt_idx, arg_idx)))
#else   /* !__GNUC__ */
    #define GNUC_PRINTF_CHECK(fmt_idx, arg_idx)
#endif  /* !__GNUC__ */

struct _sbuf_s {
	char *buf;
	int NUL;
	size_t buflen;
};

typedef struct _sbuf_s sbuf_s;

sbuf_s *sbuf_new(void);         // create an mtv_sbuf
void sbuf_delete(sbuf_s *sb);   // free an mtv_sbuf
void sbuf_reset(sbuf_s *sb);    // clear mtv_sbuf contents (doesn't free mem)
int sbuf_len(sbuf_s *sb);       // return contents length (excluding NUL)
char *sbuf_ptr(sbuf_s *sb);     // return pointer to mtv_sbuf contents
char *sbuf_detach(sbuf_s *sb);  // Detach and return mtv_sbuf contents (you must free)
void sbuf_truncate(sbuf_s *sb, int len);
void sbuf_move(sbuf_s *src, sbuf_s *dest);
void sbuf_appendstr(sbuf_s *sb, const char *string);
void sbuf_appendbytes(sbuf_s *sb, const char *str, int len);
void sbuf_appendchar(sbuf_s *sb, char c);
void sbuf_printf(sbuf_s *sb, const char *fmt, ...) GNUC_PRINTF_CHECK(2,3);
void sbuf_appendf(sbuf_s *sb, const char *fmt, ...) GNUC_PRINTF_CHECK(2,3);

#endif

