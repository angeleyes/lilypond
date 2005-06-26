
\version "2.6.0"
% possible rename to scheme- or something like that.  -gp
\header { texidoc = "@cindex Scheme Move Text
Objects, like text, can be moved around by using some Scheme code.
" }

#(define (make-text-checker text)
   (lambda (grob) (equal? text (ly:grob-property grob 'text))))

\score {
  \relative c''' {
    \override Stem  #'direction = #1
    \applyoutput #(outputproperty-compatibility (make-text-checker (make-simple-markup "m.d."))
      'extra-offset '(-3.5 . -4.5))
    a^2^"m.d."
  }
  \layout { raggedright = ##t}
}

