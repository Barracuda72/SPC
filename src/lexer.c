/*
    Лексический анализатор
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <spc_i18n.h>

#include <io.h>
#include <lexer.h>
#include <errors.h>
#include <symtable.h>
#include <debug.h>

#define LEX_BUF_SIZE 1024

//extern int io_line, io_c;

static char lex_buff[LEX_BUF_SIZE];
static int lex_pos;
static lex_val lex_v; // Значение текущего символа

static char pc = 0;	// Предыдущий символ
//static int err_flag = 0; // Флаг генерации ошибки, 0 = генерировать
static int two_point_flag = 0; // Флаг двойной точки

int lex_s = 0;
int lex_line = 0, lex_c = 0; // Начало текущего символа
static int lex_line_c = 1, lex_char_c = 1;

static void lex_step_c(int c)
{
    if (c == '\n')
    {
        lex_line_c++;
        lex_char_c = 1;
    } else {
        lex_char_c++;
    }
}

static void lex_back_c()
{
    lex_char_c--;
    if (lex_char_c <= 0)
    {
        lex_char_c = 1;
        lex_line_c--;
    }
}

static int lex_next_c()
{
    int ret = io_next_c();
    lex_step_c(ret);
    return ret;
}

lex_val lex_get_v(void)
{
	return lex_v;
}

TOKEN_TYPE lex_get_s(void)
{
    if (lex_s == 0)
      lex_next_s();

    return lex_s;
}

static TOKEN_TYPE _lex_next_s(void)
{
	char c = 0, tc = 0;
	int t = 0;
	int point_flag = 0;

	memset(lex_buff, 0, LEX_BUF_SIZE);
	lex_pos = 0;

	if (two_point_flag)
	{
        two_point_flag = 0;
        return L_TWOPOINTS;
	}

	while(c != EOF)
	{
		if(pc != 0)
		{
			c = pc;
			pc = 0;
			lex_step_c(c);
		} else {
			c = lex_next_c();
			//lex_char_c++;
		}

		if(c == EOF)
			return c;

        /*if (isspace(c))
        {
            lex_line = io_line;
            lex_c = io_c;
        }*/

        // Возможно, начался идентификатор
		if(isalpha(c) || c == '_')
		{
			do
			{
                if(lex_pos < LEX_BUF_SIZE-1)
                {
                    lex_buff[lex_pos++] = tolower(c);
                }
				c = lex_next_c();
			} while (c == '_' || isalnum(c));
			pc = c; lex_back_c();
			lex_buff[lex_pos] = 0;
			lex_v.string = lex_buff;

			if (lex_pos == LEX_BUF_SIZE -1)
                err_include(ERROR, _("LEX : identifier too long"), 0, lex_line, lex_c);

			return sym_get_code(lex_buff);
		}

        // Возможно, началось число
		if(isdigit(c) /*|| c == '+' || c == '-'*/)
		{
			do
			{
                if(lex_pos < LEX_BUF_SIZE-1)
                {
                    // Функции преобразования вещественных чисел в русской локали ожидают увидеть запятую вместо точки
                    if (c == '.')
                    {
                        if (point_flag)
                        {
                            //io_ungetc(c);
                            //pc = c;
                            c = 0;
                            lex_pos--;
                            point_flag = 0;
                            two_point_flag = 1;
                            break;
                        } else {
                            c = get_dec_point();
                            point_flag = 1;
                        }
                    }

                    lex_buff[lex_pos++] = tolower(c);
                }
				c = lex_next_c();
			} while ((c == '.') || isdigit(c) || (tolower(c) == 'e') /*|| (c == '+')
				|| (c == '-')*/);
			pc = c; lex_back_c();
            lex_buff[lex_pos] = 0;

            t = sym_get_code(lex_buff);

            errno = 0;

            if (t == L_FLOAT)
            {
                char *q = 0;
                lex_v.fnumber = strtod(lex_buff, &q);
                if (q == lex_buff)
                    errno = EINVAL;
#ifdef __DEBUG__N
                else
                    DBG("float = %f, %s\n", lex_v.fnumber, lex_buff);
#endif // __DEBUG__
            }

            else if (t == L_NUMERIC)
                lex_v.inumber = strtol(lex_buff, 0, 10);
            else
                errno = EINVAL;

            if (errno != 0)
            {
                err_include(ERROR, _("LEX : invalid numeric constant"), 0, lex_line, lex_c);
            }

			return t;
		}

        // Если ничего выше не подошло...
		switch(c)	// Let's the battle begin =)
		{
			/* Однолитеральные символы */
			case L_COMMA:
			case L_SEMICOLON:
			case L_ARROW:
			case L_LBRACKET:
			case L_RBRACKET:
			case L_PLUS:
			case L_MINUS:
			case L_RPANTH:
			case L_EQUAL:
			case L_LBRACE:
			case L_RBRACE:
				return c;

			case L_QUOTE:
				while((c = lex_next_c()) != L_QUOTE && c != EOF && c != '\n')
				{
                    if(lex_pos < LEX_BUF_SIZE-1)
                    {
                        lex_buff[lex_pos++] = c;
                    }
				}
                lex_buff[lex_pos] = 0;

				if (lex_pos == LEX_BUF_SIZE-1 || c == EOF || c == '\n')
				{
                    err_include(ERROR, _("LEX : invalid string constant"), 0, lex_line, lex_c);
				}

                if (strlen(lex_buff) == 1)
                {
                    lex_v.character = lex_buff[0];
                    return L_CHARCONST;
                } else {
                    lex_v.string = lex_buff;
                    return L_SCONST;
                }

			/* Возможное начало двулитерального символа */
			case L_SLASH:
				tc = lex_next_c();
				if(tc == L_SLASH) // Однострочный комментарий
					return L_LINECOMMENT;
				else
				{
					pc = tc; lex_back_c();
					return c;
				}

			case L_STAR:
				tc = lex_next_c();
				if(tc == L_RPANTH)
					return L_RCOMMENT;
				else
				{
					pc = tc; lex_back_c();
					return c;
				}

			case L_LPANTH:
				tc = lex_next_c();
				if(tc == L_STAR) // Комментарий
					return L_LCOMMENT;
				else
				{
					pc = tc; lex_back_c();
					return c;
				}
				break;
			case L_LESS:
				tc = lex_next_c();
				if(tc == L_EQUAL)
					return L_LEQUAL;
				else if(tc == L_GREAT)
					return L_NEQUAL;
				else
				{
					pc = tc; lex_back_c();
					return c;
				}
				break;
			case L_GREAT:
				tc = lex_next_c();
				if(tc == L_EQUAL)
					return L_GEQUAL;
				else
				{
					pc = tc; lex_back_c();
					return c;
				}
				break;
			case L_COLON:
				tc = lex_next_c();
				if(tc == L_EQUAL)
					return L_ASSIGN;
				else
				{
					pc = tc; lex_back_c();
					return c;
				}
				break;
			case L_POINT:
				tc = lex_next_c();
				if(tc == L_POINT)
					return L_TWOPOINTS;
				else
				{
					pc = tc; lex_back_c();
					return c;
				}
				break;
			case '\n':
				//lex_line_c++;// = io_line;
				//lex_char_c = 1;
				// Fall
			case ' ':
			case '\t':
				//lex_char_c++;
				// Fall
			case '\r':
                lex_line = lex_line_c;
                lex_c = lex_char_c;
				break;
			default:
                //if (!err_flag)
                    err_include(ERROR, _("LEX : unknown symbol"), 0, lex_line, lex_c);
                // else просто пропускаем
				break;
		}
	}

    // Что-то пошло не так, сюда мы попасть не должны
	return L_INVALID;
}

/*
    Упрощенная версия функции разбора, только для поиска конца многострочного комментария
*/
static int _lex_next_s_comm(void)
{
	char c = 0, tc = 0;

	while(c != EOF)
	{
		if(pc != 0)
		{
			c = pc;
			pc = 0;
			lex_step_c(c);
		} else {
			c = lex_next_c();
			//lex_char_c++;
		}

		if(c == EOF)
			return c;

		switch(c)
		{
			case L_LBRACE:
			case L_RBRACE:
				return c;
				break;

			case L_STAR:
				tc = lex_next_c();
				if(tc == L_RPANTH)
					return L_RCOMMENT;
				else
				{
					pc = tc; lex_back_c();
				}
				break;

			case L_LPANTH:
				tc = lex_next_c();
				if(tc == L_STAR) // Комментарий, начало
					return L_LCOMMENT;
				else
				{
					pc = tc; lex_back_c();
				}
				break;

			case '\n':
				/*lex_line_c++;// = io_line;
				lex_char_c = 1;*/
				return L_NEWLINE;

			case ' ':
			case '\t':
			case '\r':
			default:
                //lex_char_c++;
                lex_line = lex_line_c;
                lex_c = lex_char_c;
				break;
		}
	}

    // Что-то пошло не так, сюда мы попасть не должны
	return L_INVALID;
}

TOKEN_TYPE lex_next_s(void)
{
    int pair, tmp;
    int comm_beg_l, comm_beg_c; // Начало комментария

	lex_c = lex_char_c;
	lex_line = lex_line_c;

	lex_s = _lex_next_s();

    // Пропустим комментарии
    if (lex_s == L_LCOMMENT || lex_s == L_LBRACE || lex_s == L_LINECOMMENT)
    {
        switch (lex_s)
        {
            case L_LCOMMENT:
                pair = L_RCOMMENT;
                break;

            case L_LBRACE:
                pair = L_RBRACE;
                break;

            case L_LINECOMMENT:
                pair = L_NEWLINE;
                break;
        }

        tmp = lex_s;

        //err_flag = 1;
        comm_beg_l = lex_line;
        comm_beg_c = lex_c;

        while (lex_s != pair && lex_s != EOF)
        {
            lex_s = _lex_next_s_comm();
            if (lex_s == tmp)
                err_include(EWARN, _("LEX : nested comment"), 0, lex_line, lex_c);
        }

        if (lex_s == EOF)
        {
            err_include(ERROR, _("LEX : unclosed comment"), 0, comm_beg_l, comm_beg_c);
        } else {
            lex_s = _lex_next_s();
        }
        //err_flag = 0;
    }

    /*if (tmp == 0) // Мы только начали разбор
        return lex_next_s();
    else
        return tmp;*/
    return lex_s;
}
