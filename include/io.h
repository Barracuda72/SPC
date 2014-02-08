#ifndef __IO_H__
#define __IO_H__

int io_open(const char *fname_in, const char *fname_out);
void io_close(void);
char io_next_c(void);
void io_printf(const char *fmt, ...);

#endif //__IO_H__ 
