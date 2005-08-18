
\version "2.6.0"
\header {

  texidoc = "Grace notes do tricky things with timing. If a measure
starts with a grace note, the measure does not start at 0, but
earlier. Nevertheless, lily should not get confused. For example, line
breaks should be possible at grace notes, and the bar number should be
printed correctly.
"
}

\relative c'' { c1 \break
		\grace c8
		c1  }

\layout { raggedright = ##t }


