#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <bitset.h>
#include <syn/parser.h>

DEC_START_TOK(program);
DEC_FOLLOW_TOK(program);

DEC_PARSER_FUNC(program);

INIT_PARSER_FUNC(program);
FINI_PARSER_FUNC(program);

#endif // __PROGRAM_H__
