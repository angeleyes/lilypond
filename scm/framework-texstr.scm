;;;; framework-tex.scm --
;;;;
;;;;  source file of the GNU LilyPond music typesetter
;;;;
;;;; (c)  2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>

(define-module (scm framework-texstr)
  #:export (output-framework-tex	
	    output-classic-framework-tex))

(use-modules (ice-9 regex)
	     (ice-9 string-fun)
	     (ice-9 format)
	     (guile)
	     (srfi srfi-1)
	     (srfi srfi-13)
	     (lily))


(define (header filename)
  (format  "% header
\\input{lilypond-tex-metrics}
\\documentclass{article}
\\lilyglobalscale{1.0}
\\lilymetricsfile{~a.lpm}
\\begin{document}
" filename))

(define (footer)
  "
\\end{document}
")


(define-public (output-framework outputter book scopes fields basename )
  (let* ((paper (ly:paper-book-paper book))
	 (pages (ly:paper-book-pages book))
	 )
    (ly:outputter-dump-string outputter (header basename))
    (for-each
     (lambda (page)
       (ly:outputter-dump-stencil outputter page))
     pages)
    (ly:outputter-dump-string outputter (footer))))



(define-public (convert-to-ps . args) #t)
(define-public (convert-to-pdf . args) #t)
(define-public (convert-to-png . args) #t)
(define-public (convert-to-dvi . args) #t)
(define-public (convert-to-tex . args) #t)
