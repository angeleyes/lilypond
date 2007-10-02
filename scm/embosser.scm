;;;; embosser.scm -- scheme embosser functions
;;;;
;;;;  source file of the GNU LilyPond music typesetter
;;;; 
;;;; (c) 2007 Ralph Little
;;;;

(define-public (write-embossings embossings basename)
  (let
      loop
    ((embs embossings)
     (count 0))


    (if (pair?  embs)
	(begin
	  (ly:embossing-write
	   (car embs)
	   (if (> count 0)
	       (format #f "~a-~a.brl" basename count)
	       (format #f "~a.brl" basename)))
	  (loop (cdr embs) (1+ count))))))
