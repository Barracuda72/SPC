#include <syn/typedecl.h>
#include <syn/typedefn.h>

#include <sem/id_table.h>
#include <sem/type_table.h>

DEF_START_TOK(typedecl)
{
    L_TYPE,
    L_INVALID
};

DEF_FOLLOW_TOK(typedecl)
{
    L_INVALID
};

DEF_TOK_ARR(aux, typedecl)
{
    L_SEMICOLON,
    L_INVALID
};

DEF_PARSER_FUNC(typedecl)
{
    type_table_elem *ret = 0;
    id_table_elem *id = 0;
    lex_val lv;
    char *name;
    unsigned long *tmp = balloc();
    bits_disjunct(folls, parser_typedecl_aux, tmp);

    accept(L_TYPE);
    do
    {
        id = 0;
        ret = 0;
        if (lex_get_s() == L_IDENT)
        {
            lv = lex_get_v();
            name = strdup(lv.string);
            id = add_to_id_table(0, name, UC_TYPE);
        }
        accept(L_IDENT);
        accept(L_EQUAL);
        ret = CALL_PARSER_FUNC(typedefn, tmp);
        if (id != 0)
            id->type = ret;
        accept(L_SEMICOLON);
    } while (lex_get_s() == L_IDENT);

    free(tmp);
}

INIT_PARSER_FUNC(typedecl)
{
    INIT_PARSER_DATA(typedecl);
    INIT_PARSER_ARRAY(aux, typedecl);
}

FINI_PARSER_FUNC(typedecl)
{
    FINI_PARSER_DATA(typedecl);
    FINI_PARSER_ARRAY(aux, typedecl);
}
