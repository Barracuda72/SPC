#include <errors.h>

#include <syn/expr.h>
#include <syn/varuse.h>
#include <spc_i18n.h>

#include <sem/type_table.h>

int sem_l, sem_c;

/*
    Операции отношения = | <> | < | <= | >= | > | in
*/
DEF_TOK_ARR(rela, ops)
{
    L_EQUAL,
    L_NEQUAL,
    L_LESS,
    L_LEQUAL,
    L_GEQUAL,
    L_GREAT,
    L_IN,
    L_INVALID
};

/*
    Аддитивные операции + | - | or
*/
DEF_TOK_ARR(add, ops)
{
    L_PLUS,
    L_MINUS,
    L_OR,
    L_XOR,
    L_INVALID
};

/*
    Мультипликативные операции * | / | div | mod | and
*/
DEF_TOK_ARR(mult, ops)
{
    L_STAR,
    L_SLASH,
    L_DIV,
    L_MOD,
    L_AND,
    L_INVALID
};

DEF_TOK_ARR(aux, factor)
{
    L_RPANTH,
    L_INVALID
};

/*
    Множитель
*/
DEF_START_TOK(factor)
{
    L_IDENT,
    L_NUMERIC,
    L_FLOAT,
    L_SCONST,
    L_CHARCONST,
    L_NIL,
    L_LPANTH,
    L_NOT,
    L_INVALID
};

DEF_FOLLOW_TOK(factor)
{
    L_INVALID
};

DEF_PARSER_FUNC(factor)
{
    type_table_elem *res = 0;

    int c = lex_get_s();

    switch (c)
    {
        case L_IDENT: // Переменная или функция
            res = CALL_PARSER_FUNC(varuse, folls);
            break;

        case L_NUMERIC: // Константа
            res = inttype;
            accept(c);
            break;

        case L_FLOAT:
            res = realtype;
            accept(c);
            break;

        case L_SCONST:
            res = stringtype;
            accept(c);
            break;

        case L_CHARCONST:
            res = chartype;
            accept(c);
            break;

        case L_NIL:
            res = ptrtype;
            accept(c);
            break;

        case L_LPANTH:
            accept(L_LPANTH);
            unsigned long *tmp = balloc();
            bits_disjunct(folls, parser_factor_aux, tmp);
            res = CALL_PARSER_FUNC(expr, tmp);
            free(tmp);
            accept(L_RPANTH);
            break;

        // Множеств у меня нет
        case L_NOT:
            accept(L_NOT);
            int sl = syn_line;
            int sc = syn_c;
            type_table_elem *ttp = CALL_PARSER_FUNC(factor, folls);
            if (ttp != booltype)
                err_include(ERROR, _("boolean expression expected"), 0, sl, sc);
            res = booltype;
            break;

        default:
            parser_unexpected(_(": error in expression"));
    }

    return res;
}

/*
    Слагаемое
*/
DEF_START_TOK(term)
{
    // Заполняется в инициализаторе
    L_INVALID
};

DEF_FOLLOW_TOK(term)
{
    L_INVALID
};

DEF_PARSER_FUNC(term)
{
    unsigned long *tmp = balloc();
    bits_disjunct(folls, parser_ops_mult, tmp);

    type_table_elem *tp1, *tp2;
    tp1 = CALL_PARSER_FUNC_EX(factor, tmp, ex);
    while (bits_belong(lex_get_s(), parser_ops_mult))
    {
        int c = lex_get_s();
        sem_l = syn_line;
        sem_c = syn_c;
        accept(c);
        tp2 = CALL_PARSER_FUNC(factor, tmp/*folls*/);
        tp1 = mix_types(tp1, tp2, c);
    }
    free(tmp);
    return tp1;
}

/*
    Простое выражение
*/
DEF_START_TOK(simple_expr)
{
    // Дополняется в инициализаторе
    L_PLUS,
    L_MINUS,
    L_INVALID
};

DEF_FOLLOW_TOK(simple_expr)
{
    L_INVALID
};

DEF_PARSER_FUNC(simple_expr)
{
    type_table_elem *tp1 = 0, *tp2 = 0;

    sem_l = syn_line;
    sem_c = syn_c;

    if ((lex_get_s() == L_PLUS) || (lex_get_s() == L_MINUS))
    {
        accept(lex_get_s());
        tp2 = inttype;
    }

    unsigned long *tmp = balloc();
    bits_disjunct(folls, parser_ops_add, tmp);

    tp1 = CALL_PARSER_FUNC_EX(term, tmp, ex);

    if (tp2 != 0)
        tp1 = mix_types(tp1, tp2, L_PLUS);

    while (bits_belong(lex_get_s(), parser_ops_add))
    {
        int c = lex_get_s();
        sem_l = syn_line;
        sem_c = syn_c;
        accept(c);
        tp2 = CALL_PARSER_FUNC(term, tmp);
        tp1 = mix_types(tp1, tp2, c);
    }
    free(tmp);

    return tp1;
}

DEF_START_TOK(expr)
{
    // Заполняется в инициализаторе,
    L_INVALID
};

DEF_FOLLOW_TOK(expr)
{
    L_INVALID
};

DEF_PARSER_FUNC(expr)
{
    type_table_elem *tp1, *tp2;

    unsigned long *tmp = balloc();
    bits_disjunct(folls, parser_ops_rela, tmp);

    tp1 = CALL_PARSER_FUNC_EX(simple_expr, tmp, ex); // HACK
    if (bits_belong(lex_get_s(), parser_ops_rela)) // НЕ while!
    {
        int c = lex_get_s();
        sem_l = syn_line;
        sem_c = syn_c;
        accept(c);
        tp2 = CALL_PARSER_FUNC(simple_expr, tmp);
        tp1 = mix_types(tp1, tp2, c);
    }

    free(tmp);
    return tp1;
}

INIT_PARSER_FUNC(expr)
{
    parser_name_expr = "expression";
    INIT_PARSER_DATA(expr);

    INIT_PARSER_ARRAY(rela, ops);
    INIT_PARSER_ARRAY(add, ops);
    INIT_PARSER_ARRAY(mult, ops);
    INIT_PARSER_ARRAY(aux, factor);

    INIT_PARSER_DATA(factor);
    INIT_PARSER_DATA(term);
    INIT_PARSER_DATA(simple_expr);

    APPEND_PARSER_DATA(term, factor);
    APPEND_PARSER_DATA(simple_expr, term);
    APPEND_PARSER_DATA(expr, simple_expr);
}

FINI_PARSER_FUNC(expr)
{
    FINI_PARSER_DATA(expr);
    FINI_PARSER_DATA(factor);
    FINI_PARSER_DATA(term);
    FINI_PARSER_DATA(simple_expr);

    FINI_PARSER_ARRAY(rela, ops);
    FINI_PARSER_ARRAY(add, ops);
    FINI_PARSER_ARRAY(mult, ops);
    FINI_PARSER_ARRAY(aux, factor);
}
