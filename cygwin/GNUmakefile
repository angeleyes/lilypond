depth = ..

NAME = cygwin

SEXECUTABLES=bug-lilypond-cygwin lily-wins
STEPMAKE_TEMPLATES = install install-out script

include $(depth)/make/stepmake.make

# For Cygwin builds only
ifneq ($(CYGWIN_BUILD),)

include $(stepdir)/executable-targets.make

$(outdir)/%.sh: %.sh $(config_make)
	cat $< | sed $(sed-atvariables) > $@
	chmod 755 $@

default: $(OUTIN_FILES)

endif

