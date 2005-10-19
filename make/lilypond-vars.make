##
## settings to run LilyPond

# environment settings.
export PATH:=$(top-build-dir)/lily/$(outconfbase):$(top-build-dir)/buildscripts/$(outconfbase):$(top-build-dir)/scripts/$(outconfbase):$(PATH):
export LILYPONDPREFIX:=$(build_lilypond_datadir)/$(TOPLEVEL_VERSION)
export PYTHONPATH:=$(top-build-dir)/python/$(outconfbase):$(PYTHONPATH)
export DVIPSHEADERS:=$(top-build-dir)/mf/out::

the-script-dir=$(wildcard $(script-dir))

ABC2LY = $(script-dir)/abc2ly.py
CONVERT_LY = $(script-dir)/convert-ly.py
LILYPOND = $(top-build-dir)/lily/$(outconfbase)/lilypond
LILYPOND_BOOK = $(script-dir)/lilypond-book.py
LILYPOND_BOOK_INCLUDES = -I $(src-dir)/ -I $(outdir) -I$(input-dir) -I $(input-dir)/regression/ -I $(input-dir)/test/ -I $(input-dir)/tutorial/ -I $(top-build-dir)/mf/$(outconfbase)/  -I $(top-build-dir)/mf/out/
LILYPOND_BOOK_FLAGS = --process="$(LILYPOND) --backend=eps --formats=ps,png --header=texidoc -I $(top-src-dir)/input/test -dinternal-type-checking -danti-alias-factor=2 -dgs-font-load"


#texi-html for www only:
LILYPOND_BOOK_FORMAT=$(if $(subst out-www,,$(notdir $(outdir))),texi,texi-html)
LY2DVI = $(LILYPOND)
LYS_TO_TELY = $(buildscript-dir)/lys-to-tely.py
