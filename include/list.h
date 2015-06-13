#ifndef __LIST_H__
#define __LIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    Функции работы со списками
*/

/*
    Базовый тип списка
*/
struct _list
{
    // Значение элемента списка
    void *value;
    // Указатель на следующий элемент
    struct _list *next;
};

typedef struct _list list;

list *add_to_list(list **l, void *a);
list *add_to_list_t(list **l, void *a);

int list_len(list *l);

#define list_temp(x) \
    t_##x

#define list_temp2(x) \
    t2_##x

#define list_val(x) \
    x->value

#define list_tval(x) \
    list_val(list_temp(x))

#define list_for_each(x) \
    list *list_temp(x) = x; \
    while(t_##x != 0)

#define list_next(x) \
    list_temp(x) = list_temp(x)->next

#define list_next_clear(x) \
    list *list_temp2(x) = list_temp(x)->next; \
    free(list_temp(x)); \
    list_temp(x) = list_temp2(x)

#endif
