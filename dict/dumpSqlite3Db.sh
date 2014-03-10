#!/bin/sh
if [ ! -f geodic.sq3 ]; then echo "copy 'geodic.sq3' to the current directory first."; fi
if [ -f geodic.sq3 ]; then echo ".dump" | sqlite3 geodic.sq3 | gzip -c > geodic.dump.gz; fi
