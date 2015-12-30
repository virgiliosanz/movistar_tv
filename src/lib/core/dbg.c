#include <core/dbg.h>

static enum debug_level __global_debug_level = debug_level_trace;

void
debug_level_set(enum debug_level level)
{
	__global_debug_level = level;
}

enum debug_level
debug_level_get()
{
	return __global_debug_level;
}

bool
debug_level_is_visible(enum debug_level level)
{
	return (__global_debug_level >= level);
}

char *
debug_level_str(enum debug_level level)
{
	switch (level) {
		case debug_level_error:
			return "ERROR";
			break;

		case debug_level_warn:
			return "WARN";
			break;

		case debug_level_trace:
			return "TRACE";
			break;
		default:
			return "UNKNOWN";
	}

	return "";
}

void
activity(const char *fmt, ...)
{
	char buff[20];
	struct tm *gm_now;

	time_t now = time (0);
	gm_now = gmtime(&now);
	strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", gm_now);
	printf("%s - ", buff);

	va_list vargs;
	va_start(vargs, fmt);
	vprintf(fmt, vargs);
	va_end(vargs);

	printf("\n");
}
