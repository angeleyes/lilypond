\version "1.7.3"

startCluster = #(make-span-event 'ClusterEvent START)
stopCluster = #(make-span-event 'ClusterEvent STOP)

groupOpen = #(make-span-event 'NoteGroupingEvent START)
groupClose = #(make-span-event 'NoteGroupingEvent STOP)


cr = #(make-span-event 'CrescendoEvent START)
rc = #(make-span-event 'CrescendoEvent STOP)
decr = #(make-span-event 'DecrescendoEvent START)
rced = #(make-span-event 'DecrescendoEvent STOP)

startTextSpan = #(make-span-event 'TextSpanEvent START)
stopTextSpan = #(make-span-event 'TextSpanEvent STOP)

cresc = \notes {
  #(ly:export (make-event-chord (list cr)))
  \property Voice.crescendoText = #'((font-shape . italic) "cresc.")
  \property Voice.crescendoSpanner = #'dashed-line
}

% ah, this is handy: maybe drop resetting of properties in
% dynamic-engraver ?
endcresc = \notes {
  #(ly:export (make-event-chord (list rc)))
  \property Voice.crescendoText \unset
  \property Voice.crescendoSpanner \unset
}

dim = \notes {
  #(ly:export (make-event-chord (list decr)))

  \property Voice.decrescendoText = #"dim."
  \property Voice.decrescendoSpanner = #'dashed-line
}

enddim = \notes {
  #(ly:export (make-event-chord (list rced)))
   \property Voice.decrescendoText \unset
 \property Voice.decrescendoSpanner \unset
}

%{

cresc = \spanevent \start "crescendo"
endcresc = \spanevent \stop "crescendo"

%}

% better name sustainstart/stop? 
sustainUp = #(make-span-event 'SustainEvent STOP)
sustainDown = #(make-span-event 'SustainEvent START)

unaCorda = #(make-span-event 'UnaCordaEvent START)
treCorde = #(make-span-event 'UnaCordaEvent STOP)

sostenutoDown = #(make-span-event 'SostenutoEvent START)
sostenutoUp = #(make-span-event 'SostenutoEvent STOP)

%crescpoco = \property Voice.crescendoText = "cresc. poco a poco"
%decresc = \property Voice.crescendoText = "decr."
%dim = \property Voice.crescendoText = "dim."
