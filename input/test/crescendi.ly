
\version "2.1.36"
\header{
	texidoc="@cindex Crescendi
Crescendi can be printed in a number of different ways.
" }
\score{
\notes\relative c''{
a1\fff\> a\pp\!
a\< a\!
\set crescendoText = \markup { \italic \bold "cresc." }
\set crescendoSpanner = #'dashed-line
a\mf\< a a\! 
a\< a\!
}
\paper{
raggedright = ##t
}
\midi{
\tempo 1 = 60
}
}

