#ifndef __debug_h__
#define __debug_h__

#ifndef NDEBUG

enum _debug_level_e {
	debug_level_error,
	debug_level_warn,
	debug_level_trace,
	debug_level_all
};
typedef enum _debug_level_e debug_level_e;
const char *debug_levels[] = {"ERROR", "WARN", "TRACE", "ALL"};
debug_level_e __global_debug_level = debug_level_error;

#define debug_set_level(level) do {__global_debug_level = level;} while (0)

#define debug(level, M, ...) \
do { \
	if (__global_debug_level >= level) { \
		fprintf(stderr, "[%s] %s (%d) - %s: " M "\n", \
			debug_levels[level], \
			__FILE__, __LINE__, __func__, ##__VA_ARGS__); \
	} \
} while(0)

#define error(M, ...) debug(debug_level_error, M, ##__VA_ARGS__)
#define warn(M, ...)  debug(debug_level_warn,  M, ##__VA_ARGS__)
#define trace(M, ...) debug(debug_level_trace, M, ##__VA_ARGS__)

#define ok(A) \
do {\
	if(!(A)) { \
		error("Check (%s) Failed!", #A); \
	} \
} while (0)

#define ok_or_goto(A, label) \
do {\
	if(!(A)) { \
		error("Check (%s) failed!", #A); \
		goto label; \
	} \
} while (0)

#else

#define debug_set_level(level)
#define debug(level, M, ...)
#define error(M, ...)
#define warn(M, ...)
#define trace(M, ...)
#define ok(A)
#define ok_or_goto(A, label)

#endif

#endif

