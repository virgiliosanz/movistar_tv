#include <core/dbg.h>

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

