#include "config.h"

char *
test_sbuf()
{
	sbuf_s *s = sbuf_new();
	mu_assert(s != NULL, "buf es NULL");

	sbuf_printf(s, "%s", "¡¡¡¡Hola!!!! ");
	trace(" =%s= ", sbuf_ptr(s));
	mu_assert(strcmp(sbuf_ptr(s), "¡¡¡¡Hola!!!! ") == 0,
	          "No es igual a ¡¡¡¡Hola!!!!: %s", sbuf_ptr(s));

	sbuf_appendf(s, "%d", 5);
	trace("%s", sbuf_ptr(s));
	mu_assert(strcmp(sbuf_ptr(s), "¡¡¡¡Hola!!!! 5") == 0,
	          "No es igual a ¡¡¡¡Hola!!!! 5 : %s", sbuf_ptr(s));

	sbuf_appendf(s, "Hola %d", 6);
	trace("%s", sbuf_ptr(s));
	mu_assert(strcmp(sbuf_ptr(s), "¡¡¡¡Hola!!!! 5Hola 6") == 0,
	          "No es igual a ¡¡¡¡Hola!!!! 5Hola 6 : %s", sbuf_ptr(s));
	trace("%s - %d", sbuf_ptr(s), sbuf_len(s));
	mu_assert(24 == sbuf_len(s), "Sbuf tiene %d caracteres", sbuf_len(s));

	sbuf_delete(s);
	s = sbuf_new();

	sbuf_printf(s, "Hola %d", 5);
	mu_assert(strcmp(sbuf_ptr(s), "Hola 5") == 0,
	          "No es igual a Hola 5 : %s", sbuf_ptr(s));
	sbuf_appendf(s, "Adios: %s pero que: %d",
	             "Este es un texto muy largo para ver si esto funciona o no estamos locos",
	             167897);
	mu_assert(strcmp(sbuf_ptr(s), "Hola 5Adios: Este es un texto muy largo para ver si esto funciona o no estamos locos pero que: 167897") == 0,
	          "No es igual : %s", sbuf_ptr(s));




	sbuf_delete(s);
	return NULL;
}

char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_sbuf);

	return NULL;
}

RUN_TESTS(all_tests);

