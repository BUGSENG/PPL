/* Specializations of std::numeric_limits for "checked" types.
   Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

This file is part of the Parma Polyhedra Library (PPL).

The PPL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The PPL is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#ifndef PPL_checked_numeric_limits_hh
#define PPL_checked_numeric_limits_hh 1

#include "Checked_Number.defs.hh"
#include "checked_int.inlines.hh"
#include <limits>

namespace std {

using namespace Parma_Polyhedra_Library;

#define PPL_SPECIALIZE_LIMITS_INT(T)					\
/*! \brief Partial specialization of std::numeric_limits. */		\
template <typename Policy>						\
 class numeric_limits<Checked_Number<T, Policy> >                       \
  : public numeric_limits<T> {						\
 private:								\
  typedef Checked_Number<T, Policy> Type;                               \
									\
 public:								\
  static const bool has_infinity = Policy::has_infinity;		\
  static const bool has_quiet_NaN =  Policy::has_nan;                   \
                                                                        \
  static Type min() {							\
    return Checked::Extended_Int<Policy, T>::min;                       \
  }									\
                                                                        \
  static Type max() {							\
    return Checked::Extended_Int<Policy, T>::max;                       \
  }									\
									\
  static Type infinity() {						\
    return								\
      Policy::has_infinity						\
      ? PLUS_INFINITY                                                   \
      : static_cast<Type>(0);						\
  }									\
									\
  static Type quiet_NaN() {						\
    return								\
      Policy::has_nan                                                   \
      ? NOT_A_NUMBER                                                    \
      : static_cast<Type>(0);						\
  }									\
};

PPL_SPECIALIZE_LIMITS_INT(signed char)
PPL_SPECIALIZE_LIMITS_INT(signed short)
PPL_SPECIALIZE_LIMITS_INT(signed int)
PPL_SPECIALIZE_LIMITS_INT(signed long)
PPL_SPECIALIZE_LIMITS_INT(signed long long)

PPL_SPECIALIZE_LIMITS_INT(unsigned char)
PPL_SPECIALIZE_LIMITS_INT(unsigned short)
PPL_SPECIALIZE_LIMITS_INT(unsigned int)
PPL_SPECIALIZE_LIMITS_INT(unsigned long)
PPL_SPECIALIZE_LIMITS_INT(unsigned long long)

#undef PPL_SPECIALIZE_LIMITS_INT

#define PPL_SPECIALIZE_LIMITS_FLOAT(T)					\
/*! \brief Partial specialization of std::numeric_limits. */		\
template <typename Policy>						\
struct numeric_limits<Checked_Number<T, Policy> > \
  : public numeric_limits<T> {						\
};

#if PPL_SUPPORTED_FLOAT
PPL_SPECIALIZE_LIMITS_FLOAT(float)
#endif
#if PPL_SUPPORTED_DOUBLE
PPL_SPECIALIZE_LIMITS_FLOAT(double)
#endif
#if PPL_SUPPORTED_LONG_DOUBLE
PPL_SPECIALIZE_LIMITS_FLOAT(long double)
#endif

#undef PPL_SPECIALIZE_LIMITS_FLOAT

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Partial specialization of std::numeric_limits.
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
template <typename Policy>
class
numeric_limits<Checked_Number<mpz_class, Policy> >
  : public numeric_limits<mpz_class> {
private:
  typedef Checked_Number<mpz_class, Policy> Type;

public:
  static const bool has_infinity = Policy::has_infinity;
  static const bool has_quiet_NaN =  Policy::has_nan;

  static Type infinity() {
    return
      Policy::has_infinity
      ? PLUS_INFINITY
      : static_cast<Type>(0);
  }

  static Type quiet_NaN() {
    return
      Policy::has_nan
      ? NOT_A_NUMBER
      : static_cast<Type>(0);
  }
};

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Partial specialization of std::numeric_limits.
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
template <typename Policy>
class
numeric_limits<Checked_Number<mpq_class, Policy> >
: public numeric_limits<mpq_class> {
private:
  typedef Checked_Number<mpq_class, Policy> Type;

public:
  static const bool has_infinity = Policy::has_infinity;
  static const bool has_quiet_NaN =  Policy::has_nan;

  static Type infinity() {
    return
      Policy::has_infinity
      ? PLUS_INFINITY
      : static_cast<Type>(0);
  }

  static Type quiet_NaN() {
    return
      Policy::has_nan
      ? NOT_A_NUMBER
      : static_cast<Type>(0);
  }
};

} // namespace std

#endif // !defined(PPL_checked_numeric_limits_hh)
