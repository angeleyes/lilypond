
\version "2.1.30"
\header{

texidoc=" Beams on ledgered notes should always reach the middle staff
line.  The second beam counting from the note head side, should never
be lower than the second staff line.  This does not hold for grace
note beams.  Override with @code{no-stem-extend}.  "

}


\score { 
  \context Voice \notes\relative c {
     f8[ f]   f64[ f] 
     \grace { 
       f8[ e8] 
      \override Stem  #'no-stem-extend = ##f
       f8[ e8] 
      \revert Stem #'no-stem-extend
    }
	 f8[ f]
	
  }
  \paper {
    raggedright = ##t
  }  
  \midi { }
}
