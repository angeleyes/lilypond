# -*-makefile-*-
.PHONY: all clean dist menuify nl out scripts site TAGS tree

PYTHON = python
SCRIPTS = $(wildcard *.py *.scm) 
SCRIPTDIR = scripts

VERSION = 1.0
DISTDIR = lily-web-$(VERSION)

FILES = GNUmakefile newweb.css $(HTML) $(NON_HTML) $(SCRIPTS) 

outball = site.tar.gz

LANG =
SITE = site

ifneq ($(LANG),)
SITE = $(LANG)
mo = out/locale/$(LANG)/LC_MESSAGES/newweb.mo
po/$(LANG).po: po/newweb.pot
	msgmerge $@ $^ > $@.new
# only update upon success
	mv $@.new $@

$(mo): po/$(LANG).po
	mkdir -p $(dir $@)
	msgfmt --output=$@ $<

# Only regenerate for LANGs
$(LANG)/%.svg: site/%.svg $(mo)
	mkdir -p $(dir $@)
	LANG=$(LANG) $(PYTHON) $(SCRIPTDIR)/translate.py --remove-quotes --outdir=$(dir $@) $(@:$(LANG)/%=site/%)

# no inkscape on lilypond.org
.PRECIOUS: %.png %.svg

%.png: %.svg
	inkscape --export-png=$@- $<
	pngtopnm $@- | pnmcrop | pnmtopng > $@
	rm $@-

out/site/%.$(LANG).png: $(LANG)/%.png
#out/site/graphics/%.$(LANG).png: $(LANG)/graphics/%.png
	cp $< $@

endif

EXT = .jpeg .ly .pdf .png
HTML = $(shell find $(SITE) -name '*.html')
NON_HTML = $(shell find site -false $(EXT:%=-or -name '*%'))
TREE = $(shell cd site && find . -type d -not -name CVS)
PY = $(shell find scripts site -name '*.py')
SVG = $(shell find site -name '*.svg')

LANGUAGES = nl

all: scripts linktree menuify $(LANGUAGES)

scripts:
	python $(SCRIPTDIR)/big-page.py site/about/automated-engraving

# no silly buttons
site: all

TAGS:
	etags $$(find scripts site -name '*.html' -o -name '.py')

po/newweb.pot: $(PY) $(SVG)
	xgettext --default-domain=newweb --language=python --output=$@ $(PY) $(SVG)

nl:
	$(MAKE) LANG=$@ png menuify

png: $(SVG:site/graphics/%.svg=out/site/graphics/%.$(LANG).png)

new:
	mkdir -p $(LANG)
	cd $(LANG) && mkdir -p $(TREE)
#	$(foreach i, $(HTML), cp -i $(i) $(LANG)/$(i:site/%=%) &&) true

tree:
# Let's not.  This runs on lilypond.org, and it has no inkscape too.
#	rm -rf out/site
	mkdir -p out/site
	cd out/site && mkdir -p $(TREE)

menuify: $(mo)
	LANG=$(LANG) $(PYTHON) $(SCRIPTDIR)/format-page.py --verbose --outdir=out $(HTML)

linktree: tree
	$(foreach i, $(NON_HTML), ln -f $i out/$i &&) true
	cd out && touch .xvpics && rm -rf $$(find -name .xvpics)
	ln -f newweb.css out/site

out/$(outball): site
	cd out && tar czvf $(outball) site 


WEBSERVER=base.lilypond.org:
upload: site
	cvs commit -m 'upload commit'  
	cd out/site \
	   && chgrp -R lilypond . \
	   && chmod -R g+w * \
	   && chmod 2775 . $$(find . -type d) \
&& rsync -go --stats --progress -rltvu . $(WEBSERVER)/var/www/lilypond/web/
#	   && rsync --delete -go --stats --progress -rltvu . $(WEBSERVER)/var/www/lilypond/web/

dist:
	mkdir $(DISTDIR)
	cp --parents -av $(FILES) $(DISTDIR)
	tar cfz $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

clean:
	rm -rf out

