\version "1.7.6"
\header {

texidoc = "setting sparseTies causes only one tie to be
generated per chord pair."

}

	
\score { 
  \context Voice \notes\relative c {
	\property Voice.sparseTies = ##t
	c''  <<c e g>> ~ <<c e g>> 
  }
  \paper {
    linewidth=-1.0
  }  
  \midi { }
}
%% new-chords-done %%
