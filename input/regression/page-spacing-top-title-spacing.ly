\version "2.13.2"

#(set-default-paper-size "a6")

\book {

  \header {
    texidoc = "@var{top-title-spacing} controls the spacing
from the top margin to a title (or markup), provided that it is
the first system on a page."
    title = "Title" }

  \paper {
    top-title-spacing = #'((minimum-distance . 30))
    ragged-bottom = ##t
  }

  { c'1 \pageBreak c'1 }
}