%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.38"

\header {
  lsrtags = "editorial-and-educational-use, text"

  texidoc = "
PostScript code can be directly inserted inside a @code{\\markup}
block. 

"
  doctitle = "Embedding native PostScript in a \markup block"
} % begin verbatim
% PostScript is a registred trademark by Adobe Systems Inc.

\relative c'' {
  a-\markup { \postscript #"3 4 moveto 5 3 rlineto stroke" }
  -\markup { \postscript #"[ 0 1 ] 0 setdash 3 5 moveto 5 -3 rlineto stroke " }
  
  b-\markup { \postscript #"3 4 moveto 0 0 1 2 8 4 20 3.5 rcurveto stroke" }
  s2
  a'1
}