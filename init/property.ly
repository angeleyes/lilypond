% property.ly
% list of properties that lily recognises
% and some shorthands (ugh)

%{

PROPERTIES

name			value	effect			shorthand

[Voice]
ydirection		-1	force stem down		\stemdown
ydirection		0  	stem direction free	\stemboth
ydirection		1	force stem up		\stemup

[Score?]
beamslopedamping	0	no damping		\beamslopeproportional	
beamslopedamping	1	damping1)		\beamslopedamped
beamslopedamping	100000	zero slope		\beamslopezero

[Score?]
beamquantisation	0	no quantisations	\beamposfree
beamquantisation	1	quantise pos and slope	\beamposnormal
beamquantisation	2	quantise avoide wedge2)	\beampostraditional


[Staff?]
instrument		ascii	midi instrument table lookup


1) after beam slope damping table suggested in [Wanske]
2) [Wanske] as well as [Ross] suggests that beams sloped upward must not 
   start sitting on a staffline, and beams sloped downward must not hang 
   from a staffline (similar for beam-ends).  This would create a wedge
   that is traditionally being avoided because it could easily be filled-up 
   with ink.
   However, avoiding these wedges restricts the freedom of beams quite a lot 
   while they don't seem to be a problem in modern printing.
   In no piece of sheetmusic engraved after 1953 (Baerenreiter) i've seen 
   these wedges being avoided.

%}

%hmm, (these) abbrevs suck, imo
% i guess they're meant as some form of doco
% that's what i use them for...
% should compile a list with recognised properties!
stemup = {
	\skip 1*0;
	% Stupid hack to make < { \stemup } > work
	\property Voice.ydirection = \up 
	}
stemboth= {
	\skip 1*0;
	\property Voice.ydirection = \center
}
stemdown = { 	
	\skip 1*0;
	\property Voice.ydirection = \down
}

% ugh, cluttering global namespace...
none=0
free=0
normal=1
traditional=2
infinity=10000

beamslopeproportional = {
	\property Score.beamslopedamping = \none
}

beamslopedamped = {
	\property Score.beamslopedamping = \normal
}

beamslopezero = {
	\property Score.beamslopedamping = \infinity
}

% this sucks, you'd want to pass an array, at least
% (or embedded code: you still can't dictate the slope / stemlength)
beamposfree = {
	\property Score.beamquantisation = \none
}

beamposnormal = {
	\property Score.beamquantisation = \normal
}

beampostraditional = {
	\property Score.beamquantisation = \traditional
}

