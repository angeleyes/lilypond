#!/usr/bin/python

import __main__
import gettext
import getopt
import os
import re
import sys

LANGUAGES = (
	'nl',
	)

outdir = '/tmp'
verbose = 0
localedir = 'out/locale'
remove_quotes = 0

try:
	import gettext
	gettext.bindtextdomain ('newweb', localedir)
	gettext.textdomain ('newweb')
	_ = gettext.gettext
except:
	def _ (s):
		return s
underscore = _

def file_lang (file, lang):
	(base, ext) = os.path.splitext (file)
	base = os.path.splitext (base)[0]
	if lang and lang != 'site':
		return base + '.' + lang + ext
	return base + ext


def translate (s, lang):
	def grab_gettext (match):
		s = match.group (1)
		s = re.sub (r'''\\([\'"\\])''', r'''\1''', s)
		return gettext.gettext (s)

	# FIXME: simplistic REs
	if remove_quotes:
		s = re.sub (r'''_ *\("(([^"]|([^\\]\\"))*)"\)''', grab_gettext, s)
	else:
		s = re.sub (r'''_ *\(("([^"]|([^\\]\\"))*")\)''', grab_gettext, s)
	return s


def do_file (file_name):
	if verbose:
		sys.stderr.write ('%s...\n' % file_name)
	lang = os.environ['LANG']
	if not lang in LANGUAGES:
		raise 'no such language: ' + lang

	##out_file_name = file_lang (file_name, lang)
	out_file_name = os.path.basename (file_name)
	out_file_name = os.path.join (outdir, out_file_name)

	if file_name == out_file_name:
		raise 'cowardly resisting to overwrite source: ' + file_name

	s = open (file_name).read ()
	t = translate (s, lang)
	open (out_file_name, 'w').write (t)


def do_options ():
	global outdir, remove_quotes, verbose
	(options, files) = getopt.getopt (sys.argv[1:], '',
					  ['help', 'outdir=', 'remove-quotes',
					   'verbose'])
	for (o, a) in options:
		if o == '--help':
			sys.stdout.write (r'''
Usage:
translate --outdir=DIRECTORY FOO

This script is licensed under the GNU GPL
''')
		elif o == '--outdir':
			outdir = a
		elif o == '--remove-quotes':
			remove_quotes = 1
		elif o == '--verbose':
			verbose = 1
		else:
			assert unimplemented
	return files


def main ():
	files = do_options ()
	for i in files:
		do_file (i)

if __name__ == '__main__':
	main ()

