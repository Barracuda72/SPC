#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <io.h>
#include <lexer.h>
#include <errors.h>
#include <symtable.h>

extern int io_line, io_c;

static char lex_buff[1024];
static int lex_pos;
static char pc = 0;	// Предыдущий символ

int lex_s = 0;
int lex_line = 0, lex_c = 0;

char *lex_get_s(void)
{
	return lex_buff;
}

static int _lex_next_s(void)
{
	char c, tc;
	memset(lex_buff, 0, 1024);
	lex_pos = 0;
	while(1)
	{
		if(pc != 0) 
		{
			c = pc;
			pc = 0;
		} else {
			c = io_next_c();
		}

		if(c == EOF)
			return c;

		if(isalpha(c) || c == '_')
		{
			do
			{
				lex_buff[lex_pos++] = tolower(c);
				c = io_next_c();
			} while (c == '_' || isalnum(c));
			pc = c;
			return sym_get_code(lex_buff);
		}
/*
		if((isalnum(c) || c == '_'  ))
		{
			lex_buff[lex_pos++] = tolower(c);
			continue;
		} else if (lex_pos != 0) {
			pc = c;
			return sym_get_code(lex_buff);//lex_get_code(lex_buff);
		}
*/
		if(isdigit(c))
		{
			do
			{
				lex_buff[lex_pos++] = tolower(c);
				c = io_next_c();
			} while ((c == '.') || isdigit(c) || (tolower(c) == 'e') || (c == '+') 
				|| (c == '-'));
			pc = c;
			return sym_get_code(lex_buff);
		}

		switch(c)	// Let's the battle begin =)
		{
			/* Однолитеральные символы */
			case ',':
			case ';':
			case '^':
			case '[':
			case ']':
			case '+':
			case '-':
			case ')':
			case '=':
				return c;
				break;
			case '\'':
				while((c = io_next_c()) != '\'')
				{
					lex_buff[lex_pos++] = c;
				}
				return L_SCONST;
			case '{':
				// Комментарий
				while(io_next_c() != '}');
				//io_next_c();
				continue;
				break;
			/* Возможное начало двулитерального символа */
			case '/':
				tc = io_next_c();
				if(tc == '/')
					while(io_next_c() != '\n');
				else
				{
					pc = tc;
					return c;
				}
				break;
			case '*':
				tc = io_next_c();
				if(tc == ')')
					return L_RCOMMENT;
				else
				{
					pc = tc;
					return c;
				}
				break;
			case '(':
				tc = io_next_c();
				if(tc == '*')
					return L_LCOMMENT;
				else
				{
					pc = tc;
					return c;
				}
				break;
			case '<':
				tc = io_next_c();
				if(tc == '=')
					return L_LEQUAL;
				else if(tc == '>')
					return L_NEQUAL;
				else
				{
					pc = tc;
					return c;
				}
				break;
			case '>':
				tc = io_next_c();
				if(tc == '=')
					return L_GEQUAL;
				else
				{
					pc = tc;
					return c;
				}
				break;
			case ':':
				tc = io_next_c();
				if(tc == '=')
					return L_ASSIGN;
				else
				{
					pc = tc;
					return c;
				}
				break;
			case '.':
				tc = io_next_c();
				if(tc == '.')
					return L_TWOPOINTS;
				else
				{
					pc = tc;
					return c;
				}
				break;
			case '\n':
				lex_line = io_line;
				lex_c = 0;
			case ' ':
			case '\t':
				lex_c++;
			case '\r':
				break;
			default:
				err_include(ERROR, "LEX : unknown symbol", 0, io_line, io_c);
				break;
		}
	}
}

int lex_next_s(void)
{
	int tmp = lex_s;
	lex_c = io_c;
	lex_line = io_line;
	lex_s = _lex_next_s();
	return tmp;
}
