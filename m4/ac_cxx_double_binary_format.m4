dnl A function to detect the binary format used by C++ doubles.
dnl Copyright (C) 2001-2006 Roberto Bagnara <bagnara@cs.unipr.it>
dnl
dnl This file is part of the Parma Polyhedra Library (PPL).
dnl
dnl The PPL is free software; you can redistribute it and/or modify it
dnl under the terms of the GNU General Public License as published by the
dnl Free Software Foundation; either version 2 of the License, or (at your
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
dnl
AC_DEFUN([AC_CXX_DOUBLE_BINARY_FORMAT],
[
ac_save_CPPFLAGS="$CPPFLAGS"
ac_save_LIBS="$LIBS"
AC_LANG_PUSH(C++)

AC_MSG_CHECKING([the binary format of C++ doubles])
ac_cxx_double_binary_format=unknown
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <limits>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#if SIZEOF_DOUBLE == 8

double
convert(uint32_t msp, uint32_t lsp) {
  union {
    double value;
    struct {
#ifdef WORDS_BIGENDIAN
      uint32_t msp;
      uint32_t lsp;
#else
      uint32_t lsp;
      uint32_t msp;
#endif
    } parts;
  } u;

  u.parts.msp = msp;
  u.parts.lsp = lsp;
  return u.value;
}

int
main() {
  return std::numeric_limits<double>::is_iec559
    && (convert(0xaaacccaaUL, 0xacccaaacUL)
	== -4.018242396032647e-103
    &&	convert(0xcccaaaccUL, 0xcaaacccaUL)
	  == -85705035845709846787631445265530356117787053916987832397725696.0)
    ? 0 : 1;
}

#else // SIZEOF_DOUBLE != 8

int
main() {
  return 1;
}

#endif // SIZEOF_DOUBLE != 8
]])],
  AC_DEFINE(CXX_DOUBLE_BINARY_FORMAT, float_ieee754_double,
    [The binary format of C++ doubles, if supported; undefined otherwise.])
  ac_cxx_double_binary_format="IEEE754 Double Precision")

AC_MSG_RESULT($ac_cxx_double_binary_format)

if test x"$ac_cxx_double_binary_format" = x"unknown"
then
  ac_supported_double=0
else
  ac_supported_double=1
fi
AM_CONDITIONAL(SUPPORTED_DOUBLE, test $ac_supported_double = 1)
AC_DEFINE_UNQUOTED(PPL_SUPPORTED_DOUBLE, $ac_supported_double,
  [Not zero if doubles are supported.])

AC_LANG_POP(C++)
CPPFLAGS="$ac_save_CPPFLAGS"
LIBS="$ac_save_LIBS"
])
