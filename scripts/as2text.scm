#!/usr/bin/guile \
-e main -s
!#
;;;; as2text.scm -- Translate AsciiScript to Text
;;;;
;;;; source file of the GNU LilyPond music typesetter
;;;; 
;;;; (c) 2000 Jan Nieuwenhuizen <janneke@gnu.org>

;;;; library funtions
(use-modules
   (ice-9 debug)
  (ice-9 getopt-long)
  (ice-9 string-fun)
  (ice-9 regex))


;;; Script stuff
(define program-name "as2text")

(define subst-version "@TOPLEVEL_VERSION@")

(define program-version 	
  (if (eq? subst-version (string-append "@" "TOPLEVEL_VERSION" "@"))
      "unknown"
      subst-version))

(define (show-version)
  (display (string-append 
	    program-name " - LilyPond version " program-version "\n")
	   (current-error-port)))

(define (show-help)
  (display "Convert AsciiScript to text.

Usage: as2text [OPTION]... AS-FILE

Options:
  -h,--help          this help
  -o,--output=FILE   set output file
  -v,--version       show version
" (current-error-port)))

(define (gulp-file name)
  (let ((port (catch 'system-error (lambda () (open-file name "r"))
		     (lambda args #f))))
       (if port 
	   (let ((content (let loop ((text ""))
			       (let ((line (read-line port)))
				    (if (or (eof-object? line)
					    (not line)) 
					text
					(loop (string-append text line "\n")))))))
		(close port)
		content)
	   (begin
	    (display 
	     (string-append "warning: no such file: " name "\n")
	     (current-error-port))
	    ""))))

(define (with-exention name ext)
  (if (equal? ext (substring name (max 0 (- (string-length name) 
					    (string-length ext)))))
      name
      (string-append name ext)))

(define (do-file file-name output-name)
  (let ((output-file (current-output-port))
	(ascii-script (gulp-file (with-exention file-name ".as"))))
       (eval-string ascii-script)))

;;; Script entry point
(define (main args)
  (show-version)
  (let ((options (getopt-long args
			      `((output (single-char #\o)
                                          (value #t))
			      	(help (single-char #\h))
			      	(version (single-char #\v))))))
    (define (opt tag default)
      (let ((pair (assq tag options)))
        (if pair (cdr pair) default)))

    (if (assq 'help options)
	(begin (show-help) (exit 0)))
	    
    (if (assq 'version options)
	(exit 0))

    (let ((output-name (opt 'output-name "-"))
	   (files (let ((foo (opt '() '())))
		       (if (null? foo) 
			   (list "-")
			   foo))))
	 (do-file (car files) output-name))))

;;;;
;;;; Ascii Script plotting
;;;;

;;; Global variables

;; Ascii-Art signature
(define tag-line "lily")

(define first-line #t)

;; cursor
(define cur-x 0)
(define cur-y 0)

;; canvas dimensions
(define canvas-width 65)
(define canvas-height 20)

;; font database
(define fonts '())

;; current font
(define cur-font "")

;; 
(define line-char "-")

;; the plotting fields
(define canvas 0)
;; urg: 
;; make-uniform array of characters,
;; or 1-dim array of strings?
;; (set! canvas (make-array " " canvas-height canvas-width)))

;; urg, this kind of naming costs too much indenting
(define (split c s r)
  (separate-fields-discarding-char c s r))

(define (strip s)
  (sans-surrounding-whitespace s))


;;; Helper functions

(define (af-gulp-file name)
  (set! %load-path 
	(cons (string-append 
	       (getenv 'LILYPONDPREFIX) "/mf") %load-path))
  (let ((path (%search-load-path name)))
       (if path
	   (gulp-file path)
	   (gulp-file name))))

(define (char-width c)
  (let ((bbox (car c)))
       (inexact->exact (* .001 (caddr bbox)))))

;; urg: use smart table
(define (xmerge-chars old new)
  (cond ((equal? new " ") old)
	((and (equal? old "|") (equal? new "-")) "+")
	((and (equal? old "-") (equal? new "|")) "+")
	(else new)))

(define (merge-chars old new)
  (cond ((equal? new " ") old)
	(else new)))

(define (plot x y c)
  (let ((ny (- (* -1 y) 1)))
       (if (array-in-bounds? canvas ny x)
	   (array-set! canvas (merge-chars (array-ref canvas ny x) c) ny x)
	   (display (string-append "ouch: " (number->string x)  ","
				   (number->string ny) "\n")))))

(define (plot-char c)
  (let ((bbox (car c))
	(glyph (cadr c))) 
       ;; BBox: (llx lly urx ury) * 1000
       (let ((dx (inexact->exact (* .001 (car bbox))))
	     ;(dy (inexact->exact (* .001 (cadr bbox))))
	     (dy (inexact->exact (- (* .001 (cadddr bbox)) 1)))
	     (len (length glyph)))
	    ;;(display "Bbox: ") (display bbox) (newline)
	    ;;(display "dy: ") (display dy) (newline)
	    (do ((line glyph (cdr line))
		 (i 0 (+ i 1)))
		((= i len))
		(plot-string (+ cur-x dx) (+ (- cur-y i) dy) (car line))))))

(define (plot-string x y s)
  (do ((i 0 (+ i 1)))
      ((= i (string-length s)))
      (plot (+ x i) y (substring s i (+ i 1)))))

(define (show-char char)
  (display char))

(define (show-font name)
	(let ((font (assoc name fonts)))
	     (map (lambda (x) (show-char x)) font)))

(define (generate-default-font)
  (let loop ((chars '()) (i 32))
       (if (= 127 i) 
	   chars 
	   (loop 
	    (cons (list i '(0 0 1000 1000) 
			(list (make-string 1 (integer->char i)))) 
		  chars) 
	    (+ i 1)))))

(define (get-font name)
  (let ((entry (assoc name fonts)))
       (if entry
	   (cdr entry)
	   (begin
	    (display 
	     (string-append "warning: no such font: " name "\n")
	     (current-error-port))
	    (get-font "default")))))

(define (get-char font n)
  (let ((entry (assoc n font)))
       (if entry
	   (cdr entry)
	   (begin
	    (display 
	     (string-append "warning: no such char: (" 
			    cur-font
			    ", "
			    (number->string n ) ")\n")
	     (current-error-port))
	    '()))))


;;; AsciiScript commands

(define (char n)
  (let* ((font (get-font cur-font))
	 (c (get-char font n)))
	(if c
	    (plot-char c))))

(define (end-output) 
  (display (string-append 
	    (make-string (- canvas-width (string-length tag-line)) #\space)
	    tag-line "\n")))

(define (sign x)
  (if (= x 0)
      1
      (inexact->exact (/ x (abs x)))))

(define (h-line len)
  (let ((step (sign len)))
       (do ((i 0 (+ i step)))
	   ((= i len))
	   (plot (+ cur-x i) cur-y line-char))))

(define (v-line len)
  (let ((step (sign len)))
       (do ((i 0 (+ i step)))
	   ((= i len)) (plot cur-x (+ cur-y i) line-char))))

(define (header x y)
  (display (string-append x y "\n") (current-error-port)))

(define (header-end) "")

(define (rline-to dx dy)
  (plot (inexact->exact cur-x) (inexact->exact cur-y) line-char)
  (plot (inexact->exact (+ cur-x dx)) (inexact->exact (+ cur-y dy)) line-char)
  (if (or (> (abs dx) 1) (> (abs dy) 1))
      (let ((x cur-x)
	    (y cur-y)
	    (hx (/ dx 2))
	    (hy (/ dy 2))
	    )
	   (plot (inexact->exact (+ cur-x hx)) (inexact->exact (+ cur-y hy)) line-char)
	   (rline-to hx hy)
	   (move-to x y)
	   (rmove-to hx hy)
	   (rline-to hx hy)
	   )))

(define (dissect-char text)
  (let* ((char (split #\nl text list))
	 (id (car char))
	 (code (string->number 
		(strip 
		 (substring id 
			    (+ (string-index id #\C) 1)
			    (string-index id #\;)))))
	 (bbox (map string->number 
		    (split #\space (strip (substring
					   id 
					   (+ (string-rindex id #\B) 1)
					   (string-rindex id #\;)))
			   list))))
	(list (list code bbox (cdr char)))))

(define (load-font name mag)
  (let ((text (af-gulp-file (string-append name ".af"))))
       (if (< 0 (string-length text))
	   (let* ((char-list (cdr (split #\np 
					 (regexp-substitute/global 
					  #f "\t[^\n]*\n" text 'pre "" 'post) 
					 list)))
		  (font (apply append (map dissect-char char-list))))
		 (set! fonts (cons (cons name font) fonts))))))

(define (move-to x y)
  (set! cur-x x)
  (set! cur-y y))

(define (put c)
  (plot cur-x cur-y c))

(define (rmove-to dx dy)
  (set! cur-x (+ cur-x dx))
  (set! cur-y (+ cur-y dy)))

(define (select-font name)
  (set! cur-font name))

(define (set-line-char c)
  (set! line-char c))

(define (start-line height)
  (if first-line 
      (begin
       (set! first-line #f)
       (set! fonts (cons (cons "default" (generate-default-font)) fonts))))
  (if (defined? 'mudelapaperlinewidth)
      (set! canvas-width 
	    (inexact->exact (string->number mudelapaperlinewidth))))
  (set! canvas-height height)
  (set! canvas (make-array " " canvas-height canvas-width)))

(define (stop-line)
  (display 
   (apply string-append 
	  (map (lambda (x) (string-append (apply string-append x) "\n")) 
	       (array->list canvas)))))

(define (text s)
  (let ((n (string-length s))
	(font (get-font cur-font)))
       (do ((i 0 (+ i 1)))
	   ((= i n)) 
	    (let* ((n (char->integer (string-ref s i)))
		   (c (get-char font n)))
		  (plot-char c)
		  (rmove-to (char-width c) 0)))))

