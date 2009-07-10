depth=..
outdir = out/
#imagedir = $(depth)/site/images
imagedir = images/
exampledir = examples/

all: $(outdir)
	texi2html -I=$(imagedir) -I=$(exampledir) --output=$(outdir) --init-file=web-texi2html.init --split=section lilypond-general.texi
	cp css/* out/
	cp $(exampledir)/*.png out/
	cp $(imagedir)/* out/

generate-examples:
	cd examples && make;

$(outdir):
	mkdir -p $@

clean:
	rm -rf out-www/

