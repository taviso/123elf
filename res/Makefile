LDLIBS =
CFLAGS = -ggdb3 -O0
CPPFLAGS = -I.. -W -Wall -Wno-missing-field-initializers -Wno-sign-compare -Wno-unused-parameter -Wno-pointer-sign
LDFLAGS = $(CFLAGS)

.PHONY: clean

all: resgen resources

resources: l123txt3.ri

%.ri: %.txt | resgen
	./resgen < $< > $@

clean:
	$(RM) *.o *.ri hlp resgen
