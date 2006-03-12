#!@PYTHON@
#
# convert-ly.py -- Update old LilyPond input files (fix name?)
#
# source file of the GNU LilyPond music typesetter
#
# (c) 1998--2006  Han-Wen Nienhuys <hanwen@cs.uu.nl>
#                 Jan Nieuwenhuizen <janneke@gnu.org>
#
# converting rules are found in python/convertrules.py
#

import os
import sys
import string
import re


datadir = '@local_lilypond_datadir@'
if not os.path.isdir (datadir):
	datadir = '@lilypond_datadir@'

sys.path.insert (0, os.path.join (datadir, 'python'))

if os.environ.has_key ('LILYPONDPREFIX'):
	datadir = os.environ['LILYPONDPREFIX']
	while datadir[-1] == os.sep:
		datadir= datadir[:-1]
		
	datadir = os.path.join (datadir, "share/lilypond/current/")
sys.path.insert (0, os.path.join (datadir, 'python'))

# dynamic relocation, for GUB binaries.
bindir = os.path.split (sys.argv[0])[0]
for p in ['share', 'lib']:
	datadir = os.path.abspath (bindir + '/../%s/lilypond/current/python/' % p)
	sys.path.insert (0, datadir)

import lilylib as ly
global _;_=ly._

from convertrules import *

lilypond_version_re_str = '\\\\version *\"([0-9.]+)"'
lilypond_version_re = re.compile (lilypond_version_re_str)


help_summary = _ (
'''Update LilyPond input to newer version.  By default, update from the
version taken from the \\version command, to the current LilyPond version.

Examples:

  convert-ly -e old.ly
  convert-ly --from=2.3.28 --to 2.5.21 foobar.ly
''')

copyright = ('Jan Nieuwenhuizen <janneke@gnu.org>',
	     'Han-Wen Nienhuys <hanwen@cs.uu.nl>')

program_name = os.path.basename (sys.argv[0])
program_version = '@TOPLEVEL_VERSION@'

add_version = 1

def warning (s):
	sys.stderr.write (program_name + ": " + _ ("warning: %s") % s + '\n')

def error (s):
	sys.stderr.write (program_name + ": " + _ ("error: %s") % s + '\n')

def identify (port=sys.stderr):
	port.write ('%s (GNU LilyPond) %s\n' % (program_name, program_version))

def warranty ():
	identify ()
	sys.stdout.write ('''
Copyright (c) %s by

  Han-Wen Nienhuys
  Jan Nieuwenhuizen

%s
%s
'''  ( '2001--2006',
       _('Distributed under terms of the GNU General Public License.'),
       _('It comes with NO WARRANTY.')))



def get_option_parser ():
	p = ly.get_option_parser (usage='convert-ly [OPTIONS] FILE',
				  version="@TOPLEVEL_VERSION@",
				  description=help_summary)

	p.add_option ('-f', '--from', 
		      action="store",
		      metavar=_ ("VERSION"),
		      dest="from_version",
		      help=_('start from VERSION [default: \\version found in file]'),
		      default='')
	
	p.add_option ('-e', '--edit', help=_('edit in place'),
		      action='store_true')
	p.add_option ('-n', '--no-version',
		      help=_ ('do not add \\version command if missing'),
		      action='store_true',
		      dest='skip_version_add',
		      default=False)
	
	p.add_option ("-s", '--show-rules',
		      help=_('print rules [default: --from=0, --to=@TOPLEVEL_VERSION@]'),
		      dest='show_rules',
		      action='store_true', default=False)
	
	p.add_option ('-t', '--to',
		      help=_('convert to VERSION [default: @TOPLEVEL_VERSION@]'),
		      metavar=_('VERSION'),
		      action='store',
		      dest="to_version",
		      default='')

	p.add_option_group  ('bugs',
			     description='''Report bugs via http://post.gmane.org/post.php'''
			     '''?group=gmane.comp.gnu.lilypond.bugs\n''')
	
	return p



def str_to_tuple (s):
	return tuple (map (int, string.split (s, '.')))

def tup_to_str (t):
	return string.join (map (lambda x: '%s' % x, list (t)), '.')

def version_cmp (t1, t2):
	for x in [0, 1, 2]:
		if t1[x] - t2[x]:
			return t1[x] - t2[x]
	return 0

def get_conversions (from_version, to_version):
	def is_applicable (v, f = from_version, t = to_version):
		return version_cmp (v[0], f) > 0 and version_cmp (v[0], t) <= 0
	return filter (is_applicable, conversions)

def latest_version ():
	return conversions[-1][0]

def show_rules (file, from_version, to_version):
	for x in conversions:
		if (not from_version or x[0] > from_version) \
		   and (not to_version or x[0] <= to_version):
			file.write  ('%s: %s\n' % (tup_to_str (x[0]), x[2]))

def do_conversion (str, from_version, to_version):
	"""Apply conversions from FROM_VERSION to TO_VERSION.  Return
tuple (LAST,STR), with the last succesful conversion and the resulting
string."""
	conv_list = get_conversions (from_version, to_version)

	if error_file:
		error_file.write (_ ("Applying conversion: "))
		
	last_conversion = ()
	try:
		for x in conv_list:
			error_file.write (tup_to_str (x[0]))
			if x != conv_list[-1]:
				error_file.write (', ')
			str = x[1] (str)
			last_conversion = x[0]

	except FatalConversionError:
		error_file.write (_ ("error while converting")) 
		error_file.write ('\n')
		error_file.write (_ ("Aborting"))
		error_file.write ('\n')



	return (last_conversion, str)



def guess_lilypond_version (filename):
	s = open (filename).read ()
	m = lilypond_version_re.search (s)
	if m:
		return m.group (1)
	else:
		return ''

class FatalConversionError:
	pass

class UnknownVersion:
	pass

def do_one_file (infile_name):
	sys.stderr.write (_ ("Processing `%s\'... ") % infile_name)
	sys.stderr.write ('\n')

	from_version = None
	to_version = None
	if global_options.from_version:
		from_version = global_options.from_version
	else:
		guess = guess_lilypond_version (infile_name)
		if not guess:
			raise UnknownVersion ()
		from_version = str_to_tuple (guess)

	if global_options.to_version:
		to_version = global_options.to_version
	else:
		to_version = latest_version ()


	if infile_name:
		infile = open (infile_name, 'r')
	else:
		infile = sys.stdin


	(last, result) = do_conversion (infile.read (), from_version, to_version)
	infile.close ()

	if last:
		newversion = r'\version "%s"' % tup_to_str (last)
		if lilypond_version_re.search (result):
			result = re.sub (lilypond_version_re_str,
					 '\\' + newversion, result)
		elif add_version:
			result = newversion + '\n' + result
			
		error_file.write ('\n')			
	
		if global_options.edit:
			try:
				os.remove(infile_name + '~')
			except:
				pass
			os.rename (infile_name, infile_name + '~')
			outfile = open (infile_name, 'w')
		else:
			outfile = sys.stdout


		outfile.write (result)

	sys.stderr.flush ()

def do_options ():
	opt_parser = get_option_parser()
	(options, args) = opt_parser.parse_args ()


	if options.from_version:
		options.from_version = str_to_tuple (options.from_version)
	if options.to_version:
		options.to_version = str_to_tuple (options.to_version)

	options.outfile_name = ''
	global global_options
	global_options = options

	if not args and not options.show_rules:
		opt_parser.print_help ()
		sys.exit (2)

	return args

def main ():
	files = do_options ()

	# should parse files[] to read \version?
	if global_options.show_rules:
		show_rules (sys.stdout, global_options.from_version, global_options.to_version)
		sys.exit (0)

	identify (sys.stderr)

	for f in files:
		if f == '-':
			f = ''
		elif not os.path.isfile (f):
			error (_ ("can't open file: `%s'") % f)
			if len (files) == 1:
				sys.exit (1)
			continue
		try:
			do_one_file (f)
		except UnknownVersion:
			error (_ ("can't determine version for `%s'. Skipping") % f)

	sys.stderr.write ('\n')

main ()
