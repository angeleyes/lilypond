
\version "2.1.36"
\header{
texidoc = "Here @code{startGraceMusic} should set @code{no-stem-extend} to 
true; the two grace beams should be the same here.
"
}


\score { 
  \context Voice \notes\relative c {
    \grace { 
       f8[ e8] 
      \override Stem  #'no-stem-extend = ##t
       f8[ e8] 
      \revert Stem #'no-stem-extend
    }
    a4
	
  }
  \paper {
    raggedright = ##t
  }  
  \midi { }
}

