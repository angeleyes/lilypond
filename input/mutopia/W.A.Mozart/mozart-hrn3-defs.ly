% #(ly:set-point-and-click 'line-column)

longgrace = \property Voice.Stem \override #'stroke-style = #'()
endlonggrace = \property Voice.Stem \revert #'stroke-style
ritenuto = \markup { \italic  "rit." }

\version "1.7.18"
  
cresc = \notes {
    #(ly:export (make-event-chord (list (make-span-event 'CrescendoEvent START)))) 
    \property Voice.crescendoText = \markup { \italic \bold "cresc." }
    \property Voice.crescendoSpanner = #'dashed-line
}

%%
%% TODO: a better mechanism for tweaking Grace settings.
%%

startGraceMusic = \sequential { 
    \startGraceMusic 
    \property Voice.Beam \override #'space-function
       = #(lambda (beam mult) (* 0.8 0.8))
    \property Voice.Beam \override #'thickness = #(* 0.384 (/ 0.6 0.48))
}

stopGraceMusic= \sequential {
    \property Voice.Beam \revert #'thickness
    \property Voice.Beam \revert #'space-function
    \stopGraceMusic
}

\paper{
    \translator {
        \ScoreContext
        skipBars = ##t
        midiInstrument = #"french horn"
        %% try to mimic Breitkopf
        RehearsalMark \override #'padding = #1
        MultiMeasureRest \override #'padding = #0.5
        restNumberThreshold = #1

	RehearsalMark \override #'font-series = #'bold
	RehearsalMark \override #'font-relative-size = #3

        Beam \override #'thickness = #0.6
        Beam \override #'space-function = #(lambda (beam mult) 0.8)
        Slur \override #'beautiful = #0.3
    }
    \translator {
        \StaffContext
        minimumVerticalExtent = #'(-4.5 . 4.5)
    }
    % #(define fonts my-sheet)

    indent = 10. \mm
    linewidth = 189. \mm

}
%% new-chords-done %%
