SUBDIRS = src

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = grapevine-3dengine.pc

EXTRA_DIST = autogen.sh

# Autotools want's to install the .la next to .so as default
# so let's use Autotools to delete that .la.
install-exec-hook:
	find $(DESTDIR)$(libdir) -type f -name \*.la -delete
