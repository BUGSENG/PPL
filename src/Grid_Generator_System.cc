/* Grid_Generator_System class implementation (non-inline functions).
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

#include <config.h>

#include "Grid_Generator_System.defs.hh"
#include "Grid_Generator_System.inlines.hh"
#include "Scalar_Products.defs.hh"

#include <cassert>
#include <iostream>

namespace PPL = Parma_Polyhedra_Library;

void
PPL::Grid_Generator_System::recycling_insert(Grid_Generator_System& gs) {
  const dimension_type old_num_rows = num_rows();
  const dimension_type gs_num_rows = gs.num_rows();
  const dimension_type old_num_cols = num_columns();
  const dimension_type gs_num_cols = gs.num_columns();
  if (old_num_cols >= gs_num_cols)
    add_zero_rows(gs_num_rows,
		  Linear_Row::Flags(NECESSARILY_CLOSED,
				    Linear_Row::RAY_OR_POINT_OR_INEQUALITY));
  else {
    add_zero_rows_and_columns(gs_num_rows,
			      gs_num_cols - old_num_cols,
			      Linear_Row::Flags(NECESSARILY_CLOSED,
						Linear_Row::RAY_OR_POINT_OR_INEQUALITY));
    // Swap the parameter divisor column into the new last column.
    swap_columns(old_num_cols - 1, num_columns() - 1);
  }
  set_index_first_pending_row(old_num_rows + gs_num_rows);
  // Swap one coefficient at a time into the newly added rows, instead
  // of swapping each entire row.  This ensures that the added rows
  // have the same capacities as the existing rows.
  for (dimension_type i = gs_num_rows; i-- > 0; )
    operator[](old_num_rows + i).coefficient_swap(gs[i]);
}

void
PPL::Grid_Generator_System::recycling_insert(Grid_Generator& g) {
  dimension_type old_num_rows = num_rows();
  const dimension_type old_num_cols = num_columns();
  const dimension_type g_num_cols = g.size();
  if (old_num_cols >= g_num_cols)
    add_zero_rows(1,
		  Linear_Row::Flags(NECESSARILY_CLOSED,
				    Linear_Row::RAY_OR_POINT_OR_INEQUALITY));
  else {
    add_zero_rows_and_columns(1,
			      g_num_cols - old_num_cols,
			      Linear_Row::Flags(NECESSARILY_CLOSED,
						Linear_Row::RAY_OR_POINT_OR_INEQUALITY));
    // Swap the parameter divisor column into the new last column.
    swap_columns(old_num_cols - 1, num_columns() - 1);
  }
  set_index_first_pending_row(old_num_rows + 1);
  // Swap one coefficient at a time into the newly added rows, instead
  // of swapping each entire row.  This ensures that the added rows
  // have the same capacities as the existing rows.
  operator[](old_num_rows).coefficient_swap(g);
}

void
PPL::Grid_Generator_System::insert(const Grid_Generator& g) {
  dimension_type g_space_dim = g.space_dimension();

  if (g.is_parameter())
    if (g.all_homogeneous_terms_are_zero()) {
      dimension_type initial_space_dim = space_dimension();
      if (initial_space_dim < g_space_dim) {
	// Adjust the space dimension.
	add_zero_columns(g_space_dim - initial_space_dim);
	// Swap the parameter divisor column into the new last column.
	swap_columns(g_space_dim + 1, initial_space_dim + 1);
	assert(OK());
      }
      return;
    }

  {
    // This block is a substitute for Generator_System::insert, in
    // which the single call to Linear_System::insert has been
    // inlined.

    // We are sure that the matrix has no pending rows
    // and that the new row is not a pending generator.
    assert(num_pending_rows() == 0);

    // TODO: Consider whether, if possible, it would be better to wrap
    //       an NNC Generator, storing the generator divisor in the
    //       epsilon column.

    // This is a modified copy of Linear_System::insert.  It is here
    // to force Grid_Generator::OK to be used (to work around the
    // normalization assertions in Linear_System::OK) and so that the
    // parameter divisor column can be moved during the insert.

    // The added row must be strongly normalized and have the same
    // topology as the system.
    assert(topology() == g.topology());
    // This method is only used when the system has no pending rows.
    assert(num_pending_rows() == 0);

    const dimension_type old_num_rows = num_rows();
    const dimension_type old_num_columns = num_columns();
    const dimension_type g_size = g.size();

    // Resize the system, if necessary.
    assert(is_necessarily_closed());
    if (g_size > old_num_columns) {
      add_zero_columns(g_size - old_num_columns);
      if (old_num_rows > 0)
	// Swap the existing parameter divisor column into the new
	// last column.
	swap_columns(old_num_columns - 1, g_size - 1);
      Matrix::add_row(g);
    }
    else if (g_size < old_num_columns)
      if (old_num_rows == 0)
	Matrix::add_row(Linear_Row(g, old_num_columns, row_capacity));
      else {
	// Create a resized copy of the row (and move the parameter
	// divisor coefficient to its last position).
	Linear_Row tmp_row(g, old_num_columns, row_capacity);
	std::swap(tmp_row[g_size - 1], tmp_row[old_num_columns - 1]);
	Matrix::add_row(tmp_row);
      }
    else
      // Here r_size == old_num_columns.
      Matrix::add_row(g);

  } // Generator_System::insert(g) substitute.

  set_index_first_pending_row(num_rows());
  set_sorted(false);

  assert(OK());
}

void
PPL::Grid_Generator_System
::affine_image(dimension_type v,
	       const Linear_Expression& expr,
	       Coefficient_traits::const_reference denominator) {
  // This is mostly a copy of Generator_System::affine_image.

  Grid_Generator_System& x = *this;
  // `v' is the index of a column corresponding to a "user" variable
  // (i.e., it cannot be the inhomogeneous term).
  assert(v > 0 && v <= x.space_dimension());
  assert(expr.space_dimension() <= x.space_dimension());
  assert(denominator > 0);

  const dimension_type n_columns = x.num_columns();
  const dimension_type n_rows = x.num_rows();

  // Compute the numerator of the affine transformation and assign it
  // to the column of `*this' indexed by `v'.
  TEMP_INTEGER(numerator);
  for (dimension_type i = n_rows; i-- > 0; ) {
    Grid_Generator& row = x[i];
    Scalar_Products::assign(numerator, expr, row);
    std::swap(numerator, row[v]);
  }

  if (denominator != 1)
    // Since we want integer elements in the matrix,
    // we multiply by `denominator' all the columns of `*this'
    // having an index different from `v'.
    for (dimension_type i = n_rows; i-- > 0; ) {
      Grid_Generator& row = x[i];
      for (dimension_type j = n_columns; j-- > 0; )
	if (j != v)
	  row[j] *= denominator;
    }

  // If the mapping is not invertible we may have transformed valid
  // lines and rays into the origin of the space.
  const bool not_invertible = (v > expr.space_dimension() || expr[v] == 0);
  if (not_invertible)
    x.remove_invalid_lines_and_rays();
}

PPL_OUTPUT_DEFINITIONS(Grid_Generator_System);

bool
PPL::Grid_Generator_System::ascii_load(std::istream& s) {
  // This is a copy of Generator_System::ascii_load, to force
  // Grid_Generator_System::OK to be called, in order to work around
  // the assertions in Linear_System::OK.

  // FIXME: Gridify this.  Add an ascii_dump to match.

  std::string str;
  if (!(s >> str) || str != "topology")
    return false;
  if (!(s >> str))
    return false;
  if (str == "NECESSARILY_CLOSED")
    set_necessarily_closed();
  else {
    if (str != "NOT_NECESSARILY_CLOSED")
      return false;
    set_not_necessarily_closed();
  }

  dimension_type nrows;
  dimension_type ncols;
  if (!(s >> nrows))
    return false;
  if (!(s >> str))
    return false;
  if (!(s >> ncols))
      return false;
  resize_no_copy(nrows, ncols);

  if (!(s >> str) || (str != "(sorted)" && str != "(not_sorted)"))
    return false;
  set_sorted(str == "(sorted)");
  dimension_type index;
  if (!(s >> str) || str != "index_first_pending")
    return false;
  if (!(s >> index))
    return false;
  set_index_first_pending_row(index);

  Grid_Generator_System& x = *this;
  for (dimension_type i = 0; i < x.num_rows(); ++i) {
    for (dimension_type j = 0; j < x.num_columns(); ++j)
      if (!(s >> const_cast<Coefficient&>(x[i][j])))
	return false;

    if (!(s >> str))
      return false;
    if (str == "L")
      x[i].set_is_line();
    else
      x[i].set_is_ray_or_point();

    // Checking for equality of actual and declared types.
    switch (x[i].type()) {
    case Grid_Generator::LINE:
      if (str == "L")
	continue;
      break;
    case Grid_Generator::PARAMETER:
      if (str == "R")
	continue;
      break;
    case Grid_Generator::POINT:
      if (str == "P")
	continue;
      break;
    }
    // Reaching this point means that the input was illegal.
    return false;
  }

  // Checking for well-formedness.

  assert(OK());
  return true;
}

bool
PPL::Grid_Generator_System::OK() const {
  if (topology() == NOT_NECESSARILY_CLOSED) {
#ifndef NDEBUG
    std::cerr << "Grid_Generator_System is NOT_NECESSARILY_CLOSED"
	      << std::endl;
#endif
    return false;
  }

  if (is_sorted()) {
#ifndef NDEBUG
    std::cerr << "Grid_Generator_System is marked as sorted."
	      << std::endl;
#endif
    return false;
  }

  // A Generator_System and hence a Grid_Generator_System must be a
  // valid Linear_System; do not check for strong normalization, since
  // this will be done when checking each individual generator.
  if (!Linear_System::OK(false))
    return false;

  // Checking each generator in the system.
  const Grid_Generator_System& x = *this;
  for (dimension_type i = num_rows(); i-- > 0; )
    if (!x[i].OK())
      return false;

  // All checks passed.
  return true;
}

/*! \relates Parma_Polyhedra_Library::Grid_Generator_System */
std::ostream&
PPL::IO_Operators::operator<<(std::ostream& s,
			      const Grid_Generator_System& gs) {
  Grid_Generator_System::const_iterator i = gs.begin();
  const Grid_Generator_System::const_iterator gs_end = gs.end();
  if (i == gs_end)
    return s << "false";
  while (true) {
    s << *i++;
    if (i == gs_end)
      return s;
    s << ", ";
  }
}

void
PPL::Grid_Generator_System
::add_universe_rows_and_columns(dimension_type dims) {
  assert(num_columns() > 0);
  dimension_type col = num_columns() - 1;
  add_zero_rows_and_columns(dims, dims,
			    Linear_Row::Flags(NECESSARILY_CLOSED,
					      Linear_Row::LINE_OR_EQUALITY));
  unset_pending_rows();
  // Swap the parameter divisor column into the new last column.
  swap_columns(col, col + dims);
  // Set the diagonal element of each added rows.
  dimension_type rows = num_rows();
  for (dimension_type row = rows - dims; row < rows; ++row, ++col)
    const_cast<Coefficient&>(operator[](row)[col]) = 1;
}

void
PPL::Grid_Generator_System
::remove_space_dimensions(const Variables_Set& to_be_removed) {
  // The removal of no dimensions from any system is a no-op.  This
  // case also captures the only legal removal of dimensions from a
  // 0-dim system.
  if (to_be_removed.empty())
    return;

  // Dimension-compatibility check: the variable having maximum space
  // dimension is the one occurring last in the set.
  const dimension_type
    min_space_dim = to_be_removed.rbegin()->space_dimension();
  if (space_dimension() < min_space_dim) {
    std::ostringstream s;
    s << "PPL::Grid_Generator_System::remove_space_dimensions(vs):\n"
      << "this->space_dimension() == " << space_dimension()
      << ", required space dimension == " << min_space_dim << ".";
    throw std::invalid_argument(s.str());
  }

  // For each variable to be removed, replace the corresponding column
  // by shifting left the columns to the right that will be kept.
  Variables_Set::const_iterator tbr = to_be_removed.begin();
  Variables_Set::const_iterator tbr_end = to_be_removed.end();
  dimension_type dst_col = tbr->space_dimension();
  dimension_type src_col = dst_col + 1;
  for (++tbr; tbr != tbr_end; ++tbr) {
    dimension_type tbr_col = tbr->space_dimension();
    // Move all columns in between to the left.
    while (src_col < tbr_col)
      // FIXME: consider whether Linear_System must have a swap_columns()
      // method.  If the answer is "no", remove this Matrix:: qualification.
      Matrix::swap_columns(dst_col++, src_col++);
    ++src_col;
  }
  // Move any remaining columns.
  const dimension_type num_cols = num_columns();
  while (src_col < num_cols)
    // FIXME: consider whether Linear_System must have a swap_columns()
    // method.  If the answer is "no", remove this Matrix:: qualification.
    Matrix::swap_columns(dst_col++, src_col++);

  // The number of remaining columns is `dst_col'.
  Matrix::remove_trailing_columns(num_cols - dst_col);



  remove_invalid_lines_and_rays();
}

void
PPL::Grid_Generator_System
::remove_higher_space_dimensions(dimension_type new_dimension) {
  dimension_type space_dim = space_dimension();
  // Dimension-compatibility check.
  if (new_dimension > space_dim) {
    std::ostringstream s;
    s << "PPL::Grid_Generator_System::remove_higher_space_dimensions(n):\n"
      << "this->space_dimension() == " << space_dim
      << ", required space dimension == " << new_dimension << ".";
    throw std::invalid_argument(s.str());
  }

  // The removal of no dimensions from any system is a no-op.  Note
  // that this case also captures the only legal removal of dimensions
  // from a system in a 0-dim space.
  if (new_dimension == space_dim)
    return;

  if (new_dimension == 0)
    clear();
  else {
    // Swap the parameter divisor column into the column that will
    // become the last column.
    swap_columns(new_dimension + 1, space_dim + 1);
    Matrix::remove_trailing_columns(space_dim - new_dimension);
    remove_invalid_lines_and_rays();
  }
}
