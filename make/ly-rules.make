# Mudela_rules.make

.SUFFIXES: .doc .dvi .mudtex .tely .texi .ly


$(outdir)/%.latex: %.doc
	-chmod a+w $@
	$(PYTHON) $(LILYPOND_BOOK) $(LILYPOND_BOOK_INCLUDES) $(LILYPOND_BOOK_FLAGS) --verbose --dependencies --outdir=$(outdir) $<
	chmod -w $@

# don't do ``cd $(outdir)'', and assume that $(outdir)/.. is the src dir.
# it is not, for --srcdir builds
$(outdir)/%.texi: %.tely
	-chmod a+w $@
	set|egrep '(TEX|LILY)'  # ugh, what's this?
	$(PYTHON) $(LILYPOND_BOOK) $(LILYPOND_BOOK_INCLUDES) --dependencies --outdir=$(outdir) --format=$(LILYPOND_BOOK_FORMAT) --verbose $(LILYPOND_BOOK_FLAGS) $<
	chmod -w $@

$(outdir)/%.texi: $(outdir)/%.tely
	-chmod a+w $@
# debugging:
#	set|egrep '(TEX|LILY)'
	$(PYTHON) $(LILYPOND_BOOK) $(LILYPOND_BOOK_INCLUDES) --dependencies --outdir=$(outdir) --format=$(LILYPOND_BOOK_FORMAT) --verbose $(LILYPOND_BOOK_FLAGS) $<
#
# DON'T REMOVE SOURCE FILES, otherwise the .TEXI ALWAYS OUT OF DATE.
#	rm -f $<
	chmod -w $@

# nexi: no-lily texi
# for plain info doco: don't run lily
$(outdir)/%.nexi: %.tely
	-chmod a+w $@
	$(PYTHON) $(LILYPOND_BOOK) $(LILYPOND_BOOK_INCLUDES) --dependencies --outdir=$(outdir) --format=$(LILYPOND_BOOK_FORMAT) --no-lily $(LILYPOND_BOOK_FLAGS) -o $(notdir $@) $<
	chmod -w $@

# nfo: info from non-lily texi
$(outdir)/%.info: $(outdir)/%.nexi
	$(MAKEINFO) --output=$(outdir)/$(*F).info $<

# nfo: info from non-lily texi
#$(outdir)/%.nfo: $(outdir)/%.nexi
#	$(MAKEINFO) --output=$(outdir)/$(*F).info $<

#
# let's not do this: this interferes with the lilypond-book dependency mechanism.
#
##$(outdir)/%.tex: $(outdir)/%.ly
##	$(LILYPOND) $(LILYPOND_BOOK_INCLUDES) -o $@ $< 

#
# Timothy's booklet
#
$(outdir)/%-book.ps: $(outdir)/%.ps
	psbook $< $<.tmp
	pstops  '2:0L@.7(21cm,0)+1L@.7(21cm,14.85cm)' $<.tmp $@

$(outdir)/%.pdf: $(outdir)/%.dvi
	dvips -u lilypond.map -o $@.pdfps  -Ppdf $<
	ps2pdf $@.pdfps $@


$(outdir)/%.html.omf: %.tely
	$(call GENERATE_OMF,html)

$(outdir)/%.pdf.omf: %.tely
	$(call GENERATE_OMF,pdf)

$(outdir)/%.ps.gz.omf: %.tely
	$(call GENERATE_OMF,ps.gz) 


$(outdir)/%.html.omf: $(outdir)/%.texi
	$(call GENERATE_OMF,html)

$(outdir)/%.pdf.omf: $(outdir)/%.texi
	$(call GENERATE_OMF,pdf)

$(outdir)/%.ps.gz.omf: $(outdir)/%.texi
	$(call GENERATE_OMF,ps.gz) 
