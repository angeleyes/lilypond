;;;; layout-page-layout.scm -- page breaking and page layout
;;;;
;;;;  source file of the GNU LilyPond music typesetter
;;;;
;;;; (c) 2004--2007 Jan Nieuwenhuizen <janneke@gnu.org>
;;;;	      Han-Wen Nienhuys <hanwen@xs4all.nl>

(define-module (scm layout-page-layout)
  #:use-module (srfi srfi-1)
  #:use-module (oop goops describe)
  #:use-module (oop goops)
  #:use-module (scm paper-system)
  #:use-module (scm page)
  #:use-module (scm layout-page-dump)
  #:use-module (lily)
  #:export (post-process-pages optimal-page-breaks make-page-from-systems
	    page-breaking-wrapper
	    ;; utilities for writing custom page breaking functions
            line-height line-next-space line-next-padding
	    line-minimum-distance line-ideal-distance
	    first-line-position
	    line-ideal-relative-position line-minimum-relative-position
            line-minimum-position-on-page stretchable-line?
	    page-maximum-space-to-fill page-maximum-space-left space-systems))

(define (page-breaking-wrapper paper-book)
  "Compute line and page breaks by calling the page-breaking paper variable,
  then performs the post process function using the page-post-process paper
  variable. Finally, return the pages."
  (let* ((paper (ly:paper-book-paper paper-book))
         (pages ((ly:output-def-lookup paper 'page-breaking) paper-book)))
    ((ly:output-def-lookup paper 'page-post-process) paper pages)
    pages))

(define (post-process-pages layout pages)
  "If the write-page-layout paper variable is true, dumps page breaks
  and tweaks."
  (if (ly:output-def-lookup layout 'write-page-layout #f)
      (write-page-breaks pages)))

;;;
;;; Utilities for computing line distances and positions
;;;
(define (line-height line)
  "Return the system height, that is the length of its vertical extent."
  (interval-length (paper-system-extent line Y)))

(define (line-next-space line next-line layout)
  "Return space to use between `line' and `next-line'.
  `next-line' can be #f, meaning that `line' is the last line."
  (let* ((title (paper-system-title? line))
	 (next-title (and next-line (paper-system-title? next-line))))
    (cond ((and title next-title)
	   (ly:output-def-lookup layout 'between-title-space))
	  (title
	   (ly:output-def-lookup layout 'after-title-space))
	  (next-title
	   (ly:output-def-lookup layout 'before-title-space))
	  (else
	   (ly:prob-property
	    line 'next-space
	    (ly:output-def-lookup layout 'between-system-space))))))

(define (line-next-padding line next-line layout)
  "Return padding to use between `line' and `next-line'.
  `next-line' can be #f, meaning that `line' is the last line."
  (ly:prob-property
   line 'next-padding
   (ly:output-def-lookup layout 'between-system-padding)))


(define (line-minimum-distance line next-line layout ignore-padding)
  "Minimum distance between `line' reference position and `next-line'
 reference position. If next-line is #f, return #f."
  (and next-line
       (max 0 (- (+ (interval-end (paper-system-extent next-line Y))
		    (if ignore-padding 0 (line-next-padding line next-line layout)))
		 (interval-start (paper-system-extent line Y))))))

(define (line-ideal-distance line next-line layout ignore-padding)
  "Ideal distance between `line' reference position and `next-line'
 reference position. If next-line is #f, return #f."
  (and next-line
       (+ (max 0 (- (+ (interval-end (paper-system-staff-extents next-line))
		       (if ignore-padding 0 (line-next-padding line next-line layout)))
		    (interval-start (paper-system-staff-extents line))))
	  (line-next-space line next-line layout))))

(define (first-line-position line layout)
  "Position of the first line on page"
  (max (+ (ly:output-def-lookup layout 'page-top-space)
	  (interval-end (paper-system-staff-extents line)))
       (interval-end (paper-system-extent line Y))))

(define (line-ideal-relative-position line prev-line layout ignore-padding)
  "Return ideal position of `line', relative to `prev-line' position.
  `prev-line' can be #f, meaning that `line' is the first line."
  (if (not prev-line)
      ;; first line on page
      (first-line-position line layout)
      ;; not the first line on page
      (max (line-minimum-distance prev-line line layout ignore-padding)
	   (line-ideal-distance prev-line line layout ignore-padding))))

(define (line-minimum-relative-position line prev-line layout ignore-padding)
  "Return position of `line', relative to `prev-line' position.
  `prev-line' can be #f, meaning that `line' is the first line."
  (if (not prev-line)
      ;; first line on page
      (first-line-position line layout)
      ;; not the first line on page
      (line-minimum-distance prev-line line layout ignore-padding)))

(define (line-minimum-position-on-page line prev-line prev-position page)
  "If `line' fits on `page' after `prev-line', which position on page is
  `prev-position', then return the line's postion on page, otherwise #f.
  `prev-line' can be #f, meaning that `line' is the first line."
  (let* ((layout (ly:paper-book-paper (page-property page 'paper-book)))
         (position (+ (line-minimum-relative-position line prev-line layout #f)
                      (if prev-line prev-position 0.0)))
         (bottom-position (- position
                             (interval-start (paper-system-extent line Y)))))
    (and (or (not prev-line)
             (< bottom-position (page-printable-height page)))
         position)))

(define (stretchable-line? line)
  "Say whether a system can be stretched."
  (not (or (ly:prob-property? line 'is-title)
	   (let ((system-extent (paper-system-staff-extents line)))
	     (= (interval-start system-extent)
		(interval-end	system-extent))))))

(define (page-maximum-space-to-fill page lines paper)
  "Return the space between the first line top position and the last line
  bottom position. This constitutes the maximum space to fill on `page'
  with `lines'."
  (let ((last-line (car (last-pair lines))))
    (- (page-printable-height page)
       (first-line-position (first lines) paper)
       (ly:prob-property last-line
			 'bottom-space 0.0)
       (- (interval-start (paper-system-extent last-line Y))))))

(define (page-maximum-space-left page)
  (let ((paper (ly:paper-book-paper (page-property page 'paper-book))))
    (let bottom-position ((lines (page-property page 'lines))
                          (prev-line #f)
                          (prev-position #f))
      (if (null? lines)
          (page-printable-height page)
          (let* ((line (first lines))
                 (position (line-minimum-position-on-page
                            line prev-line prev-position page)))
            (if (null? (cdr lines))
                (and position
                     (- (page-printable-height page)
                        (- position
                           (interval-start (paper-system-extent line Y)))))
                (bottom-position (cdr lines) line position)))))))

;;;
;;; Utilities for distributing systems on a page
;;;

(define (space-systems space-to-fill lines ragged paper ignore-padding)
  "Compute lines positions on page: return force and line positions as a pair.
 force is #f if lines do not fit on page."
  (let* ((empty-stencil (ly:make-stencil '() '(0 . 0) '(0 . 0)))
	 (empty-prob (ly:make-prob 'paper-system (list `(stencil . ,empty-stencil))))
	 (cdr-lines (append (cdr lines)
			    (if (<= (length lines) 1)
				(list empty-prob)
				'())))
	 (springs (map (lambda (prev-line line)
			 (list (line-ideal-distance prev-line line paper ignore-padding)
			       (/ 1.0 (line-next-space prev-line line paper))))
		       lines
		       cdr-lines))
	 (rods (map (let ((i -1))
		      (lambda (prev-line line)
			(set! i (1+ i))
			(list i (1+ i)
			      (line-minimum-distance prev-line line paper ignore-padding))))
		       lines
		       cdr-lines))
	 (space-result
	  (ly:solve-spring-rod-problem springs rods space-to-fill ragged)))
    (cons (car space-result)
	  (map (let ((topskip (first-line-position (first lines) paper)))
		 (lambda (y)
		   (+ y topskip)))
	       (cdr space-result)))))

(define (make-page-from-systems paper-book lines page-number ragged last)
  "Return a new page, filled with `lines'."
  (let* ((page (make-page paper-book
			  'lines lines
			  'page-number page-number
			  'is-last last))
	 (posns (if (null? lines)
		    (list)
		    (let* ((paper (ly:paper-book-paper paper-book))
			   (space-to-fill (page-maximum-space-to-fill
					    page lines paper))
			   (spacing (space-systems space-to-fill lines ragged paper #f)))
		      (if (or (not (car spacing)) (inf? (car spacing)))
			  (cdr (space-systems space-to-fill lines ragged paper #t))
			  (cdr spacing))))))
    (page-set-property! page 'configuration posns)
    page))

;;;
;;; Page breaking function
;;;

;; Optimal distribution of
;; lines over pages; line breaks are a given.

;; TODO:
;;
;; - density scoring
;; - separate function for word-wrap style breaking?
;; - ragged-bottom? ragged-last-bottom?

(define (get-path node done)
  "Follow NODE.PREV, and return as an ascending list of pages. DONE
is what have collected so far, and has ascending page numbers."
  (if (page? node)
      (get-path (page-prev node) (cons node done))
      done))

(define (combine-penalties force user best-paths
			   inter-system-space force-equalization-factor)
  (let* ((prev-force (if (null? best-paths)
			 0.0
			 (page-force (car best-paths))))
	 (prev-penalty (if (null? best-paths)
			   0.0
			   (page-penalty (car best-paths))))
	 (relative-force (/ force inter-system-space))
	 (abs-relative-force (abs relative-force)))
    (+ (* abs-relative-force (+ abs-relative-force 1))
       prev-penalty
       (* force-equalization-factor (/ (abs (- prev-force force))
				       inter-system-space))
       user)))

(define (walk-paths done-lines best-paths current-lines last current-best
		    paper-book page-alist)
  "Return the best optimal-page-break-node that contains
CURRENT-LINES. DONE-LINES.reversed ++ CURRENT-LINES is a consecutive
ascending range of lines, and BEST-PATHS contains the optimal breaks
corresponding to DONE-LINES.

CURRENT-BEST is the best result sofar, or #f."
  (let* ((paper (ly:paper-book-paper paper-book))
	 (this-page (make-page
		     paper-book
		     'is-last last
		     'page-number (if (null? best-paths)
				      (ly:output-def-lookup paper 'first-page-number)
				      (1+ (page-page-number (first best-paths))))))
	 (ragged-all (eq? #t (ly:output-def-lookup paper 'ragged-bottom)))
	 (ragged-last (eq? #t (ly:output-def-lookup paper 'ragged-last-bottom)))
	 (ragged (or ragged-all (and ragged-last last)))
	 (space-to-fill (page-maximum-space-to-fill this-page current-lines paper))
	 (vertical-spacing (space-systems space-to-fill current-lines ragged paper #f))
	 (satisfied-constraints (car vertical-spacing))
	 (force (if satisfied-constraints
		    (if (and last ragged-last)
			0.0
			satisfied-constraints)
		    10000))
	 (positions (cdr vertical-spacing))
	 (get-break-penalty (lambda (sys)
			      (ly:prob-property sys 'penalty 0.0)))
	 (user-nobreak-penalties (- (apply + (filter negative?
						     (map get-break-penalty
							  (cdr current-lines))))))
	 (user-penalty (+ (max (get-break-penalty (car current-lines)) 0.0)
			  user-nobreak-penalties))
	 (total-penalty (combine-penalties
			 force user-penalty best-paths
			 (ly:output-def-lookup paper 'between-system-space)
			 (ly:output-def-lookup paper 'verticalequalizationfactor 0.3)))
	 (new-best (if (or (not current-best)
			   (and satisfied-constraints
				(< total-penalty (page-penalty current-best))))
		       (begin
			 (map (lambda (x)
				(page-set-property! this-page
						    (car x)
						    (cdr x)))
			      (list (cons 'prev (if (null? best-paths)
						    #f
						    (car best-paths)))
				    (cons 'lines current-lines)
				    (cons 'force force)
				    (cons 'configuration positions)
				    (cons 'penalty total-penalty)))
			 this-page)
		       current-best)))
    (if #f ;; debug
	(display
	 (list
	  "\nuser pen " user-penalty
	  "\nsatisfied-constraints" satisfied-constraints
	  "\nlast? " last "ragged?" ragged
	  "\nis-better " is-better " total-penalty " total-penalty "\n"
	  "\nconfig " positions
	  "\nforce " force
	  "\nlines: " current-lines "\n")))
    (if #f ; debug
	(display (list "\nnew-best is " (page-lines new-best)
		       "\ncontinuation of "
		       (if (null? best-paths)
			   "start"
			   (page-lines (car best-paths))))))
    (if (and (pair? done-lines)
	     ;; if this page is too full, adding another line won't help
	     satisfied-constraints)
	(walk-paths (cdr done-lines) (cdr best-paths)
		    (cons (car done-lines) current-lines)
		    last new-best
		    paper-book page-alist)
	new-best)))

(define (walk-lines done best-paths todo paper-book page-alist)
  "Return the best page breaking as a single
page node for optimally breaking TODO ++
DONE.reversed. BEST-PATHS is a list of break nodes corresponding to
DONE."
  (if (null? todo)
      (car best-paths)
      (let* ((this-line (car todo))
	     (last (null? (cdr todo)))
	     (next (walk-paths done best-paths (list this-line) last #f
			       paper-book page-alist)))
	(walk-lines (cons this-line done)
		    (cons next best-paths)
		    (cdr todo)
		    paper-book
		    page-alist))))

(define-public (optimal-page-breaks paper-book)
  "Return pages as a list starting with 1st page. Each page is a 'page Prob."
  (let* ((paper (ly:paper-book-paper paper-book))
	 (lines (ly:paper-book-systems paper-book))
	 (page-alist (layout->page-init paper)) 
	 (force-equalization-factor (ly:output-def-lookup
				     paper 'verticalequalizationfactor 0.3)))
    (ly:message (_ "Calculating page breaks..."))
    (let* ((best-break-node (walk-lines '() '() lines paper-book page-alist))
	   (break-nodes (get-path best-break-node '())))
      (page-set-property! (car (last-pair break-nodes)) 'is-last #t)
      (if #f; (ly:get-option 'verbose)
	  (begin
	    (display (list
		      "\nbreaks: " (map (lambda (node)
					  (ly:prob-property (car (page-lines node))
							    'number))
					break-nodes)
		      "\nsystems " (map page-lines break-nodes)
		      "\npenalties " (map page-penalty break-nodes)
		      "\nconfigs " (map page-configuration break-nodes)))))
      ;; construct page stencils.
      (for-each page-stencil break-nodes)
      break-nodes)))
