\version "2.3.22"

\header {
    texidoc = "@cindex Bar Number Every Fifth Reset
If you would like the bar numbers to appear at regular intervals, but
not starting from measure zero, you can use a context function,
@code{set-bar-number-visibility}, to set automatically
@code{barNumberVisibility}, so that the bar numbers appear at regular
intervals, starting from the measure in which 
@code{set-bar-number-visibility} is set using @code{\applycontext}.
"

}

resetBarnum = \context Score \applycontext
  #(set-bar-number-visibility 4)
\score {
    <<
         \transpose c c'' {
	    \override Score.BarNumber  #'break-visibility =#end-of-line-invisible
	    \override Score.RehearsalMark  #'padding = #2.5
	    \mark "A" \resetBarnum
	    \repeat unfold 10 c1
	    \mark \default \resetBarnum
	    \repeat unfold 8 c
            \bar "|."
        }
    >>
\layout{raggedright = ##t}
}
