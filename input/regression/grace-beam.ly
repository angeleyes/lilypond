\header
{
texidoc = "Grace beams and normal beams may occur simultaneously.
Unbeamed grace notes are not put into normal beams.
"
}

\version "1.7.18"
\score { \notes\relative c'' { c4  d8-[ \grace {  e32-[  d c d] } e8]  e-[  e \grace { f16 } e8 e]   }}

