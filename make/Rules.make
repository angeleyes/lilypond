#
# project  LilyPond -- the musical typesetter
# title	   generic make rules
# file	   make/Rules.make
#
# Copyright (c) 1997 by    
#   	Jan Nieuwenhuizen <jan@digicash.com>
#	Han-Wen Nienhuys <hanwen@stack.nl>

# this is supposed to clear all suffixes:
.SUFFIXES:

# so why does make still consider xx.y : RCS/xx.y,v ?
# there is no suffix ,v anymore!
.SUFFIXES: .cc .o .hh .y .l .pod .text .1 .dep .html


# compile rules:
#
$(outdir)/%.o: %.cc
	$(DO_CXX_COMPILE)

$(outdir)/%.o: $(outdir)/%.cc
	$(DO_CXX_COMPILE)

$(outdir)/%.cc: %.y
	$(BISON) $<
	mv $(shell basename $@ .cc ).tab.c $@

$(outdir)/%.hh: %.y
	$(BISON) -d $<
	mv $(shell basename $@ .hh ).tab.h $@
	mv $(shell basename $@ .hh ).tab.c $(outdir)/$(shell basename $@ .hh).cc

$(outdir)/%.cc: %.l
	$(FLEX) -Cfe -p -p -t $< > $@
# could be faster:
#	$(FLEX) -8 -Cf -t $< > $@

# outdirs:
#
# ?$(outdir)/%.dep:
%.dep:
	touch $@


# build and config stuff: (could make this generic default rule...)
#
%/.build:
	echo 0 > $@


$(depth)/%.text: check-doc-deps
	rm -f $@
	ln `find ${depth}/Documentation -name $@|head -1` .
