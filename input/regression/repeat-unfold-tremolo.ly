\version "2.2.0"
\header {

    texidoc = "Unfolding tremolo repeats. All fragments fill one
 measure with 16th notes exactly."

}


\score{
    \notes{
	\time 2/4 
	\apply #unfold-repeats 	
	{ \repeat tremolo 4 { c'16 e' } |
	  \repeat tremolo 8 c'16 } |

	\time 3/4 
	\apply #unfold-repeats 	
	{ \repeat tremolo 6 { c'16 e' } |
	  \repeat tremolo 12 c'16 } | \bar "|."
	
	\bar "|."

	
    }
    \paper { raggedright = ##t }  
}
