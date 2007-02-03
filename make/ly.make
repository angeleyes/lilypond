#!/usr/bin/make
#
# Mutopia Makefile Project
#
# Rename this file to GNUmakefile, and issue `make help'
#


#
# Magic: find and include LilyPond's StepMake rules
#
# 0: try local tree
# 1: follow LILYPOND_DATADIR
# 2: try source tree in home
# 3: try installed tree in $HOME
# 4: try system installed tree
# 5: try system installed tree
#
make-root=$(wildcard $(depth)/make)
make-root?=$(wildcard $(LILYPOND_DATADIR)/make)
make-root?=$(wildcard $(HOME)/usr/src/lilypond/make)
make-root?=$(wildcard $(HOME)/usr/share/lilypond/make)
make-root?=$(wildcard /usr/share/lilypond/make)
make-root?=$(wildcard /usr/local/share/lilypond/make)
#make-root=<LilyPond's datadir>/make
ifneq ($(make-root),)
### some versions apparently choke on $(message)
### $(message running from $(make-root))
depth:=$(make-root)/..
LOCALSTEPMAKE_TEMPLATES=ly mutopia
include $(make-root)/stepmake.make
else
$(error can't find LilyPond's stepmake installation)
endif
#


#
# Mutopia/user targets.
# This needs some work.
#

#
# Name of mutopia project
#
name=book
tarball=$(name)
parts=$(patsubst %.ly,%,$(wildcard *-part.ly))

#
# scores for target local-WWW (duh)
#
examples=

#
# scores for target mutopia
#
mutopia-examples=$(name) $(parts)

# moved to ly-rules.make
# Timothy's booklet
#
#$(outdir)/%-book.ps: $(outdir)/%.ps
#	psbook $< $<.tmp
#	pstops  '2:0L@.7(21cm,0)+1L@.7(21cm,14.85cm)' $<.tmp $@

#
# Catch-all target: type `make foo' to make out/foo.ps,
# or make `foo-book' to make out/foo-book.ps
#
%: $(outdir)/%.ps
	@echo Generated $< for target $@.

#
# Also clean hand-compiled stuff in cwd
#
local-clean: local-auto-gen-clean

# Compose string from two parts: must not remove myself.
auto-gen-tag=Generated
auto-gen-tag+= automatically by

local-auto-gen-clean:
	rm -f `grep -l '$(auto-gen-tag)' *`
	rm -f *.dvi *.png

