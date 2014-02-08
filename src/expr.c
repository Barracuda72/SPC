#include <stdint.h>
#include <expr.h>
#include <rec.h>
#include <lexer.h>
#include <type.h>
#include <syntax.h>
#include <errors.h>
#include <constr.h>

extern int lex_s, lex_line, lex_c;

tree *expr_ident(rec_desc **area)
{
	tree *res = 0;
	int id = rec_search(lex_get_s(), 0, 0, *area);
	if(id == REC_NOTFOUND)
	{
		err_include(ERROR, "unknown identifier", 0, lex_line, lex_c);
		err_include(ENOTE, lex_get_s(), 0, lex_line, lex_c);
		return (tree *)-1;
	} else {
		int r = rec_get_class(id, *area);
		switch(r)
		{
			case REC_PROCEDURE:
				accept(L_IDENT);
				err_include(ERROR, "procedure used as function", 0, lex_line, lex_c);
				return (tree *)-1;

			case REC_FUNCTION:
				return constr_func_call(area);

			case REC_VARIABLE:
				res = tree_var(rec_search(lex_get_s(), 0, 0, *area));
				accept(L_IDENT);
				return res;

			case REC_CONST:
				res = tree_var(rec_search(lex_get_s(), 0, 0, *area));	// Считаем паскалевские константы переменными
				accept(L_IDENT);
				return res;
		}
	}
	return (tree *)-1;
}

/*
	Логическое выражение
	TODO: Исправить!
*/
tree *expr_bool(rec_desc **area)
{
	switch(lex_s)
	{
		case L_TRUE:
			accept(lex_s);
			return tree_logic_expr(1);
		case L_FALSE:
			accept(lex_s);
			return tree_logic_expr(0);
		default:
			err_include(ERROR, "unsupported logic expression", 0, lex_line, lex_c);
			return 0;
	}
}

tree *expr(rec_desc **area)
{
	/*
		Выражение всегда возвращает значение
	*/
	tree *res = -1;
	switch(lex_s)
	{
		case L_IDENT:
			// Функция, константа или переменная
			res = expr_ident(area);
			break;

		case L_FLOAT:
			res = tree_const(lex_get_s(), TREE_FCONST);
			accept(L_FLOAT);
			break;

		case L_NUMERIC:
			res = tree_const(lex_get_s(), TREE_ICONST);
			accept(L_NUMERIC);
			break;

		case L_SCONST:
			res = tree_const(lex_get_s(), TREE_SCONST);
			accept(L_SCONST);
			break;

		case '(':
			accept('(');
			res = expr(area);
			accept(')');
			break;
	}
	return res;
}
