#include <errors.h>

#include <syn/operator.h>
#include <syn/varuse.h>
#include <syn/expr.h>
#include <symtable.h>

#include <spc_i18n.h>
#include <errors.h>
#include <sem/type_table.h>

/*
    Оператор присваивания
*/
DEF_START_TOK(assign_operator)
{
    L_IDENT,
    L_INVALID
};

DEF_FOLLOW_TOK(assign_operator)
{
    L_INVALID
};

DEF_TOK_ARR(aux, assign_operator)
{
    L_ASSIGN,
    L_INVALID
};

DEF_PARSER_FUNC(assign_operator)
{
    unsigned long *tmp = balloc();
    bits_disjunct(folls, parser_assign_operator_aux, tmp);
    bits_disjunct(tmp, parser_start_expr, tmp);

    type_table_elem *tp1, *tp2, *ret;

    tp1 = CALL_PARSER_FUNC_EX(varuse, tmp, parser_assign_operator_aux);
    free(tmp);

    int sl, sc;
    ret = 0;
    sl = syn_line;
    sc = syn_c;
    accept(L_ASSIGN);
    tp2 = CALL_PARSER_FUNC_EX(expr, folls, ex);
    if (!can_assign(tp1, tp2))
    {
        char *st = malloc(1024);
        sprintf(st, "expected '%s' but got '%s'", get_type_name(tp1), get_type_name(tp2));
        err_include(ERROR, _("type mismatch:"), st, sl, sc);
    } else {
        ret = tp1;
    }

    return ret;
}

/*
    Оператор инициализации параметра цикла for
*/
/*
DEF_START_TOK(for_init)
{
    L_IDENT,
    L_INVALID
};

DEF_FOLLOW_TOK(for_init)
{
    L_TO,
    L_DOWNTO,
    L_INVALID
};

DEF_TOK_ARR(aux, for_init)
{
    L_ASSIGN,
    L_INVALID
};

DEF_PARSER_FUNC(for_init)
{
    unsigned long *tmp = balloc();
    bits_disjunct(folls, parser_for_init_aux, tmp);
    bits_disjunct(tmp, parser_start_expr, tmp);

    CALL_PARSER_FUNC(varuse, tmp);
    free(tmp);

    accept(L_ASSIGN);
    CALL_PARSER_FUNC(expr, folls);
}*/

/*
    Цикл с параметром
*/
DEF_START_TOK(for_operator)
{
    L_FOR,
    L_INVALID
};

DEF_FOLLOW_TOK(for_operator)
{
    L_INVALID
};

DEF_TOK_ARR(aux1, for_operator)
{
    L_TO,
    L_DOWNTO,
    L_INVALID
};

DEF_TOK_ARR(aux2, for_operator)
{
    L_DO,
    L_INVALID
};

DEF_PARSER_FUNC(for_operator)
{
    unsigned long *tmp;
    tmp = balloc();
    bits_disjunct(folls, parser_for_operator_aux1, tmp);
    bits_disjunct(tmp, parser_for_operator_aux2, tmp);

    type_table_elem *tp1 = 0, *tp2 = 0;
    int sl1 = 0, sc1 = 0;

    accept(L_FOR);
    int sl = syn_line, sc = syn_c;
    tp1 = CALL_PARSER_FUNC_EX(assign_operator, tmp, parser_for_operator_aux1);
    if (!can_iterate(tp1))
    {
        err_include(ERROR, _("invalid loop initialization"), 0, sl, sc);
        tp1 = 0;
    }

    if (lex_get_s() == L_TO)
        accept(L_TO);
    else
        accept(L_DOWNTO);

    //bits_sub(tmp, parser_for_operator_aux1, tmp);
    bits_disjunct(folls, parser_for_operator_aux2, tmp);

    if (bits_belong(lex_get_s(), parser_start_expr))
    {
        sl1 = syn_line, sc1 = syn_c;
        tp2 = CALL_PARSER_FUNC(expr, tmp);
        if (!can_iterate(tp2))
        {
            err_include(ERROR, _("invalid loop finalization"), 0, sl1, sc1);
            tp2 = 0;
        }
    } else {
        //parser_unexpected("expression");
    }

    if (!can_assign(tp1, tp2))
    {
        char *st = malloc(1024*2);
        sprintf(st, _("expected '%s' but got '%s'"), get_type_name(tp1), get_type_name(tp2));
        err_include(ERROR, _("type mismatch:"), st, sl1, sc1);
    }
    free(tmp);

    accept(L_DO);
    CALL_PARSER_FUNC(g_operator, folls/*, parser_for_operator_aux2*/);
}

/*
    Оператор в общем
*/
DEF_START_TOK(g_operator)
{
    // Заполняется в инициализаторе
    L_INVALID
};

DEF_FOLLOW_TOK(g_operator)
{
    L_INVALID
};

DEF_PARSER_FUNC(g_operator)
{
    while (bits_belong(lex_get_s(), parser_start_g_operator))
    {
        if (lex_get_s() == L_BEGIN) {
            CALL_PARSER_FUNC(complex_operator, folls);
        } else if (lex_get_s() == L_FOR) {
            CALL_PARSER_FUNC(for_operator, folls);
        } else if (lex_get_s() == L_IDENT) {
            CALL_PARSER_FUNC(assign_operator, folls);
        } else {
            err_include(EFATAL, "unsupported operator:", sym_get_sym(lex_get_s()), syn_line, syn_c);
        }
    }
}

/*
    Составной оператор
*/
DEF_START_TOK(complex_operator)
{
    L_BEGIN,
    L_INVALID
};

DEF_FOLLOW_TOK(complex_operator)
{
    L_INVALID
};

DEF_TOK_ARR(aux, complex_operator)
{
    L_END,
    L_SEMICOLON,
    L_INVALID
};

DEF_PARSER_FUNC(complex_operator)
{
    unsigned long *tmp = balloc();
    bits_disjunct(folls, parser_complex_operator_aux, tmp);

    accept(L_BEGIN);
    if (bits_belong(lex_get_s(), parser_start_g_operator))
    {
        CALL_PARSER_FUNC(g_operator, tmp);
    }
    while (lex_get_s() == L_SEMICOLON)
    {
        accept(L_SEMICOLON);
        if (bits_belong(lex_get_s(), parser_start_g_operator))
            CALL_PARSER_FUNC(g_operator, tmp);
    }
    /*while (bits_belong(lex_get_s(), parser_start_g_operator))
    {
        CALL_PARSER_FUNC(g_operator, 0);
        accept(L_SEMICOLON);
    }*/
    accept(L_END);
    free(tmp);
}

INIT_PARSER_FUNC(g_operator)
{
    INIT_PARSER_DATA(complex_operator);
    INIT_PARSER_DATA(g_operator);
    INIT_PARSER_DATA(for_operator);
    INIT_PARSER_DATA(assign_operator);
    //INIT_PARSER_DATA(for_init);

    APPEND_PARSER_DATA(g_operator, complex_operator);
    APPEND_PARSER_DATA(g_operator, for_operator);
    APPEND_PARSER_DATA(g_operator, assign_operator);

    INIT_PARSER_ARRAY(aux, complex_operator);
    INIT_PARSER_ARRAY(aux, assign_operator);
    //INIT_PARSER_ARRAY(aux, for_init);
    INIT_PARSER_ARRAY(aux1, for_operator);
    INIT_PARSER_ARRAY(aux2, for_operator);
}

FINI_PARSER_FUNC(g_operator)
{
    FINI_PARSER_DATA(g_operator);
    FINI_PARSER_DATA(complex_operator);
    FINI_PARSER_DATA(for_operator);
    FINI_PARSER_DATA(assign_operator);
    //FINI_PARSER_DATA(for_init);

    FINI_PARSER_ARRAY(aux, complex_operator);
    FINI_PARSER_ARRAY(aux, assign_operator);
    //FINI_PARSER_ARRAY(aux, for_init);
    FINI_PARSER_ARRAY(aux1, for_operator);
    FINI_PARSER_ARRAY(aux2, for_operator);
}
