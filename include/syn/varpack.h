#ifndef __VARPACK_H__
#define __VARPACK_H__

#include <bitset.h>
#include <syn/parser.h>

DEC_START_TOK(varpack);
DEC_FOLLOW_TOK(varpack);

DEC_PARSER_FUNC(varpack);

INIT_PARSER_FUNC(varpack);
FINI_PARSER_FUNC(varpack);

#endif // __VARPACK_H__
