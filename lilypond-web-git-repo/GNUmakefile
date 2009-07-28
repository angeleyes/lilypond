outdir = out/
exampledir = generated-examples/
imagedir = imported/images/

all: $(outdir)
	cd imported/
	# disgusting hackery with outdir
	texi2html -I=$(imagedir) -I=$(exampledir) --output=../$(outdir) --init-file=web-texi2html.init --split=node --node-files lilypond-general.texi
	cp css/* out/
	cp $(exampledir)/*.png out/
	cp $(imagedir)/*.png out/
	cp $(imagedir)/*.jpg out/
	mv $(outdir)/lilypond-general.html $(outdir)/index.html
	cp ../lilypond.org.htaccess $(outdir)/.htaccess

generate-examples:
	cd imported/examples && make DEST=../../generated-examples

$(outdir):
	mkdir -p $@

clean:
	rm -rf $(outdir)

