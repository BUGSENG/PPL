/* GMP_Integer class declaration.
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

#ifndef PPL_GMP_Integer_defs_hh
#define PPL_GMP_Integer_defs_hh 1

#include "GMP_Integer.types.hh"
#include "globals.types.hh"
#include <cstddef>

namespace Parma_Polyhedra_Library {

//! \name Accessor Functions
//@{

//! Returns a const reference to the underlying integer value.
/*! \relates GMP_Integer */
const mpz_class& raw_value(const GMP_Integer& x);

//! Returns a reference to the underlying integer value.
/*! \relates GMP_Integer */
mpz_class& raw_value(GMP_Integer& x);

//@} // Accessor Functions

//! \name Memory Size Inspection Functions
//@{

#ifndef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Returns the total size in bytes of the memory occupied by \p x.
/*! \relates GMP_Integer */
#endif // !defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
memory_size_type total_memory_in_bytes(const GMP_Integer& x);

#ifndef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Returns the size in bytes of the memory managed by \p x.
/*! \relates GMP_Integer */
#endif // !defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
memory_size_type external_memory_in_bytes(const GMP_Integer& x);

//@} // Memory Size Inspection Functions

//! \name Arithmetic Operators
//@{

//! Assigns to \p x its negation.
/*! \relates GMP_Integer */
void neg_assign(GMP_Integer& x);

//! Assigns to \p x the negation of \p y.
/*! \relates GMP_Integer */
void neg_assign(GMP_Integer& x, const GMP_Integer& y);

//! Assigns to \p x its absolute value.
/*! \relates GMP_Integer */
void abs_assign(GMP_Integer& x);

//! Assigns to \p x the absolute value of \p y.
/*! \relates GMP_Integer */
void abs_assign(GMP_Integer& x, const GMP_Integer& y);

//! Assigns to \p x the remainder of the division of \p y by \p z.
/*! \relates GMP_Integer */
void rem_assign(GMP_Integer& x,
		const GMP_Integer& y, const GMP_Integer& z);

//! Assigns to \p x the greatest common divisor of \p y and \p z.
/*! \relates GMP_Integer */
void gcd_assign(GMP_Integer& x,
		const GMP_Integer& y, const GMP_Integer& z);

//! Extended GCD.
/*! \relates GMP_Integer
  Assigns to \p x the greatest common divisor of \p y and \p z, and to
  \p s and \p t the values such that \p y * \p s + \p z * \p t = \p x.
*/
void gcdext_assign(GMP_Integer& x, GMP_Integer& s, GMP_Integer& t,
		   const GMP_Integer& y, const GMP_Integer& z);

//! Assigns to \p x the least common multiple of \p y and \p z.
/*! \relates GMP_Integer */
void lcm_assign(GMP_Integer& x,
		const GMP_Integer& y, const GMP_Integer& z);

//! Assigns to \p x the value <CODE>x + y * z</CODE>.
/*! \relates GMP_Integer */
void add_mul_assign(GMP_Integer& x,
		    const GMP_Integer& y, const GMP_Integer& z);

//! Assigns to \p x the value <CODE>x - y * z</CODE>.
/*! \relates GMP_Integer */
void sub_mul_assign(GMP_Integer& x,
		    const GMP_Integer& y, const GMP_Integer& z);

/*! \brief
  If \p z divides \p y, assigns to \p x the quotient of the integer
  division of \p y and \p z.

  \relates GMP_Integer
  The behavior is undefined if \p z does not divide \p y.
*/
void exact_div_assign(GMP_Integer& x,
		      const GMP_Integer& y, const GMP_Integer& z);

//! Assigns to \p x the integer square root of \p y.
/*! \relates GMP_Integer */
void sqrt_assign(GMP_Integer& x, const GMP_Integer& y);

/*! \brief
  Returns a negative, zero or positive value depending on whether
  \p x is lower than, equal to or greater than \p y, respectively.

  \relates GMP_Integer
*/
int cmp(const GMP_Integer& x, const GMP_Integer& y);

//@} // Arithmetic Operators

} // namespace Parma_Polyhedra_Library

#include "GMP_Integer.inlines.hh"

#endif // !defined(PPL_GMP_Integer_defs_hh)
