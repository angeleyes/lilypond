#!/usr/bin/python

import __main__
import getopt
import gettext
import os
import re
import sys
import versiondb
import safeeval

# The directory to hold the translated and menuified tree.
outdir = '/var/www'
verbose = 0

C = 'site'
LANGUAGES = safeeval.eval_file ('scripts/languages.py')

localedir = 'out/locale'
try:
    import gettext
    gettext.bindtextdomain ('newweb', localedir)
    gettext.textdomain ('newweb')
    _ = gettext.gettext
except:
    def _ (s):
        return s
underscore = _


PAGE_TEMPLATE = '''@MAIN@'''

# ugh, move to .ithml
top_script = '''\
<SCRIPT type="text/javascript">
 <!--
  function setfocus ()
   {
    document.f_lily_google.brute_query.focus ();
    return true;
   }
  // !-->
 </SCRIPT>
'''

MENU_TEMPLATE = '''\
<TABLE cellpadding="0" cellspacing="0">
 <TR>%s</TR>
</TABLE>
'''

MENU_SEP = '''\
<TABLE width="100%" cellpadding="1" cellspacing="0">
 <TR>
  <TD class="menuactive"></TD>
 </TR>
</TABLE>
<TABLE width="100%" cellpadding="1" cellspacing="0">
 <TR>
  <TD></TD>
 </TR>
</TABLE>
'''

MENU_ITEM = '''\
<TD class="%%(css)sleftedge" width="1"></TD>
<TD class="%%(css)s">%s</TD>
<TD class="%%(css)srightedge" width="1"></TD>
'''
MENU_ITEM_INACTIVE = MENU_ITEM \
          % '<A class="%(css)s" href="%(url)s">%(name)s</A>'
MENU_ITEM_ACTIVE = MENU_ITEM \
         % '<A class="%(css)s" href="%(url)s">[%(name)s]</A>'

LOCATION_ITEM = '<a href="%(url)s">%(name)s</a>'
LOCATION_SEP = ' &gt; '

LOCATION_TITLE = 'LilyPond - %s'

language_available = _ ("Other languages: %s.") % "%(language_menu)s"
browser_language = _ ("About <A HREF='%s'>automatic language selection</A>.") \
           % "%(root_url)sabout/browser-language"

LANGUAGES_TEMPLATE = '''\
<P>
 %(language_available)s
 <BR>
 %(browser_language)s
</P>
''' % vars ()



version_db_file_name = 'lilypond.versions'
version_builds={}
branches = [(2,11), (2,10), (2,9), (2,8), (2,7), (2,6)]

def read_build_versions (name):
    version_db = versiondb.VersionDataBase (name)

    for branch in branches:
        branch_str = 'v' + '.'.join (['%d' % vc for vc in branch])
    
        for p in version_db.platforms ():
            (v, b, url) = version_db.get_last_release (p, branch)
            v = '.'.join (['%d' % vc for vc in v])
            version_builds[branch_str + '-' + p] = ('%s-%d' % (v,b), url)

        (version, b, url) =  version_db.get_last_release ('source', branch)
        version_builds[branch_str + '-source'] = ('.'.join (['%d' % vc for vc in version]),
                                                  url)

def dir_lang (file, lang):
    return '/'.join ([lang] + file.split ('/')[1:])


def file_lang (file, lang):
    (base, ext) = os.path.splitext (file)
    base = os.path.splitext (base)[0]
    if lang and lang != C:
        return base + '.' + lang + ext
    return base + ext




def format_page (html, file_name, lang):
    class dir_entry:
        def __init__ (self, dir, up):
            self.name = dir
            self.up = up

    class item:
        def __init__ (self, dir_entry, menu_entry):
            URL = 0
            NAME = 1
            self.is_active = is_active (dir_entry, menu_entry[URL])
            self.up = dir_entry.up
            self.url = menu_entry[URL]
            self.name = menu_entry[NAME]

    def list_directories (dir, up):
        '''Return list of DIR-ENTRYs for DIR and all parent DIRs.'''
        if not dir:
            return []
        return [dir_entry (dir, up)] \
           + list_directories (os.path.split (dir)[0], up + 1)

    def is_active (dir_entry, url):
        '''Return if DIR_ENTRY is used in URL.'''
        return (url == dir_split[dir_entry.up - 1] \
                or os.path.join (dir_entry.name, url) == file_name)

    def get_menu (dir):
        '''Return menu list for DIR.'''
        f = os.path.join (dir_lang (dir, C), 'menu-entries.py')

        if os.path.isfile (f):
            def strip_gettext_call (label):
                if type (label) == type (()):
                    label = label[2][0]

                assert type(label) == type ('')
                return label
            
            menu = [(name, _ (strip_gettext_call (label))) for (name, label) in safeeval.eval_file (f)]
        else:
            menu = [('', os.path.splitext (dir)[0]),]
        return menu

    def directory_menu (dir_entry):
        return map (lambda x: item (dir_entry, x),
              get_menu (dir_entry.name))

    def location_menu (dir_entry):
        active = filter (lambda x: is_active (dir_entry, x[0]),
                get_menu (dir_entry.name))
        return map (lambda x: item (dir_entry, x), active)

    def itemize (item, css, ACTIVE, INACTIVE):
        is_active = item.is_active
        name = item.name
        url = '../' * item.up + item.url
        if item.is_active:
            css += 'active'
            s = ACTIVE
        else:
            s = INACTIVE
        return s % vars ()

    def menuitemize (item):
        return itemize (item, 'menu',
                MENU_ITEM_ACTIVE, MENU_ITEM_INACTIVE)

    def locationize (item):
        return itemize (item, 'location',
                LOCATION_ITEM, LOCATION_ITEM)

    def grab_title (match):
        titles.append (match.group (1))
        return ''

    def grab_ihtml (match):
        s = match.group (1)
        for d in (dir, dir_lang (dir, lang), dir_lang (dir, C), lang, C):
            n = os.path.join (d, s)
            if os.path.exists (n):
                return open (n).read ()
        return match.group (0)

    def grab_gettext (match):
        return gettext.gettext (match.group (1))

    def grab_gettext1 (match):
        s = gettext.gettext (match.group (1))
        return s % match.group (2)
    def grab_gettext2 (match):
        s = gettext.gettext (match.group (1))
        return s % (match.group (3), match.group (4))
    def grab_gettext3 (match):
        s = gettext.gettext (match.group (1))
        return s % (match.group (3), match.group (4), match.group (5))
    def grab_gettext4 (match):
        s = gettext.gettext (match.group (1))
        return s % (match.group (3), match.group (4),
              match.group (5), match.group (6))

    
    dir = os.path.dirname (file_name)
    dir_split = [x for x in reversed (dir.split ('/'))]
    base_name = os.path.basename (file_name)
    is_index = base_name == 'index.html'
    directories = list_directories (dir, 0)
    depth = len (directories)

    # Make menu.
    listings = map (directory_menu, directories)
    menus = map (lambda x: map (menuitemize, x), listings)
    menu_strings = map (lambda x: MENU_TEMPLATE % ' '.join (x),
              menus)
    menu = MENU_SEP.join (reversed (menu_strings))


    # Make location.
    locations = filter (lambda x: x, map (location_menu, directories))
    locations = map (lambda x: x[0], reversed (locations))

    is_root = is_index and depth == 1
    if is_root:
        locations = []
    else:
        home = item (dir_entry (lang, depth - 1), ('', _ ("Home")))
        locations = [home] + locations

    location = LOCATION_SEP.join (map (locationize, locations))
    location_title = LOCATION_TITLE % ' - '.join (map (lambda x: x.name,
                                                       locations[1:]))
                                           

    # Ugh: title and script hacks.
    titles = [location_title]
    root_url = '../' * (depth - 1)
    main = re.sub ('<title>(.*?)</title>', grab_title, html)

    script = ''
    onload = ''
    if is_root:
        script = top_script
        # Ugh: search box is moved to bottom,
        #      which makes browser scroll down upon focussing it.
        #onload = "setfocus ();"

    # Find available translations of this page.
    rel_name = '/'.join (file_name.split ('/')[1:])
    available = filter (lambda x: lang != x[0] \
              and os.path.exists (os.path.join (x[0], rel_name)),
              LANGUAGES)

    # Create language menu.
    language_menu = ''
    for (prefix, name) in available:
        lang_file = file_lang (base_name, prefix)
        if language_menu != '':
            language_menu += ', '
        language_menu += '<a href="%(lang_file)s">%(name)s</a>' % vars ()

    languages = ''
    if language_menu:
        languages = LANGUAGES_TEMPLATE % vars ()

    page_template = PAGE_TEMPLATE
    f = os.path.join (dir_lang (dir, C), 'template.ihtml')
    if dir != lang and os.path.isfile (f):
        page_template = open (f).read ()

    if lang != C:
        slang = lang
        dot_lang = '.' + lang
        iso_lang = lang
    else:
        slang = ''
        dot_lang = ''
        iso_lang = 'en'

    # Do @AT@ substitution.

    # Ugh: factor 2 slowdown
    # page = re.sub ('@MAIN@', main, page_template)
    i = page_template.index ('@MAIN@')
    page = page_template[:i] + main + page_template[i+6:]

    page = re.sub ('@([-A-Za-z0-9.]*.ihtml)@', grab_ihtml, page)
    page = re.sub ('@LOCATION@', location, page)
    page = re.sub ('@MENU@', menu, page)
    page = re.sub ('@SCRIPT@', script, page)
    page = re.sub ('@TITLE@', titles[-1], page)
    page = re.sub ('@FILE_NAME@', file_name, page)
    page = re.sub ('@ONLOAD@', onload, page)
    page = re.sub ('@ISO_LANG@', iso_lang, page)
    page = re.sub ('@LANG@', slang, page)
    page = re.sub ('@.LANG@', dot_lang, page)

    page = re.sub ('@DEPTH@', root_url, page)
    page = re.sub ('@DOC@', os.path.join (root_url, '../doc/'), page)
    page = re.sub ('@IMAGES@', os.path.join (root_url, 'images/'), page)
    page = re.sub ('_ *\("([^"]*)"\)', grab_gettext, page)
    page = re.sub ('_ *\("([^"]*%s[^"]*)",\s*"([^"]*)"\)',
           grab_gettext1, page)
    page = re.sub ('_ *\("(([^"]*%s[^"]*){2})",\s*"([^"]*)",\s*"([^"]*)"\)',
           grab_gettext2, page)
    page = re.sub ('_ *\("(([^"]*%s[^"]*){3})",\s*"([^"]*)",\s*"([^"]*)",\s*"([^"]*)"\)',
           grab_gettext3, page)
    page = re.sub ('_ *\("(([^"]*%s[^"]*){4})",\s*"([^"]*)",\s*"([^"]*)",\s*"([^"]*)",\s*"([^"]*)"\)',
           grab_gettext4, page)
    page = re.sub ('__\(', '_ (', page)
    page = re.sub ('\$\Date: (.*) \$', '\\1', page)

    # Strip .html, .png suffix for auto language selection (content
    # negotiation).
    page = re.sub ('''(href|src)=[\'"]([^/][.]*[^.:\'"]*)(.html|.png)(#[^"\']*|)[\'"]''',
                   '\\1="\\2\\4"', page)

    # Add menu after stripping: must not have autoselection for language menu.
    page = re.sub ('@LANGUAGE_MENU@', languages, page)
    
    page = re.sub ('@LANGUAGE_MENU@', '', page)
    
    for (p, (v, url)) in version_builds.items():
        page  = re.sub ('@' + p + '-VERSION@', v, page)
        page  = re.sub ('@' + p + '-URL@', url, page)
    
    return page


def do_file (file_name):
    if verbose:
        sys.stderr.write ('%s...\n' % file_name)
    lang = file_name.split ('/')[0]
    if lang == C:
        out_file_name = file_name
    else:
        out_file_name = file_lang (dir_lang (file_name, C), lang)
    out_file_name = os.path.join (outdir, out_file_name)

    if file_name == out_file_name:
        raise 'cowardly resisting to overwrite source: ' + file_name

    html = open (file_name).read ()
    page = format_page (html, file_name, lang)
    open (out_file_name, 'w').write (page)


def do_options ():
    global outdir, verbose
    
    (options, files) = getopt.getopt (sys.argv[1:], '',
                                      ['outdir=', 'help',
                                       'version-db=', 'verbose'])
    for (o, a) in options:
        if o == '--outdir':
            outdir = a
        elif o == '--verbose':
            verbose = 1
        elif o == '--version-db':
            version_db_file_name = a
        elif o == '--help':
            sys.stdout.write (r'''
Usage:
format-page --outdir=DIRECTORY [--verbose]

This script is licensed under the GNU GPL
''')
        else:
            assert unimplemented
    return files


def main ():
    files = do_options ()
    read_build_versions (version_db_file_name)
    global PAGE_TEMPLATE
    f = os.path.join (dir_lang ('', C), 'template.ihtml')
    if os.path.isfile (f):
        PAGE_TEMPLATE = open (f).read ()

    for i in files:
        do_file (i)

if __name__ == '__main__':
    main ()
