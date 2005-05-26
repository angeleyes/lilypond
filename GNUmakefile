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
	msgmerge --update $@ $^

$(mo): po/$(LANG).po
	mkdir -p $(dir $@)
	msgfmt --output=$@ $<

# Only regenerate for LANGs
$(LANG)/%.svg: site/%.svg $(mo) scripts/translate.py GNUmakefile
	mkdir -p $(dir $@)
	LANG=$(LANG) $(PYTHON) $(SCRIPTDIR)/translate.py --remove-quotes --outdir=$(dir $@) $(@:$(LANG)/%=site/%)

# no inkscape on lilypond.org
.PRECIOUS: %.png %.svg

INKSCAPE = inkscape

%.png: %.svg GNUmakefile
	-$(INKSCAPE) --export-png=$@- --export-background-opacity=0 $<
	-convert -crop 0x0 $@- $@ && rm $@-

out/site/%.$(LANG).png: $(LANG)/%.png
	-cp $< $@

endif

EXT = .jpeg .ly .pdf .png
HTML = $(shell find $(SITE) -name '*.html')
IHTML = $(shell find site -name '*.ihtml')
NON_HTML = $(shell find site -false $(EXT:%=-or -name '*%'))
TREE = $(shell cd site && find . -type d -not -name CVS)
PY = $(shell find scripts site -name '*.py')
SVG = $(shell find site -name '*.svg')

# Do not publish non-polished or non-finished or outdated translations.
LANGUAGES = nl

all: scripts linktree menuify $(LANGUAGES) apache-1.3.x-fixup

# In Apache 1.3.x, .nl always has a higher LanguagePriority than the
# empty content-language, everyone will see the Dutch pages.
# Adding .en pages links fixes that.
apache-1.3.x-fixup:
	rm -f $$(find out/site -name '*.en.html')
	for i in $$(find out/site -name '*.html' | grep -v '\...\.html'); do\
		b=$$(basename $$i .html);\
		ln -sf $$b.html $$(dirname $$i)/$$b.en.html;\
	done;
	rm -f $$(find out/site -name '*.en.png')
	for i in $$(find out/site -name '*.png' | grep -v '\...\.png'); do\
		b=$$(basename $$i .png);\
		ln -sf $$b.html $$(dirname $$i)/$$b.en.png;\
	done;

scripts:
	python $(SCRIPTDIR)/big-page.py site/about/automated-engraving

# no silly buttons
site: all

TAGS:
	etags $$(find scripts site -name '*.html' -o -name '.py')

po/newweb.pot: $(PY) $(SVG) $(IHTML)
	xgettext --default-domain=newweb --language=python --keyword=_ --join --output=$@ $(PY) $(SVG)
	xgettext --default-domain=newweb --language=c --keyword=_ --keyword=_@ --join --output=$@  $(IHTML)

nl:
	$(MAKE) LANG=$@ png menuify

check-translation:
	python $(SCRIPTDIR)/check-translation.py $(HTML)

png: $(SVG:site/graphics/%.svg=out/site/graphics/%.$(LANG).png)

new:
	mkdir -p $(LANG)
	cd $(LANG) && mkdir -p $(TREE)
#	$(foreach i, $(HTML), cp -i $(i) $(LANG)/$(i:site/%=%) &&) true

tree:
	rm -rf out/site
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


WEBSERVER=lilypond.org:
upload: site
	cvs commit -m 'upload commit'  
	cd out/site \
	   && chgrp -R lilypond . \
	   && chmod -R g+w * \
	   && chmod 2775 . $$(find . -type d) \
	   && rsync -go --stats --progress -rltvu . $(WEBSERVER)/var/www/lilypond/web/
# do not --delete, lilypond.org has no inkscape, transated pngs will be removed
#	   && rsync --delete -go --stats --progress -rltvu . $(WEBSERVER)/var/www/lilypond/web/

dist:
	mkdir $(DISTDIR)
	cp --parents -av $(FILES) $(DISTDIR)
	tar cfz $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

clean:
	rm -rf out

foe:
	echo $(IHTML)
