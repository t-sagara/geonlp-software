#!/bin/sh
touch INSTALL NEWS README COPYING AUTHORS ChangeLog
autoheader
libtoolize --force
aclocal
automake --add-missing --copy
autoconf
