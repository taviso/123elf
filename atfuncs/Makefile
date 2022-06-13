CPPFLAGS=-I..
CFLAGS=-m32 -ggdb3 -O0 -fno-stack-protector
LDFLAGS=$(CFLAGS)
LDLIBS=

.PHONY: clean

all: atfuncs.a

atfuncs.a: date.o
	$(AR) r $@ date.o

clean:
	rm -f *.a *.o
