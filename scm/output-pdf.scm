;;;; pdf.scm -- implement Scheme output routines for PDF.
;;;;
;;;;  source file of the GNU LilyPond music typesetter
;;;; 
;;;; (c)  2001--2004 Stephen Peters <portnoy@portnoy.org>


;currently no font commands; this is a helper for pdftex.scm.

(define-module (scm output-pdf))



(define this-module (current-module))

(use-modules
 (guile)
 (lily))



; simple commands to store and update currentpoint.  This makes the
; other procedures simple rewrites of the PostScript code.

(define currentpoint (cons 0 0))
(define (showcp) 
  (string-append (ly:number-pair->string currentpoint) " "))
(define (moveto x y)
  (set! currentpoint (cons x y))
  (string-append (showcp) "m "))
(define (moveto-pair pair)
  (moveto (car pair) (cdr pair)))
(define (rmoveto x y)
  (moveto (+ x (car currentpoint)) (+ y (cdr currentpoint))))
(define (lineto x y)
  (set! currentpoint (cons x y))
  (string-append (showcp) "l "))
(define (lineto-pair pair)
  (lineto (car pair) (cdr pair)))
(define (rlineto x y)
  (lineto (+ x (car currentpoint)) (+ y (cdr currentpoint))))
(define (curveto x1 y1 x2 y2 x y)
  (set! currentpoint (cons x y))
  (string-append (ly:number->string x1) " " (ly:number->string y1) " "
		 (ly:number->string x2) " " (ly:number->string y2) " "
		 (ly:number->string x) " " (ly:number->string y) " c "))
(define (curveto-pairs pt1 pt2 pt)
  (curveto (car pt1) (cdr pt1) (car pt2) (cdr pt2) (car pt) (cdr pt)))
(define (closefill) "h f ")
(define (closestroke) "S ")
(define (setlinewidth w) (string-append (ly:number->string w) " w "))
(define (setgray g) (string-append (ly:number->string g) " g "))
(define (setlineparams) "1 j 1 J ")

(define (beam width slope thick blot)
  (let ((ht (* slope width)))
    (string-append (moveto 0 (- (/ thick 2)))
		   (rlineto width ht)
		   (rlineto 0 thick)
		   (lineto 0 (/ thick 2))
		   (closefill))))

(define (comment s) 
  (string-append "% " s "\n"))

(define (brack-traject pair ds alpha)
  (let ((alpha-rad (* alpha (/ 3.141592654 180))))
    (cons (+ (car pair) (* (cos alpha-rad) ds))
	  (+ (cdr pair) (* (sin alpha-rad) ds)))))

(define (bracket arch_angle arch_width arch_height height arch_thick thick)
  (let* ((halfht (+ (/ height 2) thick))
	 (farpt (cons (+ thick arch_height) 
		      (+ (- halfht arch_thick) arch_width)))
	 (halfbrack 
	  (string-append (moveto 0 0)
			 (lineto thick 0)
			 (lineto thick (- halfht arch_thick))
			 (curveto-pairs
			  (brack-traject (cons thick 
					       (- halfht arch_thick))
					 (* 0.4 arch_height) 0)
			  (brack-traject farpt 
					 (* -0.25 arch_height) 
					 arch_angle)
			  farpt)
			 (curveto-pairs 
			  (brack-traject farpt
					 (* -0.15 arch_height)
					 arch_angle)
			  (brack-traject (cons (/ thick 2) halfht)
					 (/ arch_height 2) 0)
			  (cons 0 halfht))
			 (lineto 0 0)
			 (closefill))))
    (string-append (setlinewidth (/ thick 2))
		   (setlineparams)
		   "q 1 0 0 -1 0 0 cm " ; flip coords
		   halfbrack
		   "Q " ; grestore
		   halfbrack)))

(define (char i)
  (invoke-char " show" i))


(define (dashed-slur thick dash l)
  (string-append (setlineparams)
		 "[ " (ly:number->string dash) " "
		 (ly:number->string (* 10 thick)) " ] 0 d "
		 (setlinewidth thick)
		 (moveto-pair (car l))
		 (apply curveto (cdr l))
		 (closestroke)))

(define (dashed-line thick on off dx dy)
  (string-append (setlineparams)
		 "[ " (ly:number->string on) " "
		 (ly:number->string off) " ] 0 d "
		 (setlinewidth thick)
		 (moveto 0 0)
		 (lineto dx dy)
		 (closestroke)))

(define (repeat-slash width slope beamthick)
  (let* ((height (/ beamthick slope))
	 (xwid (sqrt (+ (* beamthick beamthick) (* height height)))))
    (string-append (moveto 0 0)
		   (rlineto xwid 0)
		   (rlineto width (* slope width))
		   (rlineto (- xwid) 0)
		   (closefill))))

(define (end-output) "")

(define (experimental-on) "")

(define (filledbox breadth width depth height) 
  (string-append (ly:number->string (- breadth)) " " 
		 (ly:number->string (- depth)) " "
		 (ly:number->string (+ breadth width)) " "
		 (ly:number->string (+ depth height))
		 " re f "))

(define (round-filled-box breadth width depth height blotdiam)
  (let* ((rad (/ blotdiam 2))
 	 (h (- height rad))
 	 (d (- depth rad))
 	 (w (- width rad))
 	 (b (- breadth rad)))
    (string-append " 0 J "
 		   (setlinewidth blotdiam)
 		   "1 j "
 		   (moveto (- b) (- d))
 		   (rlineto (+ b w) 0)
 		   (rlineto 0 (+ d h))
 		   (rlineto (- (+ b w)) 0)
 		   (rlineto 0 (- (+ d h)))
 		   "b ")))

;; PDF doesn't have the nifty arc operator.  This uses a fast
;; approximation with two curves.  It bulges out a bit more than a
;; true circle should along the 45 degree axes, but most users won't
;; notice.
(define (dot x y radius)
  (string-append (moveto (- x radius) y)
 		 (curveto (- x radius) (+ y (* 1.3333 radius))
 			  (+ x radius) (+ y (* 1.3333 radius))
 			  (+ x radius) y)
 		 (curveto (+ x radius) (- y (* 1.3333 radius))
 			  (- x radius) (- y (* 1.3333 radius))
 			  (- x radius) y)
 		 "f "))


(define (round-filled-box breadth width depth height blot) 
  (filledbox breadth width depth height))

(define (font-def i s) "")

(define (font-switch i) "")

(define (header-end) "")

(define (lily-def key val) "")

(define (header creator generate) "")

(define (invoke-char s i)
  (string-append 
   "(\\" (ly:inexact->string i 8) ") " s " " ))

(define (placebox x y s) "")

(define (bezier-sandwich l thick)
  (string-append (setlinewidth thick)
		 (moveto-pair (list-ref l 7))
		 (curveto-pairs (list-ref l 4)
				(list-ref l 5)
				(list-ref l 6))
		 (lineto-pair (list-ref l 3))
		 (curveto-pairs (list-ref l 0)
				(list-ref l 1)
				(list-ref l 2))
		 "B "
		 (bezier-ending (list-ref l 3) (list-ref l 0) (list-ref l 5))
		 (bezier-ending (list-ref l 7) (list-ref l 0) (list-ref l 5))))

(define (bezier-ending z0 z1 z2)
  (let ((x0 (car z0))
	(y0 (cdr z0))
	(x1 (car z1))
	(y1 (cdr z1))
	(x2 (car z2))
	(y2 (cdr z2)))
    (dot x0 y0 
	 (/ (sqrt (+ (* (- x1 x2) (- x1 x2)) 
		     (* (- y1 y2) (- y1 y2)))) 2))))


(define (start-system width height) "")

(define (stem breadth width depth height) 
  (filledbox breadth width depth height))

(define (stop-system) "")

(define (text s) "")

(define (polygon points blotdiameter) "") ;; TODO

(define (draw-line thick fx fy tx ty)
  (string-append (setlineparams)
		 (setlinewidth thick)
		 (moveto fx fy)
		 (lineto tx ty)
		 "S "))

(define (unknown) "\n unknown\n")

; Problem here -- we're using /F18 for the font, but we don't know 
; for sure that that will exist.

(define (ez-ball ch letter-col ball-col)
  (let ((origin (cons 0.45 0)))
    (string-append (setgray 0)
		   (setlinewidth 1.1)
		   (moveto-pair origin) (lineto-pair origin)
		   (closestroke)
		   (setgray ball-col)
		   (setlinewidth 0.9)
		   (moveto-pair origin) (lineto-pair origin)
		   (closestroke)
		   (setgray letter-col)
		   (moveto-pair origin)
		   "BT "
		   "/F18 0.85 Tf "
		   "-0.28 -0.30 Td " ; move for text block
		   "[(" ch ")] TJ ET ")))

(define (define-origin a b c ) "")
(define (no-origin) "")

(define-public (pdf-output-expression expr port)
  (display (eval expr this-module) port) )


; Local Variables:
; scheme-program-name: "guile"
; End:
