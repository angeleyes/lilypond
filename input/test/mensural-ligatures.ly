\version "2.3.22"
% TODO:
% check with ancient- stuff.  rename, merge, something.  -gp

\header { texidoc = "@cindex Ancient Mensural Ligatures
In mensural ligatures, notes with ancient durations are printed
in a tight manner."
}



% Note that the horizontal alignment of the fermatas obeys to the
% graphical width of the ligatures rather to the musical moment in time.
% This is intended behaviour.

voice =  \transpose c c' {
  \set Score.timing = ##f
  \set Score.defaultBarType = "empty"
  g\longa c\breve a\breve f\breve d'\longa^\fermata
  \bar "|"
  \[
    g\longa c\breve a\breve f\breve d'\longa^\fermata
  \]
  \bar "|"
  e1 f1 a\breve g\longa^\fermata
  \bar "|"
  \[
    e1 f1 a\breve g\longa^\fermata
  \]
  \bar "|"
  e1 f1 a\breve g\longa^\fermata
  \bar "||"
}

\paper {
    linethickness = \staffspace / 5.0
}
\score {
    \context ChoirStaff <<
	\new MensuralStaff <<
	    \context MensuralVoice <<
		\voice
	    >>
	>>
	\new Staff <<
	    \context Voice <<
		\voice
	    >>
	>>
    >>
    \layout {
	\context {
	    \Voice
	    \name MensuralVoice
	    \alias Voice
	    \remove Ligature_bracket_engraver
	    \consists Mensural_ligature_engraver
	    \override NoteHead #'style = #'mensural
	}
	\context {
	    \Staff
	    \name MensuralStaff
	    \alias Staff
	    \accepts MensuralVoice
	    \consists Custos_engraver
	    \override TimeSignature #'style = #'mensural
	    \override KeySignature #'style = #'mensural
	    \override Accidental #'style = #'mensural
	    \override Custos #'style = #'mensural
	    \override Custos #'neutral-position = #3
	    \override Custos #'neutral-direction = #-1
	    clefGlyph = #"clefs-petrucci-g"
	    clefPosition = #-2
	    clefOctavation = #-0
	}
	\context {
	    \RemoveEmptyStaffContext
	    \accepts MensuralVoice
        }
	\context {
	    \Score
	    \accepts MensuralStaff
	}
    }
}

