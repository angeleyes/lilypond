\version "1.7.18"
\header { texidoc = "@cindex Preset Extent
Grob extents may be hard coded using grob properties.  This
requires Grob::preset_extent () function. " 
}

\score {
    \context Lyrics \lyrics {
	\property Lyrics . LyricHyphen \set #'extra-offset = #'(0.0 . -5.0) 
	foo --
	\property Lyrics . LyricText \set #'X-extent-callback = #Grob::preset_extent
	\property Lyrics . LyricText \set #'X-extent = #'(-10.0 . 10.0)
 bar baz
	}
    \paper { raggedright = ##t}
}
    
%% new-chords-done %%
