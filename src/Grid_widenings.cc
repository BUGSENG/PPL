/* Grid class implementation
   (non-inline widening-related member functions).
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

#include "Grid.defs.hh"

#include <cassert>
#include <iostream>

namespace PPL = Parma_Polyhedra_Library;

void
PPL::Grid::select_wider_congruences(const Grid& y,
				    Congruence_System& cgs_selected) const {
  // Private method: the caller must ensure the following conditions
  // (beside the inclusion `y <= x').
  assert(space_dim == y.space_dim);
  assert(!marked_empty());
  assert(!y.marked_empty());
  assert(congruences_are_minimized());
  assert(y.congruences_are_minimized());

  // Note: row counters start at 0, to preserve the original order in
  // the selected congruences.
  for (dimension_type dim = con_sys.space_dimension(), x_row = 0, y_row = 0;
       dim > 0; --dim) {
    assert(dim_kinds[dim] == CON_VIRTUAL
	   || dim_kinds[dim] == y.dim_kinds[dim]);
    switch (dim_kinds[dim]) {
    case PROPER_CONGRUENCE:
      {
	const Congruence& cg = con_sys[x_row];
	const Congruence& y_cg = y.con_sys[y_row];
	if (cg.is_equal_at_dimension(dim, y_cg))
	  // The leading diagonal entries are equal.
	  cgs_selected.insert(cg);
	++x_row;
	++y_row;
      }
      break;
    case EQUALITY:
      cgs_selected.insert(con_sys[x_row]);
      ++x_row;
      ++y_row;
      break;
    case CON_VIRTUAL:
      y.dim_kinds[dim] == CON_VIRTUAL || ++y_row;
      break;
    }
  }
}

void
PPL::Grid::congruence_widening_assign(const Grid& const_y, unsigned* tp) {
  Grid& x = *this;
  Grid& y = const_cast<Grid&>(const_y);
  // Dimension-compatibility check.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("widening_assign(y)", "y", y);

  // Stable behavior is only guaranteed if y is contained in or equal
  // to x.
#ifndef NDEBUG
  {
    // Assume y is contained in or equal to x.
    const Grid x_copy = x;
    const Grid y_copy = y;
    assert(x_copy.contains(y_copy));
  }
#endif

  // Leave `x' the same if `x' or `y' is zero-dimensional or empty.
  if (x.space_dim == 0 || x.marked_empty() || y.marked_empty())
    return;

  // Ensure that the `x' congruences are in minimal form.
  if (x.congruences_are_up_to_date()) {
    if (!x.congruences_are_minimized()) {
      if (simplify(x.con_sys, x.dim_kinds)) {
	// `x' is empty.
	x.set_empty();
	return;
      }
      x.set_congruences_minimized();
    }
  }
  else
    x.update_congruences();

  // Ensure that the `y' congruences are in minimal form.
  if (y.congruences_are_up_to_date()) {
    if (!y.congruences_are_minimized()) {
      if (simplify(y.con_sys, y.dim_kinds)) {
	// `y' is empty.
	y.set_empty();
	return;
      }
      y.set_congruences_minimized();
    }
  }
  else
    y.update_congruences();

  if (con_sys.num_equalities() < y.con_sys.num_equalities())
    return;

  // Copy into `cgs' the congruences of `x' that are common to `y',
  // according to the grid widening.
  Congruence_System cgs;
  x.select_wider_congruences(y, cgs);

  if (cgs.num_rows() == con_sys.num_rows())
    // All congruences were selected, thus the result is `x'.
    return;

  // A strict subset of the congruences was selected.

  Grid result(x.space_dim);
  result.add_recycled_congruences(cgs);

  // Check whether we are using the widening-with-tokens technique
  // and there are still tokens available.
  if (tp && *tp > 0) {
    // There are tokens available.  If `result' is not a subset of
    // `x', then it is less precise and we use one of the available
    // tokens.
    if (!x.contains(result))
      --(*tp);
  }
  else
    // No tokens.
    std::swap(x, result);

  assert(x.OK(true));
}

void
PPL::Grid::limited_congruence_extrapolation_assign(const Grid& y,
						   const Congruence_System& cgs,
						   unsigned* tp) {
  Grid& x = *this;

  // Check dimension compatibility.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("limited_extrapolation_assign(y, cgs)",
				 "y", y);
  // `cgs' must be dimension-compatible with the two grids.
  const dimension_type cgs_space_dim = cgs.space_dimension();
  if (x.space_dim < cgs_space_dim)
    throw_dimension_incompatible("limited_extrapolation_assign(y, cgs)",
				 "cgs", cgs);

  const dimension_type cgs_num_rows = cgs.num_rows();
  // If `cgs' is empty (of rows), fall back to ordinary widening.
  if (cgs_num_rows == 0) {
    x.widening_assign(y, tp);
    return;
  }

#ifndef NDEBUG
  {
    // Assume that y is contained in or equal to x.
    const Grid x_copy = x;
    const Grid y_copy = y;
    assert(x_copy.contains(y_copy));
  }
#endif

  if (y.marked_empty())
    return;
  if (x.marked_empty())
    return;

  // The limited widening between two grids in a zero-dimensional
  // space is also a grid in a zero-dimensional space.
  if (x.space_dim == 0)
    return;

  // Update the generators of `x': these are used to select, from the
  // congruences in `cgs', those that must be added to the widened
  // grid.
  if (!x.generators_are_up_to_date() && !x.update_generators())
    // `x' is empty.
    return;

  if (tp == NULL || *tp == 0) {
    // Widening may change the grid, so add the congruences.
    Congruence_System new_cgs;
    // The congruences to be added need only be satisfied by all the
    // generators of `x', as `y <= x'.  Iterate upwards here, to keep
    // the relative ordering of congruences (just for aesthetics).
    for (dimension_type i = 0; i < cgs_num_rows; ++i) {
      const Congruence& cg = cgs[i];
      if (x.relation_with(cg) == Poly_Con_Relation::is_included())
	new_cgs.insert(cg);
    }
    x.congruence_widening_assign(y, tp);
    x.add_recycled_congruences(new_cgs);
  }
  else
    // There are tokens, so widening will leave the grid the same.
    x.congruence_widening_assign(y, tp);

  assert(OK());
}

void
PPL::Grid::select_wider_generators(const Grid& y,
				   Grid_Generator_System& ggs_selected) const {
  // Private method: the caller must ensure the following conditions
  // (beside the inclusion `y <= x').
  assert(space_dim == y.space_dim);
  assert(!marked_empty());
  assert(!y.marked_empty());
  assert(generators_are_minimized());
  assert(y.generators_are_minimized());

  // Note: row counters start at 0, to preserve the original order in
  // the selected generators.
  for (dimension_type dim = 0, x_row = 0, y_row = 0;
       dim <= gen_sys.space_dimension(); ++dim) {
    assert(dim_kinds[dim] == LINE
           || y.dim_kinds[dim] == GEN_VIRTUAL
	   || dim_kinds[dim] == y.dim_kinds[dim]);
    switch (dim_kinds[dim]) {
    case PARAMETER:
      {
	const Grid_Generator& gg = gen_sys[x_row];
	const Grid_Generator& y_gg = y.gen_sys[y_row];
	if (gg.is_equal_at_dimension(dim, y_gg))
	  // The leading diagonal entry is equal.
	  ggs_selected.insert(gg);
        else {
          Linear_Expression e;
          for (dimension_type i = gg.space_dimension(); i-- > 0; )
            e += gg.coefficient(Variable(i)) * Variable(i);
          ggs_selected.insert(grid_line(e));
	}
	++x_row;
	++y_row;
      }
      break;
    case LINE:
      ggs_selected.insert(gen_sys[x_row]);
      ++x_row;
      ++y_row;
      break;
    case GEN_VIRTUAL:
      y.dim_kinds[dim] == GEN_VIRTUAL || ++y_row;
      break;
    }
  }
}

void
PPL::Grid::generator_widening_assign(const Grid& const_y, unsigned* tp) {
  Grid& x = *this;
  Grid& y = const_cast<Grid&>(const_y);
  // Dimension-compatibility check.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("generator_widening_assign(y)", "y", y);

  // Stable behavior is only guaranteed if y is contained in or equal
  // to x.
#ifndef NDEBUG
  {
    // Assume y is contained in or equal to x.
    const Grid x_copy = x;
    const Grid y_copy = y;
    assert(x_copy.contains(y_copy));
  }
#endif

  // Leave `x' the same if `x' or `y' is zero-dimensional or empty.
  if (x.space_dim == 0 || x.marked_empty() || y.marked_empty())
    return;

  // Ensure that the `x' generators are in minimal form.
  if (x.generators_are_up_to_date()) {
    if (!x.generators_are_minimized()) {
      simplify(x.gen_sys, x.dim_kinds);
      assert(!x.gen_sys.has_no_rows());
      x.set_generators_minimized();
    }
  }
  else
    x.update_generators();

  if (x.marked_empty())
    return;

  // Ensure that the `y' generators are in minimal form.
  if (y.generators_are_up_to_date()) {
    if (!y.generators_are_minimized()) {
      simplify(y.gen_sys, y.dim_kinds);
      assert(!y.gen_sys.has_no_rows());
      y.set_generators_minimized();
    }
  }
  else
    y.update_generators();

  if (gen_sys.num_rows() > y.gen_sys.num_rows())
    return;

  if (gen_sys.num_lines() > y.gen_sys.num_lines())
    return;

  // Copy into `ggs' the generators of `x' that are common to `y',
  // according to the grid widening.
  Grid_Generator_System ggs;
  x.select_wider_generators(y, ggs);

  if (ggs.num_parameters() == gen_sys.num_parameters())
    // All parameters are kept as parameters, thus the result is `x'.
    return;

  // A strict subset of the parameters was selected.

  Grid result(x.space_dim, EMPTY);
  result.add_recycled_grid_generators(ggs);

  // Check whether we are using the widening-with-tokens technique
  // and there are still tokens available.
  if (tp && *tp > 0) {
    // There are tokens available.  If `result' is not a subset of
    // `x', then it is less precise and we use one of the available
    // tokens.
    if (!x.contains(result))
      --(*tp);
  }
  else
    // No tokens.
    std::swap(x, result);

  assert(x.OK(true));
}

void
PPL::Grid::limited_generator_extrapolation_assign(const Grid& y,
						  const Congruence_System& cgs,
						  unsigned* tp) {
  Grid& x = *this;

  // Check dimension compatibility.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("limited_extrapolation_assign(y, cgs)",
				 "y", y);
  // `cgs' must be dimension-compatible with the two grids.
  const dimension_type cgs_space_dim = cgs.space_dimension();
  if (x.space_dim < cgs_space_dim)
    throw_dimension_incompatible("limited_extrapolation_assign(y, cgs)",
				 "cgs", cgs);

  const dimension_type cgs_num_rows = cgs.num_rows();
  // If `cgs' is empty (of rows), fall back to ordinary widening.
  if (cgs_num_rows == 0) {
    x.generator_widening_assign(y, tp);
    return;
  }

#ifndef NDEBUG
  {
    // Assume that y is contained in or equal to x.
    const Grid x_copy = x;
    const Grid y_copy = y;
    assert(x_copy.contains(y_copy));
  }
#endif

  if (y.marked_empty())
    return;
  if (x.marked_empty())
    return;

  // The limited widening between two grids in a zero-dimensional
  // space is also a grid in a zero-dimensional space.
  if (x.space_dim == 0)
    return;

  // Update the generators of `x': these are used to select, from the
  // congruences in `cgs', those that must be added to the widened
  // grid.
  if (!x.generators_are_up_to_date() && !x.update_generators())
    // `x' is empty.
    return;

  if (tp == NULL || *tp == 0) {
    // Widening may change the grid, so add the congruences.
    Congruence_System new_cgs;
    // The congruences to be added need only be satisfied by all the
    // generators of `x', as `y <= x'.  Iterate upwards here, to keep
    // the relative ordering of congruences (just for aesthetics).
    for (dimension_type i = 0; i < cgs_num_rows; ++i) {
      const Congruence& cg = cgs[i];
      if (x.relation_with(cg) == Poly_Con_Relation::is_included())
	new_cgs.insert(cg);
    }
    x.generator_widening_assign(y, tp);
    x.add_recycled_congruences(new_cgs);
  }
  else
    // There are tokens, so widening will leave the grid the same.
    x.generator_widening_assign(y, tp);

  assert(OK());
}

void
PPL::Grid::widening_assign(const Grid& const_y, unsigned* tp) {
  Grid& x = *this;
  Grid& y = const_cast<Grid&>(const_y);
  // Dimension-compatibility check.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("widening_assign(y)", "y", y);

  // Stable behavior is only guaranteed if y is contained in or equal
  // to x.
#ifndef NDEBUG
  {
    // Assume y is contained in or equal to x.
    const Grid x_copy = x;
    const Grid y_copy = y;
    assert(x_copy.contains(y_copy));
  }
#endif

  // If the `x' congruences are up to date and `y' congruences are up
  // to date use the congruence widening.
  if (x.congruences_are_up_to_date() && y.congruences_are_up_to_date()) {
    x.congruence_widening_assign(y, tp);
    return;
  }

  // If the `x' generators are up to date and `y' generators are up to
  // date use the generator widening.
  if (x.generators_are_up_to_date() && y.generators_are_up_to_date()) {
    x.generator_widening_assign(y, tp);
    return;
  }

  x.congruence_widening_assign(y, tp);
}

void
PPL::Grid::limited_extrapolation_assign(const Grid& y,
					const Congruence_System& cgs,
					unsigned* tp) {
  Grid& x = *this;

  // Check dimension compatibility.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("limited_extrapolation_assign(y, cgs)",
				 "y", y);
  // `cgs' must be dimension-compatible with the two grids.
  const dimension_type cgs_space_dim = cgs.space_dimension();
  if (x.space_dim < cgs_space_dim)
    throw_dimension_incompatible("limited_extrapolation_assign(y, cgs)",
				 "cgs", cgs);

  const dimension_type cgs_num_rows = cgs.num_rows();
  // If `cgs' is empty (of rows), fall back to ordinary widening.
  if (cgs_num_rows == 0) {
    x.widening_assign(y, tp);
    return;
  }

#ifndef NDEBUG
  {
    // Assume that y is contained in or equal to x.
    const Grid x_copy = x;
    const Grid y_copy = y;
    assert(x_copy.contains(y_copy));
  }
#endif

  if (y.marked_empty())
    return;
  if (x.marked_empty())
    return;

  // The limited widening between two grids in a zero-dimensional
  // space is also a grid in a zero-dimensional space.
  if (x.space_dim == 0)
    return;

  // Update the generators of `x': these are used to select, from the
  // congruences in `cgs', those that must be added to the widened
  // grid.
  if (!x.generators_are_up_to_date() && !x.update_generators())
    // `x' is empty.
    return;

  if (tp == NULL || *tp == 0) {
    // Widening may change the grid, so add the congruences.
    Congruence_System new_cgs;
    // The congruences to be added need only be satisfied by all the
    // generators of `x', as `y <= x'.  Iterate upwards here, to keep
    // the relative ordering of congruences (just for aesthetics).
    for (dimension_type i = 0; i < cgs_num_rows; ++i) {
      const Congruence& cg = cgs[i];
      if (x.relation_with(cg) == Poly_Con_Relation::is_included())
	new_cgs.insert(cg);
    }
    x.widening_assign(y, tp);
    x.add_recycled_congruences(new_cgs);
  }
  else
    // There are tokens, so widening will leave the grid the same.
    x.widening_assign(y, tp);

  assert(OK());
}
