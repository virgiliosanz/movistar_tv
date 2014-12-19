#ifndef __mtv_string_h__
#define __mtv_string_h__

size_t str_split(const char *str, const char delim, char ***tokens);
#define str_free_tokens(tokens, n) \
do { \
	for(size_t i = 0; i < n; i++) {if (tokens[i]) free(tokens[i]);} \
	free(tokens); \
} while (0)

char *strtoupper(char *s);
char *strntoupper(char* s, size_t n);
char *strtolower(char *s);
char *strntolower(char* s, size_t n);

char *strdup_from_to(const char *from, const char *to);

#ifndef _GNU_SOURCE
char *strdup(const char *s);
#endif

#endif

