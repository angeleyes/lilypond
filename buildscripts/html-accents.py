#!@PYTHON@

# html-accents.py -- convert (some) latin1 chars to html
# pod2html is so broken...
# 
# source file of the GNU LilyPond music typesetter
# 
# (c) 1998 Jan Nieuwenhuizen <janneke@gnu.org>

name = 'html-accents'
version = '0.1'

import os
import sys
sys.path.append ('@abs-step-bindir@')
sys.path.append (os.environ['HOME'] + '/usr/src/lilypond/stepmake/bin')

import getopt
from string import *
import regex
import regsub
import time

def program_id ():
    return name + ' version ' + version;

def identify ():
    sys.stdout.write (program_id () + '\n')

def help ():
    sys.stdout.write ("Usage: " + name + " [options] INFILE OUTFILE\n"
		 + "Convert (some) latin1 chars to html &xxx;\n\n"
		 + "Options:\n"
		 + "  -h, --help             print this help\n"
		 + "  -p, --package=DIR      specify package\n"
		      )
    sys.exit (0)

# chars = {'�':'&egrave;', }
chars = { 
'�':'&aacute;',
'�':'&acirc;', 
'�':'&aelig;',
'�':'&agrave;', 
'�':'&aring;', 
'�':'&atilde;',
'�':'&auml;',

'�':'&ccedil;',

'�':'&eacute;', 
'�':'&ecirc;',
'�':'&egrave;', 
'�':'&euml;',

'�':'&iacute;',
'�':'&icirc;',
'�':'&igrave;', 
'�':'&iuml;',

'�':'&ntilde;',

'�':'&oacute;',
'�':'&ocirc;',
'�':'&ograve;', 
'�':'&oslash;',
'�':'&otilde;',
'�':'&ouml;',

'�':'&uacute;',
'�':'&ucirc;',
'�':'&ugrave;', 
'�':'&uuml;' 
}

def convert_accents (inname, outname):
    from flower import *
    text = File (inname)
    # ugh
    html = File (outname, 'w')

    while not text.eof ():
	line = text.readline ()
	for i in chars.keys ():
	    line = regsub.gsub (i, chars[i], line)
	html.write (line)
    text.close ()
    html.close ()

def main ():
    identify ()
    (options, files) = getopt.getopt (
	sys.argv[1:], 'hp:', ['help', 'package='])
    for opt in options:
	o = opt[0]
	a = opt[1]
	if o== '--help' or o == '-h':
	    help ()
	elif o == '-p' or o == '--package':
	    topdir = a
	else:
	    print o
	    raise getopt.error

    sys.path.append (topdir + '/stepmake/bin')
    from packagepython import *
    package = Package (topdir)
    packager = Packager ()

    from flower import *

    convert_accents (files[0], files[1])

main ()

