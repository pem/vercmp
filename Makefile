#
# pem 2021-11-14
#
#

CC=gcc -std=c99

CCOPTS=-pedantic -Wall -Werror

CCDEFS=

CFLAGS=-g -DDEBUG $(CCOPTS) $(CCDEFS)
#CFLAGS=-O $(CCOPTS) $(CCDEFS)
LDFLAGS=
LDLIBS=

PROG=vercmp-test

LIB=libvercmp.a

SRC=vercmp.c

OBJ=$(SRC:%.c=%.o)

all:	$(PROG)

$(PROG):	vercmp-test.o $(LIB)

$(LIB):	$(COBJ) $(OBJ)
	rm -f $(LIB)
	$(AR) qc $(LIB) $(COBJ) $(OBJ)
	ranlib $(LIB)

clean:
	$(RM) vercmp-test.o $(OBJ) core

cleanall:	clean
	$(RM) $(PROG) $(LIB) make.deps

make.deps:
	gcc -MM $(CFLAGS) $(SRC) > make.deps

include make.deps
