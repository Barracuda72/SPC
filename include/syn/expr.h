#ifndef __EXPR_H__
#define __EXPR_H__

#include <bitset.h>
#include <syn/parser.h>

DEC_START_TOK(expr);
DEC_FOLLOW_TOK(expr);

DEC_PARSER_FUNC(expr);

INIT_PARSER_FUNC(expr);
FINI_PARSER_FUNC(expr);

#endif // __EXPR_H__
