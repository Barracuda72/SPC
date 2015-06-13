#ifndef __ID_TABLE_H__
#define __ID_TABLE_H__

#include <sem/sem_type.h>
#include <sem/type_table.h>

/*
    Работа с таблицей идентификаторов
*/

typedef struct _id_table_elem
{
    // Значение хэша
    unsigned long hash;
    // Сам идентификатор
    char *name;
    // Тип использования
    enum USE_CLASS uclass;
    // Тип идентификатора
    struct _type_table_elem *type;
    // Для процедур: встречалось ли опережающее описание
    // unsigned char forw;
    // Для констант: значение символа
    int value;
    // Ссылки для обеспечения связи
    // FIXME: пока тупо односвязный список, но потом будет преобразован
    // в бинарное дерево
    // struct _id_table_elem *left, *right;
} id_table_elem;

/*
    Добавление элемента в таблицу идентификаторов
*/
id_table_elem *add_to_id_table(unsigned long hash, char *name, enum USE_CLASS uclass);

/*
    Поиск определяющего вхождения для соответствующего прикладного
*/
id_table_elem *search_id_table(unsigned long hash, char *name, enum USE_CLASS *uclass);


void id_init(void);
#endif
