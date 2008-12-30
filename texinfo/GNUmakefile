depth=..
outdir = out
imagedir = $(depth)/site/images

all: $(outdir)
	texi2html -I=$(imagedir) --output=$(outdir) --split=section newweb.tely 

$(outdir):
	mkdir -p $@

clean:
	rm -rf out-www/

