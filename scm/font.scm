;;;
;;; font.scm -- implement Font stuff
;;;
;;; source file of the GNU LilyPond music typesetter
;;; 
;;; (c)  2000--2003 Jan Nieuwenhuizen <janneke@gnu.org>
;;;

(define style-to-font-alist
  `(
))

(define (font-field name font-descr)
      (list-ref
       font-descr
       (cond
	((eq? name 'font-relative-size)  0)
	((eq? name 'font-series) 1)
	((eq? name 'font-shape) 2)
	((eq? name 'font-family) 3)
	((eq? name 'font-design-size) 4)
	(else (ly:warning "unknown font field name"))
	)
       ))



;; most of these routines have been reimplemented in C++ 

;; TODO TODO . (should not use filtering?)
;; this is bad, since we generate garbage every font-lookup.
;; otoh, if the qualifiers is narrow enough , we don't generate much garbage.

(define (filter-field field-name value font-descr-alist)
  "return those descriptions from FONT-DESCR-LIST whose FIELD-NAME matches VALUE"
      (filter
       (lambda (x) (let* (field-value (font-field field-name (car x))) 
		     (or (eq? field-value '*) (eq? value field-value))))
       font-descr-alist)
      )


(define size-independent-fonts
  `(
    ((* * * braces *) . ("feta-braces00"
			 "feta-braces10"
			 "feta-braces20"
			 "feta-braces30"
			 "feta-braces40"
			 "feta-braces50"
			 "feta-braces60"
			 "feta-braces70"
			 "feta-braces80") )))

;; FIXME: what about this comment?:

;; font-lookup seems a  little inefficient -- walking this entire list
;; for a single font.
;;
;;   should really have name/pt size at the front of the list.
;;   (also tried to vary the order of this list, with little effect)
;;
;; (font-relative-size font-series font-shape font-family 
;; font-design-size)

(define paper20-style-sheet-alist
  '(
    ;; why are font file names strings, not symbols?
    ((3 medium upright number 13) . "feta-nummer13")
    ((2 medium upright number 13) . "feta-nummer13")
    ((1 medium upright number 11) . "feta-nummer11")
    ((0 medium upright number 10) . "feta-nummer10")
    ((-1 medium upright number 8) . "feta-nummer8")
    ((-2 medium upright number 7) . "feta-nummer7")
    ((-3 medium upright number 6) . "feta-nummer6")
    ((-4 medium upright number 5) . "feta-nummer5")
    ((-5 medium upright number 4) . "feta-nummer4")

    ((4 medium upright roman 17) . "cmr17")
    ((3 medium upright roman 17) . "cmr17")
    ((2 medium upright roman 12) . "cmr12")
    ((1 medium upright roman 12) . "cmr12")
    ((0 medium upright roman 10) . "cmr10")
    ((-1 medium upright roman 8) . "cmr8" )
    ((-2 medium upright roman 7) . "cmr7" )
    ((-3 medium upright roman 6) . "cmr6" )
    ((-4 medium upright roman 5) . "cmr5" )
    ((-5 medium upright roman 5) . "cmr5" )

    ((3 medium italic roman 12) . "cmti12")
    ((2 medium italic roman 12) . "cmti12")
    ((1 medium italic roman 12) . "cmti12") ;;; ugh. Should add magnification here! 
    ((0 medium italic roman 10) . "cmti10")
    ((-1 medium italic roman 8) . "cmti8")    
    ((-2 medium italic roman 7) . "cmti7")
    ((-3 medium italic roman 7) . "cmti7")    

    ;;; cmbx17 is sauter, not commonly available as Type1.
    ((4 bold upright roman 17) . "cmbx17") 
    ((3 bold upright roman 17) . "cmbx17")
    
    ((2 bold upright roman 14) . "cmbx14")
    ((1 bold upright roman 12) . "cmbx12")
    ((0 bold upright roman 10) . "cmbx10")
    ((-1 bold upright roman 8) . "cmbx8")
    ((-2 bold upright roman 7) . "cmbx7")
    
    ((2 bold italic roman 12) . "cmbxti12")
    ((1 bold italic roman 12) . "cmbxti12")
    ((0 bold italic roman 10) . "cmbxti10")

    ((-1 bold italic roman 8) . "cmbxti8")
    ((-2 bold italic roman 7) . "cmbxti7")
    
    ((4 medium upright typewriter 17) . "cmtt17")
    ((3 medium upright typewriter 17) . "cmtt17")
    ((2 medium upright typewriter 12) . "cmtt12")
    ((1 medium upright typewriter 12) . "cmtt12")
    ((0 medium upright typewriter 10) . "cmtt10")
    ((-1 medium upright typewriter 8) . "cmtt8" )
    ((-2 medium upright typewriter 7) . "cmtt7" )
    ((-3 medium upright typewriter 6) . "cmtt6" )
    ((-4 medium upright typewriter 5) . "cmtt5" )
    ((-5 medium upright typewriter 5) . "cmtt5" )
    
    ((3 medium caps roman 12) . "cmcsc12")
    ((2 medium caps roman 12) . "cmcsc12")
    ((1 medium caps roman 12) . "cmcsc12")
    ((0 medium caps roman 10) . "cmcsc10")
    ((-1 medium caps roman 8) . "cmcsc8")
    ((-2 medium caps roman 7) . "cmcsc7")
    ((-3 medium caps roman 7) . "cmcsc7")

    ((3 * * dynamic 19) . "feta-din19")
    ((2 * * dynamic 19) . "feta-din19")
    ((1 * * dynamic 17) . "feta-din17")
    ((0 * * dynamic 14) . "feta-din14")
    ((-1 * * dynamic 12) . "feta-din12")
    ((-2 * * dynamic 9) . "feta-din9")
    ((-3 * * dynamic 8) . "feta-din8")
    ((-4 * * dynamic 7) . "feta-din7")
    ((-5 * * dynamic 6) . "feta-din6")

    ((2 * * music 26) . ("feta26" "parmesan26"))
    ((1 * * music 23) . ("feta23" "parmesan23"))
    ((0 * * music 20) . ("feta20" "parmesan20"))
    ((-1 * * music 16) . ("feta16" "parmesan16"))
    ((-2 * * music 13) . ("feta13" "parmesan13"))
    ((-3 * * music 11) . ("feta11" "parmesan11"))
    ((-4 * * music 11) . ("feta11" "parmesan11"))
    
    ((2 * * ancient 26) . ("feta26" "parmesan26"))
    ((1 * * ancient 23) . ("feta23" "parmesan23"))
    ((0 * * ancient 20) . ("feta20" "parmesan20"))
    ((-1 * * ancient 16) . ("feta16" "parmesan16"))
    ((-2 * * ancient 13) . ("feta13" "parmesan13"))
    ((-3 * * ancient 11) . ("feta11" "parmesan11"))
    ((-4 * * ancient 11) . ("feta11" "parmesan11"))

    ((0 * * math 10) . "msam10")
    ((-1 * * math 10) . "msam10")
    ((-2 * * math 10) . "msam10")
    ((-3 * * math 10) . "msam10")
   ))

;; 
(define (change-relative-size font-desc decrement)
  "return a FONT-DESCR with relative size decremented by DECREMENT"

  (if (number? (car font-desc))
      (cons (- (car font-desc) decrement) (cdr font-desc))
      font-desc)
  )

(define (change-rhs-size font-desc from to )
  (cons (car font-desc)
	(regexp-substitute/global #f from (cdr font-desc) 'pre to 'post))

  )
  
;; 
(define (map-alist-keys func list)
  "map a  function FUNC over the keys of an alist LIST, leaving the vals. "
  (if (null?  list)
      '()
      (cons (cons (func (caar list)) (cdar list))
	    (map-alist-keys func (cdr list)))
      ))
 

;; 
(define (map-alist-vals func list)
  "map a function FUNC over the vals of  LIST, leaving the keys."
  (if (null?  list)
      '()
      (cons (cons  (caar list) (func (cdar list)))
	    (map-alist-vals func (cdr list)))
      ))

(define (change-style-sheet-relative-size sheet x)
  (map-alist-keys (lambda (descr) (change-relative-size descr  x)) sheet))


;; make style sheet for each paper version.
(define font-list-alist
  (map-alist-vals (lambda (x) (change-style-sheet-relative-size
			       paper20-style-sheet-alist x))
		  '((paper11 . -3)
		    (paper13 . -2)
		    (paper16 . -1)
		    (paper20 . 0)
		    (paper23 . 1)
		    (paper26 . 2)
		    ))
  )

;;
;; make a kludged up paper-19 style sheet. Broken by virtual fonts.
;;
(if #f
    (set! font-list-alist
      (cons
       (cons
	'paper19
	(map (lambda (x) (change-rhs-size x "20" "19"))
	     paper20-style-sheet-alist))
       font-list-alist)))

(define-public (make-font-list sym)
  (append size-independent-fonts
	  (cdr (assoc sym font-list-alist))))

(define (qualifiers-to-fontnames  qualifiers font-descr-alist)
  " reduce the font list by successively applying a font-qualifier."
  (if (null? qualifiers)
      font-descr-alist
      
      (qualifiers-to-fontnames
       (cdr qualifiers)
       (filter-field (caar qualifiers) (cdar qualifiers) font-descr-alist)
      )
  ))

(define (wild-eq? x y)
  (or (eq? x y)
      (eq? x '*)
      (eq? y '*)))
       
(define (font-qualifies? qualifiers font-desc)
  "does FONT-DESC satisfy QUALIFIERS?"
  (if (null? qualifiers) #t
      (if (wild-eq? (font-field (caar qualifiers) font-desc) (cdar qualifiers))
	  (font-qualifies? (cdr qualifiers) font-desc)
	  #f)))

(define (find-first-font qualifiers fonts)
  (if (null? fonts)
      ""
      (if (font-qualifies? qualifiers (caar fonts))
	  (cdar fonts)
	  (find-first-font qualifiers (cdr fonts))
	)
      ))


(define (select-unique-font qualifiers fonts)
  "return a single font from FONTS (or a default, if none found)
and warn if the selected font is not unique.
"
  (let*  (
	  (err (current-error-port))
	  )
    

  (if (not (= (length fonts) 1))
      (begin
	(display "\ncouldn't find unique font satisfying " err)
	(write qualifiers err)
	(display " found " err)
	(if (null? fonts)
	    (display "none" err)
	    (write (map cdr  fonts) err))
	))
  
  (if (null? fonts)
      "cmr10"
      (cdar fonts))	; return the topmost.
  
  ))


; there used to be a Scheme  properties-to-font-name function,
; but that is  superseeded by the C++ version  out of speed concerns.


(define-public (markup-to-properties abbrev-alist style-alist markup)
  "DOCME."
  ;; (display "markup: `")
  ;; (write markup)
  ;; (display "'\n")
  
  (if (pair? markup)
      ;; This is hairy.  We want to allow:
      ;;    ((foo bar) "text")
      ;;    ((foo (bar . 1)) "text")
      ;;    ((foo . (0 . 1))) 
      
      (if (and (symbol? (car markup))
	       (or (not (pair? (cdr markup)))
		   (number? (cadr markup))))
	  (if (equal? '() (cdr markup))
	      (markup-to-properties abbrev-alist style-alist (car markup))
	      (list markup))
	  
	  (if (equal? '() (cdr markup))
	      (markup-to-properties abbrev-alist style-alist (car markup))
	      (append (markup-to-properties abbrev-alist style-alist (car markup))
		      (markup-to-properties abbrev-alist style-alist (cdr markup)))))
      
      ;; markup is single abbreviation
      (let ((entry (assoc markup
			  ;; assoc-chain?
			  (append abbrev-alist style-alist))))
	(if entry
	    (cdr entry)
	    (list (cons markup #t))))))


; fixme, how's this supposed to work?
; and why don't we import font-setting from elt?
(define (style-to-font-name sheet style)
  (let* ((entry (assoc style style-to-font-alist))
	 (qualifiers (if entry (cdr entry) '()))
	 (font (find-first-font qualifiers sheet))
	 (err (current-error-port))
	 )

    (if (equal? font "")
	(begin
	  (display "\ncouldn't find any font satisfying " err)
	  (write qualifiers err)
	  "cmr10"
	  )
	font)	; return the topmost.
    ))

(if #f (begin
	 (define (test-module)
	   (display (filter pair? '(1 2 (1 2) (1 .2)))
		    (display (filter-field 'font-name 'cmbx paper20-style-sheet-alist))
		    
		    (display (qualifiers-to-fontname '((font-name . cmbx)) paper20-style-sheet-alist))
		    (display (style-to-font-name 'paper20 'large)))
	   )))

;;; ascii-script font init
(define as-font-sheet-alist
  '((as5 . (((* * * braces *) . ("as-braces9"))
	    ((* * * number *) . "as-number1")
	    ((0 * * music *) . ("as5"))
	    ((0 * * roman *) . "as-dummy")))
    (as9 . (((* * * braces *) . ("as-braces9"))
	    ((0 medium upright number 10) . "as-number4")
	    ((* * * number 5) . "as-number1")
	    ((0 * * music *) . ("as9"))
	    ((0 * * roman *) . "as-dummy")))))

(define-public (as-make-font-list sym)
  (set! font-list-alist
	(append font-list-alist as-font-sheet-alist))
  (make-font-list sym))




