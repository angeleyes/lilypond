#!@PYTHON@
# mup2ly.py -- mup input converter
# 
# source file of the GNU LilyPond music typesetter
#
# (c) 2001

'''
TODO:
   LOTS: we get all notes out now, rest after 1.4

   * lyrics (partly done)
   * bars
   * slurs,ties
   * staff settings
   * tuplets
   * grace
   * ornaments
   * midi settings
   * titling
   * chords entry mode
   * repeats, percent repeats
   
'''

import os
import fnmatch
import stat
import string
import re
import getopt
import sys
import __main__
import operator
import tempfile


# if set, LILYPONDPREFIX must take prevalence
# if datadir is not set, we're doing a build and LILYPONDPREFIX 
datadir = '@local_lilypond_datadir@'
if os.environ.has_key ('LILYPONDPREFIX') \
   or '@local_lilypond_datadir@' == '@' + 'local_lilypond_datadir' + '@':
	datadir = os.environ['LILYPONDPREFIX']
else:
	datadir = '@local_lilypond_datadir@'

sys.path.append (os.path.join (datadir, 'python'))
sys.path.append (os.path.join (datadir, 'python/out'))

program_name = sys.argv[0]
program_version = '@TOPLEVEL_VERSION@'
original_dir = os.getcwd ()
temp_dir = os.path.join (original_dir,  '%s.dir' % program_name)
errorport = sys.stderr
keep_temp_dir_p = 0
verbose_p = 0

localedir = '@localedir@'
try:
	import gettext
	gettext.bindtextdomain ('lilypond', localedir)
	gettext.textdomain ('lilypond')
	_ = gettext.gettext
except:
	def _ (s):
		return s


program_name = 'mup2ly'
help_summary = _ ("Convert mup to LilyPond source.")

option_definitions = [
	('', 'd', 'debug', _ ("debug")),
	('NAME[=EXP]', 'D', 'define', _ ("define macro NAME [optional expansion EXP]")),
	('', 'h', 'help', _ ("print this help")),
	('FILE', 'o', 'output', _ ("write output to FILE")),
	('', 'E', 'pre-process', _ ("only pre-process")),
	('', 'V', 'verbose', _ ("be verbose")),
	('', 'v', 'version', _ ("print version number")),
	('', 'w', 'warranty', _ ("show warranty and copyright")),
	]


################################################################
# lilylib.py -- options and stuff
# 
# source file of the GNU LilyPond music typesetter

try:
	import gettext
	gettext.bindtextdomain ('lilypond', localedir)
	gettext.textdomain ('lilypond')
	_ = gettext.gettext
except:
	def _ (s):
		return s

program_version = '@TOPLEVEL_VERSION@'
if program_version == '@' + 'TOPLEVEL_VERSION' + '@':
	program_version = '1.5.54'

def identify ():
	sys.stdout.write ('%s (GNU LilyPond) %s\n' % (program_name, program_version))

def warranty ():
	identify ()
	sys.stdout.write ('\n')
	sys.stdout.write (_ ('Copyright (c) %s by') % '2001--2005')
	sys.stdout.write ('\n')
	sys.stdout.write ('  Han-Wen Nienhuys')
	sys.stdout.write ('  Jan Nieuwenhuizen')
	sys.stdout.write ('\n\n')
	sys.stdout.write (_ ("Distributed under terms of the GNU General Public License."))
	sys.stdout.write (_ ("It comes with NO WARRANTY."))
	sys.stdout.write ('\n')

def progress (s):
	errorport.write (s + '\n')

def warning (s):
	progress (_ ("warning: ") + s)

def user_error (s, e=1):
	errorport.write (program_name + ":" + _ ("error: ") + s + '\n')
	sys.exit (e)
	
def error (s):
	'''Report the error S.  Exit by raising an exception. Please
	do not abuse by trying to catch this error. If you do not want
	a stack trace, write to the output directly.

	RETURN VALUE

	None
	
	'''
	
	progress (_ ("error: ") + s)
	raise _ ("Exiting ... ")

def getopt_args (opts):
	'''Construct arguments (LONG, SHORT) for getopt from  list of options.'''
	short = ''
	long = []
	for o in opts:
		if o[1]:
			short = short + o[1]
			if o[0]:
				short = short + ':'
		if o[2]:
			l = o[2]
			if o[0]:
				l = l + '='
			long.append (l)
	return (short, long)

def option_help_str (o):
	'''Transform one option description (4-tuple ) into neatly formatted string'''
	sh = '  '	
	if o[1]:
		sh = '-%s' % o[1]

	sep = '  '
	if o[1] and o[2]:
		sep = ', '
		
	long = ''
	if o[2]:
		long= '--%s' % o[2]

	arg = ''
	if o[0]:
		if o[2]:
			arg = '='
		arg = arg + o[0]
	return '  ' + sh + sep + long + arg


def options_help_str (opts):
	'''Convert a list of options into a neatly formatted string'''
	w = 0
	strs =[]
	helps = []

	for o in opts:
		s = option_help_str (o)
		strs.append ((s, o[3]))
		if len (s) > w:
			w = len (s)

	str = ''
	for s in strs:
		str = str + '%s%s%s\n' % (s[0], ' ' * (w - len(s[0])  + 3), s[1])
	return str

def help ():
	ls = [(_ ("Usage: %s [OPTIONS]... FILE") % program_name),
	      ('\n\n'),
	      (help_summary),
	      ('\n\n'),
	      (_ ("Options:")),
	      ('\n'),
	      (options_help_str (option_definitions)),
	      ('\n\n'),
	      (_ ("Report bugs to %s.") %
	      "http://post.gmane.org/post.php?group=gmane.comp.gnu.lilypond.bugs"),
	      ('\n')]
	map (sys.stdout.write, ls)
	
def setup_temp ():
	"""
	Create a temporary directory, and return its name.
	"""
	
	global temp_dir
	if not keep_temp_dir_p:
		temp_dir = tempfile.mktemp (program_name)
	try:
		os.mkdir (temp_dir, 0777)
	except OSError:
		pass

	return temp_dir


def system (cmd, ignore_error = 0, quiet =0):
	"""Run CMD. If IGNORE_ERROR is set, don't complain when CMD returns non zero.

	RETURN VALUE

	Exit status of CMD
	"""
	
	if verbose_p:
		progress (_ ("Invoking `%s\'") % cmd)

	st = os.system (cmd)
	if st:
		name = re.match ('[ \t]*([^ \t]*)', cmd).group (1)
		msg = name + ': ' + _ ("command exited with value %d") % st
		if ignore_error:
			if not quiet:
				warning (msg + ' ' + _ ("(ignored)") + ' ')
		else:
			error (msg)

	return st


def cleanup_temp ():
	if not keep_temp_dir_p:
		if verbose_p:
			progress (_ ("Cleaning %s...") % temp_dir)
		shutil.rmtree (temp_dir)


def strip_extension (f, ext):
	(p, e) = os.path.splitext (f)
	if e == ext:
		e = ''
	return p + e


def cp_to_dir (pattern, dir):
	"Copy files matching re PATTERN from cwd to DIR"
	# Duh.  Python style portable: cp *.EXT OUTDIR
	# system ('cp *.%s %s' % (ext, outdir), 1)
	files = filter (lambda x, p=pattern: re.match (p, x), os.listdir ('.'))
	map (lambda x, d=dir: shutil.copy2 (x, os.path.join (d, x)), files)


def mkdir_p (dir, mode=0777):
	if not os.path.isdir (dir):
		makedirs (dir, mode)


# if set, LILYPONDPREFIX must take prevalence
# if datadir is not set, we're doing a build and LILYPONDPREFIX 
datadir = '@local_lilypond_datadir@'

if os.environ.has_key ('LILYPONDPREFIX') :
	datadir = os.environ['LILYPONDPREFIX']
else:
	datadir = '@local_lilypond_datadir@'


while datadir[-1] == os.sep:
	datadir= datadir[:-1]

sys.path.insert (0, os.path.join (datadir, 'python'))

################################################################
# END Library


output = 0

#
# PMX cut and paste
#

def encodeint (i):
	return chr (i  + ord ('A'))

	
actab = {-2: 'eses', -1: 'es', 0 : '', 1: 'is', 2:'isis'}

def pitch_to_lily_string (tup):
	(o,n,a) = tup

        nm = chr((n + 2) % 7 + ord ('a'))
	nm = nm + actab[a]
	if o > 0:
		nm = nm + "'" * o
	elif o < 0:
		nm = nm + "," * -o
	return nm

def gcd (a,b):
	if b == 0:
		return a
	c = a
	while c: 
		c = a % b
		a = b
		b = c
	return a

def rat_simplify (r):
	(n,d) = r
	if d < 0:
		d = -d
		n = -n
	if n == 0:
		return (0,1)
	else:
		g = gcd (n, d)
		return (n/g, d/g)
	
def rat_multiply (a,b):
	(x,y) = a
	(p,q) = b

	return rat_simplify ((x*p, y*q))

def rat_divide (a,b):
	(p,q) = b
	return rat_multiply (a, (q,p))

tuplet_table = {
	2: 3,
	3: 2,
	5: 4
}


def rat_add (a,b):
	(x,y) = a
	(p,q) = b

	return rat_simplify ((x*q + p*y, y*q))

def rat_neg (a):
	(p,q) = a
	return (-p,q)


def rat_larger (a,b):
	return rat_subtract (a, b )[0] > 0

def rat_subtract (a,b ):
	return rat_add (a, rat_neg (b))

def rat_to_duration (frac):
	log = 1
	d = (1,1)
	while rat_larger (d, frac):
		d = rat_multiply (d, (1,2))
		log = log << 1

	frac = rat_subtract (frac, d)
	dots = 0
	if frac == rat_multiply (d, (1,2)):
		dots = 1
	elif frac == rat_multiply (d, (3,4)):
		dots = 2
	return (log, dots)	


class Barcheck :
	def __init__ (self):
		pass
	def dump (self):
		return '|\n'


class Meter :
	def __init__ (self,nums):
		self.nums = nums
	def dump (self):
		return ' %{ FIXME: meter change %} '
		
class Beam:
	def __init__ (self, ch):
		self.char = ch
	def dump (self):
		return self.char

class Slur:
	def __init__ (self,id):
		self.id = id
		self.start_chord = None
		self.end_chord = None
		
	def calculate (self):
		s =self.start_chord
		e= self.end_chord

		if e and s:
			s.note_suffix = s.note_suffix + '-('
			e.note_prefix = e.note_suffix + "-)"
		else:
			sys.stderr.write ("\nOrphaned slur")
			
class Voice:
	def __init__ (self, n):
                self.number = n
		self.entries = []
		self.chords = []
		self.staff = None
		self.current_slurs = []
		self.slurs = []
                
	def toggle_slur (self, id):
		
		for s in self.current_slurs:
			if s.id == id:
				self.current_slurs.remove (s)
				s.end_chord = self.chords[-1]
				return
		s = Slur (id)
		s.start_chord = self.chords[-1]
		self.current_slurs.append (s)
		self.slurs.append (s)
		
	def last_chord (self):
                if len (self.chords):
                        return self.chords[-1]
                else:
                        ch = Chord ()
                        ch.basic_duration = 4
                        return ch
                
	def add_chord (self, ch):
		self.chords.append (ch)
		self.entries.append (ch)
                
	def add_nonchord (self, nch):
		self.entries.append (nch)

	def idstring (self):
		return 'staff%svoice%s ' % (encodeint (self.staff.number) , encodeint(self.number))
        
	def dump (self):
		str = ''
                #if not self.entries:
                #        #return '\n'
                #        #ugh ugh
                #        return '\n%s = {}\n\n' % self.idstring ()
                ln = '  '
                one_two = ("One", "Two")
                if self.staff.voices [1 - self.number].entries:
                        ln = ln + '\\voice%s\n  ' % one_two[self.number]
		for e in self.entries:
			next = e.dump ()
			if next[-1] == '\n':
				str  = str + ln + next + ' '
				ln = '  '
				continue
			
			if len (ln) +len (next) > 72:
				str = str+ ln + '\n'
				ln = '  '
			ln = ln + next + ' '
			
			
		str = str  + ln
		id = self.idstring ()
			
		str = '''%s = \\context Voice = %s {
%s
}

'''% (id, id, str)
		return str
        
	def calculate_graces (self):
		lastgr = 0
		lastc = None
		for c in self.chords:
			if c.grace and  not lastgr:
				c.chord_prefix = c.chord_prefix + '\\grace { '
			elif not c.grace and lastgr:
				lastc.chord_suffix = lastc.chord_suffix + ' } '
			lastgr = c.grace
			lastc = c
                        
	def calculate (self):
		self.calculate_graces ()
		for s in self.slurs:
			s.calculate ()

class Clef:
	def __init__ (self, cl):
		self.type = cl
		
	def dump (self):
		return '\\clef %s' % self.type

key_sharps = ('c', 'g', 'd', 'a', 'e', 'b', 'fis')
key_flats = ('BUG', 'f', 'bes', 'es', 'as', 'des', 'ges')

class Key:
	def __init__ (self, sharps, flats):
		self.flats = flats
		self.sharps = sharps
		
	def dump (self):
		if self.sharps and self.flats:
			k = '\\keysignature %s ' % 'TODO'
		elif self.sharps:
			k = '\\key %s \major' % key_sharps[self.sharps]
		elif self.flats:
			k = '\\key %s \major' % key_flats[self.flats]
		return k

class Time:
	def __init__ (self, frac):
		self.frac = frac
		
	def dump (self):
		return '\\time %d/%d' % (self.frac[0], self.frac[1])
	

clef_table = {
	'b':'bass'  ,
	'r':'baritone',
	'n':'tenor',
	'a':'alto',
	'm':'mezzosoprano',
	's':'soprano',
	't':'treble',
	'f':'frenchviolin',
	}

class Staff:
	def __init__ (self, n):
                # ugh
		self.voices = (Voice (0), Voice (1))
                
		self.clef = None
		self.time = None
		self.key = None
		self.instrument = 0
		self.number = n
		
		i = 0
		for v in self.voices:
			v.staff = self
			v.number = i
			i = i+1
                        
	#def set_clef (self, letter):
	#	clstr = clef_table[letter]
	#	self.voices[0].add_nonchord (Clef (clstr))
		
	def calculate (self):
		for v in self.voices:
			v.calculate ()
                        
	def idstring (self):
		return 'staff%s' % encodeint (self.number)

	def dump (self):
		str = ''

		refs = ''
		for v in self.voices:
			if v.entries:
				# urg
				if v == self.voices[0]:
					if self.clef:
						refs = refs + self.clef.dump ()
					if self.time:
						refs = refs + self.time.dump ()
					if self.key:
						refs = refs + self.key.dump ()
					if refs:
						refs = '\n  ' + refs
				str = str + v.dump()
				refs = refs + '\n  \\' + v.idstring ()
		str = str + '''
%s = \context Staff = %s <<%s
>>

''' % (self.idstring (), self.idstring (), refs)
		return str

class Tuplet:
	def __init__ (self, number, base, dots):
		self.chords = []
		self.number = number
		self.replaces = tuplet_table[number]
		self.base = base
		self.dots = dots
		
		length = (1,base)
		if dots == 1:
			length = rat_multiply (length, (3,2))
		elif dots == 2:
			length = rat_multiply (length, (7,4))

		length = rat_multiply (length, (1,self.replaces))

		(nb,nd) =rat_to_duration (length)

		self.note_base = nb
		self.note_dots = nd

	def add_chord (self, ch):
		ch.dots = self.note_dots
		ch.basic_duration = self.note_base
		self.chords.append (ch)

		if len (self.chords) == 1:
			ch.chord_prefix = '\\times %d/%d { ' % (self.replaces, self.number)
		elif len (self.chords) == self.number:
			ch.chord_suffix = ' }' 
		
class Chord:
	def __init__ (self):
		self.pitches = []
		self.multimeasure = 0
		self.dots = 0
		self.basic_duration = 0
		self.scripts = []
		self.grace = 0
		self.chord_prefix = ''
		self.chord_suffix = ''
		self.note_prefix = ''
		self.note_suffix = ''

        # maybe use import copy?
	def copy (self):
		ch = Chord ()
		#for i in self.pitches:
		#	ch.pitches.append (i)
		ch.pitches = self.pitches[:]
		ch.multimeasure = self.multimeasure
		ch.dots = self.dots
		ch.basic_duration = self.basic_duration
		#for i in self.scripts:
		#	ch.scripts.append (i)
		ch.scripts = self.scripts[:]
		ch.grace = self.grace

		ch.chord_prefix = self.chord_prefix
		ch.chord_suffix = self.chord_suffix
		ch.note_prefix = self.note_prefix
		ch.note_suffix = self.note_suffix
		return ch

		
	def dump (self):
		str = ''

		sd = ''
		if self.basic_duration == 0.5:
			sd = '\\breve'
		else:
			sd = '%d' % self.basic_duration
		sd = sd + '.' * self.dots 
		for p in self.pitches:
			if str:
				str = str + ' ' 
			str = str + pitch_to_lily_string (p)


		str = self.note_prefix +str  + self.note_suffix
		
		if len (self.pitches) > 1:
			str = '<%s>' % str
		elif self.multimeasure:
			str = 'R'
		elif len (self.pitches) == 0:
			str = 'r'

		str = str + sd
		for s in self.scripts:
			str = str + '-' + s

		str = self.chord_prefix + str + self.chord_suffix
		
		return str
		
SPACE=' \t\n'
DIGITS ='0123456789'
basicdur_table = {
	9: 0.5,
	0: 0 ,
	2: 2 ,
	4: 4 ,
	8: 8 ,
	1: 16,
	3: 32,
	6: 64
	}


ornament_table = {
	't': '\\prall',
	'm': '\\mordent',
	'x': '"x"',
	'+': '+',
	'u': '"pizz"',
	'p': '|',
	'(': '"paren"',
	')': '"paren"',
	'g': '"segno"',
	'.': '.',
	'fd': '\\fermata',
	'f': '\\fermata',
	'_': '-',
	'T': '\\trill',
	'>': '>',
	'^': '^',
	}

# http://www.arkkra.com/doc/uguide/contexts.html

contexts = (
	'header', 
	'footer', 
	'header2', 
	'footer2', 
	'score', 
	'staff',
	'voice',
	'grids', 
	'music',
	)

class Parser:
	def __init__ (self, lines):
		self.parse_function = self.parse_context_music
		self.staffs = []
                self.current_voices = []
		self.forced_duration = None
		self.last_name = 0
		self.last_oct = 0		
		self.tuplets_expected = 0
		self.tuplets = []
		self.clef = None
		self.time = None
		self.key = None
		
		self.parse (lines)
		
        def parse_compound_location (self, line):
		colon = string.index (line, ':')
                s = line[:colon]
                debug (s)
                line = line[colon + 1:]
                debug (line)
                self.current_voices = []
                ##self.current_staffs = []
                map (self.parse_location, string.split (s, '&'))
                return line

        def parse_location (self, line):
		m = re.match ('^([-,0-9]+) *([-,0-9]*)', string.lstrip (line))
                
                def range_list_to_idxs (s):
                        
                        # duh
                        def flatten (l):
                                f = []
                                for i in l:
                                        for j in i:
                                                f.append (j)
                                return f
                                         
                        def range_to_list (s):
                                if string.find (s, '-') >= 0:
                                        debug ('s: ' + s)
                                        l = map (string.lstrip,
                                                 string.split (s, '-'))
                                        r = range (string.atoi (l[0]) - 1,
                                                   string.atoi (l[1]))
                                else:
                                        r = (string.atoi (s) - 1,)
                                return r
                        
                        ranges = string.split (s, ',')
                        l = flatten (map (range_to_list, ranges))
                        l.sort ()
                        return l
                
                staff_idxs = range_list_to_idxs (m.group (1))
                if m.group (2):
                        voice_idxs = range_list_to_idxs (m.group (2))
                else:
                        voice_idxs = [0]
                for s in staff_idxs:
                        while s > len (self.staffs) - 1:
                                self.staffs.append (Staff (s))
                        for v in voice_idxs:
                                self.current_voices.append (self.staffs[s].voices[v])
                        
	def parse_note (self, line):
                # FIXME: 1?
                oct = 1
                name = (ord (line[0]) - ord ('a') + 5) % 7
                # FIXME: does key play any role in this?
		alteration = 0
		debug ('NOTE: ' + `line`)
                line = string.lstrip (line[1:])
		while line:
                        if len (line) > 1 and line[:2] == '//':
                                line = 0
                                break
			elif line[0] == '#':
				alteration = alteration + 1
			elif line[0] == '&':
				alteration = alteration - 1
			elif line[0] == '+':
                                oct = oct + 1 
			elif line[0] == '-':
                                oct = oct - 1
                        else:
                                skipping (line[0])
			line = string.lstrip (line[1:])
		return (oct, name, alteration)
			
	def parse_chord (self, line):
		debug ('CHORD: ' + line)
		line = string.lstrip (line)
		ch = Chord ()
		if not line:
			#ch = self.current_voices[0].last_chord ()
			ch = self.last_chord.copy ()
		else:
			m = re.match ('^([0-9]+)([.]*)', line)
			if m:
				ch.basic_duration = string.atoi (m.group (1))
				line = line[len (m.group (1)):]
				if m.group (2):
					ch.dots = len (m.group (2))
					line = line[len (m.group (2)):]
                        else:
                                #ch.basic_duration = self.current_voices[0].last_chord ().basic_duration
                                ch.basic_duration = self.last_chord.basic_duration
                                
                        line = string.lstrip (line)
                        if len (line) > 1 and line[:2] == '//':
                                line = 0
                        #ugh
                        if not line:
				debug ('nline: ' + line)
                                #ch = self.current_voices[0].last_chord ()
				n = self.last_chord.copy ()
				n.basic_duration = ch.basic_duration
				n.dots = ch.dots
                                ch = n
				debug ('ch.pitsen:' + `ch.pitches`)
				debug ('ch.dur:' + `ch.basic_duration`)
			else:
				debug ('eline: ' + line)
				
			while line:
                                if len (line) > 1 and line[:2] == '//':
                                        line = 0
                                        break
                                elif line[:1] == 'mr':
					ch.multimeasure = 1
                                        line = line[2:]
                                elif line[:1] == 'ms':
					ch.multimeasure = 1
                                        line = line[2:]
				elif line[0] in 'rs':
                                        line = line[1:]
                                        pass
				elif line[0] in 'abcdefg':
					m = re.match ('([a-g][-#&+]*)', line)
					l = len (m.group (1))
					pitch = self.parse_note (line[:l])
                                        debug ('PITCH: ' + `pitch`)
					ch.pitches.append (pitch)
					line = line[l:]
                                        break
				else:
					skipping (line[0])
					line = line[1:]
                                line = string.lstrip (line)
		debug ('CUR-VOICES: ' + `self.current_voices`)
		map (lambda x, ch=ch: x.add_chord (ch), self.current_voices)
		self.last_chord = ch

	def parse_lyrics_location (self, line):
		line = line.lstrip (line)
		addition = 0
		m = re.match ('^(between[ \t]+)', line)
		if m:
			line = line[len (m.group (1)):]
			addition = 0.5
		else:
			m = re.match ('^(above [ \t]+)', line)
			if m:
				line = line[len (m.group (1)):]
				addition = -0.5
			else:
				addlyrics = 1
		
	def parse_voice (self, line):
		line = string.lstrip (line)
		# `;' is not a separator, chords end with ';'
		chords = string.split (line, ';')[:-1]
		# mup resets default duration and pitch each bar
		self.last_chord = Chord ()
		self.last_chord.basic_duration = 4
		map (self.parse_chord, chords)

	def init_context_header (self, line):
		self.parse_function = self.parse_context_header
					
	def parse_context_header (self, line):
		debug ('header: ' + line)
		skipping (line)
		
	def init_context_footer (self, line):
		self.parse_function = self.parse_context_footer

	def parse_context_footer (self, line):
		debug ('footer: ' + line)
		skipping (line)

	def init_context_header2 (self, line):
		self.parse_function = self.parse_context_header2

	def parse_context_header2 (self, line):
		debug ('header2: ' + line)
		skipping (line)

	def init_context_footer2 (self, line):
		self.parse_function = self.parse_context_footer2

	def parse_context_footer2 (self, line):
		debug ('footer2: ' + line)
		skipping (line)

	def init_context_score (self, line):
		self.parse_function = self.parse_context_score

	def parse_context_score (self, line):
		debug ('score: ' + line)
		line = string.lstrip (line)
		# ugh: these (and lots more) should also be parsed in
		# context staff.  we should have a class Staff_properties
		# and parse/set all those.
		m = re.match ('^(time[ \t]*=[ \t]*([0-9]+)[ \t]*/[ \t]*([0-9]+))', line)
		if m:
			line = line[len (m.group (1)):]
			self.time = Time ((string.atoi (m.group (2)),
					   string.atoi (m.group (3))))

		m = re.match ('^(key[ \t]*=[ \t]*([0-9]+)[ \t]*(#|@))', line)
		if m:
			line = line[len (m.group (1)):]
			n = string.atoi (m.group (2))
			if m.group (3) == '#':
				self.key = Key (n, 0)
			else:
				self.key = Key (0, n)
		skipping (line)

	def init_context_staff (self, line):
		self.parse_function = self.parse_context_staff

	def parse_context_staff (self, line):
		debug ('staff: ' + line)
		skipping (line)

	def init_context_voice (self, line):
		self.parse_function = self.parse_context_voice

	def parse_context_voice (self, line):
		debug ('voice: ' + line)
		skipping (line)

	def init_context_grids (self, line):
		self.parse_function = self.parse_context_grids

	def parse_context_grids (self, line):
		debug ('grids: ' + line)
		skipping (line)

	def init_context_music (self, line):
		self.parse_function = self.parse_context_music

	def parse_context_music (self, line):
		debug ('music: ' + line)
                line = string.lstrip (line)
                if line and line[0] in '0123456789':
                        line = self.parse_compound_location (line)
                        self.parse_voice (line)
		else:
			m = re.match ('^(TODOlyrics[ \t]+)', line)
			if m:
				line = line[len (m.group (1)):]
				self.parse_lyrics_location (line[7:])
				self.parse_lyrics (line)
			else:
				skipping (line)

	def parse (self, lines):
		# shortcut: set to official mup maximum (duh)
		# self.set_staffs (40)
		for line in lines:
                        debug ('LINE: ' + `line`)
			m = re.match ('^([a-z]+2?)', line)
			
			if m:
				word = m.group (1)
				if word in contexts:
					eval ('self.init_context_%s (line)' % word)
					continue
                                else:
                                        warning (_ ("no such context: %s") % word)
                                        skipping (line)
			else:
                                debug ('FUNC: ' + `self.parse_function`)
				self.parse_function (line)
				
		for c in self.staffs:
			# hmm
			if not c.clef and self.clef:
				c.clef = self.clef
			if not c.time and self.time:
				c.time = self.time
			if not c.key and self.key:
				c.key = self.key
			c.calculate ()

	def dump (self):
		str = ''

		refs = ''
		for s in self.staffs:
			str = str +  s.dump ()
			refs = refs + '\n    \\' + s.idstring ()

		str = str + '''

\score {
  <<%s
  >>
  \layout {}
  \midi {}
}
''' % refs 
		return str


class Pre_processor:
	def __init__ (self, raw_lines):
		self.lines = []
		self.active = [1]
		self.process_function = self.process_line
		self.macro_name = ''
		self.macro_body = ''
		self.process (raw_lines)

	def process_line (self, line):
		global macros
		m = re.match ('^([ \t]*([a-zA-Z]+))', line)
		s = line
		if m:
			word = m.group (2)
			debug ('MACRO?: ' + `word`)
			if word in pre_processor_commands:
				line = line[len (m.group (1)):]
				eval ('self.process_macro_%s (line)' % word)
				s = ''
			else:
				if macros.has_key (word):
					s = macros[word] + line[len (m.group (1)):]
		if not self.active [-1]:
			s = ''
		return s

	def process_macro_body (self, line):
		global macros
		# dig this: mup allows ifdefs inside macro bodies
		s = self.process_line (line)
		m = re.match ('(.*[^\\\\])(@)(.*)', s)
		if m:
			self.macro_body = self.macro_body + '\n' + m.group (1)
			macros[self.macro_name] = self.macro_body
			debug ('MACROS: ' + `macros`)
			# don't do nested multi-line defines
			self.process_function = self.process_line
			if m.group (3):
				s = m.group (3)
			else:
				s = ''
		else:
			self.macro_body = self.macro_body + '\n' + s
			s = ''
		return s

	# duh: mup is strictly line-based, except for `define',
	# which is `@' terminated and may span several lines
	def process_macro_define (self, line):
		global macros
		# don't define new macros in unactive areas
		if not self.active[-1]:
			return
		m = re.match ('^[ \t]*([a-zA-Z][a-zA-Z1-9_]*)(([^@]*)|(\\\\@))(@)?', line)
		n = m.group (1)
		if m.group (5):
			if m.group (2):
				e = m.group (2)
			else:
				e = ''
			macros[n] = e
			debug ('MACROS: ' + `macros`)
		else:
			# To support nested multi-line define's
			# process_function and macro_name, macro_body
			# should become lists (stacks)
			# The mup manual is undetermined on this
			# and I haven't seen examples doing it.
			#
			# don't do nested multi-line define's
			if m.group (2):
				self.macro_body = m.group (2)
			else:
				self.macro_body = ''
			self.macro_name = n
			self.process_function = self.process_macro_body
		
	def process_macro_ifdef (self, line):
		m = re.match ('^[ \t]*([a-zA-Z][a-zA-Z1-9_]*)', line)
		if m:
			
			active = self.active[-1] and macros.has_key (m.group (1))
			debug ('ACTIVE: %d' % active)
			self.active.append (active)

	def process_macro_ifndef (self, line):
		m = re.match ('^[ \t]*([a-zA-Z][a-zA-Z1-9_]*)', line)
		if m:
			active = self.active[-1] and not macros.has_key (m.group (1))
			self.active.append (active)

	def process_macro_else (self, line):
		debug ('ELSE')
		self.active[-1] = not self.active[-1]
		
	def process_macro_endif (self, line):
		self.active = self.active[:-1]
			
	def process (self, raw_lines):
		s = ''
		for line in raw_lines:
			ls = string.split (self.process_function (line), '\n')
			for i in ls:
				if i:
					s = s + string.rstrip (i)
					if s and s[-1] == '\\':
						s = string.rstrip (s[:-1])
					elif s:
						self.lines.append (s)
						s = ''


debug_p = 0
only_pre_process_p = 0
def debug (s):
        if debug_p:
                progress ('DEBUG: ' + s)

def skipping (s):
	if verbose_p or debug_p:
                progress ('SKIPPING: ' + s)

(sh, long) = getopt_args (__main__.option_definitions)
try:
	(options, files) = getopt.getopt (sys.argv[1:], sh, long)
except:
	help ()
	sys.exit (2)

macros = {}
pre_processor_commands = (
	'define',
	'else',
	'endif',
	'ifdef',
	'ifndef',
	)

for opt in options:
	o = opt[0]
	a = opt[1]
        if 0:
                pass
	elif o== '--debug' or o == '-d':
                debug_p = 1
	elif o== '--define' or o == '-D':
		if string.find (a, '=') >= 0:
			(n, e) = string.split (a, '=')
		else:
			n = a
			e = ''
		macros[n] = e
	elif o== '--pre-process' or o == '-E':
		only_pre_process_p = 1
	elif o== '--help' or o == '-h':
		help ()
		sys.exit (0)
	elif o== '--verbose' or o == '-V':
                verbose_p = 1
	elif o == '--version' or o == '-v':
		identify ()
		sys.exit (0)
	elif o == '--output' or o == '-o':
		output = a
	else:
		print o
		raise getopt.error

# writes to stdout for help2man
# don't call 
# identify ()
# sys.stdout.flush ()

# handy emacs testing
# if not files:
# 	files = ['template.mup']

if not files:
	files = ['-']
	
for f in files:
	h = None
	if f == '-':
		h = sys.stdin
	elif f and not os.path.isfile (f):
		f = strip_extension (f, '.mup') + '.mup'
		
	h = open (f)
	progress ( _("Processing `%s'..." % f))
	raw_lines = h.readlines ()
	p = Pre_processor (raw_lines)
	if only_pre_process_p:
		if not output:
			output = os.path.basename (re.sub ('(?i).mup$', '.mpp', f))
	else:
		e = Parser (p.lines)
		if not output:
			output = os.path.basename (re.sub ('(?i).mup$', '.ly', f))
		if output == f:
			output = os.path.basename (f + '.ly')
			
	if f == '-':
		output = '-'
		out_h = sys.stdout
	else:
		out_h = open (output, 'w')

	progress (_ ("Writing `%s'...") % output)

	tag = '%% Lily was here -- automatically converted by %s from %s' % ( program_name, f)
	if only_pre_process_p:
		# duh
		ly = string.join (p.lines, '\n')
	else:
		ly = tag + '\n\n' + e.dump ()

	out_h.write (ly)
	out_h.close ()
	if debug_p:
		print (ly)
	
