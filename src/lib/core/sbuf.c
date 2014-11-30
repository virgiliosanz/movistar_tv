#include <core/all.h>

static void
sbuf_init(sbuf_s *sb)
{
	sb->buf    = NULL;
	sb->NUL    = 0;
	sb->buflen = 0;
}

sbuf_s *
sbuf_new()
{
	sbuf_s *sb = malloc(sizeof(*sb));
	check_mem(sb);
	sbuf_init(sb);

	return sb;

error:
	return NULL;
}

void
sbuf_delete(sbuf_s *sb)
{
	free(sb->buf);
	free(sb);
}

void
sbuf_reset(sbuf_s *sb)
{
	sb->NUL = 0;
}

char *
sbuf_ptr(sbuf_s *sb)
{
	return sb->NUL ? sb->buf : "";
}

int
sbuf_len(sbuf_s *sb)
{
	return sb->NUL;
}

char *
sbuf_detach(sbuf_s *sb)
{
	char* buf = sb->buf;
	if (!buf)
		buf = strdup("");

	sbuf_init(sb);
	return buf;
}

void
sbuf_move(sbuf_s *src, sbuf_s *dest)
{
	free(dest->buf);

	dest->buf    = src->buf;
	dest->NUL    = src->NUL;
	dest->buflen = src->buflen;

	sbuf_init(src);
}

void
sbuf_sruncate(sbuf_s *sb, int len)
{
	if (len < 0 || len > sb->NUL)
		return;
	sb->NUL = len;
	if (sb->buf)
		sb->buf[sb->NUL] = '\0';
}

/* Extend the buffer in sb by at least len bytes.
 * Note len should include the space required for the NUL terminator */
static void
sbuf_extendby(sbuf_s *sb, size_t len)
{
	len += sb->NUL;

	if (len <= sb->buflen)
		return;

	if (!sb->buflen)
		sb->buflen = 64; //min size (to alleviate fragmentation)

	while (len > sb->buflen)
		sb->buflen *= 2;

	char* buf = realloc(sb->buf, sb->buflen);
	if (!buf) abort(); //out of mem
	sb->buf = buf;
}

void
sbuf_appendbytes(sbuf_s *sb, const char *str, int len)
{
	sbuf_extendby(sb, len + 1);
	memcpy(&sb->buf[sb->NUL], str, len);
	sb->NUL += len;
	sb->buf[sb->NUL] = '\0';
}

void
sbuf_appendchar(sbuf_s *sb, char c)
{
	sbuf_extendby(sb, 2);
	sb->buf[sb->NUL++] = c;
	sb->buf[sb->NUL] = '\0';
}

void
sbuf_appendstr(sbuf_s *sb, const char *str)
{
	sbuf_appendbytes(sb, str, strlen(str));
}

static void
sbuf_vappendf(sbuf_s *sb, const char *fmt, const va_list ap)
{
	size_t num_required;
	while ((num_required = vsnprintf(sb->buf    + sb->NUL,
					 sb->buflen - sb->NUL,
					 fmt,
					 ap)) >= (sb->buflen - sb->NUL)) {
		sbuf_extendby(sb, num_required + 1);
	}

	sb->NUL += num_required;
}

void
sbuf_printf(sbuf_s *sb, const char *fmt, ...)
{
	sbuf_reset(sb);

	va_list ap;
	va_start(ap, fmt);
	sbuf_vappendf(sb, fmt, ap);
	va_end(ap);
}

void
sbuf_appendf(sbuf_s *sb, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	sbuf_vappendf(sb, fmt, ap);
	va_end(ap);
}


