;;; batch-script aqua-pill-button for LilyPond's newweb
;;; uses slightly modified version of
;;; http://wingimp.hp.infoseek.co.jp/files/script/aqua-pill-button.scm
;;; http://www.webcon.net/~imorgan/aqua_howto.html

(define (lily-button text hover)
  (let* ((hover? (not (= 0 hover)))
	 (filename (string-append text (if hover? "-hover" "") ".png"))
	 (font "-adobe-helvetica-bold-r-normal-*-30-*-*-*-p-*-*-*")
	 (font-size 20)
	 (text-color (if hover? '(224 224 224) '(0 0 0)))
	 (base-color (if hover? '(64 192 255) '(224 224 224)))
	 (background-color '(255 255 255))
	 (padding-x 8)
	 (padding-y 6)
	 (ratio 1)
	 (blur 1)
	 (shadow 0)
	 (glow 0)
	 (anti-alias 1)
	 (flatten 0)
	 (image (script-fu-aqua-pill-button
		 text
		 font-size
		 font
		 text-color
		 base-color
		 background-color
		 padding-x
		 padding-y
		 ratio
		 blur
		 shadow
		 glow
		 anti-alias
		 flatten)))

    (gimp-image-merge-visible-layers image CLIP-TO-IMAGE)
    ;;(gimp-image-resize image width height (/ width 2) (/ height 2))
    (file-png-save 1 image (car (gimp-image-active-drawable image))
		   filename "" 0 9 0 0 0 0 0)
  (gimp-displays-flush)
  image))

(script-fu-register "lily-button"
                    "<Toolbox>/Xtns/Script-Fu/Web Page Themes/LilyPond/Button"
                    "GNU LilyPond Link"
                    "Janneke Nieuwenhuizen janneke@gnu.org"
                    "Janneke"
                    "2003"
                    ""
                    SF-VALUE "Text String" "\"about\""
                    SF-VALUE "Hover String" "0"
                   )

(define (lily-buttons buttons)
  (map (lambda (x) (lily-button x 0) (lily-button x 1)) buttons))

(script-fu-register "lily-buttons"
                    "<Toolbox>/Xtns/Script-Fu/Web Page Themes/LilyPond/Buttons"
                    "GNU LilyPond Link"
                    "Janneke Nieuwenhuizen janneke@gnu.org"
                    "Janneke"
                    "2003"
                    ""
                    SF-VALUE "Text String" "\"about\""
                   )

;; URG, why does -i not work?
;;gimp -s -b '(lily-button "about" 0)' '(gimp-quit 0)'
;;gimp -s -b '(lily-button "about" 1)' '(gimp-quit 0)'

;;gimp -c -s -b '(lily-buttons (list "about" "use" "download"))' '(gimp-quit 0)'
