\version "2.1.7"
% A simple scale in LilyPond
%
% Type:
%
%     lilypond example-1
%     xdvi example-1     # or your dvi viewer here
%

%%
%% For learning LilyPond, please read the tutorial, included in the
%% user-manual.
%% 

\score { 
  \context Voice \notes\relative c {
    c' d e f g a b c
  }
  \paper {  }  
  \midi { }
}
