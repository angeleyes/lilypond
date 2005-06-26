\header
{

    texidoc = "With @code{\\cueDuring} and @code{\\quoteDuring},
fragments of previously entered music may be
quoted. @code{quotedEventTypes} will determines what things are
quoted. In this example, a 16th rests is not quoted, since
@code{rest-event} is not in @code{quotedEventTypes}."

}
\version "2.6.0"
\layout {
    raggedright = ##t
}


quoteMe = \relative c' { fis4 r16  a8.-> b4-\ff c }

\addquote quoteMe \quoteMe 
original = \relative c'' { c8 d s2 es8 gis8 }

<<
    \new Staff {
	\set Staff.instrument = "quoteMe"
	\quoteMe
    }
    \new Staff {
	\set Staff.instrument = "orig"
	\original
    }
    \new Staff \relative c'' <<
	
	\set Staff.instrument = "orig+quote"	
	\set Staff.quotedEventTypes = #'(note-event articulation-event)
	\original
	{ s4 \quoteDuring #"quoteMe" { s2. } }
    >>
>>
