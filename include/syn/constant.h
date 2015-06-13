#ifndef __CONSTANT_H__
#define __CONSTANT_H__

#include <bitset.h>
#include <syn/parser.h>

/*
    Блок определения константы
*/

DEC_START_TOK(constdecl);
DEC_FOLLOW_TOK(constdecl);

DEC_PARSER_FUNC(constdecl);

/*
    Константа
*/
DEC_START_TOK(constant);
DEC_FOLLOW_TOK(constant);

DEC_PARSER_FUNC(constant);

INIT_PARSER_FUNC(constant);
FINI_PARSER_FUNC(constant);

#endif // __CONSTANT_H__
