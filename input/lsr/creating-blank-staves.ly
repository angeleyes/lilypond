%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.38"

\header {
  lsrtags = "staff-notation, editorial-and-educational-use, contexts-and-engravers, paper-and-layout"
 texidoc = "
To create blank staves, you must generate empty measures, removing also
from the @code{Score} context the @code{Bar_number_engraver}, and from
the @code{Staff} context the @code{Time_signature_engraver}, the
@code{Clef_engraver} and the @code{Bar_engraver}. 
" }
% begin verbatim
\header {
  tagline = ""
}

#(set-global-staff-size 20)

\score {
  { 
    \repeat unfold 12 { s1 \break } 
  }
  \layout {
    indent = 0\in
    \context {
      \Staff
      \remove Time_signature_engraver
      \remove Clef_engraver
      \remove Bar_engraver
    }
    \context {
      \Score
      \remove Bar_number_engraver
    }
  }
}

\paper {
  #(set-paper-size "letter")
  raggedlastbottom = ##f
  linewidth = 7.5\in
  leftmargin = 0.5\in
  bottommargin = 0.25\in
  topmargin = 0.25\in
}