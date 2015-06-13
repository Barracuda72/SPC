#include <string.h>

#include <list.h>

#include <syn/varpack.h>
#include <syn/typedefn.h>

#include <sem/sem_type.h>
#include <sem/id_table.h>
#include <sem/type_table.h>

static list *int_var_list = 0;

void add_var()
{
    if (lex_get_s() == L_IDENT)
    {
        lex_val tmp = lex_get_v();
        char *name = strdup(tmp.string);
        id_table_elem *tmp3 = add_to_id_table(0, name, UC_VAR);
        if (tmp3 != 0)
            add_to_list(&int_var_list, tmp3);
    }
}

DEF_START_TOK(varpack)
{
    L_IDENT,
    L_COLON, // HACK
    L_INVALID
};

DEF_FOLLOW_TOK(varpack)
{
    L_SEMICOLON,
    L_INVALID
};


DEF_START_TOK(varlist)
{
    L_IDENT,
    L_INVALID
};

DEF_FOLLOW_TOK(varlist)
{
    L_INVALID
};

DEF_PARSER_FUNC(varlist)
{
    add_var();
    accept(L_IDENT);

    while(lex_get_s() == L_COMMA)
    {
        accept(L_COMMA);
        add_var();
        accept(L_IDENT);
    }
}

DEF_TOK_ARR(aux, varpack)
{
    L_COLON,
    //L_IDENT,
    L_INVALID
};

DEF_TOK_ARR(aux2, varpack)
{
    L_INVALID
};

DEF_PARSER_FUNC(varpack)
{
    unsigned long *tmp = balloc();
    bits_disjunct(folls, parser_varpack_aux2, tmp);
    int_var_list = 0;
    CALL_PARSER_FUNC_EX(varlist, tmp, parser_varpack_aux);
    free(tmp);
    accept(L_COLON);
    type_table_elem *tp = (type_table_elem *)CALL_PARSER_FUNC(typedefn, folls);
    list_for_each(int_var_list)
    {
        id_table_elem *q = list_tval(int_var_list);
        if (q->type == -1)
            q->type = tp;
        else
            q->type = 0;
        list_next_clear(int_var_list);
    }
}

INIT_PARSER_FUNC(varpack)
{
    parser_name_varlist = "identifier";
    INIT_PARSER_DATA(varpack);
    INIT_PARSER_DATA(varlist);
    INIT_PARSER_ARRAY(aux, varpack);
    INIT_PARSER_ARRAY(aux2, varpack)

    bits_disjunct(parser_varpack_aux, parser_start_typedefn, parser_varpack_aux2);
}

FINI_PARSER_FUNC(varpack)
{
    FINI_PARSER_DATA(varpack);
    FINI_PARSER_DATA(varlist);
    FINI_PARSER_ARRAY(aux, varpack);
    FINI_PARSER_ARRAY(aux2, varpack);
}
