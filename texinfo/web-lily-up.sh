#!/bin/sh
rsync -avrz --delete \
  ~/src/web-gop/texinfo/out/ graham@lilypond.org:public_html/out/
rsync -avrz --delete \
  ~/src/web-gop/texinfo/pictures/ graham@lilypond.org:public_html/pictures/
rsync -avrz --delete \
  ~/src/web-gop/texinfo/examples/*.png graham@lilypond.org:public_html/examples/

