BFD_INP_TARGET = coff-i386
BFD_OUT_TARGET = coff-i386
OBJCOPY_FLAGS  = --wildcard --localize-symbols=localize.lst --globalize-symbols=globalize.lst --redefine-syms=redefine.lst $(shell cat symbols.lst)
OBJCOPY_FILES = localize.lst globalize.lst redefine.lst undefine.lst symbols.lst
OPTFLAGS = -O2
CFLAGS  = -freg-struct-return -W -Wall -m32 $(OPTFLAGS) -fno-stack-protector
CPPFLAGS = -I. -D_FILE_OFFSET_BITS=64 -D_TIME_BITS=64 -D_GNU_SOURCE -I ttydraw -Wno-unused-parameter
ASFLAGS = --32
LDFLAGS = $(CFLAGS) -lc -B. -Wl,-b,$(BFD_OUT_TARGET) -no-pie
LDLIBS = -lncurses -ltinfo -lm
PATH := .:$(PATH)
prefix = /usr/local

# The list of terminals we generate keymaps for by default.
KEYMAPS  = xterm rxvt-unicode
KEYMAPS := $(KEYMAPS) $(patsubst %,%-256color,$(KEYMAPS))
KEYMAPS += screen.xterm-256color vt100 $(TERM)

define BFD_TARGET_ERROR
Your version of binutils was compiled without coff-i386 target support.
You can try running ./binutils.sh to build a version that does support it.
endef

export BFD_TARGET_ERROR

.PHONY: clean check distclean install uninstall

all: check 123 keymaps
	@size 123

debug: OPTFLAGS=-ggdb3 -O0
debug: all

check:
	@objdump --info | egrep -q '^coff-i386$$' || (echo "$$BFD_TARGET_ERROR"; false)

orig/123.o:
	@echo You need to run the extract.sh script to get the 1-2-3 files.
	@false

123.o: orig/123.o $(OBJCOPY_FILES) | coffsyrup
	objcopy -I $(BFD_INP_TARGET) -O $(BFD_OUT_TARGET) $(OBJCOPY_FLAGS) $< $@
	coffsyrup $@ $(@:.o=.tmp.o) $$(cat undefine.lst)
	mv $(@:.o=.tmp.o) $@

dl_init.o: orig/dl_init.o
	objcopy -I $(BFD_INP_TARGET) -O $(BFD_OUT_TARGET) $(OBJCOPY_FLAGS) $< $@

ttydraw/ttydraw.a:
	$(MAKE) -C ttydraw OPTFLAGS="$(OPTFLAGS)"

atfuncs/atfuncs.a:
	$(MAKE) -C atfuncs OPTFLAGS="$(OPTFLAGS)"

123OBJS=src/showme.o   \
        src/invalid.o

bin/123: 123.o dl_init.o main.o wrappers.o patch.o filemap.o graphics.o draw.o filename.o import.o $(123OBJS) | ttydraw/ttydraw.a atfuncs/atfuncs.a forceplt.o
	@mkdir -p $(@D)
	$(CC) forceplt.o $(CFLAGS) $(LDFLAGS) $^ -Wl,--whole-archive,ttydraw/ttydraw.a,atfuncs/atfuncs.a,--no-whole-archive -o $@ $(LDLIBS)

123: bin/123
	@ln -fs $^ $@

keymap/keymap:
	$(MAKE) -C keymap

# This generates the keymaps in a seperate directory based on the first letter.
$(sort $(KEYMAPS)): keymap/keymap
	mkdir -p share/lotus/keymaps/$(shell printf "%c" $@)
	-keymap/keymap $@ > share/lotus/keymaps/$(shell printf "%c" $@)/$@

keymaps: $(KEYMAPS)

clean:
	$(RM) *.o src/*.o coffsyrup 123
	$(RM) vgcore.* core.* core
	$(RM) -r bin share/lotus/keymaps
	$(MAKE) -C ttydraw clean
	$(MAKE) -C atfuncs clean
	$(MAKE) -C keymap clean

distclean: clean
	./gzip.sh clean
	./binutils.sh clean
	./extract.sh clean

install: all
	install -Dm 755 "bin/123" "$(prefix)/bin/123"
	install -Dm 644 "share/man/man1/123.1" "$(prefix)/share/man/man1/123.1"
	install -Dm 644 "share/lotus/etc/l123set.cf" "$(prefix)/share/lotus/etc/l123set.cf"
	find "share/lotus/keymaps" -type f -exec install -Dm 644 {} "$(prefix)/{}" \;
	install -Dm 644 "share/lotus/123.v10/sysV386/lotus.bcf" "$(prefix)/share/lotus/123.v10/sysV386/lotus.bcf"
	install -Dm 644 "share/lotus/123.v10/sysV386/lib/wyse50-lts123" "$(prefix)/share/lotus/123.v10/sysV386/lib/wyse50-lts123"
	find "share/lotus/123.v10/cbd" -type f -exec install -Dm 644 {} "$(prefix)/{}" \;
	find "share/lotus/123.v10/fonts" -type f -exec install -Dm 644 {} "$(prefix)/{}" \;
	find "share/lotus/123.v10/hlp" -type f -exec install -Dm 644 {} "$(prefix)/{}" \;
	find "share/lotus/123.v10/keymaps" -type f -exec install -Dm 644 {} "$(prefix)/{}" \;
	find "share/lotus/123.v10/pbd" -type f -exec install -Dm 644 {} "$(prefix)/{}" \;
	find "share/lotus/123.v10/ri" -type f -exec install -Dm 644 {} "$(prefix)/{}" \;
	find "share/lotus/123.v10/smpfiles" -type f -exec install -Dm 644 {} "$(prefix)/{}" \;

uninstall:
	$(RM) "$(prefix)/bin/123"
	$(RM) "$(prefix)/share/man/man1/123.1"
	$(RM) -r "$(prefix)/share/lotus"
