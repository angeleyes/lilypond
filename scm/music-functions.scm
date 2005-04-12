;;;; music-functions.scm --
;;;;
;;;;  source file of the GNU LilyPond music typesetter
;;;; 
;;;; (c) 1998--2005 Jan Nieuwenhuizen <janneke@gnu.org>
;;;;                 Han-Wen Nienhuys <hanwen@cs.uu.nl>

;; (use-modules (ice-9 optargs)) 

;;; ly:music-property with setter
;;; (ly:music-property my-music 'elements)
;;;   ==> the 'elements property
;;; (set! (ly:music-property my-music 'elements) value)
;;;   ==> set the 'elements property and return it
(define-public ly:music-property
  (make-procedure-with-setter ly:music-property
			      ly:music-set-property!))

(define-public ly:grob-property
  (make-procedure-with-setter ly:grob-property
			      ly:grob-set-property!))

(define-public (music-map function music)
  "Apply @var{function} to @var{music} and all of the music it contains.

First it recurses over the children, then the function is applied to MUSIC.
"
  (let ((es (ly:music-property music 'elements))
	(e (ly:music-property music 'element)))
    (set! (ly:music-property music 'elements) 
	  (map (lambda (y) (music-map function y)) es))
    (if (ly:music? e)
	(set! (ly:music-property music 'element)
	      (music-map function  e)))
    (function music)))

(define-public (music-filter pred? music)
  "Filter out music expressions that do not satisfy PRED."
  
  (define (inner-music-filter pred? music)
    "Recursive function."
    (let* ((es (ly:music-property music 'elements))
	   (e (ly:music-property music 'element))
	   (as (ly:music-property music 'articulations))
	   (filtered-as (filter ly:music? (map (lambda (y) (inner-music-filter pred? y)) as)))
	   (filtered-e (if (ly:music? e)
			   (inner-music-filter pred? e)
			   e))
	   (filtered-es (filter ly:music? (map (lambda (y) (inner-music-filter pred? y)) es))))
      (set! (ly:music-property music 'element) filtered-e)
      (set! (ly:music-property music 'elements) filtered-es)
      (set! (ly:music-property music 'articulations) filtered-as)
      ;; if filtering emptied the expression, we remove it completely.
      (if (or (not (pred? music))
	      (and (eq? filtered-es '()) (not (ly:music? e))
		   (or (not (eq? es '()))
		       (ly:music? e))))
	  (set! music '()))
      music))

  (set! music (inner-music-filter pred? music))
  (if (ly:music? music)
      music
      (make-music 'Music)))	  ;must return music.

(define-public (display-music music)
  "Display music, not done with music-map for clarity of presentation."
  (display music)
  (display ": { ")  
  (let ((es (ly:music-property music 'elements))
	(e (ly:music-property music 'element)))
    (display (ly:music-mutable-properties music))
    (if (pair? es)
	(begin (display "\nElements: {\n")
	       (map display-music es)
	       (display "}\n")))
    (if (ly:music? e)
	(begin
	  (display "\nChild:")
	  (display-music e))))
  (display " }\n")
  music)

;;;
;;; A scheme music pretty printer
;;;
(define (markup-expression->make-markup markup-expression)
  "Transform `markup-expression' into an equivalent, hopefuly readable, scheme expression.
For instance, 
  \\markup \\bold \\italic hello
==>
  (markup #:line (#:bold (#:italic (#:simple \"hello\"))))"
  (define (proc->command-keyword proc)
    "Return a keyword, eg. `#:bold', from the `proc' function, eg. #<procedure bold-markup (layout props arg)>"
    (let ((cmd-markup (symbol->string (procedure-name proc))))
      (symbol->keyword (string->symbol (substring cmd-markup 0 (- (string-length cmd-markup)
								  (string-length "-markup")))))))
  (define (transform-arg arg)
    (cond ((and (pair? arg) (pair? (car arg))) ;; a markup list
	   (apply append (map inner-markup->make-markup arg)))
	  ((pair? arg)			       ;; a markup
	   (inner-markup->make-markup arg))
	  (else				       ;; scheme arg
	   arg)))
  (define (inner-markup->make-markup mrkup)
    (let ((cmd (proc->command-keyword (car mrkup)))
	  (args (map transform-arg (cdr mrkup))))
      `(,cmd ,@args)))
  ;; body:
  (if (string? markup-expression)
      markup-expression
      `(markup ,@(inner-markup->make-markup markup-expression))))

(define-public (music->make-music obj)
  "Generate a expression that, once evaluated, may return an object equivalent to `obj',
that is, for a music expression, a (make-music ...) form."
  (cond (;; markup expression
	 (markup? obj)
	 (markup-expression->make-markup obj))
	(;; music expression
	 (ly:music? obj)
	 `(make-music 
	   ',(ly:music-property obj 'name)
	   ,@(append (map (lambda (prop)
			    (list
			     (car prop)
			     (if (and (not (markup? (cdr prop)))
				      (list? (cdr prop))
				      (pair? (cdr prop))) ;; property is a non-empty list
				 `(list ,@(map music->make-music (cdr prop)))
				 (music->make-music (cdr prop)))))
			  (remove (lambda (prop)
				    (eqv? (car prop) 'origin))
				  (ly:music-mutable-properties obj))))))
	(;; moment
	 (ly:moment? obj)
	 `(ly:make-moment ,(ly:moment-main-numerator obj)
			  ,(ly:moment-main-denominator obj)
			  ,(ly:moment-grace-numerator obj)
			  ,(ly:moment-grace-denominator obj)))
	(;; note duration
	 (ly:duration? obj)
	 `(ly:make-duration ,(ly:duration-log obj)
			    ,(ly:duration-dot-count obj)
			    ,(car (ly:duration-factor obj))
			    ,(cdr (ly:duration-factor obj))))
	(;; note pitch
	 (ly:pitch? obj)
	 `(ly:make-pitch ,(ly:pitch-octave obj)
			 ,(ly:pitch-notename obj)
			 ,(ly:pitch-alteration obj)))
	(;; scheme procedure
	 (procedure? obj)
	 (or (procedure-name obj) obj))
	(;; a symbol (avoid having an unquoted symbol)
	 (symbol? obj)
	 `',obj)
	(;; an empty list (avoid having an unquoted empty list)
	 (null? obj)
	 `'())
	(else
	 obj)))

(use-modules (ice-9 pretty-print))
(define*-public (display-scheme-music obj #:optional (port (current-output-port)))
  "Displays `obj', typically a music expression, in a friendly fashion,
which often can be read back in order to generate an equivalent expression.

Returns `obj'.
"
  (pretty-print (music->make-music obj) port)
  (newline)
  obj)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (shift-one-duration-log music shift dot)
  "  add SHIFT to ly:duration-log and optionally 
  a dot to any note encountered. This scales the music up by a factor 
  2^shift * (2 - (1/2)^dot)"
  (let ((d (ly:music-property music 'duration)))
    (if (ly:duration? d)
	(let* ((cp (ly:duration-factor d))
	       (nd (ly:make-duration (+ shift (ly:duration-log d))
				     (+ dot (ly:duration-dot-count d))
				     (car cp)
				     (cdr cp))))
	  (set! (ly:music-property music 'duration) nd)))
    music))



(define-public (shift-duration-log music shift dot)
  (music-map (lambda (x) (shift-one-duration-log x shift dot))
	     music))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; clusters.

(define-public (note-to-cluster music)
  "Replace NoteEvents by ClusterNoteEvents."
  (if (eq? (ly:music-property music 'name) 'NoteEvent)
      (make-music 'ClusterNoteEvent
		  'pitch (ly:music-property music 'pitch)
		  'duration (ly:music-property music 'duration))
      music))

(define-public (notes-to-clusters music)
  (music-map note-to-cluster music))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; repeats.

(define-public (unfold-repeats music)
  "
This function replaces all repeats  with unfold repeats. "
  
  (let ((es (ly:music-property music 'elements))
	(e  (ly:music-property music 'element))
	)
    (if (memq 'repeated-music (ly:music-property music 'types))
	(begin
	  (if (equal? (ly:music-property music 'iterator-ctor)
		      Chord_tremolo_iterator::constructor)
	      (let* ((seq-arg? (memq 'sequential-music
				     (ly:music-property e 'types)))
		     (count  (ly:music-property music 'repeat-count))
		     (dot-shift (if (= 0 (remainder count 3))
				    -1 0)))

		(if (= 0 -1)
		    (set! count (* 2 (quotient count 3))))
		
		(shift-duration-log music (+ (if seq-arg? 1 0)
					     (ly:intlog2 count)) dot-shift)
		
		(if seq-arg?
		    (ly:music-compress e (ly:make-moment (length (ly:music-property e 'elements)) 1)))))
	  
	  (set! (ly:music-property music 'length-callback)
		Repeated_music::unfolded_music_length)
	  (set! (ly:music-property music 'start-callback)
		Repeated_music::first_start)
	  (set! (ly:music-property music 'iterator-ctor)
		Unfolded_repeat_iterator::constructor)))
    
    (if (pair? es)
	(set! (ly:music-property music 'elements)
	      (map unfold-repeats es)))
    (if (ly:music? e)
	(set! (ly:music-property music 'element)
	      (unfold-repeats e)))
    music))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; property setting music objs.

(define-public (make-grob-property-set grob gprop val)
  "Make a Music expression that sets GPROP to VAL in GROB. Does a pop first,
i.e.  this is not an override"
  (make-music 'OverrideProperty
	      'symbol grob
	      'grob-property gprop
	      'grob-value val
	      'pop-first #t))

(define-public (make-grob-property-override grob gprop val)
  "Make a Music expression that sets GPROP to VAL in GROB. Does a pop first,
i.e.  this is not an override"
  (make-music 'OverrideProperty
	      'symbol grob
	      'grob-property gprop
	      'grob-value val))

(define-public (make-grob-property-revert grob gprop)
  "Revert the grob property GPROP for GROB."
  (make-music 'OverrideProperty
	      'symbol grob
	      'grob-property gprop))

(define direction-polyphonic-grobs
  '(Stem Tie Rest Slur Script TextScript Dots DotColumn Fingering))

(define-safe-public (make-voice-props-set n)
  (make-sequential-music
   (append
    (map (lambda (x) (make-grob-property-set x 'direction
					     (if (odd? n) -1 1)))
	 direction-polyphonic-grobs)
    (list
     (make-grob-property-set 'NoteColumn 'horizontal-shift (quotient n 2))
     (make-grob-property-set 'MultiMeasureRest 'staff-position (if (odd? n) -4 4)))))) 

(define-safe-public (make-voice-props-revert)
  (make-sequential-music
   (append
    (map (lambda (x) (make-grob-property-revert x 'direction))
	 direction-polyphonic-grobs)
    (list (make-grob-property-revert 'NoteColumn 'horizontal-shift))
    (list (make-grob-property-revert 'MultiMeasureRest 'staff-position)))))


(define-safe-public (context-spec-music m context #:optional id)
  "Add \\context CONTEXT = ID to M. "
  (let ((cm (make-music 'ContextSpeccedMusic
			'element m
			'context-type context)))
    (if (string? id)
	(set! (ly:music-property cm 'context-id) id))
    cm))

(define-public (descend-to-context m context)
  "Like context-spec-music, but only descending. "
  (let ((cm (context-spec-music m context)))
    (ly:music-set-property! cm 'descend-only #t)
    cm))

(define-public (make-non-relative-music mus)
  (make-music 'UnrelativableMusic
	      'element mus))

(define-public (make-apply-context func)
  (make-music 'ApplyContext
	      'procedure func))

(define-public (make-sequential-music elts)
  (make-music 'SequentialMusic
	      'elements elts))

(define-public (make-simultaneous-music elts)
  (make-music 'SimultaneousMusic
	      'elements elts))

(define-safe-public (make-event-chord elts)
  (make-music 'EventChord
	      'elements elts))

(define-public (make-skip-music dur)
  (make-music 'SkipMusic
	      'duration dur))

(define-public (make-grace-music music)
  (make-music 'GraceMusic
	      'element music))

;;;;;;;;;;;;;;;;

;; mmrest
(define-public (make-multi-measure-rest duration location)
  (make-music 'MultiMeasureRestMusicGroup
	      'origin location
	      'elements (list (make-music 'BarCheck
					  'origin location)
			      (make-event-chord (list (make-music 'MultiMeasureRestEvent
								  'origin location
								  'duration duration)))
			      (make-music 'BarCheck
					  'origin location))))

(define-public (glue-mm-rest-texts music)
  "Check if we have R1*4-\\markup { .. }, and if applicable convert to
a property set for MultiMeasureRestNumber."
  (define (script-to-mmrest-text script-music)
    "Extract 'direction and 'text from SCRIPT-MUSIC, and transform into property sets."
    (let ((dir (ly:music-property script-music 'direction))
	  (p   (make-music 'MultiMeasureTextEvent
			   'text (ly:music-property script-music 'text))))
      (if (ly:dir? dir)
	  (set! (ly:music-property p 'direction) dir))
      p))
  (if (eq? (ly:music-property music 'name) 'MultiMeasureRestMusicGroup)
      (let* ((text? (lambda (x) (memq 'script-event (ly:music-property x 'types))))
	     (es (ly:music-property  music 'elements))
	     (texts (map script-to-mmrest-text	(filter text? es)))
	     (others (remove text? es)))
	(if (pair? texts)
	    (set! (ly:music-property music 'elements)
		  (cons (make-event-chord texts) others)))))
  music)


(define-public (make-property-set sym val)
  (make-music 'PropertySet
	      'symbol sym
	      'value val))

(define-public (make-ottava-set octavation)
  (let ((m (make-music 'ApplyContext)))
    (define (ottava-modify context)
      "Either reset middleCPosition to the stored original, or remember
old middleCPosition, add OCTAVATION to middleCPosition, and set
OTTAVATION to `8va', or whatever appropriate."	    
      (if (number? (ly:context-property	 context 'middleCPosition))
	  (if (= octavation 0)
	      (let ((where (ly:context-property-where-defined context 'middleCPosition))
		    (oc0 (ly:context-property context 'originalCentralCPosition)))
		(ly:context-set-property! context 'middleCPosition oc0)
		(ly:context-unset-property where 'originalCentralCPosition)
		(ly:context-unset-property where 'ottavation))
	      (let* ((where (ly:context-property-where-defined context 'middleCPosition))
		     (c0 (ly:context-property context 'middleCPosition))
		     (new-c0 (+ c0 (* -7 octavation)))
		     (string (cdr (assoc octavation '((2 . "15ma")
						      (1 . "8va")
						      (0 . #f)
						      (-1 . "8va bassa")
						      (-2 . "15ma bassa"))))))
		(ly:context-set-property! context 'middleCPosition new-c0)
		(ly:context-set-property! context 'originalCentralCPosition c0)
		(ly:context-set-property! context 'ottavation string)))))
    (set! (ly:music-property m 'procedure) ottava-modify)
    (context-spec-music m 'Staff)))

(define-public (set-octavation ottavation)
  (ly:export (make-ottava-set ottavation)))

(define-public (make-time-signature-set num den . rest)
  "Set properties for time signature NUM/DEN.  Rest can contain a list
of beat groupings "
  (let* ((set1 (make-property-set 'timeSignatureFraction (cons num den)))
	 (beat (ly:make-moment 1 den))
	 (len  (ly:make-moment num den))
	 (set2 (make-property-set 'beatLength beat))
	 (set3 (make-property-set 'measureLength len))
	 (set4 (make-property-set 'beatGrouping (if (pair? rest)
						    (car rest)
						    '())))
	 (basic	 (list set1 set2 set3 set4)))
    (descend-to-context
     (context-spec-music (make-sequential-music basic) 'Timing) 'Score)))

(define-public (make-mark-set label)
  "Make the music for the \\mark command."  
  (let* ((set (if (integer? label)
		  (context-spec-music (make-property-set 'rehearsalMark label)
				      'Score)
		  #f))
	 (ev (make-music 'MarkEvent))
	 (ch (make-event-chord (list ev))))
    (if set
	(make-sequential-music (list set ch))
	(begin
	  (set! (ly:music-property ev 'label) label)
	  ch))))

(define-public (set-time-signature num den . rest)
  (ly:export (apply make-time-signature-set `(,num ,den . ,rest))))

(define-safe-public (make-penalty-music pen page-pen)
  (make-music 'BreakEvent
	      'penalty pen
	      'page-penalty page-pen))

(define-safe-public (make-articulation name)
  (make-music 'ArticulationEvent
	      'articulation-type name))

(define-public (make-lyric-event string duration)
  (make-music 'LyricEvent
	      'duration duration
	      'text string))

(define-safe-public (make-span-event type spandir)
  (make-music type
	      'span-direction spandir))

(define-public (set-mus-properties! m alist)
  "Set all of ALIST as properties of M." 
  (if (pair? alist)
      (begin
	(set! (ly:music-property m (caar alist)) (cdar alist))
	(set-mus-properties! m (cdr alist)))))

(define-public (music-separator? m)
  "Is M a separator?"
  (let ((ts (ly:music-property m 'types)))
    (memq 'separator ts)))

;;; splitting chords into voices.
(define (voicify-list lst number)
  "Make a list of Musics.

   voicify-list :: [ [Music ] ] -> number -> [Music]
   LST is a list music-lists.

   NUMBER is 0-base, i.e. Voice=1 (upstems) has number 0.
"
  (if (null? lst)
      '()
      (cons (context-spec-music
	     (make-sequential-music
	      (list (make-voice-props-set number)
		    (make-simultaneous-music (car lst))))
	     'Voice  (number->string (1+ number)))
	    (voicify-list (cdr lst) (1+ number)))))

(define (voicify-chord ch)
  "Split the parts of a chord into different Voices using separator"
  (let ((es (ly:music-property ch 'elements)))
    (set! (ly:music-property  ch 'elements)
	  (voicify-list (split-list es music-separator?) 0))
    ch))

(define-public (voicify-music m)
  "Recursively split chords that are separated with \\ "
  (if (not (ly:music? m))
      (ly:error (_ "music expected: ~S") m))
  (let ((es (ly:music-property m 'elements))
	(e (ly:music-property m 'element)))

    (if (pair? es)
	(set! (ly:music-property m 'elements) (map voicify-music es)))
    (if (ly:music? e)
	(set! (ly:music-property m 'element)  (voicify-music e)))
    (if (and (equal? (ly:music-property m 'name) 'SimultaneousMusic)
	     (reduce (lambda (x y ) (or x y)) #f (map music-separator? es)))
	(set! m (context-spec-music (voicify-chord m) 'Staff)))
    m))

(define-public (empty-music)
  (ly:export (make-music 'Music)))
;;;

					; Make a function that checks score element for being of a specific type. 
(define-public (make-type-checker symbol)
  (lambda (elt)
    ;;(display	symbol)
    ;;(eq? #t (ly:grob-property elt symbol))
    (not (eq? #f (memq symbol (ly:grob-property elt 'interfaces))))))

(define-public ((outputproperty-compatibility func sym val) grob g-context ao-context)
  (if (func grob)
      (set! (ly:grob-property grob sym) val)))


(define-public ((set-output-property grob-name symbol val)  grob grob-c context)
  "Usage:

\\applyoutput #(set-output-property 'Clef 'extra-offset '(0 . 1))

"
  (let ((meta (ly:grob-property grob 'meta)))
    (if (equal?  (cdr (assoc 'name meta)) grob-name)
	(set! (ly:grob-property grob symbol) val))))


;;
(define-public (smart-bar-check n)
  "Make	 a bar check that checks for a specific bar number. 
"
  (let ((m (make-music 'ApplyContext)))
    (define (checker tr)
      (let* ((bn (ly:context-property tr 'currentBarNumber)))
	(if (= bn n)
	    #t
	    (ly:error
	     ;; FIXME: uncomprehensable message
	     (_ "Bar check failed.  Expect to be at ~a, instead at ~a")
	     n bn))))
    (set! (ly:music-property m 'procedure) checker)
    m))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; warn for bare chords at start.

(define (has-request-chord elts)
  (reduce (lambda (x y) (or x y)) #f
	  (map (lambda (x)
		 (equal? (ly:music-property x 'name) 'RequestChord))
	       elts)))

(define (ly:music-message music msg)
  (let ((ip (ly:music-property music 'origin)))
    (if (ly:input-location? ip)
	(ly:input-message ip msg)
	(ly:warning msg))))

(define (check-start-chords music)
  "Check music expression for a Simultaneous_music containing notes\n(ie. Request_chords),
without context specification. Called  from parser."
  (let ((es (ly:music-property music 'elements))
	(e (ly:music-property music 'element))
	(name (ly:music-property music 'name)))
    (cond ((equal? name "Context_specced_music") #t)
	  ((equal? name "Simultaneous_music")
	   (if (has-request-chord es)
	       (ly:music-message music "Starting score with a chord.\nInsert an explicit \\context before chord")
	       (map check-start-chords es)))
	  ((equal? name "SequentialMusic")
	   (if (pair? es)
	       (check-start-chords (car es))))
	  (else (if (ly:music? e) (check-start-chords e)))))
  music)



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; setting stuff for grace context.
;;

(define (vector-extend v x)
  "Make a new vector consisting of V, with X added to the end."
  (let* ((n (vector-length v))
	 (nv (make-vector (+ n 1) '())))
    (vector-move-left! v 0 n nv 0)
    (vector-set! nv n x)
    nv))

(define (vector-map f v)
  "Map	F over V. This function returns nothing."
  (do ((n (vector-length v))
       (i 0 (+ i 1)))
      ((>= i n))
    (f (vector-ref v i))))

(define (vector-reverse-map f v)
  "Map	F over V, N to 0 order. This function returns nothing."
  (do ((i (- (vector-length v) 1) (- i 1)))
      ((< i 0))
    (f (vector-ref v i))))

;; TODO:  make a remove-grace-property too.
(define-public (add-grace-property context-name grob sym val)
  "Set SYM=VAL for GROB in CONTEXT-NAME. "
  (define (set-prop context)
    (let* ((where (ly:context-property-where-defined context 'graceSettings))
	   (current (ly:context-property where 'graceSettings))
	   (new-settings (append current
				 (list (list context-name grob sym val)))))
      (ly:context-set-property! where 'graceSettings new-settings)))
  (ly:export (context-spec-music (make-apply-context set-prop) 'Voice)))



(defmacro-public def-grace-function (start stop)
  `(def-music-function (parser location music) (ly:music?)
     (make-music 'GraceMusic
		 'origin location
		 'element (make-music 'SequentialMusic
				      'elements (list (ly:music-deep-copy ,start)
						      music
						      (ly:music-deep-copy ,stop))))))

(defmacro-public def-music-function (args signature . body)
  "Helper macro for `ly:make-music-function'.
Syntax:
  (def-music-function (parser location arg1 arg2 ...) (arg1-type? arg2-type? ...)
    ...function body...)
"
  `(ly:make-music-function (list ,@signature)
			   (lambda (,@args)
			     ,@body)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define-public (cue-substitute quote-music)
  "Must happen after quote-substitute."
  
  (if (vector? (ly:music-property quote-music 'quoted-events))
      (let* ((dir (ly:music-property quote-music 'quoted-voice-direction))
	     (main-voice (if (eq? 1 dir) 1 0))
	     (cue-voice (if (eq? 1 dir) 0 1))
	     (main-music (ly:music-property quote-music 'element))
	     (return-value quote-music))
	
	(if (or (eq? 1 dir) (eq? -1 dir))
	    
	    ;; if we have stem dirs, change both quoted and main music
	    ;; to have opposite stems.
	    (begin
	      (set! return-value

		    ;; cannot context-spec Quote-music, since context
		    ;; for the quotes is determined in the iterator.
		    (make-sequential-music
		     (list
		      (context-spec-music (make-voice-props-set cue-voice) 'CueVoice "cue")
		      quote-music
		      (context-spec-music (make-voice-props-revert)  'CueVoice "cue"))))
	      (set! main-music
		    (make-sequential-music
		     (list
		      (make-voice-props-set main-voice)
		      main-music
		      (make-voice-props-revert))))
	      (set! (ly:music-property quote-music 'element) main-music)))

	return-value)
      quote-music))

(define-public ((quote-substitute quote-tab) music)
  (let* ((quoted-name (ly:music-property music 'quoted-music-name))
	 (quoted-vector (if (string? quoted-name)
			    (hash-ref quote-tab quoted-name #f)
			    #f)))
    
    (if (string? quoted-name)
	(if  (vector? quoted-vector)
	     (set! (ly:music-property music 'quoted-events) quoted-vector)
	     (ly:warning (_ "can't find quoted music `~S'" quoted-name))))
    music))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; switch it on here, so parsing and init isn't checked (too slow!)
;;
;; automatic music transformations.

(define (switch-on-debugging m)
  (if (defined? 'set-debug-cell-accesses!)
      (set-debug-cell-accesses! 15000))
  m)

(define (music-check-error music)
  (define found #f)
  (define (signal m)
    (if (and (ly:music? m)
	     (eq? (ly:music-property m 'error-found) #t))
	(set! found #t)))
  
  (for-each signal (ly:music-property music 'elements))
  (signal (ly:music-property music 'element))

  (if found
      (set! (ly:music-property music 'error-found) #t))
  music)

(define (precompute-music-length music)
  (set! (ly:music-property music 'length)
	(ly:music-length music))
  music)

(define-public toplevel-music-functions
  (list
   (lambda (music parser) (voicify-music music))
   (lambda (x parser) (music-map glue-mm-rest-texts x))
   (lambda (x parser) (music-map music-check-error x))
   (lambda (x parser) (music-map precompute-music-length x))
   (lambda (music parser)

     (music-map (quote-substitute (ly:parser-lookup parser 'musicQuotes))  music))
   
   ;; switch-on-debugging
   (lambda (x parser) (music-map cue-substitute x))
;   (lambda (x parser) (music-map display-scheme-music x))

   ))

;;;;;;;;;;;;;;;;;
;; lyrics

(define (apply-durations lyric-music durations) 
  (define (apply-duration music)
    (if (and (not (equal? (ly:music-length music) ZERO-MOMENT))
	     (ly:duration?  (ly:music-property music 'duration)))
	(begin
	  (set! (ly:music-property music 'duration) (car durations))
	  (set! durations (cdr durations)))))
  
  (music-map apply-duration lyric-music))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;

(define-public ((add-balloon-text object-name text off) grob orig-context cur-context)
  "Usage: see input/regression/balloon.ly "
  (let* ((meta (ly:grob-property grob 'meta))
	 (nm (if (pair? meta) (cdr (assoc 'name meta)) "nonexistant"))
	 (cb (ly:grob-property grob 'print-function)))
    (if (equal? nm object-name)
	(begin
	  (set! (ly:grob-property grob 'print-function) Balloon_interface::print)
	  (set! (ly:grob-property grob 'balloon-original-callback) cb)
	  (set! (ly:grob-property grob 'balloon-text) text)
	  (set! (ly:grob-property grob 'balloon-text-offset) off)
	  (set! (ly:grob-property grob 'balloon-text-props) '((font-family . roman)))))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; accidentals

(define-public (set-accidentals-properties extra-natural
					   auto-accs auto-cauts
					   context)
  (context-spec-music
   (make-sequential-music
    (append (if (boolean? extra-natural)
		(list (make-property-set 'extraNatural extra-natural))
		'())
	    (list (make-property-set 'autoAccidentals auto-accs)
		  (make-property-set 'autoCautionaries auto-cauts))))
   context))

(define-public (set-accidental-style style . rest)
  "Set accidental style to STYLE. Optionally takes a context argument,
e.g. 'Staff or 'Voice. The context defaults to Voice, except for piano styles, which
use GrandStaff as a context. "
  (let ((context (if (pair? rest)
		     (car rest) 'Staff))
	(pcontext (if (pair? rest)
		      (car rest) 'GrandStaff)))
    (ly:export
     (cond
      ;; accidentals as they were common in the 18th century.
      ((equal? style 'default)
       (set-accidentals-properties #t '(Staff (same-octave . 0))
				   '() context))
      ;; accidentals from one voice do NOT get cancelled in other voices
      ((equal? style 'voice)
       (set-accidentals-properties #t '(Voice (same-octave . 0))
				   '() context))
      ;; accidentals as suggested by Kurt Stone, Music Notation in the 20th century.
      ;; This includes all the default accidentals, but accidentals also needs cancelling
      ;; in other octaves and in the next measure.
      ((equal? style 'modern)
       (set-accidentals-properties #f '(Staff (same-octave . 0) (any-octave . 0) (same-octave . 1))
				   '()	context))
      ;; the accidentals that Stone adds to the old standard as cautionaries
      ((equal? style 'modern-cautionary)
       (set-accidentals-properties #f '(Staff (same-octave . 0))
				   '(Staff (any-octave . 0) (same-octave . 1))
				   context))
      ;; Multivoice accidentals to be read both by musicians playing one voice
      ;; and musicians playing all voices.
      ;; Accidentals are typeset for each voice, but they ARE cancelled across voices.
      ((equal? style 'modern-voice)
       (set-accidentals-properties  #f
				    '(Voice (same-octave . 0) (any-octave . 0) (same-octave . 1)
					    Staff (same-octave . 0) (any-octave . 0) (same-octave . 1))
				    '()
				    context))
      ;; same as modernVoiceAccidental eccept that all special accidentals are typeset
      ;; as cautionaries
      ((equal? style 'modern-voice-cautionary)
       (set-accidentals-properties #f
				   '(Voice (same-octave . 0))
				   '(Voice (any-octave . 0) (same-octave . 1)
					   Staff (same-octave . 0) (any-octave . 0) (same-octave . 1))
				   context))
      ;; stone's suggestions for accidentals on grand staff.
      ;; Accidentals are cancelled across the staves in the same grand staff as well
      ((equal? style 'piano)
       (set-accidentals-properties #f
				   '(Staff (same-octave . 0)
					   (any-octave . 0) (same-octave . 1)
					   GrandStaff (any-octave . 0) (same-octave . 1))
				   '()
				   pcontext))
      ((equal? style 'piano-cautionary)
       (set-accidentals-properties #f
				   '(Staff (same-octave . 0))
				   '(Staff (any-octave . 0) (same-octave . 1)
					   GrandStaff (any-octave . 0) (same-octave . 1))
				   pcontext))
      ;; do not set localKeySignature when a note alterated differently from
      ;; localKeySignature is found.
      ;; Causes accidentals to be printed at every note instead of
      ;; remembered for the duration of a measure.
      ;; accidentals not being remembered, causing accidentals always to be typeset relative to the time signature
      ((equal? style 'forget)
       (set-accidentals-properties '()
				   '(Staff (same-octave . -1))
				   '() context))
      ;; Do not reset the key at the start of a measure.  Accidentals will be
      ;; printed only once and are in effect until overridden, possibly many
      ;; measures later.
      ((equal? style 'no-reset)
       (set-accidentals-properties '()
				   '(Staff (same-octave . #t))
				   '()
				   context))
      (else
       (ly:warning (_ "unknown accidental style: ~S" style))
       (make-sequential-music '()))))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define-public (skip-of-length mus)
  "Create a skip of exactly the same length as MUS."
  (let* ((skip
	  (make-music
	   'SkipEvent
	   'duration (ly:make-duration 0 0))))

    (make-event-chord (list (ly:music-compress skip (ly:music-length mus))))))

(define-public (mmrest-of-length mus)
  "Create a mmrest of exactly the same length as MUS."
  
  (let* ((skip
	  (make-multi-measure-rest
	   (ly:make-duration 0 0) '())))
    (ly:music-compress skip (ly:music-length mus))
    skip))
