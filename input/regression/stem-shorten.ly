
\version "2.3.17"

\header {
    
    texidoc = "If note head is `over' the center line, the stem is
 shortened.  This happens with forced stem directions, and with some
 chord configurations."

}

\score {
     \relative c'' {
	\stemDown d c b a g f e
	\stemNeutral
	\stemUp a b c d e f g a
	\stemNeutral
	<g, e'>2 <b e g> <c e g> <d f g>
	
    } 
    \paper  {
	raggedright = ##t
    }
}
