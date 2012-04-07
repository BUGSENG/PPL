/* Linear_Row class implementation (non-inline functions).
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2012 BUGSENG srl (http://bugseng.com)

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
site: http://bugseng.com/products/ppl/ . */

#include "ppl-config.h"
#include "Linear_Row.defs.hh"
#include "Coefficient.defs.hh"
#include <algorithm>
#include <iostream>

namespace PPL = Parma_Polyhedra_Library;

void
PPL::Linear_Row::sign_normalize() {
  if (is_line_or_equality()) {
    Linear_Row& x = *this;
    const dimension_type sz = x.size();
    // `first_non_zero' indicates the index of the first
    // coefficient of the row different from zero, disregarding
    // the very first coefficient (inhomogeneous term / divisor).
    dimension_type first_non_zero;
    for (first_non_zero = 1; first_non_zero < sz; ++first_non_zero)
      if (x[first_non_zero] != 0)
	break;
    if (first_non_zero < sz)
      // If the first non-zero coefficient of the row is negative,
      // we negate the entire row.
      if (x[first_non_zero] < 0) {
	for (dimension_type j = first_non_zero; j < sz; ++j)
	  neg_assign(x[j]);
	// Also negate the first coefficient.
	neg_assign(x[0]);
      }
  }
}

bool
PPL::Linear_Row::check_strong_normalized() const {
  Linear_Row tmp = *this;
  tmp.strong_normalize();
  return compare(*this, tmp) == 0;
}

/*! \relates Parma_Polyhedra_Library::Linear_Row */
int
PPL::compare(const Linear_Row& x, const Linear_Row& y) {
  const bool x_is_line_or_equality = x.is_line_or_equality();
  const bool y_is_line_or_equality = y.is_line_or_equality();
  if (x_is_line_or_equality != y_is_line_or_equality)
    // Equalities (lines) precede inequalities (ray/point).
    return y_is_line_or_equality ? 2 : -2;

  // Compare all the coefficients of the row starting from position 1.
  const dimension_type xsz = x.size();
  const dimension_type ysz = y.size();
  const dimension_type min_sz = std::min(xsz, ysz);
  dimension_type i;
  for (i = 1; i < min_sz; ++i)
    if (const int comp = cmp(x[i], y[i]))
      // There is at least a different coefficient.
      return (comp > 0) ? 2 : -2;

  // Handle the case where `x' and `y' are of different size.
  if (xsz != ysz) {
    for( ; i < xsz; ++i)
      if (const int sign = sgn(x[i]))
	return (sign > 0) ? 2 : -2;
    for( ; i < ysz; ++i)
      if (const int sign = sgn(y[i]))
	return (sign < 0) ? 2 : -2;
  }

  // If all the coefficients in `x' equal all the coefficients in `y'
  // (starting from position 1) we compare coefficients in position 0,
  // i.e., inhomogeneous terms.
  if (const int comp = cmp(x[0], y[0]))
    return (comp > 0) ? 1 : -1;

  // `x' and `y' are equal.
  return 0;
}

void
PPL::Linear_Row::linear_combine(const Linear_Row& y, const dimension_type k) {
  Linear_Row& x = *this;
  // We can combine only vector of the same dimension.
  PPL_ASSERT(x.size() == y.size());
  PPL_ASSERT(y[k] != 0 && x[k] != 0);
  // Let g be the GCD between `x[k]' and `y[k]'.
  // For each i the following computes
  //   x[i] = x[i]*y[k]/g - y[i]*x[k]/g.
  PPL_DIRTY_TEMP_COEFFICIENT(normalized_x_k);
  PPL_DIRTY_TEMP_COEFFICIENT(normalized_y_k);
  normalize2(x[k], y[k], normalized_x_k, normalized_y_k);
  for (dimension_type i = size(); i-- > 0; )
    if (i != k) {
      Coefficient& x_i = x[i];
      x_i *= normalized_y_k;
      sub_mul_assign(x_i, y[i], normalized_x_k);
    }
  x[k] = 0;
  x.strong_normalize();
}

bool
PPL::Linear_Row::is_zero() const {
  const Linear_Row& x = *this;
  for (dimension_type i = x.size(); i-- > 0; )
    if (x[i] != 0)
      return false;
  return true;
}

bool
PPL::Linear_Row::all_homogeneous_terms_are_zero() const {
  const Linear_Row& x = *this;
  for (dimension_type i = x.size(); --i > 0; )
    if (x[i] != 0)
      return false;
  return true;
}

namespace {

// These are the keywords that indicate the individual assertions.
const char* rpi_valid_text = "RPI_V";
const char* is_rpi_text = "RPI";
const char* nnc_valid_text = "NNC_V";
const char* is_nnc_text = "NNC";
const char* bit_texts[] = {rpi_valid_text, is_rpi_text,
                           nnc_valid_text, is_nnc_text};

} // namespace

void
PPL::Linear_Row::Flags::ascii_dump(std::ostream& s) const {
  bool is_rpi = test_bits(1U << Flags::rpi_bit);
  bool is_nnc = test_bits(1U << Flags::nnc_bit);
  bool rpi_valid;
  bool nnc_valid;
#ifdef NDEBUG
  // Dump unhandled bits always as true so to permit loading from a
  // debug enabled executable without failing assertions.
  rpi_valid = true;
  nnc_valid = true;
#else
  rpi_valid = test_bits(1U << Flags::rpi_validity_bit);
  nnc_valid = test_bits(1U << Flags::nnc_validity_bit);
#endif
  s << (rpi_valid ? '+' : '-') << rpi_valid_text << ' '
    << (is_rpi ? '+' : '-') << is_rpi_text << ' '
    << ' '
    << (nnc_valid ? '+' : '-') << nnc_valid_text << ' '
    << (is_nnc ? '+' : '-') << is_nnc_text;
}

PPL_OUTPUT_DEFINITIONS_ASCII_ONLY(Linear_Row::Flags)

bool
PPL::Linear_Row::Flags::ascii_load(std::istream& s) {
  std::string str;
  // Assume that the bits are used in sequence.
  reset_bits(std::numeric_limits<base_type>::max());
  for (unsigned int bit = 0;
       bit < (sizeof(bit_texts) / sizeof(char*));
       ++bit) {
    if (!(s >> str))
      return false;
    if (str[0] == '+') {
      bool load = true;
#ifdef NDEBUG
      // Do not load unhandled bits.
      if (bit == rpi_validity_bit || bit == nnc_validity_bit)
        load = false;
#endif
      if (load)
        set_bits(1U << (Dense_Row::Flags::first_free_bit + bit));
    }
    else if (str[0] != '-')
      return false;
    if (str.compare(1, strlen(bit_texts[bit]), bit_texts[bit]) != 0)
      return false;
  }
  return true;
}

void
PPL::Linear_Row::ascii_dump(std::ostream& s) const {
  const Dense_Row& x = *this;
  const dimension_type x_size = x.size();
  s << "size " << x_size << " ";
  for (dimension_type i = 0; i < x_size; ++i)
    s << x[i] << ' ';
  s << "f ";
  flags().ascii_dump(s);
  s << "\n";
}

PPL_OUTPUT_DEFINITIONS_ASCII_ONLY(Linear_Row)

bool
PPL::Linear_Row::ascii_load(std::istream& s) {
  std::string str;
  if (!(s >> str) || str != "size")
    return false;
  dimension_type new_size;
  if (!(s >> new_size))
    return false;

  Dense_Row& x = *this;
  const dimension_type old_size = x.size();
  if (new_size < old_size)
    x.shrink(new_size);
  else if (new_size > old_size) {
    Dense_Row y(new_size, Dense_Row::Flags());
    x.m_swap(y);
  }

  for (dimension_type col = 0; col < new_size; ++col)
    if (!(s >> x[col]))
      return false;
  if (!(s >> str) || str != "f")
    return false;
  
  Flags f;
  if (!f.ascii_load(s))
    return false;
  set_flags(f);
  
  return true;
}

bool
PPL::Linear_Row::OK() const {
  return Dense_Row::OK();
}

bool
PPL::Linear_Row::OK(const dimension_type row_size,
		    const dimension_type row_capacity) const {
  return Dense_Row::OK(row_size, row_capacity);
}
