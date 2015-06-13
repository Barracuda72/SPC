#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdlib.h>

#include <bitset.h>
#include <lexer.h>

extern int syn_line, syn_c;
extern const char *parser_int_error;
extern const char *syn_err;

typedef void * (*parser_func)(unsigned long *, unsigned long *);

void *parser_exec(unsigned long *start, unsigned long *end, unsigned long *folls, parser_func pf, const char *name);
void skip1(unsigned long *folls);
void skip2(unsigned long *start, unsigned long *folls);
void accept(TOKEN_TYPE t);
void init_parser(void);
void fini_parser(void);
void parser_unexpected(const char *extmsg);

#define INIT_PARSER_ARRAY(funcname, arrname) \
  { \
    if (parser_##arrname##_##funcname == 0) \
        parser_##arrname##_##funcname = bits_from_array(int_##arrname##_##funcname);\
  }

#define INIT_PARSER_DATA(funcname) \
  { \
    INIT_PARSER_ARRAY(funcname, start); \
    INIT_PARSER_ARRAY(funcname, folls); \
  }

#define FINI_PARSER_ARRAY(funcname, arrname) \
  { \
    if (parser_##arrname##_##funcname != 0) \
        free(parser_##arrname##_##funcname);\
  }

#define FINI_PARSER_DATA(funcname) \
  { \
    FINI_PARSER_ARRAY(funcname, start); \
    FINI_PARSER_ARRAY(funcname, folls); \
  }\

#define APPEND_PARSER_DATA(funcname, addname) \
  { \
    bits_disjunct(parser_start_##funcname, \
        parser_start_##addname, parser_start_##funcname); \
  }

#define INIT_PARSER_FUNC(funcname)\
  void init_parser_##funcname(void)

#define FINI_PARSER_FUNC(funcname)\
  void fini_parser_##funcname(void)

#define CALL_INIT_FUNC(funcname)\
  init_parser_##funcname()

#define CALL_FINI_FUNC(funcname)\
  fini_parser_##funcname()

#define DEC_PARSER_FUNC(funcname)\
  void *parse_##funcname (unsigned long *, unsigned long *)

#if 1
// Нейтрализация включена

#define DEF_PARSER_FUNC(funcname) \
  void *int_parse_##funcname (unsigned long *, unsigned long *);\
  static char *parser_name_##funcname = #funcname;\
  void *parse_##funcname (unsigned long *folls, unsigned long *ex) \
  { \
    return parser_exec(parser_start_##funcname, folls, ex, int_parse_##funcname, parser_name_##funcname);\
  }\
  \
  void *int_parse_##funcname (unsigned long *folls, unsigned long *ex)

#else
// Нейтрализация выключена
#define DEF_PARSER_FUNC(funcname) \
  void *parse_##funcname (unsigned long *folls, unsigned long *ex)

#endif

#define DEF_PARSER_FUNC_UNWRAP(funcname) \
  void *parse_##funcname (unsigned long *folls, unsigned long *ex)

#define CALL_PARSER_FUNC(funcname, funcarg) \
  parse_##funcname(funcarg, 0)

#define CALL_PARSER_FUNC_EX(funcname, funcarg, funcex) \
  parse_##funcname(funcarg, funcex)

#define DEF_TOK_ARR(arrname, arrtype)\
  unsigned long * parser_##arrtype##_##arrname = 0;\
  static unsigned long int_##arrtype##_##arrname[] =

#define DEF_START_TOK(arrname)\
  DEF_TOK_ARR(arrname, start)

#define DEF_FOLLOW_TOK(arrname)\
  DEF_TOK_ARR(arrname, folls)

#define DEC_TOK_ARR(arrname, arrtype)\
  extern unsigned long * parser_##arrtype##_##arrname;

#define DEC_START_TOK(arrname)\
  DEC_TOK_ARR(arrname, start)

#define DEC_FOLLOW_TOK(arrname)\
  DEC_TOK_ARR(arrname, folls)

#endif // __PARSER_H__
