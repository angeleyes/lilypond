\version "1.9.8"

\header {
texidoc = "Note head lines (e.g. glissando)
run between centers of the note heads."
}

su = \notes{ \translator Staff = up}
sd = \notes{ \translator Staff = down}

\score {
    \context PianoStaff
        \notes <<
	\context Staff = up   {
            \property PianoStaff.connectArpeggios = ##t
	    \showStaffSwitch
	    \clef F
	    c4 d \sd b a g8 f16 e32 d \su g2 \glissando a,4 \sd \break a2. \su g4 \glissando f1
	}
	\context Staff = down {
	    \clef F s1*4
	}
    >>
\paper {
    linewidth = 8.0 \cm
}
}


