.PHONY: site

SCRIPTS=$(wildcard *.py *.scm) 

all: site

site: menuify renderlys buttons

menuify:
	python format-page.py --outdir out/  `find site -name '*.html'`
	for a in `find site -name '*.png' -or -name '*.pdf'` ; do cp $$a out/$$a  ;done
	cp newweb.css out/site


renderlys:
	$(foreach  a, $(shell find site -name '*.ly'), \
		cp $(a) out/$(a) && \
		(cd  $(dir out/$(a) ) ; ly2dvi --preview --html --png --pdf $(notdir $(a))) && ) true

VERSION=0.0
DISTDIR=lily-web-$(VERSION)

FILES=$(SCRIPTS) GNUmakefile newweb.css `find site -name '*.html' -or -name '*.py' -or -name '*.ly' -or -name '*.png'`

outball=site.tar.gz

buttons:
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

