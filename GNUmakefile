# -*-makefile-*-
.PHONY: all clean dist menuify nl out scripts site TAGS tree

PYTHON=python
SCRIPTS=$(wildcard *.py *.scm) 
SCRIPTDIR=scripts

VERSION=1.0
DISTDIR=lily-web-$(VERSION)

FILES = GNUmakefile newweb.css $(HTML) $(NON_HTML) $(SCRIPTS) 

outball=site.tar.gz

SITE = site
EXT = .jpeg .ly .pdf .png
##HTML = $(shell cd site && find . -name '*.html')
##HTML = $(shell find $(SITE) -name '*.html')
HTML_SITE = $(shell find site -name '*.html')
HTML = $(HTML_SITE:site/%=$(SITE)/%)
NON_HTML = $(shell cd site && find . -false $(EXT:%=-or -name '*%'))
##TREE = $(shell find $(SITE) -type d -not -name CVS)
TREE_SITE = $(shell find site -type d -not -name CVS)
TREE = $(TREE_SITE:site/%=$(SITE)/%)
PY = $(shell find scripts site -name '*.py')

ifneq ($(SITE),site)
mo = out/locale/$(SITE)/LC_MESSAGES/newweb.mo
endif


all: scripts $(mo) menuify

scripts:
	python $(SCRIPTDIR)/big-page.py site/about/automated-engraving

# no silly buttons
site: tree menuify

TAGS:
	etags $$(find site -name '*.html')

dummy:

po/newweb.pot: dummy
	xgettext --default-domain=newweb --output-dir=po $(PY)
	#mv po/newweb.po $@

po/$(SITE).po: po/newweb.pot
	msgmerge $@ $^ > $@.new
	# only update upon success
	mv $@.new $@

$(mo): po/$(SITE).po
	mkdir -p $(dir $@)
	msgfmt --output=$@ $< 

nl: all
	$(MAKE) SITE=nl all

new:
	mkdir -p $(TREE:site/%=$(LANG)/%)
#	$(foreach i, $(HTML), cp -i $(i) $(LANG)/$(i:site/%=%) &&) true

tree:
	rm -rf out/$(SITE)
#	mkdir -p $(TREE:%=out/$(SITE)/%)
	mkdir -p $(TREE:%=out/%)

menuify: tree
	$(foreach i, $(NON_HTML), \
		(ln $(SITE)/$i out/$(SITE)/$i 2>/dev/null \
			|| ln site/$i out/$(SITE)/$i) &&) true
	LANG=$(SITE) $(PYTHON) $(SCRIPTDIR)/format-page.py --outdir=out $(HTML)
	cd out && touch .xvpics && rm -rf $$(find -name .xvpics)
	ln newweb.css out/$(SITE)

out/$(outball): site
	cd out && tar czvf $(outball) site 


WEBSERVER=base.lilypond.org:
upload: site
	cvs commit -m 'upload commit'  
	cd out/site \
	   && chgrp -R lilypond . \
	   && chmod -R g+w * \
	   && chmod 2775 . $$(find . -type d) \
	   && rsync --delete -go --stats --progress -rltvu . $(WEBSERVER)/var/www/lilypond/web/

dist:
	mkdir $(DISTDIR)
	cp --parents -av $(FILES) $(DISTDIR)
	tar cfz $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

clean:
	rm -rf out

