\version "2.1.30"

chordmodifiers = #default-chord-modifier-list

whiteTriangleMarkup = #(make-override-markup
			'(font-family . math) (make-simple-markup "M"))

blackTriangleMarkup = #(make-override-markup
			'(font-family . math) (make-simple-markup "N"))

ignatzekExceptionMusic = \notes{
	<c e gis>1-\markup { "+" }
	<c es ges>-\markup { \super "o" } % should be $\circ$ ?
	<c es ges bes>-\markup { \super \combine "o" "/" }
	<c es ges beses>-\markup { \super  "o7" }
}

ignatzekExceptions = #(sequential-music-to-chord-exceptions
		       ignatzekExceptionMusic #t)

partialJazzMusic = \notes{
    <c d>1-\markup { \normal-size-super "2" }
    <c es>-\markup { "m" }
    <c f>-\markup { \normal-size-super "sus4" }
    <c g>-\markup { \normal-size-super "5" }
    
    %% TODO, partial exceptions
    <c es f>-\markup { "m" }-\markup { \normal-size-super "sus4" }
    <c d es>-\markup { "m" }-\markup { \normal-size-super "sus2" }
}

%% TODO: compatibility ignatzek code
fullJazzExceptions = #(sequential-music-to-chord-exceptions
                       ignatzekExceptionMusic #f)

partialJazzExceptions = #(sequential-music-to-chord-exceptions
                          partialJazzMusic #f)

		       
