;;;; output-svg.scm -- implement Scheme output routines for SVG1
;;;;
;;;;  source file of the GNU LilyPond music typesetter
;;;;
;;;; (c) 2002--2005 Jan Nieuwenhuizen <janneke@gnu.org>

;;;; http://www.w3.org/TR/SVG11
;;;; http://www.w3.org/TR/SVG12/ -- page, pageSet in draft

;;;; TODO:
;;;;  * .cff MUST NOT be in fc's fontpath.
;;;;    - workaround: remove mf/out from ~/.fonts.conf,
;;;;      instead add ~/.fonts and symlink all /mf/out/*otf there.
;;;;    - bug in fontconfig/freetype/pango?

;;;;  * inkscape page/pageSet support
;;;;  * inkscape SVG-font support
;;;;    - use fontconfig/fc-cache for now, see output-gnome.scm

(define-module (scm output-svg))
(define this-module (current-module))

(use-modules
 (guile)
 (ice-9 regex)
 (lily)
 (srfi srfi-13))


(define lily-unit-length 1.75)

(define (dispatch expr)
  (let ((keyword (car expr)))
    (cond
     ((eq? keyword 'some-func) "")
     ;;((eq? keyword 'placebox) (dispatch (cadddr expr)))
     (else
      (if (module-defined? this-module keyword)
	  (apply (eval keyword this-module) (cdr expr))
	  (begin
	    (ly:warning (_ "undefined: ~S") keyword)
	    ""))))))

;; Helper functions
(define-public (attributes attributes-alist)
  (apply string-append
	 (map (lambda (x) (format #f " ~s=\"~a\"" (car x) (cdr x)))
	      attributes-alist)))

(define-public (eo entity . attributes-alist)
  "o = open"
  (format #f "<~S~a>\n" entity (attributes attributes-alist)))

(define-public (eoc entity . attributes-alist)
  " oc = open/close"
  (format #f "<~S~a/>\n" entity (attributes attributes-alist)))

(define-public (ec entity)
  "c = close"
  (format #f "</~S>\n" entity))



(define-public (entity entity string . attributes-alist)
  (if (equal? string "")
      (apply eoc entity attributes-alist)
      (string-append
       (apply eo (cons entity attributes-alist)) string (ec entity))))

(define (offset->point o)
  (format #f " ~S,~S" (car o)  (- (cdr o))))

(define (svg-bezier lst close)
  (let* ((c0 (car (list-tail lst 3)))
	 (c123 (list-head lst 3)))
    (string-append
     (if (not close) "M " "L ")
     (offset->point c0)
     "C " (apply string-append (map offset->point c123))
     (if (not close) "" (string-append
			 "L " (offset->point close))))))

(define (sqr x)
  (* x x))

(define (integer->entity integer)
  (format #f "&#x~x;" integer))

(define (char->entity char)
  (integer->entity (char->integer char)))

(define (string->entities string)
  (apply string-append
	 (map (lambda (x) (char->entity x)) (string->list string))))

(define pango-description-regexp-comma
  (make-regexp "^([^,]+), ?([-a-zA-Z_]*) ([0-9.]+)$"))

(define pango-description-regexp-nocomma
  (make-regexp "^([^ ]+) ([-a-zA-Z_]*) ?([0-9.]+)$"))

(define (pango-description-to-svg-font str)
  (let*
      ((size 4.0)
       (family "Helvetica")
       (style #f)
       (match-1 (regexp-exec pango-description-regexp-comma str))
       (match-2 (regexp-exec pango-description-regexp-nocomma str))
       (match (if match-1
		  match-1
		  match-2)))

    (if (regexp-match? match)
	(begin
	  (set! family (match:substring match 1))
	  (if (< 0 (string-length (match:substring match 2)))
	      (set! style (match:substring match 2)))
	  (set! size
		(string->number (match:substring match 3))))

	(ly:warning (_ "can't decypher Pango description: ~a") str))

    (set! style
	  (if (string? style)
	      (format "font-style:~a;" style)
	      ""))
    
    (format "font-family:~a;~afont-size:~a;text-anchor:west"
	    family
	    style
	    (/ size lily-unit-length))
    ))

;;; FONT may be font smob, or pango font string
(define (svg-font font)
  (if (string? font)
      (pango-description-to-svg-font font)
      (let ((name-style (font-name-style font))
	    (size (modified-font-metric-font-scaling font))
	    (anchor "west"))

	(format #f "font-family:~a;font-style:~a;font-size:~a;text-anchor:~a;"
		(car name-style) (cadr name-style)
		size anchor))))

(define (fontify font expr)
  (entity 'text expr
	  `(style . ,(svg-font font))
	  '(fill . "currentColor")
	  ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; stencil outputters
;;;

;;; catch-all for missing stuff
;;; comment this out to see find out what functions you miss :-)

(if #f
    (begin
      (define (dummy . foo) "")
      (map (lambda (x) (module-define! this-module x dummy))
	   (append
	    (ly:all-stencil-expressions)
	    (ly:all-output-backend-commands)))
      ))

(define (url-link url x y)
  (string-append
   (eo 'a `(xlink:href . ,url))
   (eoc 'rect
	`(x . ,(car x))
	`(y . ,(car y))
	`(width . ,(- (cdr x) (car x)))
	`(height . ,(- (cdr y) (car y)))
	'(fill . "none")
	'(stroke . "none")
	'(stroke-width . "0.0"))
   (ec 'a)))

(define (grob-cause offset grob)
  "")

(define (no-origin)
  "")



(define (bezier-sandwich lst thick)
  (let* ((first (list-tail lst 4))
	 (first-c0 (car (list-tail first 3)))
	 (second (list-head lst 4)))
    (entity 'path ""
	    '(stroke-linejoin . "round")
	    '(stroke-linecap . "round")
	    `(stroke-width . ,thick)
	    '(stroke . "currentColor")
	    '(fill . "currentColor")
	    `(d . ,(string-append (svg-bezier first #f)
				  (svg-bezier second first-c0)))
	    )))

(define (char font i)
  (dispatch
   `(fontify ,font ,(entity 'tspan (char->entity (integer->char i))))))

(define-public (comment s)
  (string-append "<!-- " s " !-->\n"))

(define (draw-line thick x1 y1 x2 y2 . alist)
  
  (apply entity 'line ""
	 (append
	  `((stroke-linejoin . "round")
	    (stroke-linecap . "round")
	    (stroke-width . ,thick)
	    (stroke . "currentColor")
	    (x1 . ,x1)
	    (y1 . ,(- y1))
	    (x2 . ,x2)
	    (y2 . ,(- y2)))
	  alist)))

(define (dashed-line thick on off dx dy)
  (draw-line thick 0 0 dx dy `(style . ,(format "stroke-dasharray:~a,~a;" on off))))

(define (named-glyph font name)
  (dispatch
   `(fontify ,font ,(entity 'tspan
			    (integer->entity
			     (ly:font-glyph-name-to-charcode font name))))))

(define (placebox x y expr)
  (entity 'g
	  
	  ;; FIXME -- JCN
	  ;;(dispatch expr)
	  expr
	  `(transform . ,(format #f "translate (~f, ~f)"
				 x (- y)))))

(define (polygon coords blot-diameter is-filled)
  (entity
   'polygon ""
   '(stroke-linejoin . "round")
   '(stroke-linecap . "round")
   `(stroke-width . ,blot-diameter)
   `(fill . ,(if is-filled "currentColor" "none"))
   '(stroke . "currentColor")
   `(points . ,(string-join
		(map offset->point (ly:list->offsets '() coords))))
   ))

(define (round-filled-box breapth width depth height blot-diameter)
  (entity 'rect ""
	  ;; The stroke will stick out.  To use stroke,
	  ;; the stroke-width must be subtracted from all other dimensions.
	  ;;'(stroke-linejoin . "round")
	  ;;'(stroke-linecap . "round")
	  ;;`(stroke-width . ,blot)
	  ;;'(stroke . "red")
	  ;;'(fill . "orange")

	  `(x . ,(- breapth))
	  `(y . ,(- height))
	  `(width . ,(+ breapth width))
	  `(height . ,(+ depth height))
	  `(ry . ,(/ blot-diameter 2))
	  ))

(define (circle radius thick is-filled)
  (entity
   'circle ""
   '(stroke-linejoin . "round")
   '(stroke-linecap . "round")
   `(fill . ,(if is-filled "currentColor" "none"))
   `(stroke . "currentColor")
   `(stroke-width . ,thick)
   `(r . ,radius)))

(define (text font string)
  (dispatch `(fontify ,font ,(entity 'tspan (string->entities string)))))

(define (utf-8-string pango-font-description string)
  (dispatch `(fontify ,pango-font-description ,(entity 'tspan string))))



(define (setcolor r g b)
  (format "<g color=\"rgb(~a%,~a%,~a%)\">"
	  (* 100 r) (* 100 g) (* 100 b)
	  ))

(define (resetcolor)
  "</g>")
