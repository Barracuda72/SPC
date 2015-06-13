#include <errors.h>
#include <list.h>
#include <spc_i18n.h>
#include <syn/typedefn.h>
#include <syn/constant.h>

#include <sem/type_table.h>
#include <sem/id_table.h>
/*
    Перечислимый тип
*/
DEF_START_TOK(enum_type)
{
    L_LPANTH,
    L_INVALID
};

DEF_FOLLOW_TOK(enum_type)
{
    L_INVALID
};

DEF_PARSER_FUNC(enum_type)
{
    char *name = 0;
    type_table_elem *ret = 0;
    id_table_elem *id;
    int i = 1;

    list *constants = 0;

    ret = add_to_type_table(inttype, UT_ENUM);

    accept(L_LPANTH);
    lex_val lv;

    lv = lex_get_v();

    if (lex_get_s() == L_IDENT)
    {
        name = strdup(lv.string);
        id = add_to_id_table(0, name, UC_CONST);
        if (id != 0)
        {
            id->type = ret;
            id->value = i++;
            add_to_list_t(&constants, id);
        }
    }
    accept(L_IDENT);

    while(lex_get_s() == L_COMMA)
    {
        accept(L_COMMA);

        lv = lex_get_v();

        if (lex_get_s() == L_IDENT)
        {
            name = strdup(lv.string);
            id = add_to_id_table(0, name, UC_CONST);
            if (id != 0)
            {
                id->type = ret;
                id->value = i++;
                add_to_list_t(&constants, id);
            }
        }
        accept(L_IDENT);
    }
    accept(L_RPANTH);

    ret->constants = constants;

    return ret;
}

/*
    Интервальный тип
*/
DEF_START_TOK(interval_type)
{
    L_CHARCONST,
    L_NUMERIC,
    L_PLUS,
    L_MINUS,
    L_INVALID
};

DEF_FOLLOW_TOK(interval_type)
{
    L_INVALID
};

DEF_TOK_ARR(aux, intervaltype)
{
    L_TWOPOINTS,
    L_INVALID
};

DEF_PARSER_FUNC(interval_type)
{
    /*TOKEN_TYPE t = lex_get_s();
    if (t == L_NUMERIC || t == L_CHARCONST)
        accept(t);
    else
        accept(L_NUMERIC);

    accept(L_TWOPOINTS);

//    accept(t); // Примитивная проверка на константу того же типа

    t = lex_get_s();
    if (t == L_NUMERIC || t == L_CHARCONST)
        accept(t);
    else
        accept(L_NUMERIC);*/
    unsigned long *tmp = balloc();
    bits_disjunct(folls, parser_intervaltype_aux, tmp);

    type_table_elem *tp1, *tp2, *ret = 0;
    int sl = syn_line, sc = syn_c;
    tp1 = CALL_PARSER_FUNC(constant, tmp);
    accept(L_TWOPOINTS);
    tp2 = CALL_PARSER_FUNC(constant, folls);

    if ((tp1 == 0) || (tp2 == 0))
        ret = 0;
    else if (tp1 == tp2)
    {
        ret = add_to_type_table(tp1, UT_LIMITED);
        // TODO: fill min and max
    }
    else
        err_include(ERROR, _("types should match"), 0, sl, sc);

    free(tmp);

    return ret;
}

/*
    Простой тип = перечислимый + интервальный + имя типа
*/
DEF_START_TOK(simple_type)
{
    L_IDENT,
    L_INVALID
};

DEF_FOLLOW_TOK(simple_type)
{
    L_INVALID
};

DEF_PARSER_FUNC(simple_type)
{
    type_table_elem *ret = 0;
    if (lex_get_s() == L_CHARCONST || lex_get_s() == L_NUMERIC || lex_get_s() == L_PLUS || lex_get_s() == L_MINUS)
        ret = CALL_PARSER_FUNC(interval_type, folls);
    else if (lex_get_s() == L_LPANTH)
        ret = CALL_PARSER_FUNC(enum_type, folls);
    else if (lex_get_s() == L_IDENT)
    {
        lex_val lv = lex_get_v();
        char *name = strdup(lv.string);
        int sl = syn_line, sc = syn_c;
        accept(L_IDENT);
        unsigned long uclass = UC_UNK;
        id_table_elem *ie = search_id_table(0, name, &uclass);
        if (ie == 0)
            err_include(ERROR, _("undeclared identifier:"), name, sl, sc);
        // Если дальше идет две точки, то это будет интервальный тип
        if (lex_get_s() == L_TWOPOINTS)
        {
            type_table_elem *tp;
            accept(L_TWOPOINTS);
            tp = CALL_PARSER_FUNC(constant, folls);
            if ((ie == 0) || (tp == 0) || (ie->type == 0))
                ret = 0;
            else if (ie->type == tp)
            {
                ret = add_to_type_table(tp, UT_LIMITED);
                // TODO: fill min and max
            }
            else
                err_include(ERROR, _("types should match"), 0, sl, sc);
        } else {
            // Это имя типа
            if ((ie != 0) && (uclass == UC_TYPE))
                ret = ie->type;
            else if (uclass != UC_UNK)
                err_include (ERROR, _("wrong identifier use:"), name, sl, sc);
        }
    } else// Этого не должно случиться, но все же
        err_include(EFATAL, _(parser_int_error), _("simple_type called for wrong type!"), syn_line, syn_c);

    return ret;
}

/*
    Составной тип, запись или массив (пока только массив)
*/
DEF_START_TOK(complex_type)
{
    L_ARRAY,
    L_INVALID
};

DEF_FOLLOW_TOK(complex_type)
{
    L_INVALID
};

DEF_TOK_ARR(aux, complextype)
{
    L_RBRACKET,
    L_COMMA,
    L_INVALID
};

DEF_PARSER_FUNC(complex_type)
{
    type_table_elem *tp1, *tp2, *ret = 0;
    list *indexes = 0;
    accept(L_ARRAY);
    if (lex_get_s() == L_LBRACKET)
    {
        accept(L_LBRACKET);
        unsigned long *tmp = balloc();
        bits_disjunct(folls, parser_complextype_aux, tmp);

        tp1 = CALL_PARSER_FUNC(simple_type, tmp);
        add_to_list_t(&indexes, tp1);
        while (lex_get_s() == L_COMMA)
        {
            accept(L_COMMA);
            tp1 = CALL_PARSER_FUNC(simple_type, tmp);
            add_to_list_t(&indexes, tp1);
        }
        accept(L_RBRACKET);

        free(tmp);
    }
    accept(L_OF);
    tp2 = CALL_PARSER_FUNC(typedefn, folls);
    ret = add_to_type_table(tp2, UT_ARRAY);
    ret -> indexes = indexes;

    return ret;
}

/*
    Ссылочный тип
*/
DEF_START_TOK(pointer_type)
{
    L_ARROW,
    L_INVALID
};

DEF_FOLLOW_TOK(pointer_type)
{
    L_INVALID
};

DEF_PARSER_FUNC(pointer_type)
{
    lex_val lv;
    char *name;
    id_table_elem *id;
    accept(L_ARROW);
    if (lex_get_s() == L_IDENT)
    {
        unsigned long uclass = UC_TYPE;
        char *name = strdup(lv.string);
        id = search_id_table(0, name, &uclass);
    }
    accept(L_IDENT);

    if (id != 0)
    {
        type_table_elem *p = add_to_type_table(id->type, UT_REFERENCE);
        return p;
    }
    else
        return ptrtype; // FIXME: вообще-то должен быть нормальный ссылочный тип
}

/*
    Тип = простой тип + составной тип + ссылочный тип
*/
DEF_START_TOK(typedefn)
{
    // Заполняется в инициализаторе
    L_INVALID
};

DEF_FOLLOW_TOK(typedefn)
{
    L_INVALID
};

DEF_PARSER_FUNC(typedefn)
{
    type_table_elem * ret = 0;
    if(bits_belong(lex_get_s(), parser_start_simple_type))
        ret = CALL_PARSER_FUNC(simple_type, folls);
    else if (bits_belong(lex_get_s(), parser_start_complex_type))
        ret = CALL_PARSER_FUNC(complex_type, folls);
    else if (lex_get_s() == L_ARROW)
        ret = CALL_PARSER_FUNC(pointer_type, folls);
    else {
        //err_include(EFATAL, _(parser_int_error), "typedefn called for non-type", syn_line, syn_c);
        syn_c += 1;
        parser_unexpected(_(": type expected"));
    }
    return ret;
}

INIT_PARSER_FUNC(typedefn)
{
    parser_name_typedefn = "type";

    INIT_PARSER_DATA(enum_type);
    INIT_PARSER_DATA(interval_type);
    INIT_PARSER_DATA(complex_type);

    INIT_PARSER_DATA(simple_type);

    APPEND_PARSER_DATA(simple_type, enum_type);
    APPEND_PARSER_DATA(simple_type, interval_type);

    INIT_PARSER_DATA(pointer_type);

    INIT_PARSER_DATA(typedefn);

    APPEND_PARSER_DATA(typedefn, simple_type);
    APPEND_PARSER_DATA(typedefn, pointer_type);
    APPEND_PARSER_DATA(typedefn, complex_type);

    INIT_PARSER_ARRAY(aux, complextype);
    INIT_PARSER_ARRAY(aux, intervaltype);
}

FINI_PARSER_FUNC(typedefn)
{
    FINI_PARSER_DATA(enum_type);
    FINI_PARSER_DATA(interval_type);
    FINI_PARSER_DATA(pointer_type);
    FINI_PARSER_DATA(simple_type);
    FINI_PARSER_DATA(complex_type);

    FINI_PARSER_DATA(typedefn);

    FINI_PARSER_ARRAY(aux, complextype);
    FINI_PARSER_ARRAY(aux, intervaltype);
}
