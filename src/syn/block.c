#include <syn/vardecl.h>
#include <syn/typedecl.h>
#include <syn/constant.h>
#include <syn/operator.h>
#include <syn/block.h>
#include <syn/parser.h>

#include <bitset.h>

/*
    Раздел меток
*/
DEF_START_TOK(label)
{
    L_LABEL,
    L_INVALID
};

DEF_FOLLOW_TOK(label)
{
    //L_SEMICOLON,
    L_VAR,
    L_TYPE,
    L_CONST,
    L_BEGIN,
    L_INVALID
};

DEF_PARSER_FUNC(label)
{
    accept(L_LABEL);
    accept(L_IDENT);
    while (lex_get_s() == L_COMMA)
    {
        accept(L_COMMA);
        accept(L_IDENT);
    }
    accept(L_SEMICOLON);
}

/*
    Блок
*/
DEF_START_TOK(block)
{
    // Заполняется в инициализаторе
    L_INVALID
};

DEF_FOLLOW_TOK(block)
{
    L_INVALID
};
/*
DEF_TOK_ARR(block, aux)
{
    L_VAR,
    L_TYPE,
    L_CONST,
    L_BEGIN,
    L_INVALID
};*/

DEF_PARSER_FUNC(block)
{
    unsigned long *tmp = balloc();
    bits_disjunct(folls, parser_folls_label, tmp);

    if (lex_get_s() == L_LABEL)
        CALL_PARSER_FUNC(label, tmp);

    if (lex_get_s() == L_CONST)
        CALL_PARSER_FUNC(constdecl, tmp);

    if (lex_get_s() == L_TYPE)
        CALL_PARSER_FUNC(typedecl, tmp);

    if (lex_get_s() == L_VAR)
        CALL_PARSER_FUNC(vardecl, tmp);

    if (lex_get_s() != L_BEGIN)
    {
        parser_unexpected(0);
        skip1(parser_start_complex_operator);
    }

    free(tmp);

    CALL_PARSER_FUNC(complex_operator, folls);
}

INIT_PARSER_FUNC(block)
{
    INIT_PARSER_DATA(label);
    INIT_PARSER_DATA(block);

    APPEND_PARSER_DATA(block, label);
    APPEND_PARSER_DATA(block, vardecl);
    APPEND_PARSER_DATA(block, typedecl);
}

FINI_PARSER_FUNC(block)
{
    FINI_PARSER_DATA(label);
    FINI_PARSER_DATA(block);
}
