
(define-module (scm framework-scm)
 #:export (output-framework)
  )

(use-modules (ice-9 regex)
	     (ice-9 string-fun)
	     (ice-9 format)
	     (guile)
	     (srfi srfi-1)
	     (ice-9 pretty-print)
	     (srfi srfi-13)
	     (lily))

(define-public (output-framework basename book scopes fields )
  (let*
      ((file (open-output-file (format "~a.scm" basename))))
    
    (display ";; raw SCM output\n" file)

  (for-each
   (lambda (page)
     (display
       ";;;;;;;;;;;;;;;;;;;;;;;;;;\n;;;PAGE\n" file)
     (pretty-print (ly:stencil-expr page) file))
   (ly:paper-book-pages book))))


(define-public (convert-to-ps . args) #t)
(define-public (convert-to-pdf . args) #t)
(define-public (convert-to-png . args) #t)
(define-public (convert-to-dvi . args) #t)
(define-public (convert-to-tex . args) #t)
