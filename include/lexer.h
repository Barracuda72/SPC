#ifndef __LEXER_H__
#define __LEXER_H__

/*
 * Структура, описывающая текущий символ
 */
typedef union {
    long inumber;
    double fnumber;
    char *string;
    char character;
} lex_val;

typedef enum
{
    L_EOF = -1,
    /*
        Однолитеральные символы
    */
    L_PLUS = '+',
    L_MINUS = '-',
    L_STAR = '*',
    L_SLASH = '/',
    L_POINT = '.',
    L_COMMA = ',',
    L_SEMICOLON = ';',
    L_COLON = ':',
    L_ARROW = '^',
    L_LBRACKET = '[',
    L_RBRACKET = ']',
    L_LBRACE = '{',
    L_RBRACE = '}',
    L_LPANTH = '(',
    L_RPANTH = ')',
    L_EQUAL = '=',
    L_QUOTE = '\'',
    L_LESS = '<',
    L_GREAT = '>',
    L_NEWLINE = '\n',

    /*
        Двулитеральные символы
    */
	L_LCOMMENT = 300,
	L_RCOMMENT,
	L_LEQUAL,
	L_GEQUAL,
	L_NEQUAL,
	L_ASSIGN,
	L_TWOPOINTS,

	/*
        Ключевые слова
    */

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
	L_END = 500,
	L_VAR,
	L_AND,
	L_XOR,
	L_NOT,
	L_FOR,
	L_DIV,
	L_MOD,
	L_NIL,
	L_SSET, // L_SET зарезервировано
	/*
		4
	*/
	L_TYPE = 600,
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
	L_BEGIN = 700,
	L_WHILE,
	L_BREAK,
	L_UNTIL,
	L_ARRAY,
	L_CONST,
	L_FALSE,
	L_SHORT,
	L_LABEL,
	/*
		6
	*/
	L_REPEAT = 800,
	L_DOWNTO,
	L_RECORD,
	L_STRING,
	/*
		--//--
	*/
	L_INTEGER = 900,
	L_PROGRAM,
	L_FUNCTION,
	L_PROCEDURE,
    /*
        Фиктивные
    */
	L_IDENT = 1000,
	L_NUMERIC,
	L_FLOAT,
	L_SCONST,
	L_CHARCONST,
	L_LINECOMMENT,

	L_INVALID = 1024
} TOKEN_TYPE;

/*
 * Получить значение текущего символа
 */
lex_val lex_get_v(void);

/*
 * Получить код текущего символа
 */
TOKEN_TYPE lex_get_s(void);

/*
 * Получить код следующего символа
 */
TOKEN_TYPE lex_next_s(void);

#endif //__LEXER_H__
