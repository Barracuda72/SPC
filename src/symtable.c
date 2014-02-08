#include <stdint.h>
#include <strings.h>
#include <ctype.h>

#include <symtable.h>
#include <errors.h>
#include <lexer.h>

/*
	Таблица символов
*/

typedef struct
{
	int code;
	char *sym;
} sym_entry;

static sym_entry sym_bols[] = 
{
	/*
		Односимвольные коды
	*/
	{'.', "."},
	{',', ","},
	{'/', "/"},
	{',', ","},
	{':', ":"},
	{'^', "^"},
	{';', ";"},
	{'[', "["},
	{']', "]"},
	{'{', "{"},
	{'}', "}"},
	{'(', "("},
	{')', ")"},
	{'+', "+"},
	{'-', "-"},
	{'=', "="},
	{'>', ">"},
	{'<', "<"},
	{'*', "*"},
	{' ', " "},
	{'\n', "\n"},
	{'\t', "\t"},
	{'\r', "\r"},
	/*
		Двухсимвольные коды простых операций
	*/
	{L_LCOMMENT, "(*"},
	{L_RCOMMENT, "*)"},
	{L_LEQUAL, "<="},
	{L_GEQUAL, ">="},
	{L_NEQUAL, "<>"},
	{L_ASSIGN, ":="},
	{L_TWOPOINTS, ".."},
	/*
		Коды-заглушки для встроенных типов лексем
	*/
	{L_IDENT, "IDENT"},
	{L_NUMERIC, "NUMERIC"},
	{L_FLOAT, "FLOAT"},
	/*
		Двухсимвольные коды
	*/
	{L_OR, "or"},
	{L_TO, "to"},
	{L_OF, "of"},
	{L_IN, "in"},
	{L_DO, "do"},
	{L_IF, "if"},
	/*
		Трехсимвольные коды
	*/
	{L_END, "end"},
	{L_VAR, "var"},
	{L_AND, "and"},
	{L_XOR, "xor"},
	{L_NOT, "not"},
	{L_FOR, "for"},
	{L_SET, "set"},
	/*
		Четырехсимвольные коды
	*/
	{L_TYPE, "type"},
	{L_GOTO, "goto"},
	{L_WITH, "with"},
	{L_CASE, "case"},
	{L_TRUE, "true"},
	{L_THEN, "then"},
	{L_ELSE, "else"},
	{L_REAL, "real"},
	{L_WORD, "word"},
	{L_BYTE, "byte"},
	{L_FILE, "file"},
	{L_TEXT, "text"},
	/*
		Пять символов
	*/
	{L_BEGIN, "begin"},
	{L_WHILE, "while"},
	{L_UNTIL, "until"},
	{L_BREAK, "break"},
	{L_ARRAY, "array"},
	{L_CONST, "const"},
	{L_FALSE, "false"},
	{L_SHORT, "short"},
	/*
		Шесть символов
	*/
	{L_REPEAT, "repeat"},
	{L_DOWNTO, "downto"},
	{L_RECORD, "record"},
	{L_STRING, "string"},
	/*
		Остальное 
	*/
	{L_INTEGER, "integer"},
	{L_PROGRAM, "program"},
	{L_FUNCTION, "function"},
	{L_PROCEDURE, "procedure"}
	//{, ""},
};

static int sym_count = sizeof(sym_bols)/sizeof(sym_entry);

extern int lex_line, lex_c;

static int isidsym(char c)
{
	return (isalnum(c)||(c == '_'));
}

static int isident(const char *s)
{
	if ((!isalpha(*s)) && (*s != '_'))
		return 0;
	s++;
	while(*s)
	{
		if(!isidsym(*s))
			return 0;
		s++;
	}
	return 1;
}

static int isnum(const char *s)
{
	int si = 0;
	while(*s)
	{
		if((*s == '-') || (*s == '+'))
		{
			if(!si)
			{
				si = 1;
				continue;
			}
			else return 0;
		}
		if(!isdigit(*s++))
			return 0;
	}
	return 1;
}

static int isfloat(const char *s)
{
	int p = 0;	// Встретилась точка
	int e = 0;	// Встретилась буква E
	int si = 0;	// Встретился знак
	int n = 0;	// Встретилась цифра
	while(*s)
	{
		if(*s == '.')
		{
			if((p == 0)&&(n == 1))	// Точка не может повторяться и следовать перед цифрой
				p = 1;
			else
				return 0;
		} else if((tolower(*s) == 'e')&&(n == 1))
		{
			if(e == 0)
				e = 1;
			else
				return 0;
		} else if((*s == '-')||(*s == '+'))
		{
			if((si == 0) && (e != 0) && (tolower(*(s - 1)) == 'e'))
				si = 1;
			else
				return 0;
		} else if(!isdigit(*s))
			return 0;
		else n = 1;
		s++;
	}
	return 1;
}

int sym_get_code(char *sym)
{
	int i;
	//printf("Sym_get_code = %s\n", sym);
	for(i = 0; i < sym_count; i++)
	{
		if(!strncasecmp(sym, sym_bols[i].sym, 100))
		{
			return sym_bols[i].code;
		}
	}
	if(isnum(sym))
		return L_NUMERIC;
	if(isfloat(sym))
		return L_FLOAT;
	if(isident(sym))
		return L_IDENT;
	err_include(ERROR, "SYM : unknown symbol", sym, lex_line, lex_c);
	return 0;
}

char *sym_get_sym(int code)
{
	int i;
	for(i = 0; i < sym_count; i++)
	{
		if(code == sym_bols[i].code)
		{
			return sym_bols[i].sym;
		}
	}
	//char s[10];
	//sprintf(&s, "%d", code);
	err_include(ERROR, "SYM : unknown code", 0, lex_line, lex_c);
	return 0;
}
