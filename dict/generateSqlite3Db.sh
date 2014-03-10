#!/bin/sh
if [ -f geodic.sq3 ]; then rm geodic.sq3; fi
gzip -dc geodic.dump.gz | sqlite3 geodic.sq3
