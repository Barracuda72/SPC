OBJECTS=io.o main.o errors.o lexer.o syntax.o symtable.o decl.o set.o constr.o rec.o tree.o expr.o \
   backend.o

OBJDIR=obj
SRCDIR=src
INCDIR=include

CFLAGS=-I./${INCDIR} -Wall -c -g
LIBS=-lssl -lcrypto

OBJS=$(addprefix ${OBJDIR}/, ${OBJECTS})

# ifeq ($(DEBUG), 1)
CFLAGS+=-g -D__DEBUG__
# endif

vpath %.o ${OBJDIR}
vpath %.c ${SRCDIR}
vpath %.h ${INCDIR}

all: dirs version ${OBJS}
	gcc -g -Xlinker --no-as-needed ${OBJS} ${LIBS} -o main

debug:
	make DEBUG=1 all

dirs:
	@mkdir -p ${OBJDIR}

version:
	@cat ${INCDIR}/config.h | cut -d' ' -f3 | xargs echo 1 + | bc | xargs -I'{}' echo "#define SPC_BUILD \"{}\"" > config.h.in
	@mv config.h.in ${INCDIR}/config.h

clean: delold
	@-rm ${OBJS} a.out main 2> /dev/null

${OBJDIR}/main.o: ${INCDIR}/config.h

delold:
	@-rm ${SRCDIR}/*~ ${INCDIR}/*~ ./*~ 2> /dev/null

${OBJDIR}/%.o: %.c %.h
	gcc ${CFLAGS} $< -o $@

test:
	./main test.pas -o test.S
#	as -g test.S -o test.o
#	ld test.o
	gcc test.S -o a.out
