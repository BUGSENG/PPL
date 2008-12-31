/* Bit_Matrix class implementation: inline functions.
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

#ifndef PPL_Bit_Matrix_inlines_hh
#define PPL_Bit_Matrix_inlines_hh 1

#include <algorithm>
#include <cassert>

namespace Parma_Polyhedra_Library {

inline
Bit_Matrix::Bit_Matrix()
  : rows(),
    row_size(0) {
}

inline dimension_type
Bit_Matrix::max_num_rows() {
  return std::vector<Bit_Row>().max_size();
}

inline
Bit_Matrix::Bit_Matrix(const dimension_type n_rows,
		       const dimension_type n_columns)
  : rows(n_rows),
    row_size(n_columns) {
}

inline
Bit_Matrix::Bit_Matrix(const Bit_Matrix& y)
  : rows(y.rows),
    row_size(y.row_size) {
}

inline
Bit_Matrix::~Bit_Matrix() {
}

inline void
Bit_Matrix::rows_erase_to_end(const dimension_type first_to_erase) {
  // The first row to be erased cannot be greater
  // than the actual number of the rows of the matrix.
  assert(first_to_erase <= rows.size());
  if (first_to_erase < rows.size())
    rows.erase(rows.begin() + first_to_erase, rows.end());
  assert(OK());
}

inline void
Bit_Matrix::columns_erase_to_end(const dimension_type first_to_erase) {
  // The first column to be erased cannot be greater
  // than the actual number of the columns of the matrix.
  assert(first_to_erase <= row_size);
  row_size = first_to_erase;
  assert(OK());
}

inline void
Bit_Matrix::swap(Bit_Matrix& y) {
  std::swap(row_size, y.row_size);
  std::swap(rows, y.rows);
}

inline Bit_Row&
Bit_Matrix::operator[](const dimension_type k) {
  assert(k < rows.size());
  return rows[k];
}

inline const Bit_Row&
Bit_Matrix::operator[](const dimension_type k) const {
  assert(k < rows.size());
  return rows[k];
}

inline dimension_type
Bit_Matrix::num_columns() const {
  return row_size;
}

inline dimension_type
Bit_Matrix::num_rows() const {
  return rows.size();
}

inline void
Bit_Matrix::clear() {
  // Clear `rows' and minimize its capacity.
  std::vector<Bit_Row>().swap(rows);
  row_size = 0;
}

inline memory_size_type
Bit_Matrix::total_memory_in_bytes() const {
  return sizeof(*this) + external_memory_in_bytes();
}

inline bool
Bit_Matrix::Bit_Row_Less_Than::
operator()(const Bit_Row& x, const Bit_Row& y) const {
  return compare(x, y) < 0;
}

inline bool
Bit_Matrix::sorted_contains(const Bit_Row& row) const {
  assert(check_sorted());
  return std::binary_search(rows.begin(), rows.end(), row,
			    Bit_Row_Less_Than());
}

/*! \relates Bit_Matrix */
inline bool
operator!=(const Bit_Matrix& x, const Bit_Matrix& y) {
  return !(x == y);
}

} // namespace Parma_Polyhedra_Library


namespace std {

/*! \relates Parma_Polyhedra_Library::Bit_Matrix */
inline void
swap(Parma_Polyhedra_Library::Bit_Matrix& x,
     Parma_Polyhedra_Library::Bit_Matrix& y) {
  x.swap(y);
}

} // namespace std

#endif // !defined(PPL_Bit_Matrix_inlines_hh)
