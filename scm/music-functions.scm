;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; tuplets.

(define-public (denominator-tuplet-formatter mus)
  (number->string (ly:get-mus-property mus 'denominator)))

(define-public (fraction-tuplet-formatter mus)
  (string-append (number->string (ly:get-mus-property mus 'numerator))
		 ":"
		 (number->string (ly:get-mus-property mus 'denominator))
		 ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define-public (shift-duration-log music shift dot)
  "Recurse through music, adding SHIFT to ly:duration-log and optionally 
  a dot to any note encountered. This scales the music up by a factor 
  2^shift * (2 - (1/2)^dot)"
  (let* ((es (ly:get-mus-property music 'elements))
         (e (ly:get-mus-property music 'element))
         (n  (ly:music-name music))
	 (f  (lambda (x)  (shift-duration-log x shift dot)))
	 )

    ;; FIXME: broken by the great music rename.
    (if (or (equal? n "Note_req")
	    (equal? n "Rest_req"))
	(let* (
	       (d (ly:get-mus-property music 'duration))
	       (cp (ly:duration-factor d))
	       (nd (ly:make-duration (+ shift (ly:duration-log d))
				  (+ dot (duration-dot-count d))
				  (car cp)
				  (cdr cp)))
	       
	       )
	  (ly:set-mus-property! music 'duration nd)
	  ))
    
    (if (pair? es)
        (ly:set-mus-property!
         music 'elements
         (map f es)))
    
    (if (ly:music? e)
        (ly:set-mus-property!
         music 'element
         (f e)))
    
    music))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; repeats.

(define-public (unfold-repeats music)
"
This function replaces all repeats  with unfold repeats. It was 
written by Rune Zedeler. "
  (let* ((es (ly:get-mus-property music 'elements))
         (e (ly:get-mus-property music 'element))
         (n  (ly:music-name music)))
 
    (if (equal? n "Repeated_music")
        (begin
	  (if (equal?
	       (ly:get-mus-property music 'iterator-ctor)
	       Chord_tremolo_iterator::constructor)
	      (shift-duration-log music  (ly:intlog2 (ly:get-mus-property music 'repeat-count)) 0)
	      )
          (ly:set-mus-property!
           music 'length Repeated_music::unfolded_music_length)
	  (ly:set-mus-property!
	   music 'start-moment-function Repeated_music::first_start)
          (ly:set-mus-property!
           music 'iterator-ctor Unfolded_repeat_iterator::constructor)))

    (if (pair? es)
        (ly:set-mus-property!
         music 'elements
         (map unfold-repeats es)))

    (if (ly:music? e)
        (ly:set-mus-property!
         music 'element
         (unfold-repeats e)))

    music))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define  (pitchify-scripts music)
  "Copy the pitch fields of the Note_requests into  Text_script_requests, to aid
Fingering_engraver."
  (define (find-note musics)
    (filter-list (lambda (m) (equal? (ly:music-name m) "Note_req")) musics)
    )
  (define (find-scripts musics)
    (filter-list (lambda (m) (equal? (ly:music-name m) "Text_script_req")) musics))

  (let* (
	 (e (ly:get-mus-property music 'element))
	 (es (ly:get-mus-property music 'elements))
	 (notes (find-note es))
	 (pitch (if (pair? notes) (ly:get-mus-property (car  notes) 'pitch) #f))
	 )

    (if pitch
	(map (lambda (x) (ly:set-mus-property! x 'pitch pitch)) (find-scripts es))
	)
	
    (if (pair? es)
        (ly:set-mus-property!
         music 'elements
         (map pitchify-scripts es)))

    (if (ly:music? e)
        (ly:set-mus-property!
         music 'element
         (pitchify-scripts e)))

    music))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; property setting music objs.
(define-public (make-grob-property-set grob gprop val)
  "Make a M-exp that sets GPROP to VAL in GROBS. Does a pop first, i.e.
this is not an override 
"
  
   (let* ((m (make-music-by-name  'OverrideProperty)))
     (ly:set-mus-property! m 'symbol grob)
     (ly:set-mus-property! m 'grob-property gprop)
     (ly:set-mus-property! m 'grob-value val)
     (ly:set-mus-property! m 'pop-first #t)
		
     m
   
   ))


(define-public (make-grob-property-revert grob gprop)
  "Revert the grob property GPROP for GROB."
   (let* ((m (make-music-by-name  'OverrideProperty)))
     (ly:set-mus-property! m 'symbol grob)
     (ly:set-mus-property! m 'grob-property gprop)
		
     m
   
   ))
   
(define-public (make-voice-props-set n)
  (make-sequential-music
   (append
      (map (lambda (x) (make-grob-property-set x 'direction
					       (if (odd? n) -1 1)))
	   '(Tie Slur Stem Dots))
      (list
       (make-grob-property-set 'NoteColumn 'horizontal-shift (quotient n 2))
       (make-grob-property-set 'MultiMeasureRest 'staff-position
			       (if (odd? n) -4 4)
			       )
       
       )
   )
  ))

(define-public (make-voice-props-revert)
  (make-sequential-music
   (list
      (make-grob-property-revert 'Tie 'direction)
      (make-grob-property-revert 'Dots 'direction)
      (make-grob-property-revert 'Stem 'direction)
      (make-grob-property-revert 'Slur 'direction)	    
      (make-grob-property-revert 'NoteColumn 'horizontal-shift)
   ))
  )

(define-public (context-spec-music m context . rest)
  "Add \context CONTEXT = foo to M. "
  
  (let* ((cm (make-music-by-name 'ContextSpeccedMusic)))
    (ly:set-mus-property! cm 'element m)
    (ly:set-mus-property! cm 'context-type context)
    (if (and  (pair? rest) (string? (car rest)))
	(ly:set-mus-property! cm 'context-id (car rest))
    )
    cm
  ))

(define-public (make-sequential-music elts)
  (let*  ((m (make-music-by-name 'SequentialMusic)))
    (ly:set-mus-property! m 'elements elts)
    m
  ))

(define-public (make-simultaneous-music elts)
  (let*  ((m (make-music-by-name 'SimultaneousMusic)))
    (ly:set-mus-property! m 'elements elts)
    m
    ))

(define-public (make-event-chord elts)
  (let*  ((m (make-music-by-name 'EventChord)))
    (ly:set-mus-property! m 'elements elts)
    m
    ))


(define-public (make-multi-measure-rest duration location)
  (let*
      (
       (start (make-music-by-name 'MultiMeasureRestEvent))
       (stop  (make-music-by-name 'MultiMeasureRestEvent))
       (skip ( make-music-by-name 'SkipEvent))
       (ch (make-music-by-name 'BarCheck))
       (ch2  (make-music-by-name 'BarCheck))
       )

    (ly:set-mus-property! start 'span-direction START)
    (ly:set-mus-property! stop 'span-direction STOP)    
    (ly:set-mus-property! skip 'duration duration)
    (map (lambda (x) (ly:set-mus-property! x 'origin location))
	 (list start stop skip ch ch2))
    (make-sequential-music
     (list
      ch
      (make-event-chord (list start))
      (make-event-chord (list skip))
      (make-event-chord (list stop))
      ch2
      ))
    ))


(define-public (make-property-set sym val)
  (let*
      (
       (m (make-music-by-name 'PropertySet))
       )
    (ly:set-mus-property! m 'symbol sym)
    (ly:set-mus-property! m 'value val)
    m
  ))

(define-public (make-time-signature-set num den . rest)
  " Set properties for time signature NUM/DEN.
Rest can contain a list of beat groupings 

"
  
  (let*
      (
       (set1 (make-property-set 'timeSignatureFraction (cons num den) ))
       (beat (ly:make-moment 1 den))
       (len  (ly:make-moment num den))
       (set2 (make-property-set 'beatLength beat))
       (set3 (make-property-set 'measureLength len))
       (set4 (make-property-set 'beatGrouping (if (pair? rest)
						  (car rest)
						  '())))
       (basic  (list set1 set2 set3 set4))
       
       )

    (context-spec-music
     (make-sequential-music basic) "Timing")))

(define-public (set-time-signature num den . rest)
  (ly:export (apply make-time-signature-set `(,num ,den . ,rest)))
  )

(define-public (make-penalty-music pen)
 (let
     ((m (make-music-by-name 'BreakEvent)))
   (ly:set-mus-property! m 'penalty pen)
   m))

(define-public (make-articulation name)
  (let* (
	 (m (make-music-by-name 'ArticulationEvent))
      )
      (ly:set-mus-property! m 'articulation-type name)
      m
  ))

(define-public (make-span-event type spandir)
  (let* (
	 (m (make-music-by-name  type))
	 )
    (ly:set-mus-property! m 'span-direction spandir)
    m
    ))

(define-public (set-mus-properties! m alist)
  "Set all of ALIST as properties of M." 
  (if (pair? alist)
      (begin
	(ly:set-mus-property! m (caar alist) (cdar alist))
	(set-mus-properties! m (cdr alist)))
  ))

(define-public (music-separator? m)
  "Is M a separator?"
  (let* ((ts (ly:get-mus-property m 'types )))
    (memq 'separator ts)
  ))

(define (split-one sep?  l acc)
  "Split off the first parts before separator and return both parts.

"
  (if (null? l)
      (cons acc '())
      (if (sep? (car l))
	  (cons acc (cdr l))
	  (split-one sep? (cdr l) (cons (car l) acc))
	  )
      ))

(define-public (split-list l sep?)
  "

(display (split-list '(a b c / d e f / g) (lambda (x) (equal? x '/))) )
=>
 ...

"
  (if (null? l)
      '()
      (let* ((c (split-one sep? l '())))
	(cons (reverse! (car c) '()) (split-list (cdr c) sep?))
	)
      )
  )

;;; splitting chords into voices.

(define (voicify-list lst number)
   "Make a list of Musics.

   voicify-list :: [ [Music ] ] -> number -> [Music]
   LST is a list music-lists.
"

   (if (null? lst) '()
       (cons (context-spec-music
	      (make-sequential-music
	       (list
		(make-voice-props-set number)
		(make-simultaneous-music (car lst))))

	      "Voice"  (number->string number))
	      (voicify-list (cdr lst) (+ number 1))
       ))
   )

(define (voicify-chord ch)
  "Split the parts of a chord into different Voices using separator"
   (let* ((es (ly:get-mus-property ch 'elements)))


     (ly:set-mus-property!  ch 'elements
       (voicify-list (split-list es music-separator?) 0))
     ch
   ))

(define (voicify-music m)
   "Recursively split chords that are separated with \\ "
   
   (if (not (ly:music? m))
       (begin (display m)
       (error "not music!"))
       )
   (let*
       ((es (ly:get-mus-property m 'elements))
	(e (ly:get-mus-property m 'element))
	)
     (if (pair? es)
	 (ly:set-mus-property! m 'elements (map voicify-music es)))
     (if (ly:music? e)
	 (ly:set-mus-property! m 'element  (voicify-music e)))
     (if
      (and (equal? (ly:music-name m) "Simultaneous_music")
	   (reduce (lambda (x y ) (or x y)) 	(map music-separator? es)))
      (voicify-chord m)
      )

     m
     ))

(define-public (empty-music)
  (ly:export (make-music-by-name 'Music))
  )
;;;

; Make a function that checks score element for being of a specific type. 
(define-public (make-type-checker symbol)
  (lambda (elt)
    ;;(display  symbol)
    ;;(eq? #t (ly:get-grob-property elt symbol))
    (not (eq? #f (memq symbol (ly:get-grob-property elt 'interfaces))))))


;;
(define-public (smart-bar-check n)
  "Make  a bar check that checks for a specific bar number. 
"
  (let*
      (
       (m (make-music-by-name 'ApplyContext))
       )
    
    (define (checker tr)
      (let* ((bn (ly:get-context-property tr 'currentBarNumber)))
	(if (= bn  n)
	    #t
	    (error
	     (format "Bar check failed, we should have reached ~a, instead at ~a\n"
		     n bn ))
	    )))

    (ly:set-mus-property! m 'procedure checker)
    m
    ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; warn for bare chords at start.

(define (has-request-chord elts)
  (reduce (lambda (x y) (or x y)) (map (lambda (x) (equal? (ly:music-name x)
							   "Request_chord")) elts)
  ))

(define (ly:music-message music msg)
  (let* (
      (ip (ly:get-mus-property music 'origin))
      )

    (if (ly:input-location? ip)
	(ly:input-message ip msg)
	(ly:warn msg))
  ))
  
(define (check-start-chords music)
  "Check music expression for a Simultaneous_music containing notes\n(ie. Request_chords), without context specification. Called  from parser."
  
     (let*
       ((es (ly:get-mus-property music 'elements))
	(e (ly:get-mus-property music 'element))
	(name (ly:music-name music)) 
	)

       (cond 
	 ((equal? name "Context_specced_music") #t)
	 ((equal? name "Simultaneous_music")

	  (if (has-request-chord es)
	      (ly:music-message music "Starting score with a chord.\nPlease insert an explicit \\context before chord")
	      (map check-start-chords es)))
	 
	 ((equal? name "Sequential_music")
	   (if (pair? es)
	       (check-start-chords (car es))))
	  (else (if (ly:music? e) (check-start-chords e )))
       
       ))
     music
     )


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; switch it on here, so parsing and init isn't checked (too slow!)

;; automatic music transformations.

(define (switch-on-debugging m)
  (set-debug-cell-accesses! 15000)
  m
  )

(define-public toplevel-music-functions
  (list check-start-chords
	voicify-music

; switch-on-debugging
	))


