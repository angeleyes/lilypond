depth=..
outdir = out/
#imagedir = $(depth)/site/images
imagedir = images/
exampledir = examples/

all: $(outdir)
	texi2html -I=$(imagedir) -I=$(exampledir) --output=$(outdir) --init-file=web-texi2html.init --split=node --node-files lilypond-general.texi
	cp css/* $(outdir)
#	cp $(exampledir)/*.png $(outdir)
#	cp $(imagedir)/*.png $(outdir)
#	cp $(imagedir)/*.jpg $(outdir)
	mv $(outdir)/lilypond-general.html $(outdir)/index.html
	cp ../lilypond.org.htaccess $(outdir)/.htaccess

pdf: $(outdir)
	cp css/* $(outdir)
	cp $(exampledir)/*.png $(outdir)
	cp $(imagedir)/*.png $(outdir)
	cp $(imagedir)/*.jpg $(outdir)
	cp *texi $(outdir)
	cd $(outdir) && texi2pdf -p lilypond-general.texi

generate-examples:
	cd examples && make DEST=.

$(outdir):
	mkdir -p $@

clean:
	rm -rf $(outdir)

