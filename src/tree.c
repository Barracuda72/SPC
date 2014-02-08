#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <rec.h>
#include <tree.h>
#include <type.h>
#include <main.h>

tree *tree_new(void)
{
	return (tree *)malloc(sizeof(tree));
}

tree *tree_var(int id)
{
	tree *res = tree_new();
	res->type = TREE_VAR;
	/*char *str = malloc(strlen(name) + 1);
	strncpy_m(str, name, 1000);
	res->param = (void *)str;*/
	res->id = id;
	res-> left = (tree *)0;
	res->right = (tree *)0;
	return res;
}

tree *tree_const(char *value, int type)
{
	tree *res = tree_new();
	res->type = type;
	char *str = malloc(strlen(value) + 1);
	strncpy_m(str, value, 1000);
	res->param = (void *)str;
	//res->id = id;
	res-> left = (tree *)0;
	res->right = (tree *)0;
	return res;
}

tree *tree_assign(tree *left, tree *right)
{
	tree *res = tree_new();
	res->type = TREE_ASSIGN;
	res->left = left;
	res->right = right;
	res->param = (void *)0;
	return res;
}

tree *tree_func_call(int id, tree *params, rec_desc *area)
{
	tree *res = tree_new();
	res->type = TREE_FUNC_CALL;
	res->left = params;
	res->id = id;
	return res;
}

tree *tree_expr_list(void)
{
	tree *res = tree_new();
	res -> type = TREE_EXPR_LIST;
	res -> param = 0;
	res -> left = 0;
	res -> right = 0;
	return res;
}

tree *tree_proc_call(int id, tree *params, rec_desc *area)
{
	tree *res = tree_new();
	res->type = TREE_PROC_CALL;
	res->param = (void *)params;
	res->id = id;
	return res;
}

tree *tree_func_desc(int id, void *param, tree *body, tree *decl)
{
	tree *res = tree_new();
	res->left = body;
	res->right = decl;
	res->param = param;
	res->id = id;
	res->type = TREE_FUNC_DECL;
	return res;
}

tree *tree_proc_desc(int id, void *param, tree *body, tree *decl)
{
	tree *res = tree_new();
	res->left = body;
	res->right = decl;
	res->param = param;
	res->id = id;
	res->type = TREE_PROC_DECL;
	return res;
}

tree *tree_logic_expr(int x)
{
	tree *res = tree_new();
	res->left = (tree *)x;
	res->right = (tree *)0;
	res->param = 0;
	res->type = TREE_LOGIC_EXPR;
	return res;
}

int tree_get_type(tree *t, rec_desc *area)
{
	int id;
	switch(t->type)
	{
		case TREE_ICONST:
			return TYPE_INTEGER;

		case TREE_FCONST:
			return TYPE_REAL;

		case TREE_SCONST:
			return TYPE_STRING;

		case TREE_FUNC_CALL:
		case TREE_VAR:
			//id = rec_search(t->param, 0, 0, area);
			return rec_get_type(t->id, area);

		default:
			return 0;
	}
}

