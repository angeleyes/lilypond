;;;; document-translation.scm -- Functions for engraver documentation
;;;;
;;;; source file of the GNU LilyPond music typesetter
;;;; 
;;;; (c) 2000--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
;;;;                 Jan Nieuwenhuizen <janneke@gnu.org>

(define (engraver-makes-grob? name-symbol grav)
  (memq name-symbol (assoc 'grobs-created (ly:translator-description grav))))

(define (engraver-accepts-music-type? name-symbol grav)
  (memq name-symbol (assoc 'events-accepted (ly:translator-description grav))))

(define (engraver-accepts-music-types? types grav)
  (if (null? types)
      #f
      (or
       (engraver-accepts-music-type? (car types) grav)
       (engraver-accepts-music-types? (cdr types) grav))))

(define (engraver-doc-string engraver in-which-contexts)
  (let* ((propsr (cdr (assoc 'properties-read (ly:translator-description engraver))))
	 (propsw (cdr (assoc 'properties-written (ly:translator-description engraver))))
	 (accepted  (cdr (assoc 'events-accepted (ly:translator-description engraver)))) 
	 (name-sym  (ly:translator-name engraver))
	 (name-str (symbol->string name-sym))
	 (desc (cdr (assoc 'description (ly:translator-description engraver))))
	 (grobs (engraver-grobs engraver)))

    (string-append
     desc
     "\n\n"
     (if (pair? accepted)
	 (string-append
	  "Music types accepted:\n\n"
	  (human-listify
	   (map (lambda (x)
		  (string-append
		   "@ref{"
		   (symbol->string x)
		   "}")) accepted)))
	 "")
     "\n\n"
     (if (pair? propsr)
	 (string-append
	  "Properties (read)"
	  (description-list->texi
	   (map (lambda (x) (property->texi 'translation  x '())) propsr)))
	 "")
     
     (if (null? propsw)
	 ""
	 (string-append
	  "Properties (write)" 
	  (description-list->texi
	   (map (lambda (x) (property->texi 'translation  x '())) propsw))))
     (if  (null? grobs)
	  ""
	  (string-append
	   "This engraver creates the following layout objects: \n "
	   (human-listify (map ref-ify (uniq-list (sort grobs string<? ))))
	   "."))

     "\n\n"

     (if in-which-contexts
	 (let* ((layout-alist (ly:output-description $defaultlayout))
		(context-description-alist (map cdr layout-alist))
		(contexts
		 (apply append
			(map
			 (lambda (x)
			   (let* ((context (cdr (assoc 'context-name x)))
				  (group (assq-ref x 'group-type))
				  (consists (append
					     (if group
						 (list group)
						 '())
					     (cdr (assoc 'consists x)))))
			     (if (member name-sym consists)
				 (list context)
				 '())))
			 context-description-alist))))
	   (string-append
	    "@code{" name-str "} is part of contexts: "
	    (human-listify (map ref-ify
				(sort
				 (map symbol->string contexts) string<?)))))
	 ""))))

;; First level Engraver description
(define (engraver-doc grav)
  (make <texi-node>
    #:name (symbol->string (ly:translator-name grav))
    #:text (engraver-doc-string grav #t)))

;; Second level, part of Context description
(define name->engraver-table (make-vector 61 '()))
(map
 (lambda (x)
   (hash-set! name->engraver-table (ly:translator-name x) x))
 (ly:get-all-translators))

(define (find-engraver-by-name name)
  "NAME is a symbol." 
  (hash-ref name->engraver-table name #f))

(define (document-engraver-by-name name)
  "NAME is a symbol."
  (let* ((eg (find-engraver-by-name name )))

    (cons (string-append "@code{" (ref-ify (symbol->string name)) "}")
	  (engraver-doc-string eg #f))))

(define (document-property-operation op)
  (let ((tag (car op))
	(body (cdr op))
	(sym (cadr op)))

    (cond
     ((equal?  tag 'push)
      (string-append
       "@item "
       (if (null? (cddr body))
	   "Revert "
	   "Set ")
       "grob-property @code{"
       (symbol->string (cadr body))
       "} in @ref{" (symbol->string sym)
       "}"
       (if (not (null? (cddr body)))
	   (string-append " to @code{" (scm->texi (cadr (cdr body))) "}" ))
       "\n"))
     ((equal? (object-property sym 'is-grob?) #t) "")
     ((equal? (car op) 'assign)
      (string-append
       "@item Set translator property @code{"
       (symbol->string (car body))
       "} to @code{"
       (scm->texi (cadr body))
       "}\n")))))

(define (context-doc context-desc)
  (let* ((name-sym (cdr (assoc 'context-name context-desc)))
	 (name (symbol->string name-sym))
	 (aliases (map symbol->string (cdr (assoc 'aliases context-desc))))
	 (desc-handle (assoc 'description context-desc))
	 (desc (if (and  (pair? desc-handle) (string? (cdr desc-handle)))
		   (cdr desc-handle) "(not documented)"))
	 
	 (accepts (cdr (assoc 'accepts context-desc)))
	 (group (assq-ref context-desc 'group-type))

	 (consists (append
		    (if group (list group)
			'())
		    (cdr (assoc 'consists context-desc))))
	 (props (cdr (assoc 'property-ops context-desc)))
	 (grobs  (context-grobs context-desc))
	 (grob-refs (map (lambda (x) (ref-ify x)) grobs)))

    (make <texi-node>
      #:name name
      #:text
      (string-append 
       desc
       (if (pair? aliases)
	   (string-append "\n\n This context is also known as: \n\n"
			  (human-listify aliases))
	   "")
       "\n\nThis context creates the following layout objects: \n\n"
       (human-listify (uniq-list (sort grob-refs string<? )))
       "."
       (if (pair? props)
	   (string-append
	    "\n\nThis context sets the following properties:\n"
	    "@itemize @bullet\n"
	    (apply string-append (map document-property-operation props))
	    "@end itemize\n")
	   "")
       
       (if (null? accepts)
	   "\n\nThis context is a `bottom' context; it can not contain other contexts."
	   (string-append
	    "\n\nContext "
	    name " can contain \n"
	    (human-listify (map ref-ify (map symbol->string accepts)))))
       
       "\n\nThis context is built from the following engravers: "
       (description-list->texi
	(map document-engraver-by-name consists))))))

(define (engraver-grobs grav)
  (let* ((eg (if (symbol? grav)
		 (find-engraver-by-name grav)
		 grav)))
    (if (eq? eg #f)
	'()
	(map symbol->string (cdr (assoc 'grobs-created (ly:translator-description eg)))))))

(define (context-grobs context-desc)
  (let* ((group (assq-ref context-desc 'group-type))
	 (consists (append
		    (if group
			(list group)
			'())
		    (cdr (assoc 'consists context-desc))))
	 (grobs  (apply append
			(map engraver-grobs consists))))
    grobs))

(define (all-contexts-doc)
  (let* ((layout-alist
	  (sort (ly:output-description $defaultlayout)
		(lambda (x y) (symbol<? (car x) (car y)))))
	 (names (sort (map symbol->string (map car layout-alist)) string<?))
	 (contexts (map cdr layout-alist)))

    (make <texi-node>
      #:name "Contexts"
      #:desc "Complete descriptions of all contexts"
      #:children
      (map context-doc contexts))))

(define all-engravers-list  (ly:get-all-translators))
(set! all-engravers-list
      (sort all-engravers-list
	    (lambda (a b) (string<? (symbol->string (ly:translator-name a))
				    (symbol->string (ly:translator-name b))))))

(define (all-engravers-doc)
  (make <texi-node>
    #:name "Engravers"
    #:desc "All separate engravers"
    #:text "See @usermanref{Modifying context plug-ins}."
    #:children
    (map engraver-doc all-engravers-list)))

(define (translation-properties-doc-string lst)
  (let* ((ps (sort (map symbol->string lst) string<?))
	 (sortedsyms (map string->symbol ps))
	 (propdescs
	  (map
	   (lambda (x) (property->texi 'translation  x '()))
	   sortedsyms))
	 (texi (description-list->texi propdescs)))
    texi))

(define (translation-doc-node)
  (make <texi-node>
    #:name "Translation"
    #:desc "From music to layout"
    #:children
    (list
     (all-contexts-doc)
     (all-engravers-doc)
     (make <texi-node>
       #:name "Tunable context properties"
       #:desc "All tunable context properties"
       #:text (translation-properties-doc-string
	       all-user-translation-properties))

     (make <texi-node>
       #:name "Internal context properties"
       #:desc "All internal context properties"
       #:text (translation-properties-doc-string
	       all-internal-translation-properties)))))
