#(ly:set-option 'old-relative)
\version "1.9.1"
\header {
  texidoc="Manual hack for nested tuplets, move outer tuplet up."
}

#(define (make-text-checker-once text)
  (lambda (grob) (and text-checker-once
		      (if (equal? text (ly:get-grob-property grob 'text))
			  (begin
			    (set! text-checker-once #f) #t)
			  #f))))

#(define text-checker-once #t)

\score {
  \notes\relative c'' {

    \property Voice.tupletNumberFormatFunction = #fraction-tuplet-formatter

    \outputproperty #(make-text-checker-once "2:3")
      #'extra-offset = #'(0 . 1.5)
    \times 2/3 {
      \times 2/3 {
        a a a
      }
      \times 3/5 {
        a a a a a
      }
    }
  }
  \paper { raggedright = ##t}
}

