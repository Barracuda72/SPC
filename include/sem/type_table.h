#ifndef __TYPE_TABLE_H__
#define __TYPE_TABLE_H__

/*
    Работа с таблицей типов
*/

#include <list.h>

#include <sem/sem_type.h>
#include <sem/id_table.h>

/*
    Список констант перечислимого типа
*/
/*typedef struct _const_list
{
    // Описание элемента в таблице имен
    struct _id_table_elem *desc;
    // Ссылка на следующий
    struct _const_list *next;
} const_list;*/

typedef struct _type_table_elem
{
    // Тип типа (лол)
    enum USE_TYPE utype;
    // Ссылка на базовый тип (для ограниченного, массива, etc)
    struct _type_table_elem *base;

    union
    {
        // Список констант для перечислимого типа
        list *constants;
        // Список типов индексов для массива
        list *indexes;
        // Максимальное и минимальное значение для ограниченного типа
        // Стандарт C2011 позволяет анонимные структуры
        struct {
            unsigned long min, max;
        };
    };

    // struct _type_table_elem *next;
} type_table_elem;

type_table_elem *add_to_type_table(type_table_elem *base, enum USE_TYPE utype);

void type_init(void);

extern type_table_elem *chartype, *inttype, *realtype, *booltype, *texttype, *ptrtype, *stringtype;

type_table_elem *mix_types(type_table_elem *tp1, type_table_elem *tp2, int code);
int can_assign(type_table_elem *tp1, type_table_elem *tp2);
int can_iterate(type_table_elem *tp1);
char *get_type_name(type_table_elem *tp);
#endif
