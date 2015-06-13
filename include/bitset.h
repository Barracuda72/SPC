#ifndef __BITSET_H__
#define __BITSET_H__

#include <lexer.h>  // Для L_INVALID

#define BITSET_LAST L_INVALID

/*
 * Выделяет память под множество
 */
unsigned long *balloc();

/*
    Преобразует массив элементов в битовое множество
*/
unsigned long *bits_from_array(unsigned long *a);

/*
    Распечатывает множество
*/
void bits_print(unsigned long *res);

/*
 * Возвращает первый элемент из множества
 */
unsigned long bits_first(unsigned long *res);

/*
    Добавляет элемент в множество
*/
void bits_append(unsigned long element, unsigned long *set);

/*
    Разность двух множеств
*/
void bits_sub(unsigned long *set1, unsigned long *set2, unsigned long *res);

/*
    Дизъюнкция ("или") двух множеств
*/
void bits_disjunct(unsigned long *set1, unsigned long *set2, unsigned long *res);

/*
    Конъюнкция ("и") двух множеств
*/
void bits_conjunct(unsigned long *set1, unsigned long *set2, unsigned long *res);

/*
    Проверка принадлежности элемента множеству
*/
int bits_belong(long element, unsigned long *set);

#endif // __BITSET_H__
