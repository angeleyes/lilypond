#(ly:set-option 'old-relative)
\version "1.9.0"
% ASCII Art output
%
% Process as:
%
%     lilypond -fas foo.ly
%     as2text foo.as

\include "paper-as5.ly"

\score {
  \notes\relative c'' {
    \time 4/4
    g1 a b c 
    g2 a b c
    g4 a b c
  }
  \paper {
    \translator {
      \StaffContext
      BarLine \override #'bar-size = #5
    }
  }
}

