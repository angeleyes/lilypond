
\version "2.1.28"
\header {

    texidoc = "@cindex Bars Always

By setting @code{barAlways} and @code{defaultBarType}, barlines may be inserted automatically everywhere."

}


\score {
	\notes \relative c''{
		\set Score.barAlways = ##t
		\set Score.defaultBarType = ":|:"
		c4 c4 c4 c4 }
	\paper{raggedright = ##t}
}

