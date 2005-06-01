#!@PYTHON@

'''
Example usage:

test:
     lilypond-book --filter="tr '[a-z]' '[A-Z]'" BOOK

convert-ly on book:
     lilypond-book --filter="convert-ly --no-version --from=1.6.11 -" BOOK

classic lilypond-book:
     lilypond-book --process="lilypond" BOOK.tely

TODO:

    *  this script is too complex. Modularize.
    
    *  ly-options: intertext?
    *  --linewidth?
    *  eps in latex / eps by lilypond -b ps?
    *  check latex parameters, twocolumn, multicolumn?
    *  use --png --ps --pdf for making images?

    *  Converting from lilypond-book source, substitute:
       @mbinclude foo.itely -> @include foo.itely
       \mbinput -> \input

'''

import __main__
import glob
import stat
import string

# Users of python modules should include this snippet
# and customize variables below.

# We'll suffer this path initialization stuff as long as we don't install
# our python packages in <prefix>/lib/pythonX.Y (and don't kludge around
# it as we do with teTeX on Red Hat Linux: set some environment variables
# (PYTHONPATH) in `etc/profile').

# If set, LILYPONDPREFIX must take prevalence.
# if datadir is not set, we're doing a build and LILYPONDPREFIX.
import getopt, os, sys
datadir = '@local_lilypond_datadir@'
if not os.path.isdir (datadir):
	datadir = '@lilypond_datadir@'
if os.environ.has_key ('LILYPONDPREFIX'):
	datadir = os.environ['LILYPONDPREFIX']
	while datadir[-1] == os.sep:
		datadir= datadir[:-1]

sys.path.insert (0, os.path.join (datadir, 'python'))

# Customize these.
#if __name__ == '__main__':

import lilylib as ly
import fontextract
global _;_=ly._
global re;re = ly.re

# Lilylib globals.
program_version = '@TOPLEVEL_VERSION@'
program_name = os.path.basename (sys.argv[0])
verbose_p = 0
pseudo_filter_p = 0
original_dir = os.getcwd ()
backend = 'ps'

help_summary = _ (
'''Process LilyPond snippets in hybrid HTML, LaTeX, or texinfo document.
Example usage:

   lilypond-book --filter="tr '[a-z]' '[A-Z]'" BOOK
   lilypond-book --filter="convert-ly --no-version --from=2.0.0 -" BOOK
   lilypond-book --process='lilypond -I include' BOOK
''')

copyright = ('Jan Nieuwenhuizen <janneke@gnu.org>',
	     'Han-Wen Nienhuys <hanwen@cs.uu.nl>')

option_definitions = [
	(_ ("FMT"), 'f', 'format',
	  _ ('''use output format FMT (texi [default], texi-html,
		latex, html)''')),
	(_ ("FILTER"), 'F', 'filter',
	  _ ("pipe snippets through FILTER [convert-ly -n -]")),
	('', 'h', 'help',
	  _ ("print this help")),
	(_ ("DIR"), 'I', 'include',
	  _ ("add DIR to include path")),
	(_ ("DIR"), 'o', 'output',
	  _ ("write output to DIR")),
	(_ ("COMMAND"), 'P', 'process',
	  _ ("process ly_files using COMMAND FILE...")),
	(_(''), '', 'psfonts',
	 _ ('''extract all PostScript fonts into INPUT.psfonts for LaTeX
	 must use this with dvips -h INPUT.psfonts''')),
	('', 'V', 'verbose',
	  _ ("be verbose")),
	('', 'v', 'version',
	  _ ("print version information")),
	('', 'w', 'warranty',
	  _ ("show warranty and copyright")),
]

include_path = [ly.abspath (os.getcwd ())]
lilypond_binary = os.path.join ('@bindir@', 'lilypond')

# Only use installed binary when we are installed too.
if '@bindir@' == ('@' + 'bindir@') or not os.path.exists (lilypond_binary):
	lilypond_binary = 'lilypond'

psfonts_p = 0
use_hash_p = 1
format = 0
output_name = ''
latex_filter_cmd = 'latex "\\nonstopmode \input /dev/stdin"'
filter_cmd = 0
process_cmd = ''
default_ly_options = { 'alt': "[image of music]" }

#
# Is this pythonic?  Personally, I find this rather #define-nesque. --hwn
#
AFTER = 'after'
BEFORE = 'before'
EXAMPLEINDENT = 'exampleindent'
FILTER = 'filter'
FRAGMENT = 'fragment'
HTML = 'html'
INDENT = 'indent'
LATEX = 'latex'
LAYOUT = 'layout'
LINEWIDTH = 'linewidth'
NOFRAGMENT = 'nofragment'
NOINDENT = 'noindent'
NOQUOTE = 'noquote'
NOTES = 'body'
NOTIME = 'notime'
OUTPUT = 'output'
OUTPUTIMAGE = 'outputimage'
PACKED = 'packed'
PAPER = 'paper'
PREAMBLE = 'preamble'
PRINTFILENAME = 'printfilename'
QUOTE = 'quote'
RAGGEDRIGHT = 'raggedright'
RELATIVE = 'relative'
STAFFSIZE = 'staffsize'
TEXIDOC = 'texidoc'
TEXINFO = 'texinfo'
VERBATIM = 'verbatim'
FONTLOAD = 'fontload'
FILENAME = 'filename'
ALT = 'alt'


# NOTIME has no opposite so it isn't part of this dictionary.
# NOQUOTE is used internally only.
no_options = {
	NOFRAGMENT: FRAGMENT,
	NOINDENT: INDENT,
}


# Recognize special sequences in the input.
#
#   (?P<name>regex) -- Assign result of REGEX to NAME.
#   *? -- Match non-greedily.
#   (?m) -- Multiline regex: Make ^ and $ match at each line.
#   (?s) -- Make the dot match all characters including newline.
#   (?x) -- Ignore whitespace in patterns.
no_match = 'a\ba'
snippet_res = {
	##
	HTML: {
		'include':
		  no_match,

		'lilypond':
		  r'''(?mx)
		    (?P<match>
		    <lilypond
		      (\s*(?P<options>.*?)\s*:)?\s*
		      (?P<code>.*?)
		    />)''',

		'lilypond_block':
		  r'''(?msx)
		    (?P<match>
		    <lilypond
		      \s*(?P<options>.*?)\s*
		    >
		    (?P<code>.*?)
		    </lilypond>)''',

		'lilypond_file':
		  r'''(?mx)
		    (?P<match>
		    <lilypondfile
		      \s*(?P<options>.*?)\s*
		    >
		    \s*(?P<filename>.*?)\s*
		    </lilypondfile>)''',

		'multiline_comment':
		  r'''(?smx)
		    (?P<match>
		    \s*(?!@c\s+)
		    (?P<code><!--\s.*?!-->)
		    \s)''',

		'singleline_comment':
		  no_match,

		'verb':
		  r'''(?x)
		    (?P<match>
		      (?P<code><pre>.*?</pre>))''',

		'verbatim':
		  r'''(?x)
		    (?s)
		    (?P<match>
		      (?P<code><pre>\s.*?</pre>\s))''',
	},

	##
	LATEX: {
		'include':
		  r'''(?smx)
		    ^[^%\n]*?
		    (?P<match>
		    \\input\s*{
		      (?P<filename>\S+?)
		    })''',

		'lilypond':
		  r'''(?smx)
		    ^[^%\n]*?
		    (?P<match>
		    \\lilypond\s*(
		    \[
		      \s*(?P<options>.*?)\s*
		    \])?\s*{
		      (?P<code>.*?)
		    })''',

		'lilypond_block':
		  r'''(?smx)
		    ^[^%\n]*?
		    (?P<match>
		    \\begin\s*(
		    \[
		      \s*(?P<options>.*?)\s*
		    \])?\s*{lilypond}
		      (?P<code>.*?)
		    ^[^%\n]*?
		    \\end\s*{lilypond})''',

		'lilypond_file':
		  r'''(?smx)
		    ^[^%\n]*?
		    (?P<match>
		    \\lilypondfile\s*(
		    \[
		      \s*(?P<options>.*?)\s*
		    \])?\s*\{
		      (?P<filename>\S+?)
		    })''',

		'multiline_comment':
		  no_match,

		'singleline_comment':
		  r'''(?mx)
		    ^.*?
		    (?P<match>
		      (?P<code>
		      %.*$\n+))''',

		'verb':
		  r'''(?mx)
		    ^[^%\n]*?
		    (?P<match>
		      (?P<code>
		      \\verb(?P<del>.)
			.*?
		      (?P=del)))''',

		'verbatim':
		  r'''(?msx)
		    ^[^%\n]*?
		    (?P<match>
		      (?P<code>
		      \\begin\s*{verbatim}
			.*?
		      \\end\s*{verbatim}))''',
	},

	##
	TEXINFO: {
		'include':
		  r'''(?mx)
		    ^(?P<match>
		    @include\s+
		      (?P<filename>\S+))''',

		'lilypond':
		  r'''(?smx)
		    ^[^\n]*?(?!@c\s+)[^\n]*?
		    (?P<match>
		    @lilypond\s*(
		    \[
		      \s*(?P<options>.*?)\s*
		    \])?\s*{
		      (?P<code>.*?)
		    })''',

		'lilypond_block':
		  r'''(?msx)
		    ^(?P<match>
		    @lilypond\s*(
		    \[
		      \s*(?P<options>.*?)\s*
		    \])?\s+?
		    ^(?P<code>.*?)
		    ^@end\s+lilypond)\s''',

		'lilypond_file':
		  r'''(?mx)
		    ^(?P<match>
		    @lilypondfile\s*(
		    \[
		      \s*(?P<options>.*?)\s*
		    \])?\s*{
		      (?P<filename>\S+)
		    })''',

		'multiline_comment':
		  r'''(?smx)
		    ^(?P<match>
		      (?P<code>
		      @ignore\s
			.*?
		      @end\s+ignore))\s''',

		'singleline_comment':
		  r'''(?mx)
		    ^.*
		    (?P<match>
		      (?P<code>
		      @c([ \t][^\n]*|)\n))''',

	# Don't do this: It interferes with @code{@{}.
	#	'verb': r'''(?P<code>@code{.*?})''',

		'verbatim':
		  r'''(?sx)
		    (?P<match>
		      (?P<code>
		      @example
			\s.*?
		      @end\s+example\s))''',
	},
}

format_res = {
	HTML: {
		'intertext': r',?\s*intertext=\".*?\"',
		'option_sep': '\s*',
	},

	LATEX: {
		'intertext': r',?\s*intertext=\".*?\"',
		'option_sep': '\s*,\s*',
	},

	TEXINFO: {
		'intertext': r',?\s*intertext=\".*?\"',
		'option_sep': '\s*,\s*',
	},
}

# Options without a pattern in ly_options.
simple_options = [
	EXAMPLEINDENT,
	FRAGMENT,
	NOFRAGMENT,
	NOINDENT,
	PRINTFILENAME,
	TEXIDOC,
	VERBATIM,
	FONTLOAD,
	FILENAME,
	ALT
]

ly_options = {
	##
	NOTES: {
		RELATIVE: r'''\relative c%(relative_quotes)s''',
	},

	##
	PAPER: {
		INDENT: r'''indent = %(indent)s''',

		LINEWIDTH: r'''linewidth = %(linewidth)s''',

		QUOTE: r'''linewidth = %(linewidth)s - 2.0 * %(exampleindent)s''',

		RAGGEDRIGHT: r'''raggedright = ##t''',

		PACKED: r'''packed = ##t''',
	},

	##
	LAYOUT: {
		NOTIME: r'''
  \context {
    \Score
    timing = ##f
  }
  \context {
    \Staff
    \remove Time_signature_engraver
  }''',
	},

	##
	PREAMBLE: {
		STAFFSIZE: r'''#(set-global-staff-size %(staffsize)s)''',
	},
}

output = {
	##
	HTML: {
		FILTER: r'''<lilypond %(options)s>
%(code)s
</lilypond>
''',

		AFTER: r'''
  </a>
</p>''',

		BEFORE: r'''<p>
  <a href="%(base)s.ly">''',

		OUTPUT: r'''
    <img align="center" valign="center"
	 border="0" src="%(image)s" alt="%(alt)s">''',

		PRINTFILENAME: '<p><tt><a href="%(base)s.ly">%(filename)s</a></tt></p>',

		QUOTE: r'''<blockquote>
%(str)s
</blockquote>
''',

		VERBATIM: r'''<pre>
%(verb)s</pre>''',
	},

	##
	LATEX: {
		OUTPUT: r'''{%%
\parindent 0pt
\catcode`\@=12
\ifx\preLilyPondExample \undefined
  \relax
\else
  \preLilyPondExample
\fi
\def\lilypondbook{}%%
\input %(base)s-systems.tex
\ifx\postLilyPondExample \undefined
  \relax
\else
  \postLilyPondExample
\fi
}''',

		PRINTFILENAME: '''\\texttt{%(filename)s}
	''',

		QUOTE: r'''\begin{quotation}
%(str)s
\end{quotation}
''',

		VERBATIM: r'''\noindent
\begin{verbatim}
%(verb)s\end{verbatim}
''',

		FILTER: r'''\begin{lilypond}[%(options)s]
%(code)s
\end{lilypond}''',
	},

	##
	TEXINFO: {
		FILTER: r'''@lilypond[%(options)s]
%(code)s
@lilypond''',

		OUTPUT: r'''
@iftex
@include %(base)s-systems.texi
@end iftex
''',

		OUTPUTIMAGE: r'''@noindent
@ifinfo
@image{%(base)s,,,%(alt)s,%(ext)s}
@end ifinfo
@html
<p>
  <a href="%(base)s.ly">
    <img align="center" valign="center"
	 border="0" src="%(image)s" alt="%(alt)s">
  </a>
</p>
@end html
''',

		PRINTFILENAME: '''@file{%(filename)s}
	''',

		QUOTE: r'''@quotation
%(str)s@end quotation
''',

		NOQUOTE: r'''@format
%(str)s@end format
''',

		VERBATIM: r'''@exampleindent 0
@example
%(verb)s@end example
''',
	},
}

PREAMBLE_LY = r'''%%%% Generated by %(program_name)s
%%%% Options: [%(option_string)s]

#(set! toplevel-score-handler ly:parser-print-score)
#(set! toplevel-music-handler (lambda (p m)
			       (ly:parser-print-score
				p (ly:music-scorify m p))))

#(define version-seen? #t)
%(preamble_string)s


%% ********************************
%% Start cut-&-pastable-section 
%% ********************************

\paper {
  #(define dump-extents #t)
  %(font_dump_setting)s
  %(paper_string)s
}

\layout {
  %(layout_string)s
}
'''

FRAGMENT_LY = r'''
%(notes_string)s
{
%% ly snippet contents follows:
%(code)s
%% end ly snippet
}
'''

FULL_LY = '''
%% ly snippet:
%(code)s
%% end ly snippet
'''

texinfo_linewidths = {
	'@afourpaper': '160\\mm',
	'@afourwide': '6.5\\in',
	'@afourlatex': '150\\mm',
	'@smallbook': '5\\in',
	'@letterpaper': '6\\in',
}

def classic_lilypond_book_compatibility (key, value):
	if key == 'singleline' and value == None:
		return (RAGGEDRIGHT, None)

	m = re.search ('relative\s*([-0-9])', key)
	if m:
		return ('relative', m.group (1))

	m = re.match ('([0-9]+)pt', key)
	if m:
		return ('staffsize', m.group (1))

	if key == 'indent' or key == 'linewidth':
		m = re.match ('([-.0-9]+)(cm|in|mm|pt|staffspace)', value)
		if m:
			f = float (m.group (1))
			return (key, '%f\\%s' % (f, m.group (2)))

	return (None, None)

def find_file (name):
	for i in include_path:
		full = os.path.join (i, name)
		if os.path.exists (full):
			return full
	ly.error (_ ("file not found: %s") % name + '\n')
	ly.exit (1)
	return ''

def verbatim_html (s):
	return re.sub ('>', '&gt;',
		       re.sub ('<', '&lt;',
			       re.sub ('&', '&amp;', s)))

def verbatim_texinfo (s):
	return re.sub ('{', '@{',
		       re.sub ('}', '@}',
			       re.sub ('@', '@@', s)))

def split_options (option_string):
	if option_string:
		if format == HTML:
			options = re.findall('[\w\.-:]+(?:\s*=\s*(?:"[^"]*"|\'[^\']*\'|\S+))?',option_string)
			for i in range(len(options)):
				options[i] = re.sub('^([^=]+=\s*)(?P<q>["\'])(.*)(?P=q)','\g<1>\g<3>',options[i])
			return options
		else:
			return re.split (format_res[format]['option_sep'],
					 option_string)
	return []

def invokes_lilypond ():
	return re.search ('^[\'\"0-9A-Za-z/]*lilypond', process_cmd)

def set_default_options (source):
	global default_ly_options
	if not default_ly_options.has_key (LINEWIDTH):
		if format == LATEX:
			textwidth = get_latex_textwidth (source)
			default_ly_options[LINEWIDTH] = \
			  '''%.0f\\pt''' % textwidth
		elif format == TEXINFO:
			for (k, v) in texinfo_linewidths.items ():
				# FIXME: @layout is usually not in
				# chunk #0:
				#
				#  \input texinfo @c -*-texinfo-*-
				#
				# Bluntly search first K items of
				# source.
				# s = chunks[0].replacement_text ()
				if re.search (k, source[:1024]):
					default_ly_options[LINEWIDTH] = v
					break

class Chunk:
	def replacement_text (self):
		return ''

	def filter_text (self):
		return self.replacement_text ()

	def ly_is_outdated (self):
		return 0

	def png_is_outdated (self):
		return 0

class Substring (Chunk):
	def __init__ (self, source, start, end, line_number):
		self.source = source
		self.start = start
		self.end = end
		self.line_number = line_number

	def replacement_text (self):
		return self.source[self.start:self.end]

class Snippet (Chunk):
	def __init__ (self, type, match, format, line_number):
		self.type = type
		self.match = match
		self.hash = 0
		self.option_dict = {}
		self.format = format
		self.line_number = line_number

	def replacement_text (self):
		return self.match.group ('match')

	def substring (self, s):
		return self.match.group (s)

	def __repr__ (self):
		return `self.__class__` + ' type = ' + self.type

class Include_snippet (Snippet):
	def processed_filename (self):
		f = self.substring ('filename')
		return os.path.splitext (f)[0] + format2ext[format]

	def replacement_text (self):
		s = self.match.group ('match')
		f = self.substring ('filename')

		return re.sub (f, self.processed_filename (), s)

class Lilypond_snippet (Snippet):
	def __init__ (self, type, match, format, line_number):
		Snippet.__init__ (self, type, match, format, line_number)
		os = match.group ('options')
		self.do_options (os, self.type)

	def ly (self):
		return self.substring ('code')

	def full_ly (self):
		s = self.ly ()
		if s:
			return self.compose_ly (s)
		return ''

	def do_options (self, option_string, type):
		self.option_dict = {}

		options = split_options (option_string)

		for i in options:
			if string.find (i, '=') > 0:
				(key, value) = re.split ('\s*=\s*', i)
				self.option_dict[key] = value
			else:
				if i in no_options.keys ():
					if no_options[i] in self.option_dict.keys ():
						del self.option_dict[no_options[i]]
				else:
					self.option_dict[i] = None

		has_linewidth = self.option_dict.has_key (LINEWIDTH)
		no_linewidth_value = 0

		# If LINEWIDTH is used without parameter, set it to default.
		if has_linewidth and self.option_dict[LINEWIDTH] == None:
			no_linewidth_value = 1
			del self.option_dict[LINEWIDTH]

		for i in default_ly_options.keys ():
			if i not in self.option_dict.keys ():
				self.option_dict[i] = default_ly_options[i]

		if not has_linewidth:
			if type == 'lilypond' or FRAGMENT in self.option_dict.keys ():
				self.option_dict[RAGGEDRIGHT] = None

			if type == 'lilypond':
				if LINEWIDTH in self.option_dict.keys ():
					del self.option_dict[LINEWIDTH]
			else:
				if RAGGEDRIGHT in self.option_dict.keys ():
					if LINEWIDTH in self.option_dict.keys ():
						del self.option_dict[LINEWIDTH]

			if QUOTE in self.option_dict.keys () or type == 'lilypond':
				if LINEWIDTH in self.option_dict.keys ():
					del self.option_dict[LINEWIDTH]

		if not INDENT in self.option_dict.keys ():
			self.option_dict[INDENT] = '0\\mm'

		# The QUOTE pattern from ly_options only emits the `linewidth'
		# keyword.
		if has_linewidth and QUOTE in self.option_dict.keys ():
			if no_linewidth_value:
				del self.option_dict[LINEWIDTH]
			else:
				del self.option_dict[QUOTE]

	def compose_ly (self, code):
		if FRAGMENT in self.option_dict.keys ():
			body = FRAGMENT_LY
		else:
			body = FULL_LY

		# Defaults.
		relative = 1
		override = {}
		# The original concept of the `exampleindent' option is broken.
		# It is not possible to get a sane value for @exampleindent at all
		# without processing the document itself.  Saying
		#
		#   @exampleindent 0
		#   @example
		#   ...
		#   @end example
		#   @exampleindent 5
		#
		# causes ugly results with the DVI backend of texinfo since the
		# default value for @exampleindent isn't 5em but 0.4in (or a smaller
		# value).  Executing the above code changes the environment
		# indentation to an unknown value because we don't know the amount
		# of 1em in advance since it is font-dependent.  Modifying
		# @exampleindent in the middle of a document is simply not
		# supported within texinfo.
		#
		# As a consequence, the only function of @exampleindent is now to
		# specify the amount of indentation for the `quote' option.
		#
		# To set @exampleindent locally to zero, we use the @format
		# environment for non-quoted snippets.
		override[EXAMPLEINDENT] = r'0.4\in'
		override[LINEWIDTH] = texinfo_linewidths['@smallbook']
		override.update (default_ly_options)

		option_list = []
		for (key, value) in self.option_dict.items ():
			if value == None:
				option_list.append (key)
			else:
				option_list.append (key + '=' + value)
		option_string = string.join (option_list, ',')

		compose_dict = {}
		compose_types = [NOTES, PREAMBLE, LAYOUT, PAPER]
		for a in compose_types:
			compose_dict[a] = []

		for (key, value) in self.option_dict.items ():
			(c_key, c_value) = \
			  classic_lilypond_book_compatibility (key, value)
			if c_key:
				if c_value:
					ly.warning \
					  (_ ("deprecated ly-option used: %s=%s" \
					    % (key, value)))
					ly.warning \
					  (_ ("compatibility mode translation: %s=%s" \
					    % (c_key, c_value)))
				else:
					ly.warning \
					  (_ ("deprecated ly-option used: %s" \
					    % key))
					ly.warning \
					  (_ ("compatibility mode translation: %s" \
					    % c_key))

				(key, value) = (c_key, c_value)

			if value:
				override[key] = value
			else:
				if not override.has_key (key):
					override[key] = None

			found = 0
			for type in compose_types:
				if ly_options[type].has_key (key):
					compose_dict[type].append (ly_options[type][key])
					found = 1
					break

			if not found and key not in simple_options:
				ly.warning (_ ("ignoring unknown ly option: %s") % key)

		# URGS
		if RELATIVE in override.keys () and override[RELATIVE]:
			relative = string.atoi (override[RELATIVE])

		relative_quotes = ''

		# 1 = central C
		if relative < 0:
			relative_quotes += ',' * (- relative)
		elif relative > 0:
			relative_quotes += "'" * relative

		program_name = __main__.program_name

		paper_string = string.join (compose_dict[PAPER],
					    '\n  ') % override
		layout_string = string.join (compose_dict[LAYOUT],
					     '\n  ') % override
		notes_string = string.join (compose_dict[NOTES],
					    '\n  ') % vars ()
		preamble_string = string.join (compose_dict[PREAMBLE],
					       '\n  ') % override

		font_dump_setting = ''
		if FONTLOAD in self.option_dict:
			font_dump_setting = '#(define-public force-eps-font-include #t)\n'

		return (PREAMBLE_LY + body) % vars ()

	# TODO: Use md5?
	def get_hash (self):
		if not self.hash:
			self.hash = abs (hash (self.full_ly ()))
		return self.hash

	def basename (self):
		if FILENAME in self.option_dict:
			return self.option_dict[FILENAME]
		if use_hash_p:
			return 'lily-%d' % self.get_hash ()
		raise 'to be done'

	def write_ly (self):
		outf = open (self.basename () + '.ly', 'w')
		outf.write (self.full_ly ())

		open (self.basename () + '.txt', 'w').write ('image of music')

	def ly_is_outdated (self):
		base = self.basename ()

		tex_file = '%s.tex' % base
		eps_file = '%s.eps' % base
		system_file = '%s-systems.tex' % base
		ly_file = '%s.ly' % base
		ok = os.path.exists (ly_file) \
		     and os.path.exists (system_file)\
		     and os.stat (system_file)[stat.ST_SIZE] \
		     and re.match ('% eof', open (system_file).readlines ()[-1])
		if ok and (not use_hash_p or FILENAME in self.option_dict):
			ok = (self.full_ly () == open (ly_file).read ())
		if ok:
			# TODO: Do something smart with target formats
			#       (ps, png) and m/ctimes.
			return None
		return self

	def png_is_outdated (self):
		base = self.basename ()
		ok = self.ly_is_outdated ()
		if format == HTML or format == TEXINFO:
			ok = ok and (os.path.exists (base + '.png')
				     or glob.glob (base + '-page*.png'))
		return not ok
	def texstr_is_outdated (self):
		if backend == 'ps':
			return 0

		base = self.basename ()
		ok = self.ly_is_outdated ()
		ok = ok and (os.path.exists (base + '.texstr'))
		return not ok

	def filter_text (self):
		code = self.substring ('code')
		s = run_filter (code)
		d = {
			'code': s,
			'options': self.match.group ('options')
		}
		# TODO
		return output[self.format][FILTER] % d

	def replacement_text (self):
		func = Lilypond_snippet.__dict__['output_' + self.format]
		return func (self)

	def get_images (self):
		base = self.basename ()
		# URGUGHUGHUGUGH
		single = '%(base)s.png' % vars ()
		multiple = '%(base)s-page1.png' % vars ()
		images = (single,)
		if os.path.exists (multiple) \
		   and (not os.path.exists (single) \
			or (os.stat (multiple)[stat.ST_MTIME] \
			    > os.stat (single)[stat.ST_MTIME])):
			images = glob.glob ('%(base)s-page*.png' % vars ())
		return images

	def output_html (self):
		str = ''
		base = self.basename ()
		if format == HTML:
			str += self.output_print_filename (HTML)
			if VERBATIM in self.option_dict:
				verb = verbatim_html (self.substring ('code'))
				str += write (output[HTML][VERBATIM] % vars ())
			if QUOTE in self.option_dict:
				str = output[HTML][QUOTE] % vars ()

		str += output[HTML][BEFORE] % vars ()
		for image in self.get_images ():
			(base, ext) = os.path.splitext (image)
			alt = self.option_dict[ALT]
			str += output[HTML][OUTPUT] % vars ()
		str += output[HTML][AFTER] % vars ()
		return str

	def output_info (self):
		str = ''
		for image in self.get_images ():
			(base, ext) = os.path.splitext (image)

			# URG, makeinfo implicitly prepends dot to extension.
			# Specifying no extension is most robust.
			ext = ''
			alt = self.option_dict[ALT]
			str += output[TEXINFO][OUTPUTIMAGE] % vars ()

		base = self.basename ()
		str += output[format][OUTPUT] % vars ()
		return str

	def output_latex (self):
		str = ''
		base = self.basename ()
		if format == LATEX:
			str += self.output_print_filename (LATEX)
			if VERBATIM in self.option_dict:
				verb = self.substring ('code')
				str += (output[LATEX][VERBATIM] % vars ())
			if QUOTE in self.option_dict:
				str = output[LATEX][QUOTE] % vars ()

		str += (output[LATEX][OUTPUT] % vars ())
		return str

	def output_print_filename (self, format):
		str = ''
		if PRINTFILENAME in self.option_dict:
			base = self.basename ()
			filename = self.substring ('filename')
			str = output[format][PRINTFILENAME] % vars ()

		return str

	def output_texinfo (self):
		str = ''
		if self.output_print_filename (TEXINFO):
			str += ('@html\n'
				+ self.output_print_filename (HTML)
				+ '\n@end html\n')
			str += ('@tex\n'
				+ self.output_print_filename (LATEX)
				+ '\n@end tex\n')
		base = self.basename ()
		if TEXIDOC in self.option_dict:
			texidoc = base + '.texidoc'
			if os.path.exists (texidoc):
				str += '@include %(texidoc)s\n\n' % vars ()

		if VERBATIM in self.option_dict:
			verb = verbatim_texinfo (self.substring ('code'))
			str += (output[TEXINFO][VERBATIM] % vars ())
			if not QUOTE in self.option_dict:
				str = output[TEXINFO][NOQUOTE] % vars ()

		str += self.output_info ()

#		str += ('@ifinfo\n' + self.output_info () + '\n@end ifinfo\n')
#		str += ('@tex\n' + self.output_latex () + '\n@end tex\n')
#		str += ('@html\n' + self.output_html () + '\n@end html\n')

		if QUOTE in self.option_dict:
			str = output[TEXINFO][QUOTE] % vars ()

		# need par after image
		str += '\n'

		return str

class Lilypond_file_snippet (Lilypond_snippet):
	def ly (self):
		name = self.substring ('filename')
		return '\\renameinput \"%s\"\n%s' \
			 % (name, open (find_file (name)).read ())

snippet_type_to_class = {
	'lilypond_file': Lilypond_file_snippet,
	'lilypond_block': Lilypond_snippet,
	'lilypond': Lilypond_snippet,
	'include': Include_snippet,
}

def find_linestarts (s):
	nls = [0]
	start = 0
	end = len (s)
	while 1:
		i = s.find ('\n', start)
		if i < 0:
			break

		i = i + 1
		nls.append (i)
		start = i

	nls.append (len (s))
	return nls

def find_toplevel_snippets (s, types):
	res = {}
	for i in types:
		res[i] = ly.re.compile (snippet_res[format][i])

	snippets = []
	index = 0
	## found = dict (map (lambda x: (x, None),
	##		      types))
	## urg python2.1
	found = {}
	map (lambda x, f = found: f.setdefault (x, None),
	     types)

	line_starts = find_linestarts (s)
	line_start_idx = 0
	# We want to search for multiple regexes, without searching
	# the string multiple times for one regex.
	# Hence, we use earlier results to limit the string portion
	# where we search.
	# Since every part of the string is traversed at most once for
	# every type of snippet, this is linear.

	while 1:
		first = None
		endex = 1 << 30
		for type in types:
			if not found[type] or found[type][0] < index:
				found[type] = None
				m = res[type].search (s[index:endex])
				if not m:
					continue

				cl = Snippet
				if snippet_type_to_class.has_key (type):
					cl = snippet_type_to_class[type]


				start = index + m.start ('match')
				line_number = line_start_idx
				while (line_starts[line_number] < start):
					line_number += 1

				line_number += 1
				snip = cl (type, m, format, line_number)
				found[type] = (start, snip)

			if found[type] \
			   and (not first \
				or found[type][0] < found[first][0]):
				first = type

				# FIXME.

				# Limiting the search space is a cute
				# idea, but this *requires* to search
				# for possible containing blocks
				# first, at least as long as we do not
				# search for the start of blocks, but
				# always/directly for the entire
				# @block ... @end block.

				endex = found[first][0]

		if not first:
			snippets.append (Substring (s, index, len (s), line_start_idx))
			break

		while (start > line_starts[line_start_idx+1]):
			line_start_idx += 1

		(start, snip) = found[first]
		snippets.append (Substring (s, index, start, line_start_idx + 1))
		snippets.append (snip)
		found[first] = None
		index = start + len (snip.match.group ('match'))

	return snippets

def filter_pipe (input, cmd):
	if verbose_p:
		ly.progress (_ ("Opening filter `%s'") % cmd)

	(stdin, stdout, stderr) = os.popen3 (cmd)
	stdin.write (input)
	status = stdin.close ()

	if not status:
		status = 0
		output = stdout.read ()
		status = stdout.close ()
		error = stderr.read ()

	if not status:
		status = 0
	signal = 0x0f & status
	if status or (not output and error):
		exit_status = status >> 8
		ly.error (_ ("`%s' failed (%d)") % (cmd, exit_status))
		ly.error (_ ("The error log is as follows:"))
		sys.stderr.write (error)
		sys.stderr.write (stderr.read ())
		ly.exit (status)

	if verbose_p:
		ly.progress ('\n')

	return output

def run_filter (s):
	return filter_pipe (s, filter_cmd)

def is_derived_class (cl, baseclass):
	if cl == baseclass:
		return 1
	for b in cl.__bases__:
		if is_derived_class (b, baseclass):
			return 1
	return 0

def process_snippets (cmd, ly_snippets, texstr_snippets, png_snippets):
	ly_names = filter (lambda x: x,
			   map (Lilypond_snippet.basename, ly_snippets))
	texstr_names = filter (lambda x: x,
			   map (Lilypond_snippet.basename, texstr_snippets))
	png_names = filter (lambda x: x,
			    map (Lilypond_snippet.basename, png_snippets))

	status = 0
	def my_system (cmd):
		status = ly.system (cmd,
				    ignore_error = 1, progress_p = 1)

		if status:
			ly.error ('Process %s exited unsuccessfully.' % cmd)
			raise Compile_error

	# UGH
	# the --process=CMD switch is a bad idea
	# it is too generic for lilypond-book.
	if texstr_names and invokes_lilypond:
		my_system (string.join ([cmd, '--backend texstr',
					 'snippet-map.ly'] + texstr_names))
		for l in texstr_names:
			my_system ('latex %s.texstr' % l)

	if ly_names:
		my_system (string.join ([cmd, 'snippet-map.ly'] + ly_names))

LATEX_DOCUMENT = r'''
%(preamble)s
\begin{document}
\typeout{textwidth=\the\textwidth}
\typeout{columnsep=\the\columnsep}
\makeatletter\if@twocolumn\typeout{columns=2}\fi\makeatother
\end{document}
'''

# Do we need anything else besides `textwidth'?
def get_latex_textwidth (source):
	m = re.search (r'''(?P<preamble>\\begin\s*{document})''', source)
	preamble = source[:m.start (0)]
	latex_document = LATEX_DOCUMENT % vars ()
	parameter_string = filter_pipe (latex_document, latex_filter_cmd)

	columns = 0
	m = re.search ('columns=([0-9.]*)', parameter_string)
	if m:
		columns = string.atoi (m.group (1))

	columnsep = 0
	m = re.search ('columnsep=([0-9.]*)pt', parameter_string)
	if m:
		columnsep = string.atof (m.group (1))

	textwidth = 0
	m = re.search ('textwidth=([0-9.]*)pt', parameter_string)
	if m:
		textwidth = string.atof (m.group (1))
		if columns:
			textwidth = (textwidth - columnsep) / columns

	return textwidth

ext2format = {
	'.html': HTML,
	'.itely': TEXINFO,
	'.latex': LATEX,
	'.lytex': LATEX,
	'.tely': TEXINFO,
	'.tex': LATEX,
	'.texi': TEXINFO,
	'.texinfo': TEXINFO,
	'.xml': HTML,
}

format2ext = {
	HTML: '.html',
	# TEXINFO: '.texinfo',
	TEXINFO: '.texi',
	LATEX: '.tex',
}

class Compile_error:
	pass

def write_file_map (lys, name):
	snippet_map = open ('snippet-map.ly', 'w')
	snippet_map.write ("""
#(define version-seen? #t)
#(ly:add-file-name-alist '(
""")
	for ly in lys:
		snippet_map.write ('("%s.ly" . "%s:%d (%s.ly)")\n'
				   % (ly.basename (),
				      name,
				      ly.line_number,
				      ly.basename ()))

	snippet_map.write ('))\n')

def do_process_cmd (chunks, input_name):
	all_lys = filter (lambda x: is_derived_class (x.__class__,
						      Lilypond_snippet),
			  chunks)

	write_file_map (all_lys, input_name)
	ly_outdated = \
	  filter (lambda x: is_derived_class (x.__class__,
					      Lilypond_snippet)
			    and x.ly_is_outdated (),
	          chunks)
	texstr_outdated = \
	  filter (lambda x: is_derived_class (x.__class__,
					      Lilypond_snippet)
			    and x.texstr_is_outdated (),
	          chunks)
	png_outdated = \
	  filter (lambda x: is_derived_class (x.__class__,
					      Lilypond_snippet)
			    and x.png_is_outdated (),
		  chunks)

	ly.progress (_ ("Writing snippets..."))
	map (Lilypond_snippet.write_ly, ly_outdated)
	ly.progress ('\n')

	if ly_outdated:
		ly.progress (_ ("Processing..."))
		ly.progress ('\n')
		process_snippets (process_cmd, ly_outdated, texstr_outdated, png_outdated)
	else:
		ly.progress (_ ("All snippets are up to date..."))
	ly.progress ('\n')

def guess_format (input_filename):
	format = None
	e = os.path.splitext (input_filename)[1]
	if e in ext2format.keys ():
		# FIXME
		format = ext2format[e]
	else:
		ly.error (_ ("can't determine format for: %s" \
			     % input_filename))
		ly.exit (1)
	return format

def write_if_updated (file_name, lines):
	try:
		f = open (file_name)
		oldstr = f.read ()
		new_str = string.join (lines, '')
		if oldstr == new_str:
			ly.progress (_ ("%s is up to date.") % file_name)
			ly.progress ('\n')
			return
	except:
		pass

	ly.progress (_ ("Writing `%s'...") % file_name)
	open (file_name, 'w').writelines (lines)
	ly.progress ('\n')

def do_file (input_filename):
	# Ugh.
	if not input_filename or input_filename == '-':
		in_handle = sys.stdin
		input_fullname = '<stdin>'
	else:
		if os.path.exists (input_filename):
			input_fullname = input_filename
		elif format == LATEX:
			# urg python interface to libkpathsea?
			input_fullname = ly.read_pipe ('kpsewhich '
						       + input_filename)[:-1]
		else:
			input_fullname = find_file (input_filename)
		in_handle = open (input_fullname)

	if input_filename == '-':
		input_base = 'stdin'
	else:
		input_base = os.path.basename \
			     (os.path.splitext (input_filename)[0])

	# Only default to stdout when filtering.
	if output_name == '-' or (not output_name and filter_cmd):
		output_filename = '-'
		output_file = sys.stdout
	else:
		if output_name:
			if not os.path.isdir (output_name):
				os.mkdir (output_name, 0777)
			os.chdir (output_name)

		output_filename = input_base + format2ext[format]
		if os.path.exists (input_filename) \
		   and os.path.exists (output_filename) \
		   and os.path.samefile (output_filename, input_fullname):
			ly.error (
			  _ ("Output would overwrite input file; use --output."))
			ly.exit (2)

	try:
		ly.progress (_ ("Reading %s...") % input_fullname)
		source = in_handle.read ()
		ly.progress ('\n')

		set_default_options (source)


		# FIXME: Containing blocks must be first, see
		#        find_toplevel_snippets.
		snippet_types = (
			'multiline_comment',
			'verbatim',
			'lilypond_block',
	#		'verb',
			'singleline_comment',
			'lilypond_file',
			'include',
			'lilypond',
		)
		ly.progress (_ ("Dissecting..."))
		chunks = find_toplevel_snippets (source, snippet_types)
		ly.progress ('\n')

		if filter_cmd:
			write_if_updated (output_filename,
					  [c.filter_text () for c in chunks])
		elif process_cmd:
			do_process_cmd (chunks, input_fullname)
			ly.progress (_ ("Compiling %s...") % output_filename)
			ly.progress ('\n')
			write_if_updated (output_filename,
					  [s.replacement_text ()
					   for s in chunks])
		
		def process_include (snippet):
			os.chdir (original_dir)
			name = snippet.substring ('filename')
			ly.progress (_ ("Processing include: %s") % name)
			ly.progress ('\n')
			return do_file (name)

		include_chunks = map (process_include,
				      filter (lambda x: is_derived_class (x.__class__,
									  Include_snippet),
					      chunks))


		return chunks + reduce (lambda x,y: x + y, include_chunks, [])
		
	except Compile_error:
		os.chdir (original_dir)
		ly.progress (_ ("Removing `%s'") % output_filename)
		ly.progress ('\n')
		raise Compile_error

def do_options ():
	global format, output_name, psfonts_p
	global filter_cmd, process_cmd, verbose_p

	(sh, long) = ly.getopt_args (option_definitions)
	try:
		(options, files) = getopt.getopt (sys.argv[1:], sh, long)
	except getopt.error, s:
		sys.stderr.write ('\n')
		ly.error (_ ("getopt says: `%s'" % s))
		sys.stderr.write ('\n')
		ly.help ()
		ly.exit (2)

	for opt in options:
		o = opt[0]
		a = opt[1]

		if 0:
			pass
		elif o == '--filter' or o == '-F':
			filter_cmd = a
			process_cmd = 0
		elif o == '--format' or o == '-f':
			format = a
			if a == 'texi-html' or a == 'texi':
				format = TEXINFO
		elif o == '--tex-backend ':
			backend = 'tex'
		elif o == '--help' or o == '-h':
			ly.help ()
			sys.exit (0)
		elif o == '--include' or o == '-I':
			include_path.append (os.path.join (original_dir,
							   ly.abspath (a)))
		elif o == '--output' or o == '-o':
			output_name = a
		elif o == '--process' or o == '-P':
			process_cmd = a
			filter_cmd = 0
		elif o == '--version' or o == '-v':
			ly.identify (sys.stdout)
			sys.exit (0)
		elif o == '--verbose' or o == '-V':
			verbose_p = 1
		elif o == '--psfonts':
			psfonts_p = 1 
		elif o == '--warranty' or o == '-w':
			if 1 or status:
				ly.warranty ()
			sys.exit (0)
	return files

def main ():
	files = do_options ()
	if not files or len (files) > 1:
		ly.help ()
		ly.exit (2)

	file = files[0]

	basename = os.path.splitext (file)[0]
	
	global process_cmd, format
	if not format:
		format = guess_format (files[0])

	formats = 'ps'
	if format == TEXINFO or format == HTML:
		formats += ',png'
	if process_cmd == '':
		process_cmd = lilypond_binary \
			      + ' --formats=%s --backend eps ' % formats

	if process_cmd:
		process_cmd += string.join ([(' -I %s' % p)
					     for p in include_path])

	ly.identify (sys.stderr)
	ly.setup_environment ()

	try:
		chunks = do_file (file)
		if psfonts_p and invokes_lilypond ():
			fontextract.verbose = verbose_p
			snippet_chunks = filter (lambda x: is_derived_class (x.__class__,
									      Lilypond_snippet),
						 chunks)

			psfonts_file = basename + '.psfonts' 
			if not verbose_p:
				ly.progress (_ ("Writing fonts to %s...") % psfonts_file)
			fontextract.extract_fonts (psfonts_file,
						   [x.basename() + '.eps'
						    for x in snippet_chunks])
			if not verbose_p:
				ly.progress ('\n')
			
	except Compile_error:
		ly.exit (1)

	if format == TEXINFO or format == LATEX:
		if not psfonts_p:
			ly.warning (_ ("option --psfonts not used"))
			ly.warning (_ ("processing with dvips will have no fonts"))

		psfonts_file = os.path.join (output_name, basename + '.psfonts')
		output = os.path.join (output_name, basename +  '.dvi' )
		
		ly.progress ('\n')
		ly.progress (_ ("DVIPS usage:"))
		ly.progress ('\n')
		ly.progress ("    dvips -h %(psfonts_file)s %(output)s" % vars ())
		ly.progress ('\n')

if __name__ == '__main__':
	main ()
