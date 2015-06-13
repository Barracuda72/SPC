#include <syn/vardecl.h>
#include <syn/varpack.h>

DEF_START_TOK(vardecl)
{
    L_VAR,
    L_INVALID
};

DEF_FOLLOW_TOK(vardecl)
{
    L_INVALID
};

DEF_TOK_ARR(aux, vardecl)
{
    //L_COLON,
    L_SEMICOLON,
    L_INVALID
};

DEF_PARSER_FUNC(vardecl)
{
    accept(L_VAR);
    unsigned long *tmp = balloc();
    bits_disjunct(folls, parser_vardecl_aux, tmp);
    do
    {
        CALL_PARSER_FUNC(varpack, tmp);
        accept(L_SEMICOLON);
    } while (lex_get_s() == L_IDENT);
    free(tmp);
}

INIT_PARSER_FUNC(vardecl)
{
    INIT_PARSER_DATA(vardecl);
    INIT_PARSER_ARRAY(aux, vardecl);
}

FINI_PARSER_FUNC(vardecl)
{
    FINI_PARSER_DATA(vardecl);
    FINI_PARSER_ARRAY(aux, vardecl);
}
