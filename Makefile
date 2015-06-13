OBJECTS=io.o main.o errors.o lexer.o symtable.o spc_i18n.o bitset.o syn/parser.o \
	syn/program.o syn/vardecl.o syn/varpack.o syn/typedecl.o syn/typedefn.o \
	syn/block.o syn/operator.o syn/expr.o syn/varuse.o syn/constant.o sem/id_table.o \
	sem/type_table.o sem/sem_type.o list.o

#syntax.o decl.o set.o constr.o rec.o tree.o expr.o backend.o

SPC_I18N=1
I18N_SRC=src/io.c src/main.c src/errors.c src/lexer.c src/symtable.c src/syn/parser.c \
	src/syn/program.c syn/vardecl.c syn/varpack.c syn/typedecl.c syn/typedefn.c \
	syn/block.c syn/operator.c syn/expr.c syn/varuse.c syn/constant.c sem/id_table.c
TEXT_DOMAIN=simple-pascal-compiler

OBJDIR=.obj
SRCDIR=src
INCDIR=include

CFLAGS=-I./${INCDIR} -Wall -Wextra -c -DTEXT_DOMAIN=\"${TEXT_DOMAIN}\" -DSPC_I18N=${SPC_I18N}
LIBS=-lssl -lcrypto

OBJS=$(addprefix ${OBJDIR}/, ${OBJECTS})

# ifeq ($(DEBUG), 1)
CFLAGS+=-g -D__DEBUG__
# endif

vpath %.o ${OBJDIR}
vpath %.c ${SRCDIR}
vpath %.h ${INCDIR}

all: dirs version ${OBJS}
	gcc -Xlinker --no-as-needed ${OBJS} ${LIBS} -o main

Debug:
	make DEBUG=1 all

Release:
	make all

dirs:
	@mkdir -p ${OBJDIR}/syn
	@mkdir -p ${OBJDIR}/sem

version:
	@cat ${INCDIR}/config.h | cut -d' ' -f3 | xargs echo 1 + | bc | xargs -I'{}' echo "#define SPC_BUILD \"{}\"" > config.h.in
	@mv config.h.in ${INCDIR}/config.h

clean: delold
	@-rm ${OBJS} a.out main 2> /dev/null

cleanDebug: clean

cleanRelease: clean

${OBJDIR}/main.o: ${INCDIR}/config.h

delold:
	@-rm ${SRCDIR}/*~ ${INCDIR}/*~ ./*~ 2> /dev/null

${OBJDIR}/%.o: %.c %.h
	gcc ${CFLAGS} $< -o $@

i18n_ex: ${I18N_SRC}
	xgettext --from-code UTF-8 -d ${TEXT_DOMAIN} -o i18n/po/${TEXT_DOMAIN}.pot -k_ -s ${I18N_SRC}

i18n_ru:
	msgfmt i18n/po/ru/${TEXT_DOMAIN}.po -o i18n/locale/ru/LC_MESSAGES/${TEXT_DOMAIN}.mo

test:
	./main test.pas -o test.S
#	as -g test.S -o test.o
#	ld test.o
	gcc test.S -o a.out

