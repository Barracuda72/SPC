#ifndef __VARUSE_H__
#define __VARUSE_H__

#include <bitset.h>
#include <syn/parser.h>

DEC_START_TOK(varuse);
DEC_FOLLOW_TOK(varuse);

DEC_PARSER_FUNC(varuse);

INIT_PARSER_FUNC(varuse);
FINI_PARSER_FUNC(varuse);

#endif // __VARUSE_H__
