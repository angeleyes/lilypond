\version "2.1.22"
\header {
texidoc = "Measure repeats may be nested with beat repeats."
}
	
\score { \notes \relative c'' \context Voice { \time 4/4
   % riff
   \repeat "percent" 2 { r8. a16 g8. a16 bes8. a16 f8 d |  a c8 ~ c8 d8 ~ d8 r8 r4 }
   
   \repeat "percent" 2 { \repeat "percent" 4 { c8 es } }   
  }
}

