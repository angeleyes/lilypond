
\version "2.7.39"
\header {
  texidoc = "@cindex Boxed Stencil

The @code{print-function} can be overridden to draw a box around an arbitrary 
grob. " }


\layout{ragged-right = ##t}
\relative c''  {

  \override TextScript  #'stencil =
  #(make-stencil-boxer 0.1 0.3 ly:text-interface::print)

  c'4^"foo"

  \override Stem  #'stencil =
  #(make-stencil-boxer 0.05 0.25 ly:stem::print)

  \override Score.RehearsalMark  #'stencil =
  #(make-stencil-boxer 0.15 0.3 ly:text-interface::print)
  b8
  \revert Stem #'stencil


  c4. c4 \mark "F" c1 
}

