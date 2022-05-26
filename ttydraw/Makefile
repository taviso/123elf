CPPFLAGS=-I.
CFLAGS=-m32 -ggdb3 -O0
LDFLAGS=$(CFLAGS)
LDLIBS=-lncurses

.PHONY: clean

all: ttydraw.a drawtest

ttydraw.a: attr.o box.o canvas.o charset.o conic.o frame.o line.o string.o transfrm.o triangle.o
	$(AR) r $@ $^

drawtest: drawtest.o ttydraw.a


clean:
	rm -f *.a *.o drawtest
