CPPFLAGS = -I.
CFLAGS = -m32 -ggdb3 -O0 -fno-stack-protector -fvisibility=hidden
LDFLAGS = $(CFLAGS)
LDLIBS != ../ncurses-config.sh
OBJECT_FILES = attr.o box.o canvas.o charset.o conic.o frame.o line.o string.o transfrm.o triangle.o

.PHONY: clean

all: ttydraw.a drawtest

ttydraw.a: $(OBJECT_FILES)
	$(AR) r $@ $(OBJECT_FILES)

drawtest: drawtest.o ttydraw.a
	$(CC) $(CFLAGS) $(LDLIBS) -o $@ drawtest.o ttydraw.a

clean:
	rm -f *.a *.o drawtest
