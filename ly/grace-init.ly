\version "1.9.0"

%%
%% TODO: these definitions should be read and executed out dynamically
%%  using an applycontext.
%%

startGraceMusic = {
    \property Voice.Stem \override  #'direction = #1
    \property Voice.Stem \override #'lengths = 
        #(map (lambda (x) (* 0.8 x)) '(2.5 2.5 3.0 4.0 5.0))
    \property Voice.Stem \override #'stem-shorten = 
        #'(0.4 0)
    \property Voice.Stem \override #'beamed-lengths =
        #(map (lambda (x) (* 0.8 x)) '(3.26))
    \property Voice.Stem \override #'beamed-minimum-free-lengths =
        #(map (lambda (x) (* 0.8 x)) '(2.5 2.0 1.5))
    \property Voice.Stem \override #'beamed-extreme-minimum-free-lengths =
        #(map (lambda (x) (* 0.8 x)) '(1.83 1.5))

    \property Voice.Stem \override #'no-stem-extend = ##t
    \property Voice.Stem \override #'stroke-style  = #"grace"
    \property Voice.Beam \override #'thickness = #0.384
    
    %% Instead of calling Beam::space_function, we should invoke
    %% the previously active beam function...
    \property Voice.Beam \override #'space-function =
      #(lambda (beam mult) (* 0.8 (Beam::space_function beam mult)))

    \property Voice.Beam \override #'position-callbacks =
      #`(,Beam::least_squares
	 ,Beam::check_concave
	 ,Beam::slope_damping)
    
    % Can't use Staff.fontSize, since time sigs, keys sigs, etc. will
    % be smaller as well.

    \property Voice.fontSize = #-2
    \property Staff.Accidental \override #'font-relative-size = #-2
    \property Voice.Slur \override #'direction = #-1
}


%% attention: order is significant: reversed from above
%% (consistent ordering reduces memory usage.)
stopGraceMusic = {
    \property Voice.Slur \revert #'direction
    \property Staff.Accidental \revert #'font-relative-size
    \property Voice.Beam \revert #'thickness

    \property Voice.Stem \revert #'stroke-style
    \property Voice.Stem \revert #'no-stem-extend
    \property Voice.Stem \revert #'beamed-extreme-minimum-free-lengths
    \property Voice.Stem \revert #'beamed-minimum-free-lengths
    \property Voice.Stem \revert #'beamed-lengths
    \property Voice.Stem \revert #'stem-shorten
    \property Voice.Stem \revert #'lengths
    \property Voice.Stem \revert #'direction    
    \property Voice.Beam \revert #'space-function
    
    \property Voice.Beam \revert #'position-callbacks
    
    % Can't use Staff.fontSize, since time sigs, keys sigs, etc. will
    % be smaller as well.

    \property Voice.fontSize \unset
}

#(define (add-to-grace-init context object prop val)
   " Override context.object #'property before entering grace context,
and restore afterwards. Use this to add settings to default grace notes.
"
   (set! stopGraceMusic
    (make-sequential-music
     (cons
      (context-spec-music  (make-grob-property-revert object prop) context)
      (ly:get-mus-property stopGraceMusic 'elements)
     )))
   (set! startGraceMusic
    (make-sequential-music
     (append
      (ly:get-mus-property startGraceMusic 'elements)
      (list (context-spec-music  (make-grob-property-set object prop val) context))
      )
     )))
   

