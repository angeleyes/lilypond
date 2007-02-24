;;;; grob-property-description.scm -- part of generated backend documentation
;;;;
;;;;  source file of the GNU LilyPond music typesetter
;;;; 
;;;; (c) 1998--2006  Han-Wen Nienhuys <hanwen@xs4all.nl>
;;;;                 Jan Nieuwenhuizen <janneke@gnu.org>

(define (define-grob-property symbol type? description)
  (if (not (equal? (object-property symbol 'backend-doc) #f))
      (ly:error (_ "symbol ~S redefined") symbol))
  
  (set-object-property! symbol 'backend-type? type?)
  (set-object-property! symbol 'backend-doc description)
  symbol)

;; put this in an alist?
(define-public
  all-user-grob-properties

  (map
   (lambda (x)
     (apply define-grob-property x))

   `(
     (X-extent ,number-pair? "Hard coded extent in X@tie{}direction.")
     (X-offset ,number? "The horizontal amount that this object is
moved relative to its X-parent.")
     (Y-extent ,number-pair? "See @code{X-extent}.")
     (Y-offset ,number? "The vertical amount that this object is moved
relative to its Y-parent.")

     (add-stem-support ,boolean? "If set, the @code{Stem} object is
included in this script's support.") 
     (after-line-breaking ,boolean? "Dummy property, used to trigger
callback for @code{after-line-breaking}.")
     (align-dir ,ly:dir? "Which side to align? @code{-1}: left side,
@code{0}: around center of width, @code{1}: right side.")
     (allow-loose-spacing ,boolean? "If set, column can be detached
from main spacing.")
     (alteration ,number? "Alteration numbers for accidental.")
     (alteration-alist ,list? "List of @code{(@var{pitch}
. @var{accidental})} pairs for key signature.")
     (arpeggio-direction ,ly:dir? "If set, put an arrow on the
arpeggio squiggly line.")
     (arrow-length ,number? "Arrow length.")
     (arrow-width ,number? "Arrow width.")
     (auto-knee-gap ,ly:dimension? "If a gap is found between note
heads where a horizontal beam fits that is larger than this number,
make a kneed beam.")
     (average-spacing-wishes ,boolean? "If set, the spacing wishes
are averaged over staves.")
     (avoid-note-head ,boolean? "If set, the stem of a chord does not
pass through all note heads, but starts at the last note head.")
     (avoid-slur ,symbol? "Method of handling slur collisions.
Choices are @code{around}, @code{inside}, @code{outside}.  If unset,
scripts and slurs ignore each other.  @code{around} only moves the
script if there is a collision; @code{outside} always moves the
script.")
     (axes ,list? "List of axis numbers.  In the case of alignment
grobs, this should contain only one number.")

     (bar-size ,ly:dimension? "The size of a bar line.")
     (barre-type ,symbol? "Type of barre indication used in a fret
diagram.  Choices include @code{curved} and @code{straight}.")
     (base-shortest-duration ,ly:moment? "Spacing is based on the
shortest notes in a piece.  Normally, pieces are spaced as if notes
at least as short as this are present.")
     (baseline-skip ,ly:dimension? "Distance between base lines of
multiple lines of text.")
     (beam-thickness ,ly:dimension? "Beam thickness, measured in
@code{staff-space} units.")
     (beam-width ,ly:dimension? "Width of the tremolo sign.")
     (beamed-stem-shorten ,list? "How much to shorten beamed stems,
when their direction is forced.  It is a list, since the value is
different depending on the number of flags and beams.")
     (beaming ,pair? "Pair of number lists.  Each number list
specifies which beams to make.  @code{0}@tie{}is the central beam,
@code{1}@tie{}is the next beam toward the note, etc.  This
information is used to determine how to connect the beaming patterns
from stem to stem inside a beam.")
     (before-line-breaking ,boolean? "Dummy property, used to trigger
a callback function.")
     (between-cols ,pair? "Where to attach a loose column to.")
     (bound-padding ,number? "The amount of padding to insert around
spanner bounds.")
     (bound-details ,list? "An alist of properties for determining
attachments of spanners to edges.")
     (bracket-flare ,number-pair? "A pair of numbers specifying how
much edges of brackets should slant outward.  Value @code{0.0} means
straight edges.")
     (bracket-visibility ,boolean-or-symbol? "This controls the
visibility of the tuplet bracket.  Setting it to false prevents
printing of the bracket.  Setting the property to @code{if-no-beam}
makes it print only if there is no beam associated with this tuplet
bracket.")
     (break-align-symbol ,symbol? "This key is used for aligning and
spacing breakable items.")
     (break-align-orders ,vector? "Defines the order in which
prefatory matter (clefs, key signatures) appears.  The format is a
vector of length@tie{}3, where each element is one order for
end-of-line, middle of line, and start-of-line, respectively.  An
order is a list of symbols.

For example, clefs are put after key signatures by setting

@example
\\override Score.BreakAlignment #'break-align-orders =
  #(make-vector 3 '(span-bar
                    breathing-sign
                    staff-bar
                    key
                    clef
                    time-signature))
@end example")
     (break-overshoot ,number-pair? "How much does a broken spanner
stick out of its bounds?")
     (break-visibility ,vector? "A vector of 3@tie{}booleans,
@code{#(@var{end-of-line} @var{unbroken} @var{begin-of-line})}.
@code{#t} means visible, @code{#f} means killed.")
     (breakable ,boolean? "Allow breaks here.")

     (c0-position ,integer? "An integer indicating the position of
middle@tie{}C.")
     (clip-edges ,boolean? "Allow outward pointing beamlets at the
edges of beams?")
     (collapse-height ,ly:dimension? "Minimum height of system start
delimiter.  If equal or smaller, the bracket is removed.")
     (color ,color? "The color of this grob.")
     (common-shortest-duration ,ly:moment? "The most common shortest
note length.  This is used in spacing.  Enlarging this sets the score
tighter.")
     (concaveness ,number? "A beam is concave if its inner stems are
closer to the beam than the two outside stems.  This number is a
measure of the closeness of the inner stems.  It is used for damping
the slope of the beam.")
     (connect-to-neighbor ,pair? "Pair of booleans, indicating whether
this grob looks as a continued break.")
     (control-points ,list? "List of offsets (number pairs) that form
control points for the tie, slur, or bracket shape.  For B@'eziers,
this should list the control points of a third-order B@'ezier curve.")

     (damping ,number? "Amount of beam slope damping.")
     (dash-fraction ,number? "Size of the dashes, relative to
@code{dash-period}.  Should be between @code{0.0} (no line) and
@code{1.0} (continuous line).")
     (dash-period ,number? "The length of one dash together with
whitespace.  If negative, no line is drawn at all.")
     (default-direction ,ly:dir? "Direction determined by note head
positions.")
     (digit-names ,vector "Names for string finger digits.")
     (direction ,ly:dir? "If @code{side-axis} is @code{1} (or
@code{#X}), then this property determines whether the object is placed
@code{#LEFT}, @code{#CENTER} or @code{#RIGHT} with respect to the
other object.  Otherwise, it determines whether the object is placed
@code{#UP}, @code{#CENTER} or #DOWN.  Numerical values may also be
used: @code{#UP}=@code{1}, @code{#DOWN}=@code{-1},
@code{#LEFT}=@code{-1}, @code{#RIGHT}=@code{1}, @code{CENTER}=@code{0}
but also other numerical values are permitted.")
     (dot-color ,symbol? "Color of dots.  Options include 
@code{black} and @code{white}.")
     (dot-count ,integer? "The number of dots.")
     (dot-radius ,number? "Radius of dots.")
     (duration-log ,integer? "The 2-log of the note head duration,
i.e. @code{0} = whole note, @code{1} = half note, etc.")

     (eccentricity ,number? "How asymmetrical to make a slur.
Positive means move the center to the right.")
     (edge-height ,pair? "A pair of numbers specifying the heights of
the vertical edges: @code{(@var{left-height} . @var{right-height})}.")
     (edge-text ,pair? "A pair specifying the texts to be set at the
edges: @code{(@var{left-text} . @var{right-text})}.")
     (expand-limit ,integer? "Maximum number of measures expanded in
church rests.")
     (extra-X-extent ,number-pair? "A grob is enlarged in
X@tie{}dimension by this much.")
     (extra-Y-extent ,number-pair? "See @code{extra-X-extent}.")
     ;; remove me?
     (extra-dy ,number? "Slope glissandi this much extra.")
     (extra-offset ,number-pair? "A pair representing an offset.  This
offset is added just before outputting the symbol, so the typesetting
engine is completely oblivious to it.")
     (extra-spacing-width ,number-pair? "In the horizontal spacing
problem, we pad each item by this amount (by adding the @q{car} on the
left side of the item and adding the @q{cdr} on the right side of the
item).  In order to make a grob take up no horizontal space at all,
set this to @code{(+inf.0 . -inf.0)}.")

     (finger-code ,symbol? "Code for the type of fingering indication
in a fret diagram.  Options include @code{none}, @code{in-dot}, and
@code{below-string}.")
     (flag-count ,number? "The number of tremolo beams.")
     (flag-style ,symbol? "A string determining what style of flag
glyph is typeset on a @code{Stem}.  Valid options include @code{()}
and @code{mensural}.  Additionally, @code{no-flag} switches off the
flag.")
     (font-encoding ,symbol? "The font encoding is the broadest
category for selecting a font.  Options include: @code{fetaMusic},
@code{fetaNumber}, @code{TeX-text}, @code{TeX-math},
@code{fetaBraces}, @code{fetaDynamic}.")
     (font-family ,symbol? "The font family is the broadest category
for selecting text fonts.  Options include: @code{sans},
@code{roman}.")
     (font-name ,string? "Specifies a file name (without extension)
of the font to load.  This setting overrides selection using
@code{font-family}, @code{font-series} and @code{font-shape}.")
     (font-series ,symbol? "Select the series of a font.  Choices
include @code{medium}, @code{bold}, @code{bold-narrow}, etc.")
     (font-shape ,symbol? "Select the shape of a font.  Choices
include @code{upright}, @code{italic}, @code{caps}.")
     (font-size ,number? "The font size, compared to the @q{normal}
size.  @code{0}@tie{}is style-sheet's normal size, @code{-1} is
smaller, @code{+1} is bigger. Each step of@tie{}1 is approximately
12% larger, 6@tie{}steps are exactly a factor@tie{}2 larger.
Fractional values are allowed.")
     (force-hshift ,number? "This specifies a manual shift for notes
in collisions.  The unit is the note head width of the first voice
note.  This is used by @internalsref{note-collision-interface}.")
     (forced ,boolean? "Manually forced accidental.")
     (fraction ,number-pair? "Numerator and denominator of a time
signature object.")
     (french-beaming ,boolean? "Use French beaming style for this
stem.  The stem stops at the innermost beams.")
     (fret-count ,integer? "The number of frets in a fret diagram.")
     ;; ugh: double, change.
     (full-size-change ,boolean? "Don't make a change clef smaller.")

     (gap ,ly:dimension? "Size of a gap in a variable symbol.")
     (gap-count ,integer? "Number of gapped beams for tremolo.")
     (glyph ,string? "A string determining what @q{style} of glyph is
typeset.  Valid choices depend on the function that is reading this
property.")
     (glyph-name-alist ,list? "An alist of key-string pairs.")
     (grow-direction ,ly:dir? "Crescendo or decrescendo?")

     (hair-thickness ,number? "Thickness of the thin line in a bar
line.")
     (head-direction ,ly:dir? "Are the note heads left or right in a
semitie?")
     (height ,ly:dimension? "Height of an object in
@code{staff-space} units.")
     (height-limit ,ly:dimension? "Maximum slur height: The longer the
slur, the closer it is to this height.")
     (horizontal-shift ,integer? "An integer that identifies ranking
of @code{NoteColumn}s for horizontal shifting.  This is used by
@internalsref{note-collision-interface}.")

     (ignore-collision ,boolean? "If set, don't do note collision
resolution on this @code{NoteColumn}.")
     (implicit ,boolean? "Is this an implicit bass figure?")
     (infinite-spacing-height ,boolean? "If true, then for the
purposes of horizontal spacing, treat this item as though it were
infinitely tall.  That is, no object from another column is allowed to
stick above or below this item.")
     (inspect-index ,integer? "If debugging is set, set beam and slur
configuration to this index, and print the respective scores.")
     (inspect-quants ,number-pair? "If debugging is set,
set beam and slur quants to this position, and print the respective
scores.")

     (keep-fixed-while-stretching ,boolean? "A grob with this property
set to true is fixed relative to the staff above it when systems are
stretched.")
     (keep-inside-line ,boolean? "If set, this column cannot have
things sticking into the margin.")
     (kern ,ly:dimension? "Amount of extra white space to add.  For
bar lines, this is the amount of space after a thick line.")
     (knee ,boolean? "Is this beam kneed?")
     (knee-spacing-correction ,number? "Factor for the optical
correction amount for kneed beams.  Set between @code{0} for no
correction and @code{1} for full correction.")

     (label-dir ,ly:dir? "Side to which a label is attached.
@code{-1} for left, @code{1}@tie{}for right.")
     (layer ,number? "The output layer (a value between 0 and@tie{}2:
Layers define the order of printing objects.  Objects in lower layers
are overprinted by objects in higher layers.")
     (ledger-line-thickness ,number-pair? "The thickness of ledger
lines.  It is the sum of 2@tie{}numbers: The first is the factor for
line thickness, and the second for staff space.  Both contributions
are added.")
     (left-bound-info ,list? "An alist of properties for determining
attachments of spanners to edges.")
     (left-padding ,ly:dimension? "The amount of space that is put
left to an object (e.g., a group of accidentals).")
     (length ,ly:dimension? "User override for the stem length of
unbeamed stems.")
     (length-fraction ,number? "Multiplier for lengths.  Used for
determining ledger lines and stem lengths.")
     (line-break-penalty ,number? "Penalty for a line break at this
column.  This affects the choices of the line breaker; it avoids a
line break at a column with a positive penalty and prefer a line break
at a column with a negative penalty.")
     (line-break-permission ,symbol? "Instructs the line breaker on
whether to put a line break at this column.  Can be @code{force} or
@code{allow}.")
     (line-break-system-details ,list? "An alist of properties to use
if this column is the start of a system.")
     (line-count ,integer? "The number of staff lines.")
     (line-positions ,list? "Vertical positions of staff lines.")
     (line-thickness ,number? "The thickness of the tie or slur
contour.")
     (long-text ,markup? "Text markup.  See @usermanref{Text
markup}.")

     (max-beam-connect ,integer? "Maximum number of beams to connect
to beams from this stem.  Further beams are typeset as beamlets.")
     (max-stretch ,number? "The maximum amount that this
@code{VerticalAxisGroup} can be vertically stretched (for example, in
order to better fill a page).")
     (measure-count ,integer? "The number of measures for a
multi-measure rest.")
     (measure-length ,ly:moment? "Length of a measure.  Used in some
spacing situations.")
     (merge-differently-dotted ,boolean? "Merge note heads in
collisions, even if they have a different number of dots.  This is
normal notation for some types of polyphonic music.

@code{merge-differently-dotted} only applies to opposing stem
directions (i.e. voice 1 &@tie{}2).")
     (merge-differently-headed ,boolean? "Merge note heads in
collisions, even if they have different note heads.  The
smaller of the two heads is rendered invisible.  This is used in
polyphonic guitar notation.  The value of this setting is used by
@internalsref{note-collision-interface}.

@code{merge-differently-headed} only applies to opposing stem
directions (i.e. voice 1 &@tie{}2).")
     (minimum-X-extent ,number-pair? "Minimum size of an object in
X@tie{}dimension, measured in @code{staff-space} units.")
     (minimum-Y-extent ,number-pair? "See @code{minimum-X-extent}.")
     (minimum-distance ,ly:dimension? "Minimum distance between rest
and notes or beam.")
     (minimum-length ,ly:dimension? "Try to make a spanner at least
this long, normally in the horizontal direction.  This requires an
appropriate callback for the @code{springs-and-rods} property.  If
added to a @code{Tie}, this sets the minimum distance between
noteheads.")
     (minimum-length-fraction ,number? "Minimum length of ledger line
as fraction of note head size.")
     (minimum-space ,ly:dimension? "Minimum distance that the victim
should move (after padding).")

     (neutral-direction ,ly:dir? "Which direction to take in the
center of the staff.")
     (neutral-position ,number? "Position (in half staff spaces) where
to flip the direction of custos stem.")
     (next ,ly:grob? "Object that is next relation (e.g., the lyric
syllable following an extender.")
     (no-ledgers ,boolean? "If set, don't draw ledger lines on this
object.")
     (no-stem-extend ,boolean? "If set, notes with ledger lines do not
get stems extending to the middle staff line.")
     (non-default ,boolean? "Set for manually specified clefs.")
     (non-musical ,boolean? "True if the grob belongs to a
@code{NonMusicalPaperColumn}.")
     (note-names ,vector? "Vector of strings containing names for
easy-notation note heads.")
     (number-type ,symbol? "Type of numbers to use in label.  Choices
include @code{roman-lower}, @code{roman-upper}, and @code{arabic}.")

     (outside-staff-horizontal-padding ,number? "By default, an
outside-staff-object can be placed so that is it very close to another
grob horizontally.  If this property is set, the outside-staff-object
is raised so that it is not so close to its neighbour.")
     (outside-staff-padding ,number? "The padding to place between
this grob and the staff when spacing according to
@code{outside-staff-priority}.")
     (outside-staff-priority ,number? "If set, the grob is positioned
outside the staff in such a way as to avoid all collisions.
In case of a potential collision, the grob with the smaller
@code{outside-staff-priority} is closer to the staff.")

     (packed-spacing ,boolean? "If set, the notes are spaced as
tightly as possible.")
     (padding ,ly:dimension? "Add this much extra space between
objects that are next to each other.")
     (page-break-penalty ,number? "Penalty for page break at this
column.  This affects the choices of the page breaker; it avoids a
page break at a column with a positive penalty and prefer a page break
at a column with a negative penalty.")
     (page-break-permission ,symbol? "Instructs the page breaker on
whether to put a page break at this column.  Can be @code{force} or
@code{allow}.")
     (page-turn-penalty ,number? "Penalty for a page turn at this
column.  This affects the choices of the page breaker; it avoids a
page turn at a column with a positive penalty and prefer a page turn
at a column with a negative penalty.")
     (page-turn-permission ,symbol? "Instructs the page breaker on
whether to put a page turn at this column.  Can be @code{force} or
@code{allow}.")
     (parenthesized ,boolean? "Parenthesize this grob.")
     (positions ,pair? "Pair of staff coordinates @code{(@var{left}
. @var{right})}, where both @var{left} and @var{right} are in
@code{staff-space} units of the current staff.")

     (ratio ,number? "Parameter for slur shape.  The higher this
number, the quicker the slur attains its @code{height-limit}.")
     (remove-empty ,boolean? "If set, remove group if it contains no
interesting items.")
     (remove-first ,boolean? "Remove the first staff of a orchestral
score?")
     (restore-first ,boolean? "Print a natural before the
accidental.")
     (rhythmic-location ,rhythmic-location? "Where (bar number,
measure position) in the score.")
     (right-bound-info ,list? "An alist of properties for determining
attachments of spanners to edges.")
     (right-padding ,ly:dimension? "Space to insert on the right side
of an object (e.g., between note and its accidentals).")
     (rotation ,list? "Number of degrees to rotate this object, and
what point to rotate around.  For example, @code{#'(45 0 0)} rotates
by 45 degrees around the center of this object.")

     (same-direction-correction ,number? "Optical correction amount
for stems that are placed in tight configurations.  This amount is
used for stems with the same direction to compensate for note-head to
stem distance.")
     (script-priority ,number? "A sorting key that determines in what
order a script is within a stack of scripts.")
     (self-alignment-X ,number? "Specify alignment of an object.  The
value @code{-1} means left aligned, @code{0}@tie{}centered, and
@code{1}@tie{}right-aligned in X@tie{}direction.  Values in-between
may also be specified.")
     (self-alignment-Y ,number? "Like @code{self-alignment-X} but for
the Y@tie{}axis.")
     (shorten-pair ,number-pair? "The lengths to shorten a
text-spanner on both sides, for example a pedal bracket.  Positive
values shorten the text-spanner, while negative values lengthen it.")
     (shortest-duration-space ,ly:dimension? "Start with this much
space for the shortest duration.  This is expressed in
@code{spacing-increment} as unit.  See also
@internalsref{spacing-spanner-interface}.")
     (shortest-playing-duration ,ly:moment? "The duration of the
shortest playing here.")
     (shortest-starter-duration ,ly:moment? "The duration of the
shortest note that starts here.")
     (side-axis ,number? "If the value is @code{#X} (or
equivalently@tie{}@code{1}), the object is placed horizontally next
to the other object.  If the value is @code{#Y} or@tie{}@code{0}, it
is placed vertically.")
     (side-relative-direction ,ly:dir? "Multiply direction of
@code{direction-source} with this to get the direction of this
object.")
     (size ,number? "Size of object, relative to standard size.")
     (skylines ,ly:skyline-pair? "Two skylines, one above and one
below this grob (or, for some grobs, to the left and to the right).")
     (slope ,number? "The slope of this object.")
     (slur-padding ,number? "Extra distance between slur and script.")
     (space-alist ,list? "A table that specifies distances between
prefatory items, like clef and time-signature.  The format is an alist
of spacing tuples: @code{(@var{break-align-symbol} @var{type}
. @var{distance})}, where @var{type} can be the symbols
@code{minimum-space} or @code{extra-space}.")
     (spacing-increment ,number? "Add this much space for a doubled
duration.  Typically, the width of a note head. See also
@internalsref{spacing-spanner-interface}.")
     (springs-and-rods ,boolean? "Dummy variable for triggering
spacing routines.")
     (stacking-dir ,ly:dir? "Stack objects in which direction?")
     (staff-padding ,ly:dimension? "Maintain this much space between
reference points and the staff.  Its effect is to align objects of
differing sizes (like the dynamic @b{p} and @b{f}) on their
baselines.")
     (staff-position ,number? "Vertical position, measured in half
staff spaces, counted from the middle line.")
     (staff-space ,ly:dimension? "Amount of space between staff lines,
expressed in global @code{staff-space}.")
     (stem-attachment ,number-pair? "A @code{(@var{x} . @var{y})} pair
where the stem attaches to the notehead.")
     (stem-end-position ,number? "Where does the stem end (the end is
opposite to the support-head)?")
     ;;[TODO: doco]
     (stem-spacing-correction ,number? "Optical correction amount for
stems that are placed in tight configurations.  For opposite
directions, this amount is the correction for two normal sized stems
that overlap completely.")
     (stemlet-length ,number? "How long should a stem over a rest
be?")
     (stencil ,ly:stencil? "The symbol to print.")
     (stencils ,list? "Multiple stencils, used as intermediate
value.")
     (strict-grace-spacing ,boolean? "If set, grace notes 
are not spaced separately, but put before musical columns.")
     (strict-note-spacing ,boolean? "If set, unbroken columns
with non-musical material (clefs, barlines, etc.) are not spaced
separately, but put before musical columns.")
     (string-count ,integer? "The number of strings in a fret
diagram.")
     (string-fret-finger-combinations ,list? "List consisting of
@code{(@var{string-number} @var{fret-number} @var{finger-number})}
entries.")
     (stroke-style ,string? "Set to @code{\"grace\"} to turn stroke
through flag on.")
     (style ,symbol? "This setting determines in what style a grob is
typeset.  Valid choices depend on the @code{stencil} callback reading
this property.")

     (text ,markup? "Text markup.  See @usermanref{Text markup}.")
;;FIXME -- Should both be the same?
     (text-direction ,ly:dir? "This controls the ordering of the
words.  The default @code{RIGHT} is for roman text.  Arabic or Hebrew
should use @code{LEFT}.")
     (thick-thickness ,number? "Bar line thickness, measured in
@code{line-thickness}.")
     (thickness ,number? "Line thickness, generally measured in
@code{line-thickness}.")
     (thin-kern ,number? "The space after a hair-line in a bar line.")
     (threshold ,number-pair? "@code{(@var{min} . @var{max})}, where
@var{min} and @var{max} are dimensions in staff space.")
     (tie-configuration ,list? "List of @code{(@var{position} .
@var{dir})} pairs, indicating the desired tie configuration, where
@var{position} is the offset from the center of the staff in staff
space and @var{dir} indicates the direction of the tie
(@code{1}=>up, @code{-1}=>down, @code{0}=>center).  A non-pair entry
in the list causes the corresponding tie to be formatted
automatically.")
     (transparent ,boolean? "This makes the grob invisible.")

     (uniform-stretching ,boolean? "If set, items stretch
proportionally to their durations.  This looks better in complex
polyphonic patterns.")
     (used ,boolean? "If set, this spacing column is kept in the
spacing problem")

     (when ,ly:moment? "Global time step associated with this column
happen?")
     (width ,ly:dimension? "The width of a grob measured in staff
space.")
     (word-space ,ly:dimension? "Space to insert between words in
texts.")

     (zigzag-length ,ly:dimension? "The length of the lines of a
zigzag, relative to @code{zigzag-width}.  A value of@tie{}@code{1}
gives 60-degree zigzags.")
     (zigzag-width ,ly:dimension? "The width of one zigzag squiggle.
This number is adjusted slightly so that the glissando line can
be constructed from a whole number of squiggles.")
     )))


;;;;;;;;;;;;;;;;
;;   INTERNAL


(define (define-internal-grob-property symbol type? description)
  (define-grob-property symbol type? description)
  (set-object-property! symbol 'backend-internal #t)
  symbol
  )


(define-public all-internal-grob-properties
  (map
   (lambda (x)
     (apply define-internal-grob-property x))
   
   `(
     ;;;;;;;;;;;;;;;;
     ;; grobs & grob arrays. (alphabetical)
     (X-common ,ly:grob? "Common refpoint for axis group.")
     (Y-common ,ly:grob? "See @code{X-common}.")
     (cached-pure-extents ,vector? "Used by a VerticalAxisGroup to cache the Y-extents of different column ranges.")
     (axis-group-parent-X ,ly:grob? "Containing X axis group")
     (axis-group-parent-Y ,ly:grob? "Containing Y axis group")
     (accidental-grobs ,list? "Alist with (NOTENAME . GROBLIST) entries")
     (adjacent-hairpins ,ly:grob-array? "List of directly neighboring hairpins")
     (all-elements ,ly:grob-array? "list of all grobs in this line. Its
function is to protect objects from being garbage collected.")
     (arpeggio ,ly:grob? "pointer to arpeggio object.")
     (beam ,ly:grob? "pointer to the beam, if applicable.")
     (bracket ,ly:grob? "the bracket for a  number.")
     (cross-staff ,boolean? "for a beam or a stem, true if we depend on inter-staff spacing")
     (direction-source ,ly:grob? "in case side-relative-direction is
set, which grob to get the direction from .")
     (dot ,ly:grob? "reference to Dots object.")
     (dots ,ly:grob-array? "multiple Dots objects.")
     (figures ,ly:grob-array? "Figured bass objects for continuation line.")
     (important-column-ranks ,vector? "Cache of columns that contain items-worth-living.")
     (items-worth-living ,ly:grob-array? "A list of interesting items. If
empty in a particular staff, then that staff is erased.")

     (glyph-name ,string? "a name of character within font.")
     (left-neighbors ,ly:grob-array? " List of
spacing-wish grobs that are close to the current column.

The closest spacing-wishes determine the actual distances between the
columns.
")
     (left-items ,ly:grob-array? "")
     (pedal-text ,ly:grob? "Pointer to the text of a mixed-style piano pedal.")
     
     (pure-Y-common ,ly:grob? "Caches the common_refpoint_of_array of the elements grob-set")
     (pure-relevant-elements ,ly:grob-array? "The subset of elements that are relevant for finding the pure-Y-extent.")
     (stem ,ly:grob? "pointer to Stem object.")
     (tremolo-flag ,ly:grob? "The tremolo object on a stem.")
     (tie ,ly:grob? "")
     (staff-symbol ,ly:grob? "the staff symbol grob that we're in.")
     (rest ,ly:grob? "the staff symbol grob that we're in.")
     (rests ,ly:grob-array? "A list of rest objects.")
     (rest-collision ,ly:grob? "rest collision that a rest is in.")
     (accidental-grob ,ly:grob? "Accidental for this note.")
     (bars ,ly:grob-array? "list of bar line pointers.")
     (bounded-by-me ,ly:grob-array? "list of spanners that have this
column as start/begin point. Only columns that have grobs or act as
bounds are spaced.")
     
     (columns ,ly:grob-array? "list of grobs, typically containing
paper-columns or note-column objects.")
     (conditional-elements ,ly:grob-array? "Internal use only")
     (encompass-objects ,ly:grob-array? "Objects that a slur should avoid
in addition to notes and stems.")
     (elements ,ly:grob-array? "list of grobs, type depending on the Grob
where this is set in.")
     (grace-spacing ,ly:grob? "a run of grace notes.")
     (heads ,ly:grob-array? "List of note heads.")

     (note-columns ,pair? "list of NoteColumn grobs.")

     (normal-stems ,ly:grob-array? "Array of visible stems.") 
     (note-heads ,ly:grob-array? "List of note head grobs")
     (note-head ,ly:grob? "A single note head")
     (right-items ,ly:grob-array? "")
     (right-neighbors ,ly:grob-array? "see left-neighbors")
     (separation-item ,ly:grob? "A separation item.")
     (slur ,ly:grob? "A pointer to a slur object")
     (spacing ,ly:grob? "the spacing spanner governing this section.")
     (spaceable-staves ,ly:grob-array? "Objects to be spaced during page layout.")
     (side-support-elements ,ly:grob-array? "the support, a list of grobs.")
     (spacing-wishes ,ly:grob-array? "List of note spacing or staff spacing objects.")
     (stems ,ly:grob-array? "list of stem objects, corresponding to the notes that the arpeggio has to be before.")
     (tuplets ,ly:grob-array? "list of smaller tuplet brackets")
     (tuplet-number ,ly:grob? "the number for a bracket.")

     
     
     ;;;;;;;;;;;;;;;;
     ;; other
     (begin-of-line-visible ,boolean? "Used for marking ChordNames that should only show changes.")
     (cause ,scheme? "Any kind of causation objects (i.e. music, or perhaps translator) that was the cause for this grob.  ")
     (circled-tip ,boolean? "Put a circle at start/end of hairpins (al/del niente)")
     (delta-position ,number? "vertical position difference")
     (details ,list? "alist of parameters for detailed grob behavior.

more information on the allowed parameters can be found by inspecting
lily/slur-scoring.cc, lily/beam-quanting.cc, and
lily/tie-formatting-problem.cc.  Setting @code{debug-tie-scoring},
@code{debug-beam-scoring} or @code{debug-slur-scoring} also provides
useful clues.

")

     (font ,ly:font-metric? "Cached font metric object")
     (head-width ,ly:dimension? "width of this ligature head")
     
     (ideal-distances ,list? "(@var{obj} . (@var{dist} . @var{strength})) pairs.")
     (interfaces ,list? "list of symbols indicating the interfaces supported by this object. Is initialized from the @code{meta} field.")
     (least-squares-dy ,number? 
		       "ideal beam slope, without damping.")

     (meta ,list? "Contains meta information. It is an alist with the
entries @code{name} and @code{interfaces}.")
     (minimum-distances ,list? "list of rods, that have the format (@var{obj} . @var{dist}).")
     
     (positioning-done ,boolean?
		       "Used to signal that a positioning element
did its job. This ensures that a positioning is only done once.")
     (pure-Y-extent ,number-pair? "The estimated height of a system")

     (quantized-positions ,number-pair? "Beam positions after quanting.")
     (quantize-position ,boolean? "If set, a vertical alignment is aligned to be within staff spaces.")
     (quant-score ,string? "Beam quanting score -- can be stored for
debugging")
     
     (script-stencil ,pair? "Pair (@code{type} . @code{arg}), which
acts as an index for looking up a Stencil object.")

     (stem-info ,pair? "caching of stem parameters")
     (shorten ,ly:dimension? "The amount of space that a
stem. Internally used to distribute beam shortening over stems. ")
     (skyline-distance ,number? "The distance between this staff and the next one, as determined by a skyline algorithm.")
     (skyline-horizontal-padding ,number? "For determining the vertical distance between 2 staves, it is possible to have a configuration which would result in a tight interleaving of grobs from the top staff and the bottom staff. The larger this parameter is, the farther apart the staves are placed in such a configuration.")
 
     (use-breve-rest ,boolean? "Use breve rests for measures longer
than a whole rest.")
     

     
     ;; ancient notation
     (join-right-amount ,number? "")
     (primitive ,integer? "Pointer to a ligature primitive, i.e. an item similar to a note head that is part of a ligature. ")

     
     ;;;;;;; TODO:
     ;; there are too many properties for ancient notation
     ;; probably neume-types (a list of symbols) would also work.

     ;; However, such this list would consist of a couple of dozens of
     ;; entries, since head prefixes may be combined in many ways.  If
     ;; the macros in gregorian-init.ly would directly set prefix-set,
     ;; all the head prefixes could be junked; however, such macros
     ;; would be quite numerous, I guess.  --jr

     (auctum ,boolean? "is this neume liquescentically augmented?")
     (ascendens ,boolean? "is this neume of an ascending type?")
     (add-cauda ,boolean? "does this flexa require an additional cauda on the left side?")
     (add-join ,boolean? "is this ligature head joined with the next one by a vertical line?")
     (cavum ,boolean? "is this neume outlined?")
     (descendens ,boolean? "is this neume of a descendent type?")
     (deminutum ,boolean? "is this neume deminished?")
     (flexa-height ,ly:dimension? "height of a flexa shape in a ligature grob in staff_space.")
     (flexa-width ,ly:dimension? "width of a flexa shape in a ligature grob in staff_space.")
     (join-heads ,boolean? "Whether to join the note heads of an ambitus grob with a vertical line.")
     (linea ,boolean? "attach vertical lines to this neume?")
     (add-stem ,boolean? "is this ligature head a virga and therefore needs an additional stem on the right side?")
     (context-info ,integer? "Within a ligature, the final glyph or shape of a head may be affected by the left and/or right neighbour head.  context-info holds for each head such information about the left and right neighbour, encoded as a bit mask.")
     (inclinatum ,boolean? "is this neume an inclinatum?")
     (oriscus ,boolean? "is this neume an oriscus?")
     (quilisma ,boolean? "is this neume a quilisma?")
     (pes-or-flexa ,boolean? "shall this neume be joined with the previous head?")
     (prefix-set ,number? "a bit mask that holds all Gregorian head prefixes, such as @code{\\virga} or @code{\\quilisma}")
     (stropha ,boolean? "Is this neume a stropha?")
     (virga ,boolean? "Is this neume a virga?")
     (x-offset ,ly:dimension? "Extra horizontal offset for ligature heads.")

     ;; end ancient notation

     )))

(define-public all-backend-properties
  (append
   all-internal-grob-properties
   all-user-grob-properties))
