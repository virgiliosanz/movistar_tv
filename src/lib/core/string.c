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
}

