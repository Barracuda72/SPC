#include <list.h>

list *add_to_list(list **l, void *a)
{
    list *tmp = malloc(sizeof(list));
    tmp->value = a;
    tmp->next = *l;
    *l = tmp;
    return tmp;
}

list *add_to_list_t(list **l, void *a)
{
    if (*l == 0)
    {
        list *tmp = malloc(sizeof(list));
        tmp->value = a;
        tmp->next = 0;
        *l = tmp;
        return tmp;
    }
    else
        return add_to_list_t(&((*l)->next), a);
}

int list_len(list *l)
{
    int i = 0;
    while (l != 0)
    {
        l = l->next;
        i++;
    }
    return i;
}
