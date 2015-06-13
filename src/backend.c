#include <stdint.h>

#include <backend.h>
#include <tree.h>
#include <rec.h>
#include <io.h>
#include <type.h>

#include <stdlib.h>
#include <openssl/md5.h>
#include <string.h>

#include <debug.h>

static void b_func_begin(int id, rec_func_param *p);
static void b_func_end(int id);
static void b_get_var_addr(int id, rec_desc *area);
static void b_get_var_value(int id, rec_desc *area);
static void b_calc_tree(tree *t);
static void b_iconst(int value);
static void b_fconst(float value);
static void b_assign(int id, rec_desc *area);
static void b_data_tree(tree *t);
static void b_func_call(int id);

static tree *t_data = 0;

void backend_print_tree(tree *t)
{
  // Вывод дерева в формате GraphViz
}

void _backend_print_tree(tree *t)
{
	t_data = t;
	//	Генерируем секцию .text
	io_printf(".text\n");
	b_calc_tree(t);
	//	Генерируем секцию .data
	io_printf(".data\n");
	b_data_tree(t);
	// HACK
	io_printf("const_format:\n.string \"%%d\"\n");
	io_printf(".long 0");
}

static rec_desc *r;
static rec_func_param *rd;

static void b_calc_tree(tree *t)
{
	if (t != 0)
	{
		switch (t -> type)
		{
			case TREE_FUNC_DECL:
			case TREE_PROC_DECL:
				rd = 0;
				if(t->param != 0)
				{
					r = (rec_desc *)(t->param);
					//DBG("r = %x\n", r);
					rd = (rec_func_param *)(r->param);
					//DBG("rd = %x\n", rd );
					DBG("Function %d with %x args\n", t->id, rd->argcnt);
				} else {
					DBG("Can't resolve function\n");
				}
				b_calc_tree(t->right);

				b_func_begin(t->id, rd);
				b_calc_tree(t->left);
				b_func_end(t->id);
				//DBG("End function %d\n", t->id);
				break;

			case TREE_DECLARATION:
				//DBG("Declaration\n");
				b_calc_tree(t->left);
				b_calc_tree(t->right);
				break;

			case TREE_OPERATOR:
				//DBG("Operator\n");
				b_calc_tree(t->right);
				b_calc_tree(t->left);
				break;

			case TREE_ASSIGN:
				//DBG("Assign\n");
				b_calc_tree(t->right);
				//DBG("to\n");
				//b_calc_tree(t->left);
				b_assign(t->left->id, rd->area);
				break;

			case TREE_VAR:
				//DBG("Variable %d\n", t->id);
				b_get_var_value(t->id, rd->area);
				break;

			case TREE_SCONST:
				//DBG("String\n");
				//io_printf("mov $const_%d, %%eax\n", t->id);
				break;

			case TREE_FCONST:
				//DBG("Float\n");
				b_fconst((float)atof(t->param));
				break;

			case TREE_ICONST:
				//DBG("Integer ");
				//DBG("%d\n", atoi(t->param));
				b_iconst(atoi(t->param));
				break;

			case TREE_PROC_CALL:
			case TREE_FUNC_CALL:
				DBG("Call to %d\n", t->id);
				b_calc_tree(t->left);
				b_func_call(t->id);
				break;

			case TREE_EXPR_LIST:
				DBG("Function parameters\n");
				b_calc_tree(t->left);
				io_printf("push %%eax\n");
				b_calc_tree(t->right);
				break;

			default:
				DBG("Unknown type %d\n", t->type);
				break;
		}
		//DBG("\n");
	}
}

static void b_const_write(rec_desc *c)
{
	switch(c->type)
	{
		case TYPE_STRING:
			io_printf("const_%d:\n.string \"%s\"\n", c->id, c->param);
			break;

		case TYPE_INTEGER:
			io_printf("const_%d:\n.long %s\n", c->id, c->param);
			break;

		case TYPE_REAL:
			io_printf("const_%d:\n.float %s\n", c->id, c->param);
			break;
	}
	free(c->param);
	c->param = 0;
}

static void b_data_tree(tree *t)
{

	if (t != 0)
	{
		switch (t -> type)
		{
			case TREE_FUNC_DECL:
			case TREE_PROC_DECL:
				rd = 0;
				if(t->param != 0)
				{
					r = (rec_desc *)(t->param);
					//DBG("r = %x\n", r);
					rd = (rec_func_param *)(r->param);
					rec_desc *area = rd->area;
					while((area != 0) && (r->level <= area->level))
					{
						if((area->class == REC_CONST) &&
							(area->param != 0))
						{
							b_const_write(area);
						}
						area = area -> next;
					}
					//DBG("rd = %x\n", rd );
					DBG("Function %d with %x args\n", t->id, rd->argcnt);
				}
				b_data_tree(t->right);
				//b_func_begin(t->id, rd);
				//b_data_tree(t->left);
				//b_func_end(t->id);
				//DBG("End function %d\n", t->id);
				break;

			case TREE_DECLARATION:
				//DBG("Declaration\n");
				b_data_tree(t->left);
				b_data_tree(t->right);
				break;

			case TREE_OPERATOR:
				//DBG("Operator\n");
				//b_data_tree(t->right);
				//b_data_tree(t->left);
				break;

			case TREE_ASSIGN:
				//DBG("Assign\n");
				//b_calc_tree(t->right);
				//DBG("to\n");
				//b_calc_tree(t->left);
				//b_assign(t->left->id, rd->area);
				break;

			case TREE_VAR:
				//DBG("Variable %d\n", t->id);
				break;

			case TREE_SCONST:
				//DBG("String\n");
				io_printf(".string const_%d, \"%s\"\n", t->id, t->param);
				break;

			case TREE_FCONST:
				//DBG("Float\n");
				//b_fconst((float)atof(t->param));
				break;

			case TREE_ICONST:
				//DBG("Integer ");
				//DBG("%d\n", atoi(t->param));
				//b_iconst(atoi(t->param));
				break;

			default:
				DBG("Unknown type %d\n", t->type);
				break;
		}
		//DBG("\n");
	}
}

static uint32_t sp = 0;	// Указатель стека
//static uint8_t code_started = 0;


static void b_get_var_value(int id, rec_desc *area)
{
	// Помещает в eax значение переменной
	// Все переменные размещаются в стеке

	b_get_var_addr(id, area);
	io_printf("mov (,%%eax,), %%eax\n");
}

static int b_get_var_size(int id, rec_desc *area)
{
	rec_desc *p = rec_get(id, rd->area);
	//rec_desc *p = rec_get(id, area);
	if(p -> param == REC_VARARG)
		return 4;

	switch(p -> type)
	{
		case TYPE_INTEGER:
		case TYPE_WORD:
		case TYPE_BYTE:
		case TYPE_SHORT:
		case TYPE_REAL:
			return 4;
		case TYPE_STRING:
			return 256;
		default:
			DBG("Error in b_get_var_size, type %d\n", p->type);
			return 0;
	}
}

static void b_get_var_addr(int id, rec_desc *area)
{
	int res = 4;
	//rec_desc *p = rec_get(fid, area);
	int c = rec_get_class(id, area);
	DBG("Class %d\n", c);
	if(c == REC_CONST)
	{
		io_printf("mov $const_%d, %%eax\n", id);
	} else {
		while(area->id != id)
		{
			if(area->class != REC_CONST)
			{
				res += b_get_var_size(area->id, area);
			}
			area = area -> next;
		}
		io_printf("mov %%ebp, %%eax\n");
		io_printf("sub $%d, %%eax\n", res);
	}
}

static int b_get_local_size(int fid, rec_desc *area)
{
	int res = 0;
	area = rd->area;
	//rec_desc *p = rec_get(fid, area);
	while((r->level < area->level) && (area->param != REC_VARARG) 
		&& (area->param != REC_ARGUMENT))
	{
		res += b_get_var_size(area->id, area);
		area = area -> next;
	}
	return res;
}

static void b_func_begin(int id, rec_func_param *p)
{
	// Генерирует пролог функции
	if(id == 0)
	{/*
		io_printf(".globl _start\n");
		io_printf(".type _start, @function\n");
		io_printf("_start:\n");*/
		io_printf(".globl main\n");
		io_printf(".type main, @function\n");
		io_printf("main:\n");
	} else {
		io_printf(".type func_%d, @function\n", id);
		io_printf("func_%d:\n", id);
	}
	// Сначала сохраняем значение esp в ebp
	// Это позволит обращаться к параметрам и переменным функции проще
	io_printf("push %%ebp\n");
	io_printf("mov %%esp, %%ebp\n");
	io_printf("sub $%d, %%esp\n", b_get_local_size(id, p->area));
}

static void b_func_end(int id)
{
	// Генерирует эпилог функции
	// Возвращаем все на место
	io_printf("leave\n");
	if(id == 0)
	{
		io_printf("mov $1, %%eax\n");
		io_printf("int $0x80\n");
	} else {
		io_printf("ret\t/* func_%d */\n\n", id);
	}
}

static void b_assign(int id, rec_desc *area)
{
	// Генерирует инструкцию присваивания
	// В eax лежит значение
	io_printf("push %%eax\n");
	sp++;
	b_get_var_addr(id, area);
	// Теперь в eax - адрес ячейки памяти, а на вершине стека - значение
	io_printf("pop (,%%eax,)\n");
	sp--;
}

static void b_iconst(int value)
{
	io_printf("mov $%d, %%eax\n", value);
}

static void b_fconst(float value)
{
	io_printf("mov $%d, %%eax\n", *(int *)(&value));
}

static void b_func_call(int id)
{
	// HACK
	rec_desc *p = rec_get(id, rd->area);
	char data[16];
	MD5("debug_w", 7, &data);
	if(!memcmp(data, p->data, 16))
	{
		io_printf("push $const_format\n");
		io_printf("call printf\n");
	} else {
		io_printf("call func_%d\n", id);
	}
}

