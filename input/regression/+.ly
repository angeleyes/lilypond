\version "2.4.0"
%% +.ly: Be the first .ly file for lys-to-tely.py.
%% Better to make lys-to-tely.py include "introduction.texi" or
%% other .texi documents too?


\header{
texidoc =

#(string-append "@unnumbered Introduction

This document presents a feature test for
LilyPond " (lilypond-version) ".  When the
text correspond with the shown notation, we consider LilyPond Officially
BugFree (tm).  This document is intended for finding bugs, and
documenting bugfixes.

TODO: order of tests (file names!), test only one feature per test.
Smaller and neater tests.
")

}

\score { \lyrics { " " }}

