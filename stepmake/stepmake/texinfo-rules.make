
.SUFFIXES: .html .info .texi .texinfo

$(outdir)/%.info: $(outdir)/%.texi
	$(MAKEINFO) -I $(outdir) --output=$@ $<

$(outdir)/%.html: $(outdir)/%.texi
	$(MAKEINFO) -I $(outdir) --output=$@ --html --no-split --no-headers $<

$(outdir)/%.html.omf: %.texi
	$(call GENERATE_OMF,html)

$(outdir)/%.pdf.omf: %.texi
	$(call GENERATE_OMF,pdf)

$(outdir)/%.ps.gz.omf: %.texi
	$(call GENERATE_OMF,ps.gz) 

# Generic rule not possible?
$(outdir)/%/%.html: $(outdir)/%.texi 
	$(MAKEINFO) -I $(outdir) --output=$@ --html --no-split --no-headers $<
	$(MAKEINFO) --output=$@ --html $<


$(outdir)/%.dvi: $(outdir)/%.texi
	cd $(outdir); texi2dvi --batch $(TEXINFO_PAPERSIZE_OPTION) $(<F)

$(outdir)/%.txt: $(outdir)/%.texi
	$(MAKEINFO) -I $(pwd) -I $(outdir) --no-split --no-headers --output $@ $<

$(outdir)/%.texi: %.texi
	rm -f $@
	cp $< $@
	chmod -w $@



