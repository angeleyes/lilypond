frag= \notes {
    \clef bass f8 as8
    c'4-~ c'16 as g f e16 g bes c' des4
    }

\score {\frag

    \paper { raggedright = ##t

	 }
}

\score { \frag
        \paper { raggedright = ##t
		 linethickness = 1.5 \pt
	     \translator {
		 \ScoreContext
		 Beam \set #'thickness = #0.3
		 Stem \set #'thickness = #0.5
		 Bar \set #'thickness = #3.6
		 Tie \set  #'thickness = #2.2

		 %% yes, this dirty.
		 Tie \set  #'extra-offset = #'(0 . 0.3)

	 }
}}
    
    
