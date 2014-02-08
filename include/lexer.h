#ifndef __LEXER_H__
#define __LEXER_H__

int lex_next_s(void);	// Получить код следующего символа
char *lex_get_s(void);	// Получить значение текущего символа

enum 
{
	L_LCOMMENT = 300,
	L_RCOMMENT,
	L_LEQUAL,
	L_GEQUAL,
	L_NEQUAL,
	L_ASSIGN,
	L_TWOPOINTS
};

enum
{
	/*
		2
	*/
	L_OR = 400,
	L_TO,
	L_OF,
	L_IN,
	L_DO,
	L_IF,
	/*
		3
	*/
	L_END,
	L_VAR,
	L_AND,
	L_XOR,
	L_NOT,
	L_FOR,
	L_SET,
	/*
		4
	*/
	L_TYPE,
	L_GOTO,
	L_WITH,
	L_CASE,
	L_TRUE,
	L_THEN,
	L_ELSE,
	L_REAL,
	L_WORD,
	L_BYTE,
	L_FILE,
	L_TEXT,
	/*
		5
	*/
	L_BEGIN,
	L_WHILE,
	L_BREAK,
	L_UNTIL,
	L_ARRAY,
	L_CONST,
	L_FALSE,
	L_SHORT,
	/*
		6
	*/
	L_REPEAT,
	L_DOWNTO,
	L_RECORD,
	L_STRING,
	/*
		--//--
	*/
	L_INTEGER,
	L_PROGRAM,
	L_FUNCTION,
	L_PROCEDURE
};

enum
{
	L_IDENT = 500,
	L_NUMERIC,
	L_FLOAT,
	L_SCONST
};

#endif //__LEXER_H__