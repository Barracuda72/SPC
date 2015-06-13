#include <errors.h>

#include <syn/constant.h>

#include <sem/type_table.h>
#include <sem/id_table.h>

#include <spc_i18n.h>

/*
    Определение константы
*/
DEF_START_TOK(constdefn)
{
    L_IDENT,
    L_INVALID
};

DEF_FOLLOW_TOK(constdefn)
{
    L_INVALID
};

DEF_PARSER_FUNC(constdefn)
{
    id_table_elem *id = 0;
    type_table_elem *tp = 0;
    if (lex_get_s() == L_IDENT)
    {
        lex_val lv = lex_get_v();
        char *name = strdup(lv.string);

        id = add_to_id_table(0, name, UC_CONST);
    }
    accept(L_IDENT);
    accept(L_EQUAL);
    tp = CALL_PARSER_FUNC(constant, folls);
    if (id != 0)
        id->type = tp;
}

/*
    Блок определения констант
*/
DEF_START_TOK(constdecl)
{
    L_CONST,
    L_INVALID
};

DEF_FOLLOW_TOK(constdecl)
{
    L_INVALID
};

DEF_TOK_ARR(aux, constdecl)
{
    L_SEMICOLON,
    L_INVALID
};

DEF_PARSER_FUNC(constdecl)
{
    unsigned long *tmp = balloc();
    bits_disjunct(folls, parser_constdecl_aux, tmp);

    accept(L_CONST);
    do
    {
        CALL_PARSER_FUNC(constdefn, tmp);
        accept(L_SEMICOLON);
    } while (lex_get_s() == L_IDENT);

    free(tmp);
}

/*
    Константа
*/
DEF_START_TOK(constant)
{
    L_PLUS,
    L_MINUS,
    L_NUMERIC,
    L_FLOAT,
    L_SCONST,
    L_CHARCONST,
    L_IDENT,
    L_INVALID
};

DEF_FOLLOW_TOK(constant)
{
    L_INVALID
};

DEF_PARSER_FUNC(constant)
{
    type_table_elem *ret = 0;
    // Со знаком - только числовые константы
    if ((lex_get_s() == L_PLUS) || (lex_get_s() == L_MINUS))
    {
        accept(lex_get_s());
        if ((lex_get_s() == L_NUMERIC) || (lex_get_s() == L_FLOAT) || (lex_get_s() == L_IDENT))
        {
            if (lex_get_s() == L_NUMERIC)
                ret = inttype;
            else if (lex_get_s() == L_FLOAT)
                ret = realtype;
            else if (lex_get_s() == L_IDENT)
            {
                int sl = syn_line, sc = syn_c;
                lex_val lv = lex_get_v();
                char *name = strdup(lv.string);
                unsigned long *uclass = UC_CONST;
                id_table_elem *id = search_id_table(0, name, &uclass);
                if (id == 0)
                    err_include (ERROR, _("wrong identifier:"), name, sl, sc);
                else
                    ret = id->type;
            }
            accept(lex_get_s());
        } else {
            parser_unexpected(0);
            accept(lex_get_s());
        }
    } else {
        if ((lex_get_s() == L_NUMERIC) || (lex_get_s() == L_FLOAT)
            || (lex_get_s() == L_IDENT) || (lex_get_s() == L_SCONST) || (lex_get_s() == L_CHARCONST))
        {
            if (lex_get_s() == L_SCONST)
                ret = inttype;
            else if (lex_get_s() == L_CHARCONST)
                ret = realtype;
            if (lex_get_s() == L_NUMERIC)
                ret = inttype;
            else if (lex_get_s() == L_FLOAT)
                ret = realtype;
            else if (lex_get_s() == L_IDENT)
            {
                int sl = syn_line, sc = syn_c;
                lex_val lv = lex_get_v();
                char *name = strdup(lv.string);
                unsigned long *uclass = UC_CONST;
                id_table_elem *id = search_id_table(0, name, &uclass);
                if (id == 0)
                    err_include (ERROR, _("wrong identifier:"), name, sl, sc);
                else
                    ret = id->type;
            }
            accept(lex_get_s());
        } else {
            parser_unexpected(0);
            accept(lex_get_s());
        }
    }

    return ret;
}

INIT_PARSER_FUNC(constant)
{
    INIT_PARSER_DATA(constant);
    INIT_PARSER_DATA(constdecl);
    INIT_PARSER_DATA(constdefn);
    INIT_PARSER_ARRAY(aux, constdecl);
}

FINI_PARSER_FUNC(constant)
{
    FINI_PARSER_DATA(constant);
    FINI_PARSER_DATA(constdecl);
    FINI_PARSER_DATA(constdefn);
    FINI_PARSER_ARRAY(aux, constdecl);
}
