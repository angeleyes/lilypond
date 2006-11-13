#! /usr/bin/python

import re
import os

def read_pipe (command):
    import os
    print command
    pipe = os.popen (command)
    output = pipe.read ()
    if pipe.close ():
        print "pipe failed: %(command)s" % locals ()
        #raise 'SystemFailed'
    return output

def file_sub (file, frm, to):
    s = open (file).read ()
    open (file, 'w').write (s.replace (frm, to))
    
for file in read_pipe ('''git grep -l "CVS Revision: [0-9]"''').split ('\n'):
    revision = ''
    if not file or file.find ('big-page') >= 0:
        continue
    print file

    changes = read_pipe ("git-rev-list HEAD %(file)s" % locals ()).split ('\n')
    for c in changes:
        if not c:
            continue
        
        diff = read_pipe ('git-diff-tree -p %(c)s -- %(file)s' % locals ())
        m = re.search ('\n\\+.*Translation of (CVS Revision: [0-9.]+)', diff)
        if m:
            print  m.group (1) ,'->', c
            file_sub (file, m.group (1), 'GIT Committish: ' + c)
            break
