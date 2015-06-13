/*
    Модуль для работы с ошибками
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <spc_i18n.h>

#include <errors.h>
#include <debug.h>

static error *first = 0, *last = 0;
#ifdef STRANGE
static int last_err_line = 0;
#endif // STRANGE

/*
 * Добавляет ошибку в список ошибок
 */
void err_include(int level, const char *message, const char *exmsg, int line, int c)
{
	error *e = malloc(sizeof(error));
	e->level = level;
	e->message = (char *)message;
	e->exmsg = (char *)exmsg;
	e->line = line;
	e->c = c;
	e->next = 0;

	if(first == 0)
		first = e;

	if(last != 0)
		last->next = e;

	last = e;

	if(level == EFATAL)
	{
		err_print();
		exit(-1);
	}
}

/*
 * Вывод одной ошибки
 */
void err_one(error *list)
{

#ifdef STRANGE
        int i = 0;
        for(i = 0; i < 6; i++)
            fputc('*', stderr);

		for(i = 1; i < list->c; i++)
			fputc(' ', stderr);

		fputc('^', stderr);
		/*
		for(; i<80; i++)
			putchar('*');*/
		fputc('\n', stderr);

		for(i = 0; i < 6; i++)
            fputc('*', stderr);
#endif
		switch (list->level)
		{
			case ENOTE:
				fprintf(stderr, _("[Note ] : "));
				break;
			case EWARN:
				fprintf(stderr, _("[Warn ] : "));
				break;
			case ERROR:
				fprintf(stderr, _("[Error] : "));
				break;
			case EFATAL:
				fprintf(stderr, _("[Fatal] : "));
				break;
			default:
				fprintf(stderr, _("[Unkn ] : "));
				break;
		}

		if(list -> line != 0)
		{
			fprintf(stderr, "%d,%d : ", list->line, list->c);
		}
		fprintf(stderr, "%s", list->message);
		if(list -> exmsg != 0)
		{
			fprintf(stderr, " %s ", list->exmsg);
		}
		fprintf(stderr, "\n");
}

/*
 * Распечатывает список ошибок
 */
void err_print(void)
{
	error *list = first;
	error *flis = first;
	while(list != 0)
	{
		err_one(list);
		list = list->next;
		free(flis);
		flis = list;
	}
}

#ifdef STRANGE
/*
 * Распечатывает ошибки в строке line, в странном стиле
 */
void err_line(int line)
{
	error *list = first;
//DBG("LINE %d\n", line);
	// Пропустим до нужной строки
	while((list != 0)&&(list->line < line))
        list = list->next;

    if (list != 0)
    	last_err_line = line;

	while((list != 0)&&(list->line == line))
	{
        err_one(list);

		list = list->next;
		/*free(first);
		first = list;*/
	}
}

/*
    Выводит оставшиеся ошибки
*/
void err_rest()
{
	error *list = first;
	error *flis = first;

	while(list != 0)
	{
        if (list->line > last_err_line)
            err_one(list);
		//fprintf(stderr, "\n");
		list = list->next;
		free(flis);
		flis = list;
	}
}

#endif
