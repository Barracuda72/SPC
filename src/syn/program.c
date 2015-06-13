#include <syn/program.h>
#include <syn/block.h>

DEF_START_TOK(program)
{
    L_PROGRAM,
    L_INVALID
};

DEF_FOLLOW_TOK(program)
{
    L_INVALID
};

/*
 * Символы, идущие после блока
 */
DEF_TOK_ARR(aux, program)
{
    L_POINT,
    L_INVALID
};

DEF_PARSER_FUNC_UNWRAP(program)
{
    accept(L_PROGRAM);
    accept(L_IDENT);
    accept(L_SEMICOLON);

    CALL_PARSER_FUNC(block, parser_program_aux);
    accept(L_POINT);
}

INIT_PARSER_FUNC(program)
{
    INIT_PARSER_DATA(program);
    INIT_PARSER_ARRAY(aux, program);
}

FINI_PARSER_FUNC(program)
{
    FINI_PARSER_DATA(program);
    FINI_PARSER_ARRAY(aux, program);
}
