#(ly:set-option 'old-relative)
\version "2.1.11"
% ASCII Art output
%
% Process as:
%
%     lilypond -fas foo.ly
%     as2text foo.as

\include "paper-as9.ly"

\score {
  \notes\relative c'' {
    \time 4/4
    g1 a b c 
    g2 a b c
    g4 a b c
  }
}

