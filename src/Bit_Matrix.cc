/* Bit_Matrix class implementation (non-inline functions).
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

#include <ppl-config.h>

#include "Bit_Matrix.defs.hh"
#include "globals.defs.hh"
#include <iostream>
#include <string>
#include <climits>

#include "swapping_sort.icc"

namespace PPL = Parma_Polyhedra_Library;

PPL::Bit_Matrix&
PPL::Bit_Matrix::operator=(const Bit_Matrix& y){
  rows = y.rows;
  row_size = y.row_size;
  assert(OK());
  return *this;
}

void
PPL::Bit_Matrix::sort_rows() {
  typedef std::vector<Bit_Row>::iterator Iter;
  // Sorting without removing duplicates.
  Iter first = rows.begin();
  Iter last = rows.end();
  swapping_sort(first, last, Bit_Row_Less_Than());
  // Moving all the duplicate elements at the end of the vector.
  Iter new_last = swapping_unique(first, last);
  // Removing duplicates.
  rows.erase(new_last, last);
  assert(OK());
}

void
PPL::Bit_Matrix::add_row(const Bit_Row& row) {
  const dimension_type new_rows_size = rows.size() + 1;
  if (rows.capacity() < new_rows_size) {
    // Reallocation will take place.
    std::vector<Bit_Row> new_rows;
    new_rows.reserve(compute_capacity(new_rows_size, max_num_rows()));
    new_rows.insert(new_rows.end(), new_rows_size, Bit_Row());
    // Put the new row in place.
    dimension_type i = new_rows_size-1;
    new_rows[i] = row;
    // Steal the old rows.
    while (i-- > 0)
      new_rows[i].swap(rows[i]);
    // Put the new rows into place.
    std::swap(rows, new_rows);
  }
  else
    // Reallocation will NOT take place: append a new empty row.
    rows.push_back(row);
  assert(OK());
}

void
PPL::Bit_Matrix::transpose() {
  const Bit_Matrix& x = *this;
  const dimension_type nrows = num_rows();
  const dimension_type ncols = num_columns();
  Bit_Matrix tmp(ncols, nrows);
  for (dimension_type i = nrows; i-- > 0; )
    for (unsigned long j = x[i].last(); j != ULONG_MAX; j = x[i].prev(j))
      tmp[j].set(i);
  swap(tmp);
  assert(OK());
}

void
PPL::Bit_Matrix::transpose_assign(const Bit_Matrix& y) {
  const dimension_type y_nrows = y.num_rows();
  const dimension_type y_ncols = y.num_columns();
  Bit_Matrix tmp(y_ncols, y_nrows);
  for (dimension_type i = y_nrows; i-- > 0; )
    for (unsigned long j = y[i].last(); j != ULONG_MAX; j = y[i].prev(j))
      tmp[j].set(i);
  swap(tmp);
  assert(OK());
}

void
PPL::Bit_Matrix::resize(dimension_type new_n_rows,
		       dimension_type new_n_columns) {
  assert(OK());
  const dimension_type old_num_rows = num_rows();
  if (new_n_columns < row_size) {
    const dimension_type num_preserved_rows
      = std::min(old_num_rows, new_n_rows);
    Bit_Matrix& x = *this;
    for (dimension_type i = num_preserved_rows; i-- > 0; )
      x[i].clear_from(new_n_columns);
  }
  row_size = new_n_columns;
  if (new_n_rows > old_num_rows) {
    if (rows.capacity() < new_n_rows) {
      // Reallocation will take place.
      std::vector<Bit_Row> new_rows;
      new_rows.reserve(compute_capacity(new_n_rows, max_num_rows()));
      new_rows.insert(new_rows.end(), new_n_rows, Bit_Row());
      // Steal the old rows.
      for (dimension_type i = old_num_rows; i-- > 0; )
	new_rows[i].swap(rows[i]);
      // Put the new vector into place.
      std::swap(rows, new_rows);
    }
    else
      // Reallocation will NOT take place.
      rows.insert(rows.end(), new_n_rows - old_num_rows, Bit_Row());
  }
  else if (new_n_rows < old_num_rows)
    // Drop some rows.
    rows.erase(rows.begin() + new_n_rows, rows.end());

  assert(OK());
}

void
PPL::Bit_Matrix::ascii_dump(std::ostream& s) const {
  const Bit_Matrix& x = *this;
  const char separator = ' ';
  s << num_rows() << separator << 'x' << separator
    << num_columns() << "\n";
  for (dimension_type i = 0; i < num_rows(); ++i) {
    for (dimension_type j = 0; j < num_columns(); ++j)
      s << x[i][j] << separator;
    s << "\n";
  }
}

PPL_OUTPUT_DEFINITIONS_ASCII_ONLY(Bit_Matrix)

bool
PPL::Bit_Matrix::ascii_load(std::istream& s) {
  Bit_Matrix& x = *this;
  dimension_type nrows;
  dimension_type ncols;
  std::string str;
  if (!(s >> nrows))
    return false;
  if (!(s >> str) || str != "x")
    return false;
  if (!(s >> ncols))
    return false;
  resize(nrows, ncols);

  for (dimension_type i = 0; i < num_rows(); ++i)
    for (dimension_type j = 0; j < num_columns(); ++j) {
      int bit;
      if (!(s >> bit))
	return false;
      if (bit)
	x[i].set(j);
      else
	x[i].clear(j);
    }

  // Check invariants.
  assert(OK());
  return true;
}

PPL::memory_size_type
PPL::Bit_Matrix::external_memory_in_bytes() const {
  memory_size_type n = rows.capacity() * sizeof(Row);
  for (dimension_type i = num_rows(); i-- > 0; )
    n += rows[i].external_memory_in_bytes();
  return n;
}

bool
PPL::Bit_Matrix::OK() const {
#ifndef NDEBUG
  using std::endl;
  using std::cerr;
#endif

  const Bit_Matrix& x = *this;
  for (dimension_type i = num_rows(); i-- > 1; ) {
    const Bit_Row& row = x[i];
    if (!row.OK())
      return false;
    else if (row.last() != ULONG_MAX && row.last() >= row_size) {
#ifndef NDEBUG
      cerr << "Bit_Matrix[" << i << "] is a row with too many bits!"
	   << endl
	   << "(row_size == " << row_size
	   << ", row.last() == " << row.last() << ")"
	   << endl;
#endif
      return false;
    }
  }
  return true;
}

#ifndef NDEBUG
bool
PPL::Bit_Matrix::check_sorted() const {
  const Bit_Matrix& x = *this;
  for (dimension_type i = num_rows(); i-- > 1; )
    if (compare(x[i-1], x[i]) > 0)
      return false;
  return true;
}
#endif

/*! \relates Parma_Polyhedra_Library::Bit_Matrix */
bool
PPL::operator==(const Bit_Matrix& x, const Bit_Matrix& y) {
  const dimension_type x_num_rows = x.num_rows();
  if (x_num_rows != y.num_rows()
      || x.num_columns() != y.num_columns())
    return false;
  for (dimension_type i = x_num_rows; i-- > 0; )
    if (x[i] != y[i])
      return false;
  return true;
}

