\version "2.3.16"
\header{
texidoc="

Markings can be attached to (invisible) barlines.
"
}

\paper { raggedright = ##t }

onestaff = \new Staff\relative c''  {
	\set Staff.instr = instr
	\set Staff.instrument = instrument \mark "B"
	 c1 \mark "A" \break c2  c2 \break
}

grstaff =  \relative c''
\context GrandStaff <<
	\new Staff {

	\set Staff.instr = instr
	
	 \mark "B" \break c1 \mark "A" c2  }
	\new Staff { c1 c2  }
>>


\score {\grstaff}

