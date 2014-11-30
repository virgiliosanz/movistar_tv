#ifndef __mtv_string_h__
#define __mtv_string_h__

size_t str_split(const char *str, const char delim, char ***tokens);
#define str_free_tokens(tokens, n) \
	for(size_t i = 0; i < n; i++) free(tokens[i]); \
	free(tokens);

#endif

