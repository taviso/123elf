BFD_INP_TARGET = coff-i386
BFD_OUT_TARGET = coff-i386
OBJCOPY_FLAGS  = --wildcard --localize-symbols=localize.lst --globalize-symbols=globalize.lst --redefine-syms=redefine.lst
OBJCOPY_FILES = localize.lst globalize.lst redefine.lst undefine.lst
CFLAGS  = -m32 -ggdb3 -O0 -fno-stack-protector
CPPFLAGS = -D_FILE_OFFSET_BITS=64 -D_TIME_BITS=64 -D_GNU_SOURCE -I ttydraw
ASFLAGS = --32
LDFLAGS = $(CFLAGS) -lc -B. -Wl,-b,$(BFD_OUT_TARGET) -no-pie
LDLIBS = -lncurses -ltinfo -lm
PATH := .:$(PATH)
KEYMAPS = xterm rxvt-unicode-256color xterm-256color $(TERM)
prefix = /usr/local

define BFD_TARGET_ERROR
Your version of binutils was compiled without coff-i386 target support.
You can try running ./binutils.sh to build a version that does support it.
endef

export BFD_TARGET_ERROR

.PHONY: clean check distclean install uninstall

all: check 123 keymaps
	@size 123

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
	$(MAKE) -C ttydraw

atfuncs/atfuncs.a:
	$(MAKE) -C atfuncs

bin/123: 123.o dl_init.o main.o wrappers.o patch.o filemap.o graphics.o draw.o | ttydraw/ttydraw.a atfuncs/atfuncs.a forceplt.o
	@mkdir -p $(@D)
	$(CC) forceplt.o $(CFLAGS) $(LDFLAGS) $^ -Wl,--whole-archive,ttydraw/ttydraw.a,atfuncs/atfuncs.a,--no-whole-archive -o $@ $(LDLIBS)

123: bin/123
	@ln -fs $^ $@

keymap/keymap:
	$(MAKE) -C keymap

# This generates the keymaps in a seperate directory based on the first letter.
$(sort $(KEYMAPS)): keymap/keymap
	mkdir -p share/lotus/keymaps/$(shell printf "%c" $@)
	keymap/keymap $@ > share/lotus/keymaps/$(shell printf "%c" $@)/$@

keymaps: $(KEYMAPS)

clean:
	$(RM) *.o coffsyrup 123
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
	install -m 0755 -d "$(prefix)/bin"
	install -m 0755 "bin/123" "$(prefix)/bin/123"
	install -m 0755 -d "$(prefix)/share/man/man1"
	install -m 0644 "share/man/man1/123.1" "$(prefix)/share/man/man1/123.1"
	install -m 0755 -d "$(prefix)/share/lotus/etc"
	install -m 0644 "share/lotus/etc/l123set.cf" "$(prefix)/share/lotus/etc/l123set.cf"
	find "share/lotus/keymaps" -type d -exec install -m 0755 -d "$(prefix)/{}" \;
	find "share/lotus/keymaps" -type f -exec install -m 0644 {} "$(prefix)/{}" \;
	install -m 0755 -d "$(prefix)/share/lotus/123.v10/sysV386"
	install -m 0644 "share/lotus/123.v10/sysV386/lotus.bcf" "$(prefix)/share/lotus/123.v10/sysV386/lotus.bcf"
	install -m 0755 -d "$(prefix)/share/lotus/123.v10/sysV386/lib"
	install -m 0644 "share/lotus/123.v10/sysV386/lib/wyse50-lts123" "$(prefix)/share/lotus/123.v10/sysV386/lib/wyse50-lts123"
	find "share/lotus/123.v10/cbd" -type d -exec install -m 0755 -d "$(prefix)/{}" \;
	find "share/lotus/123.v10/cbd" -type f -exec install -m 0644 {} "$(prefix)/{}" \;
	find "share/lotus/123.v10/fonts" -type d -exec install -m 0755 -d "$(prefix)/{}" \;
	find "share/lotus/123.v10/fonts" -type f -exec install -m 0644 {} "$(prefix)/{}" \;
	find "share/lotus/123.v10/hlp" -type d -exec install -m 0755 -d "$(prefix)/{}" \;
	find "share/lotus/123.v10/hlp" -type f -exec install -m 0644 {} "$(prefix)/{}" \;
	find "share/lotus/123.v10/keymaps" -type d -exec install -m 0755 -d "$(prefix)/{}" \;
	find "share/lotus/123.v10/keymaps" -type f -exec install -m 0644 {} "$(prefix)/{}" \;
	find "share/lotus/123.v10/pbd" -type d -exec install -m 0755 -d "$(prefix)/{}" \;
	find "share/lotus/123.v10/pbd" -type f -exec install -m 0644 {} "$(prefix)/{}" \;
	find "share/lotus/123.v10/ri" -type d -exec install -m 0755 -d "$(prefix)/{}" \;
	find "share/lotus/123.v10/ri" -type f -exec install -m 0644 {} "$(prefix)/{}" \;
	find "share/lotus/123.v10/smpfiles" -type d -exec install -m 0755 -d "$(prefix)/{}" \;
	find "share/lotus/123.v10/smpfiles" -type f -exec install -m 0644 {} "$(prefix)/{}" \;

uninstall:
	rm -f "$(prefix)/bin/123"
	rm -f "$(prefix)/share/man/man1/123.1"
	rm -rf "$(prefix)/share/lotus"
