.PHONY: site

SCRIPTS=$(wildcard *.py *.scm) 

LILYPONDDIR=$(HOME)/usr/src/lilypond
LY2DVI=python $(LILYPONDDIR)/scripts/ly2dvi.py

all: scripts menuify

scripts:
	cd site/about/automated-engraving ; python big-page.py 

# no silly buttons
site: tree menuify

menuify: tree
	python format-page.py --outdir out/  `find site -name '*.html'`
	for a in `find site -name '*.ly' -or -name '*.png' -or -name '*.jpeg' -or -name '*.pdf'` ; do   ln $$a out/$$a  ;done
	ln newweb.css out/site

tree:
	rm -rf out
	mkdir out
	for a in `find site -type d -not -name CVS `; do mkdir out/$$a; done 



VERSION=1.0
DISTDIR=lily-web-$(VERSION)

FILES=$(SCRIPTS) GNUmakefile newweb.css `find site -name '*.html' -or -name '*.py' -or -name '*.ly' -or -name '*.png'`

outball=site.tar.gz


out/$(outball): site
	cd out && tar czvf $(outball) site 


WEBSERVER=base.lilypond.org
upload: site
	cvs commit -m 'upload commit'   
	cd out/site && 	\
	chgrp -R lilypond .  && \
	chmod -R g+w * && \
	chmod 2775 . `find  -type d` && \
	rsync --delete -go --stats --progress -rltvu -e ssh . $(WEBSERVER):/var/www/lilypond/web/


dist:
	mkdir $(DISTDIR)
	cp --parents -av $(FILES) $(DISTDIR)
	tar cfz $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

clean:
	rm -rf out

