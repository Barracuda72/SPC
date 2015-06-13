#ifndef __TYPEDECL_H__
#define __TYPEDECL_H__

#include <bitset.h>
#include <syn/parser.h>

DEC_START_TOK(typedecl);
DEC_FOLLOW_TOK(typedecl);

DEC_PARSER_FUNC(typedecl);

INIT_PARSER_FUNC(typedecl);
FINI_PARSER_FUNC(typedecl);

#endif // __TYPEDECL_H__
