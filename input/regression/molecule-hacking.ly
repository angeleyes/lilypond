
\version "2.1.26"

\header { texidoc=" You can write stencil callbacks in Scheme, thus
providing custom glyphs for notation elements.  A simple example is
adding parentheses to existing stencil callbacks.

The parenthesized beam is less successful due to implementation of the
Beam. The note head is also rather naive, since the extent of the
parens are also not seen by accidentals.
"
	
}

#(define (parenthesize-callback callback)
   "Construct a function that will do CALLBACK and add parentheses.
Example usage:

  \\property NoteHead \\override #'print-function
		   =
		      #(parenthesize-callback Note_head::print)
		    
"

   
   (define (parenthesize-stencil grob)
     "This function adds parentheses to the original callback for
GROB.  The dimensions of the stencil is not affected.
"
     
     (let* (
	    (fn (ly:get-default-font grob))
	    (pclose (ly:find-glyph-by-name fn "accidentals-rightparen"))
	    (popen (ly:find-glyph-by-name fn "accidentals-leftparen"))
	    (subject (callback grob))

	    ; remember old size
	    (subject-dim-x (ly:stencil-get-extent subject 0))
	    (subject-dim-y (ly:stencil-get-extent subject 1))
	)

        ; add parens
        (set! subject
	     (ly:stencil-combine-at-edge 
	      (ly:stencil-combine-at-edge subject 0 1 pclose 0.2)
	      0 -1 popen  0.2))

	; revert old size.
       (ly:stencil-set-extent! subject 0 subject-dim-x)
       (ly:stencil-set-extent! subject 1 subject-dim-y)
       subject
    )
     )
   parenthesize-stencil
   )
    


\score {
	\notes \relative c' { c4 e

		    \override NoteHead  #'print-function
		   =
		      #(parenthesize-callback Note_head::print)
		    g bes
		    \revert NoteHead #'print-function
		    \override Beam  #'print-function
		   =
		      #(parenthesize-callback Beam::print)

		    a8 gis8 a2.
		    
		    }

	\paper { raggedright = ##t}
	}


