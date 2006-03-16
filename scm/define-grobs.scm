;;;; define-grobs.scm --
;;;;
;;;;  source file of the GNU LilyPond music typesetter
;;;;
;;;; (c) 1998--2006 Han-Wen Nienhuys <hanwen@cs.uu.nl>
;;;;                 Jan Nieuwenhuizen <janneke@gnu.org>

;;;; distances are given in line-thickness (thicknesses) and
;;;; staff-space (distances)

;;;; WARNING: the meta field should be the last one.
;;;; WARNING: don't use anonymous functions for initialization.

;; TODO: junk the meta field in favor of something more compact?

;;; todo:: reorder sensibly.

(define-public all-grob-descriptions
  `(
    (Accidental
     . (
	(avoid-slur . inside)
	(cautionary-style . parentheses)
	(stencil . ,ly:accidental-interface::print)
	(after-line-breaking
	 . ,ly:accidental-interface::after-line-breaking)
	(meta . ((class . Item)
		 (interfaces . (accidental-interface
				font-interface))))))
    
    (AccidentalSuggestion
     . (
	(stencil . ,ly:accidental-interface::print)
	(X-offset . ,(ly:make-simple-closure
		      `(,+
			,(ly:make-simple-closure (list ly:self-alignment-interface::centered-on-x-parent))
			,(ly:make-simple-closure (list ly:self-alignment-interface::x-aligned-on-self)))))
	(self-alignment-X . ,CENTER)
	(cautionary . #t)
	(cautionary-style . smaller)
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(cautionary-style . parentheses)
	(direction . ,UP)
	(staff-padding . 0.25)
	(script-priority . 0)
	(side-axis . ,X)
	(meta . ((class . Item)
		 (interfaces . (side-position-interface
				script-interface
				accidental-interface
				accidental-suggestion-interface
				self-alignment-interface
				font-interface))))))
    (AccidentalPlacement
     . (
	(left-padding . 0.2)

	(positioning-done . ,ly:accidental-placement::calc-positioning-done)
	(X-extent . ,ly:axis-group-interface::width)		      
	
	;; this is quite small, but it is very ugly to have
	;; accs closer to the previous note than to the next one.
	(right-padding . 0.15)
	(meta . ((class . Item)
		 (interfaces . (accidental-placement-interface))))))
    (Ambitus
     . (
	(axes . (0 1))
	(X-extent . ,ly:axis-group-interface::width)
	(X-extent . ,ly:axis-group-interface::height)
	(space-alist . (
			(clef . (extra-space . 0.5))
			(key-signature . (extra-space . 0.0))
			(staff-bar . (extra-space . 0.0))
			(time-signature . (extra-space . 0.0))
			(first-note . (fixed-space . 0.0))))
	(breakable . #t)
	(break-align-symbol . ambitus)
	(break-visibility . ,begin-of-line-visible)
	(meta . ((class . Item)
		 (interfaces . (axis-group-interface
				break-aligned-interface
				ambitus-interface))))))

    (AmbitusLine
     . (
	(stencil . ,ly:ambitus::print)
	(join-heads . #t)
	(thickness . 2)
	(X-offset . ,ly:self-alignment-interface::centered-on-x-parent)
	(meta . ((class . Item)
		 (interfaces . (ambitus-interface
				staff-symbol-referencer-interface
				font-interface))))))
    (AmbitusAccidental
     . (
	(font-family . music)
	(padding . 0.5)
	(X-offset . ,ly:side-position-interface::x-aligned-side)
	(direction . ,LEFT)
	(cautionary-style . parentheses)
	(stencil . ,ly:accidental-interface::print)
	(after-line-breaking . ,ly:accidental-interface::after-line-breaking)
	(side-axis . ,X)
	(meta . ((class . Item)
		 (interfaces . (item-interface
				accidental-interface
				break-aligned-interface
				side-position-interface
				font-interface))))))

    (AmbitusNoteHead
     . (
	(duration-log . 2)
	(stencil . ,ly:note-head::print)
	(glyph-name . ,note-head::calc-glyph-name)
	(Y-offset . ,ly:staff-symbol-referencer::callback)
	(meta . ((class . Item)
		 (interfaces . (font-interface
				note-head-interface
				ambitus-interface
				staff-symbol-referencer-interface
				rhythmic-head-interface
				ledgered-interface))))))

    (Arpeggio
     . ((X-extent . ,ly:arpeggio::width)
	(stencil . ,ly:arpeggio::print)
	(Y-offset . ,ly:staff-symbol-referencer::callback)
	(X-offset . ,ly:side-position-interface::x-aligned-side)
	(direction . ,LEFT)
	(padding . 0.5)
	(side-axis . ,X)
	(staff-position . 0.0)
	(meta . ((class . Item)
		 (interfaces . (arpeggio-interface
				staff-symbol-referencer-interface
				side-position-interface
				font-interface))))))

    (BarLine
     . (
	(break-align-symbol . staff-bar)
	(glyph . "|")
	(layer . 0)
	(break-visibility . ,all-visible)
	(breakable . #t)

	(stencil . ,ly:bar-line::print)
	(glyph-name . ,bar-line::calc-glyph-name)
	(bar-size .  ,ly:bar-line::calc-bar-size)
	
	(space-alist . (
			(time-signature . (extra-space . 0.75))
			(custos . (minimum-space . 2.0))
			(clef . (minimum-space . 1.0))
			(key-signature . (extra-space . 1.0))
			(key-cancellation . (extra-space . 1.0))
			(first-note . (fixed-space . 1.3))
			(next-note . (semi-fixed-space . 1.3))
			(right-edge . (extra-space . 0.0))))

	;;
	;; Ross. page 151 lists other values, we opt for a leaner look
	;;
	;; TODO:
	;; kern should scale with line-thickness too.
	(kern . 3.0)
	(thin-kern . 3.0)
	(hair-thickness . 1.9)
	(thick-thickness . 6.0)
	(meta . ((class . Item)
		 (interfaces . (bar-line-interface
				break-aligned-interface
				font-interface))))))

    (BarNumber
     . (
	(stencil . ,ly:text-interface::print)
	(breakable . #t)
	(break-visibility . ,begin-of-line-visible)
	(padding . 1.0)
	(direction . ,UP)
	(font-family . roman)
	(font-size . -2)
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(side-axis . ,Y)
	(X-offset . ,ly:self-alignment-interface::x-aligned-on-self)
	(self-alignment-X . 1)

	(meta .
	      ((class . Item)
	       (interfaces . (side-position-interface
			      text-interface
			      self-alignment-interface
			      font-interface
			      break-aligned-interface))))
	))

    (BassFigure
     . (
	(stencil . ,ly:text-interface::print)
	(meta . ((class . Item)
		 (interfaces . (text-interface
				rhythmic-grob-interface
				bass-figure-interface
				font-interface))))))
    (BassFigureBracket
     . (
	(stencil . ,ly:enclosing-bracket::print)
	(X-extent . ,ly:enclosing-bracket::width)
	(edge-height . (0.2 . 0.2))
	(meta . ((class . Item)
		 (interfaces . (enclosing-bracket-interface)) ))
	))
    (BassFigureContinuation
     . (
	(stencil . ,ly:figured-bass-continuation::print)
	(Y-offset . ,ly:figured-bass-continuation::center-on-figures)
	(meta . ((class . Spanner)
		 (interfaces . (figured-bass-continuation-interface))
		 ))))
    (BassFigureLine
     . (
	(axes . (,Y))
	(Y-extent . ,ly:axis-group-interface::height)
	(meta . ((class . Spanner)
		 (interfaces . (axis-group-interface
				))))))

    (BassFigureAlignment
     . (
	(axes . (,Y))
	(threshold . (2 . 1000))
	(positioning-done . ,ly:align-interface::calc-positioning-done)
	(Y-extent . ,ly:axis-group-interface::height)
	(stacking-dir . -1)
	(meta . ((class . Spanner)
		 (interfaces . (align-interface
				axis-group-interface))))))

    (Beam
     . (
	;; todo: clean this up a bit: the list is getting
	;; rather long.
	(gap . 0.8)
	(positions .  ,(ly:make-simple-closure
			(ly:make-simple-closure
			 (list chain-grob-member-functions
			   `(,cons 0 0)
			   ly:beam::calc-least-squares-positions
			   ly:beam::slope-damping
			   ly:beam::shift-region-to-valid
			   ly:beam::quanting
			   ))))

	;; this is a hack to set stem lengths, if positions is set.
	(quantized-positions . ,ly:beam::set-stem-lengths)
	(concaveness . ,ly:beam::calc-concaveness)
	(direction . ,ly:beam::calc-direction)
	(shorten . ,ly:beam::calc-stem-shorten)
	(beaming . ,ly:beam::calc-beaming)
	(stencil . ,ly:beam::print)

	;; TODO: should be in SLT.
	(thickness . 0.48) ; in staff-space
	(neutral-direction . ,DOWN)

	;; Whe have some unreferenced problems here.
	;;
	;; If we shorten beamed stems less than normal stems (1 staff-space),
	;; or high order less than 8th beams, patterns like
	;;     c''4 [c''8 c''] c''4 [c''16 c]
	;; are ugly (different stem lengths).
	;;
	;; But if we shorten 16th beams as much as 8th beams, a single
	;; forced 16th beam looks *very* short.

	;; We choose to shorten 8th beams the same as single stems,
	;; and high order beams less than 8th beams, so that all
	;; isolated shortened beams look nice and a bit shortened,
	;; sadly possibly breaking patterns with high order beams.
	(beamed-stem-shorten . (1.0 0.5 0.25))
	(damping . 1)
	(auto-knee-gap . 5.5)

	;; only for debugging.
	(font-family . roman)
	(meta . ((class . Spanner)
		 (interfaces . (staff-symbol-referencer-interface
				beam-interface))))))

    (BreakAlignment
     . (
	(breakable . #t)
	(stacking-dir . 1)
	(positioning-done . ,ly:break-align-interface::calc-positioning-done)
	(X-extent . ,ly:axis-group-interface::width)
	(break-align-orders . ;; end of line
			    #((
			       left-edge
			       ambitus
			       breathing-sign
			       clef
			       staff-bar
			       key-cancellation
			       key-signature
			       time-signature
			       custos)

			      ;; unbroken
			      (
			       left-edge
			       ambitus
			       breathing-sign
			       clef
			       staff-bar
			       key-cancellation
			       key-signature
			       staff
			       time-signature custos)

			      ;; begin of line
			      (
			       left-edge
			       ambitus
			       breathing-sign
			       clef
			       key-cancellation
			       key-signature
			       staff-bar
			       time-signature
			       custos)))
	(axes . (0))
	(meta . ((class . Item)
		 (interfaces . (break-alignment-interface
				axis-group-interface))))))

    (BreakAlignGroup
     . (
	(axes . (0))
	(X-extent . ,ly:axis-group-interface::width)
	(meta . ((class . Item)
		 (interfaces . (break-aligned-interface
				axis-group-interface))))))

    (BreathingSign
     . (
	(break-align-symbol . breathing-sign)
	(breakable . #t)
	(space-alist . (
			(ambitus . (extra-space . 2.0))
			(custos . (minimum-space . 1.0))
			(key-signature . (minimum-space . 1.5))
			(time-signature . (minimum-space . 1.5))
			(staff-bar . (minimum-space . 1.5))
			(clef . (minimum-space . 2.0))
			(first-note . (fixed-space . 1.0)) ;huh?
			(right-edge . (extra-space . 0.1))))
	(stencil . ,ly:text-interface::print)
	(text . ,(make-musicglyph-markup "scripts.rcomma"))
	(Y-offset . ,ly:breathing-sign::offset-callback)
	(break-visibility . ,begin-of-line-invisible)
	(meta . ((class . Item)
		 (interfaces . (break-aligned-interface
				breathing-sign-interface
				text-interface
				font-interface))))))

    (Clef
     . (
	(stencil . ,ly:clef::print)
	(glyph-name . ,ly:clef::calc-glyph-name)
	(breakable . #t)
	(font-family . music)
	(break-align-symbol . clef)
	(break-visibility . ,begin-of-line-visible)
	(space-alist . ((ambitus . (extra-space . 2.0))
			(staff-bar . (extra-space . 0.7))
			(key-cancellation . (minimum-space . 3.5))
			(key-signature . (minimum-space . 3.5))
			(time-signature . (minimum-space . 4.2))
			(first-note . (minimum-fixed-space . 5.0))
			(next-note . (extra-space . 0.5))
			(right-edge . (extra-space . 0.5))))
	(Y-offset . ,ly:staff-symbol-referencer::callback)
	(meta . ((class . Item)
		 (interfaces . (clef-interface
				staff-symbol-referencer-interface
				font-interface
				break-aligned-interface))))))

    (ClusterSpannerBeacon
     . (
	(Y-extent . ,ly:cluster-beacon::height)
	(meta . ((class . Item)
		 (interfaces . (cluster-beacon-interface))))))

    (ClusterSpanner
     . (
	(springs-and-rods . ,ly:spanner::set-spacing-rods)
	(stencil . ,ly:cluster::print)
	(minimum-length . 0.0)
	(padding . 0.25)
	(style . ramp)
	(meta . ((class . Spanner)
		 (interfaces . (cluster-interface))))))

    (ChordName
     . (
	(stencil . ,ly:text-interface::print)
	(after-line-breaking . ,ly:chord-name::after-line-breaking)
	(word-space . 0.0)
	(font-family . sans)
	(font-size . 1.5)
	(meta . ((class . Item)
		 (interfaces . (font-interface
				rhythmic-grob-interface
				text-interface
				chord-name-interface
				item-interface))))))

    (CombineTextScript
     . (
	(stencil . ,ly:text-interface::print)
	(no-spacing-rods . #t)
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(X-offset . ,ly:self-alignment-interface::x-aligned-on-self)
	(direction . ,UP)
	(padding . 0.5)
	(staff-padding . 0.5)
	(script-priority . 200)
	;; todo: add X self alignment?
	(baseline-skip . 2)
	(side-axis . ,Y)
	(font-series . bold)
	(meta . ((class . Item)
		 (interfaces . (text-script-interface
				text-interface
				side-position-interface
				font-interface))))))

    (Custos
     . (
	(break-align-symbol . custos)
	(breakable . #t)
	(stencil . ,ly:custos::print)
	(break-visibility . ,end-of-line-visible)
	(style . vaticana)
	(neutral-direction . ,DOWN)
	(Y-offset . ,ly:staff-symbol-referencer::callback)
	(space-alist . (
			(first-note . (minimum-fixed-space . 0.0))
			(right-edge . (extra-space . 0.1))))
	(meta . ((class . Item)
		 (interfaces
		  . (custos-interface
		     staff-symbol-referencer-interface
		     font-interface
		     break-aligned-interface))))))

    (DotColumn
     . (
	(axes . (0))
	(direction . ,RIGHT)
	(positioning-done . ,ly:dot-column::calc-positioning-done) 
	(X-extent . ,ly:axis-group-interface::width)
	(X-offset . ,ly:dot-column::side-position)
	(meta . ((class . Item)
		 (interfaces . (dot-column-interface
				axis-group-interface))))))

    (Dots
     . (
	(stencil . ,ly:dots::print)
	(dot-count . 1)
	(meta . ((class . Item)
		 (interfaces . (font-interface
				staff-symbol-referencer-interface
				dots-interface))))))

    (DoublePercentRepeat
     . (
	(stencil . ,ly:percent-repeat-item-interface::double-percent)
	(breakable . #t)
	(slope . 1.0)
	(font-encoding . fetaMusic)
	(width . 2.0)
	(thickness . 0.48)
	(break-align-symbol . staff-bar)
	(break-visibility . ,begin-of-line-invisible)
	(meta . ((class . Item)
		 (interfaces . (font-interface
				break-aligned-interface
				percent-repeat-interface))))))

    (DoublePercentRepeatCounter
     . (
	(stencil . ,ly:text-interface::print)
	(X-offset . ,(ly:make-simple-closure `(,+ ,(ly:make-simple-closure (list ly:self-alignment-interface::centered-on-y-parent))
						  ,(ly:make-simple-closure (list ly:self-alignment-interface::x-aligned-on-self)))))
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(font-encoding . fetaNumber)
	(self-alignment-X . 0)
	(font-size . -2) 
	(direction . ,UP)
	(padding . 0.2)
	(staff-padding . 0.25)
	(side-axis . ,Y)
	(meta . ((class . Item)
		 (interfaces . (side-position-interface
				self-alignment-interface
				percent-repeat-interface
				font-interface
				text-interface))))))
    (DynamicLineSpanner
     . (
	(axes . (1))
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(staff-padding . 0.1)
	(padding . 0.6)
	(avoid-slur . outside)
	(slur-padding . 0.3)
	(minimum-space . 1.2)
	(direction . ,DOWN)
	(side-axis . ,Y)
	(Y-extent . ,ly:axis-group-interface::height)
	(X-extent . ,ly:axis-group-interface::width)
	(meta . ((class . Spanner)
		 (interfaces . (axis-group-interface
				dynamic-interface
				dynamic-line-spanner-interface
				side-position-interface))))))

    (DynamicText
     . (

	;; todo.

	(stencil . ,ly:text-interface::print)
	(direction . ,ly:script-interface::calc-direction)

	(X-offset . ,ly:self-alignment-interface::x-aligned-on-self)
	(self-alignment-X . 0)
	(Y-offset . ,ly:self-alignment-interface::y-aligned-on-self)
	(self-alignment-Y . 0)
	(font-series . bold)
	(font-encoding . fetaDynamic)
	(font-shape . italic)
	(avoid-slur . around)
	(no-spacing-rods . #t)
	(script-priority . 100)
	(meta . ((class . Item)
		 (interfaces . (font-interface
				text-interface
				self-alignment-interface
				dynamic-interface
				script-interface))))))

    (DynamicTextSpanner
     . (
	(stencil . ,ly:dynamic-text-spanner::print)

	;; rather ugh with NCSB
	;; (font-series . bold)
	(font-shape . italic)
	(style . dashed-line)

	;; need to blend with dynamic texts.
	(font-size . 1)
	(bound-padding . 0.75)
	(dash-fraction . 0.2)
	(dash-period . 3.0)
	(meta . ((class . Spanner)
		 (interfaces . (font-interface
				text-interface
				dynamic-interface
				dynamic-text-spanner-interface
				spanner-interface))))))

    (Fingering
     . (

	;; sync with TextScript (?)

	(padding . 0.5)
	(avoid-slur . around)
	(slur-padding . 0.2)
	(staff-padding . 0.5)
	(self-alignment-X . 0)
	(self-alignment-Y . 0)
	(script-priority . 100)

	(stencil . ,ly:text-interface::print)
	(direction . ,ly:script-interface::calc-direction)

	(font-encoding . fetaNumber)
	(font-size . -5) 		; don't overlap when next to heads.
	(meta . ((class . Item)
		 (interfaces . (finger-interface
				font-interface
				text-script-interface
				text-interface
				side-position-interface
				self-alignment-interface
				item-interface))))))
    (Glissando
     . (
	(style . line)
	(gap . 0.5)
	(zigzag-width . 0.75)
	(breakable . #t)
	(X-extent . #f)
	(Y-extent . #f)
	(stencil . ,ly:line-spanner::print)
	(after-line-breaking . ,ly:line-spanner::after-line-breaking)
	(meta . ((class . Spanner)
		 (interfaces . (line-interface
				line-spanner-interface))))))

    (GridPoint
     . (
	(X-extent . (0 . 0))
	(Y-extent . (0 . 0))
	(meta . ((class . Item)
		 (interfaces . (grid-point-interface))))))

    (GridLine
     . (
	(X-extent  . ,ly:grid-line-interface::width)
	(stencil . ,ly:grid-line-interface::print)
	(self-alignment-X . ,CENTER)
	(X-offset . ,(ly:make-simple-closure
		      `(,+  ,(ly:make-simple-closure
			      (list ly:self-alignment-interface::centered-on-x-parent))
			    ,(ly:make-simple-closure
			      (list ly:self-alignment-interface::x-aligned-on-self)))))
	(layer . 0)
	(meta . ((class . Item)
		 (interfaces . (self-alignment-interface
				grid-line-interface))))))

    (Hairpin
     . (
	(stencil . ,ly:hairpin::print)
	(springs-and-rods . ,ly:spanner::set-spacing-rods)
	(after-line-breaking . ,ly:hairpin::after-line-breaking)
	(thickness . 1.0)
	(height . 0.6666)
	(minimum-length . 2.0)
	(bound-padding . 1.0)
	(self-alignment-Y . 0)
	(Y-offset . ,ly:self-alignment-interface::y-aligned-on-self)
	(meta . ((class . Spanner)
		 (interfaces . (hairpin-interface
				line-interface
				self-alignment-interface
				dynamic-interface
				spanner-interface))))))

    (HorizontalBracket
     . (
	(thickness . 1.0)
	(stencil . ,ly:horizontal-bracket::print)
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(padding . 0.2)
	(staff-padding . 0.2)
	(direction . ,DOWN)
	(side-axis . ,Y)
	(bracket-flare . (0.5 . 0.5))
	(meta . ((class . Spanner)
		 (interfaces . (horizontal-bracket-interface
				side-position-interface
				line-interface
				spanner-interface))))))
    (InstrumentName
     . (
	(padding . 0.3)
	(stencil . ,ly:system-start-text::print)
	(X-offset . ,ly:side-position-interface::x-aligned-side)
	(direction . ,LEFT)
	(self-alignment-Y . ,CENTER)
	(meta . ((class . Spanner)
		 (interfaces . (system-start-text-interface
				side-position-interface
				font-interface))))))

    (KeyCancellation
     . (
	(stencil . ,ly:key-signature-interface::print)
	(space-alist . (
			(time-signature . (extra-space . 1.25))
			(staff-bar . (extra-space . 0.6))
			(key-signature . (extra-space . 0.5))
			(right-edge . (extra-space . 0.5))
			(first-note . (fixed-space . 2.5))))
	(Y-offset . ,ly:staff-symbol-referencer::callback)
	(break-align-symbol . key-cancellation)
	(break-visibility . ,begin-of-line-invisible)
	(breakable . #t)
	(meta . ((class . Item)
		 (interfaces . (key-signature-interface
				font-interface
				break-aligned-interface))))))
    (KeySignature
     . (
	(stencil . ,ly:key-signature-interface::print)
	(space-alist . (
			(time-signature . (extra-space . 1.15))
			(staff-bar . (extra-space . 1.1))
			(right-edge . (extra-space . 0.5))
			(first-note . (fixed-space . 2.5))))
	(Y-offset . ,ly:staff-symbol-referencer::callback)
	(break-align-symbol . key-signature)
	(break-visibility . ,begin-of-line-visible)
	(breakable . #t)
	(meta . ((class . Item)
		 (interfaces . (key-signature-interface
				font-interface
				break-aligned-interface))))))
    
    
   (LaissezVibrerTie
     . (
	(stencil  . ,ly:tie::print)
	(control-points . ,ly:semi-tie::calc-control-points)
	(direction . ,ly:semi-tie::calc-direction)
	(details . ((ratio . 0.333)
		    (height-limit . 1.0)))
	(thickness . 1.0)
	(meta . ((class . Item)
		 (interfaces . (semi-tie-interface))
		 ))
	))

    (LaissezVibrerTieColumn
     . (
	(X-extent . #f)
	(Y-extent . #f)
	(head-direction . ,LEFT)
	(positioning-done . ,ly:semi-tie-column::calc-positioning-done)
	(meta . ((class . Item)
		 (interfaces . (semi-tie-column-interface))
		 ))
	))

    (LedgerLineSpanner
     . (
	(springs-and-rods . ,ly:ledger-line-spanner::set-spacing-rods)
	(stencil . ,ly:ledger-line-spanner::print)
	(X-extent . #f)
	(Y-extent . #f)
	(minimum-length-fraction . 0.25)
	(length-fraction . 0.25)
	(layer . 0)
	(meta . ((class . Spanner)
		 (interfaces . (ledger-line-interface))))))

    (LeftEdge
     . (
	(break-align-symbol . left-edge)
	(X-extent . (0 . 0))
	(breakable . #t)
	(break-visibility . ,center-invisible)
	(space-alist . (
			(custos . (extra-space . 0.0))
			(ambitus . (extra-space . 2.0))
			(time-signature . (extra-space . 1.0))
			(staff-bar . (extra-space . 0.0))
			(breathing-sign . (minimum-space . 0.0))
			(clef . (extra-space . 0.8))
			(first-note . (fixed-space . 2.0))
			(right-edge . (extra-space . 0.0))
			(key-signature . (extra-space . 0.0))
			(key-cancellation . (extra-space . 0.0))
			))
	(meta . ((class . Item)
		 (interfaces . (break-aligned-interface))))))

    (LigatureBracket
     . (
	;; ugh.  A ligature bracket is totally different from
	;; a tuplet bracket.

	(padding . 2.0)
	(thickness . 1.6)
	(edge-height . (0.7 . 0.7))
	(shorten-pair . (-0.2 . -0.2))
	(direction . ,UP)
	(positions . ,ly:tuplet-bracket::calc-positions)
	(stencil . ,ly:tuplet-bracket::print)
	(staff-padding . 0.25)
	(connect-to-neighbor . ,ly:tuplet-bracket::calc-connect-to-neighbors)
	(control-points . ,ly:tuplet-bracket::calc-control-points)
	(meta . ((class . Spanner)
		 (interfaces . (line-interface
				tuplet-bracket-interface))))))

    (LyricHyphen
     . (
	(thickness . 1.3)
	(height . 0.42)
	(dash-period . 10.0)
	(length . 0.66)
	(minimum-length . 0.3)
	(minimum-distance . 0.1)
	(padding . 0.07)
	(springs-and-rods . ,ly:hyphen-spanner::set-spacing-rods)
	(stencil . ,ly:hyphen-spanner::print)
	(Y-extent . (0 . 0))
	(meta . ((class . Spanner)
		 (interfaces . (lyric-interface
				font-interface
				lyric-hyphen-interface
				spanner-interface))))))

    (LyricExtender
     . (
	(stencil . ,ly:lyric-extender::print)
	(thickness . 0.8) ; line-thickness
	(minimum-length . 1.5)
	(Y-extent . (0 . 0))
	(meta . ((class . Spanner)
		 (interfaces . (lyric-interface
				lyric-extender-interface))))))

    (LyricSpace
     . ((minimum-distance . 0.3)
	(springs-and-rods . ,ly:hyphen-spanner::set-spacing-rods)
	(padding . 0.0)
	(Y-extent . #f)
	(X-extent . #f)
	(meta . ((class . Spanner)
		 (interfaces . (spanner-interface
				lyric-hyphen-interface spacing-interface))
		 ))
	))
    (LyricText
     . (
	(stencil . ,ly:text-interface::print)
	(X-offset . ,ly:self-alignment-interface::aligned-on-x-parent)
	(self-alignment-X . 0)
	(word-space . 0.6)
	(font-series . bold-narrow)
	(font-size . 1.0)
	(meta . ((class . Item)
		 (interfaces . (rhythmic-grob-interface
				lyric-syllable-interface
				self-alignment-interface
				text-interface
				font-interface))))))


    (MeasureGrouping
     . (
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(side-axis . ,Y)
	(stencil . ,ly:measure-grouping::print)
	(padding . 2)
	(direction . ,UP)
	(thickness . 1)
	(height . 2.0)
	(staff-padding . 3)
	(meta . ((class . Spanner)
		 (interfaces . (side-position-interface
				measure-grouping-interface))))))

    (MelodyItem
     . (
	(neutral-direction . ,DOWN)
	(meta . ((class . Item)
		 (interfaces . (melody-spanner-interface spacing-interface))))))
    (MensuralLigature
     . (
	(thickness . 1.4)
	(flexa-width . 2.0)
	(stencil . ,ly:mensural-ligature::print)
	(meta . ((class . Spanner)
		 (interfaces . (mensural-ligature-interface
				font-interface))))))

    (MetronomeMark
     . (
	(stencil . ,ly:text-interface::print)
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(direction . ,UP)
	(padding . 0.8)
	(side-axis . ,Y)
	(meta . ((class . Item)
		 (interfaces . (text-interface
				side-position-interface
				font-interface
				metronome-mark-interface))))))


    (MultiMeasureRest
     . (
	(stencil . ,ly:multi-measure-rest::print)
	(springs-and-rods . ,ly:multi-measure-rest::set-spacing-rods)
	(Y-offset . ,ly:staff-symbol-referencer::callback)
	(staff-position . 0)
	(expand-limit . 10)
	(thick-thickness . 6.6)
	(hair-thickness . 2.0)
	(padding . 1)
	(meta . ((class . Spanner)
		 (interfaces . (multi-measure-rest-interface
				multi-measure-interface
				rest-interface
				font-interface
				staff-symbol-referencer-interface))))))


    (MultiMeasureRestNumber
     . (
	(bound-padding  . 2.0)
	(springs-and-rods . ,ly:multi-measure-rest::set-text-rods)
	(stencil . ,ly:text-interface::print)
	(X-offset . ,(ly:make-simple-closure
		      `(,+ ,(ly:make-simple-closure (list ly:self-alignment-interface::x-aligned-on-self))
			   ,(ly:make-simple-closure (list ly:self-alignment-interface::x-centered-on-y-parent)))))
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(side-axis . ,Y)
	(self-alignment-X . 0)
	(direction . ,UP)
	(padding . 0.4)
	(staff-padding . 0.4)
	(font-encoding . fetaNumber)
	(meta . ((class . Spanner)
		 (interfaces . (side-position-interface
				multi-measure-interface
				self-alignment-interface
				font-interface
				text-interface))))
	      ))

    (MultiMeasureRestText
     . (
	(stencil . ,ly:text-interface::print)
	(X-offset . ,(ly:make-simple-closure
		      `(,+ ,(ly:make-simple-closure (list ly:self-alignment-interface::x-centered-on-y-parent))
			   ,(ly:make-simple-closure (list ly:self-alignment-interface::x-aligned-on-self)))))
	
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(self-alignment-X . 0)
	(direction . ,UP)
	(padding . 0.2)
	(staff-padding . 0.25)
	(meta . ((class . Spanner)
		 (interfaces . (side-position-interface
				multi-measure-interface
				self-alignment-interface
				font-interface
				text-interface))))))

    (NoteCollision
     . (
	(axes . (0 1))
	(X-extent . ,ly:axis-group-interface::width)
	(Y-extent . ,ly:axis-group-interface::height)
	(positioning-done . ,ly:note-collision-interface::calc-positioning-done)
	(meta . ((class . Item)
		 (interfaces . (note-collision-interface
				axis-group-interface))))))

    (NoteColumn
     . (
	(axes . (0 1))
	(X-extent . ,ly:axis-group-interface::width)
	(Y-extent . ,ly:axis-group-interface::height)
	(meta . ((class . Item)
		 (interfaces . (axis-group-interface
				note-column-interface))))))

    (NoteHead
     . (
	(stencil . ,ly:note-head::print)
	(stem-attachment . ,ly:note-head::calc-stem-attachment)
	(glyph-name . ,note-head::calc-glyph-name) 
	(Y-offset . ,ly:staff-symbol-referencer::callback)
	(X-offset . ,ly:note-head::stem-x-shift)
	(meta . ((class . Item)
		 (interfaces . (rhythmic-grob-interface
				rhythmic-head-interface
				font-interface
				note-head-interface
				ledgered-interface
				staff-symbol-referencer-interface))))))

    (NoteSpacing
     . (
	(stem-spacing-correction . 0.5)
	(same-direction-correction . 0.25)
	;; Changed this from 0.75.
	;; If you ever change this back, please document! --hwn
	(knee-spacing-correction . 1.0)
	(meta . ((class . Item)
		 (interfaces . (spacing-interface
				note-spacing-interface))))))

    (NoteName
     . (
	(stencil . ,ly:text-interface::print)
	(meta . ((class . Item)
		 (interfaces . (note-name-interface
				text-interface
				font-interface))))))

    (OctavateEight
     . (
	(self-alignment-X . 0)
	(break-visibility . ,begin-of-line-visible)
	(X-offset . ,(ly:make-simple-closure
		      `(,+ ,(ly:make-simple-closure (list ly:self-alignment-interface::x-aligned-on-self))
			   ,(ly:make-simple-closure (list ly:self-alignment-interface::centered-on-x-parent)))))
	
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(stencil . ,ly:text-interface::print)

	;; no Y dimensions, because of lyrics under tenor clef.
	(Y-extent . (0 . 0))
	(font-shape . italic)
	(padding . 0.6)
	(staff-padding . 0.2)
	(font-size . -4)
	(meta . ((class . Item)
		 (interfaces . (text-interface
				self-alignment-interface
				side-position-interface
				font-interface))))))

    (OttavaBracket
     . (
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(stencil . ,ly:ottava-bracket::print)
	(font-shape . italic)
	(shorten-pair . (0.0 . -0.6))
	(staff-padding . 1.0)
	(padding . 0.5)
	(minimum-length . 1.0)
	(dash-fraction . 0.3)
	(edge-height . (0 . 1.2))
	(direction . ,UP)
	(meta . ((class . Spanner)
		 (interfaces . (ottava-bracket-interface
				horizontal-bracket-interface
				line-interface
				side-position-interface
				font-interface
				text-interface))))))

    (PaperColumn
     . (
	(axes . (0))

	(before-line-breaking . ,ly:paper-column::before-line-breaking)
	;; (stencil . ,ly:paper-column::print)
	(X-extent . ,ly:axis-group-interface::width)
	

	;; debugging
	;;		         (font-size . -6) (font-name . "sans") (Y-extent . #f)
	(meta . ((class . Paper_column)
		 (interfaces . (paper-column-interface
				axis-group-interface
				spaceable-grob-interface))))))

    (ParenthesesItem
     . (
	(stencil . ,parentheses-item::print)
	(font-size . -6)
	(padding . 0.2)
	(meta . ((class . Item)
		 (interfaces . (parentheses-interface font-interface))))
	
	      ))
    (PhrasingSlur
     . ((details . ,default-slur-details)
	(control-points . ,ly:slur::calc-control-points)
	(direction . ,ly:slur::calc-direction)
	(springs-and-rods . ,ly:spanner::set-spacing-rods)
	(Y-extent . ,ly:slur::height)
	(stencil . ,ly:slur::print)		      
	(thickness . 1.1)
	(minimum-length . 1.5)
	(height-limit . 2.0)
	(ratio . 0.333)
	(meta . ((class . Spanner)
		 (interfaces . (slur-interface))))))

    (NonMusicalPaperColumn
     . (
	(axes . (0))
	(before-line-breaking . ,ly:paper-column::before-line-breaking)
	(X-extent . ,ly:axis-group-interface::width)
	;;		      (stencil . ,ly:paper-column::print)
	
	(breakable . #t)

	;; debugging stuff: print column number.
	;;		 (font-size . -6) (font-name . "sans")	(Y-extent . #f)

	(meta . ((class . Paper_column)
		 (interfaces . (paper-column-interface
				axis-group-interface
				spaceable-grob-interface))))))

    (PercentRepeat
     . (
	(springs-and-rods . ,ly:multi-measure-rest::set-spacing-rods)
	(stencil . ,ly:multi-measure-rest::percent)
	(slope . 1.0)
	(thickness . 0.48)
	(font-encoding . fetaMusic)
	(meta . ((class . Spanner)
		 (interfaces . (multi-measure-rest-interface
				font-interface
				percent-repeat-interface))))))
    (PercentRepeatCounter
     . (
	(stencil . ,ly:text-interface::print)
	(X-offset . ,(ly:make-simple-closure
		      `(,+ ,(ly:make-simple-closure (list ly:self-alignment-interface::x-centered-on-y-parent))
			   ,(ly:make-simple-closure (list ly:self-alignment-interface::x-aligned-on-self)))))
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(self-alignment-X . 0)
	(direction . ,UP)
	(padding . 0.2)
	(staff-padding . 0.25)
	(font-size . -2) 
	(font-encoding . fetaNumber)
	(meta . ((class . Spanner)
		 (interfaces . (side-position-interface
				self-alignment-interface
				percent-repeat-interface
				font-interface
				text-interface))))))

    ;; an example of a text spanner
    (PianoPedalBracket
     . (
	(stencil . ,ly:piano-pedal-bracket::print)
	(style . line)
	(bound-padding . 1.0)
	(direction . ,DOWN)
	(bracket-flare . (0.5 . 0.5))
	(edge-height . (1.0 . 1.0))
	(shorten-pair . (0.0 . 0.0))
	(thickness .  1.0)
	(meta . ((class . Spanner)
		 (interfaces . (line-interface
				piano-pedal-interface
				piano-pedal-bracket-interface))))))

    (RehearsalMark
     . (
	(stencil . ,ly:text-interface::print)
	(X-offset . ,(ly:make-simple-closure
		      `(,+ ,(ly:make-simple-closure
			     `(,ly:self-alignment-interface::x-aligned-on-self))
			   ,(ly:make-simple-closure
			     `(,ly:self-alignment-interface::centered-on-x-parent)))
		      ))
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(self-alignment-X . 0)
	(direction . ,UP)
	(breakable . #t)
	(font-size . 2)
	(baseline-skip . 2)
	(break-visibility . ,end-of-line-invisible)
	(padding . 0.8)
	(meta . ((class . Item)
		 (interfaces . (text-interface
				side-position-interface
				font-interface
				mark-interface
				self-alignment-interface))))))


    (RepeatSlash
     . (
	(stencil . ,ly:percent-repeat-item-interface::beat-slash)
	(thickness . 0.48)
	(slope . 1.7)
	(meta . ((class . Item)
		 (interfaces . (percent-repeat-interface))))))
    (RepeatTie
     . (
	(stencil  . ,ly:tie::print)
	(control-points . ,ly:semi-tie::calc-control-points)
	(direction . ,ly:semi-tie::calc-direction)
	(details . ((ratio . 0.333)
		    (height-limit . 1.0)))
	(thickness . 1.0)
	(meta . ((class . Item)
		 (interfaces . (semi-tie-interface))
		 ))
	))

    (RepeatTieColumn
     . (
	(X-extent . #f)
	(Y-extent . #f)
	(head-direction . ,RIGHT)
	(positioning-done . ,ly:semi-tie-column::calc-positioning-done)
	(meta . ((class . Item)
		 (interfaces . (semi-tie-column-interface))
		 ))
	))

    (Rest
     . (
	(stencil . ,ly:rest::print)
	(X-extent . ,ly:rest::width)
	(Y-extent . ,ly:rest::height)
	(Y-offset . ,ly:rest::y-offset-callback)
	(minimum-distance . 0.25)
	(meta . ((class . Item)
		 (interfaces . (font-interface
				rhythmic-head-interface
				rhythmic-grob-interface
				staff-symbol-referencer-interface
				rest-interface))))))

    (RestCollision
     . (
	(minimum-distance . 0.75)
	(positioning-done . ,ly:rest-collision::calc-positioning-done)
	(meta . ((class . Item)
		 (interfaces . (rest-collision-interface))))))

    (Script
     . (
	;; don't set direction here: it breaks staccato.

	;; This value is sensitive: if too large, staccato dots will move a
	;; space a away.
	(padding . 0.20)
	(staff-padding . 0.25)
	;; (script-priority . 0) priorities for scripts, see script.scm
	(X-offset . , ly:self-alignment-interface::centered-on-x-parent)
	

	(stencil . ,ly:script-interface::print)
	(direction . ,ly:script-interface::calc-direction)
	(font-encoding . fetaMusic)
	(meta . ((class . Item)
		 (interfaces . (script-interface
				side-position-interface
				font-interface))))))

    (ScriptColumn
     . (
	(before-line-breaking . ,ly:script-column::before-line-breaking)
	(meta . ((class . Item)
		 (interfaces . (script-column-interface))))))

    (SeparationItem
     . (
	(X-extent . #f)
	(Y-extent . #f)
	(meta . ((class . Item)
		 (interfaces . (spacing-interface
				separation-item-interface))))))

    (SeparatingGroupSpanner
     . (
	(springs-and-rods . ,ly:separating-group-spanner::set-spacing-rods)
	(meta . ((class . Spanner)
		 (interfaces . (only-prebreak-interface
				spacing-interface
				separation-spanner-interface))))))

    (Slur
     . ((details . ,default-slur-details)
	(control-points . ,ly:slur::calc-control-points)
	(direction . ,ly:slur::calc-direction)
	(springs-and-rods . ,ly:spanner::set-spacing-rods)
	(Y-extent . ,ly:slur::height)
	(stencil . ,ly:slur::print)
	(thickness . 1.2)
	(line-thickness . 0.8)
	(minimum-length . 1.5)
	(height-limit . 2.0)
	(ratio . 0.25)
	(avoid-slur . inside)
	(meta . ((class . Spanner)
		 (interfaces . (slur-interface))))))

 (SostenutoPedal
     . (
	(stencil . ,ly:text-interface::print)
	(direction . ,RIGHT)
	(X-offset . ,ly:self-alignment-interface::x-aligned-on-self)
	(no-spacing-rods . #t)
	(padding . 0.0) ;; padding relative to SostenutoPedalLineSpanner
	(font-shape . italic)
	(self-alignment-X . 0)
	(meta . ((class . Item)
		 (interfaces . (text-interface
				self-alignment-interface
				font-interface))))))

    (SostenutoPedalLineSpanner
     . (
	(axes . (1))
	(Y-extent . ,ly:axis-group-interface::height)
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(side-axis . ,Y)
	(staff-padding . 1.0)
	(padding . 1.2)
	(minimum-space . 1.0)
	(direction . ,DOWN)
	(meta . ((class . Spanner)
		 (interfaces . (piano-pedal-interface
				axis-group-interface
				side-position-interface))))))
    (SpacingSpanner
     . (
	(springs-and-rods . ,ly:spacing-spanner::set-springs)
	(average-spacing-wishes . #t)
	(grace-space-factor . 0.6)
	(shortest-duration-space . 2.0)
	(spacing-increment . 1.2)
	(base-shortest-duration . ,(ly:make-moment 3 16))
	(meta . ((class . Spanner)
		 (interfaces . (spacing-interface
				spacing-spanner-interface))))))

    (SpanBar
     . (
	(break-align-symbol . staff-bar)
	(Y-extent . ())
	(layer . 0)
	(breakable . #t)
	(stencil . ,ly:span-bar::print)
	(bar-size . ,ly:span-bar::calc-bar-size)
	(X-extent . ,ly:span-bar::width)
	(glyph-name . ,ly:span-bar::calc-glyph-name)
	(before-line-breaking . ,ly:span-bar::before-line-breaking)

	;; ugh duplication!

	;;
	;; Ross. page 151 lists other values, we opt for a leaner look
	;;
	(kern . 3.0)
	(thin-kern . 3.0)
	(hair-thickness . 1.6)
	(thick-thickness . 6.0)
	(meta . ((class . Item)
		 (interfaces . (span-bar-interface
				font-interface
				bar-line-interface))))))

    (StanzaNumber
     . (
	(stencil . ,ly:text-interface::print)
	(font-series . bold)
	(padding . 1.0)
	(X-offset . ,ly:side-position-interface::x-aligned-side)
	(side-axis . ,X)
	(direction . ,LEFT)
	(meta . ((class . Item)
		 (interfaces . (side-position-interface
				stanza-number-interface
				text-interface
				font-interface))))))


    (StaffSpacing
     . (
	(breakable . #t)
	(stem-spacing-correction . 0.4)
	(meta . ((class . Item)
		 (interfaces . (spacing-interface
				staff-spacing-interface))))))

   
    (StaffSymbol
     . (
	(Y-extent . ,ly:staff-symbol::height)
	(stencil . ,ly:staff-symbol::print)
	(line-count . 5)
	(ledger-line-thickness . (1.0 . 0.1))
	(layer . 0)
	(meta . ((class . Spanner)
		 (interfaces . (staff-symbol-interface))))))

    (Stem
     . (
	(direction . ,ly:stem::calc-direction)
	(default-direction . ,ly:stem::calc-default-direction)
	(stem-end-position . ,ly:stem::calc-stem-end-position)
	(neutral-direction . ,DOWN)
	(stem-info . ,ly:stem::calc-stem-info)
	(positioning-done . ,ly:stem::calc-positioning-done)
	(stencil . ,ly:stem::print)
	(X-extent . ,ly:stem::width)
	(Y-extent . ,ly:stem::height)
	(length . ,ly:stem::calc-length)
	(thickness . 1.3)
	(details
	 . (
	    ;; 3.5 (or 3 measured from note head) is standard length
	    ;; 32nd, 64th flagged stems should be longer
	    (lengths . (3.5 3.5 3.5 4.5 5.0))

	    ;; FIXME.  3.5 yields too long beams (according to Ross and
	    ;; looking at Baerenreiter examples) for a number of common
	    ;; boundary cases.  Subtracting half a beam thickness fixes
	    ;; this, but the bug may well be somewhere else.

	    ;; FIXME this should come from 'lengths
	    (beamed-lengths . (3.26 3.5 3.6))

	    ;; The 'normal' minima
	    (beamed-minimum-free-lengths . (1.83 1.5 1.25))
					;(beamed-minimum-free-lengths . (2.0 1.83 1.25))

	    ;; The 'extreme case' minima
	    (beamed-extreme-minimum-free-lengths . (2.0 1.25))

	    ;; Stems in unnatural (forced) direction should be shortened by
	    ;; one staff space, according to [Roush & Gourlay].
	    ;; Flagged stems we shorten only half a staff space.
	    (stem-shorten . (1.0 0.5))

	    ))

	;; We use the normal minima as minimum for the ideal lengths,
	;; and the extreme minima as abolute minimum length.

	(X-offset . ,ly:stem::offset-callback)
	(Y-offset . ,ly:staff-symbol-referencer::callback)
	(meta . ((class . Item)
		 (interfaces . (stem-interface
				font-interface))))))

    (StemTremolo
     . (
	(Y-extent . ,ly:stem-tremolo::height)
	(stencil . ,ly:stem-tremolo::print)
	(X-extent . #f)
	(slope . ,ly:stem-tremolo::calc-slope)
	(beam-width . 1.6) ; staff-space
	(beam-thickness . 0.48) ; staff-space
	(meta . ((class . Item)
		 (interfaces . (stem-tremolo-interface))))))

    (StringNumber
     . (
	(stencil . ,print-circled-text-callback)
	(padding . 0.5)
	(staff-padding . 0.5)
	(self-alignment-X . 0)
	(self-alignment-Y . 0)
	(script-priority . 100)
	(font-encoding . fetaNumber)
	(font-size . -5) 		; don't overlap when next to heads.
	(meta . ((class . Item)
		 (interfaces . (string-number-interface
				font-interface
				text-script-interface
				text-interface
				side-position-interface
				self-alignment-interface
				item-interface))))))
    
    (SustainPedal
     . (
	(no-spacing-rods . #t)
	(stencil . ,ly:sustain-pedal::print)
	(self-alignment-X . 0)
	(direction . ,RIGHT)
	(padding . 0.0)  ;; padding relative to SustainPedalLineSpanner
	(X-offset . ,ly:self-alignment-interface::x-aligned-on-self)
	(meta . ((class . Item)
		 (interfaces . (piano-pedal-interface
				text-spanner-interface
				text-interface
				self-alignment-interface
				font-interface))))))

    (SustainPedalLineSpanner
     . (
	(axes . (1))
	(Y-extent . ,ly:axis-group-interface::height)
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(side-axis . ,Y)
	(padding . 1.2)
	(staff-padding . 1.2)
	(minimum-space . 1.0)
	(direction . ,DOWN)
	(meta . ((class . Spanner)
		 (interfaces . (piano-pedal-interface
				axis-group-interface
				side-position-interface))))))

    (System
     . (
	(axes . (0 1))
	(X-extent . ,ly:axis-group-interface::width)
	(Y-extent . ,ly:axis-group-interface::height)
	(meta . ((class . System)
		 (interfaces . (system-interface
				axis-group-interface))))))

    (SystemStartBrace
     . (
	(style . brace)
	(padding . 0.3)
	(stencil . ,ly:system-start-delimiter::print)
	(collapse-height . 5.0)
	(X-offset . ,ly:side-position-interface::x-aligned-side)
	(direction . ,LEFT)
	(font-encoding . fetaBraces)
	(Y-extent . #f)
	(meta . ((class . Spanner)
		 (interfaces . (system-start-delimiter-interface
				side-position-interface
				font-interface))))))


    (SystemStartSquare
     . (
	(Y-extent . #f)
	(X-offset . ,ly:side-position-interface::x-aligned-side)
	(direction . ,LEFT)
	(stencil . ,ly:system-start-delimiter::print)
	(style . line-bracket)
	(thickness . 1.0)
	(meta . ((class . Spanner)
		 (interfaces . (font-interface
				side-position-interface
				system-start-delimiter-interface))))))
    (SystemStartBracket
     . (
	(Y-extent . #f)
	(padding . 0.8)
	(X-offset . ,ly:side-position-interface::x-aligned-side)
	(direction . ,LEFT)
	(stencil . ,ly:system-start-delimiter::print)
	(style . bracket)
	(collapse-height . 5.0)
	(thickness . 0.45)
	(meta . ((class . Spanner)
		 (interfaces . (font-interface
				side-position-interface
				system-start-delimiter-interface))))))

    (SystemStartBar
     . (
	(Y-extent . #f)
	(padding . 0.0)
	(X-offset . ,ly:side-position-interface::x-aligned-side)
	(direction . ,LEFT)
	(style . bar-line)
	(thickness . 1.6)
	(stencil . ,ly:system-start-delimiter::print)
	(meta . ((class . Spanner)
		 (interfaces . (side-position-interface
				system-start-delimiter-interface))))))


    (TabNoteHead
     . (
	(stencil . ,ly:text-interface::print)
	(Y-offset . ,ly:staff-symbol-referencer::callback)
	(font-size . -2)
	(stem-attachment . (1.0 . 1.35))
	(font-series . bold)
	(meta . ((class . Item)
		 (interfaces
		  . (rhythmic-head-interface
		     font-interface rhythmic-grob-interface
		     note-head-interface
		     staff-symbol-referencer-interface
		     text-interface))))))

    (TextScript
     . (
	(no-spacing-rods . #t)
	(X-offset . ,ly:self-alignment-interface::x-aligned-on-self)
	(direction . ,DOWN)

	;; sync with Fingering ?
	(padding . 0.5)
	(staff-padding . 0.5)

	(stencil . ,ly:text-interface::print)
	(direction . ,ly:script-interface::calc-direction)

	(avoid-slur . around)
	(slur-padding . 0.5)
	(script-priority . 200)
	;; todo: add X self alignment?
	(meta . ((class . Item)
		 (interfaces . (text-script-interface
				text-interface
				side-position-interface
				font-interface))))))

    (TextSpanner
     . (
	(stencil . ,ly:text-spanner::print)
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(font-shape . italic)
	(style . dashed-line)
	(staff-padding . 0.8)
	(dash-fraction . 0.2)
	(dash-period . 3.0)
	(side-axis . ,Y)
	(direction . ,UP)
	(meta . ((class . Spanner)
		 (interfaces . (text-spanner-interface
				side-position-interface
				font-interface))))))

    (Tie
     . (
	(control-points . ,ly:tie::calc-control-points)
	(avoid-slur . inside)
	(direction . ,ly:tie::calc-direction)
	(stencil . ,ly:tie::print)
	(font-size . -6)
	(details . (
		    ;; for a full list, see tie-helper.cc
		    (ratio . 0.333)
		    (center-staff-line-clearance . 0.6)
		    (tip-staff-line-clearance . 0.45)
		    (note-head-gap . 0.2)
		    (stem-gap . 0.35)
		    (height-limit . 1.0)
		    (horizontal-distance-penalty-factor . 10)
		    (min-length-penalty-factor . 20)
		    (tie-tie-collision-distance . 0.45) 
		    (tie-tie-collision-penalty . 25.0)
		    (intra-space-threshold . 1.25)
		    (outer-tie-vertical-distance-symmetry-penalty-factor . 10)
		    (outer-tie-length-symmetry-penalty-factor . 10)
		    (outer-tie-vertical-gap . 0.25)
		    (multi-tie-region-size . 1)
		    (between-length-limit . 1.0)))
	(thickness . 1.2)
	(line-thickness . 0.8)
	(meta . ((class . Spanner)
		 (interfaces . (tie-interface))))
	))

    (TieColumn
     . (
	(positioning-done . ,ly:tie-column::calc-positioning-done)
	(before-line-breaking . ,ly:tie-column::before-line-breaking)
	(X-extent . #f)
	(Y-extent . #f)
	(meta . ((class . Spanner)
		 (interfaces . (tie-column-interface))))))

    (TimeSignature
     . (
	(stencil . ,ly:time-signature::print)
	(break-align-symbol . time-signature)
	(break-visibility . ,all-visible)
	(space-alist . (
			(first-note . (fixed-space . 2.0))
			(right-edge . (extra-space . 0.5))
			(staff-bar . (minimum-space . 2.0))))
	(breakable . #t)
	(style . C)
	(meta . ((class . Item)
		 (interfaces . (time-signature-interface
				break-aligned-interface
				font-interface))))))

    (TrillSpanner
     . (
	(stencil . ,ly:dynamic-text-spanner::print)
	(edge-text . ,(cons (make-musicglyph-markup "scripts.trill")
			    ""))
	(style . trill)
	(staff-padding . 1.0)
	(padding . 0.5)
	(direction . ,UP)
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(side-axis . ,Y)
	(meta . ((class . Spanner)
		 (interfaces . (text-spanner-interface
				side-position-interface
				font-interface))))))

    (TrillPitchAccidental
     . ((X-offset . ,ly:side-position-interface::x-aligned-side)
	(padding . 0.2)
	(direction . ,LEFT)
	(font-size . -4)
	(side-axis . ,X)
	(stencil . ,ly:accidental-interface::print)
	(meta . ((class . Item)
		 (interfaces . (item-interface
				accidental-interface
				side-position-interface
				font-interface))))))

    (TrillPitchGroup
     . ((X-offset . ,ly:side-position-interface::x-aligned-side)
	(axes . (,X))
	(font-size . -4)
	(stencil . ,parenthesize-elements)
	(direction . ,RIGHT)
	(side-axis . ,X)
	(padding . 0.3)
	(meta . ((class . Item)
		 (interfaces . (side-position-interface
				note-head-interface
				rhythmic-head-interface
				font-interface
				accidental-interface
				axis-group-interface))))))

    (TrillPitchHead
     . (
	(stencil . ,ly:note-head::print)
	(duration-log . 2)
	(Y-offset . ,ly:staff-symbol-referencer::callback)
	(font-size . -4)
	(meta . ((class . Item)
		 (interfaces . (item-interface
				rhythmic-head-interface
				font-interface
				pitched-trill-interface
				ledgered-interface
				staff-symbol-referencer-interface))))))

    (TupletBracket
     . (
	(padding . 1.1)
	(thickness . 1.6)
	(edge-height . (0.7 . 0.7))
	(shorten-pair . (-0.2 . -0.2))
	(staff-padding . 0.25)
	
	(direction  . ,ly:tuplet-bracket::calc-direction)
	(positions . ,ly:tuplet-bracket::calc-positions)
	(connect-to-neighbor . ,ly:tuplet-bracket::calc-connect-to-neighbors)
	(control-points . ,ly:tuplet-bracket::calc-control-points)
	(stencil . ,ly:tuplet-bracket::print)
	
	(meta . ((class . Spanner)
		 (interfaces . (line-interface
				tuplet-bracket-interface))))))

    (TupletNumber
     . (
	(stencil . ,ly:tuplet-number::print)
	(font-shape . italic)
	(font-size . -2)
	(avoid-slur . inside)
	(meta . ((class . Spanner)
		 (interfaces . (text-interface tuplet-number-interface
				font-interface))))))
    
    (UnaCordaPedal
     . (
	(stencil . ,ly:text-interface::print)
	(font-shape . italic)
	(no-spacing-rods . #t)
	(self-alignment-X . 0)
	(direction . ,RIGHT)
	(padding . 0.0)  ;; padding relative to UnaCordaPedalLineSpanner
	(X-offset . ,ly:self-alignment-interface::x-aligned-on-self)
	(meta . ((class . Item)
		 (interfaces . (text-interface
				self-alignment-interface
				font-interface))))))

    (UnaCordaPedalLineSpanner
     . (
	(axes . (1))
	(Y-extent . ,ly:axis-group-interface::height)
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(side-axis . ,Y)
	(padding . 1.2)
	(staff-padding . 1.2)
	(minimum-space . 1.0)
	(direction . ,DOWN)
	(meta . ((class . Spanner)
		 (interfaces . (piano-pedal-interface
				axis-group-interface
				side-position-interface))))))

    (VaticanaLigature
     . (
	(thickness . 0.6)
	(flexa-width . 2.0)
	(stencil . ,ly:vaticana-ligature::print)
	(meta . ((class . Spanner)
		 (interfaces . (vaticana-ligature-interface
				font-interface))))))

    (VerticalAlignment
     . (
	(axes . (1))
	(positioning-done . ,ly:align-interface::calc-positioning-done)
	(after-line-breaking . ,ly:align-interface::stretch-after-break)
	(Y-extent . ,ly:axis-group-interface::height)
	(X-extent . ,ly:axis-group-interface::width)
	(stacking-dir . -1)
	(meta . ((class . Spanner)
		 (interfaces . (align-interface
				axis-group-interface))))))
    (VerticalAxisGroup
     . (
	(axes . (1))
	(Y-offset . ,ly:hara-kiri-group-spanner::force-hara-kiri-callback)

	(Y-extent . ,ly:hara-kiri-group-spanner::y-extent)
	(X-extent . ,ly:axis-group-interface::width)
	(meta . ((class . Spanner)
		 (interfaces . (axis-group-interface
				hara-kiri-group-interface
				vertically-spaceable-interface))))))


    (VoltaBracket
     . (
	(stencil . ,ly:volta-bracket-interface::print)
	(direction . ,UP)
	(padding . 1)
	(font-encoding . fetaNumber)
	(Y-offset . ,ly:side-position-interface::y-aligned-side)
	(side-axis . ,Y)
	(thickness . 1.6)  ;;  line-thickness
	(edge-height . (2.0 . 2.0)) ;; staff-space;
	(minimum-space . 5)
	(font-size . -4)
	(meta . ((class . Spanner)
		 (interfaces . (volta-bracket-interface
				horizontal-bracket-interface				
				line-interface
				text-interface
				side-position-interface
				font-interface)))
	      )))


    (VoiceFollower
     . (
	(style . line)
	(gap . 0.5)
	(breakable . #t)
	(X-extent . #f)
	(Y-extent . #f)
	(stencil . ,ly:line-spanner::print)
	(after-line-breaking . ,ly:line-spanner::after-line-breaking)
	(meta . ((class . Spanner)
		 (interfaces . (line-spanner-interface
				line-interface))))
	))

))

(define (completize-grob-entry x)
  "Transplant assoc key into 'name entry of 'meta of X.  Set interfaces for Item, Spanner etc.
"
  ;;  (display (car x))
  ;;  (newline)
  (let* ((name-sym  (car x))
	 (grob-entry (cdr x))
	 (meta-entry (cdr (assoc 'meta grob-entry)))
	 (class (cdr (assoc 'class meta-entry)))
	 (ifaces-entry
	  (cdr (assoc 'interfaces meta-entry))))

    (cond
     ((eq? 'Item class)
      (set! ifaces-entry (cons 'item-interface ifaces-entry)))
     ((eq? 'Spanner class)
      (set! ifaces-entry (cons 'spanner-interface ifaces-entry)))
     ((eq? 'Paper_column class)
      (set! ifaces-entry (cons 'item-interface
			       (cons 'paper-column-interface ifaces-entry))))
     ((eq? 'System class)
      (set! ifaces-entry (cons 'system-interface
			       (cons 'spanner-interface ifaces-entry))))
     (else
      (ly:warning "Unknown class ~a" class)))
    (set! ifaces-entry (cons 'grob-interface ifaces-entry))

    (set! meta-entry (assoc-set! meta-entry 'name name-sym))
    (set! meta-entry (assoc-set! meta-entry 'interfaces
				 ifaces-entry))
    (set! grob-entry (assoc-set! grob-entry 'meta meta-entry))
    (cons name-sym grob-entry)))

(set! all-grob-descriptions (map completize-grob-entry all-grob-descriptions))

;;  (display (map pair? all-grob-descriptions))

;; make sure that \property Foo.Bar =\turnOff doesn't complain

(map (lambda (x)
       ;; (display (car x)) (newline)

       (set-object-property! (car x) 'translation-type? list?)
       (set-object-property! (car x) 'is-grob? #t))
     all-grob-descriptions)

(set! all-grob-descriptions (sort all-grob-descriptions alist<?))

