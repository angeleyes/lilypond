.PHONY: site

SCRIPTS=$(wildcard *.py *.scm) 

LILYPONDDIR=$(HOME)/usr/src/lilypond
LY2DVI=python $(LILYPONDDIR)/scripts/ly2dvi.py

all: menuify

# no silly buttons
site: menuify renderlys # buttons

menuify:
	python format-page.py --outdir out/  `find site -name '*.html'`
	for a in `find site -name '*.png' -or -name '*.jpg' -or -name '*.pdf'` ; do cp $$a out/$$a  ;done
	cp newweb.css out/site

tree:
	-mkdir out
	tar cf - `find site -type d`  |tar  -C out -xf -


renderlys:
	$(foreach  a, $(shell find site -name '*.ly'), \
		cp $(a) out/$(a) && \
		(cd  $(dir out/$(a) ) ; $(LY2DVI) --preview --html --png --pdf $(notdir $(a))) && ) true

VERSION=0.0
DISTDIR=lily-web-$(VERSION)

FILES=$(SCRIPTS) GNUmakefile newweb.css `find site -name '*.html' -or -name '*.py' -or -name '*.ly' -or -name '*.png'`

outball=site.tar.gz

silly-buttons:
	mkdir -p out/images
	cd out/images && \
		cat ../buttons | \
		while read i; do \
			if [ ! -f $$i.png ]; then \
				gimp -s -b '(lily-button "'$$i'" 0)' \
					'(gimp-quit 0)'; \
				gimp -s -b '(lily-button "'$$i'" 1)' \
					'(gimp-quit 0)'; \
			fi \
		done

BUTTONS = $(shell cat out/buttons | sed -e s/^/\"/ -e s/$$/\"/)

buttons:
	mkdir -p out/images
	cd out/images && \
		gimp -c -s -b '(lily-buttons (list $(BUTTONS)))' '(gimp-quit 0)'

out/$(outball): site
	cd out && tar czvf $(outball) site images

upload: out/$(outball)
	scp out/$(outball) lilypond.org:/var/www/lilypond/newweb/out
	ssh lilypond.org 'cd /var/www/lilypond/newweb/out; tar vxzf $(outball)'

dist:
	mkdir $(DISTDIR)
	cp --parents -av $(FILES) $(DISTDIR)
	tar cfz $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

clean:
	rm -rf out

