\version "1.7.18"  %% or actually: 1.7.1 ...
% FIXME: doesn't work with 1.7.19.  Check with more recent version.
\header {

texidoc= "@cindex Add Stacato
Using make-music, you can add various stuff to notes. Here
is an example how to add staccato dots.  Note: for this simple case
one would not use scm constructs.  See separate-staccato.ly first.
"
} 

#(define (make-script x)
   (let ((m (make-music-by-name 'ArticulationEvent)))
     (ly:set-mus-property! m 'articulation-type x)
     m))
    
#(define (add-script m x)
   (if (equal? (ly:get-mus-property m 'name) 'RequestChord)
       (ly:set-mus-property! m 'elements
			    (cons (make-script x)
				  (ly:get-mus-property m 'elements)))

       (let ((es (ly:get-mus-property m 'elements))
	     (e (ly:get-mus-property m 'element)) )
	 (map (lambda (y) (add-script y x)) es)
	 (if (ly:music? e)
	     (add-script e x))))
   m)

#(define (add-staccato m)
   (add-script m "staccato"))

\score {
  \notes\relative c'' {
    a b \apply #add-staccato { c c } 
    a b \apply #add-staccato { c c } 
  }
}

%% new-chords-done %%
