\version "1.7.6"
\header {
  texidoc="Jazz chord names, but with lower case names for minor chords

BROKEN. FIXME.
"
}


%%%
%%  UGH UH GUG UHGUHGHGH GHG
% -- this is fucking up global name space.
%
% imagine doing
% 
%   lilypond Cc-chords.ly ..other-files-testing-chords...
%
% then the other files will get messed up.
%%

%{

FIXME:

%#(assoc-set! chord::names-alist-jazz
%  '((0 . 0) (2 . -1)) '(""))

%#(assoc-set! chord::names-alist-jazz
%   '((0 . 0) (2 . -1) (4 . 0) (6 . -1)) '("7"))

#(define (pitch->chord-name-text-banter pitch additions)
  (let ((name (pitch->text-banter pitch)))
    (if (member '(2 . -1) (map pitch->note-name additions))
	(cons (string-append
	       (string-downcase (substring (car name) 0 1))
	       (substring (car name) 1))
	       (cdr name))
	name)))
%}


\score {
  <
    \property ChordNames.ChordName \set #'style = #'jazz
    \context ChordNames \chords { c1 c:m c:m7 }
  >
}
%% new-chords-done %%
