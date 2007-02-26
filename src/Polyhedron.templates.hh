/* Polyhedron class implementation: non-inline template functions.
   Copyright (C) 2001-2007 Roberto Bagnara <bagnara@cs.unipr.it>

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

#ifndef PPL_Polyhedron_templates_hh
#define PPL_Polyhedron_templates_hh 1

#include "Generator.defs.hh"
#include "Old_Interval.defs.hh"
#include "MIP_Problem.defs.hh"
#include <algorithm>
#include <deque>

namespace Parma_Polyhedra_Library {

template <typename Box>
Polyhedron::Polyhedron(Topology topol, const Box& box)
  : con_sys(topol),
    gen_sys(topol),
    sat_c(),
    sat_g() {
  // Initialize the space dimension as indicated by the box.
  space_dim = box.space_dimension();

  // Check for emptiness.
  if (box.is_empty()) {
    set_empty();
    return;
  }

  // Zero-dim universe polyhedron.
  if (space_dim == 0) {
    set_zero_dim_univ();
    return;
  }

  // Insert a dummy constraint of the highest dimension to avoid the
  // need of resizing the matrix of constraints later;
  // this constraint will be removed at the end.
  con_sys.insert(Variable(space_dim - 1) >= 0);

  for (dimension_type k = space_dim; k-- > 0; ) {
    // See if we have a valid lower bound.
    bool l_closed = false;
    TEMP_INTEGER(l_n);
    TEMP_INTEGER(l_d);
    bool l_bounded = box.get_lower_bound(k, l_closed, l_n, l_d);
    if (l_bounded && topol == NECESSARILY_CLOSED && !l_closed)
      throw_invalid_argument("C_Polyhedron(const Box& box):",
			     " box has an open lower bound");
    // See if we have a valid upper bound.
    bool u_closed = false;
    TEMP_INTEGER(u_n);
    TEMP_INTEGER(u_d);
    bool u_bounded = box.get_upper_bound(k, u_closed, u_n, u_d);
    if (u_bounded && topol == NECESSARILY_CLOSED && !u_closed)
      throw_invalid_argument("C_Polyhedron(const Box& box):",
			     " box has an open upper bound");

    // See if we have an implicit equality constraint.
    if (l_bounded && u_bounded
	&& l_closed && u_closed
	&& l_n == u_n && l_d == u_d) {
      // Add the constraint `l_d*v_k == l_n'.
      con_sys.insert(l_d * Variable(k) == l_n);
    }
    else {
      // Check if a lower bound constraint is required.
      if (l_bounded) {
       if (l_closed)
	 // Add the constraint `l_d*v_k >= l_n'.
	 con_sys.insert(l_d * Variable(k) >= l_n);
       else
	 // Add the constraint `l_d*v_k > l_n'.
	 con_sys.insert(l_d * Variable(k) > l_n);
      }
      // Check if an upper bound constraint is required.
      if (u_bounded) {
       if (u_closed)
	 // Add the constraint `u_d*v_k <= u_n'.
	 con_sys.insert(u_d * Variable(k) <= u_n);
       else
	 // Add the constraint `u_d*v_k < u_n'.
	 con_sys.insert(u_d * Variable(k) < u_n);
      }
    }
  }

  // Adding the low-level constraints.
  con_sys.add_low_level_constraints();
  // Now removing the dummy constraint inserted before.
  dimension_type n_rows = con_sys.num_rows() - 1;
  con_sys[0].swap(con_sys[n_rows]);
  con_sys.set_sorted(false);
  // NOTE: here there are no pending constraints.
  con_sys.set_index_first_pending_row(n_rows);
  con_sys.erase_to_end(n_rows);

  // Constraints are up-to-date.
  set_constraints_up_to_date();
  assert(OK());
}

template <typename Box>
void
Polyhedron::shrink_bounding_box(Box& box, Complexity_Class complexity) const {
  bool reduce_complexity = (complexity != ANY_COMPLEXITY);
  if (!reduce_complexity
      || (!has_something_pending() && constraints_are_minimized())) {
    // If the constraint system is minimized, the test `is_universe()'
    // is not exponential.
    if (is_universe())
      return;
  }
  if (reduce_complexity) {
    if (marked_empty()
	|| (generators_are_up_to_date() && gen_sys.num_rows() == 0)) {
      box.set_empty();
      return;
    }
    else if (constraints_are_up_to_date()) {
      // See if there is at least one inconsistent constraint in `con_sys'.
      for (Constraint_System::const_iterator i = con_sys.begin(),
	     cs_end = con_sys.end(); i != cs_end; ++i)
	if (i->is_inconsistent()) {
	  box.set_empty();
	  return;
	}
      // If `complexity' allows it, use the MIP_Problem solver to determine
      // whether or not the polyhedron is empty.
      if (complexity == SIMPLEX_COMPLEXITY
	  // TODO: find a workaround for NNC polyhedra.
	  && is_necessarily_closed()) {
	MIP_Problem lp(con_sys.space_dimension(), con_sys);
	if (!lp.is_satisfiable()) {
	  box.set_empty();
	  return;
	}
      }
    }
  }
  else
    // The flag `reduce_complexity' is `false'.
    // Note that the test `is_empty()' is exponential in the worst case.
    if (is_empty()) {
      box.set_empty();
      return;
    }

  if (space_dim == 0)
    return;

  // The following vectors will store the lower and upper bound
  // for each dimension.
  // Lower bounds are initialized to open plus infinity.
  std::vector<LBoundary>
    lower_bound(space_dim,
		LBoundary(ERational(PLUS_INFINITY), LBoundary::OPEN));
  // Upper bounds are initialized to open minus infinity.
  std::vector<UBoundary>
    upper_bound(space_dim,
		UBoundary(ERational(MINUS_INFINITY), UBoundary::OPEN));

  if (!reduce_complexity && has_something_pending())
    process_pending();

  // TODO: use simplex to derive variable bounds, if the complexity
  // is SIMPLEX_COMPLEXITY.

  if (reduce_complexity &&
       (!generators_are_up_to_date() || has_pending_constraints())) {
    // Extract easy-to-find bounds from constraints.
    assert(constraints_are_up_to_date());

    // We must copy `con_sys' to a temporary matrix,
    // as we need to simplify all of the matrix
    // (not just the non-pending part of it).
    Constraint_System cs(con_sys);
    if (cs.num_pending_rows() > 0)
      cs.unset_pending_rows();
    if (has_pending_constraints() || !constraints_are_minimized())
      cs.simplify();

    const Constraint_System::const_iterator cs_begin = cs.begin();
    const Constraint_System::const_iterator cs_end = cs.end();

    for (Constraint_System::const_iterator i = cs_begin; i != cs_end; ++i) {
      dimension_type varid = space_dim;
      const Constraint& c = *i;
      // After `simplify()' some constraints may have become inconsistent.
      if (c.is_inconsistent()) {
	box.set_empty();
	return;
      }
      for (dimension_type j = space_dim; j-- > 0; ) {
	// We look for constraints of the form `Variable(j) == k',
	// `Variable(j) >= k', and `Variable(j) > k'.
	if (c.coefficient(Variable(j)) != 0)
	  if (varid != space_dim) {
	    varid = space_dim;
	    break;
	  }
	  else
	    varid = j;
      }
      if (varid != space_dim) {
	const Coefficient& d = c.coefficient(Variable(varid));
	const Coefficient& n = c.inhomogeneous_term();
	// The constraint `c' is of the form
	// `Variable(varid) + n / d rel 0', where
	// `rel' is either the relation `==', `>=', or `>'.
	// For the purpose of shrinking intervals, this is
	// (morally) turned into `Variable(varid) rel -n/d'.
	mpq_class q;
	assign_r(q.get_num(), n, ROUND_NOT_NEEDED);
	assign_r(q.get_den(), d, ROUND_NOT_NEEDED);
	q.canonicalize();
	// Turn `n/d' into `-n/d'.
	q = -q;
	const ERational r(q, ROUND_NOT_NEEDED);
	const Constraint::Type c_type = c.type();
	switch (c_type) {
	case Constraint::EQUALITY:
	  lower_bound[varid] = LBoundary(r, LBoundary::CLOSED);
	  upper_bound[varid] = UBoundary(r, UBoundary::CLOSED);
	  break;
	case Constraint::NONSTRICT_INEQUALITY:
	case Constraint::STRICT_INEQUALITY:
	  if (d > 0)
	  // If `d' is strictly positive, we have a constraint of the
	  // form `Variable(varid) >= k' or `Variable(varid) > k'.
	    lower_bound[varid]
	      = LBoundary(r, (c_type == Constraint::NONSTRICT_INEQUALITY
			      ? LBoundary::CLOSED
			      : LBoundary::OPEN));
	  else {
	    // Otherwise, we are sure that `d' is strictly negative
	    // and, in this case, we have a constraint of the form
	    // `Variable(varid) <= k' or `Variable(varid) < k'.
	    assert(d < 0);
	    upper_bound[varid]
	      = UBoundary(r, (c_type == Constraint::NONSTRICT_INEQUALITY
			      ? UBoundary::CLOSED
			      : UBoundary::OPEN));
	  }
	  break;
	}
      }
    }
  }
  else {
    // We are in the case where either the generators are up-to-date
    // or reduced complexity is not required.
    // Get the generators for *this.

    // We have not to copy `gen_sys', because in this case
    // we only read the generators.
    const Generator_System& gs = gen_sys;

    // We first need to identify those axes that are unbounded below
    // and/or above.
    for (Generator_System::const_iterator i = gs.begin(),
	   gs_end = gs.end(); i != gs_end; ++i) {
      // Note: using an iterator, we read also the pending part of the matrix.
      const Generator& g = *i;
      Generator::Type g_type = g.type();
      switch (g_type) {
      case Generator::LINE:
	// Any axes `j' in which the coefficient is non-zero is unbounded
	// both below and above.
	for (dimension_type j = space_dim; j-- > 0; )
	  if (g.coefficient(Variable(j)) != 0) {
	    lower_bound[j] = LBoundary(ERational(MINUS_INFINITY),
				       LBoundary::OPEN);
	    upper_bound[j] = UBoundary(ERational(PLUS_INFINITY),
				       UBoundary::OPEN);
	  }
	break;
      case Generator::RAY:
	// Axes in which the coefficient is negative are unbounded below.
	// Axes in which the coefficient is positive are unbounded above.
	for (dimension_type j = space_dim; j-- > 0; ) {
	  int sign = sgn(g.coefficient(Variable(j)));
	  if (sign < 0)
	    lower_bound[j] = LBoundary(ERational(MINUS_INFINITY),
				       LBoundary::OPEN);
	  else if (sign > 0)
	    upper_bound[j] = UBoundary(ERational(PLUS_INFINITY),
				       UBoundary::OPEN);
	}
	break;
      case Generator::POINT:
      case Generator::CLOSURE_POINT:
	{
	  const Coefficient& d = g.divisor();
	  for (dimension_type j = space_dim; j-- > 0; ) {
	    const Coefficient& n = g.coefficient(Variable(j));
	    mpq_class q;
	    assign_r(q.get_num(), n, ROUND_NOT_NEEDED);
	    assign_r(q.get_den(), d, ROUND_NOT_NEEDED);
	    q.canonicalize();
	    const ERational r(q, ROUND_NOT_NEEDED);
	    LBoundary lb(r,(g_type == Generator::CLOSURE_POINT
			    ? LBoundary::OPEN
			    : LBoundary::CLOSED));
	    if (lb < lower_bound[j])
	      lower_bound[j] = lb;
	    UBoundary ub(r, (g_type == Generator::CLOSURE_POINT
			     ? UBoundary::OPEN
			     : UBoundary::CLOSED));
	    if (ub > upper_bound[j])
	      upper_bound[j] = ub;
	  }
	}
	break;
      }
    }
  }

  TEMP_INTEGER(n);
  TEMP_INTEGER(d);

  // Now shrink the bounded axes.
  for (dimension_type j = space_dim; j-- > 0; ) {
    // Lower bound.
    const LBoundary& lb = lower_bound[j];
    const ERational& lr = lb.bound();
    if (!is_plus_infinity(lr) && !is_minus_infinity(lr)) {
      n = raw_value(lr).get_num();
      d = raw_value(lr).get_den();
      box.raise_lower_bound(j, lb.is_closed(), n, d);
    }

    // Upper bound.
    const UBoundary& ub = upper_bound[j];
    const ERational& ur = ub.bound();
    if (!is_plus_infinity(ur) && !is_minus_infinity(ur)) {
      n = raw_value(ur).get_num();
      d = raw_value(ur).get_den();
      box.lower_upper_bound(j, ub.is_closed(), n, d);
    }
  }
}

template <typename Partial_Function>
void
Polyhedron::map_space_dimensions(const Partial_Function& pfunc) {
  if (space_dim == 0)
    return;

  if (pfunc.has_empty_codomain()) {
    // All dimensions vanish: the polyhedron becomes zero_dimensional.
    if (marked_empty()
	|| (has_pending_constraints()
	    && !remove_pending_to_obtain_generators())
	|| (!generators_are_up_to_date() && !update_generators())) {
      // Removing all dimensions from the empty polyhedron.
      space_dim = 0;
      con_sys.clear();
    }
    else
      // Removing all dimensions from a non-empty polyhedron.
      set_zero_dim_univ();

    assert(OK());
    return;
  }

  const dimension_type new_space_dimension = pfunc.max_in_codomain() + 1;

  if (new_space_dimension == space_dim) {
    // The partial function `pfunc' is indeed total and thus specifies
    // a permutation, that is, a renaming of the dimensions.  For
    // maximum efficiency, we will simply permute the columns of the
    // constraint system and/or the generator system.

    // We first compute suitable permutation cycles for the columns of
    // the `con_sys' and `gen_sys' matrices.  We will represent them
    // with a linear array, using 0 as a terminator for each cycle
    // (notice that the columns with index 0 of `con_sys' and
    // `gen_sys' represent the inhomogeneous terms, and thus are
    // unaffected by the permutation of dimensions).
    // Cycles of length 1 will be omitted so that, in the worst case,
    // we will have `space_dim' elements organized in `space_dim/2'
    // cycles, which means we will have at most `space_dim/2'
    // terminators.
    std::vector<dimension_type> cycles;
    cycles.reserve(space_dim + space_dim/2);

    // Used to mark elements as soon as they are inserted in a cycle.
    std::deque<bool> visited(space_dim);

    for (dimension_type i = space_dim; i-- > 0; ) {
      if (!visited[i]) {
	dimension_type j = i;
	do {
	  visited[j] = true;
	  // The following initialization is only to make the compiler happy.
	  dimension_type k = 0;
	  if (!pfunc.maps(j, k))
	    throw_invalid_argument("map_space_dimensions(pfunc)",
				   " pfunc is inconsistent");
	  if (k == j)
	    // Cycle of length 1: skip it.
	    goto skip;

	  cycles.push_back(j+1);
	  // Go along the cycle.
	  j = k;
	} while (!visited[j]);
	// End of cycle: mark it.
	cycles.push_back(0);
      skip:
	;
      }
    }

    // If `cycles' is empty then `pfunc' is the identity.
    if (cycles.empty())
      return;

    // Permute all that is up-to-date.  Notice that the contents of
    // the saturation matrices is unaffected by the permutation of
    // columns: they remain valid, if they were so.
    if (constraints_are_up_to_date())
      con_sys.permute_columns(cycles);

    if (generators_are_up_to_date())
      gen_sys.permute_columns(cycles);

    assert(OK());
    return;
  }

  // If control gets here, then `pfunc' is not a permutation and some
  // dimensions must be projected away.

  // If there are pending constraints, using `generators()' we process them.
  const Generator_System& old_gensys = generators();

  if (old_gensys.num_rows() == 0) {
    // The polyhedron is empty.
    Polyhedron new_polyhedron(topology(), new_space_dimension, EMPTY);
    std::swap(*this, new_polyhedron);
    assert(OK());
    return;
  }

  // Make a local copy of the partial function.
  std::vector<dimension_type> pfunc_maps(space_dim, not_a_dimension());
  for (dimension_type j = space_dim; j-- > 0; ) {
    dimension_type pfunc_j;
    if (pfunc.maps(j, pfunc_j))
      pfunc_maps[j] = pfunc_j;
  }

  Generator_System new_gensys;
  for (Generator_System::const_iterator i = old_gensys.begin(),
	 old_gensys_end = old_gensys.end(); i != old_gensys_end; ++i) {
    const Generator& old_g = *i;
    Linear_Expression e(0 * Variable(new_space_dimension-1));
    bool all_zeroes = true;
    for (dimension_type j = space_dim; j-- > 0; ) {
      if (old_g.coefficient(Variable(j)) != 0
	  && pfunc_maps[j] != not_a_dimension()) {
	e += Variable(pfunc_maps[j]) * old_g.coefficient(Variable(j));
	all_zeroes = false;
      }
    }
    switch (old_g.type()) {
    case Generator::LINE:
      if (!all_zeroes)
	new_gensys.insert(line(e));
      break;
    case Generator::RAY:
      if (!all_zeroes)
	new_gensys.insert(ray(e));
      break;
    case Generator::POINT:
      // A point in the origin has all zero homogeneous coefficients.
      new_gensys.insert(point(e, old_g.divisor()));
      break;
    case Generator::CLOSURE_POINT:
      // A closure point in the origin has all zero homogeneous coefficients.
      new_gensys.insert(closure_point(e, old_g.divisor()));
      break;
    }
  }
  Polyhedron new_polyhedron(topology(), new_gensys);
  std::swap(*this, new_polyhedron);
  assert(OK(true));
}

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Polyhedron_templates_hh)
