#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <errors.h>

static error *first = 0, *last = 0;

void err_include(int level, char *message, char *exmsg, int line, int c)
{
	error *e = malloc(sizeof(error));
	e->level = level;
	e->message = message;
	e->exmsg = exmsg;
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

void err_print(void)
{
	error *list = first;
	error *flis = first;
	while(list != 0)
	{
		switch (list->level)
		{
			case ENOTE:
				fprintf(stderr, "[Note ] : ");
				break;
			case EWARN:
				fprintf(stderr, "[Warn ] : ");
				break;
			case ERROR:
				fprintf(stderr, "[Error] : ");
				break;
			case EFATAL:
				fprintf(stderr, "[Fatal] : ");
				break;
			default:
				// Неизвестный тип ошибки
				fprintf(stderr, "[Unkn ] : ");
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
		list = list->next;
		free(flis);
		flis = list;
	}
}

void err_line(int line)
{
	error *list = first;
	int i;
		//printf("Line %d, errors\n", line);
	while((list != 0)&&(list->line == line))
	{
		//printf("Line %d, errors\n", line);
		for(i = 1; i < list->c; i++)
			putchar('*');
		putchar('^');
		for(; i<80; i++)
			putchar('*');
		putchar('\n');
		switch (list->level)
		{
			case ENOTE:
				fprintf(stderr, "[Note ] : ");
				break;
			case EWARN:
				fprintf(stderr, "[Warn ] : ");
				break;
			case ERROR:
				fprintf(stderr, "[Error] : ");
				break;
			case EFATAL:
				fprintf(stderr, "[Fatal] : ");
				break;
			default:
				fprintf(stderr, "[Unkn ] : ");
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
		list = list->next;
		/*free(flis);
		flis = list;*/
	}
}
