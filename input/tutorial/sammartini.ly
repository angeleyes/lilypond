\version "1.7.5"

\include "paper16.ly"
 
viola = \notes \relative c'  \context Voice = viola {
    <c4-\arpeggio g' c>
    \voiceTwo
    g'8. b,16
    s1 s2. r4
    g
}

oboes = \notes \relative c'' \context Voice = oboes {
    \voiceOne
    s4  g8. b,16 c8 r <e'8. g> <f16 a>
    \grace <e8( g> <d4 )f> <c2 e>
    \times 2/3 { <d8  f> <e g> <f a> }
    <
        { \times 2/3 { a8 g c }  c2 }
	\\
        { f,8 e e2 }  
    >

    \grace <c,8( e> <)b8. d8.-\trill> <c16 e> | 
    [<d ( f> < )f8. a>] <)b,8 d> r [<d16( f> <f8. )a>] <b,8 d> r  |
    [<c16( e>  < )e8. g>] <c8 e,>
}

hoomPah = \repeat unfold 8 \notes
    \transpose c c {
	\translator Staff = down
	\stemUp
	c8
	\translator Staff = up
	\stemDown
	c'8 }

bassvoices = \notes \relative c' {
    c4 g8. b,16
    \context Voice \hoomPah
    \translator Staff = down
    \stemBoth 
    
    [c8 c'8] r4
    <g d'> r4
    < { r2 <<e c'>> <<c g'>> } \\
      { g2-~ | g4 c8 } >
}

\score {
    \context PianoStaff \notes <
        \context Staff = up < 
             \oboes
             \viola
         >
         \context Staff = down < \time 2/2 \clef bass
             \bassvoices
         >
    >
    \midi { }
    \paper {
        indent = 0.0
        linewidth = 15.0 \cm }
}
