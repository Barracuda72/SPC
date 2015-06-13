#ifndef __SEM_H__
#define __SEM_H__

/*
    Определения для работы семантического анализа
*/

/*
    Способ использования идентификатора
*/
enum USE_CLASS
{
    UC_PROG = 4000,
    UC_TYPE,
    UC_CONST,
    UC_VAR,
    UC_PROC,
    UC_FUNC,
    UC_UNK
};

/*
    Тип типа (лол) идентификатора
*/
enum USE_TYPE
{
    UT_SCALAR = 5000,
    UT_LIMITED,
    UT_ENUM,
    UT_ARRAY,
    UT_RECORD,
    UT_REFERENCE,
    UT_SET,
    UT_FILE
};

/*
    Опережающие определения
*/
struct _id_table_elem;
struct _type_table_elem;
struct _const_list;

/*
    Функция инициализации
*/
void sem_init(void);
#endif
