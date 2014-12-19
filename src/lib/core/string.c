#include <core/all.h>


/**
 * http://source-code-share.blogspot.com.es/2014/07/implementation-of-java-stringsplit.html
 *
 * int main()
 * {
 * 	char    months[] = "JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC";
 * 	char **tokens = NULL;
 *
 * 	printf("months=[%s]\n\n", months);
 *
 * 	size_t n = str_split(months, ',', &tokens);
 *
 * 	for (size_t i = 0; i < n; i++) {
 * 		printf("month=[%s]\n", tokens[i]);
 * 		free(tokens[i]);
 * 	}
 * 	printf("\n");
 * 	free(tokens);
 *
 * 	return 0;
 * }
 *
 */
size_t
str_split(const char *str, const char delim, char ***tokens)
{
	int count = 1;
	int token_len = 1;
	int i = 0;
	char *p;
	char *t;

	error_if(str == NULL, error);

	p = (char *)str;
	while (*p != '\0') {
		if (*p == delim)
			count++;
		p++;
	}

	*tokens = (char**) malloc(sizeof(char*) * count);
	if (*tokens == NULL)
		exit(1);

	p = (char *)str;
	while (*p != '\0') {
		if (*p == delim) {
			(*tokens)[i] = (char*) malloc(sizeof(char) * token_len);
			if ((*tokens)[i] == NULL)
				exit(1);

			token_len = 0;
			i++;
		}
		p++;
		token_len++;
	}
	(*tokens)[i] = (char*) malloc(sizeof(char) * token_len);
	if ((*tokens)[i] == NULL)
		exit(1);

	i = 0;
	p = (char *)str;
	t = ((*tokens)[i]);
	while (*p != '\0') {
		if (*p != delim && *p != '\0') {
			*t = *p;
			t++;
		}
		else {
			*t = '\0';
			i++;
			t = ((*tokens)[i]);
		}
		p++;
	}

	return count;

error:
	return 0;
}

char *
strtolower(char *s)
{
	if (NULL == s) return NULL;

	char *p = s;
  	while (*p != '\0') {
    		*p = tolower(*p);
    		p++;
  	}

  	return s;
}

char *
strntolower(char *s, size_t n)
{
	if (NULL == s) return NULL;
  	if (n == 0) return s;

  	char *p = s;
  	while (n > 0 && *p != '\0') {
    		*p = tolower(*p);
    		p++;
    		n--;
  	}

  	return s;
}

char *
strtoupper(char *s)
{
	if (NULL == s) return NULL;

  	char *p = s;
  	while (*p != '\0') {
    		*p = (char) toupper((int) *p);
    		p++;
  	}

  	return s;
}


char *
strntoupper(char *s, size_t n)
{
	if (NULL == s) return NULL;
  	if (n == 0) return s;

  	char *p = s;
  	while (n > 0 && *p != '\0') {
    		*p = (char) toupper((int) *p);
    		p++;
    		n--;
  	}

  	return s;
}



char *
strdup_from_to(const char *from, const char *to)
{
	if (NULL == from) return NULL;
	if (NULL == to) return NULL;

	const char *c;
	sbuf_s *s = NULL;

	s = sbuf_new();
	error_if(NULL == s, error);

	for (c = from; c != to; c++) {
		sbuf_appendchar(s, *c);
	}
	sbuf_appendchar(s, *to);

	char *rc = sbuf_detach(s);
	sbuf_delete(s);

	return rc;

error:
	if (s) free(s);
	return NULL;
}

#ifndef _GNU_SOURCE
char *
strdup(const char *s)
{
	char *d = malloc(strlen(s) + 1);
	if (d == NULL)
		return NULL;

	strcpy(d,s);
	return d;
}
#endif


