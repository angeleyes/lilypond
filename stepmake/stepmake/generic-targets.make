.PHONY : all clean config default diff dist doc exe help html lib TAGS\
	 po

all:	 default
	$(LOOP)

man:
	$(LOOP)

# be careful about deletion.
clean: local-clean
	-rm -f $(outdir)/*
	$(LOOP)

ifneq ($(strip $(depth)),.)
dist:
	$(MAKE) -C $(depth) dist
endif

distclean: clean
	$(LOOP)
	$(MAKE) local-distclean

cvs-clean:
	$(MAKE) local-distclean
	find . -type d -name out | xargs rm -rf
	rm -f aclocal.m4 configure

maintainerclean:
	$(LOOP)
	$(MAKE)	local-maintainerclean
	$(MAKE) local-distclean


# configure:
#
config:
	./$(src-depth)/configure
#


# target help:
#
generic-help:
	@echo -e "\
Makefile for $(PACKAGE_NAME) $(TOPLEVEL_VERSION)\n\
Usage: make ["VARIABLE=value"]... [TARGET]\n\
\n\
Targets:\n"

help: generic-help local-help
	@echo -e "\
  all         update everything\n\
  clean       remove all generated stuff in $(outdir)\n\
  check       run self tests\n\
  default     same as the empty target\n\
  exe         update all executables\n\
  help        this help\n\
  install     install programs and data (prefix=$(prefix))\n\
  lib         update all libraries\n\
  web         update website in directory \`out-www'\n\
  web-install install website documentation in (webdir=$(webdir))\n\
  web-clean   clean \`out-www' directory\n\
  TAGS        generate tagfiles\n\
\n\
\`make' may be invoked from any subdirectory.\n\
Note that all commands recurse into subdirectories;\n\
prepend \`local-' to restrict operation to the current directory.\n\
Example: \`local-clean'.\n"

local-help:

local-dist: $(DIST_FILES) $(OUT_DIST_FILES) $(NON_ESSENTIAL_DIST_FILES)
	mkdir -p $(distdir)/$(localdir)
	$(LN) $(DIST_FILES:%=$(src-dir)/%) $(distdir)/$(localdir)

	case "$(NON_ESSENTIAL_DIST_FILES)x" in x) ;; *) \
		$(LN) $(NON_ESSENTIAL_DIST_FILES:%=$(src-dir)/%) $(distdir)/$(localdir);; \
	esac
	case "$(OUT_DIST_FILES)x" in x) ;; *) \
		mkdir -p $(distdir)/$(localdir)/$(outdir); \
		$(LN) $(OUT_DIST_FILES) $(distdir)/$(localdir)/$(outdir);; \
	esac
	$(foreach i, $(SUBDIRS), $(MAKE) top-src-dir=$(top-src-dir) distdir=$(distdir) localdir=$(localdir)/$(notdir $(i)) -C $(i) local-dist &&) true



html: $(HTML_FILES)

TAGS:
	$(LOOP)
	$(MAKE) local-tags

DEEPER_TAGS_FILES = $(shell find $(pwd) -mindepth 2 -name 'TAGS')
local-tags:
	-if [ -n "$(TAGS_HEADERS)$(TAGS_SOURCES)$(DEEPER_TAGS_FILES)" ]; then \
		etags $(ETAGS_FLAGS) $(DEEPER_TAGS_FILES:%=--include=%) \
			$(TAGS_SOURCES) $(TAGS_HEADERS) $(ERROR_LOG) ; \
		ctags $(CTAGS_FLAGS) $(TAGS_SOURCES) $(TAGS_HEADERS) \
			$(ERROR_LOG) ; \
	fi

$(outdir)/version.hh: $(depth)/VERSION $(config_make) $(step-bindir)/make-version.py
	$(PYTHON) $(step-bindir)/make-version.py $< > $@

$(outdir)/config.hh: $(config_h)
	cp -p $< $@

configure: configure.in aclocal.m4
	NOCONFIGURE=yes $(src-depth)/autogen.sh
	chmod +x configure

local-clean:

local-distclean:

local-maintainerclean:

install-strip:
	$(MAKE) INSTALLPY="$(INSTALLPY) -s" install

ifeq ($(strip $(depth)),.)
final-install:
else
final-install:
	$(LOOP)

install: local-install
	$(LOOP)
endif

local-install:

uninstall: local-uninstall
	$(LOOP)

local-uninstall:

installextradoc:
	-$(INSTALLPY) -d $(DESTDIR)$(prefix)/doc/$(package)
	cp -r $(EXTRA_DOC_FILES) $(prefix)/doc/$(package)

include $(outdir)/dummy.dep $(wildcard $(outdir)/*.dep) # expect a warning here

$(outdir)/dummy.dep:
	@echo please ignore innocent warning about dummy.dep
	-mkdir -p $(outdir)
	touch $(outdir)/dummy.dep

check: local-check
	$(LOOP)

local-check:

# ugh.  ugh ugh ugh
$(config_make): $(top-src-dir)/configure
	@echo "************************************************************"
	@echo "configure changed! You should probably reconfigure manually."
	@echo "************************************************************"
	(cd $(top-build-dir); ./config.status)
	touch $@		# do something for multiple simultaneous configs.


deb:
	$(MAKE) -C $(depth)/debian
	cd $(depth) && debuild

diff:
	$(PYTHON) $(step-bindir)/package-diff.py  --outdir=$(top-src-dir)/$(outdir) --package=$(top-src-dir) $(makeflags)
	-ln -f $(depth)/$(outdir)/$(distname).diff.gz $(patch-dir)

release:
	$(PYTHON) $(step-bindir)/release.py --outdir=$(top-src-dir)/$(outdir) --package=$(top-src-dir)


################ website.

local-WWW:
local-WWW-post:
web-install:

WWW: local-WWW
	$(LOOP)

WWW-post: local-WWW-post
	$(LOOP)

web:
	$(MAKE) out=www WWW
	$(MAKE) out=www WWW-post
