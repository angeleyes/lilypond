.PHONY: all clean dist menuify scripts site TAGS tree

SCRIPTS=$(wildcard *.py *.scm) 

LILYPONDDIR=$(HOME)/usr/src/lilypond
LY2DVI=python $(LILYPONDDIR)/scripts/ly2dvi.py
SCRIPTDIR=./


all: scripts menuify

scripts:
	cd site/about/automated-engraving ; python $(SCRIPTDIR)/big-page.py 

# no silly buttons
site: tree menuify

TAGS:
	etags `find site -name '*.html'`

menuify: tree
	python $(SCRIPTDIR)/format-page.py --outdir out/  `find site -name '*.html'`
	for a in `find site -name '*.ly' -or -name '*.png' -or -name '*.jpeg' -or -name '*.pdf'` ; do   ln $$a out/$$a  ;done
	cd out ; touch .xvpics ; rm -rf `find -name  .xvpics`
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


WEBSERVER=base.lilypond.org:
upload: site
	cvs commit -m 'upload commit'   
	cd out/site && 	\
	chgrp -R lilypond .  && \
	chmod -R g+w * && \
	chmod 2775 . `find  -type d` && \
	rsync --delete -go --stats --progress -rltvu . $(WEBSERVER)/var/www/lilypond/web/


dist:
	mkdir $(DISTDIR)
	cp --parents -av $(FILES) $(DISTDIR)
	tar cfz $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

clean:
	rm -rf out

