%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.46"

\header {
  lsrtags = "expressive-marks"

  texidoc = "
The text used for crescendos and decrescendos can be changed by
modifying the context properties @code{crescendoText} and
@code{decrescendoText}.  The style of the spanner line can be changed
by modifying the context properties @code{crescendoSpanner} and
@code{decrescendoSpanner}.  Available values for the spanner properties
are @code{'hairpin}, @code{'line}, @code{'dashed-line}, and
@code{'dotted-line}. If unset, a hairpin crescendo is used: 

"
  doctitle = "Changing text and spanner styles for text dynamics"
} % begin verbatim
\relative c'' {
  \set crescendoText = \markup { \italic { cresc. poco } }
  \set crescendoSpanner = #'dotted-line
  a2\< a
  a2 a
  a2 a
  a2 a\mf
}