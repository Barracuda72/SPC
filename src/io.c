/*
    Модуль ввода-вывода
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <io.h>
#include <errors.h>
#include <spc_i18n.h>

#define IO_BUF_SIZE 1024

static FILE *in;
static FILE *out;
static char io_buff[IO_BUF_SIZE];
static char io_ungetc_buf[IO_BUF_SIZE];

static int io_bufpos = IO_BUF_SIZE;
static int io_buflen = 0;
static int io_ungetc_pos = -1;

int io_line = 1, io_c = 1;

/*
 * Производит инициализацию, открывая входной и выходной файлы
 */
int io_open(const char *fname_in, const char *fname_out)
{
	if(!strncmp("-", fname_in, 4))
		in = stdin;
	else
	{
		in = fopen(fname_in, "rb");
		if(!in)
		{
			err_include(EFATAL, _("IO : can't open input file"), 0, 0, 0);
			return EIOERR;
		}
	}

	out = fopen(fname_out, "wb");
	if(!out)
	{
		err_include(EFATAL, _("IO : can't open output file"), 0, 0, 0);
		return EIOERR;
	}

	return ENONE;
}

/*
 * Деинициализация: закрывает входной и выходной файлы
 */
void io_close(void)
{
#ifdef STRANGE
    err_rest();
#endif
	fclose(in);
	fclose(out);
}

/*
 * Возвращает следующий символ из потока символов
 */
char io_next_c(void)
{
	char r = -1;

    // Реализация ungetc
	if (io_ungetc_pos >= 0)
	{
        io_c--;
        if (io_c == 0)
        {
            io_c = 1;
            io_line--;
        }
        return io_ungetc_buf[io_ungetc_pos--];
	}

    // Если буфер кончился, считаем еще
	if(io_bufpos >= IO_BUF_SIZE)
	{
		io_buflen = fread(io_buff, 1, IO_BUF_SIZE, in);
		io_bufpos = 0;
	}

    // Если закончился файл
	if(io_bufpos >= io_buflen)
	{
		//err_include(EFATAL, "unexpected end of file", io_line, io_c);
		r = EOF;
		err_line(io_line); // TODO: HACK
	} else {
		r = io_buff[io_bufpos++];

#ifdef STRANGE
        if (io_c == 1) // Вставим отступ перед первым символом
        {
#ifdef STRANGE
			err_line(io_line-1);
#endif
            fprintf(stderr, "      "); // 6 пробелов
        }
		fputc(r, stderr);
#endif

		if(r == '\n')
		{
/*
#ifdef STRANGE
			err_line(io_line);
#endif
*/
			io_line++;
			io_c = 1;
		} else {
			io_c++;
		}
	}
	return r;
}

/*
 * Возвращает символ во входной поток
 */
void io_ungetc(char c)
{
    if(io_ungetc_pos < IO_BUF_SIZE)
    {
        io_ungetc_buf[++io_ungetc_pos] = c;
    }
}

#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)

typedef __builtin_va_list va_list;

void io_printf(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  //vprintf(fmt, args);
  vfprintf(out, fmt, args);

  va_end(args);
}
