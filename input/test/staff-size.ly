\version "2.7.13"

\header { texidoc = "@cindex Staff Size

In order to change staff sizes, both @code{staff-space} and @code{fontSize}
must be scaled."

}

\layout { raggedright = ##t}

\relative c' <<
%    \new Staff \relative c'' { \dynamicDown c4 \ff c c c }
    \new Staff \with {
	fontSize = #-3
	\override StaffSymbol #'staff-space = #(magstep -3)
    } {
	\clef bass
	c8 c c c  c c c c
    }
>>


