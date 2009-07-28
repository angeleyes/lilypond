depth=../..
lilypond_command_preview = lilypond -dpreview -dresolution=150
lilypond_command = lilypond --png -dresolution=101
mytmp = /tmp/lily-website/

all: images previews

images:
	mkdir -p $(mytmp)
	cp *.ly $(mytmp)
	cp *.ily $(mytmp)
	cd $(mytmp) && for f in *\.ly; do cd $(mytmp) && $(lilypond_command_preview) $$f; done;
	cp $(mytmp)/*.png $(DEST)

clean:
	rm -rf $(mytmp)

SCALE=`for PREVIEWFILE in *preview.png ; \
	do STEM=$$(basename "$$PREVIEWFILE" .preview.png) && \
	pngtopnm $$PREVIEWFILE > $$STEM.pnm && \
	pnmscale -w=600 $$STEM.pnm > $$STEM-small.pnm && \
	pnmtopng $$STEM-small.pnm > $$STEM-small.png && \
	mv "$$PREVIEWFILE" $$STEM.png ; done`

previews:
	cd $(DEST) && $(SCALE) && rm *.pnm

