#ifndef __debug_h__
#define __debug_h__

enum _debug_level_e {
	debug_level_error,
	debug_level_warn,
	debug_level_trace,
	debug_level_all
};
typedef enum _debug_level_e debug_level_e;
debug_level_e __global_debug_level;

char *level_to_str(debug_level_e level);
#define debug_set_level(level) do {__global_debug_level = level;} while (0)

#define debug(level, M, ...) \
if (__global_debug_level >= level) { \
	fprintf(stderr, "[%s] %s (%d) - %s - : " M "\n", \
	        level_to_str(level), \
		__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
} \

#define error(M, ...) debug(debug_level_error, M, ##__VA_ARGS__)
#define warn(M, ...)  debug(debug_level_warn,  M, ##__VA_ARGS__)
#define trace(M, ...) debug(debug_level_trace, M, ##__VA_ARGS__)

#define error_if(A, label, M, ...) \
if (A) { \
	fprintf(stderr, "[ERROR] %s (%d) - %s - : " M " - " #A, \
			__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
	fprintf(stderr, "\n"); \
	goto label; \
} \

#define warn_if(A, M, ...) \
if (A) { \
	warn("assert failed: " #A " - " M , ##__VA_ARGS__); \
} \

#define trace_if(A, M, ...) \
if (A) { \
	trace("assert failed: " #A " - " M , ##__VA_ARGS__); \
} \

void activity(const char *fmt, ...);

#endif

