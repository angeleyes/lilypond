\version "2.7.13"

%% Edit this file using a Unicode aware editor, such as GVIM, GEDIT, Emacs

%{

You may have to install additional fonts.

Red Hat Fedora

    taipeifonts fonts-xorg-truetype ttfonts-ja fonts-arabic \
	 ttfonts-zh_CN fonts-ja fonts-hebrew 

Debian GNU/Linux

   apt-get install emacs-intl-fonts xfonts-intl-.* \
	ttf-kochi-gothic ttf-kochi-mincho

%} 

\header {

  texidoc = "Various scripts may be used for texts (like titles and
lyrics) introduced by entering them in UTF-8 encoding, and using a
Pango based backend. Depending on the fonts installed, this fragment
will render Japanese, Hebrew, Cyrillic and Portuguese.

"
  
}

japanese = \lyricmode {  
  いろはにほへど ちりぬるを
  わがよたれぞ  つねならむ
  うゐのおくや  まけふこえて
  あさきゆめみじ ゑひもせず 
 }


bulgarian = \lyricmode {
  Жълтата дюля беше щастлива, че пухът, който цъфна, замръзна като гьон.
}

hebrew = \lyricmode { 
  זה כיף סתם לשמוע איך תנצח קרפד עץ טוב בגן.
}

portuguese = \lyricmode { 
  à vo cê uma can ção legal
}


\relative  { 
  c2 d e f g f e
}
\addlyrics { \hebrew }
\addlyrics { \bulgarian }
\addlyrics { \japanese }
\addlyrics { \portuguese }
