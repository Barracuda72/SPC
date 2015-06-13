#include <stdio.h>
#include <stdlib.h>

#include <lexer.h>
#include <errors.h>
#include <sem/type_table.h>
#include <spc_i18n.h>

extern int sem_l, sem_c;

//static type_table_elem *type_table = 0;
static list *type_table = 0;

int mix_line, mix_c;

extern int syn_line, syn_c;

type_table_elem *chartype, *inttype, *realtype, *booltype, *texttype, *ptrtype, *stringtype;

type_table_elem *add_to_type_table(type_table_elem *base, enum USE_TYPE utype)
{
    type_table_elem *res = malloc(sizeof(type_table_elem));
    res->base = base;
    res->utype = utype;
    //res->next = type_table;
    //type_table = res;
    add_to_list(&type_table, res);
    return res;
}
/*
type_table_elem *add_to_type_table_i(type_table_elem *base, unsigned long min, unsigned long max)
{
    type_table_elem *res = malloc(sizeof(type_table_elem));
    res->base = base;
    res->utype = UT_LIMITED;
    res->min = min;
    res->max = max;
    //res->next = type_table;
    //type_table = res;
    add_to_list(&type_table, res);
    return res;
}

type_table_elem *add_to_type_table_e(type_table_elem *base, const_list *l)
{
    type_table_elem *res = malloc(sizeof(type_table_elem));
    res->base = base;
    res->utype = UT_ENUM;
    res->constants = l;
    res->next = type_table;
    type_table = res;
    return res;
}
*/
type_table_elem *search_type_table()
{
    return 0;
}

void type_init(void)
{
    // Символьный
    chartype = add_to_type_table(0, UT_LIMITED);
    chartype->min = 0;
    chartype->max = 255;

    // Целый
    inttype = add_to_type_table(0, UT_LIMITED);
    inttype->min = 0;
    inttype->max = 65536;

    // Логический
  /*  const_list *t,*f;
    t = malloc(sizeof(const_list));
    f = malloc(sizeof(const_list));
    t->desc = add_to_id_table(0, "true", UC_CONST);
    f->desc = add_to_id_table(0, "false", UC_CONST);
    t->next = f;
    f->next = 0;*/
    id_table_elem *t, *f;
    t = add_to_id_table(0, "true", UC_CONST);
    f = add_to_id_table(0, "false", UC_CONST);

    booltype = add_to_type_table(0, UT_ENUM);
    add_to_list(&(booltype->constants), t);
    add_to_list(&(booltype->constants), f);

    // Действитеьный
    realtype = add_to_type_table(0, UT_LIMITED);

    // Текстовый файл
    texttype = add_to_type_table(chartype, UT_FILE);

    // Строка
    stringtype = add_to_type_table(0, UT_SCALAR);

    // Указатель
    ptrtype = add_to_type_table(0, UT_REFERENCE);
}


/*
    Процедура проверки и смешивания типов
*/
type_table_elem *mix_types(type_table_elem *tp1, type_table_elem *tp2, int code)
{
    if ((tp1 == 0) || (tp2 == 0))
        return 0;

    switch (code)
    {
        case L_PLUS:
            if (((tp1 == stringtype) || (tp1 == chartype)) &&
                ((tp2 == stringtype) || (tp2 == chartype)))
                return stringtype;
            // Проваливаемся сквозь
        case L_STAR:
        case L_MINUS:
            if ((tp1 == tp2) && ((tp1 == inttype)||(tp1 == realtype)))
                return tp1;
            else if ((tp1->utype == UT_LIMITED) && (tp2->utype == UT_LIMITED)
                    && (tp1->base == inttype) && (tp2->base == inttype))
                    return inttype;
            else if ((tp1->utype == UT_LIMITED) && (tp2 == inttype)
                    && (tp1->base == inttype))
                    return inttype;
            else if ((tp2->utype == UT_LIMITED) && (tp1 == inttype)
                    && (tp2->base == inttype))
                    return inttype;
            // Проваливаемся сквозь
        case L_SLASH:
            if (((tp1 == realtype) || (tp1 == inttype) || ((tp1->utype == UT_LIMITED) && (tp1->base == inttype))) &&
                ((tp2 == realtype) || (tp2 == inttype) || ((tp2->utype == UT_LIMITED) && (tp2->base == inttype))))
                return realtype;
            break;

        case L_DIV:
        case L_MOD:
            if (((tp1 == inttype) || ((tp1->utype == UT_LIMITED) && (tp1->base == inttype))) &&
                ((tp2 == inttype) || ((tp2->utype == UT_LIMITED) && (tp2->base == inttype))))
                return inttype;
            break;

        case L_XOR:
        case L_OR:
        case L_AND:
            if ((tp1 == tp2) && (tp1 == booltype))
                return booltype;
            break;

        case L_GREAT:
        case L_LESS:
        case L_EQUAL:
        case L_NEQUAL:
        case L_GEQUAL:
        case L_LEQUAL:
            if ((((tp1 == realtype) || (tp1 == inttype) || ((tp1->utype == UT_LIMITED) && (tp1->base == inttype))) &&
                ((tp2 == realtype) || (tp2 == inttype) || ((tp2->utype == UT_LIMITED) && (tp2->base == inttype)))) || (tp1 == tp2))
                return booltype;
        default:
            break;
    }

    char *err = malloc(1024);
    sprintf(err, _("'%s' and '%s'"), get_type_name(tp1), get_type_name(tp2));
    err_include(ERROR, _("wrong types:"), err, sem_l, sem_c);

    return 0;
}

/// Можно ли присвоить переменной типа 1 значение типа 2
int can_assign(type_table_elem *tp1, type_table_elem *tp2)
{

    return
    // Типы совпадают - присваивание возможно
        (tp1 == tp2) ||
    // Ошибка типов была обработана ранее
        ((tp1 == 0) || (tp2 == 0)) ||
    // Расширение типа
        ((tp1 == realtype) && (tp2 == inttype)) ||
        ((tp1 == stringtype) && (tp2 == chartype)) ||
    // Интервальный тип совместим со своим базовым
        ((tp1->utype == UT_LIMITED) && (tp1->base == tp2)) ||
    // И наоборот
        ((tp2->utype == UT_LIMITED) && (tp2->base == tp1)) ||
    // Интервальные типы с одной базой совместимы
        ((tp1->utype == UT_LIMITED) && (tp2->utype == UT_LIMITED) && (tp2->base == tp1->base)) ||
        (0);
}

/// Можно ли использовать значения типа 1 в операторе for
int can_iterate(type_table_elem *tp1)
{
    return
    // Один из типов нулевой
        (tp1 == 0) ||
    // Перечислимые и интервальные типы допустимы
        (tp1->utype == UT_ENUM) ||
        (tp1->utype == UT_LIMITED) ||
    // Допустимы целочисленный и символьный типы
        (tp1 == inttype) ||
        (tp1 == chartype) ||
        (0);
}

char *get_type_name(type_table_elem *tp)
{
    if (tp == 0)
        return _("unknown type");

    if (tp == inttype)
        return _("integer");

    if (tp == stringtype)
        return _("string");

    if (tp == realtype)
        return _("real");

    if (tp == booltype)
        return _("boolean");

    if (tp == ptrtype)
        return _("pointer");

    if (tp == chartype)
        return _("char");

    if (tp == texttype)
        return _("text");

    if (tp->utype == UT_ARRAY)
    {
        char *tmp = malloc(1024);
        sprintf(tmp, _("array of %s"), get_type_name(tp->base));
        return tmp;
    }

    if (tp->utype == UT_REFERENCE)
    {
        char *tmp = malloc(1024);
        sprintf(tmp, _("pointer of %s"), get_type_name(tp->base));
        return tmp;
    }

    if (tp->utype == UT_ENUM)
    {
        return _("enumeration");
    }

    if (tp->utype == UT_LIMITED)
    {
        char *tmp = malloc(1024);
        sprintf(tmp, _("limited of %s"), get_type_name(tp->base));
        return tmp;
    }
    return _("dummy");
}
