#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <io.h>
#include <bitset.h>
#include <errors.h>
#include <spc_i18n.h>
#include <debug.h>
#include <symtable.h>

#include <syn/parser.h>

#include <syn/program.h>
#include <syn/vardecl.h>
#include <syn/varpack.h>
#include <syn/typedecl.h>
#include <syn/typedefn.h>
#include <syn/block.h>
#include <syn/operator.h>
#include <syn/expr.h>
#include <syn/varuse.h>
#include <syn/constant.h>

extern int lex_line, lex_c;
int syn_line = 0, syn_c = 0;
const char *syn_err = "SYN: Unexpected symbol";
const char *parser_int_error = "SYN : internal compiler error :";

static void parser_ff(void)
{
    syn_line = lex_line;
    syn_c = lex_c;

    io_printf("Fast forward: %d (%s)\n", lex_get_s(), sym_get_sym(lex_get_s()));
    lex_next_s();

    if (syn_line != 1 || syn_c != 1) // Не первый символ
    {
        syn_line = lex_line;
        syn_c = lex_c;
    }
}

void skip1(unsigned long *folls)
{
    while ((lex_get_s() != EOF) && (!bits_belong(lex_get_s(), folls)))
        parser_ff();
}

void skip2(unsigned long *start, unsigned long *folls)
{
    while ((lex_get_s() != EOF) && (!bits_belong(lex_get_s(), start)) && (!bits_belong(lex_get_s(), folls)))
        parser_ff();
}

void *parser_exec(unsigned long *start, unsigned long *folls, unsigned long *ex, parser_func pf, const char *name)
{
    char *tmp;
    int t;
    void *ret = 0;

    if (start == 0)
    {
        err_include(EFATAL, _(parser_int_error), _("Empty list of starting tokens"), syn_line, syn_c);
        return 0;
    } else if(!bits_belong(lex_get_s(), start)) {
        tmp = malloc(1024);
        if (name != NULL)
        {
            sprintf(tmp, _(": got '%s' but %s expected"), sym_get_sym(lex_get_s()), name);
        } else {
            t = bits_first(start);
            sprintf(tmp, _(": got '%s' but should be '%s' or so"), sym_get_sym(lex_get_s()), sym_get_sym(t));
        }
        parser_unexpected(tmp);
        skip2(start, folls);
    }

    if(bits_belong(lex_get_s(), start))
    {
        ret = pf(folls, ex);
        if((folls != 0) && !bits_belong(lex_get_s(), folls))
        {
            tmp = malloc(1024);

            // Сначала ищем в множестве корректно ожидаемых символов
            t = bits_first(ex);

            // Если не нашли - то в множестве тех,
            // которые в принципе могут следовать за данной конструкцией
            if (t == L_INVALID)
                t = bits_first(folls);

            sprintf(tmp, _(": got '%s' but should be '%s' or so"), sym_get_sym(lex_get_s()), sym_get_sym(t));
            parser_unexpected(tmp);
            skip1(folls);
        }
    }

    return ret;
}

void accept(TOKEN_TYPE t)
{
    //char tmp[2];
    io_printf("Req %d (%s), got %d (%s) - %s\n",
        t, sym_get_sym(t),
        lex_get_s(), sym_get_sym(lex_get_s()),
        (t == lex_get_s())?"MATCH":"MISS" );
    if(lex_get_s() == t)
    {
        parser_ff();
    } else {
        char *tmp = malloc(1024);
        sprintf(tmp, _(": got '%s' but expected '%s'"), sym_get_sym(lex_get_s()), sym_get_sym(t));
        parser_unexpected(tmp);
    }
}

void init_parser(void)
{
    CALL_INIT_FUNC(program);
    CALL_INIT_FUNC(typedecl);
    CALL_INIT_FUNC(typedefn);
    CALL_INIT_FUNC(vardecl);
    CALL_INIT_FUNC(varpack);
    CALL_INIT_FUNC(varuse);
    CALL_INIT_FUNC(expr);
    CALL_INIT_FUNC(block);
    CALL_INIT_FUNC(constant);
    CALL_INIT_FUNC(g_operator);
}

void fini_parser(void)
{
    CALL_FINI_FUNC(program);
    CALL_FINI_FUNC(typedecl);
    CALL_FINI_FUNC(typedefn);
    CALL_FINI_FUNC(vardecl);
    CALL_FINI_FUNC(varpack);
    CALL_FINI_FUNC(varuse);
    CALL_FINI_FUNC(expr);
    CALL_FINI_FUNC(block);
    CALL_FINI_FUNC(constant);
    CALL_FINI_FUNC(g_operator);
}

void parser_unexpected(const char *extmsg)
{
    err_include(ERROR, _(syn_err), extmsg, syn_line, syn_c);
}
