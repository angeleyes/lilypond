%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.38"

\header {
  lsrtags = "repeats"
 texidoc = "
The @code{Volta_engraver} by default resides in the Score context, and
brackets for the repeat are thus normally only printed over the topmost
staff.  This can be adjusted by adding @code{Volta_engraver} to the
Staff context where you want the brackets to appear; see also the
\"Volta multi staff\" snippet.
" }
% begin verbatim
<<
  \new Staff { \repeat volta 2 { c'1 } \alternative { c' } }
  \new Staff { \repeat volta 2 { c'1 } \alternative { c' } }
  \new Staff \with { \consists Volta_engraver } { c'2 g' e' a' }
  \new Staff { \repeat volta 2 { c'1 } \alternative { c' } }
 >>