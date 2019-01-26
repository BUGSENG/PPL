dnl A function to check for the existence and usability of GMP.
dnl Copyright (C) 2010-2019 BUGSENG srl (http://bugseng.com)
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
dnl site: http://bugseng.com/products/ppl/ .

AC_DEFUN([AC_CHECK_CXX11],
[
AC_LANG_PUSH(C++)
AC_MSG_CHECKING([if the C++ compiler supports C++11 features])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
template <typename T>
struct check final {
  static constexpr T value{ __cplusplus };
};

typedef check<check<bool>> right_angle_brackets;

int a;
decltype(a) b;

typedef check<int> check_type;
check_type c{};
check_type&& cr = static_cast<check_type&&>(c);

static_assert(check_type::value == 201103L, "C++11 compiler");
]])],
    AC_MSG_RESULT(yes)
    ac_cv_check_cxx11=yes,
    AC_MSG_RESULT(no)
    ac_cv_check_cx11=no
  )
AC_LANG_POP(C++)
if test "$ac_cv_check_cxx11" = yes; then
  AC_DEFINE(HAVE_CXX11,,
  [Defined if the C++compiler supports C++11 features.])
fi
])
