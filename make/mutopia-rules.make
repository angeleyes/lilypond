

$(outdir)/%.ly.txt: %.ly
	ln -f $< $@

$(outdir)/%.ly.txt: $(outdir)/%.ly
	cp -f $< $@

$(outdir)/%.ly.txt: %.abc
#which file to show here -- abc seems more cute?
	ln -f $< $@

$(outdir)/%.ly: %.abc
	$(PYTHON) $(ABC2LY) --strict -o $@ $<

# hmm. notdir builds src-dir builds?
$(outdir)/%.png $(outdir)/%.pdf $(outdir)/%.ly $(outdir)/%.ps: $(outdir)/%.ly
	cd $(outdir); $(LILYPOND_BINARY) --pdf --png -danti-alias-factor=2 -ddelete-intermediate-files -dno-point-and-click -I $(src-dir)/ $(notdir $<)
	touch $(outdir)/$(basename $(notdir $<)).png

$(outdir)/%.ly: %.ly
	cp $< $@


