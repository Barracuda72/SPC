#ifndef __OPERATOR_H__
#define __OPERATOR_H__

#include <bitset.h>
#include <syn/parser.h>

DEC_START_TOK(g_operator);
DEC_FOLLOW_TOK(g_operator);

DEC_PARSER_FUNC(g_operator);

DEC_START_TOK(complex_operator);
DEC_FOLLOW_TOK(complex_operator);

DEC_PARSER_FUNC(complex_operator);

INIT_PARSER_FUNC(g_operator);
FINI_PARSER_FUNC(g_operator);

#endif // __OPERATOR_H__
