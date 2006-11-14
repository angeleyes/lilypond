#!/usr/bin/python

import __main__
import optparse
import gettext
import os
import re
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
    return '/'.join ([lang] + file.split ('/')[1:])

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
        sys.stderr.write ('running: ' + c)
    os.system (c)

def do_file (file_name):
    if verbose:
        sys.stderr.write ('%s...\n' % file_name)
    file_lang = file_name.split ('/')[0]
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

    p = optparse.OptionParser (usage="check-translation [--language=LANG] [--verbose] FILE...",
                               description="This script is licensed under the GNU GPL.")
    p.add_option ("--language",
                  action='store',
                  default='site',
                  dest="language")
    p.add_option ("--verbose",
                  action='store_true',
                  default=False,
                  dest="verbose",
                  help="the GIT directory to merge.")
                               

    (options, files) = p.parse_args ()
    verbose = options.verbose
    lang = options.language
    
    return files

def main ():
    files = do_options ()

    for i in files:
        do_file (i)

if __name__ == '__main__':
    main ()

