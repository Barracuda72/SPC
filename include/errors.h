#ifndef __ERRORS_H__
#define __ERRORS_H__

/*
	Классы ошибок
*/
#define ENOTE	1	// Замечание - можно проигнорировать
#define EWARN	2	// Предупреждение - стоит приглядеться
#define ERROR	3	// Ошибка - программа не соберется, но будет анализироваться дальше
#define EFATAL	4	// Фатальная ошибка - немедленно прерывает компиляцию

/*
	Ошибки
*/
#define ENONE	0	// А нет ошибки
#define EIOERR	-1	// Ошибка ввода-вывода

/*
	Добавить ошибку в таблицу ошибок
	Если level = EFATAL, выводим предыдущие ошибки и прекращаем компиляцию
*/
void err_include(int level, char *message, char *exmsg, int line, int c);

/*
	Вывести ошибки
*/
void err_print(void);


/*
	Вывести ошибки, относящиеся к строке line
*/
void err_line(int line);

/*
	Структура, описывающая ошибку
*/
typedef struct _error
{
	int level;
	char *message;
	char *exmsg;
	int line;
	int c;
	struct _error *next;
} error;
#endif //__ERRORS_H_