#!/bin/sh
# Run this to generate all the initial makefiles, etc.

DIE=0

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have autoconf installed to compile xdelta."
	DIE=1
}

(libtool --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have libtool installed to compile xdelta."
	DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have automake installed to compile xdelta."
	DIE=1
}

if test "$DIE" -eq 1; then
	exit 1
fi

(test -f xdelta.c) || {
	echo "You must run this script in the top-level xdelta directory"
	exit 1
}

#if test -z "$*"; then
#	echo "I am going to run ./configure with no arguments - if you wish "
#	echo "to pass any to it, please specify them on the $0 command line."
#fi

for i in .
do
  echo processing $i
  (cd $i; \
    libtoolize --copy --force; \
    aclocal $ACLOCAL_FLAGS; autoheader; \
    automake --add-missing; \
    autoheader; \
    autoconf)
done

#echo "Running ./configure --enable-maintainer-mode" "$@"
#./configure --enable-maintainer-mode "$@"

echo
#echo "Now type 'make' to compile xdelta."
echo "Now you are ready to run ./configure"

