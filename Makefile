BFD_INP_TARGET = coff-i386
BFD_OUT_TARGET = coff-i386
OBJCOPY_FLAGS  = --wildcard --localize-symbols=localize.lst --globalize-symbols=globalize.lst --redefine-syms=redefine.lst --weaken-symbols=weaken.lst
CFLAGS  = -lc -m32 -ggdb3 -O0 -fno-stack-protector
ASFLAGS = --32
LDFLAGS = $(CFLAGS) -B. -Wl,-b,coff-i386 -no-pie
LDLIBS = -lncurses -ltinfo
PATH := .:$(PATH)
prefix ?= /
bindir ?= ${prefix}usr/local/bin
sharedir = ${prefix}usr/local/share/lotus
profiledir = ${prefix}etc/profile.d

define BFD_TARGET_ERROR
Your version of binutils was compiled without coff-i386 target support.
endef

export BFD_TARGET_ERROR

.PHONY: clean check

all: check 123
	@file 123
	@size 123
	@echo Copy l123set.cf to ~/.l123set and 1-2-3 should be ready!

check:
	@objdump --info | egrep -q '^coff-i386$$' || echo "$$BFD_TARGET_ERROR"
	@objdump --info | egrep -q '^coff-i386$$'

# Functions that are not compatible from UNIX<=>Linux that we need to fixup.
UNDEF_SYMBOLS=__unix_ioctl       \
              __unix_open        \
              __unix_stat        \
              __unix_uname       \
              __unix_times       \
              __unix_sysi86      \
              __unix_fstat       \
              __unix_read        \
              __unix_fcntl

orig/123.o:
	@echo You need to run the extract.sh script to get the 1-2-3 files.
	@false

# Functions that should be compatible, but 123 does something weird.
123.o: UNDEF_SYMBOLS+=__unix_environ __wrap_lic_init
123.o: orig/123.o | coffsyrup redefine.lst globalize.lst localize.lst weaken.lst forceplt.s
	objcopy -I $(BFD_INP_TARGET) -O $(BFD_OUT_TARGET) $(OBJCOPY_FLAGS) $< $@
	coffsyrup $@ $(@:.o=.tmp.o) $(UNDEF_SYMBOLS) $$(awk '/@plt/ {print substr($$2,0,length($$2) - 4)}' forceplt.s)
	mv $(@:.o=.tmp.o) $@

dl_init.o: orig/dl_init.o
	objcopy -I $(BFD_INP_TARGET) -O $(BFD_OUT_TARGET) $(OBJCOPY_FLAGS) $< $@

123: 123.o dl_init.o main.o wrappers.o patch.o | forceplt.o
	$(CC) forceplt.o $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

install:
	mkdir -p ${bindir} ${sharedir}/bin
	cp -r ./root ${sharedir}
	install -Dm 755 123 ${sharedir}/123
	install -Dm 755 run.sh ${bindir}/123
	mkdir -p ${profiledir}
	echo export LOTUS_HOME="${sharedir}" > ${profiledir}/lotus.sh
	chmod +x ${profiledir}/lotus.sh

uninstall:
	rm -r ${sharedir}
	rm ${bindir}/123
	rm ${profiledir}/lotus

clean:
	rm -f *.o 123 coffsyrup
	rm -f vgcore.* core.* core
