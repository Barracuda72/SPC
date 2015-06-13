#ifndef __ERRORS_H__
#define __ERRORS_H__


#define STRANGE

/*
	Классы ошибок
*/
enum {
  ENOTE = 1,    // Замечание - можно проигнорировать
  EWARN,        // Предупреждение - стоит приглядеться
  ERROR,        // Ошибка - программа не соберется, но будет анализироваться дальше
  EFATAL        // Фатальная ошибка - немедленно прерывает компиляцию
};

/*
	Коды возврата функций
*/
#define ENONE	0	// А нет ошибки
#define EIOERR	-1	// Ошибка ввода-вывода

/*
	Добавить ошибку в таблицу ошибок
	Если level = EFATAL, выводим предыдущие ошибки и прекращаем компиляцию
*/
void err_include(int level, const char *message, const char *exmsg, int line, int c);

/*
	Вывести ошибки
*/
void err_print(void);

#ifdef STRANGE
/*
	Вывести ошибки, относящиеся к строке line
*/
void err_line(int line);

/*
    Вывести оставшиеся в списке ошибки
*/
void err_rest(void);
#endif

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

#endif //__ERRORS_H__
