\version "1.3.146"
\header { texidoc = "
The direction of a perfectly centred beams can be
controlled through @code{Voice.Beam}'s grob-property
@code{neutral-direction}
"}

\paper { linewidth = -1.}
\score {\notes \relative c {
  [b''8 b]
  \property Voice.Beam \set #'neutral-direction = #-1
  [b b]
}}
