\version "1.7.6"

\header{
texidoc="
In a limited number of cases, LilyPond corrects for optical spacing
effects.  In this example, space for opposite pointed stems is adjuste
"

}

\score { 
  \context Voice \notes\relative c {
    
	\time 12/4  c''4 c c c  a f' f, a 
	
  }
  \paper {
    linewidth=-1.0
  }  
}

\score { 
  \context Voice \notes\relative c {
    
	\time 12/4  c''4 c c c  a f' f, a 
	
  }
  \paper {
    linewidth=-1.0
    \translator { \ScoreContext
    SpacingSpanner \override #'stem-spacing-correction = #0.0
  }  }
}
 %% new-chords-done %%
