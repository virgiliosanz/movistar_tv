#include "config.h"

static list_s *list = NULL;
char *test1 = "test1 data";
char *test2 = "test2 data";
char *test3 = "test3 data";
char *test4 = "test4 data";

list_s *
_create_word_list(char *values[])
{
	list_s *words = list_create();

	for (size_t i = 0; values[i]; i++) {
		list_push(words, values[i]);
	}

	return words;

}

void
_trace_list(list_s *l)
{
	sbuf_s *s = sbuf_alloc();

	char *v;
	list_foreach(l, first, next, cur) {
		v = (char *)cur->value;
		sbuf_appendf(s, "'%s', ", v);

	}
	trace("%s", sbuf_ptr(s));
	sbuf_free(s);
}

char *
test_create()
{
	list = list_create();
	mu_assert(list != NULL, "Failed to create list.");

	return NULL;
}

char *
test_destroy()
{
	list_clear_destroy(list);

	return NULL;

}


char *
test_push_pop()
{
	list_push(list, test1);
	mu_assert(list_last(list) == test1, "Wrong last value.");

	list_push(list, test2);
	mu_assert(list_last(list) == test2, "Wrong last value");

	list_push(list, test3);
	mu_assert(list_last(list) == test3, "Wrong last value.");
	mu_assert(list_count(list) == 3, "Wrong count on push.");

	char *val = list_pop(list);
	mu_assert(strcmp(val, test3) == 0 , "Wrong value on pop.");

	val = list_pop(list);
	mu_assert(strcmp(val, test2) == 0, "Wrong value on pop.");

	val = list_pop(list);
	mu_assert(strcmp(val, test1) == 0, "Wrong value on pop.");
	mu_assert(list_count(list) == 0, "Wrong count after pop.");

	return NULL;
}

char *
test_unshift()
{
	list_unshift(list, test1);
	mu_assert(list_first(list) == test1, "Wrong first value.");

	list_unshift(list, test2);
	mu_assert(list_first(list) == test2, "Wrong first value");

	list_unshift(list, test3);
	mu_assert(list_first(list) == test3, "Wrong last value.");
	mu_assert(list_count(list) == 3, "Wrong count on unshift.");

	return NULL;
}

char *
test_remove()
{
	// we only need to test the middle remove case since push/shift
	// already tests the other cases

	_trace_list(list);
	char *val = list_remove(list, list->first->next);
	mu_assert(val == test2, "Wrong removed element.");
	mu_assert(list_count(list) == 2, "Wrong count after remove.");
	mu_assert(list_first(list) == test3, "Wrong first after remove.");
	mu_assert(list_last(list) == test1, "Wrong last after remove.");
	_trace_list(list);

	return NULL;
}

char *
test_shift()
{
	mu_assert(list_count(list) != 0, "Wrong count before shift.");

	char *val = list_shift(list);
	mu_assert(val == test3, "Wrong value on shift.");

	val = list_shift(list);
	mu_assert(val == test1, "Wrong value on shift.");
	mu_assert(list_count(list) == 0, "Wrong count after shift.");

	return NULL;
}

char *
test_foreach()
{
	list_s *l;
	l = list_create();
	mu_assert(l != NULL, "Cannot create list")

	list_push(l, "Uno");
	list_push(l, "Dos");
	list_push(l, "Tres");
	list_push(l, "Cuatro");

	size_t i = 0;
	list_foreach(l, first, next, cur) {
		printf("Valor: %s\n", (char *)cur->value);
		i++;
	}

	trace("%zu == %zu", i, list_count(l));
	mu_assert(i == l->count, "Incorrect length");

	return NULL;
}


list_s *
create_words()
{
	char *values[] = { "XXXX", "1234", "abcd", "xjvef", "NDSS", NULL };
	return _create_word_list(values);
}


list_s *
create_words2()
{

	char *values[] = { "zSzS", "4321", "dcba", "xjvef", NULL };
	return _create_word_list(values);
}

int
is_sorted(list_s *words)
{
	list_foreach(words, first, next, cur) {
		if (cur->next && strcmp(cur->value, cur->next->value) > 0) {
			trace("%s %s", (char *)cur->value, (char *)cur->next->value);
			return 0;
		}
	}

	return 1;
}

char *
test_bubble_sort()
{
	list_s *words = create_words();

	// should work on a list that needs sorting
	int rc = list_bubble_sort(words, (list_compare) strcmp);
	mu_assert(rc == 0, "bubble sort failed.");
	mu_assert(is_sorted(words), "words are not sorted after bubble sort.");

	// should work on an already sorted list
	rc = list_bubble_sort(words, (list_compare) strcmp);
	mu_assert(rc == 0, "bubble sort of already sorted failed.");
	mu_assert(is_sorted(words), "words should be sort if already bubble sorted.");

	list_destroy(words);

	// should work on an empty list
	words = list_create(words);
	rc = list_bubble_sort(words, (list_compare) strcmp);
	mu_assert(rc == 0, "bubble sort failed on empty list.");
	mu_assert(is_sorted(words), "words should be sorted if empty.");

	list_destroy(words);

	return NULL;
}

char *
test_merge_sort()
{
	list_s *words = create_words();

	// should work on a list that needs sorting
	list_s *res = list_merge_sort(words, (list_compare) strcmp);
	mu_assert(is_sorted(res), "words are not sorted after merge sort.");

	list_s *res2 = list_merge_sort(res, (list_compare) strcmp);
	mu_assert(is_sorted(res), "should still be sorted after merge sort.");
	list_destroy(res2);
	list_destroy(res);

	list_destroy(words);
	return NULL;
}

char *
test_concat()
{
	list_s *l1 = create_words();
	trace("List 1")
	_trace_list(l1);

	list_s *l2 = create_words2();
	trace("List 2")
	_trace_list(l2);

	size_t total = list_count(l1) + list_count(l2);
	list_s *l3 = list_concat(l1, l2);
	trace("List 3 = list 1 + list 2")
	_trace_list(l3);

	mu_assert(list_count(l3) == total, "List 3 has l1 + l2 elements");
	mu_assert(list_count(l3) == list_count(l1), "l3 and l2 are same list");

	size_t cnt = 0;
	list_foreach(l3, first, next, cur) {
		UNUSED(cur);
		cnt ++;
	}
	mu_assert(total == cnt, "total should be same as counting elements");

	l2 = list_create();
	l3 = list_concat(l1, l2);
	mu_assert(list_count(l3) == list_count(l1), "l3 and l2 are same list");
	mu_assert(list_count(l3) == total, "List 3 has l1 + l2 elements");

	l3 = list_concat(l2, l1);
	mu_assert(list_count(l3) == list_count(l1), "l3 and l2 are same list");
	mu_assert(list_count(l3) == total, "List 3 has l1 + l2 elements");

	l3 = list_merge_sort(l3, (list_compare) strcasecmp);
	trace("Sorted l3");
	_trace_list(l3);

	return NULL;
}

char *
test_search()
{
	list_s *l = create_words();
	//char *values[] = { "XXXX", "1234", "abcd", "xjvef", "NDSS", NULL };
	list_node_s *found;

	found = list_search(l, "Not found", (list_compare)strcmp);
	mu_assert(found == NULL, "Found a String not in list: %s", (char *)found->value);

	found = list_search(l, "1234", (list_compare)strcmp);
	mu_assert(found != NULL, "Not Found a String in list: %s", (char *)found->value);


	return NULL;
}

char *
all_tests()
{
	mu_suite_start();

	mu_run_test(test_create);
	mu_run_test(test_push_pop);
	mu_run_test(test_unshift);
	mu_run_test(test_remove);
	mu_run_test(test_bubble_sort);
	mu_run_test(test_merge_sort);
	mu_run_test(test_search);
	mu_run_test(test_shift);
	mu_run_test(test_destroy);
	mu_run_test(test_foreach);
	mu_run_test(test_concat);


// TODO: Test list_walk*

	return NULL;
}

RUN_TESTS(all_tests)
