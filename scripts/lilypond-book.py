#!@PYTHON@
# vim: set noexpandtab:
# TODO:
# * junk --outdir for --output 
# * Figure out clean set of options.
# * 
# * EndLilyPondOutput is def'd as vfil. Causes large white gaps.
# * texinfo: add support for @pagesize

# todo: dimension handling (all the x2y) is clumsy. (tca: Thats
#       because the values are taken directly from texinfo.tex,
#       geometry.sty and article.cls. Give me a hint, and I'll
#       fix it.)

# This is was the idea for handling of comments:
#	Multiline comments, @ignore .. @end ignore is scanned for
#	in read_doc_file, and the chunks are marked as 'ignore', so
#	lilypond-book will not touch them any more. The content of the
#	chunks are written to the output file. Also 'include' and 'input'
#	regex has to check if they are commented out.
#
#	Then it is scanned for 'lilypond', 'lilypond-file' and 'lilypond-block'.
#	These three regex's has to check if they are on a commented line,
#	% for latex, @c for texinfo.
#
#	Then lines that are commented out with % (latex) and @c (Texinfo)
#	are put into chunks marked 'ignore'. This cannot be done before
#	searching for the lilypond-blocks because % is also the comment character
#	for lilypond.
#
#	The the rest of the rexeces are searched for. They don't have to test
#	if they are on a commented out line.

import os
import stat
import string
import re
import getopt
import sys
import __main__
import operator


program_version = '@TOPLEVEL_VERSION@'
if program_version == '@' + 'TOPLEVEL_VERSION' + '@':
	program_version = '1.4pre'

#
# Try to cater for bad installations of LilyPond, that have
# broken TeX setup.  Just hope this doesn't hurt good TeX
# setups.  Maybe we should check if kpsewhich can find
# feta16.{afm,mf,tex,tfm}, and only set env upon failure.
#
datadir = '@datadir@'
environment = {
	'MFINPUTS' : datadir + '/mf:',
	'TEXINPUTS': datadir + '/tex:' + datadir + '/ps:.:',
	'TFMFONTS' : datadir + '/tfm:',
	'GS_FONTPATH' : datadir + '/afm:' + datadir + '/pfa',
	'GS_LIB' : datadir + '/ps',
}

def setup_environment ():
	for key in environment.keys ():
		val = environment[key]
		if os.environ.has_key (key):
			val = val + os.pathsep + os.environ[key]
		os.environ[key] = val



include_path = [os.getcwd()]


# g_ is for global (?)

g_here_dir = os.getcwd ()
g_dep_prefix = ''
g_outdir = ''
g_force_lilypond_fontsize = 0
g_read_lys = 0
g_do_pictures = 1
g_num_cols = 1
format = ''
g_run_lilypond = 1
no_match = 'a\ba'

default_music_fontsize = 16
default_text_fontsize = 12
paperguru = None

# this code is ugly. It should be cleaned
class LatexPaper:
	def __init__(self):
		self.m_paperdef =  {
			# the dimensions are from geometry.sty
			'a0paper': (mm2pt(841), mm2pt(1189)),
			'a1paper': (mm2pt(595), mm2pt(841)),
			'a2paper': (mm2pt(420), mm2pt(595)),
			'a3paper': (mm2pt(297), mm2pt(420)),
			'a4paper': (mm2pt(210), mm2pt(297)),
			'a5paper': (mm2pt(149), mm2pt(210)),
			'b0paper': (mm2pt(1000), mm2pt(1414)),
			'b1paper': (mm2pt(707), mm2pt(1000)),
			'b2paper': (mm2pt(500), mm2pt(707)),
			'b3paper': (mm2pt(353), mm2pt(500)),
			'b4paper': (mm2pt(250), mm2pt(353)),
			'b5paper': (mm2pt(176), mm2pt(250)),
			'letterpaper': (in2pt(8.5), in2pt(11)),
			'legalpaper': (in2pt(8.5), in2pt(14)),
			'executivepaper': (in2pt(7.25), in2pt(10.5))}
		self.m_use_geometry = None
		self.m_papersize = 'letterpaper'
		self.m_fontsize = 10
		self.m_num_cols = 1
		self.m_landscape = 0
		self.m_geo_landscape = 0
		self.m_geo_width = None
		self.m_geo_textwidth = None
		self.m_geo_lmargin = None
		self.m_geo_rmargin = None
		self.m_geo_includemp = None
		self.m_geo_marginparwidth = {10: 57, 11: 50, 12: 35}
		self.m_geo_marginparsep = {10: 11, 11: 10, 12: 10}
		self.m_geo_x_marginparwidth = None
		self.m_geo_x_marginparsep = None
		self.__body = None
	def set_geo_option(self, name, value):
		if name == 'body' or name == 'text':
			if type(value) == type(""):
				self.m_geo_textwidth =  value
			else:
				self.m_geo_textwidth =  value[0]
			self.__body = 1
		elif name == 'portrait':
			self.m_geo_landscape = 0
		elif name == 'reversemp' or name == 'reversemarginpar':
			if self.m_geo_includemp == None:
				self.m_geo_includemp = 1
		elif name == 'marginparwidth' or name == 'marginpar':
			self.m_geo_x_marginparwidth =  value
			self.m_geo_includemp = 1
		elif name == 'marginparsep':
			self.m_geo_x_marginparsep =  value
			self.m_geo_includemp = 1
		elif name == 'scale':
			if type(value) == type(""):
				self.m_geo_width = self.get_paperwidth() * float(value)
			else:
				self.m_geo_width = self.get_paperwidth() * float(value[0])
		elif name == 'hscale':
			self.m_geo_width = self.get_paperwidth() * float(value)
		elif name == 'left' or name == 'lmargin':
			self.m_geo_lmargin =  value
		elif name == 'right' or name == 'rmargin':
			self.m_geo_rmargin =  value
		elif name == 'hdivide' or name == 'divide':
			if value[0] not in ('*', ''):
				self.m_geo_lmargin =  value[0]
			if value[1] not in ('*', ''):
				self.m_geo_width =  value[1]
			if value[2] not in ('*', ''):
				self.m_geo_rmargin =  value[2]
		elif name == 'hmargin':
			if type(value) == type(""):
				self.m_geo_lmargin =  value
				self.m_geo_rmargin =  value
			else:
				self.m_geo_lmargin =  value[0]
				self.m_geo_rmargin =  value[1]
		elif name == 'margin':#ugh there is a bug about this option in
					# the geometry documentation
			if type(value) == type(""):
				self.m_geo_lmargin =  value
				self.m_geo_rmargin =  value
			else:
				self.m_geo_lmargin =  value[0]
				self.m_geo_rmargin =  value[0]
		elif name == 'total':
			if type(value) == type(""):
				self.m_geo_width =  value
			else:
				self.m_geo_width =  value[0]
		elif name == 'width' or name == 'totalwidth':
			self.m_geo_width =  value
		elif name == 'paper' or name == 'papername':
			self.m_papersize = value
		elif name[-5:] == 'paper':
			self.m_papersize = name
		else:
			pass 
			# what is _set_dimen ?? /MB
			#self._set_dimen('m_geo_'+name, value)
	def __setattr__(self, name, value):
		if type(value) == type("") and \
		   dimension_conversion_dict.has_key (value[-2:]):
			f = dimension_conversion_dict[value[-2:]]
			self.__dict__[name] = f(float(value[:-2]))
		else:
			self.__dict__[name] = value
			
	def __str__(self):
		s =  "LatexPaper:\n-----------"
		for v in self.__dict__.keys():
			if v[:2] == 'm_':
				s = s +  str (v) + ' ' + str (self.__dict__[v])
		s = s +  "-----------"
		return s
	
	def get_linewidth(self):
		w = self._calc_linewidth()
		if self.m_num_cols == 2:
			return (w - 10) / 2
		else:
			return w
	def get_paperwidth(self):
		#if self.m_use_geometry:
			return self.m_paperdef[self.m_papersize][self.m_landscape or self.m_geo_landscape]
		#return self.m_paperdef[self.m_papersize][self.m_landscape]
	
	def _calc_linewidth(self):
		# since geometry sometimes ignores 'includemp', this is
		# more complicated than it should be
		mp = 0
		if self.m_geo_includemp:
			if self.m_geo_x_marginparsep is not None:
				mp = mp + self.m_geo_x_marginparsep
			else:
				mp = mp + self.m_geo_marginparsep[self.m_fontsize]
			if self.m_geo_x_marginparwidth is not None:
				mp = mp + self.m_geo_x_marginparwidth
			else:
				mp = mp + self.m_geo_marginparwidth[self.m_fontsize]

		#ugh test if this is necessary				
		if self.__body:
			mp = 0

		if not self.m_use_geometry:
			return latex_linewidths[self.m_papersize][self.m_fontsize]
		else:
			geo_opts = (self.m_geo_lmargin == None,
				    self.m_geo_width == None,
				    self.m_geo_rmargin == None)

			if geo_opts == (1, 1, 1):
				if self.m_geo_textwidth:
					return self.m_geo_textwidth
				w = self.get_paperwidth() * 0.8
				return w - mp
			elif geo_opts == (0, 1, 1):
				 if self.m_geo_textwidth:
				 	return self.m_geo_textwidth
				 return self.f1(self.m_geo_lmargin, mp)
			elif geo_opts == (1, 1, 0):
				 if self.m_geo_textwidth:
				 	return self.m_geo_textwidth
				 return self.f1(self.m_geo_rmargin, mp)
			elif geo_opts \
					in ((0, 0, 1), (1, 0, 0), (1, 0, 1)):
				if self.m_geo_textwidth:
					return self.m_geo_textwidth
				return self.m_geo_width - mp
			elif geo_opts in ((0, 1, 0), (0, 0, 0)):
				w = self.get_paperwidth() \
				  - self.m_geo_lmargin - self.m_geo_rmargin - mp
				if w < 0:
					w = 0
				return w
			raise "Never do this!"
	def f1(self, m, mp):
		tmp = self.get_paperwidth() - m * 2 - mp
		if tmp < 0:
			tmp = 0
		return tmp
	def f2(self):
		tmp = self.get_paperwidth() - self.m_geo_lmargin \
			- self.m_geo_rmargin
		if tmp < 0:
			return 0
		return tmp

class TexiPaper:
	def __init__(self):
		self.m_papersize = 'letterpaper'
		self.m_fontsize = 12
	def get_linewidth(self):
		return texi_linewidths[self.m_papersize][self.m_fontsize]

def mm2pt(x):
	return x * 2.8452756
def in2pt(x):
	return x * 72.26999
def em2pt(x, fontsize = 10):
	return {10: 10.00002, 11: 10.8448, 12: 11.74988}[fontsize] * x
def ex2pt(x, fontsize = 10):
	return {10: 4.30554, 11: 4.7146, 12: 5.16667}[fontsize] * x

def pt2pt(x):
	return x

dimension_conversion_dict ={
	'mm': mm2pt,
	'in': in2pt,
	'em': em2pt,
	'ex': ex2pt,
	'pt': pt2pt
	}

	
# latex linewidths:
# indices are no. of columns, papersize,  fontsize
# Why can't this be calculated?
latex_linewidths = {
 	'a4paper':{10: 345, 11: 360, 12: 390},
	'a4paper-landscape': {10: 598, 11: 596, 12:592},
	'a5paper':{10: 276, 11: 276, 12: 276},
	'b5paper':{10: 345, 11: 356, 12: 356},
	'letterpaper':{10: 345, 11: 360, 12: 390},
	'letterpaper-landscape':{10: 598, 11: 596, 12:596},
	'legalpaper': {10: 345, 11: 360, 12: 390},
	'executivepaper':{10: 345, 11: 360, 12: 379}}

texi_linewidths = {
	'afourpaper': {12: mm2pt(160)},
	'afourwide': {12: in2pt(6.5)},
	'afourlatex': {12: mm2pt(150)},
	'smallbook': {12: in2pt(5)},
	'letterpaper': {12: in2pt(6)}}

option_definitions = [
  ('EXT', 'f', 'format', 'set format.  EXT is one of texi and latex.'),
  ('DIM',  '', 'default-music-fontsize', 'default fontsize for music.  DIM is assumed to be in points'),
  ('DIM',  '', 'default-lilypond-fontsize', 'deprecated, use --default-music-fontsize'),
  ('DIM', '', 'force-music-fontsize', 'force fontsize for all inline lilypond. DIM is assumed be to in points'),
  ('DIM', '', 'force-lilypond-fontsize', 'deprecated, use --force-music-fontsize'),
  ('DIR', 'I', 'include', 'include path'),
  ('', 'M', 'dependencies', 'write dependencies'),
  ('PREF', '',  'dep-prefix', 'prepend PREF before each -M dependency'),
  ('', 'n', 'no-lily', 'don\'t run lilypond'),
  ('', '', 'no-pictures', "don\'t generate pictures"),
  ('', '', 'read-lys', "don't write ly files."),
  ('FILE', 'o', 'outname', 'filename main output file'),
  ('FILE', '', 'outdir', "where to place generated files"),
  ('', 'v', 'version', 'print version information' ),
  ('', 'h', 'help', 'print help'),
  ]

# format specific strings, ie. regex-es for input, and % strings for output
output_dict= {
	'latex': {
		'output-lilypond-fragment' : r"""\begin[eps,singleline,%s]{lilypond}
  \context Staff <
    \context Voice{
      %s
    }
  >
\end{lilypond}""",
		'output-filename' : r'''

\verb+%s+:''',
		'output-lilypond': r"""\begin[%s]{lilypond}
%s
\end{lilypond}""",
		'output-verbatim': "\\begin{verbatim}%s\\end{verbatim}",
		'output-default-post': "\\def\postLilypondExample{}\n",
		'output-default-pre': "\\def\preLilypondExample{}\n",
		'usepackage-graphics': '\\usepackage{graphics}\n',
		'output-eps': '\\noindent\\parbox{\\lilypondepswidth{%(fn)s.eps}}{\includegraphics{%(fn)s.eps}}',
		'output-tex': '\\preLilypondExample \\input %(fn)s.tex \\postLilypondExample\n',
		'pagebreak': r'\pagebreak',
		},
	'texi' : {'output-lilypond': """@lilypond[%s]
%s
@end lilypond 
""",
		'output-filename' : r'''

@file{%s}:''',	  
		  'output-lilypond-fragment': """@lilypond[%s]
\context Staff\context Voice{ %s }
@end lilypond """,
		  'pagebreak': None,
		  'output-verbatim': r"""@example
%s
@end example
""",

# do some tweaking: @ is needed in some ps stuff.
# override EndLilyPondOutput, since @tex is done
# in a sandbox, you can't do \input lilyponddefs at the
# top of the document.

# should also support fragment in
		  
		  'output-all': r"""
@tex
\catcode`\@=12
\input lilyponddefs
\def\EndLilyPondOutput{}
\input %(fn)s.tex
\catcode`\@=0
@end tex
@html
<p>
<a href="%(fn)s.png">
<img border=0 src="%(fn)s.png" alt="[picture of music]">
</a>
@end html
""",
		}
	}

def output_verbatim (body):
	if __main__.format == 'texi':
		body = re.sub ('([@{}])', '@\\1', body)
	return get_output ('output-verbatim') % body


re_dict = {
	'latex': {'input': r'(?m)^[^%\n]*?(?P<match>\\mbinput{?([^}\t \n}]*))',
		  'include': r'(?m)^[^%\n]*?(?P<match>\\mbinclude{(?P<filename>[^}]+)})',
		  'option-sep' : ', *',
		  'header': r"\\documentclass\s*(\[.*?\])?",
		  'geometry': r"^(?m)[^%\n]*?\\usepackage\s*(\[(?P<options>.*)\])?\s*{geometry}",
		  'preamble-end': r'(?P<code>\\begin{document})',
		  'verbatim': r"(?s)(?P<code>\\begin{verbatim}.*?\\end{verbatim})",
		  'verb': r"(?P<code>\\verb(?P<del>.).*?(?P=del))",
		  'lilypond-file': r'(?m)^[^%\n]*?(?P<match>\\lilypondfile(\[(?P<options>.*?)\])?\{(?P<filename>.+)})',
		  'lilypond' : r'(?m)^[^%\n]*?(?P<match>\\lilypond(\[(?P<options>.*?)\])?{(?P<code>.*?)})',
		  'lilypond-block': r"(?sm)^[^%\n]*?(?P<match>\\begin(\[(?P<options>.*?)\])?{lilypond}(?P<code>.*?)\\end{lilypond})",
		  'def-post-re': r"\\def\\postLilypondExample",
		  'def-pre-re': r"\\def\\preLilypondExample",		  
		  'usepackage-graphics': r"\usepackage{graphics}",
		  'intertext': r',?\s*intertext=\".*?\"',
		  'multiline-comment': no_match,
		  'singleline-comment': r"(?m)^.*?(?P<match>(?P<code>^%.*$\n+))",
		  'numcols': r"(?P<code>\\(?P<num>one|two)column)",
		  },


	# why do we have distinction between @mbinclude and @include? 
	'texi': {
		 'include':  '(?m)^[^%\n]*?(?P<match>@mbinclude[ \n\t]+(?P<filename>[^\t \n]*))',
		 'input': no_match,
		 'header': no_match,
		 'preamble-end': no_match,
		 'landscape': no_match,
		 'verbatim': r"""(?s)(?P<code>@example\s.*?@end example\s)""",
		 'verb': r"""(?P<code>@code{.*?})""",
		 'lilypond-file': '(?m)^(?!@c)(?P<match>@lilypondfile(\[(?P<options>.*?)\])?{(?P<filename>[^}]+)})',
		 'lilypond' : '(?m)^(?!@c)(?P<match>@lilypond(\[(?P<options>.*?)\])?{(?P<code>.*?)})',
		 'lilypond-block': r"""(?m)^(?!@c)(?P<match>(?s)(?P<match>@lilypond(\[(?P<options>.*?)\])?\s(?P<code>.*?)@end lilypond\s))""",
		  'option-sep' : ', *',
		  'intertext': r',?\s*intertext=\".*?\"',
		  'multiline-comment': r"(?sm)^\s*(?!@c\s+)(?P<code>@ignore\s.*?@end ignore)\s",
		  'singleline-comment': r"(?m)^.*?(?P<match>(?P<code>@c.*$\n+))",
		  'numcols': no_match,
		 }
	}


for r in re_dict.keys ():
	olddict = re_dict[r]
	newdict = {}
	for k in olddict.keys ():
		newdict[k] = re.compile (olddict[k])
	re_dict[r] = newdict

	
def uniq (list):
	list.sort ()
	s = list
	list = []
	for x in s:
		if x not in list:
			list.append (x)
	return list
		

def get_output (name):
	return  output_dict[format][name]

def get_re (name):
	return  re_dict[format][name]

def bounding_box_dimensions(fname):
	if g_outdir:
		fname = os.path.join(g_outdir, fname)
	try:
		fd = open(fname)
	except IOError:
		error ("Error opening `%s'" % fname)
	str = fd.read ()
	s = re.search('%%BoundingBox: ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)', str)
	if s:
		return (int(s.group(3))-int(s.group(1)), 
			int(s.group(4))-int(s.group(2)))
	else:
		return (0,0)

def error (str):
	sys.stderr.write (str + "\n  Exiting ... \n\n")
	raise 'Exiting.'


def compose_full_body (body, opts):
	"""Construct the lilypond code to send to Lilypond.
	Add stuff to BODY using OPTS as options."""
	music_size = default_music_fontsize
	latex_size = default_text_fontsize
	for o in opts:
		if g_force_lilypond_fontsize:
			music_size = g_force_lilypond_fontsize
		else:
			m = re.match ('([0-9]+)pt', o)
			if m:
				music_size = string.atoi(m.group (1))

		m = re.match ('latexfontsize=([0-9]+)pt', o)
		if m:
			latex_size = string.atoi (m.group (1))

	if re.search ('\\\\score', body):
		is_fragment = 0
	else:
		is_fragment = 1
	if 'fragment' in opts:
		is_fragment = 1
	if 'nofragment' in opts:
		is_fragment = 0

	if is_fragment and not 'multiline' in opts:
		opts.append('singleline')
	if 'singleline' in opts:
		l = -1.0;
	else:
		l = __main__.paperguru.get_linewidth()

	for o in opts:
		m= re.search ('relative(.*)', o)
		v = 0
		if m:
			try:
				v = string.atoi (m.group (1))
			except ValueError:
				pass

			v = v + 1
			pitch = 'c'
			if v < 0:
				pitch = pitch + '\,' * v
			elif v > 0:
				pitch = pitch + '\'' * v

			body = '\\relative %s { %s }' %(pitch, body)
	
	if is_fragment:
		body = r"""\score { 
 \notes { %s }
  \paper { }  
}""" % body

	opts = uniq (opts)
	optstring = string.join (opts, ' ')
	optstring = re.sub ('\n', ' ', optstring)
	body = r"""
%% Generated automatically by: lilypond-book.py
%% options are %s  
\include "paper%d.ly"
\paper  { linewidth = %f \pt } 
""" % (optstring, music_size, l) + body

	# ughUGH not original options
	return body

def parse_options_string(s):
	d = {}
	r1 = re.compile("((\w+)={(.*?)})((,\s*)|$)")
	r2 = re.compile("((\w+)=(.*?))((,\s*)|$)")
	r3 = re.compile("(\w+?)((,\s*)|$)")
	while s:
		m = r1.match(s)
		if m:
			s = s[m.end():]
			d[m.group(2)] = re.split(",\s*", m.group(3))
			continue
		m = r2.match(s)
		if m:
			s = s[m.end():]
			d[m.group(2)] = m.group(3)
			continue
		m = r3.match(s)
		if m:
			s = s[m.end():]
			d[m.group(1)] = 1
			continue
		
		error ("format of option string invalid (was `%')" % s)
	return d

def scan_latex_preamble(chunks):
	# first we want to scan the \documentclass line
	# it should be the first non-comment line
	idx = 0
	while 1:
		if chunks[idx][0] == 'ignore':
			idx = idx + 1
			continue
		m = get_re ('header').match(chunks[idx][1])
		if m.group (1):
			options = re.split (',[\n \t]*', m.group(1)[1:-1])
		else:
			options = []
		for o in options:
			if o == 'landscape':
				paperguru.m_landscape = 1
			m = re.match("(.*?)paper", o)
			if m:
				paperguru.m_papersize = m.group()
			else:
				m = re.match("(\d\d)pt", o)
				if m:
					paperguru.m_fontsize = int(m.group(1))
			
		break
	while chunks[idx][0] != 'preamble-end':
		if chunks[idx] == 'ignore':
			idx = idx + 1
			continue
		m = get_re ('geometry').search(chunks[idx][1])
		if m:
			paperguru.m_use_geometry = 1
			o = parse_options_string(m.group('options'))
			for k in o.keys():
				paperguru.set_geo_option(k, o[k])
		idx = idx + 1

def scan_texi_preamble (chunks):
	# this is not bulletproof..., it checks the first 10 chunks
	for c in chunks[:10]: 
		if c[0] == 'input':
			for s in ('afourpaper', 'afourwide', 'letterpaper',
				  'afourlatex', 'smallbook'):
				if string.find(c[1], "@%s" % s) != -1:
					paperguru.m_papersize = s

def scan_preamble (chunks):
	if __main__.format == 'texi':
		scan_texi_preamble(chunks)
	else:
		assert __main__.format == 'latex'
		scan_latex_preamble(chunks)
		

def completize_preamble (chunks):
	if __main__.format == 'texi':
		return chunks
	pre_b = post_b = graphics_b = None
	for chunk in chunks:
		if chunk[0] == 'preamble-end':
			break
		if chunk[0] == 'input':
			m = get_re('def-pre-re').search(chunk[1])
			if m:
				pre_b = 1
		if chunk[0] == 'input':
			m = get_re('def-post-re').search(chunk[1])
			if m:
				post_b = 1
		if chunk[0] == 'input':
			m = get_re('usepackage-graphics').search(chunk[1])
			if m:
				graphics_b = 1
	x = 0
	while chunks[x][0] != 'preamble-end':
		x = x + 1
	if not pre_b:
		chunks.insert(x, ('input', get_output ('output-default-pre')))
	if not post_b:
		chunks.insert(x, ('input', get_output ('output-default-post')))
	if not graphics_b:
		chunks.insert(x, ('input', get_output ('usepackage-graphics')))
	return chunks


read_files = []
def find_file (name):
	"""
	Search the include path for NAME. If found, return the (CONTENTS, PATH) of the file.
	"""
	
	f = None
	nm = ''
	for a in include_path:
		try:
			nm = os.path.join (a, name)
			f = open (nm)
			__main__.read_files.append (nm)
			break
		except IOError:
			pass
	if f:
		sys.stderr.write ("Reading `%s'\n" % nm)
		return (f.read (), nm)
	else:
		error ("File not found `%s'\n" % name)
		return ('', '')

def do_ignore(match_object):
	return [('ignore', match_object.group('code'))]
def do_preamble_end(match_object):
	return [('preamble-end', match_object.group('code'))]

def make_verbatim(match_object):
	return [('verbatim', match_object.group('code'))]

def make_verb(match_object):
	return [('verb', match_object.group('code'))]

def do_include_file(m):
	"m: MatchObject"
	return [('input', get_output ('pagebreak'))] \
	     + read_doc_file(m.group('filename')) \
	     + [('input', get_output ('pagebreak'))] 

def do_input_file(m):
	return read_doc_file(m.group('filename'))

def make_lilypond(m):
	if m.group('options'):
		options = m.group('options')
	else:
		options = ''
	return [('input', get_output('output-lilypond-fragment') % 
			(options, m.group('code')))]

def make_lilypond_file(m):
	"""

	Find @lilypondfile{bla.ly} occurences and substitute bla.ly
	into a @lilypond .. @end lilypond block.
	
	"""
	
	if m.group('options'):
		options = m.group('options')
	else:
		options = ''
	(content, nm) = find_file(m.group('filename'))
	options = "filename=%s," % nm + options

	return [('input', get_output('output-lilypond') %
			(options, content))]

def make_lilypond_block(m):
	if m.group('options'):
		options = get_re('option-sep').split (m.group('options'))
	else:
	    options = []
	options = filter(lambda s: s != '', options)
	return [('lilypond', m.group('code'), options)]

def do_columns(m):
	if __main__.format != 'latex':
		return []
	if m.group('num') == 'one':
		return [('numcols', m.group('code'), 1)]
	if m.group('num') == 'two':
		return [('numcols', m.group('code'), 2)]
	
def chop_chunks(chunks, re_name, func, use_match=0):
    newchunks = []
    for c in chunks:
        if c[0] == 'input':
            str = c[1]
            while str:
                m = get_re (re_name).search (str)
                if m == None:
                    newchunks.append (('input', str))
                    str = ''
                else:
		    if use_match:
                        newchunks.append (('input', str[:m.start ('match')]))
		    else:
                        newchunks.append (('input', str[:m.start (0)]))
                    #newchunks.extend(func(m))
		    # python 1.5 compatible:
		    newchunks = newchunks + func(m)
                    str = str [m.end(0):]
        else:
            newchunks.append(c)
    return newchunks

def determine_format (str):
	if __main__.format == '':
		
		latex =  re.search ('\\\\document', str[:200])
		texinfo =  re.search ('@node|@setfilename', str[:200])

		f = ''
		g = None
		
		if texinfo and latex == None:
			f = 'texi'
		elif latex and texinfo == None: 
			f = 'latex'
		else:
			error("error: can't determine format, please specify")
		__main__.format = f

	if __main__.paperguru == None:
		if __main__.format == 'texi':
			g = TexiPaper()
		else:
			g = LatexPaper()
			
		__main__.paperguru = g


def read_doc_file (filename):
	"""Read the input file, find verbatim chunks and do \input and \include
	"""
	(str, path) = find_file(filename)
	determine_format (str)
	
	chunks = [('input', str)]
	
	# we have to check for verbatim before doing include,
	# because we don't want to include files that are mentioned
	# inside a verbatim environment
	chunks = chop_chunks(chunks, 'verbatim', make_verbatim)
	chunks = chop_chunks(chunks, 'verb', make_verb)
	chunks = chop_chunks(chunks, 'multiline-comment', do_ignore)
	#ugh fix input
	chunks = chop_chunks(chunks, 'include', do_include_file, 1)
	chunks = chop_chunks(chunks, 'input', do_input_file, 1)
	return chunks


taken_file_names = {}
def schedule_lilypond_block (chunk):
	"""Take the body and options from CHUNK, figure out how the
	real .ly should look, and what should be left MAIN_STR (meant
	for the main file).  The .ly is written, and scheduled in
	TODO.

	Return: a chunk (TYPE_STR, MAIN_STR, OPTIONS, TODO, BASE)

	TODO has format [basename, extension, extension, ... ]
	
	"""
	(type, body, opts) = chunk
	assert type == 'lilypond'
	file_body = compose_full_body (body, opts)
	basename = 'lily-' + `abs(hash (file_body))`
	for o in opts:
		m = re.search ('filename="(.*?)"', o)
		if m:
			basename = m.group (1)
			if not taken_file_names.has_key(basename):
			    taken_file_names[basename] = 0
			else:
			    taken_file_names[basename] = taken_file_names[basename] + 1
			    basename = basename + "-%i" % taken_file_names[basename]
	if not g_read_lys:
		update_file(file_body, os.path.join(g_outdir, basename) + '.ly')
	needed_filetypes = ['tex']

	if format  == 'texi':
		needed_filetypes.append('eps')
		needed_filetypes.append('png')
	if 'eps' in opts and not ('eps' in needed_filetypes):
		needed_filetypes.append('eps')
	pathbase = os.path.join (g_outdir, basename)
	def f(base, ext1, ext2):
		a = os.path.isfile(base + ext2)
		if (os.path.isfile(base + ext1) and
		    os.path.isfile(base + ext2) and
				os.stat(base+ext1)[stat.ST_MTIME] >
				os.stat(base+ext2)[stat.ST_MTIME]) or \
				not os.path.isfile(base + ext2):
			return 1
	todo = []
	if 'tex' in needed_filetypes and f(pathbase, '.ly', '.tex'):
		todo.append('tex')
	if 'eps' in needed_filetypes and f(pathbase, '.tex', '.eps'):
		todo.append('eps')
	if 'png' in needed_filetypes and f(pathbase, '.eps', '.png'):
		todo.append('png')
	newbody = ''

	if 'printfilename' in opts:
		for o in opts:
			m= re.match ("filename=(.*)", o)
			if m:
				newbody = newbody + get_output ("output-filename") % m.group(1)
				break
		
	
	if 'verbatim' in opts:
		newbody = output_verbatim (body)

	for o in opts:
		m = re.search ('intertext="(.*?)"', o)
		if m:
			newbody = newbody  + m.group (1) + "\n\n"
	if format == 'latex':
		if 'eps' in opts:
			s = 'output-eps'
		else:
			s = 'output-tex'
	else: # format == 'texi'
 		s = 'output-all'
	newbody = newbody + get_output (s) % {'fn': basename }
	return ('lilypond', newbody, opts, todo, basename)

def process_lilypond_blocks(outname, chunks):#ugh rename
	newchunks = []
	# Count sections/chapters.
	for c in chunks:
		if c[0] == 'lilypond':
			c = schedule_lilypond_block (c)
		elif c[0] == 'numcols':
			paperguru.m_num_cols = c[2]
		newchunks.append (c)
	return newchunks


def find_eps_dims (match):
	"Fill in dimensions of EPS files."
	
	fn =match.group (1)
	dims = bounding_box_dimensions (fn)
	if g_outdir:
		fn = os.path.join(g_outdir, fn)
 	
	return '%ipt' % dims[0]


def system (cmd):
	sys.stderr.write ("invoking `%s'\n" % cmd)
	st = os.system (cmd)
	if st:
		error ('Error command exited with value %d\n' % st)
	return st

def compile_all_files (chunks):
	global foutn
	eps = []
	tex = []
	png = []

	for c in chunks:
		if c[0] <> 'lilypond':
			continue
		base  = c[4]
		exts = c[3]
		for e in exts:
			if e == 'eps':
				eps.append (base)
			elif e == 'tex':
				#ugh
				if base + '.ly' not in tex:
					tex.append (base + '.ly')
			elif e == 'png' and g_do_pictures:
				png.append (base)
	d = os.getcwd()
	if g_outdir:
		os.chdir(g_outdir)
	if tex:
		# fixme: be sys-independent.
		def incl_opt (x):
			if g_outdir and x[0] <> '/' :
				x = os.path.join (g_here_dir, x)
			return ' -I %s' % x

		incs = map (incl_opt, include_path)
		lilyopts = string.join (incs, ' ' )
		if do_deps:
			lilyopts = lilyopts + ' --dependencies '
			if g_outdir:
				lilyopts = lilyopts + '--dep-prefix=' + g_outdir + '/'
		texfiles = string.join (tex, ' ')
		system ('lilypond --header=texidoc %s %s' % (lilyopts, texfiles))

		#
		# Ugh, fixing up dependencies for .tex generation
		#
		if do_deps:
			depfiles=map (lambda x: re.sub ('(.*)\.ly', '\\1.dep', x), tex)
			for i in depfiles:
				f =open (i)
				text=f.read ()
				f.close ()
				text=re.sub ('\n([^:\n]*):', '\n' + foutn + ':', text)
				f = open (i, 'w')
				f.write (text)
				f.close ()

	for e in eps:
		system(r"tex '\nonstopmode \input %s'" % e)
		system(r"dvips -E -o %s %s" % (e + '.eps', e))
	for g in png:
		cmd = r"""gs -sDEVICE=pgm  -dTextAlphaBits=4 -dGraphicsAlphaBits=4  -q -sOutputFile=- -r90 -dNOPAUSE %s -c quit | pnmcrop | pnmtopng > %s"""
		cmd = cmd % (g + '.eps', g + '.png')
		try:
			status = system (cmd)
		except:
			os.unlink (g + '.png')
			error ("Removing output file")
		
	os.chdir (d)


def update_file (body, name):
	"""
	write the body if it has changed
	"""
	same = 0
	try:
		f = open (name)
		fs = f.read (-1)
		same = (fs == body)
	except:
		pass

	if not same:
		f = open (name , 'w')
		f.write (body)
		f.close ()
	
	return not same


def getopt_args (opts):
	"Construct arguments (LONG, SHORT) for getopt from  list of options."
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
	"Transform one option description (4-tuple ) into neatly formatted string"
	sh = '  '	
	if o[1]:
		sh = '-%s' % o[1]

	sep = ' '
	if o[1] and o[2]:
		sep = ','
		
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
	"Convert a list of options into a neatly formatted string"
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

def help():
	sys.stdout.write("""Usage: lilypond-book [options] FILE\n
Generate hybrid LaTeX input from Latex + lilypond
Options:
""")
	sys.stdout.write (options_help_str (option_definitions))
	sys.stdout.write (r"""Warning all output is written in the CURRENT directory



Report bugs to bug-lilypond@gnu.org.

Written by Tom Cato Amundsen <tca@gnu.org> and
Han-Wen Nienhuys <hanwen@cs.uu.nl>
""")

	sys.exit (0)


def write_deps (fn, target, chunks):
	global read_files
	sys.stdout.write('Writing `%s\'\n' % os.path.join(g_outdir, fn))
	f = open (os.path.join(g_outdir, fn), 'w')
	f.write ('%s%s: ' % (g_dep_prefix, target))
	for d in read_files:
		f.write ('%s ' %  d)
	basenames=[]
        for c in chunks:
	        if c[0] == 'lilypond':
			(type, body, opts, todo, basename) = c;
			basenames.append (basename)
	for d in basenames:
		if g_outdir:
			d=g_outdir + '/' + d
		if g_dep_prefix:
			#if not os.isfile (d): # thinko?
			if not re.search ('/', d):
				d = g_dep_prefix + d
		f.write ('%s.tex ' %  d)
	f.write ('\n')
	#if len (basenames):
	#	for d in basenames:
	#		f.write ('%s.ly ' %  d)
	#	f.write (' : %s' % target)
	f.write ('\n')
	f.close ()
	read_files = []

def identify():
	sys.stdout.write ('lilypond-book (GNU LilyPond) %s\n' % program_version)

def print_version ():
	identify()
	sys.stdout.write (r"""Copyright 1998--1999
Distributed under terms of the GNU General Public License. It comes with
NO WARRANTY.
""")


def check_texidoc (chunks):
	n = []
        for c in chunks:
	        if c[0] == 'lilypond':
			(type, body, opts, todo, basename) = c;
			pathbase = os.path.join (g_outdir, basename)
			if os.path.isfile (pathbase + '.texidoc'):
				body = '\n@include %s.texidoc\n' % basename + body
				c = (type, body, opts, todo, basename)
		n.append (c)
	return n

def fix_epswidth (chunks):
	newchunks = []
	for c in chunks:
		if c[0] == 'lilypond' and 'eps' in c[2]:
			body = re.sub (r"""\\lilypondepswidth{(.*?)}""", find_eps_dims, c[1])
			newchunks.append(('lilypond', body, c[2], c[3], c[4]))
		else:
			newchunks.append (c)
	return newchunks


foutn=""
def do_file(input_filename):
	global foutn
	file_settings = {}
	if outname:
		my_outname = outname
	else:
		my_outname = os.path.basename(os.path.splitext(input_filename)[0])
	my_depname = my_outname + '.dep'		

	chunks = read_doc_file(input_filename)
	chunks = chop_chunks(chunks, 'lilypond', make_lilypond, 1)
	chunks = chop_chunks(chunks, 'lilypond-file', make_lilypond_file, 1)
	chunks = chop_chunks(chunks, 'lilypond-block', make_lilypond_block, 1)
	chunks = chop_chunks(chunks, 'singleline-comment', do_ignore, 1)
	chunks = chop_chunks(chunks, 'preamble-end', do_preamble_end)
	chunks = chop_chunks(chunks, 'numcols', do_columns)
	#print "-" * 50
	#for c in chunks: print "c:", c;
	#sys.exit()
	scan_preamble(chunks)
	chunks = process_lilypond_blocks(my_outname, chunks)

	foutn = os.path.join (g_outdir, my_outname + '.' + format)

	# Do It.
	if __main__.g_run_lilypond:
		compile_all_files (chunks)
		chunks = fix_epswidth (chunks)

	if __main__.format == 'texi':
		chunks = check_texidoc (chunks)

	x = 0
	chunks = completize_preamble (chunks)
	sys.stderr.write ("Writing `%s'\n" % foutn)
	fout = open (foutn, 'w')
	for c in chunks:
		fout.write (c[1])
	fout.close ()
	# should chmod -w

	if do_deps:
		write_deps (my_depname, foutn, chunks)


outname = ''
try:
	(sh, long) = getopt_args (__main__.option_definitions)
	(options, files) = getopt.getopt(sys.argv[1:], sh, long)
except getopt.error, msg:
	sys.stderr.write("error: %s" % msg)
	sys.exit(1)

do_deps = 0
for opt in options:	
	o = opt[0]
	a = opt[1]

	if o == '--include' or o == '-I':
		include_path.append (a)
	elif o == '--version' or o == '-v':
		print_version ()
		sys.exit  (0)
	elif o == '--format' or o == '-f':
		__main__.format = a
	elif o == '--outname' or o == '-o':
		if len(files) > 1:
			#HACK
			sys.stderr.write("Lilypond-book is confused by --outname on multiple files")
			sys.exit(1)
		outname = a
	elif o == '--help' or o == '-h':
		help ()
	elif o == '--no-lily' or o == '-n':
		__main__.g_run_lilypond = 0
	elif o == '--dependencies' or o == '-M':
		do_deps = 1
	elif o == '--default-music-fontsize':
		default_music_fontsize = string.atoi (a)
	elif o == '--default-lilypond-fontsize':
		print "--default-lilypond-fontsize is deprecated, use --default-music-fontsize"
		default_music_fontsize = string.atoi (a)
	elif o == '--force-music-fontsize':
		g_force_lilypond_fontsize = string.atoi(a)
	elif o == '--force-lilypond-fontsize':
		print "--force-lilypond-fontsize is deprecated, use --default-lilypond-fontsize"
		g_force_lilypond_fontsize = string.atoi(a)
	elif o == '--dep-prefix':
		g_dep_prefix = a
	elif o == '--no-pictures':
		g_do_pictures = 0
	elif o == '--read-lys':
		g_read_lys = 1
	elif o == '--outdir':
		g_outdir = a

identify()
if g_outdir:
	if os.path.isfile(g_outdir):
		error ("outdir is a file: %s" % g_outdir)
	if not os.path.exists(g_outdir):
		os.mkdir(g_outdir)
setup_environment ()
for input_filename in files:
	do_file(input_filename)
	
#
# Petr, ik zou willen dat ik iets zinvoller deed,
# maar wat ik kan ik doen, het verandert toch niets?
#   --hwn 20/aug/99
