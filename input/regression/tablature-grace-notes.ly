\version "2.13.4"

\header{ texidoc = "Fret numbers belonging to grace notes are smaller."
       }

gracenotes = \relative c {
   c4 d e f
   \grace e8 c4 d e f
   \grace \parenthesize e8 c4 d e f
   \appoggiatura e8 c4 d e f
   \acciaccatura e8 c4 d e f
   \bar "|."
}

\context StaffGroup <<
  \context Staff <<
    \clef "G_8"
    \gracenotes
  >>
  \context TabStaff <<
    \gracenotes
  >>
>>

