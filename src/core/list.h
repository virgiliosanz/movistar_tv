#ifndef __List_h__
#define __List_h__

#include <stdlib.h>

typedef struct list_node_s {
    struct list_node_s *next;
    struct list_node_s *prev;
    void *value;
} list_node_t;

typedef struct list_s {
    int count;
    list_node_t *first;
    list_node_t *last;
} list_t;

list_t *list_create();
void list_destroy(list_t *list);
void list_clear(list_t *list);
void list_clear_destroy(list_t *list);

#define list_count(A) ((A)->count)
#define list_first(A) ((A)->first != NULL ? (A)->first->value : NULL)
#define list_last(A) ((A)->last != NULL ? (A)->last->value : NULL)

void list_push(list_t *list, void *value);
void *list_pop(list_t *list);

void list_unshift(list_t *list, void *value);
void *list_shift(list_t *list);

void *list_remove(list_t *list, list_node_t *node);

#define list_foreach(L, S, M, V) list_node_t *_node = NULL;\
    list_node_t *V = NULL;\
    for(V = _node = L->S; _node != NULL; V = _node = _node->M)


/* Soting */
typedef int (*list_compare)(const void *a, const void *b);
int list_bubble_sort(list_t *list, list_compare cmp);
list_t *list_merge_sort(list_t *list, list_compare cmp);


#endif

