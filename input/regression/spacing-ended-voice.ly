#(ly:set-option 'old-relative)
\version "2.1.26"
\header { texidoc = "
A voicelet (a very short voice to get polyphonic chords correct)
should not confuse the spacing engine."
}


	\score {
\notes {  \context Staff \relative c' {
  c4
  <<
      { r4 dis'4 } \\
      { r4 fis,4 } \\
      { r bis } \\
      { s gis }
  >>
  c4
}}
  \paper { raggedright = ##t }
}

