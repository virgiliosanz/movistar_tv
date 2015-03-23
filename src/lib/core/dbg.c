#include <core/dbg.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

//debug_level_e __global_debug_level = debug_level_error;
debug_level_e __global_debug_level = debug_level_trace;

char *
level_to_str(debug_level_e level)
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
