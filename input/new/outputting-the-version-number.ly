\version "2.11.23"

\header {
  doctitle = "Outputting the version number"
  lsrtags = "text"
  texidoc = "
By putting the output of @code{lilypond-version} into a lyric or a
text markup, it is possible to print the version number of LilyPond in
a score, or in a document generated with @code{lilypond-book}.
"
}

\score { \context Lyrics  {
    \override Score.RehearsalMark  #'self-alignment-X = #LEFT
    \mark #(ly:export (string-append "Processed with LilyPond version " (lilypond-version)))
    s2
  }
}