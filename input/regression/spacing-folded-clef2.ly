\version "2.2.0"
\header {

texidoc = "A clef can be folded below notes in a different staff, if
there is space enough. With @code{Paper_column} stencil callbacks we can
show where columns are in the score."
}

\score { \notes \relative c'' <<
	\new Staff  { c4 c4 c4 \bar "|." }
	\new Staff { \clef bass c,2 \clef treble  c'2 }
	>>

	\paper { raggedright = ##t

	\context { \ScoreContext
	  \override NonMusicalPaperColumn #'print-function = #Paper_column::print
	  \override PaperColumn #'print-function = #Paper_column::print	  
	  \override NonMusicalPaperColumn #'font-family = #'roman
	  \override PaperColumn #'font-family = #'roman	  

	}
	}}


