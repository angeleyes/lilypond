
\version "2.1.22"
\header {

texidoc = "The @code{\\tag} command marks music expressions with a
name. These tagged expressions can be filtered out later. This
mechanism can be used to make different versions of the same music. In
this example, the top stave displays the music expression with all
tags included. The bottom two staves are filtered: the part has cue
notes and fingerings, but the score has not."

}

\paper { raggedright= ##t }

common =
\notes \relative c''  {

    c1
    \relative c' <<
	\tag #'part <<
	  R1 \\
	  {
	      \set fontSize =  #-1
	      c4_"cue" f2 g4 } 
        >>
	\tag #'score R1
     >>
    c1-\tag #'part ^4
}


\score {
    \notes \simultaneous { 
    \new Staff {
	\set Staff.instrument =  #"both"
	\common
	}
    \new Staff {
	\set Staff.instrument =  #"part"
	\apply #(remove-tag 'score) \common
	}
    \new Staff {
	\set Staff.instrument =  #"score"
	\apply #(remove-tag 'part) \common
	}
    }
}





