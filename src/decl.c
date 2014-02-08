#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <decl.h>
#include <syntax.h>
#include <lexer.h>
#include <errors.h>
#include <set.h>
#include <rec.h>
#include <type.h>
#include <tree.h>
#include <main.h>

#include <debug.h>

extern int lex_line, lex_c, lex_s;

int decl_var_type(void)
{
	switch (lex_s)
	{
		case L_INTEGER:
			accept(L_INTEGER);
			return TYPE_INTEGER;
		case L_FILE:
			accept(L_FILE);
			return TYPE_FILE;
		case L_BYTE:
			accept(L_BYTE);
			return TYPE_BYTE;
		case L_SHORT:
			accept(L_SHORT);
			return TYPE_SHORT;
		case L_STRING:
			accept(L_STRING);
			return TYPE_STRING;
		case L_TEXT:
			accept(L_TEXT);
			return TYPE_TEXT;
		case L_REAL:
			accept(L_REAL);
			return TYPE_REAL;
		default:
			err_include(ERROR, "unsupported type", 0, lex_line, lex_c);
			return -1;
	}
}

int decl_var_pack(rec_desc **area, uint32_t isarg)
{
	uint32_t tmp[256];
	int i,j;
	i = 0;

	if(lex_s == L_IDENT)
	{
		tmp[i++] = rec_put(lex_get_s(), REC_VARIABLE, 0, area);
		accept(L_IDENT);
	}
	while(lex_s == ',')
	{
		accept(',');
		if(lex_s == L_IDENT)
		{
			tmp[i++] = rec_put(lex_get_s(), REC_VARIABLE, 0, area);
			accept(L_IDENT);
		}
	}
	accept(':');
	int t = decl_var_type();
	for(j = 0; j < i; j++)
	{
		//printf("tmp[%d] = %d\n", i, tmp[i]);
		rec_set_type(tmp[j], t, *area);
		rec_set_param(tmp[j], (void *)isarg, *area);
	}
	return i;
}

tree *decl_var(rec_desc **area)
{
	accept(L_VAR);
	decl_var_pack(area, REC_NOARG);
	while(lex_s == ';')
	{
		accept(';');
		if(lex_s == L_IDENT)
			decl_var_pack(area, REC_NOARG);
	}
	return 0;
}

tree *decl_type(void)
{
	accept(L_TYPE);
	while(lex_s == L_IDENT)
	{
		accept(L_IDENT);
		accept('=');
		decl_var_type();
		accept(';');
	}
	return 0;
}

tree *decl_const(rec_desc **area)
{
	accept(L_CONST);
	char *data;
	while(lex_s == L_IDENT)
	{
		int id = rec_put(lex_get_s(), REC_CONST, 0, area);
		accept(L_IDENT);
		accept('=');

		data = (char *)malloc(strlen(lex_get_s()) + 1);
		//printf("Constant len is %d\n", strlen(lex_get_s()));
		strncpy_m(data, lex_get_s(), 1000);

		switch(lex_s)
		{
			case L_NUMERIC:
				accept(L_NUMERIC);
				rec_set_type(id, TYPE_INTEGER, *area);
				break;

			case L_FLOAT:
				accept(L_FLOAT);
				rec_set_type(id, TYPE_REAL, *area);
				break;

			case L_SCONST:
				accept(L_SCONST);
				rec_set_type(id, TYPE_STRING, *area);
				break;
		}
		rec_set_param(id, data, *area);
		accept(';');
	}
	return 0;
}

int decl_params(rec_desc **area)
{
	accept('(');
	int res = 0;
	int type;
	while (lex_s != ')')
	{
		if(lex_s == L_VAR)
		{
			accept(L_VAR);
			type = REC_VARARG;
		} else {
			type = REC_ARGUMENT;
		}
		res += decl_var_pack(area, type);
		if(lex_s != ')')
			while(lex_s == ';')
				accept(';');
	}
	accept(')');
	return res;
}

tree *decl_func(rec_desc **area)
{
	accept(L_FUNCTION);
	rec_func_param *par = malloc(sizeof(rec_func_param));
	par -> argcnt = 0;
	par -> area = *area;
	int id = -1;
	char *name; 
	if(lex_s == L_IDENT)
	{
		//name = strncpy_m(malloc(strlen(lex_get_s()) + 1), lex_get_s(), 1000);
		id = rec_put(lex_get_s(), REC_FUNCTION, 0, area);
		rec_set_param(id, (void *)par, *area);
	}
	rec_level_push();
	accept(L_IDENT);

	if(lex_s == '(')	// Начало параметров
		par->argcnt = decl_params(&(par -> area));
	else
		par->argcnt = 0;

	//DBG("argcnt for %d is %d\n", id, par->argcnt);

	accept(':');
	rec_set_type(id, decl_var_type(), *area);
	accept(';');
	tree *decls = 0;
	if(belong(lex_s, DECLS))
	{
		decls = decl(&(par -> area));
	}
	tree *body = operator(&(par -> area));
	tree *res = tree_func_desc(id, rec_get(id, *area), body, decls);
	accept(';');
	rec_level_pop();
	return res;
}

tree *decl_proc(rec_desc **area)
{
	accept(L_PROCEDURE);
	rec_func_param *par = malloc(sizeof(rec_func_param));
	par -> area = *area;
	par -> argcnt = 0;
	int id = -1;
	char *name; 
	if(lex_s == L_IDENT)
	{
		/*name = strncpy_m(malloc(strlen(lex_get_s()) + 1), lex_get_s(), 1000);
		par->name = name;*/
		id = rec_put(lex_get_s(), REC_PROCEDURE, 0, area);
		rec_set_param(id, (void *)par, *area);
	}
	rec_level_push();
	accept(L_IDENT);

	if(lex_s == '(')	// Начало параметров
		par->argcnt = decl_params(&(par -> area));
	else
		par->argcnt = 0;

	accept(';');
	tree *decls = 0;
	if(belong(lex_s, DECLS))
	{
		decls = decl(&(par -> area));
	}
	tree *body = operator(&(par -> area));
	tree *res = tree_func_desc(id, rec_get(id, *area), body, decls);
	accept(';');
	rec_level_pop();
	return res;
}

tree *decl(rec_desc **area)
{
	tree *res = tree_new();
	res -> type = TREE_DECLARATION;
	res -> left = 0;
	res -> right = 0;
	// Левый - собственно определение
	// Правый - продолжение списка
	tree *new = res;
	while(belong(lex_s, DECLS))
	{
		switch(lex_s)
		{
			case L_VAR:
				new->left = decl_var(area);
				break;
			case L_TYPE:
				new->left = decl_type();
				break;
			case L_CONST:
				new->left = decl_const(area);
				break;
			case L_FUNCTION:
				new->left = decl_func(area);
				break;
			case L_PROCEDURE:
				new->left = decl_proc(area);
				break;
			default:
				err_include(ERROR, "expected declaration", 0, lex_line, lex_c);
				continue;	//!!!
		}
		new -> right = tree_new();
		new -> right -> type = TREE_DECLARATION;
		new -> right -> left = 0;
		new -> right -> right = 0;
		new = new -> right;
	}
	return res;
}
