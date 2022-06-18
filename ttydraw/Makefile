CPPFLAGS=-I.
CFLAGS=-m32 $(OPTFLAGS) -fno-stack-protector -fvisibility=hidden
LDFLAGS=$(CFLAGS)
LDLIBS=-lncurses -ltinfo

.PHONY: clean

all: ttydraw.a drawtest

ttydraw.a: attr.o box.o canvas.o charset.o conic.o frame.o line.o string.o transfrm.o triangle.o
	$(AR) r $@ $^

drawtest: drawtest.o ttydraw.a


clean:
	rm -f *.a *.o drawtest
