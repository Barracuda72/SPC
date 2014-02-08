#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <lexer.h>
#include <syntax.h>
#include <errors.h>
#include <symtable.h>
#include <decl.h>
#include <set.h>
#include <constr.h>
#include <rec.h>
#include <tree.h>
#include <type.h>
#include <expr.h>

#include <debug.h>

extern int lex_line, lex_c, lex_s;
extern uint32_t rec_level;
rec_desc *global = 0;
tree *prog = 0;

void accept(int s)
{
	if(s != lex_s)
	{
		err_include(ERROR, "SYN : expecting symbol", sym_get_sym(s), lex_line, lex_c);
		err_include(ENOTE, "SYN : but got", sym_get_sym(lex_s), 0, 0);
		/*
			Восстановление в режиме "паники"
		*/
	}
	lex_next_s();
}

void header()
{
	accept(L_PROGRAM);
	accept(L_IDENT);
	accept(';');
}

tree *operator(rec_desc **area)
{
	tree *tres = 0;
	if(lex_s == L_IDENT)
	{
		int id = rec_search(lex_get_s(), 0, 0, *area);

		if (id == REC_NOTFOUND)
		{
			err_include(ERROR, "unknown identifier", 0, lex_line, lex_c);
			// Неизвестный идентификатор - что можно сделать?
			while((lex_s != ';')&&(lex_s != L_END)&&(lex_s != EOF))
			{
				accept(lex_s);
				//printf("1");
			}
			accept(lex_s);
			return operator(area);
		}

		int res = rec_get_class(id, *area);
		int t1, t2;
		switch(res)
		{
			case REC_PROCEDURE:
				// Это процедура
				tres = constr_proc_call(area);
				break;

			case REC_CONST:
				err_include(ERROR, "attempt to modify constant expression", 0,
					lex_line, lex_c);
			case REC_VARIABLE:
				t1 = rec_get_type(id, *area);
				tree *var = tree_var(rec_search(lex_get_s(),0,0,*area));
				accept(L_IDENT);
				// Присваивание значения переменной
				accept(L_ASSIGN);
				tree *e = expr(area);
				t2 = tree_get_type(e, *area);
				if(t1 != t2)
				{
					err_include(ERROR, "type mismatch", 0, lex_line, lex_c);
				}
				tres = tree_assign(var, e);
				break;
			default:
				tres = 0;
				DBG("BOO: %d\n", res);
				break;
		}
	} else {
		switch(lex_s)
		{
			case L_BEGIN:
				accept(L_BEGIN);
				tres = operators(area);
				accept(L_END);
				break;
			case L_IF:
				tres = constr_if(area);
				break;
			case L_WHILE:
				tres = constr_while(area);
				break;
			case L_REPEAT:
				tres = constr_repeat(area);
				break;
			default:
				err_include(ERROR, "unknown operator", 0, lex_line, lex_c);
				break;
		}
	}
	tree *newres = tree_new();
	newres->type = TREE_OPERATOR;
	newres->left = (tree *)0;
	newres->right = tres;
	newres->param = (void *)0;
	return newres;
}

tree *operators(rec_desc **area)
{
	/*
		Оператор может быть:
		Присваиванием - сначала идет идентификатор, затем присваивание
		Вызовом процедуры - идентификатор, круглая скобка
		Составным оператором - begin
	*/
	tree *res = 0;
	tree *tmp, *t;
	while (belong(lex_s, OPBEG))
	{
		t = operator(area);
//		printf("res = %d\n", res);
		if(res == 0)
		{
			res = t;
			tmp = t;
		} else {
			tmp->left = t;
			tmp = t;
		}
		// Если это не окончание составного оператора, то тут обязан стоять разделитель
		if (lex_s != L_END)
			accept(';');

		while (lex_s == ';')
			accept(';');

		if (lex_s == L_END)	// Блок операторов, переданный нам, кончился
			break;
	}
	return res;
}

tree *program()
{
	rec_level = 0;
	lex_next_s();
	if (lex_s == L_PROGRAM)
		header();

	int id = rec_put("main", REC_PROCEDURE, 0, &global);

	rec_func_param *par = malloc(sizeof(rec_func_param));
	par -> area = &global;
	//par -> name = "main";
	par -> argcnt = 0;

	//DBG("par = %x\n", par);

	rec_set_param(id, par, global);

	tree *decls = decl(&global);
	accept(L_BEGIN);
	tree *body = operators(&global);
	//accept(CASE);
	accept(L_END);
	accept('.');
	tree *res = tree_proc_desc(id, rec_get(id, global), body, decls);
	return res;
}
