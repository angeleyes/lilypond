\version "2.2.0"
\header {
    title = "Screech and boink"
    subtitle = "Random complex notation"
    composer = "Han-Wen Nienhuys"
}

\score {
    \notes \context PianoStaff <<
	\context Staff = up {
	    \time 4/8
	    \key c \minor


	    << {
		\revert Stem #'direction
		\change Staff = down
		\set subdivideBeams = ##t	     
		g16.[
		    \change Staff = up
		    c'''32 	\change Staff = down
		    g32 \change Staff = up
		    c'''32	\change Staff = down
		    g16]
		\change Staff = up
		\override Stem  #'direction = #1
		\set followVoice = ##t
		c'''32([ b''16 a''16 gis''16 g''32)]  } \\
	       { s4 \times 2/3 { d'16[ f' g'] } as'32[ b''32 e'' d''] } \\
	       { s4 \autoBeamOff d''8.. f''32  } \\
	       { s4 es''4 }
	   >>
	}

	\context Staff = down {
	    \clef bass
	    \key c \minor
	    \set subdivideBeams = ##f
	    \override Stem  #'french-beaming = ##t
	    \override Beam  #'thickness = #0.3
	    \override Stem  #'thickness = #4.0
	    g'16[ b16 fis16 g16]
	    << \apply #notes-to-clusters { 
		as16 <as b>
		<g b>
		<g cis>
	    } \\
	       {
		   \override Staff.Arpeggio  #'arpeggio-direction =#-1
		   <cis, e, gis, b, cis>4\arpeggio  }
	   >>
	}
    >>
    \midi { \tempo 8 = 60 }

    \paper {
	raggedright = ##t 

	\context {
	    \StaffContext
	    \consists Horizontal_bracket_engraver
	}
	
    }
}
