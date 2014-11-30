#include  <core/all.h>

list_s *
list_create()
{
	return calloc(1, sizeof(list_s));
}

void
list_destroy(list_s *list)
{
	list_foreach(list, first, next, cur) {
		if (cur->prev) {
			free(cur->prev);
		}
	}

	free(list->last);
	free(list);
}

void
list_clear(list_s *list)
{
	list_foreach(list, first, next, cur) {
		free(cur->value);
	}
}

void
list_clear_destroy(list_s *list)
{
	list_clear(list);
	list_destroy(list);
}

void
list_push(list_s *list, void *value)
{
	list_node_s *node = calloc(1, sizeof(list_node_s));
	check_mem(node);

	node->value = value;

	if (list->last == NULL) {
		list->first = node;
		list->last = node;
	} else {
		list->last->next = node;
		node->prev = list->last;
		list->last = node;
	}

	list->count++;

 error:
	return;
}

void *
list_pop(list_s *list)
{
	list_node_s *node = list->last;
	return node != NULL ? list_remove(list, node) : NULL;
}

void
list_unshift(list_s *list, void *value)
{
	list_node_s *node = calloc(1, sizeof(list_node_s));
	check_mem(node);

	node->value = value;

	if (list->first == NULL) {
		list->first = node;
		list->last = node;
	} else {
		node->next = list->first;
		list->first->prev = node;
		list->first = node;
	}

	list->count++;

 error:
	return;
}

void *
list_shift(list_s *list)
{
	list_node_s *node = list->first;
	return node != NULL ? list_remove(list, node) : NULL;
}

void *
list_remove(list_s *list, list_node_s *node)
{
	void *result = NULL;

	check(list->first && list->last, "list is empty.");
	check(node, "node can't be NULL");

	if (node == list->first && node == list->last) {
		list->first = NULL;
		list->last = NULL;
	} else if (node == list->first) {
		list->first = node->next;
		check(list->first != NULL, "Invalid list, somehow got a first that is NULL.");
		list->first->prev = NULL;
	} else if (node == list->last) {
		list->last = node->prev;
		check(list->last != NULL, "Invalid list, somehow got a next that is NULL.");
		list->last->next = NULL;
	} else {
		list_node_s *after = node->next;
		list_node_s *before = node->prev;
		after->prev = before;
		before->next = after;
	}

	list->count--;
	result = node->value;
	free(node);

error:
	return result;
}

static inline void
list_node_swap(list_node_s *a, list_node_s *b)
{
	void *temp = a->value;
	a->value = b->value;
	b->value = temp;
}

int
list_bubble_sort(list_s *list, list_compare cmp)
{
	int sorted = 1;

	if (list_count(list) <= 1) {
		return 0;	// already sorted
	}

	do {
		sorted = 1;
		list_foreach(list, first, next, cur) {
			if (cur->next) {
				if (cmp(cur->value, cur->next->value) > 0) {
					list_node_swap(cur, cur->next);
					sorted = 0;
				}
			}
		}
	} while (!sorted);

	return 0;
}

static inline list_s *
list_merge(list_s * left, list_s *right, list_compare cmp)
{
	list_s *result = list_create();
	void *val = NULL;

	while (list_count(left) > 0 || list_count(right) > 0) {
		if (list_count(left) > 0 && list_count(right) > 0) {
			if (cmp(list_first(left), list_first(right)) <= 0) {
				val = list_shift(left);
			} else {
				val = list_shift(right);
			}

			list_push(result, val);
		} else if (list_count(left) > 0) {
			val = list_shift(left);
			list_push(result, val);
		} else if (list_count(right) > 0) {
			val = list_shift(right);
			list_push(result, val);
		}
	}

	return result;
}

list_s *
list_merge_sort(list_s *list, list_compare cmp)
{
	if (list_count(list) <= 1) {
		return list;
	}

	list_s *left = list_create();
	list_s *right = list_create();
	int middle = list_count(list) / 2;

	list_foreach(list, first, next, cur) {
		if (middle > 0) {
			list_push(left, cur->value);
		} else {
			list_push(right, cur->value);
		}

		middle--;
	}

	list_s *sort_left = list_merge_sort(left, cmp);
	list_s *sort_right = list_merge_sort(right, cmp);

	if (sort_left != left)
		list_destroy(left);
	if (sort_right != right)
		list_destroy(right);

	return list_merge(sort_left, sort_right, cmp);
}

void
list_walk(list_s *list, list_apply func)
{
	list_foreach(list, first, next, cur) {
		func(cur->value);
	}
}

void
list_walk_with_state(list_s *list, list_apply_with_state func, void *state)
{
	list_foreach(list, first, next, cur) {
		func(cur->value, state);
	}
}