\version "1.7.6"

\score { 
  \context Voice \notes\relative c {
    \context Voice=VoiceOne
	\grace c8 d4 d d d d
	\grace { e16 f e f } d4 d d d d 
	
  }
  \paper { }  
  \midi { }
}
%% new-chords-done %%
