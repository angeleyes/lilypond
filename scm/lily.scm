;;;; lily.scm -- toplevel Scheme stuff
;;;;
;;;;  source file of the GNU LilyPond music typesetter
;;;; 
;;;; (c) 1998--2005 Jan Nieuwenhuizen <janneke@gnu.org>
;;;; Han-Wen Nienhuys <hanwen@cs.uu.nl>


(for-each (lambda (x)
	    (ly:add-option (car x) (cadr x) (caddr x)))
	  
	  '((point-and-click #t "use point & click")
	    (midi-debug #f "generate human readable MIDI")
	    (internal-type-checking #f "check every property assignment for types")
	    (parse-protect #t    "continue when finding errors in inline
scheme are caught in the parser. If off, halt 
on errors, and print a stack trace.")
	    (old-relative #f
			  "relative for simultaneous music works
similar to chord syntax")

	    (resolution 101 "resolution for generating bitmaps")
	    (anti-alias-factor 1 "render at higher resolution and scale down result\nto prevent jaggies in PNG")
	    (preview-include-book-title #t "include book-titles in preview images.")
	    (gs-font-load #f
			  "load fonts via Ghostscript.")
	    (gui #f "running from gui; redirect stderr to log file")
	    (delete-intermediate-files #f
				       "delete unusable PostScript files")
	    (verbose #f "value for the --verbose flag")
	    (ttf-verbosity 0
			   "how much verbosity for TTF font embedding?")
	    (debug-gc #f
		      "dump GC protection info")))


(if (defined? 'set-debug-cell-accesses!)
    (set-debug-cell-accesses! #f))

;(set-debug-cell-accesses! 1000)

(use-modules (ice-9 regex)
	     (ice-9 safe)
             (ice-9 optargs)
	     (oop goops)
	     (srfi srfi-1)
	     (srfi srfi-13)
	     (srfi srfi-14))


;; my display
(define-public (myd k v) (display k) (display ": ") (display v) (display ", "))

(define-public (print . args)
  (apply format (cons (current-output-port) args)))


;;; General settings
;;; debugging evaluator is slower.  This should
;;; have a more sensible default.

(if (ly:get-option 'verbose)
    (begin
      (debug-enable 'debug)
      (debug-enable 'backtrace)
      (read-enable 'positions)))


(define-public tex-backend?
  (member (ly:output-backend) '("texstr" "tex")))

(define-public parser #f)

(define-public (lilypond-version)
  (string-join
   (map (lambda (x) (if (symbol? x)
			(symbol->string x)
			(number->string x)))
	(ly:version))
   "."))



;; cpp hack to get useful error message
(define ifdef "First run this through cpp.")
(define ifndef "First run this through cpp.")

;; gettext wrapper for guile < 1.7.2
(if (defined? 'gettext)
    (define-public _ gettext)
    (define-public _ ly:gettext))

(define-public (ly:load x)
  (let* ((file-name (%search-load-path x)))
    (if (ly:get-option 'verbose)
	(ly:progress "[~A" file-name))
    (primitive-load file-name)
    (if (ly:get-option 'verbose)
	(ly:progress "]"))))

(define-public TEX_STRING_HASHLIMIT 10000000)

;; Cygwin
;; #(CYGWIN_NT-5.1 Hostname 1.5.12(0.116/4/2) 2004-11-10 08:34 i686)
;;
;; Debian
;; #(Linux hostname 2.4.27-1-686 #1 Fri Sep 3 06:28:00 UTC 2004 i686)
;;
;; Mingw
;; #(Windows XP HOSTNAME build 2600 5.01 Service Pack 1 i686)
;;

;; ugh, code dup.
(define-public PLATFORM
  (string->symbol
   (string-downcase
    (car (string-tokenize (vector-ref (uname) 0) char-set:letter)))))

(define-public DOS
  (let ((platform (string-tokenize
		   (vector-ref (uname) 0) char-set:letter+digit)))
    (if (null? (cdr platform)) #f
	(member (string-downcase (cadr platform)) '("95" "98" "me")))))

(case PLATFORM
  ((windows)
   (define native-getcwd getcwd)
   (define (slashify x)
     (if (string-index x #\\)
	 x
	 (string-regexp-substitute
	  "//*" "/"
	  (string-regexp-substitute "\\\\" "/" x))))
   ;; FIXME: this prints a warning.
  (define-public (ly-getcwd)
     (slashify (native-getcwd))))
  (else (define-public ly-getcwd getcwd)))

(define-public (is-absolute? file-name)
  (let ((file-name-length (string-length file-name)))
    (if (= file-name-length 0)
	#f
	(or (eq? (string-ref file-name 0) #\/)
	    (and (eq? PLATFORM 'windows)
		 (> file-name-length 2)
		 (eq? (string-ref file-name 1) #\:)
		 (eq? (string-ref file-name 2) #\/))))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (type-check-list location signature arguments)
  "Typecheck a list of arguments against a list of type
predicates. Print a message at LOCATION if any predicate failed."
  (define (recursion-helper signature arguments count) 
    (define (helper pred? arg count) 
      (if (not (pred? arg))

	  (begin
	    (ly:input-message
	     location
	     (format
	      #f (_ "wrong type for argument ~a.  Expecting ~a, found ~s")
	      count (type-name pred?) arg))
	    #f)
	  #t))

    (if (null? signature)
	#t
	(and (helper (car signature) (car arguments) count)
	     (recursion-helper (cdr signature) (cdr arguments) (1+ count)))))
  (recursion-helper signature arguments 1))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  output


;;(define-public (output-framework) (write "hello\n"))

(define output-tex-module
  (make-module 1021 (list (resolve-interface '(scm output-tex)))))
(define output-ps-module
  (make-module 1021 (list (resolve-interface '(scm output-ps)))))

(define-public (ps-output-expression expr port)
  (display (eval expr output-ps-module) port))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Safe definitions utility
(define safe-objects (list))

(define-macro (define-safe-public arglist . body)
  "Define a variable, export it, and mark it as safe, ie usable in LilyPond safe mode.
The syntax is the same as `define*-public'."
  (define (get-symbol arg)
    (if (pair? arg)
        (get-symbol (car arg))
        arg))
  (let ((safe-symbol (get-symbol arglist)))
    `(begin
       (define*-public ,arglist
         ,@body)
       (set! safe-objects (cons (cons ',safe-symbol ,safe-symbol)
                                safe-objects))
       ,safe-symbol)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; other files.

(for-each ly:load
	  ;; load-from-path
	  '("lily-library.scm"
	    "file-cache.scm"
	    "define-music-types.scm"
	    "output-lib.scm"
	    "c++.scm"
	    "chord-ignatzek-names.scm"
	    "chord-entry.scm"
	    "chord-generic-names.scm"
	    "stencil.scm"
	    "markup.scm"
	    "bass-figure.scm"
	    "music-functions.scm"
	    "part-combiner.scm"
	    "define-music-properties.scm"
	    "auto-beam.scm"
	    "chord-name.scm"

	    "ly-from-scheme.scm"
	    
	    "define-context-properties.scm"
	    "translation-functions.scm"
	    "script.scm"
	    "midi.scm"
	    "beam.scm"
	    "clef.scm"
	    "slur.scm"
	    "font.scm"
	    "encoding.scm"
	    
	    "fret-diagrams.scm"
	    "define-markup-commands.scm"
	    "define-grob-properties.scm"
	    "define-grobs.scm"
	    "define-grob-interfaces.scm"
	    "define-stencil-commands.scm"
	    "page-layout.scm"
	    "titling.scm"
	    
	    "paper.scm"
	    "backend-library.scm"
	    "x11-color.scm"

	    ;; must be after everything has been defined
	    "safe-lily.scm"))


(set! type-p-name-alist
      `(
	(,boolean-or-symbol? . "boolean or symbol")
	(,boolean? . "boolean")
	(,char? . "char")
	(,grob-list? . "list of grobs")
	(,hash-table? . "hash table")
	(,input-port? . "input port")
	(,integer? . "integer")
	(,list? . "list")
	(,ly:context? . "context")
	(,ly:dimension? . "dimension, in staff space")
	(,ly:dir? . "direction")
	(,ly:duration? . "duration")
	(,ly:grob? . "layout object")
	(,ly:input-location? . "input location")
	(,ly:moment? . "moment")
	(,ly:music? . "music")
	(,ly:pitch? . "pitch")
	(,ly:translator? . "translator")
	(,ly:font-metric? . "font metric")
	(,markup-list? . "list of markups")
	(,markup? . "markup")
	(,ly:music-list? . "list of music")
	(,number-or-grob? . "number or grob")
	(,number-or-string? . "number or string")
	(,number-pair? . "pair of numbers")
	(,number? . "number")
	(,output-port? . "output port")   
	(,pair? . "pair")
	(,procedure? . "procedure") 
	(,scheme? . "any type")
	(,string? . "string")
	(,symbol? . "symbol")
	(,vector? . "vector")))


;; debug mem leaks

(define gc-protect-stat-count 0)
(define-public (dump-gc-protects)
  (set! gc-protect-stat-count (1+ gc-protect-stat-count))
  (let* ((protects (sort
		    (hash-table->alist (ly:protects))
		    (lambda (a b)
		      (< (object-address (car a))
			 (object-address (car b))))))

	 (out-file-name (string-append
			 "gcstat-" (number->string gc-protect-stat-count)
			 ".scm"))
	 (outfile    (open-file  out-file-name  "w")))

    (display (format "Dumping gc protected objs to ~a...\n" out-file-name))
    (display
     (filter
      (lambda (x) (not (symbol? x))) 
      (map (lambda (y)
	     (let ((x (car y))
		   (c (cdr y)))

	       (string-append
		(string-join
		 (map object->string (list (object-address x) c x))
		 " ")
		"\n")))
	   protects))
     outfile)

    (if (defined? 'gc-live-object-stats)
	(let*
	    ((dummy (gc))
	     (dummy2 (gc))
	     (stats (gc-live-object-stats))
	     )

	  (for-each
	   (lambda (x)
	     (format outfile "~a: ~a\n" (car x) (cdr x)))
	   (sort (gc-live-object-stats)
		 (lambda (x y)
		   (string<? (car x) (car y)))))
	   ))

    ))

(define-public (tweak-grob-property grob sym val)
  (set! (ly:grob-property grob sym) val))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define-public (lilypond-main files)
  "Entry point for LilyPond."

  (if (null? files)
      (no-files-handler))

  (let ((failed (lilypond-all files)))
    (if (pair? failed)
	(begin
	  (ly:error (_ "failed files: ~S") (string-join failed))
	  (exit 1))
	(begin
	  ;; HACK: be sure to exit with single newline
	  (ly:message "")
	  (exit 0)))))

(define (no-files-handler)
  (ly:usage)
  (exit 2))

(define-public (lilypond-all files)
  (let* ((failed '())
	 (handler (lambda (key failed-file)
		    (set! failed (append (list failed-file) failed)))))
    ;;(handler (lambda (key . arg) (set! failed (append arg failed)))))
    (for-each (lambda (x) (lilypond-file handler x)) files)
    failed))

(define (lilypond-file handler file-name)
  (catch 'ly-file-failed
	 (lambda () (ly:parse-file file-name))
	 (lambda (x . args) (handler x file-name)))

  (if (ly:get-option 'debug-gc)
      (dump-gc-protects)))

(use-modules (scm editor))

(define-public (running-from-gui?)
  (let ((have-tty? (isatty? (current-input-port))))
    ;; If no TTY and not using safe, assume running from GUI.
    (cond
     ((eq? PLATFORM 'windows)
      (if DOS #f
      ;; This only works for i586-mingw32msvc-gcc -mwindows
      (not (string-match "standard input"
			 (format #f "~S" (current-input-port))))))
     ;; FIXME: using -dgui would be nice, but it does not work
     ((eq? PLATFORM 'foo-windows)
      (ly:get-option 'gui))
     ((eq? PLATFORM 'darwin) #f)
     (else
      (not have-tty?)))))

(define-public (gui-main files)
  (if (null? files)
      (gui-no-files-handler))
  (let* ((base (basename (car files) ".ly"))
	 (log-name (string-append base ".log")))
    (if (not (running-from-gui?))
	(ly:message (_ "Redirecting output to ~a...") log-name))
    (ly:stderr-redirect log-name "w")
    (ly:message "# -*-compilation-*-")
    (let ((failed (lilypond-all files)))
      (if (pair? failed)
	  (begin
	    ;; ugh
	    (ly:stderr-redirect "foo" "r")
	    (system (get-editor-command log-name 0 0 0))
	    (ly:error (_ "failed files: ~S") (string-join failed))
	    ;; not reached?
	    (exit 1))
	  (exit 0)))))

(define (gui-no-files-handler)
  (let* ((ly (string-append (ly:effective-prefix) "/ly/"))
	 ;; FIXME: soft-code, localize
	 (welcome-ly (string-append ly "Welcome_to_LilyPond.ly"))
	 (cmd (get-editor-command welcome-ly 0 0 0)))
    (ly:message (_ "Invoking `~a'...") cmd)
    (system cmd)
    (exit 1)))

(or (not (running-from-gui?))
    (ly:get-option 'safe)
    (define lilypond-main gui-main))
