\version "2.2.0"


\header {
texidoc = "Modern  transcriptions  of gregrorian music.
Gregorian music has no measure, no stems; it uses only half and quarter notes,
and two types of barlines, a short one indicating a rest, and a second one
indicating a breath mark."
}


barOne = \notes { \once \override Staff.BarLine  #'bar-size = #2
	\bar "|" }
barTwo = \notes { \once \override Staff.BarLine  #'extra-offset = #'(0 . 2)

		\once \override Staff.BarLine  #'bar-size = #2
	\bar "|" }
\score {

\notes \relative c' {
	\set Score.timing = ##f
	\override Staff.Stem  #'transparent = ##t
	f4 a2 \barTwo g4 a2  f2 \barOne g4( f) f(
	\bar "empty" 
	\break
	g) a2
}

}
