dnl $Id$
dnl config.m4 for extension explain

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

PHP_ARG_ENABLE(explain, whether to enable explain support,
[  --enable-explain           Enable explain support], yes, yes)

if test "$PHP_EXPLAIN" != "no"; then
  PHP_NEW_EXTENSION(explain, explain.c, $ext_shared)
fi
