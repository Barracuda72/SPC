#include <stdint.h>

#include <lexer.h>
#include <set.h>

typedef struct
{
	int set;
	int elem;
} set_elem;

static set_elem set_s[] =
{
	{DECLS, L_VAR},
	{DECLS, L_TYPE},
	{DECLS, L_CONST},
	{DECLS, L_PROCEDURE},
	{DECLS, L_FUNCTION},

	{OPBEG, L_IF},
	{OPBEG, L_CASE},
	{OPBEG, L_IDENT},
	{OPBEG, L_BEGIN},
	{OPBEG, L_WHILE},
	{OPBEG, L_FOR},
	{OPBEG, L_REPEAT}
};

static int set_cnt = sizeof(set_s)/sizeof(set_elem);

int belong(int elem, int set)
{
	int i;
	for(i = 0; i < set_cnt; i++)
		if(set_s[i].set == set)
			if(set_s[i].elem == elem)
				return 1;
	return 0;
}
