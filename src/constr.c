#include <stdint.h>

#include <constr.h>
#include <syntax.h>
#include <lexer.h>
#include <errors.h>
#include <set.h>
#include <rec.h>
#include <expr.h>
#include <tree.h>

extern int lex_s, lex_line, lex_c;
/*
	Разнообразные конструкции
*/

/*
	Условный оператор
*/

tree *constr_if(rec_desc **area)
{
	accept(L_IF);
	expr_bool(area);
	accept(L_THEN);
	operator(area);
	if(lex_s == L_ELSE)
	{
		accept(L_ELSE);
		operator(area);
	}
	return 0;
}

/*
	Цикл while
*/

tree *constr_while(rec_desc **area)
{
	accept(L_WHILE);
	expr_bool(area);
	accept(L_DO);
	operator(area);
	return 0;
}

/*
	Цикл repeat
*/
tree *constr_repeat(rec_desc **area)
{
	accept(L_REPEAT);
	operators(area);
	accept(L_UNTIL);
	expr_bool(area);
	return 0;
}

/*
	Вызов функции
*/
tree *constr_func_call(rec_desc **area)
{
	tree *tres, *t, *tmp;
	tree *arglist = tree_expr_list();

	int res = rec_search(lex_get_s(), REC_FUNCTION, 0, *area);
	if (res == REC_NOTFOUND)
	{
		err_include(ERROR, "unknown function", 0, lex_line, lex_c);
	}
	accept(L_IDENT);
	rec_func_param *p = rec_get_param(res, *area);
	int cnt = 0;
	if(p != 0)
	{
		cnt = p -> argcnt;
	}
	if(lex_s == '(')
	{
		// Параметры могут быть:
		// Константой
		// Переменной - ее нужно найти в текущей области видимости
		// Выражением - его нужно вычислить
		accept('(');
		t = tree_expr_list();
		arglist -> left = t;
		arglist -> right = expr(area);
		tmp = arglist;
		cnt--;
		
		while(lex_s == ',')
		{
			accept(',');
			t -> left = tree_expr_list();
			t -> right = expr(area);
			tmp = t;
			t = t->left;
			cnt--;
		}
		accept(')');
		free(tmp -> left);
		tmp -> left = 0;
		if(cnt != 0)
			err_include(ERROR, "Wrong number of arguments passed to call", 0, lex_line, lex_c);
	}
	tres = tree_func_call(res, arglist, *area);
	return tres;//rec_get_type(res, *area);
}

/*
	Вызов процедуры 
*/
tree *constr_proc_call(rec_desc **area)
{
	tree *tres, *t, *tmp;
	tree *arglist = tree_expr_list();

	int res = rec_search(lex_get_s(), REC_PROCEDURE, 0, *area);
	if (res == REC_NOTFOUND)
	{
		err_include(ERROR, "unknown procedure", 0, lex_line, lex_c);
	}
	accept(L_IDENT);
	rec_func_param *p = rec_get_param(res, *area);
	int cnt = 0;
	if(p != 0)
	{
		cnt = p -> argcnt;
	}
	if(lex_s == '(')
	{
		// Параметры могут быть:
		// Константой
		// Переменной - ее нужно найти в текущей области видимости
		// Выражением - его нужно вычислить
		accept('(');
		t = tree_expr_list();
		arglist -> left = t;
		arglist -> right = expr(area);
		tmp = arglist;
		cnt--;
		
		while(lex_s == ',')
		{
			accept(',');
			t -> left = tree_expr_list();
			t -> right = expr(area);
			tmp = t;
			t = t->left;
			cnt--;
		}
		accept(')');
		free(tmp -> left);
		tmp -> left = 0;
		if(cnt != 0)
			err_include(ERROR, "Wrong number of arguments passed to call", 0, lex_line, lex_c);
	}
	tres = tree_proc_call(res, arglist, *area);
	return tres;//rec_get_type(res, *area);
}
