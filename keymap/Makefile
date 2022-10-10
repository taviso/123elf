LDLIBS = -lncurses -ltinfo
CFLAGS = -ggdb3 -O0
CPPFLAGS = -I.. -W -Wall -Wno-missing-field-initializers -Wno-sign-compare -Wno-unused-parameter -Wno-pointer-sign
LDFLAGS = $(CFLAGS)

.PHONY: clean

all: keymap

clean:
	$(RM) *.o keymap
