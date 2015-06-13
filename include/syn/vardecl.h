#ifndef __VARDECL_H__
#define __VARDECL_H__

#include <bitset.h>
#include <syn/parser.h>

DEC_START_TOK(vardecl);
DEC_FOLLOW_TOK(vardecl);

DEC_PARSER_FUNC(vardecl);

INIT_PARSER_FUNC(vardecl);
FINI_PARSER_FUNC(vardecl);

#endif // __VARDECL_H__
