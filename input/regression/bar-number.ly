
\version "2.1.23"

\header {

texidoc="Bar number may be set and their padding adjusted individually.  
The counting of bar numbers is started after the anacrusis.

To prevent clashes at the beginning of a line, the padding may have to 
be increased.
"

}

\score {
  \notes \relative c'' {
      \partial 4 c4 
      c1 c c
      \set Score.currentBarNumber = #99999
      \override Score.BarNumber  #'padding = #3
      c1 c
  }
  \paper {
    raggedright = ##t
    \translator {
	\ScoreContext
	\override BarNumber #'break-visibility = #all-visible
    }
  }
}
