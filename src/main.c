#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include <config.h>
#include <io.h>
#include <errors.h>
#include <syntax.h>
#include <main.h>
#include <backend.h>

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
	fprintf(stderr, "SPC - Simple Pacsal Compiler, build %s\n", SPC_BUILD);
	fprintf(stderr, "Built at %s, %s\n", __DATE__, __TIME__);
	exit(0);
}

void usage(void)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "\t--output\t-o\tSpecify output file\n");
	fprintf(stderr, "\t--werror\t\tThreat warnings as errors\n");
	fprintf(stderr, "\t--version\t-V\tVersion info\n");
	fprintf(stderr, "\t--help\t\t-h\tThis short help\n");
// 	fprintf(stderr, "");
	exit(0);
}

int main(int argc, char *argv[])
{
	int c;
	char *in = 0, *out = 0;
	while(1)
	{
		c = getopt_long(argc, argv, "ho:V", opts, 0);
		if(c == -1) break;
		switch(c)
		{
			case 'o':
				out = optarg;
				break;
			case 'V':
				version();
				break;
			case 'h':
				usage();
				break;
			default:
				usage();
				break;
			
		}
	}

	if(optind < argc)
		in = argv[optind];

	if(!in)
		err_include(EFATAL, "missing input file", 0, 0, 0);

	if(!out)
		out = "a.out";

	//printf("%s -> %s\n", in, out);
	io_open(in, out);
	tree *data = program();
	err_print();
	backend_print_tree(data);
	io_close();
	return 0;
}

char *strncpy_m(char *dest, const char *src, int n)
{
	int i;

	for (i = 0 ; i < n && src[i] != '\0' ; i++)
                   dest[i] = src[i];
	if(i < n)
		dest[i] = src[i];	// 0x00

        return dest;
}
