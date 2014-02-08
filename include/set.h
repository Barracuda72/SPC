#ifndef __SET_H__
#define __SET_H__

/*
	Множества
*/
enum
{
	TYPES = 800,	// Типы: integer, real, etc
	DECLS,		// Определения: const, type, var, etc
	OPBEG		// Начала операторов: идентификатор, BEGIN, CASE, IF и пр.
};

int belong(int elem, int set);

#endif //__SET_H__