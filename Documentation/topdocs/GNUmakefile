depth = ../..

STEPMAKE_TEMPLATES=documentation tex texinfo topdocs
LOCALSTEPMAKE_TEMPLATES=lilypond ly
OUT_TEXI_FILES = $(TELY_FILES:%.tely=$(outdir)/%.texi)
HTML_FILES=$(TEXI_FILES:%.texi=$(outdir)/%.html) $(TELY_FILES:%.tely=$(outdir)/%.html)
README_TOP_FILES=NEWS AUTHORS INSTALL README
OUTTXT_FILES= $(outdir)/NEWS.txt

include $(depth)/make/stepmake.make

# FIXME ad-hoc file copy?
$(outdir)/NEWS.tely: $(top-src-dir)/Documentation/changes.tely
	cp -f $< $@

ifeq ($(out),www)
local-WWW-1: $(OUT_TEXI_FILES)
endif

.SECONDARY:
