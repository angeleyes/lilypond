%%  Do not edit this file; it is auto-generated from LSR!
\version "2.11.23"

\header { texidoc = "
By setting @code{voltaOnThisStaff}, repeat brackets can be put over
staves other than the topmost one in a score. 
" }

\layout {
  ragged-right = ##t 
}

vmus =  {
  \repeat volta 2 c1 \alternative { d e } 
} 

\relative c'' <<
  \new StaffGroup <<
    \context Staff \vmus
    \new Staff \vmus
  >>
  \new StaffGroup <<
    \new Staff <<
      \set Staff.voltaOnThisStaff = ##t
      \vmus >>
    \new Staff \vmus
  >>
>>