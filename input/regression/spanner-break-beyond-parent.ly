\header { texidoc = "Spanners parts that extend beyond their parents
  are killed in case of line breaks."

  }


\version "2.10.19"

\paper { ragged-bottom = ##t }

\new Staff {
  c1 \break c1
} 
\addlyrics {  welt __ }

