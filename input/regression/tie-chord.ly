\version "1.7.18"
\header{
texidoc="
When tieing chords, the outer slurs point outwards, the inner slurs
point away from the center of the staff.  Override with
@code{tieVerticalDirection}.
"
}


t =  \notes \relative c' {   <<c e g>> ~ <<c e g>> }

	\score { 
\notes \context Voice {
   \t
   \transpose c g \t

   \property Voice.TieColumn \override #'direction = #-1
   \t

  }
}

