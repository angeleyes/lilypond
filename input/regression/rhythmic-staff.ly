\version "2.4.0"
\header
{

    texidoc = "In rhythmic staves stems should go up, and bar lines
have the size for a 5 line staff. The whole rest hangs from the
rhythmic staff."  }

\layout { raggedright = ##t }

\context RhythmicStaff
{
    r4 c4. c8 r8 c8 | c2 r2 | r1
}

