#!/usr/bin/python

import sys
import re
import os
import smtplib
import email.MIMEText
import email.Message
import email.MIMEMultipart
import optparse
import time
import dbhash

sys.path.insert (0, 'lib/')

import repository


################################################################
# utils.

def system (c):
    print c
    if os.system (c):
        raise Exception ('barf')
    
    
def read_tail (file, amount=10240):
    f = open (file)
    f.seek (0, 2)
    length = f.tell()
    f.seek (- min (length, amount), 1)
    return f.read ()

def canonicalize_string (target):
    canonicalize = re.sub ('[ \t\n]', '_', target)
    canonicalize = re.sub ('[^a-zA-Z0-9-]+', '_', canonicalize)
    return canonicalize

class LogFile:
    def __init__ (self, name):
        if name:
            self.file = open (name, 'a')
        else:
            self.file = sys.stdout

        self.prefix = 'test-gub.py[%d]: ' % os.getpid ()

    def log (self, msg):
        self.file.write ('%s%s\n' % (self.prefix, msg))
        self.file.flush ()
        
    def __del__ (self):
        self.log (' *** finished')

log_file = None

################################################################
#

def result_message (parts, subject='') :
    """Concatenate PARTS to a Message object."""
    
    if not parts:
        parts.append ('(empty)')
    
    parts = [email.MIMEText.MIMEText (p) for p in parts if p]

    msg = parts[0]
    if len (parts) > 1:
        msg = email.MIMEMultipart.MIMEMultipart ()
        for p in parts:
            msg.attach (p)
    
    msg['Subject'] = subject
    msg.epilogue = ''

    return msg

def opt_parser ():
    if os.environ.has_key ('EMAIL'):
        address = os.environ['EMAIL']
    else:
        try:
            address = '%s@localhost' % os.getlogin ()
        except OSError:
            address = 'root@localhost'
    
    p = optparse.OptionParser (usage="test-gub.py [options] command command ... ")
    p.add_option ('-t', '--to',
                  action='append',
                  dest='address',
                  default=[],
                  help='where to send error report')
    p.add_option ('--dry-run',
                  dest='dry_run',
                  default=False,
                  action="store_true",
                  help="don't run any commands")
    
    p.add_option ('--bcc',
                  action='append',
                  dest='bcc_address',
                  default=[],
                  help='BCC for error report')

    p.add_option ('-f', '--from',
                  action='store',
                  dest='sender',
                  default=address,
                  help='whom to list as sender')

    p.add_option ('--branch',
                  action="store",
                  dest="branch",
                  default="HEAD",
                  help="which branch to test")
    
    p.add_option ('--repository',
                  action="store",
                  dest="repository",
                  default=".",
                  help="repository directory")
    
    p.add_option ('--tag-repo',
                  action='store',
                  dest='tag_repo',
                  default='',
                  help='where to push success tags.')

    p.add_option ('--quiet',
                  action='store_true',
                  dest='be_quiet',
                  default=False,
                  help='only send mail when there was an error.')
    
    p.add_option ('--dependent',
                  action="store_true",
                  dest="is_dependent",
                  default=False,
                  help="test targets depend on each other")
                  
    p.add_option ('--posthook',
                  action='append',
                  dest='posthooks',
                  default=[],
                  help='commands to execute after successful tests.')

    p.add_option ('--test-self',
                  action='store_true',
                  dest='test_self',
                  default=False,
                  help='run a cursory self test.')

    p.add_option ('-s', '--smtp',
                  action='store',
                  dest='smtp',
                  default='localhost',
                  help='SMTP server to use.')

    p.add_option ('--result-directory',
                  action="store",
                  dest='result_dir',
                  help="Where to store databases test results",
                  default="log")
                  
    return p


def get_db (options, name):
    name = options.result_dir + '/%s.db' % name

    db_file = os.path.join (options.result_dir, name)
    db = dbhash.open (db_file, 'c')
    return db
                        

def test_target (repo, options, target, last_patch):
    canonicalize = canonicalize_string (target)
    release_hash = repo.get_checksum ()

    done_db = get_db (options, canonicalize)
    if done_db.has_key (release_hash):
        log_file.log ('release %(release_hash)s has already been checked' % locals ())
        return None
    
    logfile = 'test-%(canonicalize)s.log' %  locals ()
    logfile = os.path.join (options.result_dir, logfile)
    
    cmd = "nice time %(target)s >& %(logfile)s" %  locals ()

    log_file.log (cmd)

    if options.dry_run:
        return ('SUCCESS', ['dryrun'])
        
    stat = os.system (cmd)
  
    result = 'unknown'
    attachments = []

    body = read_tail (logfile, 10240).split ('\n')
    if stat:
        result = 'FAIL'
        attachments = ['error for\n\n\t%s\n\n\n%s' % (target,
                                               '\n'.join (body[-0:]))]

    else:
        result = "SUCCESS"
        attachments = ['success for\n\n\t%s\n\n%s'
                       % (target,
                          '\n'.join (body[-10:]))]

    log_file.log ('%s: %s' % (target, result))
    
    done_db[release_hash] = time.ctime ()
    return (result, attachments)
    
def send_message (options, msg):
    if not options.address:
        log_file.log ('No recipients for result mail')
        return
    
    COMMASPACE = ', '
    msg['From'] = options.sender
    msg['To'] = COMMASPACE.join (options.address)
    if options.bcc_address:
        msg['BCC'] = COMMASPACE.join (options.bcc_address)
        
    msg['X-Autogenerated'] = 'lilypond'
    connection = smtplib.SMTP (options.smtp)
    connection.sendmail (options.sender, options.address, msg.as_string ())


def send_result_by_mail (options, parts, subject="Autotester result"):
    msg = result_message (parts, subject)
    send_message (options, msg)

def print_results (options, parts, subject="Autotester result"):
    print '\n===\n\nSUBJECT: ', subject
    print '\n---\n'.join (parts)
    print 'END RESULT\n==='

def real_main (options, args, handle_result):
    global log_file

    log = 'log/test-gub.log'
    if options.dry_run:
        log = ''

    log_file = LogFile (log)
    
    log_file.log (' *** %s' % time.ctime ())
    log_file.log (' *** Starting tests:\n  %s' % '\n  '.join (args))

    repo = repository.get_repository_proxy (options.repository, options.branch)
    log_file.log ("Repository %s" % str (repo))
    
    last_patch = repo.get_revision_description ()
    release_hash = repo.get_checksum ()

    release_id = '''

Last patch of this release:

%(last_patch)s\n

Checksum of revision: %(release_hash)s

''' % locals ()


    summary_body = '\n\n'
    results = {}
    failures = 0
    for a in args:
        result_tup = test_target (repo, options, a, last_patch)
        if not result_tup:
            continue

        (result, atts) = result_tup

        results[a] = result_tup
        
        success = result.startswith ('SUCCESS')
        if not (options.be_quiet and success):
            handle_result (options, atts, subject="Autotester: %s %s" % (result, a))

        summary_body += '%s\n  %s\n'  % (a, result)


        if not success:
            failures += 1
            if options.is_dependent:
                break

    if (results
        and len (args) > 1
        and (failures > 0 or not options.be_quiet)):
        
        handle_result (options,
                       [summary_body, release_id], subject="Autotester: summary")

    if failures == 0 and results:
        for p in options.posthooks:
            os.system (p)

def test_self (options, args):
    self_test_dir = 'test-gub-test.darcs'
    system ('rm -rf %s ' %  self_test_dir)
    system ('mkdir %s ' %  self_test_dir)
    os.chdir (self_test_dir)
    system ('mkdir log')
    system (r"echo -e '#!/bin/sh\ntrue\n' > foo.sh")
    system ('darcs init')
    system ('echo author > _darcs/prefs/author')
    system ('darcs add foo.sh')
    system ('darcs record -am "add bla"')
    options.repository = os.getcwd ()
    
    real_main (options, ['false', 'true', 'sh foo.sh'], print_results)

    system (r"echo -e '#!/bin/sh\nfalse\n' > foo.sh")
    system ('darcs record  -am "change bla"')
    real_main (options, ['sh foo.sh'], print_results)
    
def main ():
    (options, args) = opt_parser ().parse_args ()

    if not os.path.isdir (options.result_dir):
        os.makedirs (options.result_dir)

    options.result_dir = os.path.abspath (options.result_dir)
    
    if options.test_self:
        test_self (options, args)
    else:
        real_main (options, args, send_result_by_mail)
    


if __name__ == '__main__':    
#    test ()
    main ()