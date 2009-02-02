/* Polyhedron class implementation (non-inline public functions).
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
#include "Polyhedron.defs.hh"
#include "Scalar_Products.defs.hh"
#include "MIP_Problem.defs.hh"
#include <cstdlib>
#include <cassert>
#include <iostream>

#ifndef ENSURE_SORTEDNESS
#define ENSURE_SORTEDNESS 0
#endif

namespace PPL = Parma_Polyhedra_Library;

PPL::dimension_type* PPL::Polyhedron::simplify_num_saturators_p = 0;

size_t PPL::Polyhedron::simplify_num_saturators_size = 0;

void
PPL::Polyhedron::initialize() {
  assert(simplify_num_saturators_p == 0);
  assert(simplify_num_saturators_size == 0);
  simplify_num_saturators_p = new dimension_type[simplify_num_saturators_size];
}

void
PPL::Polyhedron::finalize() {
  delete [] simplify_num_saturators_p;
  simplify_num_saturators_p = 0;
  simplify_num_saturators_size = 0;
}

PPL::dimension_type
PPL::Polyhedron::affine_dimension() const {
  if (is_empty())
    return 0;

  const Constraint_System& cs = minimized_constraints();
  dimension_type d = space_dim;
  for (Constraint_System::const_iterator i = cs.begin(),
	 cs_end = cs.end(); i != cs_end; ++i)
    if (i->is_equality())
      --d;
  return d;
}

const PPL::Constraint_System&
PPL::Polyhedron::constraints() const {
  if (marked_empty()) {
    // We want `con_sys' to only contain the unsatisfiable constraint
    // of the appropriate dimension.
    if (con_sys.has_no_rows()) {
      // The 0-dim unsatisfiable constraint is extended to
      // the appropriate dimension and then stored in `con_sys'.
      Constraint_System unsat_cs = Constraint_System::zero_dim_empty();
      unsat_cs.adjust_topology_and_space_dimension(topology(), space_dim);
      const_cast<Constraint_System&>(con_sys).swap(unsat_cs);
    }
    else {
      // Checking that `con_sys' contains the right thing.
      assert(con_sys.space_dimension() == space_dim);
      assert(con_sys.num_rows() == 1);
      assert(con_sys[0].is_inconsistent());
    }
    return con_sys;
  }

  if (space_dim == 0) {
    // Zero-dimensional universe.
    assert(con_sys.num_rows() == 0 && con_sys.num_columns() == 0);
    return con_sys;
  }

  // If the polyhedron has pending generators, we process them to obtain
  // the constraints. No processing is needed if the polyhedron has
  // pending constraints.
  if (has_pending_generators())
    process_pending_generators();
  else if (!constraints_are_up_to_date())
    update_constraints();

  // TODO: reconsider whether to really sort constraints at this stage.
#if ENSURE_SORTEDNESS
  // We insist in returning a sorted system of constraints,
  // but sorting is useless if there are pending constraints.
  if (!has_pending_constraints())
    obtain_sorted_constraints();
#endif
  return con_sys;
}

const PPL::Constraint_System&
PPL::Polyhedron::minimized_constraints() const {
  // `minimize()' or `strongly_minimize_constraints()'
  // will process any pending constraints or generators.
  if (is_necessarily_closed())
    minimize();
  else
    strongly_minimize_constraints();
  return constraints();
}

const PPL::Generator_System&
PPL::Polyhedron::generators() const {
  if (marked_empty()) {
    assert(gen_sys.has_no_rows());
    // We want `gen_sys' to have the appropriate space dimension,
    // even though it is an empty generator system.
    if (gen_sys.space_dimension() != space_dim) {
      Generator_System gs;
      gs.adjust_topology_and_space_dimension(topology(), space_dim);
      const_cast<Generator_System&>(gen_sys).swap(gs);
    }
    return gen_sys;
  }

  if (space_dim == 0) {
    assert(gen_sys.num_rows() == 0 && gen_sys.num_columns() == 0);
    return Generator_System::zero_dim_univ();
  }

  // If the polyhedron has pending constraints, we process them to obtain
  // the generators (we may discover that the polyhedron is empty).
  // No processing is needed if the polyhedron has pending generators.
  if ((has_pending_constraints() && !process_pending_constraints())
      || (!generators_are_up_to_date() && !update_generators())) {
    // We have just discovered that `*this' is empty.
    assert(gen_sys.has_no_rows());
    // We want `gen_sys' to have the appropriate space dimension,
    // even though it is an empty generator system.
    if (gen_sys.space_dimension() != space_dim) {
      Generator_System gs;
      gs.adjust_topology_and_space_dimension(topology(), space_dim);
      const_cast<Generator_System&>(gen_sys).swap(gs);
    }
    return gen_sys;
  }

  // TODO: reconsider whether to really sort generators at this stage.
#if ENSURE_SORTEDNESS
  // We insist in returning a sorted system of generators,
  // but sorting is useless if there are pending generators.
  if (!has_pending_generators())
    obtain_sorted_generators();
#else
  // In the case of an NNC polyhedron, if the generator system is fully
  // minimized (i.e., minimized and with no pending generator), then
  // return a sorted system of generators: this is needed so that the
  // const_iterator could correctly filter out the matched closure points.
  if (!is_necessarily_closed()
      && generators_are_minimized() && !has_pending_generators())
    obtain_sorted_generators();
#endif
  return gen_sys;
}

const PPL::Generator_System&
PPL::Polyhedron::minimized_generators() const {
  // `minimize()' or `strongly_minimize_generators()'
  // will process any pending constraints or generators.
  if (is_necessarily_closed())
    minimize();
  else
    strongly_minimize_generators();
  // Note: calling generators() on a strongly minimized NNC generator
  // system will also ensure sortedness, which is required to correctly
  // filter away the matched closure points.
  return generators();
}

PPL::Grid_Generator_System
PPL::Polyhedron::grid_generators() const {
  Grid_Generator_System ggs(space_dim);
  // Trivially true point.
  ggs.insert(grid_point(0*(Variable(0))));
  // A line for each dimension.
  dimension_type dim = 0;
  while (dim < space_dim)
    ggs.insert(grid_line(Variable(dim)));
  return ggs;
}

PPL::Poly_Con_Relation
PPL::Polyhedron::relation_with(const Constraint& c) const {
  // Dimension-compatibility check.
  if (space_dim < c.space_dimension())
    throw_dimension_incompatible("relation_with(c)", "c", c);

  if (marked_empty())
    return Poly_Con_Relation::saturates()
      && Poly_Con_Relation::is_included()
      && Poly_Con_Relation::is_disjoint();

  if (space_dim == 0) {
    if (c.is_inconsistent())
      if (c.is_strict_inequality() && c.inhomogeneous_term() == 0)
	// The constraint 0 > 0 implicitly defines the hyperplane 0 = 0;
	// thus, the zero-dimensional point also saturates it.
	return Poly_Con_Relation::saturates()
	  && Poly_Con_Relation::is_disjoint();
      else
	return Poly_Con_Relation::is_disjoint();
    else if (c.is_equality() || c.inhomogeneous_term() == 0)
      return Poly_Con_Relation::saturates()
	&& Poly_Con_Relation::is_included();
    else
      // The zero-dimensional point saturates
      // neither the positivity constraint 1 >= 0,
      // nor the strict positivity constraint 1 > 0.
      return Poly_Con_Relation::is_included();
  }

  if ((has_pending_constraints() && !process_pending_constraints())
      || (!generators_are_up_to_date() && !update_generators()))
    // The polyhedron is empty.
    return Poly_Con_Relation::saturates()
      && Poly_Con_Relation::is_included()
      && Poly_Con_Relation::is_disjoint();

  return gen_sys.relation_with(c);
}

PPL::Poly_Gen_Relation
PPL::Polyhedron::relation_with(const Generator& g) const {
  // Dimension-compatibility check.
  if (space_dim < g.space_dimension())
    throw_dimension_incompatible("relation_with(g)", "g", g);

  // The empty polyhedron cannot subsume a generator.
  if (marked_empty())
    return Poly_Gen_Relation::nothing();

  // A universe polyhedron in a zero-dimensional space subsumes
  // all the generators of a zero-dimensional space.
  if (space_dim == 0)
    return Poly_Gen_Relation::subsumes();

  if (has_pending_generators())
    process_pending_generators();
  else if (!constraints_are_up_to_date())
    update_constraints();

  return
    con_sys.satisfies_all_constraints(g)
    ? Poly_Gen_Relation::subsumes()
    : Poly_Gen_Relation::nothing();
}

PPL::Poly_Con_Relation
PPL::Polyhedron::relation_with(const Congruence& cg) const {
  dimension_type cg_space_dim = cg.space_dimension();
  // Dimension-compatibility check.
  if (space_dim < cg_space_dim)
    throw_dimension_incompatible("relation_with(cg)", "cg", cg);

  if (cg.is_equality()) {
    const Constraint c(cg);
    return relation_with(c);
  }

  if (marked_empty())
    return Poly_Con_Relation::saturates()
      && Poly_Con_Relation::is_included()
      && Poly_Con_Relation::is_disjoint();

  if (space_dim == 0) {
    if (cg.is_inconsistent())
      return Poly_Con_Relation::is_disjoint();
    else
      return Poly_Con_Relation::saturates()
	&& Poly_Con_Relation::is_included();
  }

  if ((has_pending_constraints() && !process_pending_constraints())
      || (!generators_are_up_to_date() && !update_generators()))
    // The polyhedron is empty.
    return Poly_Con_Relation::saturates()
      && Poly_Con_Relation::is_included()
      && Poly_Con_Relation::is_disjoint();

  // Find the equality corresponding to the congruence (ignoring the modulus).
  Linear_Expression expr;
  for (dimension_type i = cg_space_dim; i-- > 0; ) {
    const Variable v(i);
    expr += cg.coefficient(v) * v;
  }
  expr += cg.inhomogeneous_term();
  Constraint c(expr == 0);

  // The polyhedron is non-empty so that there exists a point.
  // For an arbitrary generator point find the scalar product with
  // the equality.
  PPL_DIRTY_TEMP_COEFFICIENT(point_val);

  for (Generator_System::const_iterator g = gen_sys.begin(),
         gen_sys_end = gen_sys.end(); g != gen_sys_end; ++g) {
    if (g->is_point()) {
      Scalar_Products::assign(point_val, c, *g);
      break;
    }
  }

  // Find the 2 hyperplanes that satisfies the congruence and are
  // nearest to the point such that the point lies on or between these
  // hyperplanes.
  // Then use the relations between the polyhedron and the corresponding
  // half-spaces to determine its relation with the congruence.
  const Coefficient& modulus = cg.modulus();

  PPL_DIRTY_TEMP_COEFFICIENT(div);
  div = modulus;

  PPL_DIRTY_TEMP_COEFFICIENT(nearest);
  nearest = (point_val / div) * div;

  point_val -= nearest;
  expr -= nearest;
  if (point_val == 0)
     return relation_with(expr == 0);

  Linear_Expression next_expr;
  if (point_val > 0) {
     next_expr = expr - modulus;
  }
  else {
    expr = - (expr);
    next_expr = expr - modulus;
  }

  Poly_Con_Relation relations = relation_with(expr >= 0);
  assert(!relations.implies(Poly_Con_Relation::saturates())
	 && !relations.implies(Poly_Con_Relation::is_disjoint()));
  if (relations.implies(Poly_Con_Relation::strictly_intersects()))
    return Poly_Con_Relation::strictly_intersects();

  assert(relations == Poly_Con_Relation::is_included());
  Poly_Con_Relation next_relations = relation_with(next_expr <= 0);
  assert(!next_relations.implies(Poly_Con_Relation::saturates())
	 && !next_relations.implies(Poly_Con_Relation::is_disjoint()));
  if (next_relations.implies(Poly_Con_Relation::strictly_intersects()))
    return Poly_Con_Relation::strictly_intersects();

  assert(next_relations == Poly_Con_Relation::is_included());
  return Poly_Con_Relation::is_disjoint();
}

bool
PPL::Polyhedron::is_universe() const {
  if (marked_empty())
    return false;

  if (space_dim == 0)
    return true;

  if (!has_pending_generators() && constraints_are_up_to_date()) {
    // Search for a constraint that is not a tautology.
    for (dimension_type i = con_sys.num_rows(); i-- > 0; )
      if (!con_sys[i].is_tautological())
	return false;
    // All the constraints are tautologies.
    return true;
  }

  assert(!has_pending_constraints() && generators_are_up_to_date());

  // Try a fast-fail test.
  dimension_type num_lines = 0;
  dimension_type num_rays = 0;
  const dimension_type first_pending = gen_sys.first_pending_row();
  for (dimension_type i = first_pending; i-- > 0; )
    switch (gen_sys[i].type()) {
    case Generator::RAY:
      ++num_rays;
      break;
    case Generator::LINE:
      ++num_lines;
      break;
    default:
      break;
    }

  if (has_pending_generators()) {
    // The non-pending part of `gen_sys' was minimized:
    // a success-first test is possible in this case.
    assert(generators_are_minimized());
    if (num_lines == space_dim) {
      assert(num_rays == 0);
      return true;
    }
    assert(num_lines < space_dim);
    // Now scan the pending generators.
    dimension_type num_pending_lines = 0;
    dimension_type num_pending_rays = 0;
    const dimension_type gs_num_rows = gen_sys.num_rows();
    for (dimension_type i = first_pending; i < gs_num_rows; ++i)
      switch (gen_sys[i].type()) {
      case Generator::RAY:
	++num_pending_rays;
	break;
      case Generator::LINE:
	++num_pending_lines;
	break;
      default:
	break;
      }
    // If no pending rays and lines were found,
    // then it is not the universe polyhedron.
    if (num_pending_rays == 0 && num_pending_lines == 0)
      return false;
    // Factor away the lines already seen (to be on the safe side,
    // we assume they are all linearly independent).
    if (num_lines + num_pending_lines < space_dim) {
      const dimension_type num_dims_missing
	= space_dim - (num_lines + num_pending_lines);
      // In order to span an n dimensional space (where n = num_dims_missing),
      // at least n+1 rays are needed.
      if (num_rays + num_pending_rays <= num_dims_missing)
	return false;
    }
  }
  else {
    // There is nothing pending.
    if (generators_are_minimized()) {
      // The exact test is possible.
      assert(num_rays == 0 || num_lines < space_dim);
      return num_lines == space_dim;
    }
    else
      // Only the fast-fail test can be computed: in order to span
      // an n dimensional space (where n = space_dim - num_lines),
      // at least n+1 rays are needed.
      if (num_lines < space_dim && num_lines + num_rays <= space_dim)
	return false;
  }

  // We need the polyhedron in minimal form.
  if (has_pending_generators())
    process_pending_generators();
  else if (!constraints_are_minimized())
    minimize();
  if (is_necessarily_closed())
    return (con_sys.num_rows() == 1
	    && con_sys[0].is_inequality()
	    && con_sys[0].is_tautological());
  else {
    // NNC polyhedron.
    if (con_sys.num_rows() != 2
	|| con_sys[0].is_equality()
	|| con_sys[1].is_equality())
      return false;
    else {
      // If the system of constraints contains two rows that
      // are not equalities, we are sure that they are
      // epsilon constraints: in this case we know that
      // the polyhedron is universe.
#ifndef NDEBUG
      obtain_sorted_constraints();
      const Constraint& eps_leq_one = con_sys[0];
      const Constraint& eps_geq_zero = con_sys[1];
      const dimension_type eps_index = con_sys.num_columns() - 1;
      assert(eps_leq_one[0] > 0 && eps_leq_one[eps_index] < 0
	     && eps_geq_zero[0] == 0 && eps_geq_zero[eps_index] > 0);
      for (dimension_type i = 1; i < eps_index; ++i)
	assert(eps_leq_one[i] == 0 && eps_geq_zero[i] == 0);
#endif
      return true;
    }
  }
}

bool
PPL::Polyhedron::is_bounded() const {
  // A zero-dimensional or empty polyhedron is bounded.
  if (space_dim == 0
      || marked_empty()
      || (has_pending_constraints() && !process_pending_constraints())
      || (!generators_are_up_to_date() && !update_generators()))
    return true;

  // If the system of generators contains any line or a ray,
  // then the polyhedron is unbounded.
  for (dimension_type i = gen_sys.num_rows(); i-- > 0; )
    if (gen_sys[i].is_line_or_ray())
      return false;

  // The system of generators is composed only by
  // points and closure points: the polyhedron is bounded.
  return true;
}

bool
PPL::Polyhedron::is_topologically_closed() const {
  // Necessarily closed polyhedra are trivially closed.
  if (is_necessarily_closed())
    return true;
  // Any empty or zero-dimensional polyhedron is closed.
  if (marked_empty()
      || space_dim == 0
      || (has_something_pending() && !process_pending()))
     return true;

  // At this point there are no pending constraints or generators.
  assert(!has_something_pending());

  if (generators_are_minimized()) {
    // A polyhedron is closed if and only if all of its (non-redundant)
    // closure points are matched by a corresponding point.
    const dimension_type n_rows = gen_sys.num_rows();
    const dimension_type n_lines = gen_sys.num_lines();
    for (dimension_type i = n_rows; i-- > n_lines; ) {
      const Generator& gi = gen_sys[i];
      if (gi.is_closure_point()) {
	bool gi_has_no_matching_point = true;
	for (dimension_type j = n_rows; j-- > n_lines; ) {
	  const Generator& gj = gen_sys[j];
	  if (i != j
	      && gj.is_point()
	      && gi.is_matching_closure_point(gj)) {
	    gi_has_no_matching_point = false;
	    break;
	  }
	}
	if (gi_has_no_matching_point)
	  return false;
      }
    }
    // All closure points are matched.
    return true;
  }

  // A polyhedron is closed if, after strong minimization
  // of its constraint system, it has no strict inequalities.
  strongly_minimize_constraints();
  return marked_empty() || !con_sys.has_strict_inequalities();
}

bool
PPL::Polyhedron::contains_integer_point() const {
  // Any empty polyhedron does not contain integer points.
  if (marked_empty())
    return false;

  // A zero-dimensional, universe polyhedron has, by convention, an
  // integer point.
  if (space_dim == 0)
    return true;

  // CHECKME: do we really want to call conversion to check for emptiness?
  if (has_pending_constraints() && !process_pending())
    // Empty again.
    return false;

  // FIXME: do also exploit info regarding rays and lines, if possible.
  // Is any integer point already available?
  assert(!has_pending_constraints());
  if (generators_are_up_to_date())
    for (dimension_type i = gen_sys.num_rows(); i-- > 0; )
      if (gen_sys[i].is_point() && gen_sys[i].divisor() == 1)
	return true;

  const Constraint_System& cs = constraints();
#if 0 // TEMPORARILY DISABLED.
  MIP_Problem mip(space_dim,
		  cs.begin(), cs.end(),
		  Variables_Set(Variable(0), Variable(space_dim-1)));
#else
  // FIXME: temporary workaround, to be removed as soon as the MIP
  // problem class will correctly and precisely handle
  // ((strict) in-) equality constraints having all integer variables.
  MIP_Problem mip(space_dim);
  mip.add_to_integer_space_dimensions(Variables_Set(Variable(0),
						    Variable(space_dim-1)));
  PPL_DIRTY_TEMP_COEFFICIENT(homogeneous_gcd);
  PPL_DIRTY_TEMP_COEFFICIENT(gcd);
  PPL_DIRTY_TEMP0(mpq_class, rational_inhomogeneous);
  PPL_DIRTY_TEMP_COEFFICIENT(tightened_inhomogeneous);
  for (Constraint_System::const_iterator cs_i = cs.begin(),
	 cs_end = cs.end(); cs_i != cs_end; ++cs_i) {
    const Constraint& c = *cs_i;
    const Constraint::Type c_type = c.type();
    const Coefficient& inhomogeneous = c.inhomogeneous_term();
    if (c_type == Constraint::STRICT_INEQUALITY) {
      // CHECKME: should we change the behavior of Linear_Expression(c) ?
      // Compute the GCD of the coefficients of c
      // (disregarding the inhomogeneous term and the espilon dimension).
      homogeneous_gcd = 0;
      for (dimension_type i = space_dim; i-- > 0; )
	gcd_assign(homogeneous_gcd,
		   homogeneous_gcd, c.coefficient(Variable(i)));
      if (homogeneous_gcd == 0) {
        // NOTE: since tautological constraints are already filtered away
        // by iterators, here we must have an inconsistent constraint.
        assert(c.is_inconsistent());
        return false;
      }
      Linear_Expression le;
      for (dimension_type i = space_dim; i-- > 0; )
	le += (c.coefficient(Variable(i)) / homogeneous_gcd) * Variable(i);
      // Add the integer part of `inhomogeneous'.
      le += (inhomogeneous / homogeneous_gcd);
      // Further tighten the constraint if the inhomogeneous term
      // was integer, i.e., if `homogeneous_gcd' divides `inhomogeneous'.
      gcd_assign(gcd, homogeneous_gcd, inhomogeneous);
      if (gcd == homogeneous_gcd)
	le -= 1;
      mip.add_constraint(le >= 0);
    }
    else {
      // Equality or non-strict inequality.
      // If possible, avoid useless gcd computations.
      if (inhomogeneous == 0)
	// The inhomogeneous term cannot be tightened.
	mip.add_constraint(c);
      else {
	// Compute the GCD of the coefficients of c
	// (disregarding the inhomogeneous term)
	// to see whether or not the inhomogeneous term can be tightened.
	homogeneous_gcd = 0;
	for (dimension_type i = space_dim; i-- > 0; )
	  gcd_assign(homogeneous_gcd,
		     homogeneous_gcd, c.coefficient(Variable(i)));
        if (homogeneous_gcd == 0) {
          // NOTE: since tautological constraints are already filtered away
          // by iterators, here we must have an inconsistent constraint.
          assert(c.is_inconsistent());
          return false;
        }
	else if (homogeneous_gcd == 1)
	  // The normalized inhomogeneous term is integer:
	  // add the constraint as-is.
	  mip.add_constraint(c);
	else {
	  assert(homogeneous_gcd > 1);
	  // Here the normalized inhomogeneous term is rational:
	  // the constraint has to be tightened.
#ifndef NDEBUG
	  // `homogeneous_gcd' does not divide `inhomogeneous'.
	  // FIXME: add a divisibility test for Coefficient.
	  gcd_assign(gcd, homogeneous_gcd, inhomogeneous);
	  assert(gcd == 1);
#endif
	  if (c.type() == Constraint::EQUALITY)
	    return false;
	  // Extract the homogeneous part of the constraint.
	  Linear_Expression le = Linear_Expression(c);
	  le -= inhomogeneous;
	  // Tighten the inhomogeneous term.
	  assign_r(rational_inhomogeneous.get_num(),
		   inhomogeneous, ROUND_NOT_NEEDED);
	  assign_r(rational_inhomogeneous.get_den(),
		   homogeneous_gcd, ROUND_NOT_NEEDED);
	  // Note: canonicalization is not needed (as gcd == 1).
	  assert(is_canonical(rational_inhomogeneous));
	  assign_r(tightened_inhomogeneous,
		   rational_inhomogeneous, ROUND_DOWN);
	  tightened_inhomogeneous *= homogeneous_gcd;
	  le += tightened_inhomogeneous;
	  mip.add_constraint(le >= 0);
	}
      }
    }
  }
#endif // TEMPORARY WORKAROUND.
  return mip.is_satisfiable();
}

bool
PPL::Polyhedron::constrains(const Variable var) const {
  // `var' should be one of the dimensions of the polyhedron.
  const dimension_type var_space_dim = var.space_dimension();
  if (space_dim < var_space_dim)
    throw_dimension_incompatible("constrains(v)", "v", var);

  // An empty polyhedron constrains all variables.
  if (marked_empty())
    return true;

  if (generators_are_up_to_date() && !has_pending_constraints()) {
    // Since generators are up-to-date and there are no pending
    // constraints, the generator system (since it is well formed)
    // contains a point.  Hence the polyhedron is not empty.
    if (constraints_are_up_to_date() && !has_pending_generators())
      // Here a variable is constrained if and only if it is
      // syntactically constrained.
      goto syntactic_check;

    if (generators_are_minimized()) {
      // Try a quick, incomplete check for the universe polyhedron:
      // a universe polyhedron constrains no variable.
      // Count the number of non-pending
      // (hence, linearly independent) lines.
      dimension_type num_lines = 0;
      const dimension_type first_pending = gen_sys.first_pending_row();
      for (dimension_type i = first_pending; i-- > 0; )
	if (gen_sys[i].is_line())
	  ++num_lines;

      if (num_lines == space_dim)
	return false;
    }

    // Scan generators: perhaps we will find a generator equivalent to
    // line(var) or a pair of generators equivalent to ray(-var) and
    // ray(var).
    bool have_positive_ray = false;
    bool have_negative_ray = false;
    const dimension_type var_id = var.id();
    for (dimension_type i = gen_sys.num_rows(); i-- > 0; ) {
      const Generator& gen_sys_i = gen_sys[i];
      if (gen_sys_i.is_line_or_ray()) {
	const Linear_Row& row = gen_sys_i;
	const int sign = sgn(row.coefficient(var_id));
	if (sign != 0) {
	  for (dimension_type j = space_dim+1; --j > 0; )
	    if (j != var_id && row[j] != 0)
	      goto next;
	  if (gen_sys_i.is_line())
	    return true;
	  if (sign > 0)
	    if (have_negative_ray)
	      return true;
	    else
	      have_positive_ray = true;
	  else if (have_positive_ray)
	    return true;
	  else
	    have_negative_ray = true;
	}
      }
    next:
      ;
    }

    // We are still here: at least we know that, since generators are
    // up-to-date and there are no pending constraints, then the
    // generator system (since it is well formed) contains a point.
    // Hence the polyhedron is not empty.
    if (has_pending_generators())
      process_pending_generators();
    else if (!constraints_are_up_to_date())
      update_constraints();
    goto syntactic_check;
  }

  // We must minimize to detect emptiness and obtain constraints.
  if (!minimize())
    return true;

 syntactic_check:
  for (dimension_type i = con_sys.num_rows(); i-- > 0; )
    if (con_sys[i].coefficient(var) != 0)
      return true;
  return false;
}

bool
PPL::Polyhedron::OK(bool check_not_empty) const {
#ifndef NDEBUG
  using std::endl;
  using std::cerr;
#endif

  // The expected number of columns in the constraint and generator
  // systems, if they are not empty.
  const dimension_type poly_num_columns
    = space_dim + (is_necessarily_closed() ? 1 : 2);

  // Check whether the topologies of `con_sys' and `gen_sys' agree.
  if (con_sys.topology() != gen_sys.topology()) {
#ifndef NDEBUG
    cerr << "Constraints and generators have different topologies!"
	 << endl;
#endif
    goto bomb;
  }

  // Check whether the saturation matrices are well-formed.
  if (!sat_c.OK())
    goto bomb;
  if (!sat_g.OK())
    goto bomb;

  // Check whether the status information is legal.
  if (!status.OK())
    goto bomb;

  if (marked_empty()) {
    if (check_not_empty) {
      // The caller does not want the polyhedron to be empty.
#ifndef NDEBUG
      cerr << "Empty polyhedron!" << endl;
#endif
      goto bomb;
    }

    // An empty polyhedron is allowed if the system of constraints
    // either has no rows or only contains an unsatisfiable constraint
    // and if it has no pending constraints or generators.
    if (has_something_pending()) {
#ifndef NDEBUG
      cerr << "The polyhedron is empty, "
	   << "but it has something pending" << endl;
#endif
      goto bomb;
    }
    if (con_sys.has_no_rows())
      return true;
    else {
      if (con_sys.space_dimension() != space_dim) {
#ifndef NDEBUG
	cerr << "The polyhedron is in a space of dimension "
	     << space_dim
	     << " while the system of constraints is in a space of dimension "
	     << con_sys.space_dimension()
	     << endl;
#endif
	goto bomb;
      }
      if (con_sys.num_rows() != 1) {
#ifndef NDEBUG
	cerr << "The system of constraints for an empty polyhedron "
	     << "has more then one row"
	     << endl;
#endif
	goto bomb;
      }
      if (!con_sys[0].is_inconsistent()) {
#ifndef NDEBUG
	cerr << "Empty polyhedron with a satisfiable system of constraints"
	     << endl;
#endif
	goto bomb;
      }
      // Here we have only one, inconsistent constraint.
      return true;
    }
  }

  // A zero-dimensional, non-empty polyhedron is legal only if the
  // system of constraint `con_sys' and the system of generators
  // `gen_sys' have no rows.
  if (space_dim == 0) {
    if (has_something_pending()) {
#ifndef NDEBUG
      cerr << "Zero-dimensional polyhedron with something pending"
	   << endl;
#endif
      goto bomb;
    }
    if (!con_sys.has_no_rows() || !gen_sys.has_no_rows()) {
#ifndef NDEBUG
      cerr << "Zero-dimensional polyhedron with a non-empty"
	   << endl
	   << "system of constraints or generators."
	   << endl;
#endif
      goto bomb;
    }
    return true;
  }

  // A polyhedron is defined by a system of constraints
  // or a system of generators: at least one of them must be up to date.
  if (!constraints_are_up_to_date() && !generators_are_up_to_date()) {
#ifndef NDEBUG
    cerr << "Polyhedron not empty, not zero-dimensional"
	 << endl
	 << "and with neither constraints nor generators up-to-date!"
	 << endl;
#endif
    goto bomb;
  }

  // Here we check if the size of the matrices is consistent.
  // Let us suppose that all the matrices are up-to-date; this means:
  // `con_sys' : number of constraints x poly_num_columns
  // `gen_sys' : number of generators  x poly_num_columns
  // `sat_c'   : number of generators  x number of constraints
  // `sat_g'   : number of constraints x number of generators.
  if (constraints_are_up_to_date()) {
    if (con_sys.num_columns() != poly_num_columns) {
#ifndef NDEBUG
      cerr << "Incompatible size! (con_sys and space_dim)"
	   << endl;
#endif
      goto bomb;
    }
    if (sat_c_is_up_to_date())
      if (con_sys.first_pending_row() != sat_c.num_columns()) {
#ifndef NDEBUG
	cerr << "Incompatible size! (con_sys and sat_c)"
	     << endl;
#endif
	goto bomb;
      }
    if (sat_g_is_up_to_date())
      if (con_sys.first_pending_row() != sat_g.num_rows()) {
#ifndef NDEBUG
	cerr << "Incompatible size! (con_sys and sat_g)"
	     << endl;
#endif
	goto bomb;
      }
    if (generators_are_up_to_date())
      if (con_sys.num_columns() != gen_sys.num_columns()) {
#ifndef NDEBUG
	cerr << "Incompatible size! (con_sys and gen_sys)"
	     << endl;
#endif
	goto bomb;
      }
  }

  if (generators_are_up_to_date()) {
    if (gen_sys.num_columns() != poly_num_columns) {
#ifndef NDEBUG
      cerr << "Incompatible size! (gen_sys and space_dim)"
	   << endl;
#endif
      goto bomb;
    }
    if (sat_c_is_up_to_date())
      if (gen_sys.first_pending_row() != sat_c.num_rows()) {
#ifndef NDEBUG
	cerr << "Incompatible size! (gen_sys and sat_c)"
	     << endl;
#endif
	goto bomb;
      }
    if (sat_g_is_up_to_date())
      if (gen_sys.first_pending_row() != sat_g.num_columns()) {
#ifndef NDEBUG
	cerr << "Incompatible size! (gen_sys and sat_g)"
	     << endl;
#endif
	goto bomb;
      }

    // Check if the system of generators is well-formed.
    if (!gen_sys.OK())
      goto bomb;

    if (gen_sys.first_pending_row() == 0) {
#ifndef NDEBUG
      cerr << "Up-to-date generator system with all rows pending!"
	   << endl;
#endif
      goto bomb;
    }

    // A non-empty system of generators describing a polyhedron
    // is valid if and only if it contains a point.
    if (!gen_sys.has_no_rows() && !gen_sys.has_points()) {
#ifndef NDEBUG
      cerr << "Non-empty generator system declared up-to-date "
	   << "has no points!"
	   << endl;
#endif
      goto bomb;
    }

#if 0 // To be activated when Status keeps strong minimization flags.
    //=================================================
    // TODO: this test is wrong in the general case.
    // However, such an invariant does hold for a
    // strongly-minimized Generator_System.
    // We will activate this test as soon as the Status
    // flags will be able to remember if a system is
    // strongly minimized.

    // Checking that the number of closure points is always
    // greater than the number of points.
    if (!is_necessarily_closed()) {
      dimension_type num_points = 0;
      dimension_type num_closure_points = 0;
      dimension_type eps_index = gen_sys.num_columns() - 1;
      for (dimension_type i = gen_sys.num_rows(); i-- > 0; )
	if (!gen_sys[i].is_line_or_ray())
	  if (gen_sys[i][eps_index] > 0)
	    ++num_points;
	  else
	    ++num_closure_points;
      if (num_points > num_closure_points) {
#ifndef NDEBUG
	cerr << "# POINTS > # CLOSURE_POINTS" << endl;
#endif
	goto bomb;
      }
    }
    //=================================================
#endif

    if (generators_are_minimized()) {
      // If the system of generators is minimized, the number of
      // lines, rays and points of the polyhedron must be the same as
      // of a temporary, minimized one. If this does not happen then
      // the polyhedron is not OK.
      Constraint_System new_con_sys(topology());
      Generator_System gs_without_pending = gen_sys;
      gs_without_pending.erase_to_end(gen_sys.first_pending_row());
      gs_without_pending.unset_pending_rows();
      Generator_System copy_of_gen_sys = gs_without_pending;
      Bit_Matrix new_sat_c;
      minimize(false, copy_of_gen_sys, new_con_sys, new_sat_c);
      const dimension_type copy_num_lines = copy_of_gen_sys.num_lines();
      if (gs_without_pending.num_rows() != copy_of_gen_sys.num_rows()
	  || gs_without_pending.num_lines() != copy_num_lines
	  || gs_without_pending.num_rays() != copy_of_gen_sys.num_rays()) {
#ifndef NDEBUG
	cerr << "Generators are declared minimized, but they are not!\n"
	     << "Here is the minimized form of the generators:\n";
	copy_of_gen_sys.ascii_dump(cerr);
	cerr << endl;
#endif
	goto bomb;
      }

      // CHECKME : the following observation is not formally true
      //           for a NNC_Polyhedron. But it may be true for its
      //           representation ...

      // If the corresponding polyhedral cone is _pointed_, then
      // a minimal system of generators is unique up to positive scaling.
      // We thus verify if the cone is pointed (i.e., there are no lines)
      // and, after normalizing and sorting a copy of the system `gen_sys'
      // of the polyhedron (we use a copy not to modify the polyhedron's
      // system) and the system `copy_of_gen_sys' that has been just
      // minimized, we check if the two matrices are identical.  If
      // they are different it means that the generators of the
      // polyhedron are declared minimized, but they are not.
      if (copy_num_lines == 0) {
	copy_of_gen_sys.strong_normalize();
	copy_of_gen_sys.sort_rows();
	gs_without_pending.strong_normalize();
	gs_without_pending.sort_rows();
	if (copy_of_gen_sys != gs_without_pending) {
#ifndef NDEBUG
	  cerr << "Generators are declared minimized, but they are not!\n"
	       << "(we are in the case:\n"
	       << "dimension of lineality space equal to 0)\n"
	       << "Here is the minimized form of the generators:\n";
	  copy_of_gen_sys.ascii_dump(cerr);
	  cerr << endl;
#endif
	    goto bomb;
	}
      }
    }
  }

  if (constraints_are_up_to_date()) {
    // Check if the system of constraints is well-formed.
    if (!con_sys.OK())
      goto bomb;

    if (con_sys.first_pending_row() == 0) {
#ifndef NDEBUG
      cerr << "Up-to-date constraint system with all rows pending!"
	   << endl;
#endif
      goto bomb;
    }

    // A non-empty system of constraints describing a polyhedron
    // must contain a constraint with a non-zero inhomogeneous term;
    // such a constraint corresponds to (a combination of other
    // constraints with):
    // -* the positivity constraint, for necessarily closed polyhedra;
    // -* the epsilon <= 1 constraint, for NNC polyhedra.
    bool no_positivity_constraint = true;
    for (dimension_type i = con_sys.num_rows(); i-- > 0; )
      if (con_sys[i].inhomogeneous_term() != 0) {
	no_positivity_constraint = false;
	break;
      }
    if (no_positivity_constraint) {
#ifndef NDEBUG
      cerr << "Non-empty constraint system has no positivity constraint"
	   << endl;
#endif
      goto bomb;
    }

    if (!is_necessarily_closed()) {
      // A non-empty system of constraints describing a NNC polyhedron
      // must also contain a (combination of) the constraint epsilon >= 0,
      // i.e., a constraint with a positive epsilon coefficient.
      bool no_epsilon_geq_zero = true;
      const dimension_type eps_index = con_sys.num_columns() - 1;
      for (dimension_type i = con_sys.num_rows(); i-- > 0; )
	if (con_sys[i][eps_index] > 0) {
	  no_epsilon_geq_zero = false;
	  break;
	}
      if (no_epsilon_geq_zero) {
#ifndef NDEBUG
	cerr << "Non-empty constraint system for NNC polyhedron "
	     << "has no epsilon >= 0 constraint"
	     << endl;
#endif
	goto bomb;
      }
    }

    Constraint_System cs_without_pending = con_sys;
    cs_without_pending.erase_to_end(con_sys.first_pending_row());
    cs_without_pending.unset_pending_rows();
    Constraint_System copy_of_con_sys = cs_without_pending;
    bool empty = false;
    if (check_not_empty || constraints_are_minimized()) {
      Generator_System new_gen_sys(topology());
      Bit_Matrix new_sat_g;
      empty = minimize(true, copy_of_con_sys, new_gen_sys, new_sat_g);
    }

    if (empty && check_not_empty) {
#ifndef NDEBUG
      cerr << "Unsatisfiable system of constraints!"
	   << endl;
#endif
      goto bomb;
    }

    if (constraints_are_minimized()) {
      // If the constraints are minimized, the number of equalities
      // and of inequalities of the system of the polyhedron must be
      // the same of the temporary minimized one.
      // If it does not happen, the polyhedron is not OK.
      if (cs_without_pending.num_rows() != copy_of_con_sys.num_rows()
	  || cs_without_pending.num_equalities()
	  != copy_of_con_sys.num_equalities()) {
#ifndef NDEBUG
	cerr << "Constraints are declared minimized, but they are not!\n"
	     << "Here is the minimized form of the constraints:\n";
	copy_of_con_sys.ascii_dump(cerr);
	cerr << endl;
#endif
	goto bomb;
      }
      // The system `copy_of_con_sys' has the form that is obtained
      // after applying methods gauss() and back_substitute().
      // A system of constraints can be minimal even if it does not
      // have this form. So, to verify if the polyhedron is correct,
      // we copy the system `con_sys' in a temporary one and then
      // modify it using method simplify() (which calls both gauss()
      // and back_substitute()).
      // If the temporary system and `copy_of_con_sys' are different,
      // the polyhedron is not OK.
      copy_of_con_sys.strong_normalize();
      copy_of_con_sys.sort_rows();
      cs_without_pending.simplify();
      cs_without_pending.strong_normalize();
      cs_without_pending.sort_rows();
      if (cs_without_pending != copy_of_con_sys) {
#ifndef NDEBUG
	cerr << "Constraints are declared minimized, but they are not!\n"
	     << "Here is the minimized form of the constraints:\n";
	copy_of_con_sys.ascii_dump(cerr);
	cerr << endl;
#endif
	goto bomb;
      }
    }
  }

  if (sat_c_is_up_to_date())
    for (dimension_type i = sat_c.num_rows(); i-- > 0; ) {
      const Generator tmp_gen = gen_sys[i];
      const Bit_Row tmp_sat = sat_c[i];
      for (dimension_type j = sat_c.num_columns(); j-- > 0; )
	if (Scalar_Products::sign(con_sys[j], tmp_gen) != tmp_sat[j]) {
#ifndef NDEBUG
	  cerr << "sat_c is declared up-to-date, but it is not!"
	       << endl;
#endif
	  goto bomb;
	}
    }

  if (sat_g_is_up_to_date())
    for (dimension_type i = sat_g.num_rows(); i-- > 0; ) {
      const Constraint tmp_con = con_sys[i];
      const Bit_Row tmp_sat = sat_g[i];
      for (dimension_type j = sat_g.num_columns(); j-- > 0; )
	if (Scalar_Products::sign(tmp_con, gen_sys[j]) != tmp_sat[j]) {
#ifndef NDEBUG
	  cerr << "sat_g is declared up-to-date, but it is not!"
	       << endl;
#endif
	  goto bomb;
	}
    }

  if (has_pending_constraints()) {
    if (con_sys.num_pending_rows() == 0) {
#ifndef NDEBUG
      cerr << "The polyhedron is declared to have pending constraints, "
	   << "but con_sys has no pending rows!"
	   << endl;
#endif
      goto bomb;
    }
  }

  if (has_pending_generators()) {
    if (gen_sys.num_pending_rows() == 0) {
#ifndef NDEBUG
      cerr << "The polyhedron is declared to have pending generators, "
	   << "but gen_sys has no pending rows!"
	   << endl;
#endif
      goto bomb;
    }
  }

  return true;

 bomb:
#ifndef NDEBUG
  cerr << "Here is the guilty polyhedron:"
       << endl;
  ascii_dump(cerr);
#endif
  return false;
}

void
PPL::Polyhedron::add_constraint(const Constraint& c) {
  // Topology-compatibility check.
  if (c.is_strict_inequality() && is_necessarily_closed()) {
    // Trivially true/false strict inequalities are legal.
    if (c.is_tautological())
      return;
    if (c.is_inconsistent()) {
      set_empty();
      return;
    }
    // Here c is a non-trivial strict inequality.
    throw_topology_incompatible("add_constraint(c)", "c", c);
  }

  // Dimension-compatibility check:
  // the dimension of `c' can not be greater than space_dim.
  if (space_dim < c.space_dimension())
    throw_dimension_incompatible("add_constraint(c)", "c", c);

  if (!marked_empty())
    refine_no_check(c);
}

void
PPL::Polyhedron::add_congruence(const Congruence& cg) {
  // Dimension-compatibility check:
  // the dimension of `cg' can not be greater than space_dim.
  if (space_dim < cg.space_dimension())
    throw_dimension_incompatible("add_congruence(cg)", "cg", cg);

  // Handle the case of proper congruences first.
  if (cg.is_proper_congruence()) {
    if (cg.is_tautological())
      return;
    if (cg.is_inconsistent()) {
      set_empty();
      return;
    }
    // Non-trivial and proper congruences are not allowed.
    throw_invalid_argument("add_congruence(cg)",
			   "cg is a non-trivial, proper congruence");
  }

  assert(cg.is_equality());
  // Handle empty and 0-dim cases first.
  if (marked_empty())
    return;
  if (space_dim == 0) {
    if (cg.is_inconsistent())
      set_empty();
    return;
  }

  // Add the equality.
  Linear_Expression le(cg);
  Constraint c(le, Constraint::EQUALITY, NECESSARILY_CLOSED);
  // Enforce normalization.
  c.strong_normalize();
  refine_no_check(c);
}

bool
PPL::Polyhedron::add_constraint_and_minimize(const Constraint& c) {
  Constraint_System cs(c);
  return add_recycled_constraints_and_minimize(cs);
}

void
PPL::Polyhedron::add_generator(const Generator& g) {
  // Topology-compatibility check.
  if (g.is_closure_point() && is_necessarily_closed())
    throw_topology_incompatible("add_generator(g)", "g", g);
  // Dimension-compatibility check:
  // the dimension of `g' can not be greater than space_dim.
  const dimension_type g_space_dim = g.space_dimension();
  if (space_dim < g_space_dim)
    throw_dimension_incompatible("add_generator(g)", "g", g);

  // Dealing with a zero-dimensional space polyhedron first.
  if (space_dim == 0) {
    // It is not possible to create 0-dim rays or lines.
    assert(g.is_point() || g.is_closure_point());
    // Closure points can only be inserted in non-empty polyhedra.
    if (marked_empty()) {
      if (g.type() != Generator::POINT)
	throw_invalid_generator("add_generator(g)", "g");
      else
	set_zero_dim_univ();
    }
    assert(OK());
    return;
  }

  if (marked_empty()
      || (has_pending_constraints() && !process_pending_constraints())
      || (!generators_are_up_to_date() && !update_generators())) {
    // Here the polyhedron is empty:
    // the specification says we can only insert a point.
    if (!g.is_point())
      throw_invalid_generator("add_generator(g)", "g");
    if (g.is_necessarily_closed() || !is_necessarily_closed()) {
      gen_sys.insert(g);
      // Since `gen_sys' was empty, after inserting `g' we have to resize
      // the system of generators to have the right dimension.
      gen_sys.adjust_topology_and_space_dimension(topology(), space_dim);
      if (!is_necessarily_closed()) {
	// In the NNC topology, each point has to be matched by
	// a corresponding closure point:
	// turn the just inserted point into the corresponding
	// (normalized) closure point.
	Generator& cp = gen_sys[gen_sys.num_rows() - 1];
	cp[space_dim + 1] = 0;
	cp.normalize();
	// Re-insert the point (which is already normalized).
	gen_sys.insert(g);
      }
    }
    else {
      // Note: here we have a _legal_ topology mismatch,
      // because `g' is NOT a closure point (it is a point!)
      // However, by barely invoking `gen_sys.insert(g)' we would
      // cause a change in the topology of `gen_sys', which is wrong.
      // Thus, we insert a "topology corrected" copy of `g'.
      const Linear_Expression nc_expr = Linear_Expression(g);
      gen_sys.insert(Generator::point(nc_expr, g.divisor()));
      // Since `gen_sys' was empty, after inserting `g' we have to resize
      // the system of generators to have the right dimension.
      gen_sys.adjust_topology_and_space_dimension(topology(), space_dim);
    }
    // No longer empty, generators up-to-date and minimized.
    clear_empty();
    set_generators_minimized();
  }
  else {
    assert(generators_are_up_to_date());
    const bool has_pending = can_have_something_pending();
    if (g.is_necessarily_closed() || !is_necessarily_closed()) {
      // Since `gen_sys' is not empty, the topology and space dimension
      // of the inserted generator are automatically adjusted.
      if (has_pending)
	gen_sys.insert_pending(g);
      else
	gen_sys.insert(g);
      if (!is_necessarily_closed() && g.is_point()) {
	// In the NNC topology, each point has to be matched by
	// a corresponding closure point:
	// turn the just inserted point into the corresponding
	// (normalized) closure point.
	Generator& cp = gen_sys[gen_sys.num_rows() - 1];
	cp[space_dim + 1] = 0;
	cp.normalize();
	// Re-insert the point (which is already normalized).
	if (has_pending)
	  gen_sys.insert_pending(g);
	else
	  gen_sys.insert(g);
      }
    }
    else {
      assert(!g.is_closure_point());
      // Note: here we have a _legal_ topology mismatch, because
      // `g' is NOT a closure point.
      // However, by barely invoking `gen_sys.insert(g)' we would
      // cause a change in the topology of `gen_sys', which is wrong.
      // Thus, we insert a "topology corrected" copy of `g'.
      const Linear_Expression nc_expr = Linear_Expression(g);
      switch (g.type()) {
      case Generator::LINE:
	if (has_pending)
	  gen_sys.insert_pending(Generator::line(nc_expr));
	else
	  gen_sys.insert(Generator::line(nc_expr));
	break;
      case Generator::RAY:
	if (has_pending)
	  gen_sys.insert_pending(Generator::ray(nc_expr));
	else
	  gen_sys.insert(Generator::ray(nc_expr));
	break;
      case Generator::POINT:
	if (has_pending)
	  gen_sys.insert_pending(Generator::point(nc_expr, g.divisor()));
	else
	  gen_sys.insert(Generator::point(nc_expr, g.divisor()));
	break;
      default:
	throw_runtime_error("add_generator(const Generator& g)");
      }
    }

    if (has_pending)
      set_generators_pending();
    else {
      // After adding the new generator,
      // constraints are no longer up-to-date.
      clear_generators_minimized();
      clear_constraints_up_to_date();
    }
  }
  assert(OK());
}

bool
PPL::Polyhedron::add_generator_and_minimize(const Generator& g) {
  // TODO: this is just an executable specification.
  Generator_System gs(g);
  return add_recycled_generators_and_minimize(gs);
}

void
PPL::Polyhedron::add_recycled_constraints(Constraint_System& cs) {
  // Topology compatibility check.
  if (is_necessarily_closed() && cs.has_strict_inequalities()) {
    // We check if _all_ strict inequalities in cs are trivially false.
    // (The iterators already filter away trivially true constraints.)
    for (Constraint_System::const_iterator i = cs.begin(),
           i_end = cs.end(); i != i_end; ++i) {
      if (i->is_strict_inequality()
          && !i->is_inconsistent())
        throw_topology_incompatible("add_recycled_constraints(cs)",
                                    "cs", cs);
    }
    // If we reach this point, all strict inequalities were inconsistent.
    set_empty();
    return;
  }

  // Dimension-compatibility check:
  // the dimension of `cs' can not be greater than space_dim.
  const dimension_type cs_space_dim = cs.space_dimension();
  if (space_dim < cs_space_dim)
    throw_dimension_incompatible("add_recycled_constraints(cs)", "cs", cs);

  // Adding no constraints is a no-op.
  if (cs.has_no_rows())
    return;

  if (space_dim == 0) {
    // In a 0-dimensional space the constraints are
    // tautologies (e.g., 0 == 0 or 1 >= 0 or 1 > 0) or
    // inconsistent (e.g., 1 == 0 or -1 >= 0 or 0 > 0).
    // In a system of constraints `begin()' and `end()' are equal
    // if and only if the system only contains tautologies.
    if (cs.begin() != cs.end())
      // There is a constraint, it must be inconsistent,
      // the polyhedron is empty.
      status.set_empty();
    return;
  }

  if (marked_empty())
    return;

  // The constraints (possibly with pending rows) are required.
  if (has_pending_generators())
    process_pending_generators();
  else if (!constraints_are_up_to_date())
    update_constraints();

  // Adjust `cs' to the right topology and space dimension.
  // NOTE: we already checked for topology compatibility.
  cs.adjust_topology_and_space_dimension(topology(), space_dim);

  const bool adding_pending = can_have_something_pending();

  // Here we do not require `con_sys' to be sorted.
  // also, we _swap_ (instead of copying) the coefficients of `cs'
  // (which is not a const).
  const dimension_type old_num_rows = con_sys.num_rows();
  const dimension_type cs_num_rows = cs.num_rows();
  const dimension_type cs_num_columns = cs.num_columns();
  con_sys.add_zero_rows(cs_num_rows,
			Linear_Row::Flags(topology(),
					  Linear_Row::RAY_OR_POINT_OR_INEQUALITY));
  for (dimension_type i = cs_num_rows; i-- > 0; ) {
    // NOTE: we cannot directly swap the rows, since they might have
    // different capacities (besides possibly having different sizes):
    // thus, we steal one coefficient at a time.
    Constraint& new_c = con_sys[old_num_rows + i];
    Constraint& old_c = cs[i];
    if (old_c.is_equality())
      new_c.set_is_equality();
    for (dimension_type j = cs_num_columns; j-- > 0; )
      std::swap(new_c[j], old_c[j]);
  }

  if (adding_pending)
    set_constraints_pending();
  else {
    // The newly added ones are not pending constraints.
    con_sys.unset_pending_rows();
    // They have been simply appended.
    con_sys.set_sorted(false);
    // Constraints are not minimized and generators are not up-to-date.
    clear_constraints_minimized();
    clear_generators_up_to_date();
  }
  // Note: the constraint system may have become unsatisfiable, thus
  // we do not check for satisfiability.
  assert(OK());
}

void
PPL::Polyhedron::add_constraints(const Constraint_System& cs) {
  // TODO: this is just an executable specification.
  Constraint_System cs_copy = cs;
  add_recycled_constraints(cs_copy);
}

bool
PPL::Polyhedron::add_recycled_constraints_and_minimize(Constraint_System& cs) {
  add_recycled_constraints(cs);
  return minimize();
}

bool
PPL::Polyhedron::add_constraints_and_minimize(const Constraint_System& cs) {
  Constraint_System cs_copy = cs;
  return add_recycled_constraints_and_minimize(cs_copy);
}

void
PPL::Polyhedron::add_recycled_generators(Generator_System& gs) {
  // Topology compatibility check.
  if (is_necessarily_closed() && gs.has_closure_points())
    throw_topology_incompatible("add_recycled_generators(gs)", "gs", gs);
  // Dimension-compatibility check:
  // the dimension of `gs' can not be greater than space_dim.
  const dimension_type gs_space_dim = gs.space_dimension();
  if (space_dim < gs_space_dim)
    throw_dimension_incompatible("add_recycled_generators(gs)", "gs", gs);

  // Adding no generators is a no-op.
  if (gs.has_no_rows())
    return;

  // Adding valid generators to a zero-dimensional polyhedron
  // transform it in the zero-dimensional universe polyhedron.
  if (space_dim == 0) {
    if (marked_empty() && !gs.has_points())
      throw_invalid_generators("add_recycled_generators(gs)", "gs");
    set_zero_dim_univ();
    assert(OK(true));
    return;
  }

  // Adjust `gs' to the right topology and dimensions.
  // NOTE: we already checked for topology compatibility.
  gs.adjust_topology_and_space_dimension(topology(), space_dim);
  // For NNC polyhedra, each point must be matched by
  // the corresponding closure point.
  if (!is_necessarily_closed())
    gs.add_corresponding_closure_points();

  // The generators (possibly with pending rows) are required.
  if ((has_pending_constraints() && !process_pending_constraints())
      || (!generators_are_up_to_date() && !minimize())) {
    // We have just discovered that `*this' is empty.
    // So `gs' must contain at least one point.
    if (!gs.has_points())
      throw_invalid_generators("add_recycled_generators(gs)", "gs");
    // The polyhedron is no longer empty and generators are up-to-date.
    std::swap(gen_sys, gs);
    if (gen_sys.num_pending_rows() > 0) {
      // Even though `gs' has pending generators, since the constraints
      // of the polyhedron are not up-to-date, the polyhedron cannot
      // have pending generators. By integrating the pending part
      // of `gen_sys' we may loose sortedness.
      gen_sys.unset_pending_rows();
      gen_sys.set_sorted(false);
    }
    set_generators_up_to_date();
    clear_empty();
    assert(OK());
    return;
  }

  const bool adding_pending = can_have_something_pending();

  // Here we do not require `gen_sys' to be sorted.
  // also, we _swap_ (instead of copying) the coefficients of `gs'
  // (which is not a const).
  const dimension_type old_num_rows = gen_sys.num_rows();
  const dimension_type gs_num_rows = gs.num_rows();
  const dimension_type gs_num_columns = gs.num_columns();
  gen_sys.add_zero_rows(gs_num_rows,
			Linear_Row::Flags(topology(),
					  Linear_Row::RAY_OR_POINT_OR_INEQUALITY));
  for (dimension_type i = gs_num_rows; i-- > 0; ) {
    // NOTE: we cannot directly swap the rows, since they might have
    // different capacities (besides possibly having different sizes):
    // thus, we steal one coefficient at a time.
    Generator& new_g = gen_sys[old_num_rows + i];
    Generator& old_g = gs[i];
    if (old_g.is_line())
      new_g.set_is_line();
    for (dimension_type j = gs_num_columns; j-- > 0; )
      std::swap(new_g[j], old_g[j]);
  }

  if (adding_pending)
    set_generators_pending();
  else {
    // The newly added ones are not pending generators.
    gen_sys.unset_pending_rows();
    // They have been simply appended.
    gen_sys.set_sorted(false);
    // Constraints are not up-to-date and generators are not minimized.
    clear_constraints_up_to_date();
    clear_generators_minimized();
  }
  assert(OK(true));
}

void
PPL::Polyhedron::add_generators(const Generator_System& gs) {
  // TODO: this is just an executable specification.
  Generator_System gs_copy = gs;
  add_recycled_generators(gs_copy);
}

bool
PPL::Polyhedron::add_recycled_generators_and_minimize(Generator_System& gs) {
  // Topology compatibility check.
  if (is_necessarily_closed() && gs.has_closure_points())
    throw_topology_incompatible("add_recycled_generators_and_minimize(gs)",
				"gs", gs);
  // Dimension-compatibility check:
  // the dimension of `gs' can not be greater than space_dim.
  const dimension_type gs_space_dim = gs.space_dimension();
  if (space_dim < gs_space_dim)
    throw_dimension_incompatible("add_recycled_generators_and_minimize(gs)",
				 "gs", gs);

  // Adding no generators is equivalent to just requiring minimization.
  if (gs.has_no_rows())
    return minimize();

  // Adding valid generators to a zero-dimensional polyhedron
  // transform it in the zero-dimensional universe polyhedron.
  if (space_dim == 0) {
    if (marked_empty() && !gs.has_points())
      throw_invalid_generators("add_recycled_generators_and_minimize(gs)",
			       "gs");
    set_zero_dim_univ();
    assert(OK(true));
    return true;
  }

  // Adjust `gs' to the right topology.
  // NOTE: we already checked for topology compatibility;
  // also, we do NOT adjust dimensions now, so that we will
  // spend less time to sort rows.
  gs.adjust_topology_and_space_dimension(topology(), gs_space_dim);

  // For NNC polyhedra, each point must be matched by
  // the corresponding closure point.
  if (!is_necessarily_closed())
    gs.add_corresponding_closure_points();

  // `gs' has to be fully sorted, thus it cannot have pending rows.
  if (gs.num_pending_rows() > 0) {
    gs.unset_pending_rows();
    gs.sort_rows();
  }
  else if (!gs.is_sorted())
    gs.sort_rows();

  // Now adjusting dimensions (topology already adjusted).
  // NOTE: sortedness is preserved.
  gs.adjust_topology_and_space_dimension(topology(), space_dim);

  if (minimize()) {
    obtain_sorted_generators_with_sat_g();
    // This call to `add_and_minimize()' cannot return `false'.
    add_and_minimize(false, gen_sys, con_sys, sat_g, gs);
    clear_sat_c_up_to_date();
  }
  else {
    // The polyhedron was empty: check if `gs' contains a point.
    if (!gs.has_points())
      throw_invalid_generators("add_recycled_generators_and_minimize(gs)",
			       "gs");
    // `gs' has a point: the polyhedron is no longer empty
    // and generators are up-to-date.
    std::swap(gen_sys, gs);
    clear_empty();
    set_generators_up_to_date();
    // This call to `minimize()' cannot return `false'.
    minimize();
  }
  assert(OK(true));
  return true;
}

bool
PPL::Polyhedron::add_generators_and_minimize(const Generator_System& gs) {
  // TODO: this is just an executable specification.
  Generator_System gs_copy = gs;
  return add_recycled_generators_and_minimize(gs_copy);
}

void
PPL::Polyhedron::add_congruences(const Congruence_System& cgs) {
  // Dimension-compatibility check.
  if (space_dim < cgs.space_dimension())
    throw_dimension_incompatible("add_congruences(cgs)", "cgs", cgs);

  Constraint_System cs;
  bool inserted = false;
  for (Congruence_System::const_iterator i = cgs.begin(),
         cgs_end = cgs.end(); i != cgs_end; ++i) {
    const Congruence& cg = *i;
    if (cg.is_equality()) {
      Linear_Expression le(cg);
      Constraint c(le, Constraint::EQUALITY, NECESSARILY_CLOSED);
      // Enforce normalization.
      c.strong_normalize();
      // TODO: Consider stealing the row in c when adding it to cs.
      cs.insert(c);
      inserted = true;
    }
    else {
      assert(cg.is_proper_congruence());
      if (cg.is_inconsistent()) {
        set_empty();
        return;
      }
      if (!cg.is_tautological())
        throw_invalid_argument("add_congruences(cgs)",
                               "cgs has a non-trivial, proper congruence");
    }
  }
  // Only add cs if it contains something.
  if (inserted)
    add_recycled_constraints(cs);
}

void
PPL::Polyhedron::refine_with_constraint(const Constraint& c) {
  // Dimension-compatibility check.
  if (space_dim < c.space_dimension())
    throw_dimension_incompatible("refine_with_constraint(c)", "c", c);
  // If the polyhedron is known to be empty, do nothing.
  if (!marked_empty())
    refine_no_check(c);
}

void
PPL::Polyhedron::refine_with_congruence(const Congruence& cg) {
  // Dimension-compatibility check.
  if (space_dim < cg.space_dimension())
    throw_dimension_incompatible("refine_with_congruence(cg)", "cg", cg);

  // If the polyhedron is known to be empty, do nothing.
  if (marked_empty())
    return;

  // Dealing with a zero-dimensional space polyhedron first.
  if (space_dim == 0) {
    if (!cg.is_tautological())
      set_empty();
    return;
  }

  if (cg.is_equality()) {
    Linear_Expression le(cg);
    Constraint c(le, Constraint::EQUALITY, NECESSARILY_CLOSED);
    // Enforce normalization.
    c.strong_normalize();
    refine_no_check(c);
  }
}

void
PPL::Polyhedron::refine_with_constraints(const Constraint_System& cs) {
  // TODO: this is just an executable specification.

  // Dimension-compatibility check.
  const dimension_type cs_space_dim = cs.space_dimension();
  if (space_dim < cs_space_dim)
    throw_dimension_incompatible("refine_with_constraints(cs)a",
				 "cs", cs);

  // Adding no constraints is a no-op.
  if (cs.has_no_rows())
    return;

  if (space_dim == 0) {
    // In a 0-dimensional space the constraints are
    // tautologies (e.g., 0 == 0 or 1 >= 0 or 1 > 0) or
    // inconsistent (e.g., 1 == 0 or -1 >= 0 or 0 > 0).
    // In a system of constraints `begin()' and `end()' are equal
    // if and only if the system only contains tautologies.
    if (cs.begin() != cs.end())
      // There is a constraint, it must be inconsistent,
      // the polyhedron is empty.
      status.set_empty();
    return;
  }

  if (marked_empty())
    return;

  // The constraints (possibly with pending rows) are required.
  if (has_pending_generators())
    process_pending_generators();
  else if (!constraints_are_up_to_date())
    update_constraints();

  const bool adding_pending = can_have_something_pending();
  for (dimension_type i = cs.num_rows(); i-- > 0; ) {
    const Constraint& c = cs[i];

    if (c.is_necessarily_closed() || !is_necessarily_closed())
      // Since `con_sys' is not empty, the topology and space dimension
      // of the inserted constraint are automatically adjusted.
      if (adding_pending)
        con_sys.insert_pending(c);
      else
        con_sys.insert(c);
    else {
      // Here we know that *this is necessarily closed so even if c is
      // topologically closed, by barely invoking `con_sys.insert(c)' we
      // would cause a change in the topology of `con_sys', which is
      // wrong.  Thus, we insert a topology closed and "topology
      // corrected" version of `c'.
      Linear_Expression nc_expr = Linear_Expression(c);
      if (c.is_equality())
        if (adding_pending)
          con_sys.insert_pending(nc_expr == 0);
        else
          con_sys.insert(nc_expr == 0);
      else
        if (adding_pending)
          con_sys.insert_pending(nc_expr >= 0);
        else
          con_sys.insert(nc_expr >= 0);
    }
  }

  if (adding_pending)
    set_constraints_pending();
  else {
    // Constraints are not minimized and generators are not up-to-date.
    clear_constraints_minimized();
    clear_generators_up_to_date();
  }

  // Note: the constraint system may have become unsatisfiable, thus
  // we do not check for satisfiability.
  assert(OK());
}

void
PPL::Polyhedron::refine_with_congruences(const Congruence_System& cgs) {
  // Dimension-compatibility check.
  if (space_dim < cgs.space_dimension())
    throw_dimension_incompatible("refine_with_congruences(cgs)", "cgs", cgs);

  Constraint_System cs;
  bool inserted = false;
  for (Congruence_System::const_iterator i = cgs.begin(),
         cgs_end = cgs.end(); i != cgs_end; ++i) {
    if (i->is_equality()) {
      Linear_Expression le(*i);
      Constraint c(le, Constraint::EQUALITY, NECESSARILY_CLOSED);
      // Enforce normalization.
      c.strong_normalize();
      // TODO: Consider stealing the row in c when adding it to cs.
      cs.insert(c);
      inserted = true;
    }
    else if (i->is_inconsistent()) {
      set_empty();
      return;
    }
  }
  // Only add cgs if congruences were inserted into cgs, as the
  // dimension of cs must be at most that of the polyhedron.
  if (inserted)
    add_recycled_constraints(cs);
}

void
PPL::Polyhedron::unconstrain(const Variable var) {
  // Dimension-compatibility check.
  if (space_dim < var.id())
    throw_dimension_incompatible("unconstrain(var)", var.id());

  // Do something only if the polyhedron is non-empty.
  if (marked_empty()
      || (has_pending_constraints() && !process_pending_constraints())
      || (!generators_are_up_to_date() && !update_generators()))
    // Empty: do nothing.
    return;

  assert(generators_are_up_to_date());
  // Since `gen_sys' is not empty, the topology and space dimension
  // of the inserted generator are automatically adjusted.
  if (can_have_something_pending()) {
    gen_sys.insert_pending(Generator::line(var));
    set_generators_pending();
  }
  else {
    gen_sys.insert(Generator::line(var));
    // After adding the new generator,
    // constraints are no longer up-to-date.
    clear_generators_minimized();
    clear_constraints_up_to_date();
  }
  assert(OK(true));
}

void
PPL::Polyhedron::unconstrain(const Variables_Set& to_be_unconstrained) {
  // The cylindrification wrt no dimensions is a no-op.
  // This case also captures the only legal cylindrification
  // of a polyhedron in a 0-dim space.
  if (to_be_unconstrained.empty())
    return;

  // Dimension-compatibility check.
  const dimension_type min_space_dim = to_be_unconstrained.space_dimension();
  if (space_dim < min_space_dim)
    throw_dimension_incompatible("unconstrain(vs)", min_space_dim);

  // Do something only if the polyhedron is non-empty.
  if (marked_empty()
      || (has_pending_constraints() && !process_pending_constraints())
      || (!generators_are_up_to_date() && !update_generators()))
    // Empty: do nothing.
    return;

  assert(generators_are_up_to_date());
  // Since `gen_sys' is not empty, the topology and space dimension
  // of the inserted generators are automatically adjusted.
  Variables_Set::const_iterator tbu = to_be_unconstrained.begin();
  Variables_Set::const_iterator tbu_end = to_be_unconstrained.end();
  if (can_have_something_pending()) {
    for ( ; tbu != tbu_end; ++tbu)
      gen_sys.insert_pending(Generator::line(Variable(*tbu)));
    set_generators_pending();
  }
  else {
    for ( ; tbu != tbu_end; ++tbu)
      gen_sys.insert(Generator::line(Variable(*tbu)));
    // After adding the new generators,
    // constraints are no longer up-to-date.
    clear_generators_minimized();
    clear_constraints_up_to_date();
  }
  assert(OK(true));
}

void
PPL::Polyhedron::intersection_assign(const Polyhedron& y) {
  Polyhedron& x = *this;
  // Topology compatibility check.
  if (x.topology() != y.topology())
    throw_topology_incompatible("intersection_assign(y)", "y", y);
  // Dimension-compatibility check.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("intersection_assign(y)", "y", y);

  // If one of the two polyhedra is empty, the intersection is empty.
  if (x.marked_empty())
    return;
  if (y.marked_empty()) {
    x.set_empty();
    return;
  }

  // If both polyhedra are zero-dimensional,
  // then at this point they are necessarily non-empty,
  // so that their intersection is non-empty too.
  if (x.space_dim == 0)
    return;

  // Both systems of constraints have to be up-to-date,
  // possibly having pending constraints.
  if (x.has_pending_generators())
    x.process_pending_generators();
  else if (!x.constraints_are_up_to_date())
    x.update_constraints();

  if (y.has_pending_generators())
    y.process_pending_generators();
  else if (!y.constraints_are_up_to_date())
    y.update_constraints();

  // Here both systems are up-to-date and possibly have pending constraints
  // (but they cannot have pending generators).
  assert(!x.has_pending_generators() && x.constraints_are_up_to_date());
  assert(!y.has_pending_generators() && y.constraints_are_up_to_date());

  // If `x' can support pending constraints,
  // the constraints of `y' are added as pending constraints of `x'.
  if (x.can_have_something_pending()) {
    x.con_sys.add_pending_rows(y.con_sys);
    x.set_constraints_pending();
  }
  else {
    // `x' cannot support pending constraints.
    // If both constraint systems are (fully) sorted, then we can
    // merge them; otherwise we simply add the second to the first.
    if (x.con_sys.is_sorted()
	&& y.con_sys.is_sorted() && !y.has_pending_constraints())
      x.con_sys.merge_rows_assign(y.con_sys);
    else
      x.con_sys.add_rows(y.con_sys);
    // Generators are no longer up-to-date and constraints are no
    // longer minimized.
    x.clear_generators_up_to_date();
    x.clear_constraints_minimized();
  }
  assert(x.OK() && y.OK());
}

bool
PPL::Polyhedron::intersection_assign_and_minimize(const Polyhedron& y) {
  Polyhedron& x = *this;
  // Topology compatibility check.
  if (x.topology() != y.topology())
    throw_topology_incompatible("intersection_assign_and_minimize(y)",
				"y", y);
  // Dimension-compatibility check.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("intersection_assign_and_minimize(y)",
				 "y", y);

  // If one of the two polyhedra is empty, the intersection is empty.
  if (x.marked_empty())
    return false;
  if (y.marked_empty()) {
    x.set_empty();
    return false;
  }

  // If both polyhedra are zero-dimensional,
  // then at this point they are necessarily non-empty,
  // so that their intersection is non-empty too.
  if (x.space_dim == 0)
    return true;

  // `x' must be minimized and have sorted constraints.
  // `minimize()' will process any pending constraints or generators.
  if (!x.minimize())
    // We have just discovered that `x' is empty.
    return false;
  x.obtain_sorted_constraints_with_sat_c();

  // `y' must have updated, possibly pending constraints.
  if (y.has_pending_generators())
    y.process_pending_generators();
  else if (!y.constraints_are_up_to_date())
    y.update_constraints();

  bool empty;
  if (y.con_sys.num_pending_rows() > 0) {
    // Integrate `y.con_sys' as pending constraints of `x',
    // sort them in place and then call `add_and_minimize()'.
    x.con_sys.add_pending_rows(y.con_sys);
    x.con_sys.sort_pending_and_remove_duplicates();
    if (x.con_sys.num_pending_rows() == 0) {
      // All pending constraints were duplicates.
      x.clear_pending_constraints();
      assert(OK(true));
      return true;
    }
    empty = add_and_minimize(true, x.con_sys, x.gen_sys, x.sat_c);
  }
  else {
    y.obtain_sorted_constraints();
    empty = add_and_minimize(true, x.con_sys, x.gen_sys, x.sat_c, y.con_sys);
  }

  if (empty)
    x.set_empty();
  else {
    // On exit of the function `intersection_assign_and_minimize()'
    // the polyhedron is up-to-date and `sat_c' is meaningful.
    x.set_sat_c_up_to_date();
    x.clear_sat_g_up_to_date();
  }
  assert(x.OK(!empty));
  return !empty;
}

namespace {

struct Ruled_Out_Pair {
  PPL::dimension_type constraint_index;
  PPL::dimension_type num_ruled_out;
};

struct Ruled_Out_Less_Than {
  bool operator()(const Ruled_Out_Pair& x,
                  const Ruled_Out_Pair& y) const {
    return x.num_ruled_out > y.num_ruled_out;
  }
};

bool
add_to_system_and_check_independence(PPL::Linear_System& eq_sys,
                                     const PPL::Linear_Row& eq) {
  // Check if equality eqn is linear independent from eq_sys.
  assert(eq.is_line_or_equality());
  eq_sys.insert(eq);
  const PPL::dimension_type eq_sys_num_rows = eq_sys.num_rows();
  const PPL::dimension_type rank = eq_sys.gauss(eq_sys_num_rows);
  if (rank == eq_sys_num_rows)
    // eq is linear independent.
    return true;
  else {
    // eq is not linear independent.
    assert(rank == eq_sys_num_rows - 1);
    eq_sys.erase_to_end(rank);
    return false;
  }
}

/*
  Modifies the vector of pointers \p p_ineqs, setting to 0 those entries
  that point to redundant inequalities or masked equalities.
  The redundancy test is based on saturation matrix \p sat and
  on knowing that there exists \p rank nonredundant equalities
  (they are implicit, i.e., not explicitly listed in \p p_ineqs).
*/
void
drop_redundant_inequalities(std::vector<const PPL::Constraint*>& p_ineqs,
                            const PPL::Topology topology,
                            const PPL::Bit_Matrix& sat,
                            const PPL::dimension_type rank) {
  using namespace Parma_Polyhedra_Library;
  const dimension_type num_rows = p_ineqs.size();
  assert(num_rows > 0);
  // `rank' is the rank of the (implicit) system of equalities.
  const dimension_type space_dim = p_ineqs[0]->space_dimension();
  assert(space_dim > 0 && space_dim >= rank);
  const dimension_type num_coefficients
    = space_dim + (topology == NECESSARILY_CLOSED ? 0 : 1);
  const dimension_type min_sat = num_coefficients - rank;
  const dimension_type num_cols_sat = sat.num_columns();

  // Perform quick redundancy test based on the number of saturators.
  for (dimension_type i = num_rows; i-- > 0; ) {
    if (sat[i].empty())
      // Masked equalities are redundant.
      p_ineqs[i] = 0;
    else {
      const dimension_type num_sat = num_cols_sat - sat[i].count_ones();
      if (num_sat < min_sat)
        p_ineqs[i] = 0;
    }
  }

  // Re-examine remaining inequalities.
  // Iteration index `i' is _intentionally_ increasing.
  for (dimension_type i = 0; i < num_rows; ++i) {
    if (p_ineqs[i]) {
      for (dimension_type j = 0; j < num_rows; ++j) {
        bool strict_subset;
        if (p_ineqs[j] && i != j
            && subset_or_equal(sat[j], sat[i], strict_subset)) {
          if (strict_subset) {
            p_ineqs[i] = 0;
            break;
          }
          else
            // Here `sat[j] == sat[i]'.
            p_ineqs[j] = 0;
        }
      }
    }
  }
}

} // namespace

bool
PPL::Polyhedron::simplify_using_context_assign(const Polyhedron& y) {
  Polyhedron& x = *this;
  // Topology compatibility check.
  if (x.topology() != y.topology())
    throw_topology_incompatible("simplify_using_context_assign(y)", "y", y);
  // Dimension-compatibility check.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("simplify_using_context_assign(y)", "y", y);

  // Filter away the zero-dimensional case.
  if (x.space_dim == 0) {
    if (y.is_empty()) {
      x.set_zero_dim_univ();
      return false;
    }
    else
      return !x.is_empty();
  }

  // If `y' is empty, the biggest enlargement for `x' is the universe.
  if (!y.minimize()) {
    Polyhedron ph(x.topology(), x.space_dim, UNIVERSE);
    swap(ph);
    return false;
  }

  // If `x' is empty, the intersection is empty.
  if (!x.minimize()) {
    // Search for a constraint of `y' that is not a tautology.
    assert(!y.has_pending_generators() && y.constraints_are_up_to_date());
    for (dimension_type i = y.con_sys.num_rows(); i-- > 0; ) {
      const Constraint& y_con_sys_i = y.con_sys[i];
      if (!y_con_sys_i.is_tautological()) {
        // Found: we obtain a constraint `c' contradicting the one we
        // found, and assign to `x' the polyhedron `ph' with `c' as
        // the only constraint.
        Polyhedron ph(x.topology(), x.space_dim, UNIVERSE);
        Linear_Expression le(y_con_sys_i);
        switch (y_con_sys_i.type()) {
        case Constraint::EQUALITY:
          ph.refine_no_check(le == 1);
          break;
        case Constraint::NONSTRICT_INEQUALITY:
          ph.refine_no_check(le <= -1);
          break;
        case Constraint::STRICT_INEQUALITY:
          ph.refine_no_check(le == 0);
          break;
        }
        swap(ph);
        assert(OK());
	return false;
      }
    }
    // `y' is the universe: `x' cannot be enlarged.
    return false;
  }

  assert(x.constraints_are_minimized()
         && !x.has_something_pending()
         && y.generators_are_minimized()
         && !y.has_something_pending());
  const Constraint_System& x_cs = x.con_sys;
  const dimension_type x_cs_num_rows = x_cs.num_rows();
  const Generator_System& y_gs = y.gen_sys;

  // Record into `redundant_by_y' the info about which constraints of
  // `x' are redundant in the context `y'.  Count the number of
  // redundancies found.
  std::vector<bool> redundant_by_y(x_cs_num_rows, false);
  dimension_type num_redundant_by_y = 0;
  for (dimension_type i = 0; i < x_cs_num_rows; ++i)
    if (y_gs.satisfied_by_all_generators(x_cs[i])) {
      redundant_by_y[i] = true;
      ++num_redundant_by_y;
    }

  Constraint_System result_cs;

  if (num_redundant_by_y < x_cs_num_rows) {
    // Some constraints were not identified as redundant (yet?).
    const Constraint_System& y_cs = y.con_sys;
    const dimension_type y_cs_num_rows = y_cs.num_rows();
    // Compute into `z' the minimized intersection of `x' and `y'.
    const bool x_first = (x_cs_num_rows > y_cs_num_rows);
    Polyhedron z(x_first ? x : y);
    if (x_first)
      z.add_constraints(y_cs);
    else {
      // Only copy (and then recycle) the non-redundant constraints.
      Constraint_System tmp_cs;
      for (dimension_type i = 0; i < x_cs_num_rows; ++i) {
        if (!redundant_by_y[i])
          tmp_cs.insert(x_cs[i]);
      }
      z.add_recycled_constraints(tmp_cs);
    }
    if (!z.minimize()) {
      // The objective function is the default, zero.
      // We do not care about minimization or maximization, since
      // we are only interested in satisfiability.
      MIP_Problem lp;
      if (x.is_necessarily_closed()) {
        lp.add_space_dimensions_and_embed(x.space_dim);
        lp.add_constraints(y_cs);
      }
      else {
        // KLUDGE: temporarily mark `y_cs' if it was necessarily
        // closed, so that we can interpret the epsilon dimension as a
        // standard dimension. Be careful to reset the topology of `cs'
        // even on exceptional execution path.
        const_cast<Constraint_System&>(y_cs).set_necessarily_closed();
        try {
          lp.add_space_dimensions_and_embed(x.space_dim+1);
          lp.add_constraints(y_cs);
          const_cast<Constraint_System&>(y_cs).set_not_necessarily_closed();
        }
        catch (...) {
          const_cast<Constraint_System&>(y_cs).set_not_necessarily_closed();
          throw;
        }
      }
      // We apply the following heuristics here: constraints of `x' that
      // are not made redundant by `y' are added to `lp' depending on
      // the number of generators of `y' they rule out (the more generators
      // they rule out, the sooner they are added).  Of course, as soon
      // as `lp' becomes unsatisfiable, we stop adding.
      std::vector<Ruled_Out_Pair>
        ruled_out_vec(x_cs_num_rows - num_redundant_by_y);
      for (dimension_type i = 0, j = 0; i < x_cs_num_rows; ++i) {
        if (!redundant_by_y[i]) {
          const Constraint& c = x_cs[i];
          Topology_Adjusted_Scalar_Product_Sign sps(c);
          dimension_type num_ruled_out_generators = 0;
          for (Generator_System::const_iterator k = y_gs.begin(),
                 y_gs_end = y_gs.end(); k != y_gs_end; ++k) {
            const Generator& g = *k;
            const int sp_sign = sps(g, c);
            if (x.is_necessarily_closed()) {
              if (g.is_line()) {
                // Lines must saturate the constraint.
                if (sp_sign != 0)
                  goto ruled_out;
              }
              else {
                // `g' is either a ray, a point or a closure point.
                if (c.is_inequality()) {
                  // `c' is a non-strict inequality.
                  if (sp_sign < 0)
                    goto ruled_out;
                }
                else
                  // `c' is an equality.
                  if (sp_sign != 0)
                    goto ruled_out;
              }
            }
            else
              // The topology is not necessarily closed.
              switch (g.type()) {
              case Generator::LINE:
                // Lines must saturate the constraint.
                if (sp_sign != 0)
                  goto ruled_out;
                break;
              case Generator::POINT:
                // Have to perform the special test when dealing with
                // a strict inequality.
                switch (c.type()) {
                case Constraint::EQUALITY:
                  if (sp_sign != 0)
                    goto ruled_out;
                  break;
                case Constraint::NONSTRICT_INEQUALITY:
                  if (sp_sign < 0)
                    goto ruled_out;
                  break;
                case Constraint::STRICT_INEQUALITY:
                  if (sp_sign <= 0)
                    goto ruled_out;
                  break;
                }
                break;
              case Generator::RAY:
                // Intentionally fall through.
              case Generator::CLOSURE_POINT:
                if (c.is_inequality()) {
                  // Constraint `c' is either a strict or a non-strict
                  // inequality.
                  if (sp_sign < 0)
                    goto ruled_out;
                }
                else
                  // Constraint `c' is an equality.
                  if (sp_sign != 0)
                    goto ruled_out;
                break;
              }

            // If we reach this point, `g' satisfies `c'.
            continue;
          ruled_out:
            ++num_ruled_out_generators;
          }
          ruled_out_vec[j].constraint_index = i;
          ruled_out_vec[j].num_ruled_out = num_ruled_out_generators;
          ++j;
        }
      }
      std::sort(ruled_out_vec.begin(), ruled_out_vec.end(),
                Ruled_Out_Less_Than());

      for (std::vector<Ruled_Out_Pair>::const_iterator
             j = ruled_out_vec.begin(), rov_end = ruled_out_vec.end();
           j != rov_end;
           ++j) {
        const Constraint& c = x_cs[j->constraint_index];
        result_cs.insert(c);
        lp.add_constraint(c);
        MIP_Problem_Status status = lp.solve();
        if (status == UNFEASIBLE_MIP_PROBLEM) {
          Polyhedron result_ph(x.topology(), x.space_dim, UNIVERSE);
          result_ph.add_constraints(result_cs);
          x.swap(result_ph);
          assert(x.OK());
          return false;
        }
      }
      // Cannot exit from here.
      assert(false);
    }
    else {
      // Here `z' is not empty and minimized.
      assert(z.constraints_are_minimized()
             && z.generators_are_minimized()
             && !z.has_something_pending());
      const Constraint_System& z_cs = z.con_sys;
      const Generator_System& z_gs = z.gen_sys;
      const dimension_type z_gs_num_rows = z_gs.num_rows();

      // Compute the number of equalities in x_cs, y_cs and z_cs
      // (exploiting minimal form knowledge).
      dimension_type x_cs_num_eq = 0;
      while (x_cs[x_cs_num_eq].is_equality())
        ++x_cs_num_eq;
      dimension_type y_cs_num_eq = 0;
      while (y_cs[y_cs_num_eq].is_equality())
        ++y_cs_num_eq;
      dimension_type z_cs_num_eq = 0;
      while (z_cs[z_cs_num_eq].is_equality())
        ++z_cs_num_eq;
      assert(x_cs_num_eq <= z_cs_num_eq && y_cs_num_eq <= z_cs_num_eq);

      // Identify non-redundant equalities.
      Constraint_System nonred_eq;
      dimension_type num_nonred_eq = 0;
      const dimension_type needed_nonred_eq = z_cs_num_eq - y_cs_num_eq;
      Linear_System eqs(x.topology());
      if (needed_nonred_eq > 0) {
        // Populate eqs with the equalities from y.
        for (dimension_type i = 0; i < y_cs_num_eq; ++i)
          eqs.insert(y_cs[i]);
        // Try to find another `needed_nonred_eq' linear independent
        // equalities among those from x.
        for (dimension_type i = 0; i < x_cs_num_eq; ++i) {
          const Constraint& x_cs_i = x_cs[i];
          if (add_to_system_and_check_independence(eqs, x_cs_i)) {
            // x_cs_i is linear independent.
            nonred_eq.insert(x_cs_i);
            ++num_nonred_eq;
            if (num_nonred_eq == needed_nonred_eq)
              // Already found all the needed equalities.
              break;
          }
        }
        // NOTE: if num_nonred_eq < needed_nonred_eq
        // then we haven't found all the needed equalities yet:
        // this means that some inequalities from x actually holds
        // as "masked" equalities in the context of y.
        assert(eqs.num_rows() <= z_cs_num_eq);
        assert(num_nonred_eq <= needed_nonred_eq);
        assert(z_cs_num_eq - eqs.num_rows()
               == needed_nonred_eq - num_nonred_eq);
      }

      // Identify non-redundant inequalities.
      // Avoid useless copies (no modifications are needed).
      std::vector<const Constraint*> p_nonred_ineq;
      // Fill p_nonred_ineq with (pointers to) inequalities from y_cs ...
      for (dimension_type i = y_cs_num_eq; i < y_cs_num_rows; ++i)
        p_nonred_ineq.push_back(&y_cs[i]);
      // ... and (pointers to) non-redundant inequalities from x_cs.
      for (dimension_type i = x_cs_num_eq; i < x_cs_num_rows; ++i)
        if (!redundant_by_y[i])
          p_nonred_ineq.push_back(&x_cs[i]);

      const dimension_type p_nonred_ineq_size = p_nonred_ineq.size();
      const dimension_type y_cs_num_ineq = y_cs_num_rows - y_cs_num_eq;

      // Compute saturation info.
      const dimension_type sat_num_rows = p_nonred_ineq_size;
      Bit_Matrix sat(sat_num_rows, z_gs_num_rows);
      for (dimension_type i = sat_num_rows; i-- > 0; ) {
        const Constraint& nonred_ineq_i = *(p_nonred_ineq[i]);
        Bit_Row& sat_i = sat[i];
        for (dimension_type j = z_gs_num_rows; j-- > 0; )
          if (Scalar_Products::sign(nonred_ineq_i, z_gs[j]))
            sat_i.set(j);
        if (sat_i.empty() && num_nonred_eq < needed_nonred_eq) {
          // `nonred_ineq_i' is actually masking an equality
          // and we are still looking for some masked inequalities.
          // Iteration goes downwards, so the inequality comes from x_cs.
          assert(i >= y_cs_num_ineq);
          // Check if the equality is independent in eqs.
          Linear_Row masked_eq = Linear_Row(nonred_ineq_i);
          masked_eq.set_is_line_or_equality();
          masked_eq.sign_normalize();
          if (add_to_system_and_check_independence(eqs, masked_eq)) {
            // It is independent: add the _inequality_ to nonred_eq.
            nonred_eq.insert(nonred_ineq_i);
            ++num_nonred_eq;
          }
        }
      }
      // Here we have already found all the needed (masked) equalities.
      assert(num_nonred_eq == needed_nonred_eq);

      drop_redundant_inequalities(p_nonred_ineq, x.topology(),
                                  sat, z_cs_num_eq);

      // Place the nonredundant (masked) equalities into result_cs.
      result_cs.swap(nonred_eq);
      // Add to result_cs the nonredundant inequalities from x_cs,
      // i.e., those having indices no smaller than y_cs_num_ineq.
      for (dimension_type i = y_cs_num_ineq; i < p_nonred_ineq_size; ++i)
        if (p_nonred_ineq[i])
          result_cs.insert(*p_nonred_ineq[i]);
    }
  }

  Polyhedron result_ph(x.topology(), x.space_dim, UNIVERSE);
  result_ph.add_recycled_constraints(result_cs);
  x.swap(result_ph);
  assert(x.OK());
  return true;
}

void
PPL::Polyhedron::poly_hull_assign(const Polyhedron& y) {
  Polyhedron& x = *this;
  // Topology compatibility check.
  if (x.topology() != y.topology())
    throw_topology_incompatible("poly_hull_assign(y)", "y", y);
  // Dimension-compatibility check.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("poly_hull_assign(y)", "y", y);

  // The poly-hull of a polyhedron `p' with an empty polyhedron is `p'.
  if (y.marked_empty())
    return;
  if (x.marked_empty()) {
    x = y;
    return;
  }

  // If both polyhedra are zero-dimensional,
  // then at this point they are necessarily universe polyhedra,
  // so that their poly-hull is the universe polyhedron too.
  if (x.space_dim == 0)
    return;

  // Both systems of generators have to be up-to-date,
  // possibly having pending generators.
  if ((x.has_pending_constraints() && !x.process_pending_constraints())
      || (!x.generators_are_up_to_date() && !x.update_generators())) {
    // Discovered `x' empty when updating generators.
    x = y;
    return;
  }
  if ((y.has_pending_constraints() && !y.process_pending_constraints())
      || (!y.generators_are_up_to_date() && !y.update_generators()))
    // Discovered `y' empty when updating generators.
    return;

  // Here both systems are up-to-date and possibly have pending generators
  // (but they cannot have pending constraints).
  assert(!x.has_pending_constraints() && x.generators_are_up_to_date());
  assert(!y.has_pending_constraints() && y.generators_are_up_to_date());

  // If `x' can support pending generators,
  // the generators of `y' are added as pending generators of `x'.
  if (x.can_have_something_pending()) {
    x.gen_sys.add_pending_rows(y.gen_sys);
    x.set_generators_pending();
  }
  else {
    // `x' cannot support pending generators.
    // If both generator systems are (fully) sorted, then we can merge
    // them; otherwise we simply add the second to the first.
    if (x.gen_sys.is_sorted()
	&& y.gen_sys.is_sorted() && !y.has_pending_generators())
      x.gen_sys.merge_rows_assign(y.gen_sys);
    else
      x.gen_sys.add_rows(y.gen_sys);
    // Constraints are no longer up-to-date
    // and generators are no longer minimized.
    x.clear_constraints_up_to_date();
    x.clear_generators_minimized();
  }
  // At this point both `x' and `y' are not empty.
  assert(x.OK(true) && y.OK(true));
}

bool
PPL::Polyhedron::poly_hull_assign_and_minimize(const Polyhedron& y) {
  Polyhedron& x = *this;
  // Topology compatibility check.
  if (x.topology() != y.topology())
    throw_topology_incompatible("poly_hull_assign_and_minimize(y)", "y", y);
  // Dimension-compatibility check.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("poly_hull_assign_and_minimize(y)", "y", y);

  // The poly-hull of a polyhedron `p' with an empty polyhedron is `p'.
  if (y.marked_empty())
    return minimize();
  if (x.marked_empty()) {
    x = y;
    return minimize();
  }

  // If both polyhedra are zero-dimensional,
  // then at this point they are necessarily universe polyhedra,
  // so that their poly-hull is the universe polyhedron too.
  if (x.space_dim == 0)
    return true;

  // `x' must have minimized constraints and generators.
  // `minimize()' will process any pending constraints or generators.
  if (!x.minimize()) {
    // We have just discovered that `x' is empty.
    x = y;
    return minimize();
  }
  // x must have `sat_g' up-to-date and sorted generators.
  x.obtain_sorted_generators_with_sat_g();

  // `y' must have updated, possibly pending generators.
  if ((y.has_pending_constraints() && !y.process_pending_constraints())
      || (!y.generators_are_up_to_date() && !y.update_generators()))
    // We have just discovered that `y' is empty
    // (and we know that `x' is not empty).
    return true;

  if (y.gen_sys.num_pending_rows() > 0) {
    // Integrate `y.gen_sys' as pending generators of `x',
    // sort them in place and then call `add_and_minimize()'.
    x.gen_sys.add_pending_rows(y.gen_sys);
    x.gen_sys.sort_pending_and_remove_duplicates();
    if (x.gen_sys.num_pending_rows() == 0) {
      // All pending generators were duplicates.
      x.clear_pending_generators();
      assert(OK(true) && y.OK());
      return true;
    }
    add_and_minimize(false, x.gen_sys, x.con_sys, x.sat_g);
  }
  else {
    y.obtain_sorted_generators();
    add_and_minimize(false, x.gen_sys, x.con_sys, x.sat_g, y.gen_sys);
  }
  x.clear_sat_c_up_to_date();

  assert(x.OK(true) && y.OK());
  return true;
}

void
PPL::Polyhedron::poly_difference_assign(const Polyhedron& y) {
  Polyhedron& x = *this;
  // Topology compatibility check.
  if (x.topology() != y.topology())
    throw_topology_incompatible("poly_difference_assign(y)", "y", y);
  // Dimension-compatibility check.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("poly_difference_assign(y)", "y", y);

  // The difference of a polyhedron `p' and an empty polyhedron is `p'.
  if (y.marked_empty())
    return;
  // The difference of an empty polyhedron and of a polyhedron `p' is empty.
  if (x.marked_empty())
    return;

  // If both polyhedra are zero-dimensional,
  // then at this point they are necessarily universe polyhedra,
  // so that their difference is empty.
  if (x.space_dim == 0) {
    x.set_empty();
    return;
  }

  // TODO: This is just an executable specification.
  //       Have to find a more efficient method.

  if (y.contains(x)) {
    x.set_empty();
    return;
  }

  // Being lazy here is only harmful.
  // `minimize()' will process any pending constraints or generators.
  if (!y.minimize())
    return;
  x.minimize();

  Polyhedron new_polyhedron(topology(), x.space_dim, EMPTY);

  const Constraint_System& y_cs = y.constraints();
  for (Constraint_System::const_iterator i = y_cs.begin(),
	 y_cs_end = y_cs.end(); i != y_cs_end; ++i) {
    const Constraint& c = *i;
    assert(!c.is_tautological());
    assert(!c.is_inconsistent());
    // If the polyhedron `x' is included in the polyhedron defined by
    // `c', then `c' can be skipped, as adding its complement to `x'
    // would result in the empty polyhedron.  Moreover, if we operate
    // on C-polyhedra and `c' is a non-strict inequality, c _must_ be
    // skipped for otherwise we would obtain a result that is less
    // precise than the poly-difference.
    if (x.relation_with(c).implies(Poly_Con_Relation::is_included()))
      continue;
    Polyhedron z = x;
    const Linear_Expression e = Linear_Expression(c);
    switch (c.type()) {
    case Constraint::NONSTRICT_INEQUALITY:
      if (is_necessarily_closed())
	z.refine_no_check(e <= 0);
      else
	z.refine_no_check(e < 0);
      break;
    case Constraint::STRICT_INEQUALITY:
      z.refine_no_check(e <= 0);
      break;
    case Constraint::EQUALITY:
      if (is_necessarily_closed())
	// We have already filtered out the case
	// when `x' is included in `y': the result is `x'.
	return;
      else {
	Polyhedron w = x;
	w.refine_no_check(e < 0);
	new_polyhedron.poly_hull_assign(w);
	z.refine_no_check(e > 0);
      }
      break;
    }
    new_polyhedron.poly_hull_assign(z);
  }
  *this = new_polyhedron;

  assert(OK());
}

void
PPL::Polyhedron::
affine_image(const Variable var,
	     const Linear_Expression& expr,
	     Coefficient_traits::const_reference denominator) {
  // The denominator cannot be zero.
  if (denominator == 0)
    throw_invalid_argument("affine_image(v, e, d)", "d == 0");

  // Dimension-compatibility checks.
  // The dimension of `expr' should not be greater than the dimension
  // of `*this'.
  if (space_dim < expr.space_dimension())
    throw_dimension_incompatible("affine_image(v, e, d)", "e", expr);
  // `var' should be one of the dimensions of the polyhedron.
  const dimension_type var_space_dim = var.space_dimension();
  if (space_dim < var_space_dim)
    throw_dimension_incompatible("affine_image(v, e, d)", "v", var);

  if (marked_empty())
    return;

  if (expr.coefficient(var) != 0) {
    // The transformation is invertible:
    // minimality and saturators are preserved, so that
    // pending rows, if present, are correctly handled.
    if (generators_are_up_to_date()) {
      // Generator_System::affine_image() requires the third argument
      // to be a positive Coefficient.
      if (denominator > 0)
	gen_sys.affine_image(var_space_dim, expr, denominator);
      else
	gen_sys.affine_image(var_space_dim, -expr, -denominator);
    }
    if (constraints_are_up_to_date()) {
      // To build the inverse transformation,
      // after copying and negating `expr',
      // we exchange the roles of `expr[var_space_dim]' and `denominator'.
      Linear_Expression inverse;
      if (expr[var_space_dim] > 0) {
	inverse = -expr;
	inverse[var_space_dim] = denominator;
	con_sys.affine_preimage(var_space_dim, inverse, expr[var_space_dim]);
      }
      else {
	// The new denominator is negative: we negate everything once
	// more, as Constraint_System::affine_preimage() requires the
	// third argument to be positive.
	inverse = expr;
	inverse[var_space_dim] = denominator;
	neg_assign(inverse[var_space_dim]);
	con_sys.affine_preimage(var_space_dim, inverse, -expr[var_space_dim]);
      }
    }
  }
  else {
    // The transformation is not invertible.
    // We need an up-to-date system of generators.
    if (has_something_pending())
      remove_pending_to_obtain_generators();
    else if (!generators_are_up_to_date())
      minimize();
    if (!marked_empty()) {
      // Generator_System::affine_image() requires the third argument
      // to be a positive Coefficient.
      if (denominator > 0)
	gen_sys.affine_image(var_space_dim, expr, denominator);
      else
	gen_sys.affine_image(var_space_dim, -expr, -denominator);

      clear_constraints_up_to_date();
      clear_generators_minimized();
      clear_sat_c_up_to_date();
      clear_sat_g_up_to_date();
    }
  }
  assert(OK());
}


void
PPL::Polyhedron::
affine_preimage(const Variable var,
		const Linear_Expression& expr,
		Coefficient_traits::const_reference denominator) {
  // The denominator cannot be zero.
  if (denominator == 0)
    throw_invalid_argument("affine_preimage(v, e, d)", "d == 0");

  // Dimension-compatibility checks.
  // The dimension of `expr' should not be greater than the dimension
  // of `*this'.
  if (space_dim < expr.space_dimension())
    throw_dimension_incompatible("affine_preimage(v, e, d)", "e", expr);
  // `var' should be one of the dimensions of the polyhedron.
  const dimension_type var_space_dim = var.space_dimension();
  if (space_dim < var_space_dim)
    throw_dimension_incompatible("affine_preimage(v, e, d)", "v", var);

  if (marked_empty())
    return;

  if (expr.coefficient(var) != 0) {
    // The transformation is invertible:
    // minimality and saturators are preserved.
    if (constraints_are_up_to_date()) {
      // Constraint_System::affine_preimage() requires the third argument
      // to be a positive Coefficient.
      if (denominator > 0)
	con_sys.affine_preimage(var_space_dim, expr, denominator);
      else
	con_sys.affine_preimage(var_space_dim, -expr, -denominator);
    }
    if (generators_are_up_to_date()) {
      // To build the inverse transformation,
      // after copying and negating `expr',
      // we exchange the roles of `expr[var_space_dim]' and `denominator'.
      Linear_Expression inverse;
      if (expr[var_space_dim] > 0) {
	inverse = -expr;
	inverse[var_space_dim] = denominator;
	gen_sys.affine_image(var_space_dim, inverse, expr[var_space_dim]);
      }
      else {
	// The new denominator is negative:
	// we negate everything once more, as Generator_System::affine_image()
	// requires the third argument to be positive.
	inverse = expr;
	inverse[var_space_dim] = denominator;
	neg_assign(inverse[var_space_dim]);
	gen_sys.affine_image(var_space_dim, inverse, -expr[var_space_dim]);
      }
    }
  }
  else {
    // The transformation is not invertible.
    // We need an up-to-date system of constraints.
    if (has_something_pending())
      remove_pending_to_obtain_constraints();
    else if (!constraints_are_up_to_date())
      minimize();
    // Constraint_System::affine_preimage() requires the third argument
    // to be a positive Coefficient.
    if (denominator > 0)
      con_sys.affine_preimage(var_space_dim, expr, denominator);
    else
      con_sys.affine_preimage(var_space_dim, -expr, -denominator);
    // Generators, minimality and saturators are no longer valid.
    clear_generators_up_to_date();
    clear_constraints_minimized();
    clear_sat_c_up_to_date();
    clear_sat_g_up_to_date();
  }
  assert(OK());
}

void
PPL::Polyhedron::
bounded_affine_image(const Variable var,
		     const Linear_Expression& lb_expr,
		     const Linear_Expression& ub_expr,
		     Coefficient_traits::const_reference denominator) {
  // The denominator cannot be zero.
  if (denominator == 0)
    throw_invalid_argument("bounded_affine_image(v, lb, ub, d)", "d == 0");

  // Dimension-compatibility checks.
  // `var' should be one of the dimensions of the polyhedron.
  const dimension_type var_space_dim = var.space_dimension();
  if (space_dim < var_space_dim)
    throw_dimension_incompatible("bounded_affine_image(v, lb, ub, d)",
				 "v", var);
  // The dimension of `lb_expr' and `ub_expr' should not be
  // greater than the dimension of `*this'.
  const dimension_type lb_space_dim = lb_expr.space_dimension();
  if (space_dim < lb_space_dim)
    throw_dimension_incompatible("bounded_affine_image(v, lb, ub)",
				 "lb", lb_expr);
  const dimension_type ub_space_dim = ub_expr.space_dimension();
  if (space_dim < ub_space_dim)
    throw_dimension_incompatible("bounded_affine_image(v, lb, ub)",
				 "ub", ub_expr);

  // Any image of an empty polyhedron is empty.
  if (marked_empty())
    return;

  // Check whether `var' occurs in `lb_expr' and/or `ub_expr'.
  if (lb_expr.coefficient(var) == 0) {
    // Here `var' may only occur in `ub_expr'.
    generalized_affine_image(var,
			     LESS_OR_EQUAL,
			     ub_expr,
			     denominator);
    if (denominator > 0)
      refine_no_check(lb_expr <= denominator*var);
    else
      refine_no_check(denominator*var <= lb_expr);
  }
  else if (ub_expr.coefficient(var) == 0) {
    // Here `var' only occurs in `lb_expr'.
    generalized_affine_image(var,
			     GREATER_OR_EQUAL,
			     lb_expr,
			     denominator);
    if (denominator > 0)
      refine_no_check(denominator*var <= ub_expr);
    else
      refine_no_check(ub_expr <= denominator*var);
  }
  else {
    // Here `var' occurs in both `lb_expr' and `ub_expr'.
    // To ease the computation, we add an additional dimension.
    const Variable new_var = Variable(space_dim);
    add_space_dimensions_and_embed(1);
    // Constrain the new dimension to be equal to `ub_expr'.
    refine_no_check(denominator*new_var == ub_expr);
    // Apply the affine lower bound.
    generalized_affine_image(var,
			     GREATER_OR_EQUAL,
			     lb_expr,
			     denominator);
    if (!marked_empty())
      // Now apply the affine upper bound, as recorded in `new_var'.
      refine_no_check(new_var >= var);
    // Remove the temporarily added dimension.
    remove_higher_space_dimensions(space_dim-1);
  }
  assert(OK());
}

void
PPL::Polyhedron::
bounded_affine_preimage(const Variable var,
			const Linear_Expression& lb_expr,
			const Linear_Expression& ub_expr,
			Coefficient_traits::const_reference denominator) {
  // The denominator cannot be zero.
  if (denominator == 0)
    throw_invalid_argument("bounded_affine_preimage(v, lb, ub, d)", "d == 0");

  // Dimension-compatibility checks.
  // `var' should be one of the dimensions of the polyhedron.
  const dimension_type var_space_dim = var.space_dimension();
  if (space_dim < var_space_dim)
    throw_dimension_incompatible("bounded_affine_preimage(v, lb, ub, d)",
				 "v", var);
  // The dimension of `lb_expr' and `ub_expr' should not be
  // greater than the dimension of `*this'.
  const dimension_type lb_space_dim = lb_expr.space_dimension();
  if (space_dim < lb_space_dim)
    throw_dimension_incompatible("bounded_affine_preimage(v, lb, ub)",
				 "lb", lb_expr);
  const dimension_type ub_space_dim = ub_expr.space_dimension();
  if (space_dim < ub_space_dim)
    throw_dimension_incompatible("bounded_affine_preimage(v, lb, ub)",
				 "ub", ub_expr);

  // Any preimage of an empty polyhedron is empty.
  if (marked_empty())
    return;

  // Check whether `var' occurs in neither `lb_expr' nor `ub_expr'.
  if (lb_expr.coefficient(var) == 0 && ub_expr.coefficient(var) == 0) {
    if (denominator > 0) {
      refine_no_check(lb_expr <= denominator*var);
      refine_no_check(denominator*var <= ub_expr);
    }
    else {
      refine_no_check(ub_expr <= denominator*var);
      refine_no_check(denominator*var <= lb_expr);
    }
    unconstrain(var);
  }
  else {
    // Here `var' occurs in `lb_expr' or `ub_expr'.
    // To ease the computation, add an additional dimension.
    const Variable new_var = Variable(space_dim);
    add_space_dimensions_and_embed(1);
    // Swap dimensions `var' and `new_var'.
    std::vector<dimension_type> swapping_cycle;
    swapping_cycle.push_back(var_space_dim);
    swapping_cycle.push_back(space_dim);
    swapping_cycle.push_back(0);
    if (constraints_are_up_to_date())
      con_sys.permute_columns(swapping_cycle);
    if (generators_are_up_to_date())
      gen_sys.permute_columns(swapping_cycle);
    // Constrain the new dimension as dictated by `lb_expr' and `ub_expr'.
    // (we force minimization because we will need the generators).
    if (denominator > 0) {
      refine_no_check(lb_expr <= denominator*new_var);
      refine_no_check(denominator*new_var <= ub_expr);
    }
    else {
      refine_no_check(ub_expr <= denominator*new_var);
      refine_no_check(denominator*new_var <= lb_expr);
    }
    // Remove the temporarily added dimension.
    remove_higher_space_dimensions(space_dim-1);
  }
  assert(OK());
}

void
PPL::Polyhedron::
generalized_affine_image(const Variable var,
			 const Relation_Symbol relsym,
			 const Linear_Expression& expr,
			 Coefficient_traits::const_reference denominator) {
  // The denominator cannot be zero.
  if (denominator == 0)
    throw_invalid_argument("generalized_affine_image(v, r, e, d)", "d == 0");

  // Dimension-compatibility checks.
  // The dimension of `expr' should not be greater than the dimension
  // of `*this'.
  if (space_dim < expr.space_dimension())
    throw_dimension_incompatible("generalized_affine_image(v, r, e, d)",
				 "e", expr);
  // `var' should be one of the dimensions of the polyhedron.
  const dimension_type var_space_dim = var.space_dimension();
  if (space_dim < var_space_dim)
    throw_dimension_incompatible("generalized_affine_image(v, r, e, d)",
				 "v", var);

  // Strict relation symbols are only admitted for NNC polyhedra.
  if (is_necessarily_closed()
      && (relsym == LESS_THAN || relsym == GREATER_THAN))
    throw_invalid_argument("generalized_affine_image(v, r, e, d)",
			   "r is a strict relation symbol");
  // The relation symbol cannot be a disequality.
  if (relsym == NOT_EQUAL)
    throw_invalid_argument("generalized_affine_image(v, r, e, d)",
			   "r is the disequality relation symbol");

  // First compute the affine image.
  affine_image(var, expr, denominator);

  if (relsym == EQUAL)
    // The affine relation is indeed an affine function.
    return;

  // Any image of an empty polyhedron is empty.
  // Note: DO check for emptiness here, as we will later add a ray.
  if (is_empty())
    return;

  switch (relsym) {
  case LESS_OR_EQUAL:
    add_generator(ray(-var));
    break;
  case GREATER_OR_EQUAL:
    add_generator(ray(var));
    break;
  case LESS_THAN:
  // Intentionally fall through.
  case GREATER_THAN:
    {
      // The relation symbol is strict.
      assert(!is_necessarily_closed());
      // While adding the ray, we minimize the generators
      // in order to avoid adding too many redundant generators later.
      add_generator(ray(relsym == GREATER_THAN ? var : -var));
      minimize();
      // We split each point of the generator system into two generators:
      // a closure point, having the same coordinates of the given point,
      // and another point, having the same coordinates for all but the
      // `var' dimension, which is displaced along the direction of the
      // newly introduced ray.
      const dimension_type eps_index = space_dim + 1;
      for (dimension_type i =  gen_sys.num_rows(); i-- > 0; )
	if (gen_sys[i].is_point()) {
	  Generator& g = gen_sys[i];
	  // Add a `var'-displaced copy of `g' to the generator system.
	  gen_sys.add_row(g);
	  if (relsym == GREATER_THAN)
	    ++gen_sys[gen_sys.num_rows()-1][var_space_dim];
	  else
	    --gen_sys[gen_sys.num_rows()-1][var_space_dim];
	  // Transform `g' into a closure point.
	  g[eps_index] = 0;
	}
      clear_constraints_up_to_date();
      clear_generators_minimized();
      gen_sys.set_sorted(false);
      clear_sat_c_up_to_date();
      clear_sat_g_up_to_date();
    }
    break;
  default:
    // The EQUAL and NOT_EQUAL cases have been already dealt with.
    throw std::runtime_error("PPL internal error");
  }
  assert(OK());
}

void
PPL::Polyhedron::
generalized_affine_preimage(const Variable var,
			    const Relation_Symbol relsym,
			    const Linear_Expression& expr,
			    Coefficient_traits::const_reference denominator) {
  // The denominator cannot be zero.
  if (denominator == 0)
    throw_invalid_argument("generalized_affine_preimage(v, r, e, d)",
			   "d == 0");

  // Dimension-compatibility checks.
  // The dimension of `expr' should not be greater than the dimension
  // of `*this'.
  if (space_dim < expr.space_dimension())
    throw_dimension_incompatible("generalized_affine_preimage(v, r, e, d)",
				 "e", expr);
  // `var' should be one of the dimensions of the polyhedron.
  const dimension_type var_space_dim = var.space_dimension();
  if (space_dim < var_space_dim)
    throw_dimension_incompatible("generalized_affine_preimage(v, r, e, d)",
				 "v", var);

  // Strict relation symbols are only admitted for NNC polyhedra.
  if (is_necessarily_closed()
      && (relsym == LESS_THAN || relsym == GREATER_THAN))
    throw_invalid_argument("generalized_affine_preimage(v, r, e, d)",
			   "r is a strict relation symbol");
  // The relation symbol cannot be a disequality.
  if (relsym == NOT_EQUAL)
    throw_invalid_argument("generalized_affine_preimage(v, r, e, d)",
			   "r is the disequality relation symbol");

  // Check whether the affine relation is indeed an affine function.
  if (relsym == EQUAL) {
    affine_preimage(var, expr, denominator);
    return;
  }

  // Compute the reversed relation symbol to simplify later coding.
  Relation_Symbol reversed_relsym;
  switch (relsym) {
  case LESS_THAN:
    reversed_relsym = GREATER_THAN;
    break;
  case LESS_OR_EQUAL:
    reversed_relsym = GREATER_OR_EQUAL;
    break;
  case GREATER_OR_EQUAL:
    reversed_relsym = LESS_OR_EQUAL;
    break;
  case GREATER_THAN:
    reversed_relsym = LESS_THAN;
    break;
  default:
    // The EQUAL and NOT_EQUAL cases have been already dealt with.
    throw std::runtime_error("PPL internal error");
  }

  // Check whether the preimage of this affine relation can be easily
  // computed as the image of its inverse relation.
  const Coefficient& var_coefficient = expr.coefficient(var);
  if (var_coefficient != 0) {
    Linear_Expression inverse_expr
      = expr - (denominator + var_coefficient) * var;
    PPL_DIRTY_TEMP_COEFFICIENT(inverse_denominator);
    neg_assign(inverse_denominator, var_coefficient);
    Relation_Symbol inverse_relsym
      = (sgn(denominator) == sgn(inverse_denominator))
      ? relsym : reversed_relsym;
    generalized_affine_image(var, inverse_relsym, inverse_expr,
			     inverse_denominator);
    return;
  }

  // Here `var_coefficient == 0', so that the preimage cannot
  // be easily computed by inverting the affine relation.
  // Shrink the polyhedron by adding the constraint induced
  // by the affine relation.
  const Relation_Symbol corrected_relsym
    = (denominator > 0) ? relsym : reversed_relsym;
  switch (corrected_relsym) {
  case LESS_THAN:
    refine_no_check(denominator*var < expr);
    break;
  case LESS_OR_EQUAL:
    refine_no_check(denominator*var <= expr);
    break;
  case GREATER_OR_EQUAL:
    refine_no_check(denominator*var >= expr);
    break;
  case GREATER_THAN:
    refine_no_check(denominator*var > expr);
    break;
  default:
    // The EQUAL and NOT_EQUAL cases have been already dealt with.
    throw std::runtime_error("PPL internal error");
  }
  unconstrain(var);
  assert(OK());
}

void
PPL::Polyhedron::generalized_affine_image(const Linear_Expression& lhs,
					  const Relation_Symbol relsym,
					  const Linear_Expression& rhs) {
  // Dimension-compatibility checks.
  // The dimension of `lhs' should not be greater than the dimension
  // of `*this'.
  dimension_type lhs_space_dim = lhs.space_dimension();
  if (space_dim < lhs_space_dim)
    throw_dimension_incompatible("generalized_affine_image(e1, r, e2)",
				 "e1", lhs);
  // The dimension of `rhs' should not be greater than the dimension
  // of `*this'.
  const dimension_type rhs_space_dim = rhs.space_dimension();
  if (space_dim < rhs_space_dim)
    throw_dimension_incompatible("generalized_affine_image(e1, r, e2)",
				 "e2", rhs);

  // Strict relation symbols are only admitted for NNC polyhedra.
  if (is_necessarily_closed()
      && (relsym == LESS_THAN || relsym == GREATER_THAN))
    throw_invalid_argument("generalized_affine_image(e1, r, e2)",
			   "r is a strict relation symbol");
  // The relation symbol cannot be a disequality.
  if (relsym == NOT_EQUAL)
    throw_invalid_argument("generalized_affine_image(e1, r, e2)",
			   "r is the disequality relation symbol");

  // Any image of an empty polyhedron is empty.
  if (marked_empty())
    return;

  // Compute the actual space dimension of `lhs',
  // i.e., the highest dimension having a non-zero coefficient in `lhs'.
  for ( ; lhs_space_dim > 0; lhs_space_dim--)
    if (lhs.coefficient(Variable(lhs_space_dim - 1)) != 0)
      break;
  // If all variables have a zero coefficient, then `lhs' is a constant:
  // we can simply add the constraint `lhs relsym rhs'.
  if (lhs_space_dim == 0) {
    switch (relsym) {
    case LESS_THAN:
      refine_no_check(lhs < rhs);
      break;
    case LESS_OR_EQUAL:
      refine_no_check(lhs <= rhs);
      break;
    case EQUAL:
      refine_no_check(lhs == rhs);
      break;
    case GREATER_OR_EQUAL:
      refine_no_check(lhs >= rhs);
      break;
    case GREATER_THAN:
      refine_no_check(lhs > rhs);
      break;
    case NOT_EQUAL:
      // The NOT_EQUAL case has been already dealt with.
      throw std::runtime_error("PPL internal error");
    }
    return;
  }

  // Gather in `new_lines' the collections of all the lines having
  // the direction of variables occurring in `lhs'.
  // While at it, check whether or not there exists a variable
  // occurring in both `lhs' and `rhs'.
  Generator_System new_lines;
  bool lhs_vars_intersects_rhs_vars = false;
  for (dimension_type i = lhs_space_dim; i-- > 0; )
    if (lhs.coefficient(Variable(i)) != 0) {
      new_lines.insert(line(Variable(i)));
      if (rhs.coefficient(Variable(i)) != 0)
	lhs_vars_intersects_rhs_vars = true;
    }

  if (lhs_vars_intersects_rhs_vars) {
    // Some variables in `lhs' also occur in `rhs'.
    // To ease the computation, we add an additional dimension.
    const Variable new_var = Variable(space_dim);
    add_space_dimensions_and_embed(1);

    // Constrain the new dimension to be equal to the right hand side.
    // (check for emptiness because we will add lines).
    refine_no_check(new_var == rhs);
    if (!is_empty()) {
      // Existentially quantify the variables in the left hand side.
      add_recycled_generators(new_lines);

      // Constrain the new dimension so that it is related to
      // the left hand side as dictated by `relsym'
      // (we force minimization because we will need the generators).
      switch (relsym) {
      case LESS_THAN:
	refine_no_check(lhs < new_var);
	break;
      case LESS_OR_EQUAL:
	refine_no_check(lhs <= new_var);
	break;
      case EQUAL:
	refine_no_check(lhs == new_var);
	break;
      case GREATER_OR_EQUAL:
	refine_no_check(lhs >= new_var);
	break;
      case GREATER_THAN:
	refine_no_check(lhs > new_var);
	break;
      case NOT_EQUAL:
	// The NOT_EQUAL case has been already dealt with.
	throw std::runtime_error("PPL internal error");
      }
    }
    // Remove the temporarily added dimension.
    remove_higher_space_dimensions(space_dim-1);
  }
  else {
    // `lhs' and `rhs' variables are disjoint:
    // there is no need to add a further dimension.

    // Any image of an empty polyhedron is empty.
    // Note: DO check for emptiness here, as we will add lines.
    if (is_empty())
      return;

    // Existentially quantify the variables in the left hand side.
    add_recycled_generators(new_lines);

    // Constrain the left hand side expression so that it is related to
    // the right hand side expression as dictated by `relsym'.
    switch (relsym) {
    case LESS_THAN:
      refine_no_check(lhs < rhs);
      break;
    case LESS_OR_EQUAL:
      refine_no_check(lhs <= rhs);
      break;
    case EQUAL:
      refine_no_check(lhs == rhs);
      break;
    case GREATER_OR_EQUAL:
      refine_no_check(lhs >= rhs);
      break;
    case GREATER_THAN:
      refine_no_check(lhs > rhs);
      break;
    case NOT_EQUAL:
      // The NOT_EQUAL case has been already dealt with.
      throw std::runtime_error("PPL internal error");
    }
  }
  assert(OK());
}

void
PPL::Polyhedron::generalized_affine_preimage(const Linear_Expression& lhs,
					     const Relation_Symbol relsym,
					     const Linear_Expression& rhs) {
  // Dimension-compatibility checks.
  // The dimension of `lhs' should not be greater than the dimension
  // of `*this'.
  dimension_type lhs_space_dim = lhs.space_dimension();
  if (space_dim < lhs_space_dim)
    throw_dimension_incompatible("generalized_affine_preimage(e1, r, e2)",
				 "e1", lhs);
  // The dimension of `rhs' should not be greater than the dimension
  // of `*this'.
  const dimension_type rhs_space_dim = rhs.space_dimension();
  if (space_dim < rhs_space_dim)
    throw_dimension_incompatible("generalized_affine_preimage(e1, r, e2)",
				 "e2", rhs);

  // Strict relation symbols are only admitted for NNC polyhedra.
  if (is_necessarily_closed()
      && (relsym == LESS_THAN || relsym == GREATER_THAN))
    throw_invalid_argument("generalized_affine_preimage(e1, r, e2)",
			   "r is a strict relation symbol");
  // The relation symbol cannot be a disequality.
  if (relsym == NOT_EQUAL)
    throw_invalid_argument("generalized_affine_preimage(e1, r, e2)",
			   "r is the disequality relation symbol");

  // Any preimage of an empty polyhedron is empty.
  if (marked_empty())
    return;

  // Compute the actual space dimension of `lhs',
  // i.e., the highest dimension having a non-zero coefficient in `lhs'.
  for ( ; lhs_space_dim > 0; lhs_space_dim--)
    if (lhs.coefficient(Variable(lhs_space_dim - 1)) != 0)
      break;

  // If all variables have a zero coefficient, then `lhs' is a constant:
  // in this case, preimage and image happen to be the same.
  if (lhs_space_dim == 0) {
    generalized_affine_image(lhs, relsym, rhs);
    return;
  }

  // Gather in `new_lines' the collections of all the lines having
  // the direction of variables occurring in `lhs'.
  // While at it, check whether or not there exists a variable
  // occurring in both `lhs' and `rhs'.
  Generator_System new_lines;
  bool lhs_vars_intersects_rhs_vars = false;
  for (dimension_type i = lhs_space_dim; i-- > 0; )
    if (lhs.coefficient(Variable(i)) != 0) {
      new_lines.insert(line(Variable(i)));
      if (rhs.coefficient(Variable(i)) != 0)
	lhs_vars_intersects_rhs_vars = true;
    }

  if (lhs_vars_intersects_rhs_vars) {
    // Some variables in `lhs' also occur in `rhs'.
    // To ease the computation, we add an additional dimension.
    const Variable new_var = Variable(space_dim);
    add_space_dimensions_and_embed(1);

    // Constrain the new dimension to be equal to `lhs'
    // (also check for emptiness because we have to add lines).
    refine_no_check(new_var == lhs);
    if (!is_empty()) {
      // Existentially quantify the variables in the left hand side.
      add_recycled_generators(new_lines);

      // Constrain the new dimension so that it is related to
      // the right hand side as dictated by `relsym'.
      switch (relsym) {
      case LESS_THAN:
	refine_no_check(new_var < rhs);
	break;
      case LESS_OR_EQUAL:
	refine_no_check(new_var <= rhs);
	break;
      case EQUAL:
	refine_no_check(new_var == rhs);
	break;
      case GREATER_OR_EQUAL:
	refine_no_check(new_var >= rhs);
	break;
      case GREATER_THAN:
	refine_no_check(new_var > rhs);
	break;
      case NOT_EQUAL:
	// The NOT_EQUAL case has been already dealt with.
	throw std::runtime_error("PPL internal error");
      }
    }
    // Remove the temporarily added dimension.
    remove_higher_space_dimensions(space_dim-1);
  }
  else {
    // `lhs' and `rhs' variables are disjoint:
    // there is no need to add a further dimension.

    // Constrain the left hand side expression so that it is related to
    // the right hand side expression as dictated by `relsym'.
    switch (relsym) {
    case LESS_THAN:
      refine_no_check(lhs < rhs);
      break;
    case LESS_OR_EQUAL:
      refine_no_check(lhs <= rhs);
      break;
    case EQUAL:
      refine_no_check(lhs == rhs);
      break;
    case GREATER_OR_EQUAL:
      refine_no_check(lhs >= rhs);
      break;
    case GREATER_THAN:
      refine_no_check(lhs > rhs);
      break;
    case NOT_EQUAL:
      // The NOT_EQUAL case has been already dealt with.
      throw std::runtime_error("PPL internal error");
    }
    // Any image of an empty polyhedron is empty.
    // Note: DO check for emptiness here, as we will add lines.
    if (is_empty())
      return;
    // Existentially quantify all the variables occurring in `lhs'.
    add_recycled_generators(new_lines);
  }
  assert(OK());
}

void
PPL::Polyhedron::time_elapse_assign(const Polyhedron& y) {
  Polyhedron& x = *this;
  // Topology compatibility check.
  if (x.topology() != y.topology())
    throw_topology_incompatible("time_elapse_assign(y)", "y", y);
  // Dimension-compatibility checks.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("time_elapse_assign(y)", "y", y);

  // Dealing with the zero-dimensional case.
  if (x.space_dim == 0) {
    if (y.marked_empty())
      x.set_empty();
    return;
  }

  // If either one of `x' or `y' is empty, the result is empty too.
  if (x.marked_empty() || y.marked_empty()
      || (x.has_pending_constraints() && !x.process_pending_constraints())
      || (!x.generators_are_up_to_date() && !x.update_generators())
      || (y.has_pending_constraints() && !y.process_pending_constraints())
      || (!y.generators_are_up_to_date() && !y.update_generators())) {
    x.set_empty();
    return;
  }

  // At this point both generator systems are up-to-date,
  // possibly containing pending generators.
  Generator_System gs = y.gen_sys;
  dimension_type gs_num_rows = gs.num_rows();

  if (!x.is_necessarily_closed())
    // `x' and `y' are NNC polyhedra.
    for (dimension_type i = gs_num_rows; i-- > 0; )
      switch (gs[i].type()) {
      case Generator::POINT:
	// The points of `gs' can be erased,
	// since their role can be played by closure points.
	--gs_num_rows;
	std::swap(gs[i], gs[gs_num_rows]);
	break;
      case Generator::CLOSURE_POINT:
	{
	  Generator& cp = gs[i];
	  // If it is the origin, erase it.
	  if (cp.all_homogeneous_terms_are_zero()) {
	    --gs_num_rows;
	    std::swap(cp, gs[gs_num_rows]);
	  }
	  // Otherwise, transform the closure point into a ray.
	  else {
	    cp[0] = 0;
	    // Enforce normalization.
	    cp.normalize();
	  }
	}
	break;
      default:
	// For rays and lines, nothing to be done.
	break;
      }
  else
    // `x' and `y' are C polyhedra.
    for (dimension_type i = gs_num_rows; i-- > 0; )
      switch (gs[i].type()) {
      case Generator::POINT:
	{
	  Generator& p = gs[i];
	  // If it is the origin, erase it.
	  if (p.all_homogeneous_terms_are_zero()) {
	    --gs_num_rows;
	    std::swap(p, gs[gs_num_rows]);
	  }
	  // Otherwise, transform the point into a ray.
	  else {
	    p[0] = 0;
	    // Enforce normalization.
	    p.normalize();
	  }
	}
	break;
      default:
	// For rays and lines, nothing to be done.
	break;
      }
  // If it was present, erase the origin point or closure point,
  // which cannot be transformed into a valid ray or line.
  // For NNC polyhedra, also erase all the points of `gs',
  // whose role can be played by the closure points.
  // These have been previously moved to the end of `gs'.
  gs.erase_to_end(gs_num_rows);
  gs.unset_pending_rows();

  // `gs' may now have no rows.
  // Namely, this happens when `y' was the singleton polyhedron
  // having the origin as the one and only point.
  // In such a case, the resulting polyhedron is equal to `x'.
  if (gs_num_rows == 0)
    return;

  // If the polyhedron can have something pending, we add `gs'
  // to `gen_sys' as pending rows
  if (x.can_have_something_pending()) {
    x.gen_sys.add_pending_rows(gs);
    x.set_generators_pending();
  }
  // Otherwise, the two systems are merged.
  // `Linear_System::merge_rows_assign()' requires both systems to be sorted.
  else {
    if (!x.gen_sys.is_sorted())
      x.gen_sys.sort_rows();
    gs.sort_rows();
    x.gen_sys.merge_rows_assign(gs);
    // Only the system of generators is up-to-date.
    x.clear_constraints_up_to_date();
    x.clear_generators_minimized();
  }
  assert(x.OK(true) && y.OK(true));
}

void
PPL::Polyhedron::topological_closure_assign() {
  // Necessarily closed polyhedra are trivially closed.
  if (is_necessarily_closed())
    return;
  // Any empty or zero-dimensional polyhedron is closed.
  if (marked_empty() || space_dim == 0)
    return;

  // The computation can be done using constraints or generators.
  // If we use constraints, we will change them, so that having pending
  // constraints would be useless. If we use generators, we add generators,
  // so that having pending generators still makes sense.

  // Process any pending constraints.
  if (has_pending_constraints() && !process_pending_constraints())
    return;

  // Use constraints only if they are available and
  // there are no pending generators.
  if (!has_pending_generators() && constraints_are_up_to_date()) {
    const dimension_type eps_index = space_dim + 1;
    bool changed = false;
    // Transform all strict inequalities into non-strict ones.
    for (dimension_type i = con_sys.num_rows(); i-- > 0; ) {
      Constraint& c = con_sys[i];
      if (c[eps_index] < 0 && !c.is_tautological()) {
	c[eps_index] = 0;
	// Enforce normalization.
	c.normalize();
	changed = true;
      }
    }
    if (changed) {
      con_sys.insert(Constraint::epsilon_leq_one());
      con_sys.set_sorted(false);
      // After changing the system of constraints, the generators
      // are no longer up-to-date and the constraints are no longer
      // minimized.
      clear_generators_up_to_date();
      clear_constraints_minimized();
    }
  }
  else {
    // Here we use generators, possibly keeping constraints.
    assert(generators_are_up_to_date());
    // Add the corresponding point to each closure point.
    gen_sys.add_corresponding_points();
    if (can_have_something_pending())
      set_generators_pending();
    else {
      // We cannot have pending generators; this also implies
      // that generators may have lost their sortedness.
      gen_sys.unset_pending_rows();
      gen_sys.set_sorted(false);
      // Constraints are not up-to-date and generators are not minimized.
      clear_constraints_up_to_date();
      clear_generators_minimized();
    }
  }
  assert(OK());
}

/*! \relates Parma_Polyhedra_Library::Polyhedron */
bool
PPL::operator==(const Polyhedron& x, const Polyhedron& y) {
  // If the two polyhedra are topology-incompatible or dimension-incompatible,
  // then they cannot be the same polyhedron.
  if (x.topology() != y.topology() || x.space_dim != y.space_dim)
    return false;

  if (x.marked_empty())
    return y.is_empty();
  else if (y.marked_empty())
    return x.is_empty();
  else if (x.space_dim == 0)
    return true;

  switch (x.quick_equivalence_test(y)) {
  case Polyhedron::TVB_TRUE:
    return true;

  case Polyhedron::TVB_FALSE:
    return false;

  default:
    if (x.is_included_in(y))
      if (x.marked_empty())
	return y.is_empty();
      else
	return y.is_included_in(x);
    else
      return false;
  }
}

bool
PPL::Polyhedron::contains(const Polyhedron& y) const {
  const Polyhedron& x = *this;

  // Topology compatibility check.
  if (x.topology() != y.topology())
    throw_topology_incompatible("contains(y)", "y", y);

  // Dimension-compatibility check.
  if (x.space_dim != y.space_dim)
    throw_dimension_incompatible("contains(y)", "y", y);

  if (y.marked_empty())
    return true;
  else if (x.marked_empty())
    return y.is_empty();
  else if (y.space_dim == 0)
    return true;
  else if (x.quick_equivalence_test(y) == Polyhedron::TVB_TRUE)
    return true;
  else
    return y.is_included_in(x);
}

bool
PPL::Polyhedron::is_disjoint_from(const Polyhedron& y) const {
  Polyhedron z = *this;
  z.intersection_assign(y);
  return z.is_empty();
}

void
PPL::Polyhedron::ascii_dump(std::ostream& s) const {
  s << "space_dim " << space_dim << "\n";
  status.ascii_dump(s);
  s << "\ncon_sys ("
    << (constraints_are_up_to_date() ? "" : "not_")
    << "up-to-date)"
    << "\n";
  con_sys.ascii_dump(s);
  s << "\ngen_sys ("
    << (generators_are_up_to_date() ? "" : "not_")
    << "up-to-date)"
    << "\n";
  gen_sys.ascii_dump(s);
  s << "\nsat_c\n";
  sat_c.ascii_dump(s);
  s << "\nsat_g\n";
  sat_g.ascii_dump(s);
  s << "\n";
}

PPL_OUTPUT_DEFINITIONS(Polyhedron)

bool
PPL::Polyhedron::ascii_load(std::istream& s) {
  std::string str;

  if (!(s >> str) || str != "space_dim")
    return false;

  if (!(s >> space_dim))
    return false;

  if (!status.ascii_load(s))
    return false;

  if (!(s >> str) || str != "con_sys")
    return false;

  if (!(s >> str) || (str != "(not_up-to-date)" && str != "(up-to-date)"))
    return false;

  if (!con_sys.ascii_load(s))
    return false;

  if (!(s >> str) || str != "gen_sys")
    return false;

  if (!(s >> str) || (str != "(not_up-to-date)" && str != "(up-to-date)"))
    return false;

  if (!gen_sys.ascii_load(s))
    return false;

  if (!(s >> str) || str != "sat_c")
    return false;

  if (!sat_c.ascii_load(s))
    return false;

  if (!(s >> str) || str != "sat_g")
    return false;

  if (!sat_g.ascii_load(s))
    return false;

  // Check invariants.
  assert(OK());
  return true;
}

PPL::memory_size_type
PPL::Polyhedron::external_memory_in_bytes() const {
  return
    con_sys.external_memory_in_bytes()
    + gen_sys.external_memory_in_bytes()
    + sat_c.external_memory_in_bytes()
    + sat_g.external_memory_in_bytes();
}

/*! \relates Parma_Polyhedra_Library::Polyhedron */
std::ostream&
PPL::IO_Operators::operator<<(std::ostream& s, const Polyhedron& ph) {
  if (ph.is_empty())
    s << "false";
  else
    s << ph.minimized_constraints();
  return s;
}
