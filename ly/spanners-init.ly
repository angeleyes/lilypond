\version "2.6.0"

startGroup = #(make-span-event 'NoteGroupingEvent START)
stopGroup = #(make-span-event 'NoteGroupingEvent STOP)


cr = #(make-span-event 'CrescendoEvent START)
decr = #(make-span-event 'DecrescendoEvent START)
enddecr = #(make-span-event 'DecrescendoEvent STOP)
endcr = #(make-span-event 'CrescendoEvent STOP) 


startTextSpan = #(make-span-event 'TextSpanEvent START)
stopTextSpan = #(make-span-event 'TextSpanEvent STOP)


startTrillSpan = #(make-span-event 'TrillSpanEvent START)
stopTrillSpan = #(make-span-event 'TrillSpanEvent STOP)


% STOP: junkme!
cresc =  {
  #(ly:export (make-event-chord (list cr)))
  \set crescendoText = \markup { \italic "cresc." }
  \set crescendoSpanner = #'dashed-line
}


dim =  {
  #(ly:export (make-event-chord (list decr)))

  \set decrescendoText = \markup { \italic "dim." }
  \set decrescendoSpanner = #'dashed-line
}

enddim =  {
  #(ly:export (make-event-chord (list enddecr)))
  \unset decrescendoText 
  \unset decrescendoSpanner 
}

% ah, this is handy: maybe drop resetting of properties in
% dynamic-engraver ?
endcresc =  {
  #(ly:export (make-event-chord (list endcr)))
  \unset crescendoText 
  \unset crescendoSpanner 
}

%%%%%%%%%%%%%%%%

setTextCresc = {
    \set crescendoText = \markup { \italic "cresc." }
    \set crescendoSpanner = #'dashed-line
}

setTextDecresc = {
    \set decrescendoText = \markup { \italic "decr." }
    \set decrescendoSpanner = #'dashed-line
}

setTextDim = {
    \set decrescendoText = \markup { \italic "dim." }
    \set decrescendoSpanner = #'dashed-line
}
setHairpinCresc = {
    \unset crescendoText 
    \unset crescendoSpanner 
}
setHairpinDecresc = {
    \unset decrescendoText 
    \unset decrescendoSpanner 
}
setHairpinDim = {
    \unset decrescendoText 
    \unset decrescendoSpanner 
}



% better name sustainstart/stop? 
sustainUp = #(make-span-event 'SustainEvent STOP)
sustainDown = #(make-span-event 'SustainEvent START)

unaCorda = #(make-span-event 'UnaCordaEvent START)
treCorde = #(make-span-event 'UnaCordaEvent STOP)

sostenutoDown = #(make-span-event 'SostenutoEvent START)
sostenutoUp = #(make-span-event 'SostenutoEvent STOP)

%crescpoco = \set crescendoText = "cresc. poco a poco"
%decresc = \set crescendoText = "decr."
%dim = \set crescendoText = "dim."


% for regression testing purposes.
assertBeamQuant =
#(def-music-function (parser location l r) (pair? pair?)
  (make-grob-property-override 'Beam 'position-callbacks
   (check-quant-callbacks l r)))
    
% for regression testing purposes.
assertBeamSlope =
#(def-music-function (parser location comp) (procedure?)
  (make-grob-property-override 'Beam 'position-callbacks
   (check-slope-callbacks comp)))


