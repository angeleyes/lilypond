%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.46"

\header {
  lsrtags = "expressive-marks, tweaks-and-overrides"

  texidoc = "
The vertical ordering of scripts is controlled with the
@code{script-priority} property. The lower this number, the closer it
will be put to the note. In this example, the @code{TextScript} (the
sharp symbol) first has the lowest priority, so it is put lowest in the
first example. In the second, the prall trill (the @code{Script}) has
the lowest, so it is on the inside. When two objects have the same
priority, the order in which they are entered determines which one
comes first. 

"
  doctitle = "Controlling the vertical ordering of scripts"
} % begin verbatim
\relative c''' {
  \once \override TextScript #'script-priority = #-100
  a2^\prall^\markup { \sharp }
  
  \once \override Script #'script-priority = #-100
  a2^\prall^\markup { \sharp }
}