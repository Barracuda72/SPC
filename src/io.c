#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <io.h>
#include <errors.h>

static FILE *in;
static FILE *out;
static char io_buff[1024];
static int io_bufpos = 1024;
static int io_buflen = 0;

int io_line = 1, io_c = 1;

int io_open(const char *fname_in, const char *fname_out)
{
	if(!strncmp("-", fname_in, 4))
		in = stdin;
	else
	{
		in = fopen(fname_in, "rb");
		if(!in)
		{
			err_include(EFATAL, "can't open input file", 0, 0, 0);
			return EIOERR;
		}
	}

	out = fopen(fname_out, "wb");
	if(!out)
	{
		err_include(EFATAL, "can't open output file", 0, 0, 0);
		return EIOERR;
	}

	return ENONE;
}

void io_close(void)
{
	fclose(in);
	fclose(out);
}

char io_next_c(void)
{
	char r = -1;
	if(io_bufpos > 1023)
	{
		io_buflen = fread(io_buff, 1, 1024, in);
		io_bufpos = 0;
	}

	if(io_bufpos >= io_buflen)
	{
		//err_include(EFATAL, "unexpected end of file", io_line, io_c);
		r = EOF;
	} else {
		r = io_buff[io_bufpos++];

#ifdef STRANGE
		putchar(r);
#endif

		if(r == '\n')
		{

#ifdef STRANGE
			err_line(io_line);
#endif

			io_line++;
			io_c = 1;
		} else {
			io_c++;
		}
	}
	return r;
}
/*
static void putdec(uint32_t byte);
static void puthexi(uint32_t dword);
static void puthex(uint8_t byte);
static void puthexd(uint8_t byte);
static void vprintf(const char *fmt, va_list args);

static void putdec(uint32_t byte)
{
  uint8_t b1;
  int b[30];
  signed int nb;
  int i=0;
  
  while(1){
    b1=byte%10;
    b[i]=b1;
    nb=byte/10;
    if(nb<=0){
      break;
    }
    i++;
    byte=nb;
  }
  
  for(nb=i+1;nb>0;nb--){
    puthexd(b[nb-1]);
  }
}

static void puthexi(uint32_t dword)
{
  puthex( (dword & 0xFF000000) >>24);
  puthex( (dword & 0x00FF0000) >>16);
  puthex( (dword & 0x0000FF00) >>8);
  puthex( (dword & 0x000000FF));
}


static void puthex(uint8_t byte)
{
 unsigned  char lb, rb;

  lb=byte >> 4;
  
  rb=byte & 0x0F;
  
  
  puthexd(lb);
  puthexd(rb);
}

static void puthexd(uint8_t digit)
{
  char table[]="0123456789ABCDEF";
   fputc(table[digit], out);
}

static void putbin(uint8_t byte)
{
        int i = 0;
        for (i = 0; i <8; i++)  puthexd(((byte<<i)>>7)&0x01);
}
*/
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
/*
static void vprintf(const char *fmt, va_list args)
{
  while (*fmt) {
    
    switch (*fmt) {
    case '%':
      fmt++;

      switch (*fmt) {
      case 's':
	fputs(va_arg(args, char *), out);
	break;
	
      case 'c':
	fputc(va_arg(args, uint32_t), out);
	break;
	
      case 'i':
	putdec(va_arg(args, uint32_t));
	break;

      case 'x':
	puthex(va_arg(args, uint32_t));
	break;
	
      case 'X':
	puthexi(va_arg(args, uint32_t));
	break;

      case 'b':
	putbin(va_arg(args, uint32_t));
	break;

      }
      

      break;
      
    default:
      fputc(*fmt, out);
      break;
    }

    fmt++;
  }
}*/
