%
% setup for Request->Element conversion. Guru-only
%

Staff =	\translator {
	\type "Staff_performer";
	\accepts Voice;
	\consists "Key_performer";
	\consists "Meter_performer";
}


Thread =\translator
{
	\type "Performer_group_performer";
	\consists "Note_performer";
}

Voice = \translator
{
	\type "Performer_group_performer";
	\accepts Thread;
}

Grandstaff = \translator
{
	\type "Performer_group_performer";
	\accepts Staff;
}

Lyric_voice = \translator {
	\type "Performer_group_performer";
	\consists "Lyric_performer";
}


Lyrics = \translator { 
	\type "Staff_performer";
	\accepts Lyric_voice;
	\consists "Meter_performer";
}

Score = \translator {
	\type "Score_performer";
	instrument = piano;
	\accepts Staff;
	\accepts Grandstaff;
	\accepts Lyrics; 
	\consists "Swallow_performer";
}

