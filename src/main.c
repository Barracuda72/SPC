#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include <config.h>
#include <io.h>
#include <errors.h>
#include <lexer.h>
#include <symtable.h>
#include <spc_i18n.h>
#include <syn/parser.h>
#include <syn/program.h>
#include <sem/sem_type.h>
#include <main.h>

void lexer_check(void);
void syntax_check(void);

static char *file_in = 0;
static char *file_out = 0;

static struct option opts[] =
{
	{ "output",  1, 0, 'o' },
	{ "werror",  1, 0,  0  },
	{ "version", 1, 0, 'V' },
	{ "help",    1, 0, 'h' },
	{ 0,         0, 0,  0  }
};

void version(void)
{
	fprintf(stderr, _("SPC - Simple Pacsal Compiler, build %s\n"), SPC_BUILD);
	fprintf(stderr, _("Built at %s, %s\n"), __DATE__, __TIME__);
	exit(0);
}

void usage(void)
{
	fprintf(stderr, _("Usage:\n"));
	fprintf(stderr, _("\t--output\t-o\tSpecify output file\n"));
	fprintf(stderr, _("\t--werror\t\tThreat warnings as errors\n"));
	fprintf(stderr, _("\t--version\t-V\tVersion info\n"));
	fprintf(stderr, _("\t--help\t\t-h\tThis short help\n"));
// 	fprintf(stderr, "");
	exit(0);
}

void handle_opts(int argc, char *argv[])
{
    int c = 0;

    while(c != -1)
	{
		c = getopt_long(argc, argv, "ho:V", opts, 0);

		switch(c)
		{
			case 'o':
				file_out = optarg;
				break;
			case 'V':
				version();
				break;
			case 'h':
				usage();
				break;
            case -1:
                break;
			default:
				usage();
				break;

		}
	}

	if(optind < argc)
		file_in = argv[optind];

	if(!file_out)
		file_out = "a.out";
}

int main(int argc, char *argv[])
{
	init_gettext();
	handle_opts(argc, argv);

    if(!file_in)
    {
		err_include(EFATAL, _("SPC : missing input file"), 0, 0, 0);
		return -1;
    }

	io_open(file_in, file_out);

#if 0
	lexer_check();
#else
    syntax_check();
#endif // 1

#ifndef STRANGE
	err_print();
#endif

	io_close();
	return 0;
}

void lexer_check(void)
{
    int c = 0;
    extern int lex_line, lex_c;

    while ((c = lex_next_s()) != EOF)
	{
#if 1
        io_printf("%d(%d %d) ", c, lex_line, lex_c);
        if (c == L_SEMICOLON)
            io_printf("\n");
#else
        switch (c)
        {
            case L_NUMERIC:
                io_printf("%d ", lex_get_v().inumber);
                break;

            case L_FLOAT:
                io_printf("%f ", lex_get_v().fnumber);
                break;

            case L_SCONST:
                io_printf("'%s' ", lex_get_v().string);
                break;

            case L_CHARCONST:
                io_printf("'%c' ", lex_get_v().character);
                break;

            case L_IDENT:
                io_printf("%s ", lex_get_v().string);
                break;

            default:
                io_printf("%s ", sym_get_sym(c));
                if (c == L_SEMICOLON)
                    io_printf("\n");
                break;
        }
#endif

	}
	io_printf("\n");
}

void syntax_check(void)
{
    init_parser();
    sem_init();
    CALL_PARSER_FUNC(program, 0);
    fini_parser();
}
