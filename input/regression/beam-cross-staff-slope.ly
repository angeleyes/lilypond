\version "1.7.16"
\header {
    texidoc = "Cross staff  (kneed) beams
don't cause extreme slopes"
}

\score {
    \notes\context PianoStaff <
    \context Staff = up
    \relative c'' <
        {
	    \stemDown
            f16( \> d \! b \translator Staff = down \stemUp
            \clef treble g ~ << g e>>8-)
	    
	     e-[ \translator Staff = up
	    \stemDown
	    e e]
	    \translator Staff = down
	    \stemUp
	    [
	    e8. \translator Staff = up
	    \stemDown
	    e8.]
	    
	} \\
    >
    \context Staff = down {
        \time 3/8 \clef bass s4.*3 }
    >
    \paper { raggedright = ##t}
}

%% new-chords-done %%