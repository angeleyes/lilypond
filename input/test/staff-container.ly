\version "2.3.2"

\header {

    texidoc = "

In this preliminary test of a modern score, the staff lines are washed
out temporarily. This is done by making a tuned @code{StaffContainer},
which @code{\skip}s some notes without printing lines either and
creates a @code{\\new Staff} then in order to create the lines again.
(Be careful if you use this; it has been done by splitting the
grouping @code{Axis_group_engraver} and creating functionality into 
separate contexts, but the clefs and time signatures may not do
what you would expect.)

    "

      }



\score  {
    \notes \relative c'' <<
 	\new StaffContainer {

	    %% need << >>, otherwise we descend to the voice inside SA  
	    << \new Staff { c4 c4 } >>
	    \skip 4  % s4 would create staff.
	    
	    << \new Staff { b4 b4 } >> 
	}
 	\new StaffContainer {
	    \skip 4
	    << \context Staff { e d f \bar ":|" } >>
	    \skip 4
	}
    >>

    \paper {
	\context {
	    \Score
	    \accepts StaffContainer
	    \denies Staff
	}
	\context {
	    \type Engraver_group_engraver
	    \consists Clef_engraver
	    \consists Time_signature_engraver
	    \consists Separating_line_group_engraver
	    \consistsend "Axis_group_engraver"
	    \accepts "Staff"
	    
	    \name StaffContainer
	}
	\context {
	    \Staff
	    \remove Axis_group_engraver
	    \remove Separating_line_group_engraver
	    \remove Clef_engraver
	    \remove Time_signature_engraver
	}
	raggedright=##t
    }
}


