% possible rename to ancient- or gregorian- ?
\header {
    texidoc = "@cindex Divisiones
Divisiones are gregorian variants of breathing signs.
Choices are @code{divisioMinima}, @code{divisioMaior},
@code{divisioMaxima} and @code{finalis}, @code{virgula} and
@code{caesura}.
" }

\version "2.1.28"

\include "gregorian-init.ly"

\score {
 <<
	\context Voice \notes \transpose c c' {
	    \set Score.timing = ##f
	    \set Score.barAlways = ##t
	    \override TextScript  #'padding = #3
	    \override Staff.BarLine  #'transparent = ##t
	    \override Stem  #'transparent = ##t
            g4( a) g e( f) e
            ^\markup { "divisio minima" }
	    \divisioMinima
            g4( a) g e( f) e
            ^\markup { "divisio maior" }
	    \divisioMaior
            g4( a) g e( f) e
            ^\markup { "divisio maxima" }
	    \divisioMaxima
            g4( a) g e( f) e
            ^\markup { "finalis" }
	    \finalis
            g4( a) g e( f) e

            ^\markup { "virgula" }
	    \virgula
            g4( a) g e( f) e
            ^\markup { "caesura" }
	    \caesura
            g4( a) g e( f) e
	}
	\lyricsto "" \new  Lyrics \lyrics {
	    Blah blub, blah blam.
	    Blah blub, blah blam.
	    Blah blub, blah blam.
	    Blah blub, blah blam.
	    Blah blub, blah blam.
	    Blah blub, blah blam.
	    Blah blub, blah blam.
	}
    >>
}
