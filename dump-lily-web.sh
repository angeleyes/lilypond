#!/bin/sh

if test ! -f newweb.css; then
    echo "need to be in newweb dir" 
  exit 2
fi

cd out/site
rsync --delete --stats --progress -lavu -e ssh --exclude '*~' . x:/var/www/lilypond/newweb/
