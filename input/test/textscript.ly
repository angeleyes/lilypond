
\version "2.2.0"

\header { texidoc = "@cindex Textscript
There are different fonts and glyphs to be used with @code{\markup} command. "
}

\score{
  \notes\relative c''{
    \override TextScript  #'font-shape = #'upright
    c1^\markup { \dynamic "p" "ma sosten." }  
    c^\markup \huge "ABCD" 
    \override TextScript  #'font-series = #'bold
    c^\markup { \bold "Dal" " " \raise #0.8 \musicglyph #"scripts-segno" }
    c^\markup \huge "ABCD"
  }
	\paper{ }
}

