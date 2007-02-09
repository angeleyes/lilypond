
.SUFFIXES: .html .info .texi .texinfo

$(outdir)/%.info: $(outdir)/%.texi
	$(MAKEINFO) -I $(outdir) --output=$@ $<

$(outdir)/%-big-page.html: $(outdir)/%.texi
	$(MAKEINFO) -I $(outdir) --output=$@ --css-include=$(top-src-dir)/Documentation/texinfo.css --html --no-split -D bigpage --no-headers $<

$(outdir)/%.html: $(outdir)/%.texi
	$(MAKEINFO) -I $(outdir) --output=$@ --css-include=$(top-src-dir)/Documentation/texinfo.css --html --no-split --no-headers $<

$(outdir)/%.html.omf: %.texi
	$(call GENERATE_OMF,html)

$(outdir)/%.pdf.omf: %.texi
	$(call GENERATE_OMF,pdf)

$(outdir)/%.ps.gz.omf: %.texi
	$(call GENERATE_OMF,ps.gz)

# Generic rule not possible?
$(outdir)/%/%.html: $(outdir)/%.texi
	$(MAKEINFO) --output=$@ --css-include=$(top-src-dir)/Documentation/texinfo.css --html $<

$(outdir)/%.pdf: $(outdir)/%.texi
	cd $(outdir); texi2pdf $(TEXI2PDF_FLAGS) --batch $(TEXINFO_PAPERSIZE_OPTION) $(<F)

$(outdir)/%.txt: $(outdir)/%.texi
	$(MAKEINFO) -I $(src-dir) -I $(outdir) --no-split --no-headers --output $@ $<

$(outdir)/%.texi: %.texi
	rm -f $@
	cp $< $@



