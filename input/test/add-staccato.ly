
\version "2.1.28"

\header {

texidoc= "@cindex Add Stacato
Using @code{make-music}, you can add various stuff to notes. In this
example staccato dots are added to the notes.  For this simple case,
it is not necessary to use scm constructs (see @code{separate-staccato.ly}).
"
} 

#(define (make-script x)
   (make-music 'ArticulationEvent
               'articulation-type x))
    
#(define (add-script m x)
   (if
     (equal? (ly:music-property m 'name) 'EventChord)
     (set! (ly:music-property m 'elements)
           (cons (make-script x)
                 (ly:music-property m 'elements))))
   m)

#(define (add-staccato m)
   (add-script m "staccato"))

\score {
  \notes\relative c'' {
    a b \apply #(lambda (x) (music-map add-staccato x)) { c c } 
  }
  \paper{ raggedright = ##t }
}


