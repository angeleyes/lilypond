import inspect
import sys
import string
import re

from rational import Rational

class Output_stack_element:
	def __init__ (self):
		self.factor = Rational (1)
	def copy (self):
		o = Output_stack_element()
		o.factor = self.factor
		return o

class Output_printer:

	"""A class that takes care of formatting (eg.: indenting) a
	Music expression as a .ly file.
	
	"""
	## TODO: support for \relative.
	
	def __init__ (self):
		self._line = ''
		self._indent = 4
		self._nesting = 0
		self._file = sys.stdout
		self._line_len = 72
		self._output_state_stack = [Output_stack_element()]
		self._skipspace = False
		self._last_duration = None

	def set_file (self, file):
		self._file = file
		
	def dump_version (self):
		self.newline ()
		self.print_verbatim ('\\version "@TOPLEVEL_VERSION@"')
		self.newline ()
		
	def get_indent (self):
		return self._nesting * self._indent
	
	def override (self):
		last = self._output_state_stack[-1]
		self._output_state_stack.append (last.copy())
		
	def add_factor (self, factor):
		self.override()
		self._output_state_stack[-1].factor *=  factor

	def revert (self):
		del self._output_state_stack[-1]
		if not self._output_state_stack:
			raise 'empty'

	def duration_factor (self):
		return self._output_state_stack[-1].factor

	def print_verbatim (self, str):
		self._line += str

	def unformatted_output (self, str):
		self._nesting += str.count ('<') + str.count ('{')
		self._nesting -= str.count ('>') + str.count ('}')
		self.print_verbatim (str)
		
	def print_duration_string (self, str):
		if self._last_duration == str:
			return
		
		self.unformatted_output (str)
				     
	def add_word (self, str):
		if (len (str) + 1 + len (self._line) > self._line_len):
			self.newline()
			self._skipspace = True

		if not self._skipspace:
			self._line += ' '
		self.unformatted_output (str)
		self._skipspace = False
		
	def newline (self):
		self._file.write (self._line + '\n')
		self._line = ' ' * self._indent * self._nesting
		self._skipspace = True

	def skipspace (self):
		self._skipspace = True
		
	def __call__(self, arg):
		self.dump (arg)
	
	def dump (self, str):
		
		if self._skipspace:
			self._skipspace = False
			self.unformatted_output (str)
		else:
			words = string.split (str)
			for w in words:
				self.add_word (w)

class Duration:
	def __init__ (self):
		self.duration_log = 0
		self.dots = 0
		self.factor = Rational (1)
		
	def lisp_expression (self):
		return '(ly:make-duration %d %d %d %d)' % (self.duration_log,
							   self.dots,
							   self.factor.numerator (),
							   self.factor.denominator ())


	def ly_expression (self, factor = None):
		if not factor:
			factor = self.factor
			
		str = '%d%s' % (1 << self.duration_log, '.'*self.dots)

		if factor <> Rational (1,1):
			str += '*%d/%d' % (factor.numerator (), factor.denominator ())

		return str
	
	def print_ly (self, outputter):
		str = self.ly_expression (self.factor / outputter.duration_factor ())
		outputter.print_duration_string (str)
		
	def __repr__(self):
		return self.ly_expression()
		
	def copy (self):
		d = Duration ()
		d.dots = self.dots
		d.duration_log = self.duration_log
		d.factor = self.factor
		return d

	def get_length (self):
		dot_fact = Rational( (1 << (1 + self.dots))-1,
				     1 << self.dots)

		log = abs (self.duration_log)
		dur = 1 << log
		if self.duration_log < 0:
			base = Rational (dur)
		else:
			base = Rational (1, dur)

		return base * dot_fact * self.factor

	
class Pitch:
	def __init__ (self):
		self.alteration = 0
		self.step = 0
		self.octave = 0
		
	def __repr__(self):
		return self.ly_expression()

	def transposed (self, interval):
		c = self.copy ()
		c.alteration  += interval.alteration
		c.step += interval.step
		c.octave += interval.octave
		c.normalize ()
		
		target_st = self.semitones()  + interval.semitones()
		c.alteration += target_st - c.semitones()
		return c

	def normalize (c):
		while c.step < 0:
			c.step += 7
			c.octave -= 1
		c.octave += c.step / 7
		c.step = c.step  % 7

	
	def lisp_expression (self):
		return '(ly:make-pitch %d %d %d)' % (self.octave,
						     self.step,
						     self.alteration)

	def copy (self):
		p = Pitch ()
		p.alteration = self.alteration
		p.step = self.step
		p.octave = self.octave 
		return p

	def steps (self):
		return self.step + self.octave *7

	def semitones (self):
		return self.octave * 12 + [0,2,4,5,7,9,11][self.step] + self.alteration
	
	def ly_step_expression (self): 
		str = 'cdefgab'[self.step]
		if self.alteration > 0:
			str += 'is'* (self.alteration)
		elif self.alteration < 0:
			str += 'es'* (-self.alteration)

		return str.replace ('aes', 'as').replace ('ees', 'es')
	
	def ly_expression (self):
		str = self.ly_step_expression ()
		if self.octave >= 0:
			str += "'" * (self.octave + 1) 
		elif self.octave < -1:
			str += "," * (-self.octave - 1) 
			
		return str
	def print_ly (self, outputter):
		outputter (self.ly_expression())
	
class Music:
	def __init__ (self):
		self.parent = None
		self.start = Rational (0)
		self.comment = ''
		self.identifier = None
		
	def get_length(self):
		return Rational (0)
	
	def get_properties (self):
		return ''
	
	def has_children (self):
		return False
	
	def get_index (self):
		if self.parent:
			return self.parent.elements.index (self)
		else:
			return None
	def name (self):
		return self.__class__.__name__
	
	def lisp_expression (self):
		name = self.name()

		props = self.get_properties ()
#		props += 'start %f ' % self.start
		
		return "(make-music '%s %s)" % (name,  props)

	def set_start (self, start):
		self.start = start

	def find_first (self, predicate):
		if predicate (self):
			return self
		return None

	def print_comment (self, printer, text = None):
		if not text:
			text = self.comment

		if not text:
			return

			
		if text == '\n':
			printer.newline ()
			return
		lines = string.split (text, '\n')
		for l in lines:
			if l:
				printer.dump ('% ' + l)
			printer.newline ()
			

	def print_with_identifier (self, printer):
		if self.identifier: 
			printer ("\\%s" % self.identifier)
		else:
			self.print_ly (printer)

	def print_ly (self, printer):
		printer (self.ly_expression ())

class MusicWrapper (Music):
	def __init__ (self):
		Music.__init__(self)
		self.element = None
	def print_ly (self, func):
		self.element.print_ly (func)

class TimeScaledMusic (MusicWrapper):
	def print_ly (self, func):
		func ('\\times %d/%d ' %
		      (self.numerator, self.denominator))
		func.add_factor (Rational (self.numerator, self.denominator))
		MusicWrapper.print_ly (self, func)
		func.revert ()

class NestedMusic(Music):
	def __init__ (self):
		Music.__init__ (self)
		self.elements = []

	def append (self, what):
		if what:
			self.elements.append (what)
			
	def has_children (self):
		return self.elements

	def insert_around (self, succ, elt, dir):
		assert elt.parent == None
		assert succ == None or succ in self.elements

		
		idx = 0
		if succ:
			idx = self.elements.index (succ)
			if dir > 0:
				idx += 1
		else:
			if dir < 0:
				idx = 0
			elif dir > 0:
				idx = len (self.elements)

		self.elements.insert (idx, elt)
		elt.parent = self
		
	def get_properties (self):
		return ("'elements (list %s)"
			% string.join (map (lambda x: x.lisp_expression(),
					    self.elements)))

	def get_subset_properties (self, predicate):
		return ("'elements (list %s)"
			% string.join (map (lambda x: x.lisp_expression(),
					    filter ( predicate,  self.elements))))
	def get_neighbor (self, music, dir):
		assert music.parent == self
		idx = self.elements.index (music)
		idx += dir
		idx = min (idx, len (self.elements) -1)
		idx = max (idx, 0)

		return self.elements[idx]

	def delete_element (self, element):
		assert element in self.elements
		
		self.elements.remove (element)
		element.parent = None
		
	def set_start (self, start):
		self.start = start
		for e in self.elements:
			e.set_start (start)

	def find_first (self, predicate):
		r = Music.find_first (self, predicate)
		if r:
			return r
		
		for e in self.elements:
			r = e.find_first (predicate)
			if r:
				return r
		return None
		
class SequentialMusic (NestedMusic):
	def print_ly (self, printer):
		printer ('{')
		if self.comment:
			self.print_comment (printer)

		printer.newline()
		for e in self.elements:
			e.print_ly (printer)

		printer ('}')
		printer.newline()
			
	def lisp_sub_expression (self, pred):
		name = self.name()


		props = self.get_subset_properties (pred)
		
		return "(make-music '%s %s)" % (name,  props)
	
	def set_start (self, start):
		for e in self.elements:
			e.set_start (start)
			start += e.get_length()
			
class EventChord(NestedMusic):
	def get_length (self):
		l = Rational (0)
		for e in self.elements:
			l = max(l, e.get_length())
		return l
	
	def print_ly (self, printer):
		note_events = [e for e in self.elements if
			       isinstance (e, NoteEvent)]

		rest_events = [e for e in self.elements if
			       isinstance (e, RhythmicEvent)
			       and not isinstance (e, NoteEvent)]
		
		other_events = [e for e in self.elements if
				not isinstance (e, RhythmicEvent)]

		if rest_events:
			rest_events[0].print_ly (printer)
		elif len (note_events) == 1:
			note_events[0].print_ly (printer)
		elif note_events:
			pitches = [x.pitch.ly_expression () for x in note_events]
			printer ('<%s>' % string.join (pitches))
			note_events[0].duration.print_ly (printer)
		else:
			pass
		
		#	print  'huh', rest_events, note_events, other_events
 		for e in other_events:
			e.print_ly (printer)

		self.print_comment (printer)
			
class Event(Music):
	pass

class SpanEvent (Event):
	def __init__(self):
		Event.__init__ (self)
		self.span_direction = 0
	def get_properties(self):
		return "'span-direction  %d" % self.span_direction
	
class SlurEvent (SpanEvent):
	def ly_expression (self):
		return {-1: '(',
			0:'',
			1:')'}[self.span_direction]

class BeamEvent (SpanEvent):
	def ly_expression (self):
		return {-1: '[',
			0:'',
			1:']'}[self.span_direction]

class ArpeggioEvent(Event):
	def ly_expression (self):
		return ('\\arpeggio')


class TieEvent(Event):
	def ly_expression (self):
		return '~'

	
class RhythmicEvent(Event):
	def __init__ (self):
		Event.__init__ (self)
		self.duration = Duration()
		
	def get_length (self):
		return self.duration.get_length()
		
	def get_properties (self):
		return ("'duration %s"
			% self.duration.lisp_expression ())
	
class RestEvent (RhythmicEvent):
	def ly_expression (self):
		return 'r%s' % self.duration.ly_expression ()
	
	def print_ly (self, printer):
		printer('r')
		self.duration.print_ly (printer)

class SkipEvent (RhythmicEvent):
	def ly_expression (self):
		return 's%s' % self.duration.ly_expression () 

class NoteEvent(RhythmicEvent):
	def  __init__ (self):
		RhythmicEvent.__init__ (self)
		self.pitch = Pitch()
		self.cautionary = False
		self.forced_accidental = False
		
	def get_properties (self):
		return ("'pitch %s\n 'duration %s"
			% (self.pitch.lisp_expression (),
			   self.duration.lisp_expression ()))

	def pitch_mods (self):
		excl_question = ''
		if self.cautionary:
			excl_question += '?'
		if self.forced_accidental:
			excl_question += '!'

		return excl_question
	
	def ly_expression (self):
		return '%s%s%s' % (self.pitch.ly_expression (),
				   self.pitch_mods(),
				   self.duration.ly_expression ())

	def print_ly (self, printer):
		self.pitch.print_ly (printer)
		printer (self.pitch_mods ())  
		self.duration.print_ly (printer)

class KeySignatureChange (Music):
	def __init__ (self):
		Music.__init__ (self)
		self.scale = []
		self.tonic = Pitch()
		self.mode = 'major'
		
	def ly_expression (self):
		return '\\key %s \\%s' % (self.tonic.ly_step_expression (),
					  self.mode)
	
	def lisp_expression (self):
		pairs = ['(%d . %d)' % (i , self.scale[i]) for i in range (0,7)]
		scale_str = ("'(%s)" % string.join (pairs))

		return """ (make-music 'KeyChangeEvent
          'pitch-alist %s) """ % scale_str

class TimeSignatureChange (Music):
	def __init__ (self):
		Music.__init__ (self)
		self.fraction = (4,4)
	def ly_expression (self):
		return '\\time %d/%d ' % self.fraction
	
class ClefChange (Music):
	def __init__ (self):
		Music.__init__ (self)
		self.type = 'G'
		
	
	def ly_expression (self):
		return '\\clef "%s"' % self.type
	clef_dict = {
		"G": ("clefs.G", -2, -6),
		"C": ("clefs.C", 0, 0),
		"F": ("clefs.F", 2, 6),
		}
	
	def lisp_expression (self):
		(glyph, pos, c0) = self.clef_dict [self.type]
		clefsetting = """
		(make-music 'SequentialMusic
		'elements (list
      (context-spec-music
       (make-property-set 'clefGlyph "%s") 'Staff)
      (context-spec-music
       (make-property-set 'clefPosition %d) 'Staff)
      (context-spec-music
       (make-property-set 'middleCPosition %d) 'Staff)))
""" % (glyph, pos, c0)
		return clefsetting


def test_pitch ():
	bflat = Pitch()
	bflat.alteration = -1
	bflat.step =  6
	bflat.octave = -1
	fifth = Pitch()
	fifth.step = 4
	down = Pitch ()
	down.step = -4
	down.normalize ()
	
	
	print bflat.semitones()
	print bflat.transposed (fifth),  bflat.transposed (fifth).transposed (fifth)
	print bflat.transposed (fifth).transposed (fifth).transposed (fifth)

	print bflat.semitones(), 'down'
	print bflat.transposed (down)
	print bflat.transposed (down).transposed (down)
	print bflat.transposed (down).transposed (down).transposed (down)



def test_printer ():
	def make_note ():
		evc = EventChord()
		n = NoteEvent()
		evc.append (n)
		return n

	def make_tup ():
		m = SequentialMusic()
		m.append (make_note ())
		m.append (make_note ())
		m.append (make_note ())

		
		t = TimeScaledMusic ()
		t.numerator = 2
		t.denominator = 3
		t.element = m
		return t

	m = SequentialMusic ()
	m.append (make_tup ())
	m.append (make_tup ())
	m.append (make_tup ())
	
	printer = Output_printer()
	m.print_ly (printer)
	printer.newline ()
	
def test_expr ():
	m = SequentialMusic()
	l = 2  
	evc = EventChord()
	n = NoteEvent()
	n.duration.duration_log = l
	n.pitch.step = 1
	evc.insert_around (None, n, 0)
	m.insert_around (None, evc, 0)

	evc = EventChord()
	n = NoteEvent()
	n.duration.duration_log = l
	n.pitch.step = 3
	evc.insert_around (None, n, 0)
	m.insert_around (None, evc, 0)

 	evc = EventChord()
	n = NoteEvent()
	n.duration.duration_log = l
	n.pitch.step = 2 
	evc.insert_around (None, n, 0)
	m.insert_around (None, evc, 0)

 	evc = ClefChange()
	evc.type = 'treble'
	m.insert_around (None, evc, 0)

 	evc = EventChord()
	tonic = Pitch ()
	tonic.step = 2
	tonic.alteration = -2
	n = KeySignatureChange()
	n.tonic=tonic.copy()
	n.scale = [0, 0, -2, 0, 0,-2,-2]
	
	evc.insert_around (None, n, 0)
	m.insert_around (None, evc, 0)

	return m


if __name__ == '__main__':
	test_printer ()
	raise 'bla'
	test_pitch()
	
	expr = test_expr()
	expr.set_start (Rational (0))
	print expr.ly_expression()
	start = Rational (0,4)
	stop = Rational (4,2)
	def sub(x, start=start, stop=stop):
		ok = x.start >= start and x.start +x.get_length() <= stop
		return ok
	
	print expr.lisp_sub_expression(sub)

