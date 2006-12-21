#!/bin/sh

# startet gvim mit englischem Originaltext und deutscher Kopie zum Uebersetzen
# 1. Parameter sollte englischer Originaltitel sein

# entferne ../site/ 
de=${1/..\/site\//}
original=$1
echo $original nach $de
[ -e $de ]  || cp $original $de
gvim -c ":imap <M-p> LilyPond" -c "set scrollbind" -c "vsplit" -c "bn" -c  "set scrollbind" -c "2s=\\$\\(Revision.*\\)\\$=\1" $original $de
