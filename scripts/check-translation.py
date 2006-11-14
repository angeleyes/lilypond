#!/usr/bin/python

import __main__
import getopt
import gettext
import os
import re
import string
import sys

# The directory to hold the translated and menuified tree.
C = 'site'
lang = C
verbose = 0

LANGUAGES = (
    (C, 'English'),
    ('nl', 'Nederlands'),
    )

def dir_lang (file, lang):
    return string.join ([lang] + string.split (file, '/')[1:], '/')

##     Translation of GIT Commit: <hash>
REVISION_RE = re.compile ('.*GIT Committish: ([a-f0-9]+)', re.DOTALL)
CVS_DIFF = 'git diff %(revision)s HEAD -- %(original)s | cat'

def check_file (original, translated):
    s = open (translated).read ()
    m = REVISION_RE.match (s)
    if not m:
        raise translated + ': no GIT commit: <hash> found'
    revision = m.group (1)

    c = CVS_DIFF % vars ()
    if verbose:
        os.stderr.write ('running: ' + c)
    os.system (c)

def do_file (file_name):
    if verbose:
        sys.stderr.write ('%s...\n' % file_name)
    file_lang = string.split (file_name, '/')[0]
    if file_lang != C:
        check_lang = file_lang
    else:
        check_lang = lang
    if check_lang == C:
        raise 'cannot determine language for: ' + file_name
    
    original = dir_lang (file_name, C)
    translated = dir_lang (file_name, check_lang)
    check_file (original, translated)

def usage ():
    sys.stdout.write (r'''
Usage:
check-translation [--language=LANG] [--verbose] FILE...

This script is licensed under the GNU GPL.
''')
            
def do_options ():
    global lang, verbose
    (options, files) = getopt.getopt (sys.argv[1:], '',
                                      ['help', 'language=', 'verbose'])
    for (o, a) in options:
        if 0:
            pass
        elif o == '--help':
            usage ()
        elif o == '--language':
            lang = a
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

