\version "2.1.26"

\header {


    texidoc = #(string-append "By putting the output of
    @code{lilypond-version} into a lyric, it is possible to print the 
    version number of LilyPond in a score, or in a document generated
    with @code{lilypond-book}.  Another possibility is to append the 
    version number to the doc-string, in this manner: "

  (lilypond-version)
  )
    
}

\score { \context Lyrics \notes {
    \override Score.RehearsalMark  #'self-alignment-X = #LEFT
    \mark #(ly:export (string-append "Processed with LilyPond version " (lilypond-version)))
    s2
  }
}


