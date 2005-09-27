
\version "2.7.10"

\header{
  texidoc="
As a last resort, the placement of grobs can be adjusted manually, by
setting the @code{extra-offset} of a grob.
"
}

\layout{ raggedright = ##t }


\relative c''{
  \context Bottom
  \applyOutput #(outputproperty-compatibility
		 (make-type-checker 'note-head-interface)
		 'extra-offset '(2 . 3))
  c2
  c
  \context Score {
    \applyOutput #(outputproperty-compatibility (make-type-checker 'mark-interface) 'extra-offset '(-1 . 4))
  }
  \mark A
  d1
  \mark \default
  e
}


