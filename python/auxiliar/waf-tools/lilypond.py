"LilyPond executables support"

import os
import pproc

def check_file_from_autoconf (path, env):
    return (os.path.isfile (os.path.join (env['top-build-dir'], path)) or
            os.path.isfile (os.path.join (env['top-src-dir'], path)))

def find_lilypond (conf):
    conf.check_message_1 ('Checking for LilyPond binary')
    env_lilypond_binary = os.getenv ('LILYPOND_EXTERNAL_BINARY')
    if not env_lilypond_binary or not os.access (env_lilypond_binary, os.R_OK | os.X_OK):
        if (check_file_from_autoconf ('GNUmakefile', conf.env) and
            check_file_from_autoconf ('config.make', conf.env)):
            config_log_path = os.path.join (conf.env['top-src-dir'], 'config.log')
            config_log_path = (os.path.isfile (config_log_path) and config_log_path or
                               os.path.join (conf.env['top-build-dir'], 'config.log'))
            config_log_last_line = pproc.Popen(['tail', '-1', config_log_path],
                                    stdout=pproc.PIPE).communicate()[0]
            if config_log_last_line.startswith ('configure: exit 0'):
                env_lilypond_binary = os.path.join (conf.env['top-build-dir'],
                                                    'bin', 'lilypond --relocate')
    if env_lilypond_binary:
        conf.env['LILYPOND_BINARY'] = env_lilypond_binary
        conf.check_message_2 ('ok '+ env_lilypond_binary, color='GREEN')
    else:
        conf.check_message_2 ('not found, trying standard check', color='YELLOW')
        env_lilypond_binary = conf.find_program ('lilypond', mandatory=True)

def detect (conf):
    find_lilypond (conf)
