#(ly:set-option 'old-relative)
\version "2.1.11"
% ASCII Art output
%
% Process as:
%
%     lilypond -fas foo.ly
%     as2text foo.as


\include "paper-as5.ly"

\score {
  \context StaffGroup <<
    \context Staff=upper \notes\relative c{
      \key f \major
      \time 3/4
      r8^"Moderato" %\pp 
      <<g'-. c-.>> <<c-. es-.>> <<g-. c-.>> <<c-. es-.>> <<g-. c-.>> |
      r8 <<as-. c-.>> <<c-. es-.>>
    }
    \context Staff=lower \notes\relative c{
      \key f \major
      \time 3/4
      \clef "bass"
      <<c,2 c'>> r4 
      <<as2 as'>> r4
    }
  >>
  \paper {
    indent=4.0\char
    linewidth=78.0\char
    \translator { \StaffContext Bar \override #'bar-size = #5 }
    \translator { 
      \VoiceContext 
      % horizontal beams
      Beam \override #'height = #0
      autoBeamSettings \override #'(begin * * * *) = #(ly:make-moment 0 1)
      TextScript \set #'no-spacing-rods = ##t
    }
  }
}

