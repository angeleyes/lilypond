\header {
texidoc="Automatic beamer behaves nicely."
}
\version "1.7.18"

\include "paper16.ly"
\paper  { linewidth = 390.000000 \pt } 

\score {
  \notes \relative c'' {
    a\longa a\breve  
    a1 a2 a4 a8 a16 a32 a64 a64 
  }
  \paper {
    \translator {
      \StaffContext
	\remove "Clef_engraver"
%%	\remove "Staff_symbol_engraver"
    }
  }
}
%% new-chords-done %%