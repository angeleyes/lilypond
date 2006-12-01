;;;; output-lib.scm -- implement Scheme output helper functions
;;;;
;;;;  source file of the GNU LilyPond music typesetter
;;;; 
;;;; (c) 1998--2006 Jan Nieuwenhuizen <janneke@gnu.org>
;;;; Han-Wen Nienhuys <hanwen@xs4all.nl>




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; general

(define-public (make-stencil-boxer thickness padding callback)

  "Return function that adds a box around the grob passed as argument."
  (lambda (grob)
    
    (box-stencil (callback grob) thickness padding)))

(define-public (make-stencil-circler thickness padding callback)
  "Return function that adds a circle around the grob passed as argument."
  
  (lambda (grob) (circle-stencil (callback grob) thickness padding)))

(define-public (print-circled-text-callback grob)
  (let* ((text (ly:grob-property grob 'text))
	 
	 (layout (ly:grob-layout grob))
	 (defs (ly:output-def-lookup layout 'text-font-defaults))
	 (props (ly:grob-alist-chain grob defs))
	 (circle (ly:text-interface::interpret-markup
		  layout props (make-circle-markup text))))
    circle))

(define-public (music-cause grob)
  (let*
      ((event (event-cause grob)))

    (if (ly:stream-event? event)
	(ly:event-property event 'music-cause)
	#f)))

(define-public (event-cause grob)
  (let*
      ((cause (ly:grob-property  grob 'cause)))
    
    (cond
     ((ly:stream-event? cause) cause)
     ((ly:grob? cause) (event-cause cause))
     (else #f))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; tablature

;; The TabNoteHead tablatureFormat callback.
;; Compute the text grob-property
(define-public (fret-number-tablature-format string tuning pitch)
  (make-whiteout-markup
   (make-vcenter-markup  
    (number->string
     (- (ly:pitch-semitones pitch)
	(list-ref tuning
		  ;; remove 1 because list index starts at 0 and guitar string at 1. 
		  (- string 1)))))))

;; The 5-string banjo has got a extra string, the fifth (duh), wich
;; starts at the fifth fret on the neck. Frets on the fifth string
;; are referred to relative to the other frets:
;;   the "first fret" on the fifth string is really the sixth fret
;;   on the banjo neck.
;; We solve this by defining a new fret-number-tablature function:
(define-public (fret-number-tablature-format-banjo string tuning pitch)
  (make-whiteout-markup
   (make-vcenter-markup  
    (let ((fret (- (ly:pitch-semitones pitch) (list-ref tuning (- string 1)))))
      (number->string (cond
		       ((and (> fret 0) (= string 5))
			(+ fret 5))
		       (else fret)))))))


; default tunings for common string instruments
(define-public guitar-tuning '(4 -1 -5 -10 -15 -20))
(define-public guitar-open-g-tuning '(2 -1 -5 -10 -17 -22))
(define-public bass-tuning '(-17 -22 -27 -32))
(define-public mandolin-tuning '(16 9 2 -5))

;; tunings for 5-string banjo
(define-public banjo-open-g-tuning '(2 -1 -5 -10 7))
(define-public banjo-c-tuning '(2 -1 -5 -12 7))
(define-public banjo-modal-tuning '(2 0 -5 -10 7))
(define-public banjo-open-d-tuning '(2 -3 -6 -10 9))
(define-public banjo-open-dm-tuning '(2 -3 -6 -10 9))
;; convert 5-string banjo tuning to 4-string by removing the 5th string
(define-public (four-string-banjo tuning)
  (reverse (cdr (reverse tuning))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; note heads


(define-public (stem::calc-duration-log grob)
  (ly:duration-log
   (ly:event-property (event-cause grob) 'duration)))

(define-public (note-head::calc-duration-log grob)
  (min 2 
       (ly:duration-log
	(ly:event-property (event-cause grob) 'duration))))

(define-public (dots::calc-dot-count grob)
  (ly:duration-dot-count
   (ly:event-property (event-cause grob) 'duration)))

(define (note-head::calc-tablature-stem-attachment grob)
  (cons 0.0 1.35))



;; silly, use alist? 
(define-public (note-head::calc-glyph-name grob)
  (let*
      ((style (ly:grob-property grob 'style))
       (log (min 2 (ly:grob-property grob 'duration-log))))
    
    (case style
      ;; "default" style is directly handled in note-head.cc as a
      ;; special case (HW says, mainly for performance reasons).
      ;; Therefore, style "default" does not appear in this case
      ;; statement.  -- jr
      ((xcircle) "2xcircle")
      ((harmonic) "0harmonic")
      ((baroque) 
       ;; Oops, I actually would not call this "baroque", but, for
       ;; backwards compatibility to 1.4, this is supposed to take
       ;; brevis, longa and maxima from the neo-mensural font and all
       ;; other note heads from the default font.  -- jr
       (if (< log 0)
	   (string-append (number->string log) "neomensural")
	   (number->string log)))
      ((mensural)
       (string-append (number->string log) (symbol->string style)))
      ((petrucci)
       (if (< log 0)
	   (string-append (number->string log) "mensural")
	   (string-append (number->string log) (symbol->string style))))
      ((neomensural)
       (string-append (number->string log) (symbol->string style)))
      (else
       (if (string-match "vaticana*|hufnagel*|medicaea*" (symbol->string style))
	   (symbol->string style)
	   (string-append (number->string (max 0 log))
			  (symbol->string style)))))))

;; TODO junk completely?
(define (note-head-style->attachment-coordinates grob axis)
  "Return pair (X . Y), containing multipliers for the note head
bounding box, where to attach the stem. e.g.: X==0 means horizontally
centered, X==1 is at the right, X == -1 is at the left."

  '(1.0 . 0.0))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; bar numbers

(define-public ((every-nth-bar-number-visible n) barnum) (= 0 (modulo barnum n)))

(define-public ((modulo-bar-number-visible n m) barnum) (and (> barnum 1) (= m (modulo barnum n))))

(define-public ((set-bar-number-visibility n) tr)
  (let* ((bn (ly:context-property tr 'currentBarNumber)))
    (ly:context-set-property! tr 'barNumberVisibility (modulo-bar-number-visible n (modulo bn n)))))

(define-public (first-bar-number-invisible barnum) (> barnum 1))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; break visibility

(define-public begin-of-line-visible
  #(#f #f #t))
(define-public end-of-line-visible
  #(#t #f #f))
(define-public end-of-line-invisible
  #(#f #t #t))
(define-public spanbar-begin-of-line-invisible
  #(#t #f #f))
(define-public all-visible #(#t #t #t))
(define-public all-invisible #(#f #f #f))
(define-public begin-of-line-invisible
  #(#t #t #f))
(define-public center-invisible #(#t #f #t))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Bar lines.

;;
;; How should a  bar line behave at a break? 
;;
;; Why prepend `default-' to every scm identifier?
(define-public (bar-line::calc-glyph-name grob)
  (let* (
	 (glyph (ly:grob-property grob 'glyph))
	 (dir (ly:item-break-dir grob))
	 (result (assoc glyph 
		       '((":|:" . (":|" . "|:"))
			 ("||:" . ("||" . "|:"))
			 ("dashed" . ("dashed" . '())) 
			 ("|" . ("|" . ()))
			 ("||:" . ("||" . "|:"))
			 ("|s" . (() . "|"))
			 ("|:" . ("|" . "|:"))
			 ("|." . ("|." . ()))

			 ;; hmm... should we end with a bar line here?
			 (".|" . ("|" . ".|"))
			 (":|" . (":|" . ()))
			 ("||" . ("||" . ()))
			 (".|." . (".|." . ()))
			 ("" . ("" . ""))
			 (":" . (":" . ""))
			 ("." . ("." . ()))
			 ("empty" . (() . ()))
			 ("brace" . (() . "brace"))
			 ("bracket" . (() . "bracket"))  )))
	 (glyph-name (if (= dir CENTER)
			 glyph
		         (if (and result (string? (index-cell (cdr result) dir)))
			     (index-cell (cdr result) dir)
			     #f)))
	 )

    (if (not glyph-name)
	(ly:grob-suicide! grob))

    glyph-name))


(define-public (shift-right-at-line-begin g)
  "Shift an item to the right, but only at the start of the line."
  (if (and (ly:item? g)
	   (equal? (ly:item-break-dir g) RIGHT))
      (ly:grob-translate-axis! g 3.5 X)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Tuplets

(define-public (tuplet-number::calc-denominator-text grob)
  (number->string (ly:event-property (event-cause grob) 'denominator)))

(define-public (tuplet-number::calc-fraction-text grob)
  (let*
      ((ev (event-cause grob)))

    (format "~a:~a" 
	    (ly:event-property ev 'denominator)
	    (ly:event-property ev 'numerator))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Color

(define-public color? list?)

; predefined colors
(define-public black       '(0.0 0.0 0.0))
(define-public white       '(1.0 1.0 1.0))
(define-public red         '(1.0 0.0 0.0))
(define-public green       '(0.0 1.0 0.0))
(define-public blue        '(0.0 0.0 1.0))
(define-public cyan        '(0.0 1.0 1.0))
(define-public magenta     '(1.0 0.0 1.0))
(define-public yellow      '(1.0 1.0 0.0))

(define-public grey        '(0.5 0.5 0.5))
(define-public darkred     '(0.5 0.0 0.0))
(define-public darkgreen   '(0.0 0.5 0.0))
(define-public darkblue    '(0.0 0.0 0.5))
(define-public darkcyan    '(0.0 0.5 0.5))
(define-public darkmagenta '(0.5 0.0 0.5))
(define-public darkyellow  '(0.5 0.5 0.0))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; * Pitch Trill Heads
;; * Parentheses

(define (parenthesize-elements grob . rest)
  (let*
      ((refp (if (null? rest)
		 grob
		 (car rest)))
       (elts (ly:grob-object grob 'elements))
       (x-ext (ly:relative-group-extent elts refp X))

       (font (ly:grob-default-font grob))
       (lp (ly:font-get-glyph font "accidentals.leftparen"))
       (rp (ly:font-get-glyph font "accidentals.rightparen"))
       (padding (ly:grob-property grob 'padding 0.1)))
    
    (ly:stencil-add
     (ly:stencil-translate-axis lp (- (car x-ext) padding) X)
     (ly:stencil-translate-axis rp (+ (cdr x-ext) padding) X))
  ))


(define (parentheses-item::print me)
  (let*
      ((elts (ly:grob-object me 'elements))
       (y-ref (ly:grob-common-refpoint-of-array me elts Y))
       (x-ref (ly:grob-common-refpoint-of-array me elts X))
       (stencil (parenthesize-elements me x-ref))
       (elt-y-ext  (ly:relative-group-extent elts y-ref Y))
       (y-center (interval-center elt-y-ext)))

    (ly:stencil-translate
     stencil
     (cons
      (-
       (ly:grob-relative-coordinate me x-ref X))
      (-
       y-center
       (ly:grob-relative-coordinate me y-ref Y))))
    ))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 

(define-public (chain-grob-member-functions grob value . funcs)
  (for-each
   (lambda (func)
     (set! value (func grob value)))
   funcs)

  value)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; falls

(define-public (fall::print spanner)
  (let*
      ((delta-y (* 0.5 (ly:grob-property spanner 'delta-position)))
       (left-span (ly:spanner-bound spanner LEFT))
       (right-span (ly:spanner-bound spanner RIGHT))
       (thickness (* (ly:grob-property spanner 'thickness)
		     (ly:output-def-lookup (ly:grob-layout spanner)
					   'line-thickness)))
       (padding (ly:grob-property spanner 'padding 0.5))
       (common (ly:grob-common-refpoint right-span
					(ly:grob-common-refpoint spanner
								 left-span X)
					X))
       (left-x (+ padding
		  (interval-end  (ly:grob-robust-relative-extent
				  left-span common X))))
       (right-x (- (interval-start
		    (ly:grob-robust-relative-extent right-span common X))
		   padding))
       (self-x (ly:grob-relative-coordinate spanner common X))
       (dx (- right-x left-x))
       (exp (list 'path thickness 
		  `(quote
		    (rmoveto
		     ,(- left-x self-x) 0

		     rcurveto		     
		     ,(/ dx 3)
		     0
		     ,dx ,(* 0.66 delta-y)
		     ,dx ,delta-y
		     ))))
       )

    (ly:make-stencil
     exp
     (cons 0 dx)
     (cons (min 0 delta-y)
	   (max 0 delta-y)))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; grace spacing


(define-public (grace-spacing::calc-shortest-duration grob)
  (let*
     ((cols (ly:grob-object grob 'columns))
      (get-difference
       (lambda (idx)
	 (ly:moment-sub (ly:grob-property
			 (ly:grob-array-ref cols (1+ idx)) 'when)
			(ly:grob-property
			 (ly:grob-array-ref cols idx) 'when))))
      
      (moment-min (lambda (x y)
		    (cond
		     ((and x y)
		      (if (ly:moment<? x y)
			    x
			    y))
		     (x x)
		     (y y)))))
		     
    (fold moment-min #f (map get-difference
			     (iota (1- (ly:grob-array-length cols)))))))



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; fingering

(define-public (fingering::calc-text grob)
  (let*
      ((event (event-cause grob))
       (digit (ly:event-property event 'digit)))
    
    (if (> digit 5)
	(ly:input-message (ly:event-property event 'origin)
			  "Warning: Fingering notation for finger number ~a" digit))

    (number->string digit 10)
  ))

(define-public (string-number::calc-text grob)
  (let*
      ((digit (ly:event-property (event-cause  grob) 'string-number)))
    
    (number->string digit 10)
  ))


(define-public (stroke-finger::calc-text grob)
  (let*
      ((digit (ly:event-property (event-cause grob) 'digit))
       (text (ly:event-property (event-cause grob) 'text)))

    (if (string? text)
	text
	(vector-ref (ly:grob-property grob 'digit-names)  (1- (max (min 5 digit) 1))))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; dynamics
(define-public (hairpin::calc-grow-direction grob)
  (if (eq? (ly:event-property (event-cause grob) 'class) 'decrescendo-event)
      START
      STOP
      ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; lyrics

(define-public (lyric-text::print grob)
  "Allow interpretation of tildes as lyric tieing marks."
  
  (let*
      ((text (ly:grob-property grob 'text))
       (layout (ly:grob-layout grob))
       (defs (ly:output-def-lookup layout 'text-font-defaults))
       (props (ly:grob-alist-chain grob defs)))

    (ly:text-interface::interpret-markup layout
					 props
					 (if (string? text)
					     (make-tied-lyric-markup text)
					     text))))

(define-public ((grob::calc-property-by-copy prop) grob)
  (ly:event-property (event-cause grob) prop))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; fret boards

(define (string-frets->description string-frets string-count)
  (let*
      ((desc (list->vector
	      (map (lambda (x) (list 'mute  (1+ x)))
		   (iota string-count)))))
       
       (for-each (lambda (sf)
		   (let*
		       ((string (car sf))
			(fret (cadr sf))
			(finger (caddr sf)))

		     
		     (vector-set! desc (1- string)
				  (if (= 0 fret)
				      (list 'open string)
				      (if finger
					  (list 'place-fret string fret finger) 
					  (list 'place-fret string fret))
					  

				      ))
		     ))
		 string-frets)

       (vector->list desc)))

(define-public (fret-board::calc-stencil grob)
  (let* ((string-frets (ly:grob-property grob 'string-fret-finger-combinations))
	 (string-count (ly:grob-property grob 'string-count))
	 (layout (ly:grob-layout grob))
	 (defs (ly:output-def-lookup layout 'text-font-defaults))
	 (props (ly:grob-alist-chain grob defs)))

    (make-fret-diagram layout props
		       (string-frets->description string-frets 6))))
