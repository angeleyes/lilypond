; The GIMP -- an image manipulation program
; Copyright (C) 1995 Spencer Kimball and Peter Mattis
; 
; Aqua Pill --- create an aqua pill web button
; Copyright (C) 2001 Iccii <iccii@hotmail.com>
; 
; This script was born of follow URL idea
;   http://www.gw.total-web.net/~chr/tutorials/aqua/aqua_tutorial.html
; Above URL is missing, so I found mirrored page
;   http://cad.ntu-kpi.kiev.ua/~netlib/graph/gimp/Aqua/aqua_tutorial.html
;
; gimp.br (which country?)
;   http://www.gimp.hpg.ig.com.br/tutorial15/index.htm
; This page has been translated in Japanese
;   http://www.gnome.gr.jp/~komatsu/gimp-tut/others/aqua/aqua_tutorial.html
; 
; --------------------------------------------------------------------
;   - Changelog -
;;; version 0.7 Tue Apr  1 2003 janneke@gnu.org
;;;   (script-fu-aqua-pill-button): Resize button to image
;;;   (script-fu-aqua-pill-button): Return image
; version 0.1  2001/03/06 Iccii <iccii@hotmail.com>
;     - Initial relase
; version 0.2  2001/03/08 Iccii <iccii@hotmail.com>
;     - Move the logo generator function from base code to extra code
; version 0.3  2001/03/13 Iccii <iccii@hotmail.com>
;     - Create an Aqua Pill hrule
; version 0.4  2001/03/21 Iccii <iccii@hotmail.com>
;     - Create a Round Aqua Pill
; version 0.5  2001/03/25 Iccii <iccii@hotmail.com>
;     - Add the "remove-dust" function to remove the dust images
; version 0.5a 2001/04/02 Iccii <iccii@hotmail.com>
;     - Add antialias option
;     - Font size calculation make better
; version 0.5b 2001/05/25 Iccii <iccii@hotmail.com>
;     - A bit optimize
; version 0.5c 2001/06/21 Iccii <iccii@hotmail.com>
;     - Minor fix
; version 0.5d 2001/08/10 Iccii <iccii@hotmail.com>
;     - Delete the "remove-dust" function
; version 0.6  2001/09/17 lasm <lasm@rocketmail.com>
;     - Fixed the "Flatten" bug
; version 0.6a 2001/09/25 Iccii <iccii@hotmail.com>
;     - More better change in 0.6 change
;     - Remove Japanese character in this script
;       and add English comment, instead
; version 0.6b 2001/11/27 Iccii <iccii@hotmail.com>
;     - Added Background Color option
;     - Added Add Glow option (thanks to S Myers)
;
; --------------------------------------------------------------------
; 
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
;


(define (script-fu-aqua-pill-common
				img		; IMAGE
				base-color	; Color of image
				bg-color	; Backgruond color
				width		; Width
				height		; Height
				xpadding	; X padding
				ypadding	; Y padding
				ratio		; Round ratio
				blur		; Amount of blur
				shadow		; Enable or disable the shadow
				add-glow	; Add glow layer?
				antialias	; Enable or disable the antialias
				flatten)	; Flat all layer?

	; Remove corner od selection
  (define (remove-selection-corner img amount)
    (gimp-selection-feather img amount)
    (gimp-selection-sharpen img))

	; Create the round selection
  (define (round-select img x y width height ratio)
    (let* ((diameter (* ratio height)))
      (gimp-ellipse-select img x y diameter height ADD FALSE 0 0)
      (gimp-ellipse-select img (+ x (- width diameter)) y
			       diameter height ADD FALSE 0 0)
      (gimp-rect-select img (+ x (/ diameter 2)) y
		            (- width diameter) height ADD FALSE 0)))

  (let* (
	 (old-fg-color (car (gimp-palette-get-foreground)))
	 (old-base-color (car (gimp-palette-get-background)))
	 (shadow-height (if (eqv? shadow TRUE) 1 0))
	 (bluramount (if (> width height) (* blur height) (* blur width)))
	 (shiftx     (/ height 2))
	 (shifty     (/ height 2))
	 (width-all  (+ width  (* 2 shiftx)))
	 (height-all (+ height (* 2 shifty) (* height shadow-height 0.5)))
	 (whratio (sqrt (/ height width)))

	 (highlight-upper1-layer (car (gimp-layer-new img width-all height-all
	                                              RGBA_IMAGE "Highlight Upper1"  75 SCREEN)))
	 (highlight-upper2-layer (car (gimp-layer-new img width-all height-all
	                                              RGBA_IMAGE "Highlight Upper2"  40 SCREEN)))
	 (highlight-lower (car (gimp-layer-new img width-all height-all
	                                       RGBA_IMAGE "Highlight Lower" 100 OVERLAY)))
	 (base1-layer (car (gimp-layer-new img width-all height-all
	                                   RGBA_IMAGE "Base Layer1"  75 SCREEN)))
	 (base2-layer (car (gimp-layer-new img width-all height-all
	                                   RGBA_IMAGE "Base Layer2" 100 NORMAL)))
	)    ; end variable definition


	;; Start

	(gimp-image-undo-disable img)
	(gimp-selection-none img)
	(gimp-palette-set-foreground base-color)
	(gimp-palette-set-background '(255 255 255))
	(gimp-drawable-fill base2-layer TRANS-IMAGE-FILL)
	(gimp-drawable-fill base1-layer TRANS-IMAGE-FILL)
	(gimp-drawable-fill highlight-lower TRANS-IMAGE-FILL)
	(gimp-drawable-fill highlight-upper2-layer TRANS-IMAGE-FILL)
	(gimp-drawable-fill highlight-upper1-layer TRANS-IMAGE-FILL)


	;; Drop shadow (if enabled)

    (if (eqv? shadow TRUE)
	(let* ((shadow1-layer (car (gimp-layer-new img width-all height-all
	                                           RGBA_IMAGE "Shadow Layer1" 100 SCREEN)))
	       (shadow2-layer (car (gimp-layer-new img width-all height-all
	                                           RGBA_IMAGE "Shadow Layer2" 100 NORMAL))))
	  (gimp-image-add-layer img shadow2-layer -1)
	  (gimp-image-add-layer img shadow1-layer -1)
	  (gimp-drawable-fill shadow2-layer TRANS-IMAGE-FILL)
	  (gimp-drawable-fill shadow1-layer TRANS-IMAGE-FILL)

	;; Drop shadow layer 1

	(round-select img shiftx shifty width height ratio)
	  (gimp-selection-shrink img (* height 0.1))
	  (gimp-edit-fill shadow1-layer FG-IMAGE-FILL)
	  (gimp-hue-saturation shadow1-layer 0 0 75 0)
	  (gimp-selection-grow img (+ 1 (* bluramount 0.3)))	; Faster?
	  (plug-in-gauss-iir2 1 img shadow1-layer
	                      (+ 1 (* bluramount 0.3)) (+ 1 (* bluramount 0.3)))
	  (gimp-channel-ops-offset shadow1-layer 0 1 0 (* height 0.4))
	  (gimp-selection-none img)

	;; Drop shadow layer 2

	(round-select img shiftx shifty width height ratio)
	  (gimp-edit-fill shadow2-layer FG-IMAGE-FILL)
	  (gimp-hue-saturation shadow2-layer 0 0 0 -25)
	  (gimp-selection-grow img (+ 1 (* bluramount 0.3)))	; Faster?
	  (plug-in-gauss-iir2 1 img shadow2-layer
	                      (+ 1 (* bluramount 0.3)) (+ 1 (* bluramount 0.3)))
	  (gimp-channel-ops-offset shadow2-layer 0 1 0 (* height 0.4))
	  (gimp-selection-none img)

    )) ;; end of drop shadow effect


	;; Base layer 1,2

    (round-select img shifty shifty width height ratio)
	(gimp-edit-fill base2-layer FG-IMAGE-FILL)
	(gimp-hue-saturation base2-layer 0 0 -75 -50)
	(gimp-selection-shrink img (* height 0.01))
	(gimp-edit-fill base1-layer FG-IMAGE-FILL)
	(gimp-hue-saturation base1-layer 0 0 -50 -50)
	(gimp-selection-grow img (* height 0.01))	; Faster?
	(plug-in-gauss-iir2 1 img base1-layer
	                    (+ 1 (* bluramount 0.05)) (+ 1 (* bluramount 0.05)))
	(gimp-selection-none img)
	(gimp-image-add-layer img base2-layer -1)
	(gimp-image-add-layer img base1-layer -1)
	;; Scale up/down to antialias
	(if (eqv? antialias TRUE)
	    (let ((layer-width  (car (gimp-drawable-width  base2-layer)))
		  (layer-height (car (gimp-drawable-height base2-layer))))
	      (gimp-layer-scale base2-layer (* 2 layer-width) (* 2 layer-height) 0)
	      (gimp-layer-scale base2-layer layer-width layer-height 0)))


	;; Lower highlight

    (round-select img shifty shifty width height ratio)
	(gimp-rect-select img shifty shifty width (* height 0.5) SUB 0 0)
	(gimp-selection-shrink img (* height 0.05))
	(remove-selection-corner img (* 0.4 height))
	(gimp-edit-fill highlight-lower BG-IMAGE-FILL)
	(gimp-selection-grow img (+ 1 (* bluramount 0.4)))	; Faster?
	(plug-in-gauss-iir2 1 img highlight-lower
	                    (+ 1 (* bluramount 0.4)) (+ 1 (* bluramount 0.4)))
	(set! highlight-lower-copy (car (gimp-layer-copy highlight-lower FALSE)))
	(gimp-selection-none img)
	(gimp-image-add-layer img highlight-lower-copy -1)
	(gimp-image-add-layer img highlight-lower      -1)


	;; Upper hightlight 2

    (round-select img shiftx shifty width height ratio)
	(gimp-rect-select img shiftx (+ shifty (* height 0.35)) width height SUB 0 0)
	(gimp-selection-shrink img (* height 0.02 whratio))
	(remove-selection-corner img (+ (* height 0.4)))
	(gimp-edit-fill highlight-upper2-layer BG-IMAGE-FILL)
	(gimp-selection-grow img (+ 1 (* bluramount 0.08)))	; Faster?
	(plug-in-gauss-iir2 1 img highlight-upper2-layer
	                    (+ 1 (* bluramount 0.08)) (+ 1 (* bluramount 0.08)))
	(gimp-selection-none img)
	(gimp-image-add-layer img highlight-upper2-layer -1)


	;; Upper hightlight 1

    (round-select img shiftx shifty width height ratio)
	(gimp-rect-select img shiftx (+ shifty (* height 0.28)) width height SUB 0 0)
	(gimp-selection-shrink img (* height 0.09))
	(remove-selection-corner img (* 0.15 height))
	(gimp-edit-fill highlight-upper1-layer BG-IMAGE-FILL)
	(gimp-selection-grow img (+ 1 (* bluramount 0.05)))	; Faster?
	(plug-in-gauss-iir2 1 img highlight-upper1-layer
	                    (+ 1 (* bluramount 0.05)) (+ 1 (* bluramount 0.05)))
	(gimp-selection-none img)
	(gimp-image-add-layer img highlight-upper1-layer -1)

	;; Glow layer

    (round-select img shiftx shifty width height ratio)
	(if (eqv? add-glow TRUE)
	  (let* ((glow-layer (car (gimp-layer-new img width-all height-all
	                                          RGBA_IMAGE "Glow Layer" 60 NORMAL-MODE))))
	    (gimp-image-add-layer img glow-layer -1)
	    (gimp-selection-grow img (* bluramount 0.2))
	    (gimp-selection-feather img (* bluramount 0.3))
	    (gimp-palette-set-background base-color)
	    (gimp-drawable-fill glow-layer TRANS-IMAGE-FILL)
	    (gimp-edit-fill glow-layer BG-IMAGE-FILL)
	    (gimp-levels glow-layer VALUE-LUT 0 255 1.0 127 255)))
	(gimp-selection-none img)

	; End

	(gimp-image-undo-enable img)
	(gimp-undo-push-group-start img)
	(if (eqv? flatten TRUE)
	    (begin
	      (gimp-palette-set-background bg-color)
	      (gimp-image-flatten img)))
	(gimp-undo-push-group-end img)
	(gimp-palette-set-foreground old-fg-color)
	(gimp-palette-set-background old-base-color)
	(list img (gimp-display-new img))		; Return
    ))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Round Aqua pill

(define (script-fu-aqua-pill-round baseradius
				   base-color
				   bg-color
				   ratio
				   blur
				   shadow
				   add-glow
				   antialias
				   flatten)
    (let* ((shadow-height (if (eqv? shadow TRUE) 1 0))
	   (height baseradius)
	   (radius (/ (* ratio height) 4))
	   (width (+ 1 (* ratio  height)))
	   (shiftx     (/ height 2))
	   (shifty     (/ height 2))
	   (height-all (+ height (* 2 shiftx) (* height shadow-height 0.5)))
	   (width-all  (+ width  (* 2 shifty)))
	   (img (car (gimp-image-new width-all height-all RGB))))
	(script-fu-aqua-pill-common img base-color bg-color width height 0 0
	                            ratio blur shadow add-glow antialias flatten)
	))

(script-fu-register "script-fu-aqua-pill-round"
		    _"<Toolbox>/Xtns/Script-Fu/Web Page Themes/Aqua Pill/Round..."
		    "Create a round aqua pill image"
		    "Iccii <iccii@hotmail.com>"
		    "Iccii"
		    "Sep 2001"
		    ""
		    SF-ADJUSTMENT _"Base Radius (pixel)"	'(100 2 500 1 1 0 1)
		    SF-COLOR      _"Base Color"		'(0 127 255)
		    SF-COLOR      _"Background Color"	'(255 255 255)
		    SF-ADJUSTMENT _"Round Ratio"	'(1 0.05 9 0.05 0.5 2 0)
		    SF-ADJUSTMENT _"Blur Amount"	'(1 0.05 5 0.05 0.5 2 0)
		    SF-TOGGLE     _"Drop Shadow"	TRUE
		    SF-TOGGLE     _"Add Glow (uncheck drap shadow)"		FALSE
		    SF-TOGGLE     _"Antialias"		TRUE
		    SF-TOGGLE     _"Flatten Image"	TRUE)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; Hrule aqua pill

(define (script-fu-aqua-pill-hrule width
				   height
				   base-color
				   bg-color
				   ratio
				   blur
				   shadow
				   add-glow
				   antialias
				   flatten)
    (let* ((shadow-height (if (eqv? shadow TRUE) 1 0))
	   (radius (/ (* ratio height) 4))
	   (shiftx     (/ height 2))
	   (shifty     (/ height 2))
	   (height-all (+ height (* 2 shiftx) (* height shadow-height 0.5)))
	   (width-all  (+ width  (* 2 shifty)))
	   (img (car (gimp-image-new width-all height-all RGB))))
      (if (< (* height ratio) width)
	  (script-fu-aqua-pill-common img base-color bg-color width height 0 0
	                              ratio blur shadow add-glow antialias flatten)
	  (gimp-message "Warning: Bar Length is too short to create your image!"))
    ))

(script-fu-register "script-fu-aqua-pill-hrule"
		    _"<Toolbox>/Xtns/Script-Fu/Web Page Themes/Aqua Pill/Hrule..."
		    "Create an Hrule with the aqua image"
		    "Iccii <iccii@hotmail.com>"
		    "Iccii"
		    "Sep 2001"
		    ""
		    SF-ADJUSTMENT _"Bar Length"		'(500 2 1500 1 1 0 1)
		    SF-ADJUSTMENT _"Bar Height"		'(25 2 500 1 1 0 1)
		    SF-COLOR      _"Base Color"		'(0 127 255)
		    SF-COLOR      _"Background Color"	'(255 255 255)
		    SF-ADJUSTMENT _"Round Ratio"	'(1 0.05 5 0.05 0.5 2 0)
		    SF-ADJUSTMENT _"Blur Amount"	'(1 0.05 5 0.05 0.5 2 0)
		    SF-TOGGLE     _"Drop Shadow"	TRUE
		    SF-TOGGLE     _"Add Glow (uncheck drap shadow)"		FALSE
		    SF-TOGGLE     _"Antialias"		TRUE
		    SF-TOGGLE     _"Flatten Image"	TRUE)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;; Aqua button with text

(define (script-fu-aqua-pill-button text
				    size
				    font
				    text-color
				    base-color
				    bg-color
				    xpadding
				    ypadding
				    ratio
				    blur
				    shadow
				    add-glow
				    antialias
				    flatten)
    (let* ((shadow-height (if (eqv? shadow TRUE) 1 0))
	   (old-fg-color (car (gimp-palette-get-foreground)))
	   (old-base-color (car (gimp-palette-get-background)))
	   (img (car (gimp-image-new 256 256 RGB)))
           (tmp (gimp-palette-set-foreground text-color))
	   (text-layer (car (gimp-text-fontname img -1 0 0 text 0 TRUE size PIXELS font)))
	   (text-width  (car (gimp-drawable-width  text-layer)))
	   (text-height (car (gimp-drawable-height text-layer)))
	   (radius (/ (* ratio text-height) 4))
	   (height (+ (* 2 ypadding) text-height))
	   (width  (+ (* 2 (+ radius xpadding)) text-width))
	   (shiftx     (/ height 2))
	   (shifty     (/ height 2))
	   (height-all (+ height (* 2 shiftx) (* height shadow-height 0.5)))
	   (width-all  (+ width  (* 2 shifty))))

	(gimp-palette-set-foreground text-color)
	(gimp-palette-set-background bg-color)
	(gimp-image-resize img width-all height-all
			   (+ shiftx xpadding radius)
			   (+ shifty ypadding))
	(gimp-layer-set-offsets text-layer
			        (+ shiftx xpadding radius)
			        (+ shifty ypadding))
	(gimp-layer-resize text-layer width-all height-all
			        (+ shiftx xpadding radius)
			        (+ shifty ypadding))
	(script-fu-aqua-pill-common img base-color bg-color width height xpadding ypadding
	                            ratio blur shadow add-glow antialias FALSE)

	;; Add text
	(gimp-undo-push-group-start img)
	(gimp-image-raise-layer-to-top img text-layer)
	(gimp-image-lower-layer img text-layer)
	(gimp-image-lower-layer img text-layer)
	(set! text-layer-copy   (car (gimp-layer-copy text-layer FALSE)))
	(set! text-layer-shadow (car (gimp-layer-copy text-layer FALSE)))
	(gimp-image-add-layer img text-layer-copy   3)
	(gimp-image-add-layer img text-layer-shadow 4)
	(gimp-layer-set-mode text-layer-shadow MULTIPLY-MODE)
	(gimp-channel-ops-offset text-layer-copy   0 1 (- (* xpadding 0.1)) (- (* ypadding 0.1)))
	(gimp-channel-ops-offset text-layer-shadow 0 1 (- (* xpadding 0.1)) (* height 0.1))
	(gimp-invert text-layer-copy)
	(gimp-selection-layer-alpha text-layer-shadow)
	(gimp-palette-set-foreground base-color)
	(gimp-edit-fill text-layer-shadow FG-IMAGE-FILL)
	(gimp-selection-grow img (+ 1 (* height blur 0.1)))	; Faster?
	(plug-in-gauss-iir2 1 img text-layer-shadow
	                    (+ 1 (* height blur 0.1)) (+ 1 (* height blur 0.1)))
	(gimp-selection-none img)
	(gimp-undo-push-group-end img)

	(gimp-undo-push-group-start img)
	(if (eqv? flatten TRUE)
	    (begin
	    (gimp-palette-set-background bg-color)
	    (gimp-layer-set-name (car (gimp-image-flatten img)) text)))

	;; Resize to button
	(gimp-image-resize img width height
			   (- 0 shiftx)
			   (- 0 shifty))
	(gimp-palette-set-foreground old-fg-color)
	(gimp-palette-set-background old-base-color)
	(gimp-undo-push-group-end img)
	(gimp-displays-flush)
	img))

(script-fu-register "script-fu-aqua-pill-button"
		    _"<Toolbox>/Xtns/Script-Fu/Web Page Themes/Aqua Pill/Button..."
		    "Create an aqua pill button with text"
		    "Iccii <iccii@hotmail.com>"
		    "Iccii"
		    "Sep 2001"
		    ""
		    SF-STRING     _"Text"		"Click Me!"
		    SF-ADJUSTMENT _"Font Size (pixels)"	'(50 2 500 1 1 0 1)

		    SF-FONT       _"Font"
	; Checking winsnap plug-in (Windows or not?)
(if (symbol-bound? 'extension-winsnap (the-environment))
	; For Windows user
				"-*-Times New Roman-bold-r-*-*-24-*-*-*-p-*-iso8859-1"
	; Default setting
				"-*-tekton-*-r-*-*-24-*-*-*-p-*-*-*"
)
		    SF-COLOR      _"Text Color"		'(0 0 0)
		    SF-COLOR      _"Base Color"		'(0 127 255)
		    SF-COLOR      _"Background Color"	'(255 255 255)
		    SF-ADJUSTMENT _"Padding X"		'(10 0 100 1 10 0 1)
		    SF-ADJUSTMENT _"Padding Y"		'(10 0 100 1 10 0 1)
		    SF-ADJUSTMENT _"Round Ratio"	'(1 0.05 5 0.05 0.5 2 0)
		    SF-ADJUSTMENT _"Blur Amount"	'(1 0.05 5 0.05 0.5 2 0)
		    SF-TOGGLE     _"Drop Shadow"	TRUE
		    SF-TOGGLE     _"Add Glow (uncheck drap shadow)"		FALSE
		    SF-TOGGLE     _"Antialias"		TRUE
		    SF-TOGGLE     _"Flatten Image"	TRUE)

