#!/bin/sh
libtoolize --copy --force
aclocal --force
automake --copy --force --add-missing
autoconf --force
