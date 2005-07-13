;;
;; define-stencil-commands.scm -- define valid stencil expression heads
;;
;; source file of the GNU LilyPond music typesetter
;;
;; (c) 2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
;;

(map ly:register-stencil-expression
     '(beam
       bezier-sandwich
       blank
       bracket
       char
       circle
       dashed-line
       dashed-slur
       dot
       draw-line
       glyph-string
       named-glyph
       polygon
       repeat-slash
       round-filled-box
       text
       url-link
       utf-8-string
       white-dot
       white-text
       embedded-ps
       zigzag-line
       setcolor
       resetcolor

       grob-cause
       no-origin
       placebox
       unknown
       ))

;; TODO:
;;  - generate this list by registering the output-backend-commands
;;    output-backend-commands should have docstrings.
;;  - remove hard copies in output-ps output-tex

(define-public (ly:all-output-backend-commands)
  "Return list of output backend commands."
  '(combine-stencil
    color
    translate-stencil))

(map ly:register-stencil-expression (ly:all-output-backend-commands))
