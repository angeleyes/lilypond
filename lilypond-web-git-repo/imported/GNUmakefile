depth=..
outdir = out/
#imagedir = $(depth)/site/images
imagedir = images/
exampledir = examples/

all: $(outdir)
	texi2html -I=$(imagedir) -I=$(exampledir) --output=$(outdir) --init-file=web-texi2html.init --split=node --node-files lilypond-general.texi
	cp css/* out/
	cp $(exampledir)/*.png out/
	cp $(imagedir)/*.png out/
	cp $(imagedir)/*.jpg out/
	mv $(outdir)/lilypond-general.html $(outdir)/index.html
	cp ../lilypond.org.htaccess $(outdir)/.htaccess

generate-examples:
	cd examples && make DEST=.

$(outdir):
	mkdir -p $@

clean:
	rm -rf $(outdir)

