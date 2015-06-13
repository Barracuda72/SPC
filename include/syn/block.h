#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <bitset.h>
#include <syn/parser.h>

DEC_START_TOK(block);
DEC_FOLLOW_TOK(block);

DEC_PARSER_FUNC(block);

INIT_PARSER_FUNC(block);
FINI_PARSER_FUNC(block);

#endif // __BLOCK_H__
