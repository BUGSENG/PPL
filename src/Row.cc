/* Row class implementation (non-inline functions).
   Copyright (C) 2001 Roberto Bagnara <bagnara@cs.unipr.it>

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

#include <config.h>

#include "Row.defs.hh"

#include "globals.hh"
#include <iostream>

namespace PPL = Parma_Polyhedra_Library;

/*!
  Adds new positions to the real implementation of the row
  obtaining a new row having size \p new_size.
*/
void
PPL::Row::Impl::grow_no_copy(size_t new_size) {
  assert(size() <= new_size);
  for (size_t i = size(); i < new_size; ++i) {
    new (&vec_[i]) Integer();
    bump_size();
  }
}


/*!
  Delete elements from the real implementation of the row
  from \p new_size - th position to the end.
*/
void
PPL::Row::Impl::shrink(size_t new_size) {
  assert(new_size <= size());
  // We assume construction was done "forward".
  // We thus perform destruction "backward".
  for (size_t i = size(); i != new_size; ) {
    --i;
    // ~Integer() does not throw exceptions.  So we do.
    vec_[i].~Integer();
  }
  set_size(new_size);
}


void
PPL::Row::Impl::copy_construct(const Impl& y) {
  size_t y_size = y.size();
  for (size_t i = 0; i < y_size; ++i) {
    new (&vec_[i]) Integer(y.vec_[i]);
    bump_size();
  }
}


int
PPL::Row::first() const {
  for (size_t i = 1, size_ = size(); i < size_; ++i)
    if ((*this)[i] != 0)
      return i-1;
  return -1;
}


int
PPL::Row::next(int p) const {
  assert(p >= 0 && unsigned(p) < size()-1);
  for (size_t i = p+2, size_ = size(); i < size_; ++i)
    if ((*this)[i] != 0)
      return i-1;
  return -1;
}


int
PPL::Row::last() const {
  for (size_t i = size()-1; i >= 1; --i)
    if ((*this)[i] != 0)
      return i-1;
  return -1;
}


int
PPL::Row::prev(int n) const {
  assert(n >= 0 && unsigned(n) < size()-1);
  for (size_t i = n; i >= 1; --i)
    if ((*this)[i] != 0)
      return i-1;
  return -1;
}


/*!
  Computes the Greatest Common Divisor (GCD) among the elements of
  the row and normalizes them by the GCD itself.
  This is useful because we know that constraints and generators are
  unique up a multiplicative coefficient.
*/
void
PPL::Row::normalize() {
  Row& x = *this;
  // Compute the GCD of all the coefficients.
  // The GCD goes into tmp_Integer_1.
  tmp_Integer_1 = 0;
  size_t sz = size();
  for (size_t i = sz; i-- > 0; ) {
    const Integer& x_i = x[i];
    if (x_i != 0)
      tmp_Integer_1.gcd_assign(x_i);
  }
  if (tmp_Integer_1 > 1)
    // Divide the coefficients by the GCD.
    for (size_t i = sz; i-- > 0; )
      x[i].exact_div_assign(tmp_Integer_1);
}

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
/*!
  \param x    A row of coefficients.
  \param y    Another row.

  \return     The returned absolute value can be \f$0, 1\f$ or \f$2\f$.

  Compares \p x and \p y,
  where \p x and \p y are assumed to be of the same size.
  The comparison is such that:
  -# equalities are smaller than inequalities;
  -# lines are smaller than vertices and rays;
  -# the ordering is lexicographic;
  -# the positions compared are, in decreasing order of significance,
     1, 2, ..., \p size(), 0;
  -# the result is negative, zero, or positive if x is smaller than,
     equal to, or greater than y, respectively;
  -# when \p x and \p y are different, the absolute value of the
     result is 1 if the difference is due to the coefficient in
     position 0; it is 2 otherwise.

  When \p x and \p y represent the hyper-planes associated
  to two equality or inequality constraints, the coefficient
  at 0 is the known term.
  In this case, the return value can be characterized as follows:
  - -2, if \p x is smaller than \p y and they are \e not parallel;
  - -1, if \p x is smaller than \p y and they \e are parallel;
  -  0, if \p x and y are equal;
  - +1, if \p y is smaller than \p x and they \e are parallel;
  - +2, if \p y is smaller than \p x and they are \e not parallel.
*/
#endif // PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS

int
PPL::compare(const Row& x, const Row& y) {
  // `x' and `y' must have the same number of elements.
  assert(x.size() == y.size());
  bool x_is_line_or_equality = x.is_line_or_equality();
  bool y_is_line_or_equality = y.is_line_or_equality();
  if (x_is_line_or_equality != y_is_line_or_equality)
    // Equalities (lines) precede inequalities (ray/vertex).
    return y_is_line_or_equality ? 2 : -2;

  // Compare all the coefficients of the row starting from position 1.
  size_t sz = x.size();
  for (size_t i = 1; i < sz; ++i) {
    int comp = cmp(x[i], y[i]);
    if (comp != 0)
      // There is at least a different coefficient.
      return (comp > 0) ? 2 : -2;
  }
  // If all the coefficients in `x' equal all the coefficients in `y'
  // (starting from position 1) we compare coefficients in position 0,
  // i.e., inhomogeneous terms.
  int comp = cmp(x[0], y[0]);
  if (comp != 0)
    return (comp > 0) ? 1 : -1;

  // `x' and `y' are equal.
  return 0;
}

PPL::Integer
PPL::operator *(const Row& x, const Row& y) {
  // Scalar product is only defined  if `x' and `y' have the same size.
  assert(x.size() == y.size());
  Integer result = 0;
  for (size_t i = x.size(); i-- > 0; ) {
    // The following lines optimize the computation of result += x[i] * y[i].
    tmp_Integer_1.mul_assign(x[i], y[i]);
    result += tmp_Integer_1;
  }
  return result;
}

PPL::Integer
PPL::Row::projected_scalar_prod(const Row& x, const Row& y) {
  // The dimension of `x' must be less then the dimension of `y'.
  assert(x.size() < y.size());
  Integer result = 0;
  for (size_t i = x.size(); i-- > 0; ) {
    // The following lines optimize the computation of result += x[i] * y[i].
    tmp_Integer_1.mul_assign(x[i], y[i]);
    result += tmp_Integer_1;
  }
  return result;
}

/*!
  \param y   The row that will be combined with \p *this object.
  \param k   The position of \p *this that have to be \f$0\f$.

  Computes a linear combination between \p *this and \p y such
  that the k-th element of \p *this become \f$0\f$. Then it assigns the
  resulting row to \p *this and normalizes it.
*/
void
PPL::Row::linear_combine(const Row& y, size_t k) {
  Row& x = *this;
  // We can combine only vector of the same dimension.
  assert(x.size() == y.size());
  assert(y[k] != 0 && x[k] != 0);
  // Let g be the GCD between `x[k]' and `y[k]'.
  // For each i the following computes
  //   x[i] = x[i]*y[k]/g - y[i]*x[k]/g.
  tmp_Integer_1.gcd_assign(x[k], y[k]);
  tmp_Integer_2.exact_div_assign(x[k], tmp_Integer_1);
  tmp_Integer_3.exact_div_assign(y[k], tmp_Integer_1);
  for (size_t i = size(); i-- > 0; )
    if (i != k) {
      tmp_Integer_4.mul_assign(x[i], tmp_Integer_3);
      tmp_Integer_5.mul_assign(y[i], tmp_Integer_2);
      x[i].sub_assign(tmp_Integer_4, tmp_Integer_5);
    }
  x[k] = 0;
  x.normalize();
}

std::ostream&
PPL::operator <<(std::ostream& s, const PPL::Row& row) {
  size_t i = 0;
  size_t size = row.size();
  if (i < size)
    s << row[i];
  for (++i ; i < size; ++i)
    s << " " << row[i];
  return s;
}
