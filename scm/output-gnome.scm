;;;; output-gnome.scm -- implement GNOME canvas output
;;;;
;;;;  source file of the GNU LilyPond music typesetter
;;;; 
;;;; (c)  2004 Jan Nieuwenhuizen <janneke@gnu.org>

;;; TODO:
;;;
;;;  * Figure out and fix font scaling and character placement
;;;  * EC font package: add missing X font directories and AFMs
;;;  * User-interface, keybindings
;;;  * Implement missing stencil functions
;;;  * Implement missing commands
;;;  * More information in stencils, e.g., location and grob tag.
;;;  * Embedded Lily:
;;;    - allow GnomeCanvas or `toplevel' GtkWindow to be created
;;;      outside of LilyPond
;;;    - lilylib.
;;;  * Release schedule and packaging of dependencies.  This hack
;;;    depends on several CVS and TLA development sources.

;;; You need:
;;;
;;;   * Rotty's g-wrap >= 1.9.1 (or TLA)
;;;   * guile-gnome-platform >= 2.5.992 (or TLA)
;;;   * pango >= 1.5.2 (or CVS)
;;;
;;; See also: guile-gtk-general@gnu.org

;;; Try it
;;;
;;;   [* Get cvs and tla]
;;;
;;;   * Install gnome/gtk and libffi development stuff
;;;
;;;   * Install pango, g-wrap and guile-gnome from CVS or arch: 
;;;     see buildscripts/guile-gnome.sh
;;;  
;;;   * Build LilyPond with gui support: configure --enable-gui
;;;
;;;   * Supposing that LilyPond was built in ~/cvs/savannah/lilypond,
;;;     tell fontconfig about the feta fonts dir and run fc-cache
"
cat > ~/.fonts.conf << EOF
<fontconfig>
<dir>~/cvs/savannah/lilypond/mf/out</dir>
<dir>/usr/share/texmf/fonts/type1/public/ec-fonts-mftraced</dir>
</fontconfig>
EOF
fc-cache
"
;;;     or copy all your .pfa/.pfb's to ~/.fonts if your fontconfig
;;;     already looks there for fonts.  Check if it works by doing:
"
fc-list | grep -i lily
"
;;;
;;;   * Setup environment
"
export GUILE_LOAD_PATH=$HOME/usr/pkg/g-wrap/share/guile/site:$HOME/usr/pkg/g-wrap/share/guile/site/g-wrap:$HOME/usr/pkg/guile-gnome/share/guile:$GUILE_LOAD_PATH
export LD_LIBRARY_PATH=$HOME/usr/pkg/pango/lib:$HOME/usr/pkg/g-wrap/lib:$HOME/usr/pkg/guile-gnome/lib:$LD_LIBRARY_PATH
export XEDITOR='/usr/bin/emacsclient --no-wait +%l:%c %f'
"
;;;  * Also for GNOME point-and-click, you need to set XEDITOR and add
"
#(ly:set-point-and-click 'line-column)
"
;;;    to your .ly.
;;;
;;;  * Run lily:
"
lilypond -fgnome input/simple-song.ly
"
;;; point-and-click: (mouse-1) click on a graphical object;
;;; grob-property-list: (mouse-3) click on a graphical object.

(debug-enable 'backtrace)

(define-module (scm output-gnome))
(define this-module (current-module))

(use-modules
 (guile)
 (ice-9 regex)
 (srfi srfi-13)
 (lily)
 (gnome gtk))


;; The name of the module will change to `canvas' rsn
(if (resolve-module '(gnome gw canvas))
    (use-modules (gnome gw canvas))
    (use-modules (gnome gw libgnomecanvas)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Wrappers from guile-gnome TLA
;;; guile-gnome-devel@gnu.org--2004
;;; http://arch.gna.org/guile-gnome/archive-2004
;;;
;;; janneke@gnu.org--2004-gnome
;;; http://lilypond.org/~janneke/{arch}/2004-gnome
;;;
(if (not (defined? '<gnome-canvas-path-def>))
    (begin
      (define-class <gnome-canvas-path-def> (<gobject>)
	(closure #:init-value (gnome-canvas-path-def-new)
		 #:init-keyword #:path-def
		 #:getter get-def #:setter set-def))
      
      (define-method (moveto (this <gnome-canvas-path-def>) x y)
	(gnome-canvas-path-def-moveto (get-def this) x y))
      (define-method (curveto (this <gnome-canvas-path-def>) x1 y1 x2 y2 x3 y3)
	(gnome-canvas-path-def-curveto (get-def this)  x1 y1 x2 y2 x3 y3))
      (define-method (lineto (this <gnome-canvas-path-def>) x y)
	(gnome-canvas-path-def-lineto (get-def this) x y))
      (define-method (closepath (this <gnome-canvas-path-def>))
	(gnome-canvas-path-def-closepath (get-def this)))
      (define-method (reset (this <gnome-canvas-path-def>))
	(gnome-canvas-path-def-reset (get-def this)))
      
      (define -set-path-def set-path-def)
      (define -get-path-def get-path-def)
      
      (define-method (set-path-def (this <gnome-canvas-shape>)
				   (def <gnome-canvas-path-def>))
	(-set-path-def this (get-def def)))
      
      (define-method (get-path-def (this <gnome-canvas-shape>))
	(make <gnome-canvas-path-def> #:path-def (-get-path-def this)))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; globals

;; junkme
(define system-origin '(0 . 0))

;;; set by framework-gnome.scm
(define canvas-root #f)
(define output-scale #f)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; helper functions

(define (stderr string . rest)
  (apply format (cons (current-error-port) (cons string rest)))
  (force-output (current-error-port)))

(define (debugf string . rest)
  (if #f
      (apply stderr (cons string rest))))

(define (utf8 i)
  (cond
   ((< i #x80) (list (integer->char i)))
   ((< i #x800) (map integer->char
		     (list (+ #xc0 (quotient i #x40))
			   (+ #x80 (modulo i #x40)))))
   ((< i #x10000)
    (let ((x (quotient i #x1000))
	  (y (modulo i #x1000)))
      (map integer->char
	   (list (+ #xe0 x)
		 (+ #x80 (quotient y #x40))
		 (+ #x80 (modulo y #x40))))))
   (else FIXME)))
  
(define (custom-utf8 i)
  (if (< i 80)
      (utf8 i)
      (utf8 (+ #xee00 i))))

(define (string->utf8-string string)
  (list->string
   (apply append (map utf8 (map char->integer (string->list string))))))

(define (char->utf8-string char)
  (list->string (utf8 (char->integer char))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; stencil outputters
;;;

;;; catch-all for missing stuff
;;; comment this out to see find out what functions you miss :-)
(define (dummy . foo) #f)
(map (lambda (x) (module-define! this-module x dummy))
     (append
      (ly:all-stencil-expressions)
      (ly:all-output-backend-commands)))

(define (beam width slope thick blot)
  (define cursor '(0 . 0))
  (define (rmoveto def x y)
    (set! cursor (cons (+ x (car cursor)) (+ y (cdr cursor))))
    (moveto def (car cursor) (cdr cursor)))
  (define (rlineto def x y)
    (set! cursor (cons (+ x (car cursor)) (+ y (cdr cursor))))
    (lineto def (car cursor) (cdr cursor)))
  (let* ((def (make <gnome-canvas-path-def>))
	 (bezier (make <gnome-canvas-bpath>
		   #:parent (canvas-root)
		   #:fill-color "black"
		   #:outline-color "black"
		   #:width-units blot
		   #:join-style 'round))
	 (t (- thick blot))
	 (w (- width blot))
	 (h (* w slope)))
    
    (reset def)
    (rmoveto def (/ blot 2) (/ t 2))
    (rlineto def w (- h))
    (rlineto def 0 (- t))
    (rlineto def (- w) h)
    (rlineto def 0 t)
    (closepath def)
    (set-path-def bezier def)
    bezier))

(define (square-beam width slope thick blot)
  (let*
      ((def (make <gnome-canvas-path-def>))
       (y (* (- width) slope))
       (props (make <gnome-canvas-bpath>
		   #:parent (canvas-root)
		   #:fill-color "black"
		   #:outline-color "black"
		   #:width-units 0.0)))
    
    (reset def)
    (moveto def 0 0)
    (lineto def width y)
    (lineto def width (- y thick))
    (lineto def 0 (- thick))
    (lineto def 0 0)
    (closepath def)
    (set-path-def props def)
    props))
    

;; two beziers
(define (bezier-sandwich lst thick)
  (let* ((def (make <gnome-canvas-path-def>))
	 (bezier (make <gnome-canvas-bpath>
		   #:parent (canvas-root)
		   #:fill-color "black"
		   #:outline-color "black"
		   #:width-units thick
		   #:join-style 'round)))

    (reset def)

    ;; FIXME: LST is pre-mangled for direct ps stack usage
    ;; cl cr r l  0 1 2 3 
    ;; cr cl l r  4 5 6 7
    
     (moveto def (car (list-ref lst 3)) (- (cdr (list-ref lst 3))))
     (curveto def (car (list-ref lst 0)) (- (cdr (list-ref lst 0)))
 	     (car (list-ref lst 1)) (- (cdr (list-ref lst 1)))
 	     (car (list-ref lst 2)) (- (cdr (list-ref lst 2))))

     (lineto def (car (list-ref lst 7)) (- (cdr (list-ref lst 7))))
     (curveto def (car (list-ref lst 4)) (- (cdr (list-ref lst 4)))
 	     (car (list-ref lst 5)) (- (cdr (list-ref lst 5)))
 	     (car (list-ref lst 6)) (- (cdr (list-ref lst 6))))
     (lineto def (car (list-ref lst 3)) (- (cdr (list-ref lst 3))))

    (closepath def)
    (set-path-def bezier def)
    bezier))

(define (char font i)
  (text font (utf8 i)))

;; FIXME: naming
(define (filledbox breapth width depth height)
  (make <gnome-canvas-rect>
    #:parent (canvas-root)
    #:x1 (- breapth) #:y1 depth #:x2 width #:y2 (- height)
    #:fill-color "black"
    #:join-style 'miter))

(define (grob-cause grob)
  grob)

;; WTF is this in every backend?
(define (horizontal-line x1 x2 thickness)
  (filledbox (- x1) (- x2 x1) (* .5 thickness) (* .5 thickness)))

(define (placebox x y expr)
  (debugf "item: ~S\n" expr)
  (let ((item expr))
    ;;(if item
    ;; FIXME ugly hack to skip #unspecified ...
    (if (and item (not (eq? item (if #f #f))))
	(begin
	  (move item
		(* output-scale (+ (car system-origin) x))
		(* output-scale (- (car system-origin) y)))
	  (affine-relative item output-scale 0 0 output-scale 0 0)
	  item)
	#f)))

(define (dashed-line thick on off dx dy)
  (draw-line thick 0 0 dx dy)) 

(define (draw-line thick fx fy tx ty)
  (let*
      ((def (make <gnome-canvas-path-def>))
       (props (make <gnome-canvas-bpath>
		   #:parent (canvas-root)
		   #:fill-color "black"
		   #:outline-color "black"
		   #:width-units thick)))
    
    (reset def)
    (moveto def fx (- fy))
    (lineto def tx (- ty))
    (set-path-def props def)
    props))
    

(define (list->offsets accum coords)
  (if (null? coords)
      accum
      (cons (cons (car coords) (cadr coords))
	    (list->offsets accum (cddr coords))
      )))

(define (polygon coords blotdiameter)
  (let*
      ((def (make <gnome-canvas-path-def>))
       (props (make <gnome-canvas-bpath>
		   #:parent (canvas-root)
		   #:fill-color "black"
		   #:outline-color "black"
		   #:width-units blotdiameter))
       (points (list->offsets '() coords))
       (last-point (car (last-pair points))))

    (reset def)
    (moveto def (car last-point) (cdr last-point))
    (for-each (lambda (x)
		(lineto def (car x) (cdr x))
		) points)
    (closepath def)
    (set-path-def props def)
    props))
    

(define (round-filled-box breapth width depth height blot-diameter)
  (let ((r (/ blot-diameter 2)))
    (make <gnome-canvas-rect>
      #:parent (canvas-root)
      #:x1 (- r breapth) #:y1 (- depth r) #:x2 (- width r) #:y2 (- r height)
      #:fill-color "black"
      #:outline-color "black"
      #:width-units blot-diameter
      #:join-style 'round)))

(define (text font string)
  (define (pango-font-name font)
    (let ((name (ly:font-name font)))
      (if name
	  (regexp-substitute/global #f "^GNU-(.*)-[.0-9]*$" name 'pre 1 'post)
	  (begin
	    (stderr "font-name: ~S\n" (ly:font-name font))
	    ;; TODO s/filename/file-name/
	    (stderr "font-filename: ~S\n" (ly:font-filename font))
	    (stderr "pango-font-size: ~S\n" (pango-font-size font))
	    "ecrm12"))))
  
  (define (pango-font-size font)
    (let* ((designsize (ly:font-design-size font))
	   (magnification (* (ly:font-magnification font)))
	   

	   ;;font-name: "GNU-LilyPond-feta-20"
	   ;;font-filename: "feta20"
	   ;;pango-font-name: "lilypond-feta, regular 32"
	   ;;OPS:2.61
	   ;;scaling:29.7046771653543
	   ;;magnification:0.569055118110236
	   ;;design:20.0
  
	   ;; experimental sizing:
	   ;; where does factor come from?
	   ;;
	   ;; 0.435 * (12 / 20) = 0.261
	   ;; 2.8346456692913/ 0.261 = 10.86071137659501915708
	   ;;(ops (* 0.435 (/ 12 20) (* output-scale pixels-per-unit)))
	   ;; for size-points
	   (ops 2.61)
	   
	   (scaling (* ops magnification designsize)))
      (debugf "OPS:~S\n" ops)
      (debugf "scaling:~S\n" scaling)
      (debugf "magnification:~S\n" magnification)
      (debugf "design:~S\n" designsize)
      
      scaling))

  (make <gnome-canvas-text>
    #:parent (canvas-root)

    #:anchor 'west
    #:x 0.0 #:y 0.0
    
    #:font (pango-font-name font)
    
    #:size-points (pango-font-size font)
    ;;#:size ...
    #:size-set #t
    
    ;;apparently no effect :-(
    ;;#:scale 1.0
    ;;#:scale-set #t
    
    #:fill-color "black"
    #:text (if (string? string)
	       (string->utf8-string string)
	       (char->utf8-string (car string)))))

