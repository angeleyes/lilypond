"GNU Texinfo and Texi2html support"

import re
import os
import sys

import pproc
import Utils, TaskGen, Task, Runner, Build, Options

import custom_configure

# find_makeinfo = functools.partial (find_program_with_version, program='makeinfo')
# test_texi2pdf = functools.partial (find_program_with_version, program='texi2pdf')
# find_texi2dvi = functools.partial (find_program_with_version, program='texi2dvi')
# find_texi2html = functools.partial (find_program_with_version, program='texi2html texi2html.pl')

def find_makeinfo (conf):
    return conf.find_program ('makeinfo', var='MAKEINFO')

texi2dvi_test = """\\input texinfo
@setfilename %s.info
@settitle Lily test

@node Top
@top Texinfo texi2pdf test with EPS graphics

@image{context-example}

@bye
"""

eps_input = os.path.join (Options.options.srcdir, 'Documentation',
                          'pictures', 'context-example.eps')
texinfo_tex_dir = os.path.join (Options.options.srcdir, 'tex')

def test_texi2pdf (conf):
    texi2pdf_path = conf.find_program ('texi2pdf', var='TEXI2PDF')
    ps2pdf_path = conf.find_program ('ps2pdf', mandatory=True)
    conf.check_message_1 ('Testing execution of %s' % 'texi2pdf')
    # clean everything first
    
    pdf_from_eps = os.path.join (custom_configure.outdir,
                                 os.path.splitext (os.path.basename (eps_input))[0] + '.pdf')
    testfile_basename = 'texi_test.texi'
    testfile_path = os.path.join (custom_configure.outdir, testfile_basename)
    try:
        os.remove (pdf_from_eps)
        for path in glob.glob (os.path.join
                               (custom_configure_outdir,
                                os.path.splitext (testfile_basename)[0] + '.*')):
            os.remove (path)
    except:
        pass
    if not os.path.exists (custom_configure.outdir):
        os.makedirs (custom_configure.outdir)
    eps2pdf = pproc.Popen ([ps2pdf_path, eps_input, pdf_from_eps],
                           stdout=sys.stdout, stderr=sys.stderr)
    eps2pdf.communicate ()
    exit_code = eps2pdf.wait ()
    if exit_code:
        conf.check_message_2 ('EPS to PDF conversion of image example failed', color='YELLOW')
        return None
    testfile = open (testfile_path, 'w')
    testfile.write (texi2dvi_test % os.path.splitext (testfile_basename)[0])
    testfile.close ()
    # Ugh, assume custom_configure_outdir is a relative path and subdirectory
    up_path_components = ['..'] * len (custom_configure.outdir.split (os.path.sep))
    texinfo_tex_relative_dir = os.path.join (*(up_path_components + [texinfo_tex_dir]))
    texi2pdf = pproc.Popen ([texi2pdf_path, '--batch', testfile_basename],
                            cwd=custom_configure.outdir,
                            env={'TEXINPUTS': texinfo_tex_relative_dir + '/::'},
                            stdout=conf.log, stderr=conf.log)
    texi2pdf.communicate ()
    exit_code = texi2pdf.wait ()
    if exit_code:
        conf.check_message_2 ('call on sample file %s failed' % testfile_path, color='YELLOW')
    else:
        conf.check_message_2 ('ok', color='GREEN')

def find_texi2html (conf):
    return conf.find_program ('texi2html')

def detect (conf):
    test_texi2pdf (conf)

# CHECKME: the following function is imported from tex Waf tool
texinfo_include_re = re.compile \
 (r'@(?P<type>(?:verbatim)?include)[ \t]+(?P<file>.+?)[ \t]*')
def scan (self):
    node = self.inputs[0]
    env = self.env

    nodes = []
    names = []
    if not node: return (nodes, names)

    code = Utils.readf(node.abspath(env))

    curdirnode = self.curdirnode
    abs = curdirnode.abspath()
    for match in texinfo_include_re.finditer(code):
        path = match.group('file')
        if path:
            for k in ['', '.texi', '.itexi']:
                # add another loop for the tex include paths?
                debug('texinfo: trying %s%s' % (path, k))
                try:
                    os.stat(abs+os.sep+path+k)
                except OSError:
                    continue
                found = path+k
                node = curdirnode.find_resource(found)
                if node:
                    nodes.append(node)
            else:
                debug('texinfo: could not find %s' % path)
                names.append(path)

    debug("texinfo: found the following : %s and names %s" % (nodes, names))
    return (nodes, names)
