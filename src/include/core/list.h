#ifndef __List_h__
#define __List_h__

struct _list_node_s {
	struct _list_node_s *next;
	struct _list_node_s *prev;
	void *value;
};
typedef struct _list_node_s list_node_s;

struct _list_s {
	size_t count;
	list_node_s *first;
	list_node_s *last;
};
typedef struct _list_s list_s;

list_s *list_create();
void    list_destroy(list_s *list);
#define list_free(list) list_destroy(list);
void    list_clear(list_s *list);
void    list_clear_destroy(list_s *list);

#define list_count(A) ((A)->count)
#define list_first(A) ((A)->first != NULL ? (A)->first->value : NULL)
#define list_last(A) ((A)->last != NULL ? (A)->last->value : NULL)

void    list_push(list_s *list, void *value);
void   *list_pop(list_s *list);

void    list_unshift(list_s *list, void *value);
void   *list_shift(list_s *list);

void   *list_remove(list_s *list, list_node_s *node);

#define list_foreach(L, S, M, V) list_node_s *_node = NULL;\
    list_node_s *V = NULL;\
    for(V = _node = L->S; _node != NULL; V = _node = _node->M)

typedef void (*list_apply_cb) (void *value);
void list_walk(list_s *list, list_apply_cb func);

typedef void (*list_apply_with_state_cb) (void *value, void *state);
void list_walk_with_state(list_s *list, list_apply_with_state_cb func, void *state);

list_s *list_concat(list_s *l, list_s *r);

#define list_is_empty(list) (!list->first)

/* Soting */
typedef int (*list_compare) (const void *a, const void *b);
int     list_bubble_sort(list_s *list, list_compare cmp);
list_s *list_merge_sort(list_s *list, list_compare cmp);

#endif

