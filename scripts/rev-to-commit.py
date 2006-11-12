#! /usr/bin/python

def read_pipe (command):
    import os
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
    hunt = '''git grep -h "CVS Revision: [0-9]" %(revision)s -- %(file)s'''
    cvs_revision_string = read_pipe (hunt % locals ()).strip ()
    s = cvs_revision_string
    r = 0
    while s == cvs_revision_string:
        r += 1
        revision = 'HEAD~%d' % r
        s = read_pipe (hunt % locals ()).strip ()
    rev = 'HEAD~%d' % (r - 1)
    print "REVISION: ", revision
    commit_cmd = '''git log %(rev)s --max-count=1|grep ^commit'''
    print commit_cmd % locals ()
    commit = read_pipe (commit_cmd % locals ()).strip ()
    commit_string = commit.replace ('commit ', 'Translation of GIT commit: ')
    file_sub (file, cvs_revision_string, commit_string)
    print cvs_revision_string, '->', commit_string
