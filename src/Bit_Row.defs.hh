/* Bit_Row class declaration.
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

#ifndef PPL_Bit_Row_defs_hh
#define PPL_Bit_Row_defs_hh 1

#include "Bit_Row.types.hh"
#include "globals.types.hh"
#include <iosfwd>
#include <gmpxx.h>
#include <vector>

namespace Parma_Polyhedra_Library {

// Put them in the namespace here to declare them friends later.

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Returns <CODE>true</CODE> if and only if \p x and \p y are equal.
/*! \relates Bit_Row */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
bool operator==(const Bit_Row& x, const Bit_Row& y);

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Returns <CODE>true</CODE> if and only if \p x and \p y are not equal.
/*! \relates Bit_Row */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
bool operator!=(const Bit_Row& x, const Bit_Row& y);

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! The basic comparison function.
/*! \relates Bit_Row
  Compares \p x with \p y starting from the least significant bits.
  The ordering is total and has the following property: if \p x and \p y
  are two rows seen as sets of naturals, if \p x is a strict subset
  of \p y, then \p x comes before \p y.

  Returns
  - -1 if \p x comes before \p y in the ordering;
  -  0 if \p x and \p y are equal;
  -  1 if \p x comes after \p y in the ordering.
*/
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
int compare(const Bit_Row& x, const Bit_Row& y);

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Set-theoretic inclusion test.
/*! \relates Bit_Row */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
bool subset_or_equal(const Bit_Row& x, const Bit_Row& y);

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
/*! \brief
  Set-theoretic inclusion test: sets \p strict_subset to a Boolean
  indicating whether the inclusion is strict or not.

  \relates Bit_Row
*/
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
bool subset_or_equal(const Bit_Row& x, const Bit_Row& y,
		     bool& strict_subset);

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Set-theoretic strict inclusion test.
/*! \relates Bit_Row */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
bool strict_subset(const Bit_Row& x, const Bit_Row& y);

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Set-theoretic union.
/*! \relates Bit_Row */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
void set_union(const Bit_Row& x, const Bit_Row& y, Bit_Row& z);

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Set-theoretic intersection.
/*! \relates Bit_Row */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
void set_intersection(const Bit_Row& x, const Bit_Row& y, Bit_Row& z);

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Set-theoretic difference.
/*! \relates Bit_Row */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
void set_difference(const Bit_Row& x, const Bit_Row& y, Bit_Row& z);

} // namespace Parma_Polyhedra_Library

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! A row in a matrix of bits.
/*! \ingroup PPL_CXX_interface */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
class Parma_Polyhedra_Library::Bit_Row {
public:
  //! Default constructor.
  Bit_Row();

  //! Copy-constructor.
  Bit_Row(const Bit_Row& y);

  //! Destructor.
  ~Bit_Row();

  //! Assignment operator.
  Bit_Row& operator=(const Bit_Row& y);

  //! Swaps \p *this with \p y.
  void swap(Bit_Row& y);

  //! Returns the truth value corresponding to the bit in position \p k.
  bool operator[](unsigned long k) const;

  //! Sets the bit in position \p k.
  void set(unsigned long k);

  //! Sets bits up to position \p k (excluded).
  void set_until(unsigned long k);

  //! Clears the bit in position \p k.
  void clear(unsigned long k);

  //! Clears bits from position \p k (included) onward.
  void clear_from(unsigned long k);

  //! Clears all the bits of the row.
  void clear();

  friend int compare(const Bit_Row& x, const Bit_Row& y);
  friend bool operator==(const Bit_Row& x, const Bit_Row& y);
  friend bool operator!=(const Bit_Row& x, const Bit_Row& y);
  friend bool subset_or_equal(const Bit_Row& x, const Bit_Row& y);
  friend bool subset_or_equal(const Bit_Row& x, const Bit_Row& y,
			      bool& strict_subset);
  friend bool strict_subset(const Bit_Row& x, const Bit_Row& y);
  friend void set_union(const Bit_Row& x, const Bit_Row& y, Bit_Row& z);
  friend void set_intersection(const Bit_Row& x, const Bit_Row& y, Bit_Row& z);
  friend void set_difference(const Bit_Row& x, const Bit_Row& y, Bit_Row& z);

  //! Returns the index of the first set bit or ULONG_MAX if no bit is set.
  unsigned long first() const;

  /*! \brief
    Returns the index of the first set bit after \p position
    or ULONG_MAX if no bit after \p position is set.
  */
  unsigned long next(unsigned long position) const;

  //! Returns the index of the last set bit or ULONG_MAX if no bit is set.
  unsigned long last() const;

  /*! \brief
    Returns the index of the first set bit before \p position
    or ULONG_MAX if no bits before \p position is set.
  */
  unsigned long prev(unsigned long position) const;

  //! Returns the number of set bits in the row.
  unsigned long count_ones() const;

  //! Returns <CODE>true</CODE> if no bit is set in the row.
  bool empty() const;

  //! Returns the total size in bytes of the memory occupied by \p *this.
  memory_size_type total_memory_in_bytes() const;

  //! Returns the size in bytes of the memory managed by \p *this.
  memory_size_type external_memory_in_bytes() const;

  //! Checks if all the invariants are satisfied
  bool OK() const;

private:
  //! Bit-vector representing the row.
  mpz_t vec;

  //! Assuming \p w is nonzero, returns the index of the first set bit in \p w.
  static unsigned int first_one(mp_limb_t w);

  //! Assuming \p w is nonzero, returns the index of the last set bit in \p w.
  static unsigned int last_one(mp_limb_t w);
};

namespace std {

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Specializes <CODE>std::swap</CODE>.
/*! \relates Parma_Polyhedra_Library::Bit_Row */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
void swap(Parma_Polyhedra_Library::Bit_Row& x,
	  Parma_Polyhedra_Library::Bit_Row& y);

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Specializes <CODE>std::iter_swap</CODE>.
/*! \relates Parma_Polyhedra_Library::Bit_Row */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
void
iter_swap(std::vector<Parma_Polyhedra_Library::Bit_Row>::iterator x,
	  std::vector<Parma_Polyhedra_Library::Bit_Row>::iterator y);

} // namespace std

#include "Bit_Row.inlines.hh"

#endif // !defined(PPL_Bit_Row_defs_hh)
