;;;
;;; lilypond-mode.el --- Major mode for editing GNU LilyPond music scores
;;;
;;; source file of the GNU LilyPond music typesetter
;;;  
;;; (c) 1999--2001 Jan Nieuwenhuizen <janneke@gnu.org>
;;; 
;;; Changed 2001 Heikki Junes <heikki.junes@hut.fi>
;;;    * Add PS-compilation, PS-viewing and MIDI-play (29th Aug 2001)
;;;    * Keyboard shortcuts (12th Sep 2001)
;;;    * Inserting tags, inspired on sgml-mode (11th Oct 2001)
;;;
;;; Changed 2002 Carlos Betancourt <carlos.betancourt@chello.be>
;;;    * Added spanish-note-replacements

;;; Inspired on auctex

;;; Look lilypond-init.el or Documentation/topdocs/INSTALL.texi
;;; for installing instructions.

(load-library "lilypond-font-lock")
(load-library "lilypond-indent")

(require 'easymenu)
(require 'compile)

(defconst LilyPond-version "1.7.8"
  "`LilyPond-mode' version number.")

(defconst LilyPond-help-address "bug-lilypond@gnu.org"
  "Address accepting submission of bug reports.")

(defvar LilyPond-mode-hook nil
  "*Hook called by `LilyPond-mode'.")

(defvar LilyPond-kick-xdvi nil
  "If true, no simultaneous xdvi's are started, but reload signal is sent.")

(defvar LilyPond-command-history nil
  "Command history list.")
	
(defvar LilyPond-regexp-alist
  '(("\\([a-zA-Z]?:?[^:( \t\n]+\\)[:( \t]+\\([0-9]+\\)[:) \t]" 1 2))
  "Regexp used to match LilyPond errors.  See `compilation-error-regexp-alist'.")

(defcustom LilyPond-include-path ".:/tmp"
  "* LilyPond include path."
  :type 'string
  :group 'LilyPond)


(defun LilyPond-check-files (derived originals extensions)
  "Check that DERIVED is newer than any of the ORIGINALS.
Try each original with each member of EXTENSIONS, in all directories
in LilyPond-include-path."
  (let ((found nil)
	(regexp (concat "\\`\\("
			(mapconcat (function (lambda (dir)
				      (regexp-quote (expand-file-name dir))))
				   LilyPond-include-path "\\|")
			"\\).*\\("
			(mapconcat 'regexp-quote originals "\\|")
			"\\)\\.\\("
			(mapconcat 'regexp-quote extensions "\\|")
			"\\)\\'"))
	(buffers (buffer-list)))
    (while buffers
      (let* ((buffer (car buffers))
	     (name (buffer-file-name buffer)))
	(setq buffers (cdr buffers))
	(if (and name (string-match regexp name))
	    (progn
	      (and (buffer-modified-p buffer)
		   (or (not LilyPond-save-query)
		       (y-or-n-p (concat "Save file "
					 (buffer-file-name buffer)
					 "? ")))
		   (save-excursion (set-buffer buffer) (save-buffer)))
	      (if (file-newer-than-file-p name derived)
		  (setq found t))))))
    found))

(defun LilyPond-running ()
  (let ((process (get-process "lilypond")))
  (and process
       (eq (process-status process) 'run))))

(defun Midi-running ()
  (let ((process (get-process "midi")))
  (and process
       (eq (process-status process) 'run))))

(defun LilyPond-kill-job ()
  "Kill the currently running LilyPond job."
  (interactive)
  ;; What bout TeX, Xdvi?
  (quit-process (get-process "lilypond") t))

;; URG, should only run LilyPond-compile for LilyPond
;; not for tex,xdvi (ly2dvi?)
(defun LilyPond-compile-file (command name)
  ;; We maybe should know what we run here (Lily, ly2dvi, tex)
  ;; and adjust our error-matching regex ?
  (compile-internal command "No more errors" name ))

;; do we still need this, now that we're using compile-internal?
(defun LilyPond-save-buffer ()
  "Save buffer and set default command for compiling."
  (interactive)
  (if (buffer-modified-p)
      (progn (save-buffer)
	     (setq LilyPond-command-default "LilyPond"))))

;;; return (dir base ext)
(defun split-file-name (name)
  (let* ((i (string-match "[^/]*$" name))
	 (dir (if (> i 0) (substring name 0 i) "./"))
	 (file (substring name i (length name)))
	 (i (string-match "[^.]*$" file)))
    (if (and
	 (> i 0)
	 (< i (length file)))
	(list dir (substring file 0 (- i 1)) (substring file i (length file)))
      (list dir file ""))))


;; Should check whether in command-alist?
(defcustom LilyPond-command-default "LilyPond"
  "Default command. Must identify a member of LilyPond-command-alist."

  :group 'LilyPond
  :type 'string)
;;;(make-variable-buffer-local 'LilyPond-command-last)

(defvar LilyPond-command-current 'LilyPond-command-master)
;;;(make-variable-buffer-local 'LilyPond-command-master)


;; If non-nil, LilyPond-command-query will return the value of this
;; variable instead of quering the user. 
(defvar LilyPond-command-force nil)

(defcustom LilyPond-xdvi-command "xdvi"
  "Command used to display DVI files."

  :group 'LilyPond
  :type 'string)

(defcustom LilyPond-gv-command "gv -watch"
  "Command used to display PS files."

  :group 'LilyPond
  :type 'string)

(defcustom LilyPond-midi-command "timidity"
  "Command used to play MIDI files."

  :group 'LilyPond
  :type 'string)

(defcustom LilyPond-midi-all-command "timidity -ig"
  "Command used to play MIDI files."

  :group 'LilyPond
  :type 'string)

;; This is the major configuration variable.
(defcustom LilyPond-command-alist
  `(
    ("LilyPond" . ("lilypond %s" . "LaTeX"))
    ("TeX" . ("tex '\\nonstopmode\\input %t'" . "View"))

    ("2Dvi" . ("ly2dvi %s" . "View"))
    ("2PS" . ("ly2dvi -P %s" . "View"))
    ("2Midi" . ("lilypond -m %s" . "View"))

    ("Book" . ("lilypond-book %x" . "LaTeX"))
    ("LaTeX" . ("latex '\\nonstopmode\\input %l'" . "View"))

    ;; point-n-click (arg: exits upop USR1)
    ("SmartView" . ("xdvi %d" . "LilyPond"))
    
    ;; refreshes when kicked USR1
    ("View" . (,(concat LilyPond-xdvi-command " %d") . "LilyPond"))

    ("ViewPS" . (,(concat LilyPond-gv-command " %p") . "LilyPond"))

    ("Midi" . (,(concat LilyPond-midi-command " %m") . "LilyPond"))
    )

  "AList of commands to execute on the current document.

The key is the name of the command as it will be presented to the
user, the value is a cons of the command string handed to the shell
after being expanded, and the next command to be executed upon
success.  The expansion is done using the information found in
LilyPond-expand-list.
"
  :group 'LilyPond
  :type '(repeat (cons :tag "Command Item"
		       (string :tag "Key")
		       (cons :tag "How"
			(string :tag "Command")
			(string :tag "Next Key")))))

;; drop this?
(defcustom LilyPond-file-extensions '(".ly" ".sly" ".fly")
  "*File extensions used by manually generated TeX files."
  :group 'LilyPond
  :type '(repeat (string :format "%v")))


(defcustom LilyPond-expand-alist 
  '(
    ("%s" . ".ly")
    ("%t" . ".tex")
    ("%d" . ".dvi")
    ("%p" . ".ps")
    ("%l" . ".tex")
    ("%x" . ".tely")
    ("%m" . ".midi")
    )
    
  "Alist of expansion strings for LilyPond command names."
  :group 'LilyPond
  :type '(repeat (cons :tag "Alist item"
		  (string :tag "Symbol")
		  (string :tag "Expansion")))) 


(defcustom LilyPond-command-Show "View"
  "*The default command to show (view or print) a LilyPond file.
Must be the car of an entry in `LilyPond-command-alist'."
  :group 'LilyPond
  :type 'string)
  (make-variable-buffer-local 'LilyPond-command-Show)

(defcustom LilyPond-command-Print "Print"
  "The name of the Print entry in LilyPond-command-Print."
  :group 'LilyPond
  :type 'string)

(defun xLilyPond-compile-sentinel (process msg)
  (if (and process
	   (= 0 (process-exit-status process)))
      (setq LilyPond-command-default
	      (cddr (assoc LilyPond-command-default LilyPond-command-alist)))))

;; FIXME: shouldn't do this for stray View/xdvi
(defun LilyPond-compile-sentinel (buffer msg)
  (if (string-match "^finished" msg)
      (setq LilyPond-command-default
	    (cddr (assoc LilyPond-command-default LilyPond-command-alist)))))

;;(make-variable-buffer-local 'compilation-finish-function)
(setq compilation-finish-function 'LilyPond-compile-sentinel)

(defun LilyPond-command-query (name)
  "Query the user for what LilyPond command to use."
  (let* ((default (cond ((if (string-equal name "emacs-lily")
			     (LilyPond-check-files (concat name ".tex")
						   (list name)
						   LilyPond-file-extensions)
			   (if (buffer-modified-p) 
			       (if (y-or-n-p "Save buffer before next command? ")
				   (LilyPond-save-buffer)))
			   ;;"LilyPond"
			   LilyPond-command-default))
			(t LilyPond-command-default)))

         (completion-ignore-case t)
	 
	 (answer (or LilyPond-command-force
		     (completing-read
		      (concat "Command: (default " default ") ")
		      LilyPond-command-alist nil t nil 'LilyPond-command-history))))

    ;; If the answer is "LilyPond" it will not be expanded to "LilyPond"
    (let ((answer (car-safe (assoc answer LilyPond-command-alist))))
      (if (and answer
	       (not (string-equal answer "")))
	  answer
	default))))


;; FIXME: find ``\score'' in buffers / make settable?
(defun LilyPond-master-file ()
  ;; duh
  (buffer-file-name))

(defun LilyPond-command-master ()
  "Run command on the current document."
  (interactive)
  (LilyPond-command (LilyPond-command-query (LilyPond-master-file))
		    'LilyPond-master-file))

(defun LilyPond-command-lilypond ()
  "Run lilypond for the current document."
  (interactive)
  (LilyPond-command (LilyPond-command-menu "LilyPond") 'LilyPond-master-file)
)

(defun LilyPond-command-formatdvi ()
  "Format the dvi output of the current document."
  (interactive)
  (LilyPond-command (LilyPond-command-menu "2Dvi") 'LilyPond-master-file)
)

(defun LilyPond-command-formatps ()
  "Format the ps output of the current document."
  (interactive)
  (LilyPond-command (LilyPond-command-menu "2PS") 'LilyPond-master-file)
)

(defun LilyPond-command-smartview ()
  "View the dvi output of current document."
  (interactive)
  (LilyPond-command (LilyPond-command-menu "SmartView") 'LilyPond-master-file)
)

(defun LilyPond-command-view ()
  "View the dvi output of current document."
  (interactive)
  (LilyPond-command (LilyPond-command-menu "View") 'LilyPond-master-file)
)

(defun LilyPond-command-viewps ()
  "View the ps output of current document."
  (interactive)
  (LilyPond-command (LilyPond-command-menu "ViewPS") 'LilyPond-master-file)
)

(defun LilyPond-command-midi ()
  "Play midi corresponding to the current document."
  (interactive)
  (LilyPond-command (LilyPond-command-menu "Midi") 'LilyPond-master-file)
)

(defun count-rexp (start end rexp)
  "Print number of found regular expressions in the region."
  (interactive "r")
  (save-excursion
    (save-restriction
      (narrow-to-region start end)
      (goto-char (point-min))
      (count-matches rexp))))

(defun count-midi-words ()
  "Print number of scores before the curser."
  (interactive)
  (count-rexp (point-min) (point-max) "\\\\midi"))
 
(defun count-midi-words-backwards ()
  "Print number of scores before the curser."
  (interactive)
  (count-rexp (point-min) (point) "\\\\midi"))
 
(defun LilyPond-command-next-midi ()
  "Play next midi score of the current document."
  (interactive)
  (if (Midi-running)
      (quit-process (get-process "midi") t)
    (LilyPond-compile-file 
     (let ((fname (LilyPond-master-file))
	   (allcount (string-to-number (substring (count-midi-words) 0 -12)))
	   (count (string-to-number (substring (count-midi-words-backwards) 0 -12))))
       (concat  LilyPond-midi-command " "
		(substring fname 0 -3) ; suppose ".ly"
		(if (and (> allcount 1) (> count 0)) ; not first score
		    (if (eq count allcount)          ; last score
			(concat "-" (number-to-string (+ count -1)))
		      (concat "-" (number-to-string count))))
		".midi"))
     "Midi")))

(defun LilyPond-command-all-midi ()
  "Play next midi score of the current document."
  (interactive)
  (if (Midi-running)
      (quit-process (get-process "midi") t)
    (LilyPond-compile-file 
     (let ((fname (LilyPond-master-file))
	   (allcount (string-to-number (substring (count-midi-words) 0 -12))))
       (concat  LilyPond-midi-all-command " "
		(if (> allcount 0) ; at least one midi-score
		    (concat (substring fname 0 -3) ".midi "))
		(if (> allcount 1) ; more than one midi-score
		    (concat (substring fname 0 -3) "-?.midi "))
 		(if (> allcount 9) ; etc.
		    (concat (substring fname 0 -3) "-??.midi"))
 		(if (> allcount 99) ; not first score
		    (concat (substring fname 0 -3) "-???.midi"))))
     "Midi")))

(defun LilyPond-un-comment-region (start end level)
  "Remove up to LEVEL comment characters from each line in the region."
  (interactive "*r\np") 
  (comment-region start end (- level)))

;; FIXME, this is broken
(defun LilyPond-region-file (begin end)
  (let (
	;; (dir "/tmp/")
	;; urg
	(dir "./")
	(base "emacs-lily")
	;; Hmm
	(ext (if (string-match "^[\\]score" (buffer-substring begin end))
		 ".ly"
	       (if (< 50 (abs (- begin end)))
		   ".fly"
		 ".sly"))))
    (concat dir base ext)))

(defun LilyPond-command-region (begin end)
  "Run LilyPond on the current region."
  (interactive "r")
  (write-region begin end (LilyPond-region-file begin end) nil 'nomsg)
  (LilyPond-command (LilyPond-command-query
		     (LilyPond-region-file begin end))
		    '(lambda () (LilyPond-region-file begin end))))

(defun LilyPond-command-buffer ()
  "Run LilyPond on buffer."
  (interactive)
  (LilyPond-command-region (point-min) (point-max)))

(defun LilyPond-command-expand (string file)
  (let ((case-fold-search nil))
    (if (string-match "%" string)
	(let* ((b (match-beginning 0))
	       (e (+ b 2))
	       (l (split-file-name file))
	       (dir (car l))
	       (base (cadr l)))
	  (LilyPond-command-expand
	   (concat (substring string 0 b)
		   dir
		   base
		   (let ((entry (assoc (substring string b e)
				       LilyPond-expand-alist)))
		     (if entry (cdr entry) ""))
		   (substring string e))
	   file))
      string)))

(defun LilyPond-shell-process (name buffer command)
  (let ((old (current-buffer)))
    (switch-to-buffer-other-window buffer)
    ;; If we empty the buffer don't see messages scroll by.
    ;; (erase-buffer)
    
    (start-process-shell-command name buffer command)
    (switch-to-buffer-other-window old)))
  

(defun LilyPond-command (name file)
  "Run command NAME on the file you get by calling FILE.

FILE is a function return a file name.  It has one optional argument,
the extension to use on the file.

Use the information in LilyPond-command-alist to determine how to run the
command."
  
  (let ((entry (assoc name LilyPond-command-alist)))
    (if entry
	(let ((command (LilyPond-command-expand (cadr entry)
						(apply file nil))))
	  (if (string-equal name "View")
	      (let ((buffer-xdvi (get-buffer-create "*view*")))
		(if LilyPond-kick-xdvi
		  (let ((process-xdvi (get-buffer-process buffer-xdvi)))
		    (if process-xdvi
			(signal-process (process-id process-xdvi) 'SIGUSR1)
		      (LilyPond-shell-process name buffer-xdvi command)))
		  (LilyPond-shell-process name buffer-xdvi command)))
	    (progn
	      (setq LilyPond-command-default name)
	      (LilyPond-compile-file command name)))))))
	  
;; XEmacs stuff
;; Sadly we need this for a macro in Emacs 19.
(eval-when-compile
  ;; Imenu isn't used in XEmacs, so just ignore load errors.
  (condition-case ()
      (require 'imenu)
    (error nil)))


;;; Keymap

(defvar LilyPond-mode-map ()
  "Keymap used in `LilyPond-mode' buffers.")

;; Note:  if you make changes to the map, you must do
;;    M-x set-variable LilyPond-mode-map nil
;;    M-x eval-buffer
;;    M-x LilyPond-mode
;; to let the changest take effect

(if LilyPond-mode-map
    ()
  (setq LilyPond-mode-map (make-sparse-keymap))
  (define-key LilyPond-mode-map "\C-c\C-l" 'LilyPond-command-lilypond)
  (define-key LilyPond-mode-map "\C-c\C-r" 'LilyPond-command-region)
  (define-key LilyPond-mode-map "\C-c\C-b" 'LilyPond-command-buffer)
  (define-key LilyPond-mode-map "\C-c\C-k" 'LilyPond-kill-job)
  (define-key LilyPond-mode-map "\C-c\C-c" 'LilyPond-command-master)
  (define-key LilyPond-mode-map "\C-c\C-d" 'LilyPond-command-formatdvi)
  (define-key LilyPond-mode-map "\C-c\C-f" 'LilyPond-command-formatps)
  (define-key LilyPond-mode-map "\C-c\C-s" 'LilyPond-command-smartview)
  (define-key LilyPond-mode-map "\C-c\C-v" 'LilyPond-command-view)
  (define-key LilyPond-mode-map "\C-c\C-p" 'LilyPond-command-viewps)
  (define-key LilyPond-mode-map "\C-c\C-m" 'LilyPond-command-next-midi)
  (define-key LilyPond-mode-map "\C-x\C-s" 'LilyPond-save-buffer)
  (define-key LilyPond-mode-map "\C-cf" 'font-lock-fontify-buffer)
  (define-key LilyPond-mode-map "\C-ci" 'LilyPond-quick-note-insert)
  (define-key LilyPond-mode-map "\C-cn" 'LilyPond-insert-tag-notes)
  (define-key LilyPond-mode-map "\C-cs" 'LilyPond-insert-tag-score)
  (define-key LilyPond-mode-map "\C-c:" 'LilyPond-un-comment-region)
  (define-key LilyPond-mode-map "\C-c;" 'comment-region)
  (define-key LilyPond-mode-map ")" 'LilyPond-electric-close-paren)
  (define-key LilyPond-mode-map ">" 'LilyPond-electric-close-paren)
  (define-key LilyPond-mode-map "}" 'LilyPond-electric-close-paren)
  )

;;; Menu Support

(defun LilyPond-quick-note-insert()
  "Insert notes with fewer key strokes by using a simple keyboard piano."
  (interactive)
  (setq dutch-notes
	'(("k" "a") ("l" "b") ("a" "c") ("s" "d") 
	  ("d" "e") ("f" "f") ("j" "g") ("r" "r")))
  (setq dutch-note-ends '("eses" "es" "" "is" "isis"))
  (setq dutch-note-replacements '("" ""))
  (setq finnish-note-replacements
	'(("eeses" "eses") ("ees" "es") ("aeses" "asas") ("aes" "as") ("b" "h")
	  ("beses" "heses") ("bes" "b") ("bis" "his") ("bisis" "hisis")))
			      ; add more translations of the note names
  (setq spanish-note-replacements
	'(("c" "do") ("d" "re") ("e" "mi") ("f" "fa") ("g" "sol") ("a" "la") ("b" "si")
      ("cis" "dos") ("cisis" "doss") ("ces" "dob") ("ceses" "dobb")
      ("dis" "res") ("disis" "ress") ("des" "reb") ("deses" "rebb")
      ("eis" "mis") ("eisis" "miss") ("ees" "mib") ("eeses" "mibb")
      ("fis" "fas") ("fisis" "fass") ("fes" "fab") ("feses" "fabb")
      ("gis" "sols") ("gisis" "solss") ("ges" "solb") ("geses" "solbb")
      ("ais" "las") ("aisis" "lass") ("aes" "lab") ("aeses" "labb")
      ("bis" "sis") ("bisis" "siss") ("bes" "sib") ("beses" "sibb")))
  (setq other-keys "()<>~}")
  (setq accid 0) (setq octav 0) (setq durat "") (setq dots 0)

  (message "Press h for help.") (sit-for 0 750 1)

  (setq note-replacements dutch-note-replacements)
  (while (not (= 27 ; esc to quit
    (setq x (read-char-exclusive 
	     (format " | a[_]s[_]d | f[_]j[_]k[_]l | r with ie ,' 12345678 . 0 (<~>)/}\\b\\n Esc \n | c | d | e | f | g | a | %s | r with %s%s%s%s"
		     (if (string= (car(cdr(assoc "b" note-replacements))) "h")
			 "h" "b")
		     (nth (+ accid 2) dutch-note-ends)
		     (make-string (abs octav) (if (> octav 0) ?' ?,)) 
		     durat 
		     (if (string= durat "") "" (make-string dots ?.)))))))
;    (insert (number-to-string x)) ; test numbers for characters
    (setq note (cdr (assoc (char-to-string x) dutch-notes)))
    (cond
     ((= x 127) (backward-kill-word 1)) ; backspace
     ((= x 13) (progn (insert "\n") (LilyPond-indent-line)))) ; return
    (setq x (char-to-string x))
    (cond
     ((and (string< x "9") (string< "0" x))
      (progn (setq durat (int-to-string (expt 2 (- (string-to-int x) 1))))
	     (setq dots 0)))
     ((string= x " ") (insert " "))
     ((string= x "/") (progn (insert "\\times ")
			     (while (not (and (string< x "9") (string< "0" x)))
			       (setq x (char-to-string (read-char-exclusive "Insert a number for the denominator (\"x/\")"))))
			     (insert (format "%s/" x)) (setq x "/")
			     (while (not (and (string< x "9") (string< "0" x)))
			       (setq x (char-to-string (read-char-exclusive "Insert a number for the numerator (\"/y\")"))))
			     (insert (format "%s { " x))))
     ((string= x "0") (progn (setq accid 0) (setq octav 0) 
			     (setq durat "") (setq dots 0)))
     ((string= x "i") (setq accid (if (= accid 2) 0 (max (+ accid 1) 1))))
     ((string= x "e") (setq accid (if (= accid -2) 0 (min (+ accid -1) -1))))
     ((string= x "'") (setq octav (if (= octav 4) 0 (max (+ octav 1) 1))))
     ((string= x ",") (setq octav (if (= octav -4) 0 (min (+ octav -1) -1))))
     ((string= x ".") (setq dots (if (= dots 4) 0 (+ dots 1))))
     ((not (null (member x (split-string other-keys ""))))
      (insert (format "%s " x)))
     ((not (null note))
      (progn
	(setq note 
	      (format "%s%s" (car note) (if (string= "r" (car note)) "" 
					  (nth (+ accid 2) dutch-note-ends))))
	(setq notetwo (car (cdr (assoc note note-replacements))))
	(if (not (null notetwo)) (setq note notetwo))
	(insert
	 (format "%s%s%s%s " 
		 note
		 (if (string= "r" note) ""
		     (make-string (abs octav) (if (> octav 0) ?' ?,)))
		 durat
		 (if (string= durat "") "" (make-string dots ?.))))
	(setq accid 0) (setq octav 0) (setq durat "") (setq dots 0)))
     ((string= x "t") (progn (setq note-replacements dutch-note-replacements)
			     (message "Selected Dutch notes") 
			     (sit-for 0 750 1))) ; t
     ((string= x "n") (progn (setq note-replacements finnish-note-replacements)
			     (message "Selected Finnish/Deutsch notes") 
			     (sit-for 0 750 1))) ; n
			      ; add more translations of the note names
     ((string= x "p") (progn (setq note-replacements spanish-note-replacements)
			     (message "Selected Spanish notes") 
			     (sit-for 0 750 1))) ; p
     ((string= x "h") 
      (progn (message "Insert notes with fewer key strokes. For example \"i,5.f\" produces \"fis,32. \".") (sit-for 5 0 1) 
	     (message "Add also \"a ~ a\"-ties, \"a ( ) b\"-slurs and \"< a b >\"-chords.") (sit-for 5 0 1) 
	     (message "Note names are in Du(t)ch by default. Hit 'n' for Fin(n)ish/Deutsch note names. Hit 'p' for S(p)anish note names") (sit-for 5 0 1) 
	     (message "Backspace deletes last note, return starts a new indented line and Esc quits.") (sit-for 5 0 1) 
	     (message "Insert note triplets \"\\times 2/3 { a b } \" by typing \"/23ab}\".") (sit-for 5 0 1) 
	     (message "Remember to add all other details as well.") (sit-for 5 0 1)))
    )))

(defun LilyPond-insert-string (text pre post)
  "Insert text to the buffer if non-empty string is given."
  (interactive)
  (let ((str (read-string text)))
    (if (string-equal str "")
	nil
      (progn (insert pre str post)
	     t))))
 
(define-skeleton LilyPond-insert-tag-notes
  "LilyPond notes tag."
  nil
;  (if (bolp) nil ?\n)
  (progn
    (insert "\\notes ")
    (if (not (LilyPond-insert-string "Relative: " "\\relative " " "))
	(LilyPond-insert-string "Transpose: " "\\transpose " " "))
    ())
  "{ " _ " }")

(define-skeleton LilyPond-insert-tag-score
  "LilyPond score tag."
  nil
  (if (bolp) nil ?\n)
  "\\score {\n"
  "   " _ "\n"
  "   \\paper {  }\n"
  (if (y-or-n-p "Insert \"\\header\" field? ")
      (concat "   \\header {\n      " 
	      (skeleton-read "Piece: " "piece = " str) "\n"
	      (if (y-or-n-p "Insert \"opus\" field? ")
		  (concat "      " (skeleton-read "Opus: " "opus = " str) "\n"))
	      "   }\n"))
  (if (y-or-n-p "Insert \"\\midi\" field? ")
      (concat "   \\midi { " 
	      (skeleton-read "Midi: " "\\tempo 4 = " str)  
	      " }\n"))
  "}\n")

(defun LilyPond-command-menu-entry (entry)
  ;; Return LilyPond-command-alist ENTRY as a menu item.
  (let ((name (car entry)))
    (cond ((and (string-equal name LilyPond-command-Print)
		LilyPond-printer-list)
	   (let ((command LilyPond-print-command)
		 (lookup 1))
	     (append (list LilyPond-command-Print)
		     (mapcar 'LilyPond-command-menu-printer-entry
			     LilyPond-printer-list))))
	  (t
	   (vector name (list 'LilyPond-command-menu name) t)))))


(easy-menu-define LilyPond-command-menu
    LilyPond-mode-map
    "Menu used in LilyPond mode."
  (append '("Command")
	  '(("Command on"
	     [ "Master File" LilyPond-command-select-master
	       :keys "C-c C-c" :style radio
	       :selected (eq LilyPond-command-current 'LilyPond-command-master) ]
	     [ "Buffer" LilyPond-command-select-buffer
	       :keys "C-c C-b" :style radio
	       :selected (eq LilyPond-command-current 'LilyPond-command-buffer) ]
	     [ "Region" LilyPond-command-select-region
	       :keys "C-c C-r" :style radio
	       :selected (eq LilyPond-command-current 'LilyPond-command-region) ]))
;	  (let ((file 'LilyPond-command-on-current))
;	    (mapcar 'LilyPond-command-menu-entry LilyPond-command-alist))
;;; Some kind of mapping which includes :keys might be more elegant
	  '([ "LilyPond" (LilyPond-command (LilyPond-command-menu "LilyPond") 'LilyPond-master-file) :keys "C-c C-l"])
	  '([ "TeX" (LilyPond-command (LilyPond-command-menu "TeX") 'LilyPond-master-file) ])
	  '([ "2Dvi" (LilyPond-command (LilyPond-command-menu "2Dvi") 'LilyPond-master-file) :keys "C-c C-d"])
	  '([ "2PS" (LilyPond-command (LilyPond-command-menu "2PS") 'LilyPond-master-file) :keys "C-c C-f"])
	  '([ "2Midi" (LilyPond-command (LilyPond-command-menu "2Midi") 'LilyPond-master-file)])
	  '([ "Book" (LilyPond-command (LilyPond-command-menu "Book") 'LilyPond-master-file) ])
	  '([ "LaTeX" (LilyPond-command (LilyPond-command-menu "LaTeX") 'LilyPond-master-file) ])
	  '([ "SmartView" (LilyPond-command (LilyPond-command-menu "SmartView") 'LilyPond-master-file) :keys "C-c C-s"])
	  '([ "View" (LilyPond-command (LilyPond-command-menu "View") 'LilyPond-master-file) :keys "C-c C-v"])
	  '([ "ViewPS" (LilyPond-command (LilyPond-command-menu "ViewPS") 'LilyPond-master-file) :keys "C-c C-p"])
	  '([ "Midi (off)" (LilyPond-command-next-midi) :keys "C-c C-m"])
	  '([ "Midi all" (LilyPond-command-all-midi)])
	  ))

(easy-menu-define LilyPond-mode-menu
  LilyPond-mode-map
  "Menu used in LilyPond mode."
  (append '("LilyPond")
	  '(("Insert"
	     [ "\\notes..."  LilyPond-insert-tag-notes t]
	     [ "\\score..."  LilyPond-insert-tag-score t]
	     ["Quick Notes"  LilyPond-quick-note-insert t]
	     ))
	  '(("Miscellaneous"
	     ["Uncomment Region" LilyPond-un-comment-region t]
	     ["Comment Region" comment-region t]
	     ["Refontify buffer" font-lock-fontify-buffer t]
 	     ))
	  ))

(defconst LilyPond-imenu-generic-re "^\\([a-zA-Z]+\\) *="
  "Regexp matching Identifier definitions.")

(defvar LilyPond-imenu-generic-expression
  (list (list nil LilyPond-imenu-generic-re 1))
  "Expression for imenu")

(defun LilyPond-command-select-master ()
  (interactive)
  (message "Next command will be on the master file")
  (setq LilyPond-command-current 'LilyPond-command-master))

(defun LilyPond-command-select-buffer ()
  (interactive)
  (message "Next command will be on the buffer")
  (setq LilyPond-command-current 'LilyPond-command-buffer))

(defun LilyPond-command-select-region ()
  (interactive)
  (message "Next command will be on the region")
  (setq LilyPond-command-current 'LilyPond-command-region))

(defun LilyPond-command-menu (name)
  ;; Execute LilyPond-command-alist NAME from a menu.
  (let ((LilyPond-command-force name))
    (funcall LilyPond-command-current)))

(defun LilyPond-mode ()
  "Major mode for editing LilyPond music files.

This mode knows about LilyPond keywords and line comments, not about
indentation or block comments.  It features easy compilation, error
finding and viewing of a LilyPond source buffer or region.

COMMANDS
\\{LilyPond-mode-map}
VARIABLES

LilyPond-command-alist\t\talist from name to command
LilyPond-xdvi-command\t\tcommand to display dvi files -- bit superfluous"
  (interactive)
  ;; set up local variables
  (kill-all-local-variables)

  (make-local-variable 'font-lock-defaults)
  (setq font-lock-defaults '(LilyPond-font-lock-keywords))

  ;; string and comments are fontified explicitly
  (make-local-variable 'font-lock-keywords-only)
  (setq font-lock-keywords-only t)

  ;; Multi-line font-locking needs Emacs 21.1 or newer.
  ;; For older versions there is hotkey "C-c f".
  (make-local-variable 'font-lock-multiline) 
  (setq font-lock-multiline t) 

  (make-local-variable 'paragraph-separate)
  (setq paragraph-separate "^[ \t]*$")

  (make-local-variable 'paragraph-start)
  (setq	paragraph-start "^[ \t]*$")

  (make-local-variable 'comment-start)
  (setq comment-start "%")

  (make-local-variable 'comment-start-skip)
  (setq comment-start-skip "%{? *")

  (make-local-variable 'comment-end)
  (setq comment-end "")

  (make-local-variable 'block-comment-start)
  (setq block-comment-start "%{")

  (make-local-variable 'block-comment-end)  
  (setq block-comment-end   "%}")

  (make-local-variable 'indent-line-function)
  (setq indent-line-function 'LilyPond-indent-line)

    (set-syntax-table LilyPond-mode-syntax-table)
  (setq major-mode 'LilyPond-mode)
  (setq mode-name "LilyPond")
  (setq local-abbrev-table LilyPond-mode-abbrev-table)
  (use-local-map LilyPond-mode-map)

  (make-local-variable 'imenu-generic-expression)
  (setq imenu-generic-expression LilyPond-imenu-generic-expression)
  (imenu-add-to-menubar "Index")

    ;; run the mode hook. LilyPond-mode-hook use is deprecated
  (run-hooks 'LilyPond-mode-hook))

(defun LilyPond-version ()
  "Echo the current version of `LilyPond-mode' in the minibuffer."
  (interactive)
  (message "Using `LilyPond-mode' version %s" LilyPond-version))

(provide 'lilypond-mode)
;;; lilypond-mode.el ends here

