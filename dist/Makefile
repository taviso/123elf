PKGNAME = lotus123r3
VERSION = 1.0
REV = 1

all: $(PKGNAME)_$(VERSION)-$(REV)_i386.deb

%.deb: control
	mkdir -m0755 -p $(@:.deb=)/DEBIAN
	cp control $(@:.deb=)/DEBIAN
	$(MAKE) -C .. install prefix="$(CURDIR)/$(@:.deb=)/usr"
	dpkg-deb --build --root-owner-group $(@:.deb=)

clean:
	$(RM) *.deb
	$(RM) -r $(PKGNAME)_$(VERSION)-$(REV)_i386
