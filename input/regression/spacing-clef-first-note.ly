\version "2.1.23"
\header {

    texidoc = "Clef changes at the start of a line get much more space
than clef changes halfway the line."

}

\score  {\notes  {
    << \new Staff {
	c'2
	\clef bass e16 f a
	\clef treble b
	}
      \new Staff  {
	  c'4 c'4 c'4 
      }>>
    }
    \paper { raggedright = ##t
\translator { \StaffContext
	      TimeSignature = \turnOff
	 }

     }}

