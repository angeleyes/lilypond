# encoding: utf-8

import os
import re
import pproc

from Configure import conf

version_string_re = re.compile (r'([0-9]+)\.([0-9]+)|([0-9]{6,8})')
version_detection_failure = 'cannot determine'

def split_version_match (m):
    version_numbers_list = []
    for g in m.groups():
        try:
            version_numbers_list.append (int (g))
        except ValueError:
            pass
        except TypeError:
            pass
    return version_numbers_list

def get_program_version (conf, program_path,
                         required_version=None, recommended_version=None):
    program_name = os.path.basename (program_path)
    conf.check_message_1 ('Checking version of %s' % program_name)
    version_test = [program_path, '--version']
    try:
        process = pproc.Popen (version_test,
                               stdout=pproc.PIPE, stderr=pproc.PIPE)
        version_message = process.communicate()[0]
        if not version_message:
            alternate_version_test = [program_path, ' -V']
            process = pproc.Popen(alternate_version_test,
                                  stdout=pproc.PIPE,
                                  stderr=pproc.PIPE)
            version_message = process.communicate()[0]
            if not version_message:
                raise Exception
        m = version_string_re.search (version_message)
        if m is None:
            raise Exception
        version_tuple = split_version_match (m)
        detected_version_string = '.'.join (map (str, version_tuple))
        if not required_version is None:
            required_version_tuple = \
                split_version_match (version_string_re.match (required_version))
            if not recommended_version is None:
                recommended_version_tuple = \
                    split_version_match (version_string_re.match (recommended_version))
            if version_tuple < required_version_tuple:
                conf.miss_program (program_name, detected_version_string,
                                   '>=' + required_version, mandatory=True)
                return None
            elif recommended_version and version_tuple < recommended_version_tuple:
                conf.miss_program (program_name, detected_version_string,
                                   '>=' + recommended_version, mandatory=False)
                return version_tuple
            else:
                conf.check_message_2 ('ok ' + detected_version_string,
                                      color='GREEN')
                return version_tuple
    except:
            conf.check_message_2 (version_detection_failure, color='PINK')
            raise

@conf
def find_program_with_version (conf, program, required_version=None,
                               recommended_version=None, mandatory=True):
    program = conf.cmd_to_list (program)
    program_path = conf.find_program (program, var=program[0].upper())
    if program_path:
        version = get_program_version (conf, program_path,
                                       required_version, recommended_version)
    else:
        version = None
        if recommended_version or required_version:
            version_recommendation = '>= ' + (recommended_version or
                                              required_version)
        else:
            version_recommendation = ''
        conf.miss_program (conf.cmd_to_list (program[0], version,
                                             version_recommendation, mandatory))
    return (program, version)

@conf
def err_handler (conf, fun, error):
    """Generic error handler for a configuration context

This function searches an appropriate error handler
"""

@conf
def miss_program (conf, program, detected_version, requirement, mandatory):
    """TODO: add to missing or required programs lists, and
issue messages with conf.check_message_2 with appropriate color
"""
