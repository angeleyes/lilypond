;;;
;;; lilypond-mode.el --- Major mode for editing GNU LilyPond music scores
;;;
;;; source file of the GNU LilyPond music typesetter
;;;  
;;; (c)  1999--2003 Jan Nieuwenhuizen <janneke@gnu.org>
;;; 
;;; Changed 2001--2003 Heikki Junes <heikki.junes@hut.fi>
;;;    * Add PS-compilation, PS-viewing and MIDI-play (29th Aug 2001)
;;;    * Keyboard shortcuts (12th Sep 2001)
;;;    * Inserting tags, inspired on sgml-mode (11th Oct 2001)
;;;    * Autocompletion & Info (23rd Nov 2002)

;;; Inspired on auctex

;;; Look lilypond-init.el or Documentation/topdocs/INSTALL.texi
;;; for installing instructions.

(require 'easymenu)
(require 'compile)

(defconst LilyPond-version "1.7.30"
  "`LilyPond-mode' version number.")

(defconst LilyPond-help-address "bug-lilypond@gnu.org"
  "Address accepting submission of bug reports.")

(defvar LilyPond-mode-hook nil
  "*Hook called by `LilyPond-mode'.")

(defvar LilyPond-region-file-prefix "emacs-lily"
  "File prefix for commands on buffer or region.")

;; FIXME: find ``\score'' in buffers / make settable?
(defun LilyPond-master-file ()
  ;; duh
  (buffer-file-name))

(defvar LilyPond-kick-xdvi nil
  "If true, no simultaneous xdvi's are started, but reload signal is sent.")

(defvar LilyPond-command-history nil
  "Command history list.")
	
(defvar LilyPond-regexp-alist
  '(("\\([a-zA-Z]?:?[^:( \t\n]+\\)[:( \t]+\\([0-9]+\\)[:) \t]" 1 2))
  "Regexp used to match LilyPond errors.  See `compilation-error-regexp-alist'.")

(defvar LilyPond-imenu nil
  "A flag to tell whether LilyPond-imenu is turned on.")
(make-variable-buffer-local 'LilyPond-imenu)

(defcustom LilyPond-include-path ".:/tmp"
  "* LilyPond include path."
  :type 'string
  :group 'LilyPond)

(defun LilyPond-words-filename ()
  "The file containing LilyPond \keywords \Identifiers and ReservedWords.
Finds file lilypond-words from load-path."
  (let ((fn nil)
	(lp load-path)
	(words-file "lilypond.words"))
    (while (and (> (length lp) 0) (not fn))
      (setq fn (concat (car lp) "/" words-file))
      (if (not (file-readable-p fn)) 
	  (progn (setq fn nil) (setq lp (cdr lp)))))
    (if (not fn)
	(progn (message "Warning: `lilypond.words' not found in `load-path'. See `lilypond-init.el'.")
	       (sit-for 5 0)))
    fn))

(defun LilyPond-add-dictionary-word (x)
  "Contains all words: \keywords \Identifiers and ReservedWords."
  (nconc '(("" . 1)) x))

;; creates dictionary if empty
(if (and (eq (length (LilyPond-add-dictionary-word ())) 1)
	 (not (eq (LilyPond-words-filename) nil)))
    (progn
      (setq b (find-file-noselect (LilyPond-words-filename) t t))
      (setq m (set-marker (make-marker) 1 (get-buffer b)))
      (setq i 1)
      (while (> (buffer-size b) (marker-position m))
	(setq i (+ i 1))
	(setq copy (copy-alist (list (eval (symbol-name (read m))))))
	(setcdr copy i)
	(LilyPond-add-dictionary-word (list copy)))
      (kill-buffer b)))

(defvar LilyPond-insert-tag-current ""
  "The last command selected from the LilyPond-Insert -menu.")

(defconst LilyPond-menu-keywords 
  (let ((wordlist '()) ; add syntax entries to lilypond.words
	(co (all-completions "" (LilyPond-add-dictionary-word ())))
	(currword ""))
    (progn
      (while (> (length co) 0)
	(setq currword (car co))
	(if (string-equal "-" (car (setq co (cdr co))))
	    (progn
	      (add-to-list 'wordlist currword)
	      (while (and (> (length co) 0)
			  (not (string-equal "-" (car (setq co (cdr co))))))))))
      wordlist))
  "Keywords inserted from LilyPond-Insert-menu.")

(defconst LilyPond-keywords
  (let ((wordlist '("\\score")) ; add \keywords to lilypond.words
	(co (all-completions "" (LilyPond-add-dictionary-word ())))
	(currword ""))
    (progn
      (while (> (length co) 0)
	(setq currword (car co))
	(if (> (length currword) 1)
	    (if (and (string-equal "\\" (substring currword 0 1))
		     (not (string-match "[^a-zA-Z]" (substring currword 1)))
		     (string-equal (downcase currword) currword))
		(add-to-list 'wordlist currword)))
	(if (string-equal "-" (car (setq co (cdr co))))
	    (while (and (> (length co) 0)
			(not (string-equal "-" (car (setq co (cdr co)))))))))
      wordlist))
  "LilyPond \\keywords")

(defconst LilyPond-identifiers 
  (let ((wordlist '("\\voiceOne")) ; add \Identifiers to lilypond.words
	(co (all-completions "" (LilyPond-add-dictionary-word ()))))
    (progn
      (while (> (length co) 0)
	(setq currword (car co))
	(if (> (length currword) 1)
	    (if (and (string-equal "\\" (substring currword 0 1))
		     (not (string-match "[^a-zA-Z]" (substring currword 1)))
		     (not (string-equal (downcase currword) currword)))
		(add-to-list 'wordlist currword)))
	(if (string-equal "-" (car (setq co (cdr co))))
	    (while (and (> (length co) 0)
			(not (string-equal "-" (car (setq co (cdr co)))))))))
      wordlist))
  "LilyPond \\Identifiers")

(defconst LilyPond-reserved-words 
  (let ((wordlist '("Staff")) ; add ReservedWords to lilypond.words
	(co (all-completions "" (LilyPond-add-dictionary-word ()))))
    (progn
      (while (> (length co) 0)
	(setq currword (car co))
	(if (> (length currword) 0)
	    (if (not (string-match "[^a-zA-Z]" currword))
		(add-to-list 'wordlist currword)))
	(if (string-equal "-" (car (setq co (cdr co))))
	    (while (and (> (length co) 0)
			(not (string-equal "-" (car (setq co (cdr co)))))))))
      wordlist))
  "LilyPond ReservedWords")

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
  "Check the currently running LilyPond compiling jobs."
  (let ((process-names (list "lilypond" "tex" "2dvi" "2ps" "2midi" 
			     "book" "latex"))
	(running nil))
    (while (setq process-name (pop process-names))
      (setq process (get-process process-name))
      (if (and process 
	       (eq (process-status process) 'run))
	  (push process-name running)))
    running)) ; return the running jobs

(defun LilyPond-midi-running ()
  "Check the currently running Midi processes."
  (let ((process-names (list "midi" "midiall"))
	(running nil))
    (while (setq process-name (pop process-names))
      (setq process (get-process process-name))
      (if (and process 
	       (eq (process-status process) 'run))
	  (push process-name running)))
    running)) ; return the running jobs

(defun LilyPond-kill-jobs ()
  "Kill the currently running LilyPond compiling jobs."
  (interactive)
  (let ((process-names (LilyPond-running))
	(killed nil))
    (while (setq process-name (pop process-names))
      (quit-process (get-process process-name) t)
      (push process-name killed))
    killed)) ; return the killed jobs

(defun LilyPond-kill-midi ()
  "Kill the currently running midi processes."
  (let ((process-names (LilyPond-midi-running))
	(killed nil))
    (while (setq process-name (pop process-names))
      (quit-process (get-process process-name) t)
      (push process-name killed))
    killed)) ; return the killed jobs

;; URG, should only run LilyPond-compile for LilyPond
;; not for tex,xdvi (ly2dvi?)
(defun LilyPond-compile-file (command name)
  ;; We maybe should know what we run here (Lily, ly2dvi, tex)
  ;; and adjust our error-matching regex ?
  (compile-internal
   (if (eq LilyPond-command-current 'LilyPond-command-master)
       command
     ;; use temporary directory for Commands on Buffer/Region
     ;; hm.. the directory is set twice, first to default-dir
     (concat "cd " (LilyPond-temp-directory) "; " command))
   "No more errors" name))

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

(defcustom LilyPond-all-midi-command "timidity -ia"
  "Command used to play MIDI files."

  :group 'LilyPond
  :type 'string)

(defun LilyPond-command-current-midi ()
  "Play midi corresponding to the current document."
  (interactive)
  (LilyPond-command (LilyPond-command-menu "Midi") 'LilyPond-master-file))

(defun LilyPond-command-all-midi ()
  "Play midi corresponding to the current document."
  (interactive)
  (LilyPond-command (LilyPond-command-menu "MidiAll") 'LilyPond-master-file))

(defun count-rexp (start end rexp)
  "Print number of found regular expressions in the region."
  (interactive "r")
  (save-excursion
    (save-restriction
      (narrow-to-region start end)
      (goto-char (point-min))
      (count-matches rexp))))

(defun count-midi-words ()
  "Check number of midi-scores before the curser."
  (if (eq LilyPond-command-current 'LilyPond-command-region)
      (count-rexp (mark t) (point) "\\\\midi")
    (count-rexp (point-min) (point-max) "\\\\midi")))
 
(defun count-midi-words-backwards ()
  "Check number of midi-scores before the curser."
  (if (eq LilyPond-command-current 'LilyPond-command-region)
      (count-rexp (mark t) (point) "\\\\midi")
    (count-rexp (point-min) (point) "\\\\midi")))
 
(defun LilyPond-string-current-midi ()
  "Check the midi file of the following midi-score in the current document."
  (let ((fnameprefix (if (eq LilyPond-command-current 'LilyPond-command-master)
			 (substring (LilyPond-master-file) 0 -3); suppose ".ly"
		       LilyPond-region-file-prefix))
	(allcount (string-to-number (substring (count-midi-words) 0 -12)))
	(count (string-to-number (substring (count-midi-words-backwards) 0 -12))))
    (concat  fnameprefix
	     (if (and (> allcount 1) (> count 0)) ; not first score
		 (if (eq count allcount)          ; last score
		     (concat "-" (number-to-string (+ count -1)))
		   (concat "-" (number-to-string count))))
	     ".midi")))

(defun LilyPond-string-all-midi ()
  "Return the midi files of the current document in ascending order."
  (let ((fnameprefix (if (eq LilyPond-command-current 'LilyPond-command-master)
			 (substring (LilyPond-master-file) 0 -3); suppose ".ly"
		       LilyPond-region-file-prefix))
	(allcount (string-to-number (substring (count-midi-words) 0 -12))))
    (concat (if (> allcount 0)  ; at least one midi-score
		(concat fnameprefix ".midi "))
	    (if (> allcount 1)  ; more than one midi-score
		(concat fnameprefix "-[1-9].midi "))
	    (if (> allcount 9)  ; etc.
		(concat fnameprefix "-[1-9][0-9].midi"))
	    (if (> allcount 99) ; not first score
		(concat fnameprefix "-[1-9][0-9][0-9].midi")))))

;; This is the major configuration variable.
(defcustom LilyPond-command-alist
  ;; Should expand this to include possible keyboard shortcuts which
  ;; could then be mapped to define-key and menu.
  `(
    ("LilyPond" . ("lilypond %s" . "LaTeX"))
    ("TeX" . ("tex '\\nonstopmode\\input %t'" . "View"))

    ("2Dvi" . ("ly2dvi %s" . "View"))
    ("2PS" . ("ly2dvi -P %s" . "ViewPS"))
    ("2Midi" . ("lilypond -m %s" . "View"))

    ("Book" . ("lilypond-book %x" . "LaTeX"))
    ("LaTeX" . ("latex '\\nonstopmode\\input %l'" . "View"))

    ;; point-n-click (arg: exits upop USR1)
    ("SmartView" . ("xdvi %d" . "LilyPond"))

    ;; refreshes when kicked USR1
    ("View" . (,(concat LilyPond-xdvi-command " %d") . "LilyPond"))
    ("ViewPS" . (,(concat LilyPond-gv-command " %p") . "LilyPond"))

    ;; The following are refreshed in LilyPond-command:
    ;; - current-midi depends on cursor position and
    ("Midi" . (,(concat LilyPond-midi-command " " (LilyPond-string-current-midi)) . "LilyPond" )) ; 
    ;; - all-midi depends on number of midi-score.
    ("MidiAll" . (,(concat LilyPond-all-midi-command " " (LilyPond-string-all-midi)) . "LilyPond"))
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
(defcustom LilyPond-file-extension ".ly"
  "*File extension used in LilyPond sources."
  :group 'LilyPond
  :type 'string)


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
  (let* ((default (cond ((if (string-equal name LilyPond-region-file-prefix)
			     (LilyPond-check-files (concat name ".tex")
						   (list name)
						   (list LilyPond-file-extension))
			   (if (verify-visited-file-modtime (current-buffer))
			       (if (buffer-modified-p)
				   (if (y-or-n-p "Save buffer before next command? ")
				       (LilyPond-save-buffer)))
			     (if (y-or-n-p "The command will be invoked to an already saved buffer. Revert it? ")
				 (revert-buffer t t)))
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

(defun LilyPond-command-master ()
  "Run command on the current document."
  (interactive)
  (LilyPond-command-select-master)
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

(defun LilyPond-command-formatmidi ()
  "Format the midi output of the current document."
  (interactive)
  (LilyPond-command (LilyPond-command-menu "2Midi") 'LilyPond-master-file)
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

;; FIXME, this is broken
(defun LilyPond-region-file (begin end)
  (let (
	;; (dir "./")
 	(dir (LilyPond-temp-directory))
	(base LilyPond-region-file-prefix)
	(ext LilyPond-file-extension))
    (concat dir base ext)))

;;; Commands on Region work if there is an appropriate '\score'.
(defun LilyPond-command-region (begin end)
  "Run LilyPond on the current region."
  (interactive "r")
  (if (or (> begin (point-min)) (< end (point-max)))
      (LilyPond-command-select-region))
  (write-region begin end (LilyPond-region-file begin end) nil 'nomsg)
  (LilyPond-command (LilyPond-command-query
		     (LilyPond-region-file begin end))
		    '(lambda () (LilyPond-region-file begin end)))
  ;; Region may deactivate even if buffer was intact, reactivate?
  ;; Currently, also deactived regions are used.
  )

(defun LilyPond-command-buffer ()
  "Run LilyPond on buffer."
  (interactive)
  (LilyPond-command-select-buffer)
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
						(apply file nil)))
	      (jobs nil)
	      (job-string "no jobs"))
	  (if (member name (list "View" "ViewPS"))
	      ;; is USR1 a right signal for viewps?
	      (let ((buffer-xdvi (get-buffer-create (concat "*" name "*"))))
		(if LilyPond-kick-xdvi
		  (let ((process-xdvi (get-buffer-process buffer-xdvi)))
		    (if process-xdvi
			(signal-process (process-id process-xdvi) 'SIGUSR1)
		      (LilyPond-shell-process name buffer-xdvi command)))
		  (LilyPond-shell-process name buffer-xdvi command)))
	    (progn
	      (if (string-equal name "Midi")
		  (progn
		    (setq command (concat LilyPond-midi-command " " (LilyPond-string-current-midi)))
		    (if (LilyPond-kill-midi)
			(setq job-string nil)))) ; either stop or start playing
	      (if (string-equal name "MidiAll")
		  (progn
		    (setq command (concat LilyPond-all-midi-command " " (LilyPond-string-all-midi)))
		    (LilyPond-kill-midi))) ; stop and start playing
	      (if (and (member name (list "Midi" "MidiAll")) job-string)
		  (if (file-newer-than-file-p
		       (LilyPond-master-file)
		       (concat (substring (LilyPond-master-file) 0 -3) ".midi"))
		      (if (y-or-n-p "Midi older than source. Reformat midi?")
			  (progn
			    (LilyPond-command-formatmidi)
			    (while (LilyPond-running)
			      (message "Starts playing midi once it is built.")
			      (sit-for 0 100))))))
	      (if (member name (list "LilyPond" "TeX" "2Midi" "2PS" "2Dvi" 
				     "Book" "LaTeX"))
		  (if (setq jobs (LilyPond-running))
		      (progn
			(setq job-string "Process") ; could also suggest compiling after process has ended
			(while jobs
			  (setq job-string (concat job-string " \"" (pop jobs) "\"")))
			(setq job-string (concat job-string " is already running; kill it to proceed "))
			(if (y-or-n-p job-string)
			    (progn
			      (setq job-string "no jobs")
			      (LilyPond-kill-jobs)
			      (while (LilyPond-running)
				(sit-for 0 100)))
			  (setq job-string nil)))))

	      (setq LilyPond-command-default name)
	      (if (string-equal job-string "no jobs")
		  (LilyPond-compile-file command name))))))))
	  
(defun LilyPond-mark-active ()
  "Check if there is an active mark."
  (and transient-mark-mode
       (if (string-match "XEmacs\\|Lucid" emacs-version) (mark) mark-active)))

(defun LilyPond-temp-directory ()
  "Temporary file directory for Commands on Region."
  (interactive)
  (if (string-match "XEmacs\\|Lucid" emacs-version)
      (concat (temp-directory) "/")
    temporary-file-directory))

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
  ;; Put keys to LilyPond-command-alist and fetch them from there somehow.
  (define-key LilyPond-mode-map "\C-c\C-l" 'LilyPond-command-lilypond)
  (define-key LilyPond-mode-map "\C-c\C-r" 'LilyPond-command-region)
  (define-key LilyPond-mode-map "\C-c\C-b" 'LilyPond-command-buffer)
  (define-key LilyPond-mode-map "\C-c\C-k" 'LilyPond-kill-jobs)
  (define-key LilyPond-mode-map "\C-c\C-c" 'LilyPond-command-master)
  (define-key LilyPond-mode-map "\C-cm" 'LilyPond-command-formatmidi)
  (define-key LilyPond-mode-map "\C-c\C-d" 'LilyPond-command-formatdvi)
  (define-key LilyPond-mode-map "\C-c\C-f" 'LilyPond-command-formatps)
  (define-key LilyPond-mode-map "\C-c\C-s" 'LilyPond-command-smartview)
  (define-key LilyPond-mode-map "\C-c\C-v" 'LilyPond-command-view)
  (define-key LilyPond-mode-map "\C-c\C-p" 'LilyPond-command-viewps)
  (define-key LilyPond-mode-map [(control c) return] 'LilyPond-command-current-midi)
  (define-key LilyPond-mode-map [(control c) (control return)] 'LilyPond-command-all-midi)
  (define-key LilyPond-mode-map "\C-x\C-s" 'LilyPond-save-buffer)
  (define-key LilyPond-mode-map "\C-cf" 'font-lock-fontify-buffer)
  (define-key LilyPond-mode-map "\C-ci" 'LilyPond-insert-tag-current)
  ;; the following will should be overriden by Lilypond Quick Insert Mode
  (define-key LilyPond-mode-map "\C-cq" 'LilyPond-quick-insert-mode)
  (define-key LilyPond-mode-map "\C-c;" 'LilyPond-comment-region)
  (define-key LilyPond-mode-map ")" 'LilyPond-electric-close-paren)
  (define-key LilyPond-mode-map ">" 'LilyPond-electric-close-paren)
  (define-key LilyPond-mode-map "}" 'LilyPond-electric-close-paren)
  (define-key LilyPond-mode-map "]" 'LilyPond-electric-close-paren)
  (if (string-match "XEmacs\\|Lucid" emacs-version)
      (define-key LilyPond-mode-map [iso-left-tab] 'LilyPond-autocompletion)
    (define-key LilyPond-mode-map [iso-lefttab] 'LilyPond-autocompletion))
  (define-key LilyPond-mode-map "\C-c\t" 'LilyPond-info-index-search)
  )

;;; Menu Support

;;; This mode was originally LilyPond-quick-note-insert by Heikki Junes.
;;; The original version has been junked since CVS-1.97,
;;; in order to merge the efforts done by Nicolas Sceaux.
;;; LilyPond Quick Insert Mode is a major mode, toggled by C-c q.
(defun LilyPond-quick-insert-mode ()
  "Insert notes with fewer key strokes by using a simple keyboard piano."
  (interactive)
  (progn 
    (message "Invoke (C-c q) from keyboard. If you still see this message,") (sit-for 5 0)
    (message "then you have not installed LilyPond Quick Insert Mode (lyqi).") (sit-for 5 0)
    (message "Download lyqi from http://nicolas.sceaux.free.fr/lilypond/lyqi.html,") (sit-for 5 0)
    (message "see installation instructions from lyqi's README -file.") (sit-for 5 0)
    (message "You need also eieio (Enhanced Integration of Emacs Interpreted Objects).") (sit-for 5 0)
    (message "Download eieio from http://cedet.sourceforge.net/eieio.shtml,") (sit-for 5 0)
    (message "see installation instructions from eieio's INSTALL -file.") (sit-for 5 0)
    (message "")
    ))    

(defun LilyPond-pre-word-search ()
  "Fetch the alphabetic characters and \\ in front of the cursor."
  (let ((pre "")
	(prelen 0)
	(ch (char-before (- (point) 0))))
    (while (and ch (or (and (>= ch 65) (<= ch 90))  ; not bolp, A-Z
		       (and (>= ch 97) (<= ch 122)) ; a-z
		       (= ch 92)))                  ; \\
      (setq pre (concat (char-to-string ch) pre))
      (setq prelen (+ prelen 1))
      (setq ch (char-before (- (point) prelen))))
    pre))

(defun LilyPond-post-word-search ()
  "Fetch the alphabetic characters behind the cursor."
  (let ((post "")
	(postlen 0)
	(ch (char-after (+ (point) 0))))
    (while (and ch (or (and (>= ch 65) (<= ch 90))    ; not eolp, A-Z
		       (and (>= ch 97) (<= ch 122)))) ; a-z
      (setq post (concat post (char-to-string ch)))
      (setq postlen (+ postlen 1))
      (setq ch (char-after (+ (point) postlen))))
    post))

(defun LilyPond-autocompletion ()
  "Show completions in mini-buffer for the given word."
  (interactive)
  (let ((pre (LilyPond-pre-word-search))
	(post (LilyPond-post-word-search))
	(compsstr ""))
    ;; insert try-completion and show all-completions
    (if (> (length pre) 0)
	(progn
	  (setq trycomp (try-completion pre (LilyPond-add-dictionary-word ())))
	  (if (char-or-string-p trycomp)
	      (if (string-equal (concat pre post) trycomp)
		  (goto-char (+ (point) (length post)))
		(progn
		  (delete-region (point) (+ (point) (length post)))
		  (insert (substring trycomp (length pre) nil))))
	    (progn
	      (delete-region (point) (+ (point) (length post)))
	      (font-lock-fontify-buffer))) ; only inserting fontifies
	
	(setq complist (all-completions pre (LilyPond-add-dictionary-word ())))
	(while (> (length complist) 0)
	  (setq compsstr (concat compsstr "\"" (car complist) "\" "))
	  (setq complist (cdr complist)))
	(message compsstr) 
	(sit-for 0 100)))))

(defun LilyPond-info ()
  "Launch Info for lilypond."
  (interactive)
  (info "lilypond"))
  
(defun LilyPond-music-glossary-info ()
  "Launch Info for music-glossary."
  (interactive)
  (info "music-glossary"))

(defun LilyPond-internals-info ()
  "Launch Info for lilypond-internals."
  (interactive)
  (info "lilypond-internals"))
  
(defun LilyPond-info-index-search ()
  "In `*info*'-buffer, launch `info lilypond --index-search word-under-cursor'"
  (interactive)
  (let ((str (concat (LilyPond-pre-word-search) (LilyPond-post-word-search))))
    (if (and (> (length str) 0) 
	     (string-equal (substring str 0 1) "\\"))
	(setq str (substring str 1 nil)))
    (LilyPond-info)
    (Info-index str)))

(defun LilyPond-insert-tag-current (&optional word)
  "Set the current tag to be inserted."
  (interactive)
  (if word
      (setq LilyPond-insert-tag-current word))
  (if (memq LilyPond-insert-tag-current LilyPond-menu-keywords)
      (LilyPond-insert-tag)
    (message "No tag was selected from LilyPond->Insert tag-menu.")))

(defun LilyPond-insert-tag ()
  "Insert syntax for given tag. The definitions are in lilypond.words."
  (interactive)
  (setq b (find-file-noselect (LilyPond-words-filename) t t))
  (let ((word LilyPond-insert-tag-current)
	(found nil)
	(p nil)
	(query nil)
        (m (set-marker (make-marker) 1 (get-buffer b)))
        (distance (if (LilyPond-mark-active)
		      (abs (- (mark-marker) (point-marker))) 0))
       )
   ;; find the place first
   (if (LilyPond-mark-active)
       (goto-char (min (mark-marker) (point-marker))))
   (while (and (not found) (> (buffer-size b) (marker-position m)))
    (setq copy (car (copy-alist (list (eval (symbol-name (read m)))))))
    (if (string-equal word copy) (setq found t)))
   (if found (insert word))
   (if (> (buffer-size b) (marker-position m))
       (setq copy (car (copy-alist (list (eval (symbol-name (read m))))))))
   (if (not (string-equal "-" copy)) 
       (setq found nil))
   (while (and found (> (buffer-size b) (marker-position m)))
    ;; find next symbol
    (setq copy (car (copy-alist (list (eval (symbol-name (read m)))))))
    ;; check whether it is the word, or the word has been found
    (cond 
     ((string-equal "-" copy) (setq found nil))
     ((string-equal "%" copy) (insert " " (read-string "Give Arguments: ")))
     ((string-equal "_" copy) 
      (progn 
       (setq p (point))
       (goto-char (+ p distance))))
     ((string-equal "\?" copy) (setq query t))
     ((string-equal "\!" copy) (setq query nil))
     ((string-equal "\\n" copy) 
      (if (not query)
       (progn (LilyPond-indent-line) (insert "\n") (LilyPond-indent-line))))
     ((string-equal "{" copy) 
      (if (not query) 
	  (progn (insert " { "))))
     ((string-equal "}" copy)
      (if (not query)
       (progn (insert " } ") (setq query nil) )))
     ((not query)
      (insert copy))
     (query
      (if (y-or-n-p (concat "Proceed with `" copy "'? "))
       (progn (insert copy) (setq query nil))))
   ))
   (if p (goto-char p))
   (kill-buffer b))
)

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
;;;	  (let ((file 'LilyPond-command-on-current))
;;;	    (mapcar 'LilyPond-command-menu-entry LilyPond-command-alist))
;;; Some kind of mapping which includes :keys might be more elegant
;;; Put keys to LilyPond-command-alist and fetch them from there somehow.
	  '([ "LilyPond" LilyPond-command-lilypond t])
	  '([ "TeX" (LilyPond-command (LilyPond-command-menu "TeX") 'LilyPond-master-file) ])
	  '([ "2Dvi" LilyPond-command-formatdvi t])
	  '([ "2PS" LilyPond-command-formatps t])
	  '([ "2Midi" LilyPond-command-formatmidi t])
	  '([ "Book" (LilyPond-command (LilyPond-command-menu "Book") 'LilyPond-master-file) ])
	  '([ "LaTeX" (LilyPond-command (LilyPond-command-menu "LaTeX") 'LilyPond-master-file) ])
	  '([ "Kill jobs" LilyPond-kill-jobs t])
	  '("-----")
	  '([ "SmartView" LilyPond-command-smartview t])
	  '([ "View" LilyPond-command-view t])
	  '([ "ViewPS" LilyPond-command-viewps t])
	  '("-----")
	  '([ "Midi (toggle)" LilyPond-command-current-midi t])
	  '([ "Midi all" LilyPond-command-all-midi t])
	  ))

(defun LilyPond-menu-keywords (arg)
  "Make vector for LilyPond-mode-menu."
  (vector arg (list 'LilyPond-insert-tag-current arg) :style 'radio :selected (list 'eq 'LilyPond-insert-tag-current arg)))

;;; LilyPond-mode-menu should not be interactive, via "M-x LilyPond-<Tab>"
(easy-menu-define LilyPond-mode-menu
  LilyPond-mode-map
  "Menu used in LilyPond mode."
  (append '("LilyPond")
	  '(["Add index menu" LilyPond-add-imenu-menu])
	  (list (cons "Insert tag" 
                (cons ["Previously selected" LilyPond-insert-tag-current t] 
                (cons "-----"
		      (mapcar 'LilyPond-menu-keywords 
			      (reverse LilyPond-menu-keywords))))))
	  '(("Miscellaneous"
	     ["Autocompletion"   LilyPond-autocompletion t]
	     ["(Un)comment Region" LilyPond-comment-region t]
	     ["Refontify buffer" font-lock-fontify-buffer t]
	     "-----"
	     ["Quick Insert Mode"  LilyPond-quick-insert-mode :keys "C-c q"]
 	     ))
	  '(("Info"
	     ["LilyPond" LilyPond-info t]
	     ["LilyPond index-search" LilyPond-info-index-search t]
	     ["Music Glossary" LilyPond-music-glossary-info t]
	     ["LilyPond internals" LilyPond-internals-info t]
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
    (if (eq LilyPond-command-current 'LilyPond-command-region)
	(if (eq (mark t) nil)
	    (progn (message "The mark is not set now") (sit-for 0 500))
	  (progn (if (not (not (LilyPond-mark-active)))
		     (progn (message "Region is not active, using region between inactive mark and current point.") (sit-for 0 500)))
		 (LilyPond-command-region (mark t) (point))))
      (funcall LilyPond-command-current))))

(defun LilyPond-add-imenu-menu ()
  (interactive)
  "Add an imenu menu to the menubar."
  (if (not LilyPond-imenu)
      (progn
	(imenu-add-to-menubar "Index")
	(redraw-frame (selected-frame))
	(setq LilyPond-imenu t))
    (message "%s" "LilyPond-imenu already exists.")))
(put 'LilyPond-add-imenu-menu 'menu-enable '(not LilyPond-imenu))

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

  (LilyPond-mode-set-syntax-table '(?\< ?\> ?\{ ?\}))
  (setq major-mode 'LilyPond-mode)
  (setq mode-name "LilyPond")
  (setq local-abbrev-table LilyPond-mode-abbrev-table)
  (use-local-map LilyPond-mode-map)

  ;; In XEmacs imenu was synched up with: FSF 20.4
  (make-local-variable 'imenu-generic-expression)
  (setq imenu-generic-expression LilyPond-imenu-generic-expression)
  ;; (imenu-add-to-menubar "Index") ; see LilyPond-add-imenu-menu

  ;; In XEmacs one needs to use 'easy-menu-add'.
  (if (string-match "XEmacs\\|Lucid" emacs-version)
      (progn
	(easy-menu-add LilyPond-mode-menu)
	(easy-menu-add LilyPond-command-menu)))

  ;; Use Command on Region even for inactive mark (region).
  (if (string-match "XEmacs\\|Lucid" emacs-version)
      (setq zmacs-regions nil)
    (setq mark-even-if-inactive t))

  ;; Context dependent syntax tables in LilyPond-mode
  (make-local-hook 'post-command-hook) ; XEmacs requires
  (add-hook 'post-command-hook 'LilyPond-mode-context-set-syntax-table nil t)

  ;; Turn on paren-mode buffer-locally, i.e., in LilyPond-mode
  (if (string-match "XEmacs\\|Lucid" emacs-version)
      (progn
	(make-local-variable 'paren-mode)
	(paren-set-mode 'paren)
	(make-local-variable 'blink-matching-paren)
	(setq blink-matching-paren t)
	)
    (progn
      (make-local-variable 'blink-matching-paren-on-screen)
      (setq blink-matching-paren-on-screen t)
     ))

  ;; run the mode hook. LilyPond-mode-hook use is deprecated
  (run-hooks 'LilyPond-mode-hook))

(defun LilyPond-version ()
  "Echo the current version of `LilyPond-mode' in the minibuffer."
  (interactive)
  (message "Using `LilyPond-mode' version %s" LilyPond-version))

(load-library "lilypond-font-lock")
(load-library "lilypond-indent")

(provide 'lilypond-mode)
;;; lilypond-mode.el ends here

