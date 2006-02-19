/* Specialized "checked" functions for GMP's mpq_class numbers.
   Copyright (C) 2001-2006 Roberto Bagnara <bagnara@cs.unipr.it>

This file is part of the Parma Polyhedra Library (PPL).

The PPL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
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

#ifndef PPL_checked_mpq_inlines_hh
#define PPL_checked_mpq_inlines_hh 1

#include <sstream>

namespace Parma_Polyhedra_Library {

namespace Checked {

template <typename Policy>
inline Result
classify_mpq(const mpq_class& v, bool nan, bool inf, bool sign) {
  if ((Policy::store_nan || Policy::store_infinity)
      && ::sgn(v.get_den()) == 0) {
    int s = ::sgn(v.get_num());
    if (Policy::store_nan && (nan || sign) && s == 0)
      return VC_NAN;
    if (!inf && !sign)
      return VC_NORMAL;
    if (Policy::store_infinity) {
      if (s < 0)
	return inf ? VC_MINUS_INFINITY : V_LT;
      if (s > 0)
	return inf ? VC_PLUS_INFINITY : V_GT;
    }
  }
  if (sign)
    return sgn<Policy>(v);
  return VC_NORMAL;
}

SPECIALIZE_CLASSIFY(mpq, mpq_class)

template <typename Policy>
inline bool
is_nan_mpq(const mpq_class& v) {
  return Policy::store_nan
    && ::sgn(v.get_den()) == 0
    && ::sgn(v.get_num()) == 0;
}

SPECIALIZE_IS_NAN(mpq, mpq_class)

template <typename Policy>
inline bool
is_minf_mpq(const mpq_class& v) {
  return Policy::store_infinity
    && ::sgn(v.get_den()) == 0
    && ::sgn(v.get_num()) < 0;
}

SPECIALIZE_IS_MINF(mpq, mpq_class)

template <typename Policy>
inline bool
is_pinf_mpq(const mpq_class& v) {
  return Policy::store_infinity
    && ::sgn(v.get_den()) == 0
    && ::sgn(v.get_num()) > 0;
}

SPECIALIZE_IS_PINF(mpq, mpq_class)

template <typename Policy>
inline bool
is_int_mpq(const mpq_class& v) {
  return !is_nan<Policy>(v) && v.get_den() == 1;
}

SPECIALIZE_IS_INT(mpq, mpq_class)

template <typename Policy>
inline Result
set_special_mpq(mpq_class& v, Result r) {
  Result c = classify(r);
  if (Policy::store_nan && c == VC_NAN) {
    v.get_num() = 0;
    v.get_den() = 0;
  }
  else if (Policy::store_infinity) {
    switch (c) {
    case VC_MINUS_INFINITY:
      v.get_num() = -1;
      v.get_den() = 0;
      break;
    case VC_PLUS_INFINITY:
      v.get_num() = 1;
      v.get_den() = 0;
      break;
    default:
      break;
    }
  }
  return r;
}

SPECIALIZE_SET_SPECIAL(mpq, mpq_class)

SPECIALIZE_COPY(generic, mpq_class)

template <typename Policy, typename From>
inline Result
construct_mpq_base(mpq_class& to, const From& from, Rounding_Dir) {
  new (&to) mpq_class(from);
  return V_EQ;
}

SPECIALIZE_CONSTRUCT(mpq_base, mpq_class, mpz_class)
SPECIALIZE_CONSTRUCT(mpq_base, mpq_class, signed char)
SPECIALIZE_CONSTRUCT(mpq_base, mpq_class, signed short)
SPECIALIZE_CONSTRUCT(mpq_base, mpq_class, signed int)
SPECIALIZE_CONSTRUCT(mpq_base, mpq_class, signed long)
SPECIALIZE_CONSTRUCT(mpq_base, mpq_class, unsigned char)
SPECIALIZE_CONSTRUCT(mpq_base, mpq_class, unsigned short)
SPECIALIZE_CONSTRUCT(mpq_base, mpq_class, unsigned int)
SPECIALIZE_CONSTRUCT(mpq_base, mpq_class, unsigned long)
SPECIALIZE_CONSTRUCT(mpq_base, mpq_class, float)
SPECIALIZE_CONSTRUCT(mpq_base, mpq_class, double)

template <typename Policy, typename From>
inline Result
construct_mpq_long_double(mpq_class& to, const From& from, Rounding_Dir dir) {
  // FIXME: this is an incredibly inefficient implementation!
  new (&to) mpq_class;
  std::stringstream ss;
  output_float<Policy, long double>(ss, from, Numeric_Format(), dir);
  return input_mpq(to, ss);
}

SPECIALIZE_CONSTRUCT(mpq_long_double, mpq_class, long double)

template <typename Policy, typename From>
inline Result
assign_mpq_base(mpq_class& to, const From& from, Rounding_Dir) {
  to = from;
  return V_EQ;
}

SPECIALIZE_ASSIGN(mpq_base, mpq_class, mpz_class)
SPECIALIZE_ASSIGN(mpq_base, mpq_class, signed char)
SPECIALIZE_ASSIGN(mpq_base, mpq_class, signed short)
SPECIALIZE_ASSIGN(mpq_base, mpq_class, signed int)
SPECIALIZE_ASSIGN(mpq_base, mpq_class, signed long)
SPECIALIZE_ASSIGN(mpq_base, mpq_class, unsigned char)
SPECIALIZE_ASSIGN(mpq_base, mpq_class, unsigned short)
SPECIALIZE_ASSIGN(mpq_base, mpq_class, unsigned int)
SPECIALIZE_ASSIGN(mpq_base, mpq_class, unsigned long)
SPECIALIZE_ASSIGN(mpq_base, mpq_class, float)
SPECIALIZE_ASSIGN(mpq_base, mpq_class, double)

template <typename Policy, typename From>
inline Result
assign_mpq_signed_int(mpq_class& to, const From from, Rounding_Dir) {
  if (sizeof(From) <= sizeof(signed long))
    to = static_cast<signed long>(from);
  else {
    mpz_ptr m = to.get_num().get_mpz_t();
    if (from >= 0)
      mpz_import(m, 1, 1, sizeof(From), 0, 0, &from);
    else {
      From n = -from;
      mpz_import(m, 1, 1, sizeof(From), 0, 0, &n);
      mpz_neg(m, m);
    }
    to.get_den() = 1;
  }
  return V_EQ;
}

SPECIALIZE_ASSIGN(mpq_signed_int, mpq_class, signed long long)

template <typename Policy, typename From>
inline Result
assign_mpq_unsigned_int(mpq_class& to, const From from, Rounding_Dir) {
  if (sizeof(From) <= sizeof(unsigned long))
    to = static_cast<unsigned long>(from);
  else {
    mpz_import(to.get_num().get_mpz_t(), 1, 1, sizeof(From), 0, 0, &from);
    to.get_den() = 1;
  }
  return V_EQ;
}

SPECIALIZE_ASSIGN(mpq_unsigned_int, mpq_class, unsigned long long)

template <typename Policy, typename From>
inline Result
assign_mpq_long_double(mpq_class& to, const From& from, Rounding_Dir dir) {
  // FIXME: this is an incredibly inefficient implementation!
  std::stringstream ss;
  output_float<Policy, long double>(ss, from, Numeric_Format(), dir);
  return input_mpq(to, ss);
}

SPECIALIZE_ASSIGN(mpq_long_double, mpq_class, long double)

template <typename Policy>
inline Result
neg_mpq(mpq_class& to, const mpq_class& from, Rounding_Dir) {
  mpq_neg(to.get_mpq_t(), from.get_mpq_t());
  return V_EQ;
}

SPECIALIZE_NEG(mpq, mpq_class, mpq_class)

template <typename Policy>
inline Result
add_mpq(mpq_class& to, const mpq_class& x, const mpq_class& y, Rounding_Dir) {
  to = x + y;
  return V_EQ;
}

SPECIALIZE_ADD(mpq, mpq_class, mpq_class, mpq_class)

template <typename Policy>
inline Result
sub_mpq(mpq_class& to, const mpq_class& x, const mpq_class& y, Rounding_Dir) {
  to = x - y;
  return V_EQ;
}

SPECIALIZE_SUB(mpq, mpq_class, mpq_class, mpq_class)

template <typename Policy>
inline Result
mul_mpq(mpq_class& to, const mpq_class& x, const mpq_class& y, Rounding_Dir) {
  to = x * y;
  return V_EQ;
}

SPECIALIZE_MUL(mpq, mpq_class, mpq_class, mpq_class)

template <typename Policy>
inline Result
div_mpq(mpq_class& to, const mpq_class& x, const mpq_class& y, Rounding_Dir) {
  if (CHECK_P(Policy::check_div_zero, sgn(y) == 0))
    return set_special<Policy>(to, V_DIV_ZERO);
  to = x / y;
  return V_EQ;
}

SPECIALIZE_DIV(mpq, mpq_class, mpq_class, mpq_class)

template <typename Policy>
inline Result
rem_mpq(mpq_class& to, const mpq_class& x, const mpq_class& y, Rounding_Dir) {
  if (CHECK_P(Policy::check_div_zero, sgn(y) == 0))
    return set_special<Policy>(to, V_MOD_ZERO);
  to = x / y;
  to.get_num() %= to.get_den();
  return V_EQ;
}

SPECIALIZE_REM(mpq, mpq_class, mpq_class, mpq_class)

template <typename Policy>
inline Result
mul2exp_mpq(mpq_class& to, const mpq_class& x, int exp, Rounding_Dir dir) {
  if (exp < 0)
    return div2exp<Policy>(to, x, -exp, dir);
  mpz_mul_2exp(to.get_num().get_mpz_t(), x.get_num().get_mpz_t(), exp);
  to.get_den() = x.get_den();
  to.canonicalize();
  return V_EQ;
}

SPECIALIZE_MUL2EXP(mpq, mpq_class, mpq_class)

template <typename Policy>
inline Result
div2exp_mpq(mpq_class& to, const mpq_class& x, int exp, Rounding_Dir dir) {
  if (exp < 0)
    return mul2exp<Policy>(to, x, -exp, dir);
  to.get_num() = x.get_num();
  mpz_mul_2exp(to.get_den().get_mpz_t(), x.get_den().get_mpz_t(), exp);
  to.canonicalize();
  return V_EQ;
}

SPECIALIZE_DIV2EXP(mpq, mpq_class, mpq_class)

template <typename Policy>
inline Result
abs_mpq(mpq_class& to, const mpq_class& from, Rounding_Dir) {
  to = abs(from);
  return V_EQ;
}

SPECIALIZE_ABS(mpq, mpq_class, mpq_class)

template <typename Policy>
inline Result
add_mul_mpq(mpq_class& to, const mpq_class& x, const mpq_class& y, Rounding_Dir) {
  to += x * y;
  return V_EQ;
}

SPECIALIZE_ADD_MUL(mpq, mpq_class, mpq_class, mpq_class)

template <typename Policy>
inline Result
sub_mul_mpq(mpq_class& to, const mpq_class& x, const mpq_class& y, Rounding_Dir) {
  to -= x * y;
  return V_EQ;
}

SPECIALIZE_SUB_MUL(mpq, mpq_class, mpq_class, mpq_class)

extern mpz_class rational_sqrt_precision_parameter;

template <typename Policy>
inline Result
sqrt_mpq(mpq_class& to, const mpq_class& from, Rounding_Dir dir) {
  if (CHECK_P(Policy::check_sqrt_neg, from < 0))
    return set_special<Policy>(to, V_SQRT_NEG);
  const unsigned long k = 3200;
  mpz_class& to_num = to.get_num();
  mul2exp<Policy>(to_num, from.get_num(), 2*k, dir);
  Result rdiv = div<Policy>(to_num, to_num, from.get_den(), dir);
  Result rsqrt = sqrt<Policy>(to_num, to_num, dir);
  mpz_class& to_den = to.get_den();
  to_den = 1;
  mul2exp<Policy>(to_den, to_den, k, dir);
  to.canonicalize();
  return rdiv != V_EQ ? rdiv : rsqrt;
}

SPECIALIZE_SQRT(mpq, mpq_class, mpq_class)

template <typename Policy>
inline Result
input_mpq(mpq_class& to, std::istream& is, Rounding_Dir dir) {
  Result r = input_mpq(to, is);
  if (r == VC_MINUS_INFINITY)
    return assign<Policy>(to, MINUS_INFINITY, dir);
  if (r == VC_PLUS_INFINITY)
    return assign<Policy>(to, PLUS_INFINITY, dir);
  return set_special<Policy>(to, r);
}

template <typename Policy>
inline Result
output_mpq(std::ostream& os,
	   const mpq_class& from,
	   const Numeric_Format&,
	   Rounding_Dir) {
  os << from;
  return V_EQ;
}

SPECIALIZE_INPUT(mpq, mpq_class)
SPECIALIZE_OUTPUT(mpq, mpq_class)

inline memory_size_type
external_memory_in_bytes(const mpq_class& x) {
  return external_memory_in_bytes(x.get_num())
    + external_memory_in_bytes(x.get_den());
}

} // namespace Checked

//! Returns the precision parameter used for rational sqrt calculations.
inline mpz_class
rational_sqrt_precision_parameter() {
  return Checked::rational_sqrt_precision_parameter;
}

//! Sets the precision parameter used for rational sqrt calculations to \p p.
inline void
set_rational_sqrt_precision_parameter(const mpz_class& p) {
  Checked::rational_sqrt_precision_parameter = p;
}

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_checked_mpq_inlines_hh)
