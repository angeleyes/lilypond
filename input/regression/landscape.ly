
#(set-default-paper-size "a6" 'landscape)

pattern = \relative { a b c d \break }

\book {    
    \score {
	\new Staff  {
	    \repeat unfold 15 \pattern
	}
    }
}


