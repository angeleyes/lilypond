depth=../..
lilypond_command_preview = lilypond -dpreview -dresolution=101
lilypond_command = lilypond --png -dresolution=101
mytmp = /tmp/lily-website/

all:
	mkdir -p $(mytmp)
	cp *.ly $(mytmp)
	cp *.ily $(mytmp)
	cd $(mytmp) && for f in *\.ly; do cd $(mytmp) && $(lilypond_command_preview) $$f; done;
	cd $(mytmp) && for f in *\.ly; do cd $(mytmp) && $(lilypond_command) $$f; done;
	cp $(mytmp)/*.png .

clean:
	rm -rf $(mytmp)


