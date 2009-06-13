depth=..
outdir = out/
imagedir = $(depth)/site/images

all: $(outdir)
	texi2html -I=$(imagedir) --output=$(outdir) --init-file=web-texi2html.init --split=section lilypond-general.texi
	cp *.css out/
	cp images/* out/

$(outdir):
	mkdir -p $@

clean:
	rm -rf out-www/

