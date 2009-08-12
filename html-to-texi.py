#! /usr/bin/python

import re
import sys

header = '''\\input texinfo @c -*- coding: utf-8; mode: texinfo; -*-
@ignore
    Translation of GIT committish: FILL-IN-HEAD-COMMITTISH

    When revising a translation, copy the HEAD committish of the
    version that you are working on.  See TRANSLATION for details.
@end ignore
'''

print header

body = sys.stdin.read ()
body = re.sub ('(?ms)<!--\s*\n*(.*?)FILL-IN(.*?)\s*\n*!?-->', '', body)
body = re.sub ('(?ms)\n*\s*<!--\s*\n*(.*?)\s*\n*!?-->', r'@ignore\n\1\n@end ignore', body)
body = re.sub ('(?ms)<a(?:\s|\n)*href="([^"]*)"\s*>(.*?)</a>', r'@ref{\1,\2}', body)

body = re.sub ('(?ms)<a(?:\s|\n)*name="([^"]*)"\s*> *</a>', r'@node \1 ', body)
body = re.sub ('(?ms)<h1>(.*?)</h1>', r'@chapheading \1', body)
body = re.sub ('(?ms)<h2>(.*?)</h2>', r'@unnumberedsec \1', body)
#body = re.sub ('(?ms)<a(?:\s|\n)*name="([^"]*)"\s*> *</a>', '', body)
#body = re.sub ('(?ms)<h2>(.*?)</h2>', r'@node \1\n@unnumberedsec \1', body)

body = re.sub ('(?ms)<img(?:\s|\n)*[^>]*src="*([^">]*)(.png|.jpeg)"*.*?>', r'@image{\1,,,\2}', body)
body = re.sub ('\n*<br>\s*\n*', r'@*\n', body)
body = re.sub ('\n*<br[^>]*>\s*\n*', r'@*\n', body)
body = re.sub ('(?ms)<em>(.*?)</em>', r'@emph{\1}', body)
body = re.sub ('(?ms)<blockquote>(.*?)</blockquote>', r'@quote{\1}', body)
body = re.sub ('(?ms)<tt>(.*?)</tt>', r'@code{\1}', body)
body = re.sub ('(?ms)<li>(.*?)(</li>)', r'@item\n\1\n', body)
body = re.sub ('(?ms)<li>(.*?)(<li>)', r'@item\n\1\n\2', body)
body = re.sub ('(?ms)<li>(.*?)(<li>)', r'@item\n\1\n\2', body)
body = re.sub ('(?ms)<li>(.*?)(<li>)', r'@item\n\1\n\2', body)
body = re.sub ('(?ms)<li>(.*?)(<li>)', r'@item\n\1\n\2', body)
body = re.sub ('(?ms)<li>(.*?)(<li>)', r'@item\n\1\n\2', body)
body = re.sub ('(?ms)<li>(.*?)(</(?:u|o)l>)', r'@item\n\1\n\2', body)
body = re.sub ('(?ms)<ul>', r'@itemize', body)
body = re.sub ('(?ms)</ul>', r'@end itemize', body)

body = re.sub ('(?ms)<ol>', r'@enumerate POSITIVE-INTEGER', body)
body = re.sub ('(?ms)<ol\s+start="*(.*?)"*>', r'@enumerate \1', body)
body = re.sub ('(?ms)</ol>', r'@end enumerate', body)

body = re.sub ('&ldquo;(.*?)&rdquo;', r'@qq{\1}', body)
body = re.sub ('&auml;', '@"a', body)
body = re.sub ('(?ms)\s*<p>\s*', '\n\n', body)
body = re.sub ('\n*\s*</p>\s*\n*', '\n\n', body)
body = re.sub ('(?ms)</?font\s*.*?>', '', body)
body = re.sub ('(?ms)<code>(.*?)</code>', r'@code{\1}', body)
body = re.sub ('(?ms)<em>(.*?)</em>', r'@emph{\1}', body)
body = re.sub ('(?ms)<b>(.*?)</b>', r'@strong{\1}', body)
body = re.sub ('(?ms)<pre>(.*?)</pre>', r'@verbatim\n\1\n@end verbatim', body)
body = re.sub ('(?ms)<PRE>(.*?)</PRE>', r'@verbatim\n\1\n@end verbatim', body)

body = re.sub ('(?ms)<table>', r'@table asis', body)
body = re.sub ('(?ms)<table [^>]*>', r'@table asis', body)
body = re.sub ('(?ms)</table>', r'@end table', body)
body = re.sub ('(?ms)<tr>(.*?)</tr>', r'@item\n\1\n', body)
body = re.sub ('(?ms)<th>(.*?)</th>', r'@item\n\1\n', body)
body = re.sub ('(?ms)<td[^>]*>(.*?)</td>', r'@tab\n\1\n', body)

#body = re.sub ('(?ms)\s*<p align="(center|left|right)">\s*((?:.|\n)*)\n\n', '@divClass{float-\\1}\n\\2\n@divEnd', body)
body = re.sub ('(?ms)\s*<p align="(center|left|right)"\s*>\s*', '\n\n@divClass{float-\\1}\n@divEnd\n', body)

# FIXME: fix this, once we have the substitution stuff done
#  more such rules are needed
body = re.sub ('@DOC@', r'@@DOC@@', body)
body = re.sub ('@IMAGES@/*', r'pictures/', body)

body += '\n@bye\n'

print body

