\version "1.7.6"


fragment = \notes {
  \property Voice.Beam \set #'positions = #'(4 . 0)
  [c'8 c]
}

\paper { linewidth = -1. } 

\score {
  \notes\relative c \fragment
  \paper { }  
}
%% new-chords-done %%
