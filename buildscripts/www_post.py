#!@PYTHON@

## This is www_post.py. This script is the main stage
## of toplevel GNUmakefile local-WWW-post target.

# USAGE: www_post PACKAGE_NAME TOPLEVEL_VERSION BUILDSCRIPT-DIR OUTDIR TARGETS
# please call me from top of the source directory

import sys
import os
import re
import gettext

package_name, package_version, buildscript_dir, outdir, targets = sys.argv[1:]
targets = targets.split (' ')
outdir = os.path.normpath (outdir)
doc_dirs = ['input', 'Documentation', outdir]
target_pattern = os.path.join (outdir, '%s-root')

static_files = {
    # ugly hack: the following overwrites HTML Info dir with a link to
    # the (more useful) documentation index
    os.path.join ('Documentation/user', outdir, 'index.html'):
    '''<META HTTP-EQUIV="refresh" content="0;URL=../index.html">
<html><body>Redirecting to the documentation index...</body></html>\n''',
    os.path.join (outdir, 'index.html'):
    '''<META HTTP-EQUIV="refresh" content="0;URL=Documentation/index.html">
<html><body>Redirecting to the documentation index...</body></html>\n''',
    os.path.join (outdir, 'VERSION'):
    package_version + '\n' }

for f, contents in static_files.items ():
    open (f, 'w').write (contents)


sys.path.append (buildscript_dir)
import mirrortree
import add_html_footer
import langdefs

sys.stderr.write ("Mirrorring...\n")
dirs, symlinks, files = mirrortree.walk_tree (
    tree_roots = doc_dirs,
    process_dirs = outdir,
    exclude_dirs = '(' + '|'.join ([l.code for l in langdefs.LANGUAGES]) + r'|po|out|\w*?-root)(/|$)',
    find_files = r'.*?\.(?:midi|html|pdf|png|txt|ly|signature)$|VERSION',
    exclude_files = r'lily-[0-9a-f]+.*\.pdf')

# actual mirrorring stuff
html_files = []
hardlinked_files = []
for f in files:
    if f.endswith ('.html'):
        html_files.append (f)
    else:
        hardlinked_files.append (f)
dirs = [re.sub ('/' + outdir, '', d) for d in dirs]
while outdir in dirs:
    dirs.remove (outdir)
dirs = list( set (dirs))
dirs.sort ()

strip_file_name = {}
strip_re = re.compile (outdir + '/')
for t in targets:
    out_root = target_pattern % t
    strip_file_name[t] = lambda s: os.path.join (target_pattern % t, (strip_re.sub ('', s)))
    os.mkdir (out_root)
    map (os.mkdir, [os.path.join (out_root, d) for d in dirs])
    for f in hardlinked_files:
        os.link (f, strip_file_name[t] (f))
    for l in symlinks:
        p = mirrortree.new_link_path (os.path.normpath (os.readlink (l)), os.path.dirname (l), strip_re)
        os.symlink (p, strip_file_name[t] (l))

# need this for content negotiation with documentation index
if 'online' in targets:
    f = open (os.path.join (target_pattern % 'online', 'Documentation/.htaccess'), 'w')
    f.write ('#.htaccess\nDirectoryIndex index\n')
    f.close ()

# load gettext messages catalogs
translation = {}
localedir = os.path.join (buildscript_dir, '../Documentation/po', outdir)
for l in langdefs.LANGUAGES:
    if l.enabled and l.code != 'en':
        try:
            translation[l.code] = gettext.translation('lilypond-doc', localedir, [l.code]).gettext
        except:
            translation[l.code] = lambda s: s

add_html_footer.build_pages_dict (html_files)
for t in targets:
    sys.stderr.write ("Processing HTML pages for %s target...\n" % t)
    add_html_footer.add_html_footer (
        translation = translation,
        package_name = package_name,
        package_version = package_version,
        target = t,
        name_filter = strip_file_name[t])

