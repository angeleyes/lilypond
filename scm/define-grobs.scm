;;;; grob-description.scm -- part of generated backend documentation
;;;;
;;;;  source file of the GNU LilyPond music typesetter
;;;; 
;;;; (c)  1998--2003  Han-Wen Nienhuys <hanwen@cs.uu.nl>
;;;;                 Jan Nieuwenhuizen <janneke@gnu.org>

;;;; distances are given in stafflinethickness (thicknesses) and
;;;; staffspace (distances)

;;;; WARNING: the meta field should be the last one.
;;;; WARNING: don't use anonymous functions for initialization. 

;; TODO: junk the meta field in favor of something more compact?
(define-public all-grob-descriptions
  `(
    (Accidental
     . (
	(molecule-callback . ,Accidental_interface::brew_molecule)
	(font-family . music)
	(cautionary-style . parentheses)
	(after-line-breaking-callback . ,Accidental_interface::after_line_breaking)		(meta . ((interfaces . (item-interface accidental-interface font-interface))))
	))
    (AccidentalPlacement
     . (
	(X-extent-callback . ,Axis_group_interface::group_extent_callback)
	(left-padding . 0.3)

	;; this is quite small, but it is very ugly to have
	;; accs closer to the previous note than to the next one.
	(right-padding . 0.15)
	(meta . ((interfaces . (item-interface accidental-placement-interface))))
	))

    (Ambitus
     . (
       (breakable . #t)
       (break-align-symbol . ambitus)
       (molecule-callback . ,Ambitus::brew_molecule)
       (font-family . music)
       (note-head-style . "noteheads-2")
       (visibility-lambda . ,begin-of-line-visible)
       (join-heads . #t)
       (space-alist . (
		       (clef . (extra-space . 0.0))
		       (key-signature . (extra-space . 0.0))
		       (staff-bar . (extra-space . 0.0))
		       (time-signature . (extra-space . 0.0)) 
		       (first-note . (extra-space . 0.0))
		       ))
       (meta . ((interfaces . (ambitus-interface staff-symbol-referencer-interface break-aligned-interface item-interface  font-interface))))
       ))

    (Arpeggio
     . (
	(X-extent-callback . ,Arpeggio::width_callback)
	(Y-extent-callback . #f)	       
	(molecule-callback . ,Arpeggio::brew_molecule)
	(Y-offset-callbacks . (,Staff_symbol_referencer::callback))
	(X-offset-callbacks . (,Side_position_interface::aligned_side))
	(direction . -1)
	(staff-position . 0.0)
	(meta . ((interfaces . (arpeggio-interface staff-symbol-referencer-interface side-position-interface item-interface font-interface))))
	))

    (BarLine
     . (
	(break-align-symbol . staff-bar)
	(glyph . "|")
	(break-glyph-function . ,default-break-barline)
	(bar-size-procedure . ,Bar_line::get_staff_bar_size)
	(molecule-callback . ,Bar_line::brew_molecule)	   
	(break-visibility . ,all-visible)
	(breakable . #t)
	(before-line-breaking-callback . ,Bar_line::before_line_breaking)
	(space-alist . (
			(time-signature . (extra-space . 0.75)) 
			(custos . (minimum-space . 2.0))
			(clef .   (minimum-space . 1.0))
			(key-signature . (extra-space . 1.0))
			(first-note . (extra-space . 1.3))
			(right-edge . (extra-space . 0.0))
			))

	;;
	;; Ross. page 151 lists other values, we opt for a leaner look
	;; 
	(kern . 3.0)
	(thin-kern . 3.0)
	(hair-thickness . 1.6)
	(thick-thickness . 6.0)
	(meta . ((interfaces . (bar-line-interface item-interface   break-aligned-interface font-interface))))
	))

    
    (BarNumber
     . (
	(molecule-callback . ,Text_item::brew_molecule)
	(breakable . #t)
	(break-visibility . ,begin-of-line-visible)
	(padding . 1.0)
	(direction . 1)
	(font-family . roman)
	(font-relative-size . -1)
	(Y-offset-callbacks . (,Side_position_interface::aligned_side))
	(X-offset-callbacks . (,Self_alignment_interface::aligned_on_self))
	(self-alignment-X . 1)
	(extra-offset . (1.3 . 0))
	(meta .
	      ((interfaces . (side-position-interface
			      text-interface
			      self-alignment-interface
			      font-interface item-interface  break-aligned-interface))))

	     ))

    (BassFigure
     . (
	(molecule-callback . ,Text_item::brew_molecule)
	(Y-offset-callbacks . (,Self_alignment_interface::aligned_on_self))
	(direction . 1)
	(font-family . number)
	(font-relative-size . -3)
	(kern . 0.2)
	(meta . ((interfaces . (text-interface 							rhythmic-grob-interface
												bass-figure-interface item-interface
												self-alignment-interface font-interface))))
	))
    (Beam
     . (
	;; todo: clean this up a bit: the list is getting
	;; rather long.
	(molecule-callback . ,Beam::brew_molecule)
	(concaveness-gap . 2.0)
	(concaveness-threshold . 0.08)
	
	(positions . (#f . #f))
	(position-callbacks . (,Beam::least_squares
			       ,Beam::check_concave
			       ,Beam::slope_damping
			       ,Beam::shift_region_to_valid
			       ,Beam::quanting
			      ))

	;; TODO: should be in SLT.
	(thickness . 0.48) ; in staff-space
	(before-line-breaking-callback . ,Beam::before_line_breaking)
	(after-line-breaking-callback . ,Beam::after_line_breaking)
	(neutral-direction . -1)
	(dir-function . ,beam-dir-majority-median)
	
	;; Whe have some unreferenced problems here.
	;;
	;; If we shorten beamed stems less than normal stems (1 staffspace),
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
	
	(outer-stem-length-limit . 0.2)
	(slope-limit . 0.2)
	(flag-width-function . ,beam-flag-width-function)
	(damping . 1)
	(auto-knee-gap . 5.5)

	;; only for debugging.
;	(font-name . "cmr10")
	
	(space-function . ,Beam::space_function)
	(meta . ((interfaces . (staff-symbol-referencer-interface beam-interface spanner-interface))))
	))

    (BreakAlignment
     . (
	(breakable . #t)
	(stacking-dir . 1)
	(axes . (0))
	(X-extent-callback . ,Axis_group_interface::group_extent_callback)
	(meta . ((interfaces . (break-alignment-interface item-interface axis-group-interface)))))
	)

    (BreakAlignGroup
     . (
	(axes  . (0))
	(X-offset-callbacks . (,Break_align_interface::alignment_callback))
	(X-extent-callback . ,Axis_group_interface::group_extent_callback)
	(meta . ((interfaces . (break-aligned-interface item-interface axis-group-interface item-interface ))))
	))

    (BreathingSign
     . (
	(break-align-symbol . breathing-sign)
	(breakable . #t)
	(space-alist . (
			(ambitus . (extra-space . 2.0))
			(custos . (minimum-space . 1.0))
			(key-signature . (minimum-space . 1.5))
			(staff-bar . (minimum-space . 1.5))
			(clef . (minimum-space . 2.0))
			(first-note . (minimum-space . 1.0))
			(right-edge . (extra-space . 0.1))
			))
	(molecule-callback . ,Text_item::brew_molecule)
	(text . ,(make-musicglyph-markup "scripts-rcomma"))
	(Y-offset-callbacks . (,Breathing_sign::offset_callback))
	(break-visibility . ,begin-of-line-invisible)
	(meta . ((interfaces . (break-aligned-interface breathing-sign-interface text-interface font-interface item-interface ))))
	))

    (Clef
     . (
	(molecule-callback . ,Clef::brew_molecule)
	(before-line-breaking-callback . ,Clef::before_line_breaking)
	(breakable . #t)
	(font-family . music)	   
	(break-align-symbol . clef)
	(break-visibility . ,begin-of-line-visible)
	(space-alist . (
			(ambitus . (extra-space . 2.0))
			(staff-bar . (minimum-space .  3.7))
			(key-signature . (minimum-space . 4.0))
			(time-signature . (minimum-space . 4.2))
			(first-note . (minimum-space . 5.0))
			(next-note . (extra-space . 0.5))
			(right-edge . (extra-space . 0.5))
			))
	(Y-offset-callbacks  . (,Staff_symbol_referencer::callback)) 
	(meta . ((interfaces . (clef-interface staff-symbol-referencer-interface font-interface break-aligned-interface item-interface ))))
	))
    
    (ClusterSpannerBeacon
     . (
	(molecule-callback . #f)
	(meta . ((interfaces . (cluster-beacon-interface item-interface))))
	))
    
    (ClusterSpanner
     . (
	(molecule-callback . ,Cluster::brew_molecule)
	(spacing-procedure . ,Spanner::set_spacing_rods)		
	(minimum-length . 0.0)
	(padding . 0.25)
	(style . ramp)
	(meta . ((interfaces . (cluster-interface spanner-interface))))
	))

    (ChordName
     . (
	(molecule-callback . ,Text_item::brew_molecule)
	(after-line-breaking-callback . ,Chord_name::after_line_breaking)
	(word-space . 0.0)
	(font-family . roman)
	(meta . ((interfaces . (font-interface 							rhythmic-grob-interface
												text-interface chord-name-interface item-interface ))))
	))

    (Custos
     . (
	(break-align-symbol . custos)
	(breakable . #t)
	(molecule-callback . ,Custos::brew_molecule)
	(break-visibility . ,end-of-line-visible)
	(style . vaticana)
	(neutral-position . 0)
	(neutral-direction . -1)
	(adjust-if-on-staffline . #t)
	(Y-offset-callbacks . (,Staff_symbol_referencer::callback))
	(space-alist . (
			(first-note . (minimum-space . 0.0))
			(right-edge . (extra-space . 0.1))
			))
	(meta . ((interfaces
		  . (custos-interface staff-symbol-referencer-interface
				      font-interface
				      break-aligned-interface item-interface ))))
	))


    (DotColumn
     . (
	(axes . (0))
	(direction . 1)
	(X-extent-callback . ,Axis_group_interface::group_extent_callback)
	(X-offset-callbacks . (,Dot_column::side_position))
	(meta . ((interfaces . (dot-column-interface axis-group-interface item-interface ))))
	))

    (Dots
     . (
	(molecule-callback . ,Dots::brew_molecule)
	(dot-count . 1)
	(Y-offset-callbacks  . (,Dots::quantised_position_callback ,Staff_symbol_referencer::callback))
	(meta . ((interfaces . (font-interface staff-symbol-referencer-interface dots-interface item-interface ))))
	))

    (DoublePercentRepeat .
			 (
			  (molecule-callback . ,Percent_repeat_item_interface::double_percent)
			  (breakable . #t)
			  (slope . 1.0)
			  (font-family . music)
			  (width . 2.0)
			  (thickness . 0.48)
			  (break-align-symbol . staff-bar)
			  (break-visibility . ,begin-of-line-invisible)
			  (meta . ((interfaces . (font-interface
						  break-aligned-interface
						  percent-repeat-interface item-interface ))))
			 ))

    (DynamicText
     . (
	(Y-offset-callbacks . (,Self_alignment_interface::aligned_on_self))
	(molecule-callback . ,Text_item::brew_molecule)
	(X-offset-callbacks . (,Self_alignment_interface::aligned_on_self
			       ,Self_alignment_interface::centered_on_parent))
	(self-alignment-X . 0)
	(no-spacing-rods . #t)
	(script-priority . 100)
	(font-series . bold)
	(font-family . dynamic)
	(font-shape . italic)
	(self-alignment-Y . 0)
	(meta . ((interfaces . (font-interface text-interface self-alignment-interface  dynamic-interface script-interface item-interface ))))
	))

    (DynamicLineSpanner
     . (
	(axes . (1))
	(Y-extent-callback . ,Axis_group_interface::group_extent_callback)	
	(Y-offset-callbacks . (,Side_position_interface::aligned_side))
	(padding . 0.6)
	(minimum-space . 1.2)
	(direction . -1)
	(meta . ((interfaces . (dynamic-interface axis-group-interface side-position-interface spanner-interface))))
	))

    (LeftEdge
     . (
	(break-align-symbol . left-edge)
	(X-extent-callback . ,Grob::point_dimension_callback)
	(breakable . #t)
	(space-alist . (
			(custos . (extra-space . 0.0))
			(ambitus . (extra-space . 2.0))
			(time-signature . (extra-space . 0.0)) 
			(staff-bar . (extra-space . 0.0))
			(breathing-sign . (minimum-space  . 0.0))
			(clef . (extra-space . 0.85))
			(first-note . (extra-space . 1.0))
			(right-edge . (extra-space . 0.0))
			(key-signature . (extra-space . 0.0))
			))
	(meta . ((interfaces . (break-aligned-interface item-interface ))))
	))

    (Fingering
     . (
	(molecule-callback . ,Text_item::brew_molecule)
	(padding . 0.6)
	;;		(direction . -1)
	(self-alignment-X . 0)
	(self-alignment-Y . 0)
	(script-priority . 100)
	(font-family . number)
	(font-relative-size . -3)
	(font-shape . upright)
	(meta . ((interfaces . (finger-interface font-interface text-script-interface text-interface side-position-interface self-alignment-interface item-interface ))))
	))


    (RemoveEmptyVerticalGroup
     . (
	(Y-offset-callbacks . (,Hara_kiri_group_spanner::force_hara_kiri_callback))
	(Y-extent-callback . ,Hara_kiri_group_spanner::y_extent)
	(remove-first . #t)
	(axes . (1))
	(meta . ((interfaces . (axis-group-interface hara-kiri-group-interface item-interface  spanner-interface))))
	))

    (Hairpin
     . (
	(molecule-callback . ,Hairpin::brew_molecule)
	(thickness . 1.0)
	(height . 0.6666)
	(spacing-procedure . ,Spanner::set_spacing_rods)
	(minimum-length . 2.0)
	(if-text-padding . 1.0)
	(width-correct . -1.0)

	(dash-thickness . 1.2)
	(dash-length . 4.0)
	(self-alignment-Y . 0)
	(Y-offset-callbacks . (,Self_alignment_interface::aligned_on_self))
	(meta . ((interfaces . (hairpin-interface self-alignment-interface dynamic-interface spanner-interface))))
	))

    (HorizontalBracket
     . (
	(thickness . 1.0)
	(molecule-callback . ,Horizontal_bracket::brew_molecule)
	(Y-offset-callbacks . (,Side_position_interface::aligned_side))
	(padding . 0.2)
	(direction . -1)
	(meta . ((interfaces . (horizontal-bracket-interface side-position-interface spanner-interface))))
	))
    (InstrumentName
     . (
	(breakable . #t)
	(Y-offset-callbacks . (,Self_alignment_interface::aligned_on_self
			       ,Side_position_interface::aligned_on_support_refpoints))
	;; This direction is for aligned_on_support_refpoints
	;; (?) --hwn
	(direction . 0)
	(space-alist . (
			(left-edge . (extra-space . 1.0))
			))

	(self-alignment-Y . 0)
	(molecule-callback . ,Text_item::brew_molecule)		
	(break-align-symbol . instrument-name)
	(break-visibility . ,begin-of-line-visible)
	(baseline-skip . 2)
	(font-family . roman)
	(meta . ((interfaces . (font-interface self-alignment-interface side-position-interface text-interface break-aligned-interface item-interface ))))
	))

    (KeySignature
     . (
	(molecule-callback . ,Key_signature_interface::brew_molecule)
	(space-alist . (
			(time-signature . (extra-space . 1.25))
			(staff-bar .  (extra-space . 1.1))
			(right-edge . (extra-space . 0.5))
			(first-note . (extra-space . 2.5))
			))
	(Y-offset-callbacks . (,Staff_symbol_referencer::callback))
	(break-align-symbol . key-signature)
	(break-visibility . ,begin-of-line-visible)
	(breakable . #t)
	(meta . ((interfaces . (key-signature-interface  font-interface  break-aligned-interface item-interface ))))
	))

    (LigatureBracket
     . (
	(ligature-primitive-callback . ,Note_head::brew_molecule)
	(direction . 1)
	(gap . 0.0)
	(padding . 2.0)
	(thickness . 1.6)
	(edge-height . (0.7 . 0.7))
	(shorten-pair . (-0.2 . -0.2))
	(before-line-breaking-callback . ,Tuplet_bracket::before_line_breaking)
	(after-line-breaking-callback . ,Tuplet_bracket::after_line_breaking)
	(molecule-callback . ,Tuplet_bracket::brew_molecule)
	(meta .  ((interfaces . (tuplet-bracket-interface spanner-interface))))
	))

    (LyricHyphen
     . (
	(thickness . 1.0)
	(height . 0.4)
	(minimum-length .  0.5) 
	(maximum-length .  100)
	(spacing-procedure . ,Hyphen_spanner::set_spacing_rods)
	(molecule-callback . ,Hyphen_spanner::brew_molecule)
	(Y-extent-callback . ,Grob::point_dimension_callback)
	(meta . ((interfaces . (lyric-hyphen-interface spanner-interface))))
	))

    (LyricExtender
     . (
	(molecule-callback . ,Lyric_extender::brew_molecule)
	(height . 0.8) ; stafflinethickness;
	(right-trim-amount . 0.5)
	(Y-extent-callback . ,Grob::point_dimension_callback)
	(meta . ((interfaces . (lyric-extender-interface spanner-interface))))
	))

    (LyricText
     . (
	(molecule-callback . ,Text_item::brew_molecule)
	(X-offset-callbacks . (,Self_alignment_interface::centered_on_parent
			       ,Self_alignment_interface::aligned_on_self))
	(self-alignment-X . 0)
	(word-space . 0.6)
	(ignore-length-mismatch . #f)
	(begin-alignment . 4)
	(end-alignment . 2)
	(font-family . roman)
	(font-shape . upright)
	
	(meta . ((interfaces . (rhythmic-grob-interface lyric-syllable-interface self-alignment-interface text-interface font-interface item-interface ))))
	))

    (MensuralLigature
     . (
	(thickness . 1.4)
	(flexa-width . 2.0)
	(ligature-primitive-callback . ,Mensural_ligature::brew_ligature_primitive)
	(molecule-callback . ,Mensural_ligature::brew_molecule)
	(meta . ((interfaces . (mensural-ligature-interface font-interface))))
	))

    (RehearsalMark
     . (
	(molecule-callback . ,Text_item::brew_molecule)
	(X-offset-callbacks . (,Self_alignment_interface::aligned_on_self))
	(Y-offset-callbacks . (,Side_position_interface::aligned_side))	
	(self-alignment-X . 0)
	(break-align-symbol . time-signature)
	(direction . 1)
	(breakable . #t)
	(font-relative-size . 1)
	(font-family . roman)
	(baseline-skip . 2)
	(break-visibility . ,end-of-line-invisible)
	(padding . 0.8)
	(meta . ((interfaces . (text-interface side-position-interface font-interface mark-interface self-alignment-interface item-interface ))))
	))
     (MetronomeMark
     . (
	(molecule-callback . ,Text_item::brew_molecule)
	(Y-offset-callbacks . (,Side_position_interface::aligned_side))	
	(direction . 1)
	(breakable . #t)
	(font-family . roman)
	(break-visibility . ,end-of-line-invisible)
	(padding . 0.8)
	(meta . ((interfaces . (text-interface side-position-interface font-interface metronome-mark-interface item-interface))))
	))
    (MeasureGrouping
     . (
	(Y-offset-callbacks . (,Side_position_interface::out_of_staff 
			       ,Side_position_interface::aligned_side))
	(molecule-callback . ,Measure_grouping::brew_molecule)
	(padding . 2)
	(direction . 1)
	(thickness . 1)
	(height . 2.0)
	(staff-padding . 3)
	(meta . ((interfaces . (spanner-interface side-position-interface measure-grouping-interface))))
	))
    (MultiMeasureRest
     . (
	(spacing-procedure . ,Multi_measure_rest::set_spacing_rods)
	(molecule-callback . ,Multi_measure_rest::brew_molecule)
	(Y-offset-callbacks . (,Staff_symbol_referencer::callback))
	(staff-position . 0)
	(expand-limit . 10)
	(thick-thickness . 6.6)
	(hair-thickness . 2.0)
	(padding . 1)
	(meta . ((interfaces . (multi-measure-rest-interface rest-interface font-interface staff-symbol-referencer-interface))))
	))
    
    (MultiMeasureRestNumber
     . (
	(molecule-callback . ,Text_item::brew_molecule)
	(X-offset-callbacks . (,Self_alignment_interface::aligned_on_self
			       ,Self_alignment_interface::centered_on_other_axis_parent))
	(Y-offset-callbacks . (,Side_position_interface::aligned_side))
	(self-alignment-X . 0)
	(direction . 1)
	(padding . 1.3)
	(font-family . number)
	(meta . ((interfaces . (side-position-interface self-alignment-interface font-interface spanner-interface text-interface))))
	))
    (MultiMeasureRestText
     . (
	(molecule-callback . ,Text_item::brew_molecule)
	(X-offset-callbacks . (,Self_alignment_interface::aligned_on_self
			       ,Self_alignment_interface::centered_on_other_axis_parent))
	(Y-offset-callbacks . (,Side_position_interface::aligned_side))
	(self-alignment-X . 0)
	(direction . 1)
	(padding . 1.5)
	(font-family . roman)
	(meta . ((interfaces . (side-position-interface self-alignment-interface font-interface spanner-interface text-interface))))
	))
 (NoteCollision
     . (
	(axes . (0 1))
	;; Ugh, should not be hard-coded. 
	(note-width . 1.321)
	(X-extent-callback . ,Axis_group_interface::group_extent_callback)
	(Y-extent-callback . ,Axis_group_interface::group_extent_callback)	
	(meta . ((interfaces . (note-collision-interface axis-group-interface item-interface ))))
	))

    (NoteColumn
     . (
	(axes . (0 1))
	(X-extent-callback . ,Axis_group_interface::group_extent_callback)
	(Y-extent-callback . ,Axis_group_interface::group_extent_callback)	
	(meta . ((interfaces . (axis-group-interface note-column-interface item-interface ))))
	))

    (NoteHead
     . (
	(style . default)
	(molecule-callback . ,Note_head::brew_molecule)
	(ligature-primitive-callback . ,Note_head::brew_molecule)
	(glyph-name-procedure . ,find-notehead-symbol)
	(Y-offset-callbacks  . (,Staff_symbol_referencer::callback))
	(stem-attachment-function . ,note-head-style->attachment-coordinates)
	(meta . ((interfaces . (rhythmic-grob-interface rhythmic-head-interface font-interface note-head-interface staff-symbol-referencer-interface item-interface ))))
	))

    (Glissando
     . (
	(style . line)
	(gap . 0.5)
	(zigzag-width . 0.75)
	(breakable . #t)
	(X-extent-callback . #f)
	(Y-extent-callback . #f)			 
	(after-line-breaking-callback . ,Line_spanner::after_line_breaking)
	(molecule-callback . ,Line_spanner::brew_molecule)
	(meta . ((interfaces . (line-spanner-interface spanner-interface))))
	))

    (VoiceFollower
     . (
	(style . line)
	(gap . 0.5)
	(breakable . #t)
	(X-extent-callback . #f)
	(Y-extent-callback . #f)			 
	(molecule-callback . ,Line_spanner::brew_molecule)
	(after-line-breaking-callback . ,Line_spanner::after_line_breaking)
	(meta . ((interfaces . (line-spanner-interface spanner-interface))))
	))

    (NoteName
     . (
	(molecule-callback . ,Text_item::brew_molecule)
	(font-family . roman)
	(meta . ((interfaces . (note-name-interface text-interface font-interface item-interface ))))
	))

    (OctavateEight
     . (
	(self-alignment-X . 0)
	(break-visibility . ,begin-of-line-visible)
	(X-offset-callbacks . (,Self_alignment_interface::centered_on_parent ,Self_alignment_interface::aligned_on_self))
	(Y-offset-callbacks . (,Side_position_interface::aligned_side))
	(molecule-callback . ,Text_item::brew_molecule)
	(font-shape . italic)
	(padding . 0.4)
	(font-relative-size . -2)
	(font-family . roman)
	(meta . ((interfaces . (text-interface self-alignment-interface side-position-interface font-interface item-interface ))))
	))

    (PaperColumn
     . (
	(axes . (0))
	(before-line-breaking-callback . ,Paper_column::before_line_breaking)
	(X-extent-callback . ,Axis_group_interface::group_extent_callback)

;	        (molecule-callback . ,Paper_column::brew_molecule) (font-name . "cmr8") (Y-extent-callback . #f)
	(meta . ((interfaces . (paper-column-interface axis-group-interface spaceable-grob-interface item-interface ))))
	))

    (PhrasingSlur
     . (
	(molecule-callback . ,Slur::brew_molecule)
	(thickness . 1.2)		
	(spacing-procedure . ,Spanner::set_spacing_rods)		
	(minimum-length . 1.5)
	(after-line-breaking-callback . ,Slur::after_line_breaking)
	(extremity-rules . ,default-slur-extremity-rules)
	(extremity-offset-alist . ,default-phrasing-slur-extremity-offset-alist)
	(de-uglify-parameters . (1.5  0.8  -2.0))
	(Y-extent-callback . ,Slur::height)
	(height-limit . 2.0)
	(ratio . 0.333)
	(beautiful . 0.5)
	(y-free . 0.75)
	(attachment . (#f . #f))
	(attachment-offset . ((0 . 0) . (0 . 0)))
	(slope-limit . 0.8)
	(details . ((force-blowfit . 0.5)
		    (bezier-pct-c0 . -0.2) (bezier-pct-c3 . 0.000006)
		    (bezier-pct-out-max . 0.8) (bezier-pct-in-max . 1.2)
		    (bezier-area-steps . 1.0)))
	(meta . ((interfaces . (slur-interface spanner-interface))))
	))

    (NonMusicalPaperColumn
     . (
	(axes . (0))
	(X-extent-callback . ,Axis_group_interface::group_extent_callback)
	(before-line-breaking-callback . ,Paper_column::before_line_breaking)
	;; debugging stuff: print column number.
;	(molecule-callback . ,Paper_column::brew_molecule) (font-name . "cmr8")	(Y-extent-callback . #f)


	(meta .  ((interfaces . (paper-column-interface axis-group-interface spaceable-grob-interface item-interface ))))
	))

    (PercentRepeat
     . (
	(spacing-procedure . ,Multi_measure_rest::set_spacing_rods)
	(molecule-callback . ,Multi_measure_rest::percent)
	(slope . 1.0)
	(thickness . 0.48)
	(minimum-width . 12.5) ; staffspace
	(font-family . music)
	(meta . ((interfaces . (multi-measure-rest-interface  spanner-interface font-interface percent-repeat-interface))))
	))

    (PianoPedalBracket   ;; an example of a text spanner
     . (
	(molecule-callback . ,Text_spanner::brew_molecule)
	(after-line-breaking-callback . ,Piano_pedal_bracket::after_line_breaking)
	(font-family . roman)
	(style . line)
	(if-text-padding . 1.0)
	(width-correct . 0)
	(enclose-bounds . #t)
	(direction . -1)
	(bracket-flare . (0.5 . 0.5))
	(edge-height . (1.0 . 1.0))
	(shorten-pair . (0.0 . 0.0))
	(thickness .  1.0)
	(meta . ((interfaces . (text-spanner-interface piano-pedal-interface piano-pedal-bracket-interface spanner-interface))))
	))

    (RepeatSlash
     . (
	(molecule-callback . , Percent_repeat_item_interface::beat_slash)
	(thickness . 0.48)
	(slope . 1.7)
	(meta . ((interfaces . (percent-repeat-interface item-interface ))))
	))
    (Rest
     . (
	(after-line-breaking-callback . ,Rest::after_line_breaking)
	(X-extent-callback . ,Rest::extent_callback)
	(Y-extent-callback . ,Rest::extent_callback)		
	(molecule-callback . ,Rest::brew_molecule)
	(Y-offset-callbacks . (,Staff_symbol_referencer::callback)) 
	(minimum-beam-collision-distance . 0.75)
	(meta . (
		 (interfaces . (font-interface
				rhythmic-head-interface
											rhythmic-grob-interface

				staff-symbol-referencer-interface
				rest-interface item-interface ))
		))))

    (RestCollision
     . (
	(minimum-distance . 0.75)
	(meta . ((interfaces . (rest-collision-interface item-interface ))))
	))

    (Script
     . (
	;; don't set direction here: it breaks staccato.
	(molecule-callback . ,Script_interface::brew_molecule)

	;; This value is sensitive: if too large, staccato dots will move a
	;; space a away.
	(padding . 0.25)
	;; (script-priority . 0) priorities for scripts, see script.scm
	(X-offset-callbacks . (,Self_alignment_interface::centered_on_parent))
	(before-line-breaking-callback . ,Script_interface::before_line_breaking)
	(font-family . music)
	(meta . ((interfaces . (script-interface side-position-interface font-interface item-interface ))))
	))

    (ScriptColumn
     . (
	(before-line-breaking-callback . ,Script_column::before_line_breaking)
	(meta . ((interfaces . (script-column-interface item-interface ))))
	))

    (Slur
     . (
	(molecule-callback . ,Slur::brew_molecule)
	(thickness . 1.2)		
	(spacing-procedure . ,Spanner::set_spacing_rods)		
	(minimum-length . 1.5)
	(after-line-breaking-callback . ,Slur::after_line_breaking)
	(extremity-rules . ,default-slur-extremity-rules)
	(extremity-offset-alist . ,default-slur-extremity-offset-alist)
	(de-uglify-parameters . (1.5  0.8  -2.0))
	(Y-extent-callback . ,Slur::height)
	(height-limit . 2.0)
	(ratio . 0.333)
	(beautiful . 0.5)
	(y-free . 0.75)
	(attachment . (#f . #f))
	(attachment-offset . ((0 . 0) . (0 . 0)))
	(slope-limit . 0.8)
	(details . ((force-blowfit . 0.5)
		    (bezier-pct-c0 . -0.2) (bezier-pct-c3 . 0.000006)
		    (bezier-pct-out-max . 0.8) (bezier-pct-in-max . 1.2)
		    (bezier-area-steps . 1.0)))
	(meta . ((interfaces . (slur-interface spanner-interface))))
	))

    (SpacingSpanner
     . (
	(spacing-procedure .  ,Spacing_spanner::set_springs)
	(grace-space-factor . 0.6)
	(shortest-duration-space . 2.0)
	(spacing-increment . 1.2)
	(base-shortest-duration . ,(ly:make-moment 1 8))
	(meta . ((interfaces . (spacing-interface spacing-spanner-interface spanner-interface))))
	))

    (SpanBar
     . (
	(break-align-symbol . staff-bar)
	(bar-size-procedure . ,Span_bar::get_bar_size) 
	(molecule-callback . ,Span_bar::brew_molecule)
	(break-visibility . ,begin-of-line-invisible)
	(X-extent-callback . ,Span_bar::width_callback)
	(Y-extent-callback . ())
	(breakable . #t)
	(glyph . "|")
	(before-line-breaking-callback . ,Span_bar::before_line_breaking)
	;; ugh duplication! 

	;;
	;; Ross. page 151 lists other values, we opt for a leaner look
	;; 
	(kern . 3.0)
	(thin-kern . 3.0)
	(hair-thickness . 1.6)
	(thick-thickness . 6.0)
	(meta . ((interfaces . (span-bar-interface font-interface
						   bar-line-interface item-interface ))))
	))

    (StanzaNumber
     . (
	(breakable . #t)
	(molecule-callback . ,Text_item::brew_molecule)		
	(break-align-symbol . clef)
	(break-visibility . ,begin-of-line-visible)
	(font-family . roman)
	(meta . ((interfaces . (break-aligned-interface text-interface font-interface item-interface ))))		
	))

    (StaffSpacing
     . (
	(breakable . #t)
	(stem-spacing-correction . 0.4)

	(meta . ((interfaces . (spacing-interface staff-spacing-interface item-interface ))))
	))
    (NoteSpacing
     . (
	(stem-spacing-correction . 0.5)

	;; Changed this from 0.75.
	;; If you ever change this back, please document! --hwn
	(knee-spacing-correction . 1.0)
	
	(meta . ((interfaces . (spacing-interface note-spacing-interface item-interface ))))
	))

    (StaffSymbol
     . (
	(molecule-callback . ,Staff_symbol::brew_molecule)
	(line-count . 5)
	(layer . 0)
	(meta . ((interfaces . (staff-symbol-interface spanner-interface))))
	))

    (SostenutoPedal
     . (
	(molecule-callback . ,Text_item::brew_molecule)
	(direction . 1)
	(X-offset-callbacks . (,Self_alignment_interface::aligned_on_self))
	(no-spacing-rods . #t)
	(padding . 0.0) ;; padding relative to SostenutoPedalLineSpanner
	(font-family . roman)
	(font-shape . italic)
	(self-alignment-X . 0)
	(meta . ((interfaces . (text-interface  self-alignment-interface font-interface item-interface))))
	))

    (SostenutoPedalLineSpanner 
     . (
	(axes . (1))
	(Y-extent-callback . ,Axis_group_interface::group_extent_callback)	
	(Y-offset-callbacks . (,Side_position_interface::aligned_side))
	

	(padding . 1.2)
	(minimum-space . 1.0)
	(direction . -1)
	(meta . ((interfaces . (piano-pedal-interface axis-group-interface side-position-interface  spanner-interface))))
	))
	
    (Stem
     . (

	;; this list is rather long. Trim --hwn
	(before-line-breaking-callback . ,Stem::before_line_breaking)
	(molecule-callback . ,Stem::brew_molecule)
	(thickness . 1.3)

	;; 3.5 (or 3 measured from note head) is standar length
	;; 32nd, 64th flagged stems should be longer
	(lengths . (3.5 3.5 3.5 4.5 5.0))
	
	;; Stems in unnatural (forced) direction should be shortened by
	;; one staff space, according to [Roush & Gourlay].
	;; Flagged stems we shorten only half a staff space.
	(stem-shorten . (1.0 0.5))

	;; default stem direction for note on middle line
	(neutral-direction . -1)
	
	;; [Wanske]: standard length (but no shorter than minimum).
	;; (beamed-lengths . (3.5))
	
	;; FIXME.  3.5 yields too long beams (according to Ross and
	;; looking at Baerenreiter examples) for a number of common
	;; boundary cases.  Subtracting half a beam thickness fixes
	;; this, but the bug may well be somewhere else.
	(beamed-lengths . (3.26 3.26 1.5))
	
	;; [Wanske] lists three sets of minimum lengths.  One
	;; set for the nomal case, and one set for beams with `der
	;; Balkenendpunkt weiter "uber bzw. unter die Systemgrenze
	;; hinaus (bei Gruppen mit grossem Tonumfang)' and the extreme
	;; case.

	;; Note that Wanske lists numbers lengths starting from top of
	;; head, so we must add half a staff space.
	
	;; We use the normal minima as minimum for the ideal lengths,
	;; and the extreme minima as abolute minimum length.
	
	;; The 'normal' minima
	(beamed-minimum-free-lengths . (2.5 2.0 1.0))
	
	;; The 'extreme case' minima
	(beamed-extreme-minimum-free-lengths . (1.83 1.5 1.25))

	(X-offset-callbacks . (,Stem::off_callback))
	(X-extent-callback . ,Stem::dim_callback)	
	(Y-extent-callback . ,Stem::height)
	(Y-offset-callbacks . (,Staff_symbol_referencer::callback))
	(adjust-if-on-staffline . #t)
	(font-family . music)	   
	(avoid-note-head . #f)
	(up-to-staff . #f)
	(meta . ((interfaces . (stem-interface  font-interface item-interface ))))
	))

    (StemTremolo
     . (
	(molecule-callback . ,Stem_tremolo::brew_molecule)
	(Y-extent-callback . ,Stem_tremolo::height)
	(X-extent-callback . #f)

	(beam-width . 1.6) ; staff-space
	(beam-thickness . 0.48) ; staff-space
	(meta . ((interfaces . (stem-tremolo-interface item-interface ))))
	))

    (SeparationItem
     . (
	(meta . ((interfaces . (spacing-interface separation-item-interface item-interface ))))
	))

    (SeparatingGroupSpanner
     . (
	(spacing-procedure . ,Separating_group_spanner::set_spacing_rods)
	(meta . ((interfaces . (spacing-interface separation-spanner-interface spanner-interface))))
	))

    (SustainPedal
     . (
	(no-spacing-rods . #t)
	(molecule-callback . ,Sustain_pedal::brew_molecule)
	(self-alignment-X . 0)
	(direction . 1)
	(padding . 0.0)  ;; padding relative to SustainPedalLineSpanner
	(X-offset-callbacks . (,Self_alignment_interface::aligned_on_self))
	(meta . ((interfaces . (piano-pedal-interface text-spanner-interface text-interface self-alignment-interface font-interface item-interface))))
	))

    (SustainPedalLineSpanner 
     . (
	(axes . (1))
	(Y-extent-callback . ,Axis_group_interface::group_extent_callback)
	(Y-offset-callbacks . (,Side_position_interface::aligned_side))
	
	(padding . 1.2)
	(minimum-space . 1.0)
	(direction . -1)
	(meta . ((interfaces . (piano-pedal-interface axis-group-interface side-position-interface spanner-interface))))
	))

    (System
     . (
	(axes . (0 1))
	(X-extent-callback . ,Axis_group_interface::group_extent_callback)
	(Y-extent-callback . ,Axis_group_interface::group_extent_callback)	
	(meta . ((interfaces . (system-interface axis-group-interface spanner-interface))))
	))

    (SystemStartBrace
     . (
	(glyph . "brace")
	(molecule-callback . ,System_start_delimiter::brew_molecule)
	(collapse-height . 5.0)
	(font-family . braces)
	(Y-extent-callback . #f)
	(meta . ((interfaces . (system-start-delimiter-interface font-interface))))
	))

    (SystemStartBracket
     . (
	(Y-extent-callback . #f)
	(molecule-callback . ,System_start_delimiter::brew_molecule)
	(glyph . "bracket")
	(arch-height . 1.5)
	(arch-angle . 50.0)
	(arch-thick . 0.25)
	(arch-width . 1.5)
	(bracket-collapse-height . 1)
	(thickness . 0.25)
	(meta . ((interfaces . (system-start-delimiter-interface spanner-interface))))
	))

    (SystemStartBar
     . (
	(Y-extent-callback . #f)
	(molecule-callback . ,System_start_delimiter::brew_molecule)
	(glyph . "bar-line")
	(thickness . 1.6)
	(after-line-breaking-callback . ,System_start_delimiter::after_line_breaking)
	(meta . ((interfaces . (system-start-delimiter-interface spanner-interface))))
	))

    (TextScript
     . (
	(molecule-callback . ,Text_item::brew_molecule)
	(no-spacing-rods . #t)
	(X-offset-callbacks . (,Self_alignment_interface::aligned_on_self))
	(direction . -1)
	(padding . 0.5)
	(script-priority . 200)
	;; todo: add X self alignment?
	(baseline-skip . 2)
	(font-family . roman)
	(meta . ((interfaces . (text-script-interface text-interface side-position-interface font-interface item-interface ))))
	))

    (TextSpanner
     . (
	(molecule-callback . ,Text_spanner::brew_molecule)
	(font-family . roman)
	(style . line)

	;; urg, only for (de)cresc. text spanners
	(if-text-padding . 1.0)
	(width-correct . -1)
	(direction . 1)
	(meta . ((interfaces . (text-spanner-interface side-position-interface font-interface spanner-interface))))		
	))
    
    (OttavaSpanner
     . (
	(Y-offset-callbacks . (,Side_position_interface::aligned_side))
	(molecule-callback . ,Text_spanner::brew_molecule)
	(font-family . roman)
	(enclose-bounds . #t)
	(width-correct . 0.0)
	(style . dashed-line)
	(edge-height . (0 . 1.5))
	(direction . 1)
	(meta . ((interfaces . (text-spanner-interface side-position-interface font-interface spanner-interface))))		
	))
    
    (TabNoteHead
     . (
	(font-family . roman)
	(style . default)
	(molecule-callback . ,tablature-molecule-callback)
	(Y-offset-callbacks  . (,Staff_symbol_referencer::callback))
	(extra-offset . (0 . -0.65)) ;; UGH! TODO: Clean this up!
	(stem-attachment-function . ,tablature-stem-attachment-function)
	(meta . ((interfaces
		  . (rhythmic-head-interface
		     font-interface
		     note-head-interface staff-symbol-referencer-interface
		     text-interface item-interface ))))
	))


    (Tie
     . (
	(molecule-callback . ,Tie::brew_molecule)
	(spacing-procedure . ,Spanner::set_spacing_rods)
	(staffline-clearance . 0.35)
	(details . ((ratio . 0.333) (height-limit . 1.0)))
	(thickness . 1.2)
	(x-gap . -0.1)
	(y-offset . 0.6)
	(minimum-length  . 2.5)
	(meta . ((interfaces . (tie-interface spanner-interface))))
	))

    (TieColumn
     . (
	(after-line-breaking-callback . ,Tie_column::after_line_breaking)
	(X-extent-callback . ())
	(Yoo-extent-callback . ())	
	(meta . ((interfaces . (tie-column-interface spanner-interface))))
	))

    (TimeSignature
     . (
	(molecule-callback . ,Time_signature::brew_molecule)
;	(molecule-callback . ,Text_item::brew_molecule)	
	(break-align-symbol . time-signature)
	(break-visibility . ,all-visible)
	(space-alist . (
			(first-note . (extra-space . 2.0))
			(right-edge . (extra-space . 0.5))
			(staff-bar .  (minimum-space . 2.0))
			))
	(breakable . #t)
	(style . C)
;	(text . (,time-signature-glue-markup)) 
	(font-family . number)
	(meta . ((interfaces . (time-signature-interface break-aligned-interface font-interface item-interface ))))
	))

    (TupletBracket
     . (
	(gap . 2.0)
	(padding . 0.9)
	(thickness . 1.6)
	(edge-height . (0.7 . 0.7))
	(shorten-pair . (-0.2 . -0.2))
	(before-line-breaking-callback . ,Tuplet_bracket::before_line_breaking)
	(after-line-breaking-callback . ,Tuplet_bracket::after_line_breaking)
	(molecule-callback . ,Tuplet_bracket::brew_molecule)
	(font-family . roman)
	(font-shape . italic)
	(font-series . bold)

	(font-relative-size . -1)
	(meta .  ((interfaces . (text-interface tuplet-bracket-interface font-interface spanner-interface))))
	))

    (UnaCordaPedal
     . (
	(molecule-callback . ,Text_item::brew_molecule)
	(font-family . roman)
	(font-shape . italic)
	(no-spacing-rods . #t)
	(self-alignment-X . 0)
	(direction . 1)
	(padding . 0.0)  ;; padding relative to UnaCordaPedalLineSpanner
	(X-offset-callbacks . (,Self_alignment_interface::aligned_on_self))
	(meta . ((interfaces . (text-interface self-alignment-interface font-interface item-interface ))))
	))

    (UnaCordaPedalLineSpanner 
     . (
	(axes . (1))
	(Y-extent-callback . ,Axis_group_interface::group_extent_callback)	
	(Y-offset-callbacks . (,Side_position_interface::aligned_side))
	
	(padding . 1.2)
	(minimum-space . 1.0)
	(direction . -1)
	(meta . ((interfaces . (piano-pedal-interface axis-group-interface side-position-interface spanner-interface))))
	))

    (VaticanaLigature
     . (
	(thickness . 0.6)
	(flexa-width . 2.0)
	(ligature-primitive-callback . ,Vaticana_ligature::brew_ligature_primitive)
	(molecule-callback . ,Vaticana_ligature::brew_molecule)
	(meta . ((interfaces . (vaticana-ligature-interface font-interface))))
	))

    (VoltaBracket
     . (
	(molecule-callback . ,Volta_bracket_interface::brew_molecule)
	(direction . 1)
	(padding . 1)
	(font-family . number)
	(Y-offset-callbacks . (,Side_position_interface::aligned_side))
	(thickness . 1.6)  ;  stafflinethickness
	(height . 2.0) ; staffspace;
	(minimum-space . 5)
	(font-family . number)
	(font-relative-size . -2)
	(meta . ((interfaces . (volta-bracket-interface text-interface side-position-interface font-interface spanner-interface))))
	))
    
    (VerticalAlignment
     . (
	(axes . (1))
	(Y-extent-callback . ,Axis_group_interface::group_extent_callback)
	(stacking-dir . -1)
	(meta . ((interfaces . (align-interface axis-group-interface spanner-interface))))
	))

    (VerticalAxisGroup
     . (
	(axes . (1))
	(Y-extent-callback . ,Axis_group_interface::group_extent_callback)	
	
	(meta . ((interfaces . (axis-group-interface spanner-interface))))
	))
   )
 )




(define (completize-grob-entry x)
  "transplant assoc key into 'name entry of 'meta of X
"
  (let* ((name-sym  (car x))
	 (grob-entry (cdr x))
	 (metaentry (cdr (assoc 'meta grob-entry)))
	 (ifaces-entry
	  (cdr (assoc 'interfaces metaentry)))

	)
    (set! metaentry (assoc-set! metaentry 'name name-sym))
    (set! metaentry (assoc-set! metaentry 'interfaces
				(cons 'grob-interface ifaces-entry)))
    (set! grob-entry (assoc-set! grob-entry 'meta metaentry))
    (cons name-sym grob-entry)))

(set! all-grob-descriptions (map completize-grob-entry all-grob-descriptions))



					;  (display  (map pair? all-grob-descriptions))


;; make sure that \property Foo.Bar =\turnOff doesn't complain

(map (lambda (x)
					; (display (car x)) (newline)

       (set-object-property! (car x) 'translation-type? list?)
       (set-object-property! (car x) 'is-grob? #t))
     all-grob-descriptions)


(set! all-grob-descriptions (sort all-grob-descriptions alist<?))
