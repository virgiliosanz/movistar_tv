#ifndef __debug_h__
#define __debug_h__
#include <core/all.h>

enum debug_level {
	debug_level_error,
	debug_level_warn,
	debug_level_trace,

	debug_level_all
};

void             debug_level_set(enum debug_level level); // trace if not set
enum debug_level debug_level_get();
char            *debug_level_str(enum debug_level level);
bool             debug_level_is_visible(enum debug_level level);

#define debug(level, M, ...) \
    if (debug_level_is_visible(level)) { \
	    fprintf(stderr, "[%s] %s (%d) - %s - : " M "\n", \
	            debug_level_str(level), \
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

#define UNUSED(x) (void)(x)

#endif

