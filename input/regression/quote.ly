\header
{

    texidoc = "With @code{\\quote}, fragments of previously entered
music may be quoted. @code{quotedEventTypes} will determines what
things are quoted. In this example, a 16th rests is not quoted, since
@code{rest-event} is not in @code{quotedEventTypes}."

}
\version "2.3.22"
\layout {
    raggedright = ##t
}


quoteMe = \relative c' { fis4 r16  a8.-> b4-\ff }

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
	\new Voice {
	    s4
	    \set fontSize = #-4
	    \override Stem #'lengths = #'(2.5 2.5 3.0 3.0)
	    \quote quoteMe 2.
	}
    >>
>>
