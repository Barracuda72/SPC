#ifndef __TYPEDEFN_H__
#define __TYPEDEFN_H__

#include <bitset.h>
#include <syn/parser.h>

DEC_START_TOK(typedefn);
DEC_FOLLOW_TOK(typedefn);

DEC_PARSER_FUNC(typedefn);

INIT_PARSER_FUNC(typedefn);
FINI_PARSER_FUNC(typedefn);

#endif // __TYPEDEFN_H__
