\version "1.7.18"
\header {

    texidoc = "Check whether all @code{duration} properties can 
be printed. This once helped track down an obscure memory corruption
bug.  "

}



#(define (duration-check music)
  "Check all rest durations in MUSIC"

  (let* ((name (ly:music-name music))
	 (elts (ly:get-mus-property music 'elements))
         (e (ly:get-mus-property music 'element))
         (duration (ly:get-mus-property music 'duration)))

    (if (not (equal? '() duration))
	(begin
	  (write "duration: " (current-error-port))
	  (write duration (current-error-port))
	  (newline (current-error-port)))
	
	(if (equal? name "Rest_req")
	    (begin
	      (write "URG" (current-error-port))
	      (write name (current-error-port))
	      (write music (current-error-port))
	      (newline (current-error-port))
	      ;;(scm-error 'system-error #f "urg" #f #f)))
	      (error "boo")
	      )))

    (if (pair? elts)
	(map duration-check elts))
    
    (if (ly:music? e)
	  (duration-check e)))

  music)

\score {
  \apply #duration-check
  <
      \notes\relative c'' {
	  a b8 c16 <<d e>>4.
      }
      \notes\relative c'' {
	  a b8 c16 d a2 r
      }
  >
}
%% new-chords-done %%
