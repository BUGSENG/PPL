/* Integer class implementation: inline functions.
   Copyright (C) 2001-2003 Roberto Bagnara <bagnara@cs.unipr.it>

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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#ifndef PPL_Integer_inlines_hh
#define PPL_Integer_inlines_hh 1

namespace Parma_Polyhedra_Library {

inline void
negate(Integer& x) {
  mpz_neg(x.get_mpz_t(), x.get_mpz_t());
}

inline void
gcd_assign(Integer& x, const Integer& y) {
  mpz_gcd(x.get_mpz_t(), x.get_mpz_t(), y.get_mpz_t());
}

inline void
gcd_assign(Integer& x, const Integer& y, const Integer& z) {
  mpz_gcd(x.get_mpz_t(), y.get_mpz_t(), z.get_mpz_t());
}

inline void
lcm_assign(Integer& x, const Integer& y) {
  mpz_lcm(x.get_mpz_t(), x.get_mpz_t(), y.get_mpz_t());
}

inline void
lcm_assign(Integer& x, const Integer& y, const Integer& z) {
  mpz_lcm(x.get_mpz_t(), y.get_mpz_t(), z.get_mpz_t());
}

inline void
exact_div_assign(Integer& x, const Integer& y) {
  mpz_divexact(x.get_mpz_t(), x.get_mpz_t(), y.get_mpz_t());
}

inline void
exact_div_assign(Integer& x, const Integer& y, const Integer& z) {
  mpz_divexact(x.get_mpz_t(), y.get_mpz_t(), z.get_mpz_t());
}

inline void
sqrt_assign(Integer& x) {
  mpz_sqrt(x.get_mpz_t(), x.get_mpz_t());
}

inline void
sqrt_assign(Integer& x, const Integer& y) {
  mpz_sqrt(x.get_mpz_t(), y.get_mpz_t());
}

inline int
cmp(const Integer& x, const Integer& y) {
  return mpz_cmp(x.get_mpz_t(), y.get_mpz_t());
}

inline const Integer&
Integer_zero() {
  static Integer z(0);
  return z;
}

inline const Integer&
Integer_one() {
  static Integer o(1);
  return o;
}

} // namespace Parma_Polyhedra_Library

inline void
std::swap(Parma_Polyhedra_Library::Integer& x,
	  Parma_Polyhedra_Library::Integer& y) {
  mpz_swap(x.get_mpz_t(), y.get_mpz_t());
}

#endif // !defined(PPL_Integer_inlines_hh)
