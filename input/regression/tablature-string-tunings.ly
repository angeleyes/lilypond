\header {

    texidoc = "For other tunings, it is sufficient to set
    @code{stringTunings}. The number of staff lines is adjusted
    accordingly."

}

\version "2.1.36"

\score  {
    \notes \new TabStaff {
	\set TabStaff.stringTunings = #'(5  10 15 20)
	\relative c''  { c4 d e f }
       }
}
 
