
\header {
  texidoc = "Beams can be printed across line breaks, if forced.
"

}
\version "2.10.0"
\layout { ragged-right= ##t }

\relative c''  {
  \set Score.allowBeamBreak = ##t
  \time 3/16 c16-[ d e \break f-] 
}