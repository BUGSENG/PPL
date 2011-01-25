dnl A function to check whether the C++ compiler has the `remainder' bug.
dnl Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
dnl Copyright (C) 2010-2011 BUGSENG srl (http://bugseng.com)
dnl
dnl This file is part of the Parma Polyhedra Library (PPL).
dnl
dnl The PPL is free software; you can redistribute it and/or modify it
dnl under the terms of the GNU General Public License as published by the
dnl Free Software Foundation; either version 3 of the License, or (at your
dnl option) any later version.
dnl
dnl The PPL is distributed in the hope that it will be useful, but WITHOUT
dnl ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
dnl FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
dnl for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software Foundation,
dnl Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
dnl
dnl For the most up-to-date information see the Parma Polyhedra Library
dnl site: http://www.cs.unipr.it/ppl/ .

AC_DEFUN([AC_CXX_HAS_REMAINDER_BUG],
[

ac_save_CPPFLAGS="$CPPFLAGS"
ac_save_LIBS="$LIBS"
AC_LANG_PUSH(C++)

AC_MSG_CHECKING([if the compiler has the remainder bug])
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <climits>

int minus_one(int n) {
  return (n+1)*(n-1)-n*n;
}

int p(int x, int y) {
  int z = x % y;
  return z;
}

int main(int argc, char** argv) {
  if (p(INT_MIN, minus_one(argc)) != 0)
    return 1;
  else
    return 0;
}
]])],
  AC_MSG_RESULT(no)
  ac_cv_cxx_has_remainder_bug=no,
  AC_MSG_RESULT(yes)
  ac_cv_cxx_has_remainder_bug=yes,
  AC_MSG_RESULT([assuming yes])
  ac_cv_cxx_has_remainder_bug=yes)

if test x"$ac_cv_cxx_has_remainder_bug" = xyes
then
  value=1
else
  value=0
fi
AC_DEFINE_UNQUOTED(PPL_CXX_HAS_REMAINDER_BUG, $value,
  [Not zero if the C++ compiler has the remainder bug.])

AC_LANG_POP(C++)
CPPFLAGS="$ac_save_CPPFLAGS"
LIBS="$ac_save_LIBS"
])

