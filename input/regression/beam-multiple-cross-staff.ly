\version "2.7.13"
\header {

  texidoc = "Kneed beams (often happens with cross-staff beams)
should look good when there are multiple beams: all the beams should
go on continuously at the staff change. Stems in both staves reach up
to the last beam.
"
}

\layout {
  raggedright= ##t
}

\context PianoStaff \relative c' <<
  \context Staff = SA {
    \stemDown
    c8[ c16 \change Staff = SB \stemUp c16 ]
    \stemNeutral
    f[ g \change Staff = SA a c] 
  }
  \context Staff = SB \relative c' {
    \clef bass
    b8[ b16 \stemUp b,,16 ]

    g'4\rest
    b,8[ b16 \stemDown b''16 b ]
  }	     
  
>>


