#include <errors.h>

#include <syn/varuse.h>
#include <syn/expr.h>

#include <sem/id_table.h>
#include <sem/type_table.h>

#include <spc_i18n.h>

#include <debug.h>

DEF_START_TOK(varuse)
{
    L_IDENT,
    L_INVALID
};

DEF_FOLLOW_TOK(varuse)
{
    L_INVALID
};

DEF_TOK_ARR(aux, varuse)
{
    L_RBRACKET,
    L_COMMA,
    L_INVALID
};

DEF_PARSER_FUNC(varuse)
{
    unsigned long *tmp = balloc();
    bits_disjunct(folls, parser_varuse_aux, tmp);

    id_table_elem *desc = 0;
    type_table_elem *res = 0;
    lex_val lv = lex_get_v();
    char *name = strdup(lv.string);
    if (lex_get_s() == L_IDENT)
    {
        unsigned long uc = UC_UNK;
        desc = search_id_table(0, lv.string, &uc);
        if (desc == 0)
        {
            err_include(ERROR, _("undeclared identifier:"), strdup(lv.string), syn_line, syn_c);
            desc = add_to_id_table(0, strdup(lv.string), UC_VAR);
        } else if ((uc != UC_VAR) && (uc != UC_CONST))
        {
            err_include(ERROR, _("not a variable:"), name, syn_line, syn_c);
            desc = 0;
        } else {
            res = desc->type;
        }
    }
    accept(L_IDENT);

    // Чтобы не вводить дополнительную сущность для разбора массива

    char *err;
    int i = 0; // Флаг индексов массива
    type_table_elem *et = 0, *idx = 0;
    list *indexes;
    while (lex_get_s() == L_LBRACKET)
    {
        i = 0;
        et = 0;
        indexes = 0;

        if ((res != 0) && (res->utype != UT_ARRAY))
        {
            err_include(ERROR, _("not an array:"), name, syn_line, syn_c);
            res = 0;
        }

        if ((res != 0) && (res->utype == UT_ARRAY))
            indexes = res->indexes;

        accept(L_LBRACKET);
        // Выражение
        et = CALL_PARSER_FUNC(expr, tmp);
        if (indexes != 0)
        {
            idx = indexes -> value;
            indexes = indexes->next;
        }
        if (!can_assign(idx, et))
        {
            err = malloc(1024);
            sprintf(err, _("'%s', expected '%s'"), get_type_name(et), get_type_name(idx));
            err_include(ERROR, _("illegal array index type:"), err, syn_line, syn_c);
        }
        //DBG("%x, %x\n", idx, et);
        // Здесь пропустим проверку количества индексов, ибо минимум один
        // индекс в описании типа должен быть, а если при использовании его не указали -
        // ошибка будет на этапе разбора выражения
        while (lex_get_s() == L_COMMA)
        {
            int sl = syn_line, sc = syn_c;
            accept(L_COMMA);
            int sl1 = syn_line, sc1 = syn_c;
            et = CALL_PARSER_FUNC(expr, tmp);
            if (indexes != 0)
            {
                idx = indexes -> value;
                indexes = indexes->next;
            } else if (i == 0) {
                // Индексы кончились, и мы об этом еще не сообщали
                err_include(ERROR, _("too many indexes for this array type"), 0, sl, sc);
                i = 1;
                idx = 0;
            }
            if (!can_assign(idx, et))
            {
                err = malloc(1024);
                sprintf(err, _("'%s', expected '%s'"), get_type_name(et), get_type_name(idx));
                err_include(ERROR, _("illegal array index type:"), err, syn_line, syn_c);
            }
        }
        if (indexes != 0)
            err_include (ERROR, _("too few indexes for this array type"), 0, syn_line, syn_c);
        accept(L_RBRACKET);

        // Сдвинем тип
        if (res != 0)
            res = res->base;
    }
    free(tmp);

    return res;
}

INIT_PARSER_FUNC(varuse)
{
    INIT_PARSER_DATA(varuse);
    INIT_PARSER_ARRAY(aux, varuse);
}

FINI_PARSER_FUNC(varuse)
{
    FINI_PARSER_DATA(varuse);
    FINI_PARSER_ARRAY(aux, varuse);
}
