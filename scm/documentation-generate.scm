;;; generate-documentation.scm -- Generate documentation
;;;
;;; source file of the GNU LilyPond music typesetter
;;; 
;;; (c)  2000--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
;;; Jan Nieuwenhuizen <janneke@gnu.org>

;;; File entry point for generated documentation

;;; Running LilyPond on this file generates the documentation

(debug-enable 'debug)
(debug-enable 'backtrace)
(read-enable 'positions)

;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;; TODO : make modules of these!
;;;;;;;;;;;;;;;;

(define load-files '("documentation-lib.scm"
		     "document-functions.scm"
		     "document-translation.scm"
		     "document-music.scm"
		     "document-backend.scm"
		     "document-markup.scm"
		     ))
(map ly:load load-files)


;;(define no-copies #t)  ; from 490 to 410K, but doesn't look nice yet
;;
;; Also, copies of interfaces use up lots more space, but that's
;; functional because the default property values of the interfaces
;; are described...
(define no-copies #f)

(define file-name "lilypond-internals")
(define outname (string-append file-name ".texi"))
(define out-port (open-output-file outname))

(writing-wip outname)

(display
 (string-append
  "@c -*-texinfo-*-"
  (texi-file-head "LilyPond internals" outname "(lilypond.info)")
  "

@ifhtml
@macro usermanref{NAME}
@uref{../lilypond/\\NAME\\.html,\\NAME\\}
@cindex \\NAME\\
@end macro
@end ifhtml

@ifinfo
@macro usermanref{NAME}
@inforef{\\NAME\\,,lilypond}
@cindex \\NAME\\
@end macro
@end ifinfo

@ifhtml
@macro glossaryref{NAME}
@uref{../music-glossary/\\NAME\\.html,\\NAME\\}
@cindex \\NAME\\
@end macro
@end ifhtml

@ifinfo
@macro glossaryref{NAME}
@inforef{\\NAME\\,,music-glossary}
@cindex \\NAME\\
@end macro
@end ifinfo


@ignore
@omftitle LilyPond internals
@omfcreator Han-Wen Nienhuys and Jan Nieuwenhuizen
@omfdescription Programmer's reference of the LilyPond music engraving system
@omftype user's guide
@omflanguage English
@omfcategory Applications|Publishing
@end ignore


") out-port)

(define top-node
  (make <texi-node>
    #:name "Top"
    #:children
    (list
     (music-doc-node)
     (translation-doc-node)
     (backend-doc-node)
     (all-scheme-functions-doc)
     (markup-doc-node)
     (make <texi-node>
       #:name "Index"
       #:text "
@unnumbered Concept index

@printindex cp

@unnumbered Variable index

@printindex vr

@unnumbered Function index

@printindex fn

\n@bye"

       
     )
    )))


(dump-node top-node out-port 0)
(newline (current-error-port))
