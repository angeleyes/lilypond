

all: $(OUT_FILES)


local-WWW: $(ly_examples) $(fly_examples) $(ps_examples) $(gif_examples)
	(cd $(outdir); $(PYTHON) ../$(buildscripts)/mutopia-index.py --package=$(topdir) --prefix=../ --suffix=/$(outdir) $(html_subdirs) $(all_examples))
	$(PYTHON) $(step-bindir)/add-html-footer.py --package=$(topdir) --index=$(depth)/$(outdir)/index.html $(outdir)/index.html
	echo $^ > $(depth)/wwwlist
