#! /usr/bin/python

import re

filename = 'to-texi.html'
old = file (filename).read ()

old = re.sub ('(?ms)<a(?:\s|\n)*href="([^"]*)"\s*>(.*?)</a>', r'@uref{\1,\2}', old)
old = re.sub ('(?ms)<img(?:\s|\n)*src="([^"]*)".*?>', r'@image{\1,,,png}', old)
old = re.sub ('\n*<br>\s*\n*', r'@*\n', old)
old = re.sub ('(?ms)<em>(.*?)</em>', r'@emph{\1}', old)
old = re.sub ('(?ms)<blockquote>(.*?)</blockquote>', r'@quote{\1}', old)
old = re.sub ('(?ms)<tt>(.*?)</tt>', r'@code{\1}', old)
old = re.sub ('(?ms)<ul>', r'@itemize', old)
old = re.sub ('(?ms)</ul>', r'@end itemize', old)
old = re.sub ('(?ms)<li>(.*?)</li>', r'@item\n\1\n', old)
old = re.sub ('&ldquo;(.*?)&rdquo;', r'@qq{\1}', old)
old = re.sub ('\n*\s*<p>\s*\n*', r'\n', old)
old = re.sub ('\n*\s*</p>\s*\n*', r'\n', old)
old = re.sub ('\n*\s*<!--\s*\n*(.*?)\s*\n*!?-->', r'@ignore\n\1\n@end ignore', old)
old = re.sub ('(?ms)</?font\s*.*?>', '', old)

# FIXME: fix this, once we have the substitution stuff done
#  more such rules are needed
old = re.sub ('@DOC@', r'@@DOC@@', old)

dl = re.search ('(?ms)<dl>(.*?)</dl>', old).group (1)
items = re.findall ('(?ms)^\s*<dt>.*?</dd>', dl)



texi = ''
for i in items:
    try:
        header = re.search ('(?ms)<dt>(.*)</dt>', i).group (1)
    except:
        print 'BARF: no </DT>, please fix your html'
        print i
        barf
    header = re.sub ('(?sm)^(\s|\n)*', '', header)
    header = re.sub ('</?b>', '', header)
    header = re.sub ('</?em>', '', header)
    header = re.sub ('[.]?\s*-\s*\n\s*', '.  ', header)

    body = re.search ('(?ms)<dd>(.*)</dd>', i).group (1)
    body = re.sub ('(?sm)^(\s|\n)*', '', body)
    body = re.sub ('(?ms)<code>(.*?)</code>', r'@code{\1}', body)
    body = re.sub ('(?ms)<em>(.*?)</em>', r'@emph{\1}', body)
    body = re.sub ('(?ms)<b>(.*?)</b>', r'@strong{\1}', body)
    body = re.sub ('(?ms)<pre>(.*?)</pre>', r'@verbatim\n\1\n@end verbatim', body)

    item =  '\n@newsItem\n@subheading %(header)s\n%(body)s\n@newsEnd\n' % locals ()
    header = '<empty>'
    print item
    texi += item

#print texi

