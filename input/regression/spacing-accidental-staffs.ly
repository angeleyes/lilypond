\version "2.1.30"

\header { 
texidoc = "Accidentals in different staves do not affect the
spacing of the quarter notes here."
}

\score { \notes \relative c'' << \new Staff { \time 4/4

 c8[ c8 cis8 cis8]
 cis8[ cis8 cis8 cis]


 }
   { \key d \major cis4 cis4 cis4 cis!4  } >>

   \paper { raggedright = ##t} 
 }



