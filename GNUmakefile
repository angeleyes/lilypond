SCRIPTS=$(wildcard *.py) 

site: menuify renderlys

menuify:
	python format-page.py --outdir out/  `find site -name '*.html'`
	for a in `find site -name '*.png' -or -name '*.pdf'` ; do cp $$a out/$$a  ;done 


renderlys:
	$(foreach  a, $(shell find site -name '*.ly'), \
		cp $(a) out/$(a) && \
		(cd  $(dir out/$(a) ) ; ly2dvi --preview --html --png --pdf $(notdir $(a))) && ) true

VERSION=0.0
DISTDIR=lily-web-$(VERSION)

FILES=$(SCRIPTS) GNUmakefile `find site -name '*.html' -or -name '*.py' -or -name '*.ly' -or -name '*.png'`

dist:
	mkdir $(DISTDIR)
	cp --parents -av $(FILES) $(DISTDIR)
	tar cfz $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)
clean:
	rm -rf out



