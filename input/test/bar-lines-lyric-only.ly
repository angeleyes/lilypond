\version "2.3.22"
\header { texidoc = "@cindex Bar line lyric only
You can move @code{Bar_engraver} and @code{Span_bar_engraver} to 
a different engraving context, if you want, for example, bar lines 
on lyrics. "
}

\score {
    \relative c' \context ChoirStaff <<
	\new Staff { c1 c1 c1}
	\lyricmode <<
	    \new Lyrics  { bla1 die bla }
	>>
	\new Staff { c1 c1 c1} 
    >>


    \layout  {
	raggedright = ##t
	\context {
	    \Lyrics
	    \consists Bar_engraver


	    %% need procedure, since lyrics doesn't have a staff_sym engraver.
	    \override BarLine #'bar-size-procedure = #(lambda (x) 3.0)
	}
	\context{
	    \ChoirStaff
	    \remove "Span_bar_engraver"
	}
	\context {
	    \Staff
	    \remove "Bar_engraver"
	}
    }
}

