# -*-makefile-*-

# Do not publish non-polished or non-finished or outdated translations.
LANGUAGES = nl fr

.PHONY: all clean dist menuify out scripts site TAGS tree $(LANGUAGES)

PYTHON = python
SCRIPTS = $(wildcard scripts/*.py scripts/*.scm scripts/*.sh)
SCRIPTDIR = scripts

VERSION = $(shell expr "$$(grep 'Revision: [0-9]\+' site/index.html)" : '.*Revision: \([0-9]\+\.[0-9]\+\)')
DISTDIR = lily-web-$(VERSION)

READMES = ChangeLog README TRANSLATION
SITE_HTML = $(shell find site -name '*.html')
LOCAL_HTML = $(shell find fr nl -name '*.html')
FILES = GNUmakefile newweb.css \
 $(SITE_HTML) $(IHTML) $(LOCAL_HTML) $(NON_HTML) $(READMES) $(SCRIPTS)
MAKE_LANGUAGE=	$(MAKE) LANG=$@ png menuify DOWNLOAD_URL="$(DOWNLOAD_URL)"

outball = site.tar.gz

LANG =
SITE = site

DOWNLOAD_URL = 'http://lilypond.org/download'
INKSCAPE = inkscape

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


%.png: %.svg GNUmakefile
	-$(INKSCAPE) --export-area-drawing --export-png=$@ --export-background-opacity=0 $<

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
		ln -sf $$b.png $$(dirname $$i)/$$b.en.png;\
	done;

scripts:
	python $(SCRIPTDIR)/big-page.py site/about/automated-engraving
	python $(SCRIPTDIR)/big-page.py fr/about/automated-engraving

# no silly buttons
site: all

TAGS:
	etags $$(find scripts site -name '*.html' -o -name '.py')

po/newweb.pot: $(PY) $(SVG) $(IHTML)
	xgettext --default-domain=newweb --language=python --keyword=_ --join --output=$@ $(PY) $(SVG)
	xgettext --default-domain=newweb --language=c --keyword=_ --keyword=_@ --join --output=$@  $(IHTML)



define LANGUAGE_template
$(1):
	$(MAKE) LANG=$(1) png menuify DOWNLOAD_URL="$(DOWNLOAD_URL)"
endef

$(foreach lang,$(LANGUAGES),$(eval $(call LANGUAGE_template,$(lang))))

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
	$(PYTHON) $(SCRIPTDIR)/versiondb.py --download --dbfile lilypond.versions --url http://download.linuxaudio.org/lilypond/
	LANG=$(LANG) $(PYTHON) $(SCRIPTDIR)/format-page.py --version-db lilypond.versions --verbose $(FMP_OPTIONS) --outdir=out $(HTML)

linktree: tree
	$(foreach i, $(NON_HTML), ln -f $i out/$i &&) true
	cd out && touch .xvpics && rm -rf $$(find -name .xvpics)
	ln -f newweb.css out/site

out/$(outball): site
	cd out && tar czvf $(outball) site 


WEBSERVER=lilypond.org:
upload: site
	$(if $(CVS_COMMIT), cvs commit -m 'upload commit', true)   
	-cd out/site \
	   && chgrp -R lilypond . \
	   && chmod -R g+w * \
	   && chmod 2775 . $$(find . -type d)
	cd out/site \
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
