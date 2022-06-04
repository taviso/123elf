BFD_INP_TARGET = coff-i386
BFD_OUT_TARGET = coff-i386
OBJCOPY_FLAGS  = --wildcard --localize-symbols=localize.lst --globalize-symbols=globalize.lst --redefine-syms=redefine.lst
OBJCOPY_FILES = localize.lst globalize.lst redefine.lst undefine.lst
CFLAGS  = -m32 -ggdb3 -O0 -fno-stack-protector
CPPFLAGS = -D_FILE_OFFSET_BITS=64 -D_TIME_BITS=64 -D_GNU_SOURCE -I ttydraw
ASFLAGS = --32
LDFLAGS = $(CFLAGS) -lc -B. -Wl,-b,$(BFD_OUT_TARGET) -no-pie
NCURSES_LIBS != ./ncurses-config.sh
LDLIBS = $(NCURSES_LIBS) -lm
OBJECT_FILES = 123.o dl_init.o main.o wrappers.o patch.o filemap.o graphics.o draw.o ttydraw/ttydraw.a atfuncs/atfuncs.a forceplt.o
workdir != pwd
PATH := $(workdir):$(PATH)
KEYMAPS != echo xterm rxvt-unicode-256color xterm-256color $(TERM) | tr ' ' '\n' | sort -u
prefix = /usr/local

.PHONY: clean check distclean install uninstall

all: check 123 keymaps
	@size 123

check:
	./check-binutils-coff.sh

coffsyrup:
	$(CC) -o $@ coffsyrup.c

orig/123.o:
	@echo You need to run the extract.sh script to get the 1-2-3 files.
	@false

123.o: coffsyrup orig/123.o $(OBJCOPY_FILES)
	objcopy -I $(BFD_INP_TARGET) -O $(BFD_OUT_TARGET) $(OBJCOPY_FLAGS) orig/123.o $@
	./coffsyrup $@ $(@:.o=.tmp.o) $$(cat undefine.lst)
	mv $(@:.o=.tmp.o) $@

dl_init.o: orig/dl_init.o
	objcopy -I $(BFD_INP_TARGET) -O $(BFD_OUT_TARGET) $(OBJCOPY_FLAGS) orig/dl_init.o $@

forceplt.o: forceplt.s
	as --32 -o $@ forceplt.s

ttydraw/ttydraw.a:
	$(MAKE) -C ttydraw

atfuncs/atfuncs.a:
	$(MAKE) -C atfuncs

bin/123: $(OBJECT_FILES)
	@mkdir -p $(@D)
	$(CC) forceplt.o $(CFLAGS) $(LDFLAGS) $(OBJECT_FILES) -Wl,--whole-archive,ttydraw/ttydraw.a,atfuncs/atfuncs.a,--no-whole-archive -o $@ $(LDLIBS)

123: bin/123
	@ln -fs bin/123 $@

keymap/keymap:
	$(MAKE) -C keymap

# This generates the keymaps in a seperate directory based on the first letter.
$(KEYMAPS): keymap/keymap
	./install-keymap.sh $@

keymaps: $(KEYMAPS)

clean:
	rm -f *.o coffsyrup 123
	rm -f vgcore.* core.* core
	rm -rf bin share/lotus/keymaps
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
	rm -f "$(prefix)/bin/123"
	rm -f "$(prefix)/share/man/man1/123.1"
	rm -rf "$(prefix)/share/lotus"
