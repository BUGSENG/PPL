/* GenSys class implementation (non-inline functions).
   Copyright (C) 2001-2003 Roberto Bagnara <bagnara@cs.unipr.it>

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

#include "GenSys.defs.hh"
#include "GenSys.inlines.hh"

#include "Constraint.defs.hh"
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

namespace PPL = Parma_Polyhedra_Library;

bool
PPL::GenSys::adjust_topology_and_dimension(Topology new_topology,
					   dimension_type new_space_dim) {
  assert(space_dimension() <= new_space_dim);

  if (num_rows() == 0) {
    if (topology() != new_topology)
      if (is_necessarily_closed())
	set_not_necessarily_closed();
      else
	set_necessarily_closed();
    assert(OK());
    return true;
  }

  // Here `num_rows() > 0'.
  dimension_type old_space_dim = space_dimension();
  dimension_type cols_to_be_added = new_space_dim - old_space_dim;
  Topology old_topology = topology();

  if (cols_to_be_added > 0)
    if (old_topology != new_topology)
      if (new_topology == NECESSARILY_CLOSED) {
	// A NOT_NECESSARILY_CLOSED generator system
	// can be converted to a NECESSARILY_CLOSED one
	// only if it does not contain closure points.
	if (has_closure_points())
	  return false;
	// Remove the epsilon column and, after that,
	// add the missing dimensions. This ensures that
	// non-zero epsilon coefficients will be cleared.
	remove_columns(old_space_dim + 1);
	set_necessarily_closed();
	add_zero_columns(cols_to_be_added);
      }
      else {
	// A NECESSARILY_CLOSED generator system is converted into
	// a NOT_NECESSARILY_CLOSED one by adding a further column
	// and setting the epsilon coordinate of all points to 1.
	// Note: normalization is preserved.
	add_zero_columns(++cols_to_be_added);
	GenSys& gs = *this;
	dimension_type eps_index = new_space_dim + 1;
	for (dimension_type i = num_rows(); i-- > 0; )
	  gs[i][eps_index] = gs[i][0];
	set_not_necessarily_closed();
      }
    else {
      // Topologies agree: first add the required zero columns ...
      add_zero_columns(cols_to_be_added);
      // ... and, if needed, move the epsilon coefficients
      // to the new last column.
      if (old_topology == NOT_NECESSARILY_CLOSED)
	swap_columns(old_space_dim + 1, new_space_dim + 1);
    }
  else
    // Here `cols_to_be_added == 0'.
    if (old_topology != new_topology)
      if (new_topology == NECESSARILY_CLOSED) {
	// A NOT_NECESSARILY_CLOSED generator system
	// can be converted in to a NECESSARILY_CLOSED one
	// only if it does not contain closure points.
	if (has_closure_points())
	  return false;
	// We just remove the column of the epsilon coefficients.
	remove_columns(old_space_dim + 1);
	set_necessarily_closed();
      }
      else {
	// Add the column of the epsilon coefficients
	// and set the epsilon coordinate of all points to 1.
	// Note: normalization is preserved.
	add_zero_columns(1);
	GenSys& gs = *this;
	dimension_type eps_index = new_space_dim + 1;
	for (dimension_type i = num_rows(); i-- > 0; )
	  gs[i][eps_index] = gs[i][0];
	set_not_necessarily_closed();
      }
  // We successfully adjusted dimensions and topology.
  assert(OK());
  return true;
}

// TODO: would be worth to avoid adding closure points
// that already are in the system of generators?
// To do this efficiently we could sort the system and
// perform insertions keeping its sortedness.
void
PPL::GenSys::add_corresponding_closure_points() {
  assert(!is_necessarily_closed());
  // NOTE: we always add (pending) rows at the end of the generator system.
  // Updating `index_first_pending', if needed, is done by the caller.
  GenSys& gs = *this;
  dimension_type n_rows = gs.num_rows();
  dimension_type eps_index = gs.num_columns() - 1;
  for (dimension_type i = n_rows; i-- > 0; ) {
    const Generator& g = gs[i];
    if (g[eps_index] > 0) {
      // `g' is a point: adding the closure point.
      Generator cp = g;
      cp[eps_index] = 0;
      // Enforcing normalization.
      cp.normalize();
      gs.add_pending_row(cp);
    }
  }
}


// TODO: would be worth to avoid adding points
// that already are in the system of generators?
// To do this efficiently we could sort the system and
// perform insertions keeping its sortedness.
void
PPL::GenSys::add_corresponding_points() {
  assert(!is_necessarily_closed());
  // NOTE: we always add (pending) rows at the end of the generator system.
  // Updating `index_first_pending', if needed, is done by the caller.
  GenSys& gs = *this;
  dimension_type n_rows = gs.num_rows();
  dimension_type eps_index = gs.num_columns() - 1;
  for (dimension_type i = 0; i < n_rows; i++) {
    const Generator& g = gs[i];
    if (g[0] > 0 && g[eps_index] == 0) {
      // `g' is a closure point: adding the point.
      // Note: normalization is preserved.
      Generator p = g;
      p[eps_index] = p[0];
      gs.add_pending_row(p);
    }
  }
}

bool
PPL::GenSys::has_closure_points() const {
  // Avoiding the repeated tests on topology.
  if (is_necessarily_closed())
    return false;
  const GenSys& gs = *this;
  dimension_type eps_index = gs.num_columns() - 1;
  for (dimension_type i = num_rows(); i-- > 0; )
    if (gs[i][0] != 0 && gs[i][eps_index] == 0)
      return true;
  return false;
}


bool
PPL::GenSys::has_points() const {
  const GenSys& gs = *this;
  // Avoiding the repeated tests on topology.
  if (is_necessarily_closed())
    for (dimension_type i = num_rows(); i-- > 0; ) {
      if (gs[i][0] != 0)
	return true;
    }
  else {
    // is_necessarily_closed() == false.
    dimension_type eps_index = gs.num_columns() - 1;
    for (dimension_type i = num_rows(); i-- > 0; )
    if (gs[i][eps_index] != 0)
      return true;
  }
  return false;
}


void
PPL::GenSys::const_iterator::skip_forward() {
  PPL::Matrix::const_iterator gsp_end = gsp->end();
  if (i != gsp_end) {
    Matrix::const_iterator i_next = i;
    ++i_next;
    if (i_next != gsp_end) {
      const Generator& cp = static_cast<const Generator&>(*i);
      const Generator& p = static_cast<const Generator&>(*i_next);
      if (cp.is_closure_point()
	  && p.is_point()
	  && cp.is_matching_closure_point(p))
	i = i_next;
    }
  }
}


void
PPL::GenSys::insert(const Generator& g) {
  // We are sure that the matrix has no pending rows
  // and that the new row is not a pending generator.
  assert(num_pending_rows() == 0);
  if (topology() == g.topology())
    Matrix::insert(g);
  else
    // `*this' and `g' have different topologies.
    if (is_necessarily_closed()) {
      // Padding the matrix with the column
      // corresponding to the epsilon coefficients:
      // all points must have epsilon coordinate equal to 1
      // (i.e., the epsilon coefficient is equal to the divisor);
      // rays and lines must have epsilon coefficient equal to 0.
      // Note: normalization is preserved.
      dimension_type eps_index = num_columns();
      add_zero_columns(1);
      GenSys& gs = *this;
      for (dimension_type i = num_rows(); i-- > 0; ) {
	Generator& gen = gs[i];
	if (gen[0] != 0)
	  gen[eps_index] = gen[0];
      }
      set_not_necessarily_closed();
      // Inserting the new generator.
      Matrix::insert(g);
    }
    else {
      // The generator system is NOT necessarily closed:
      // copy the generator, adding the missing dimensions
      // and the epsilon coefficient.
      dimension_type new_size = 2 + std::max(g.space_dimension(),
					     space_dimension());
      Generator tmp_g(g, new_size);
      // If it was a point, set the epsilon coordinate to 1
      // (i.e., set the coefficient equal to the divisor).
      // Note: normalization is preserved.
      if (tmp_g[0] != 0)
	tmp_g[new_size - 1] = tmp_g[0];
      tmp_g.set_not_necessarily_closed();
      // Inserting the new generator.
      Matrix::insert(tmp_g);
    }
}

void
PPL::GenSys::insert_pending(const Generator& g) {
  if (topology() == g.topology())
    Matrix::insert_pending(g);
  else
    // `*this' and `g' have different topologies.
    if (is_necessarily_closed()) {
      // Padding the matrix with the column
      // corresponding to the epsilon coefficients:
      // all points must have epsilon coordinate equal to 1
      // (i.e., the epsilon coefficient is equal to the divisor);
      // rays and lines must have epsilon coefficient equal to 0.
      // Note: normalization is preserved.
      dimension_type eps_index = num_columns();
      add_zero_columns(1);
      GenSys& gs = *this;
      for (dimension_type i = num_rows(); i-- > 0; ) {
	Generator& gen = gs[i];
	if (gen[0] != 0)
	  gen[eps_index] = gen[0];
      }
      set_not_necessarily_closed();
      // Inserting the new generator.
      Matrix::insert_pending(g);
    }
    else {
      // The generator system is NOT necessarily closed:
      // copy the generator, adding the missing dimensions
      // and the epsilon coefficient.
      dimension_type new_size = 2 + std::max(g.space_dimension(),
					     space_dimension());
      Generator tmp_g(g, new_size);
      // If it was a point, set the epsilon coordinate to 1
      // (i.e., set the coefficient equal to the divisor).
      // Note: normalization is preserved.
      if (tmp_g[0] != 0)
	tmp_g[new_size - 1] = tmp_g[0];
      tmp_g.set_not_necessarily_closed();
      // Inserting the new generator.
      Matrix::insert_pending(tmp_g);
    }
}

PPL::dimension_type
PPL::GenSys::num_lines() const {
  // We are sure that this method is applied only to a matrix
  // that does not contain pending rows.
  assert(num_pending_rows() == 0);
  dimension_type n = 0;
  // If the Matrix happens to be sorted, take advantage of the fact
  // that lines are at the top of the system.
  if (is_sorted()) {
    dimension_type nrows = num_rows();
    for (dimension_type i = 0; i < nrows && (*this)[i].is_line(); ++i)
      ++n;
  }
  else
    for (dimension_type i = num_rows(); i-- > 0 ; )
      if ((*this)[i].is_line())
	++n;
  return n;
}

PPL::dimension_type
PPL::GenSys::num_rays() const {
  // We are sure that this method is applied only to a matrix
  // that does not contain pending rows.
  assert(num_pending_rows() == 0);
  dimension_type n = 0;
  // If the Matrix happens to be sorted, take advantage of the fact
  // that rays and points are at the bottom of the system and
  // rays have the inhomogeneous term equal to zero.
  if (is_sorted()) {
    const GenSys& x = *this;
    for (dimension_type i = num_rows(); i != 0 && x[--i].is_ray_or_point(); )
      if (x[i][0] == 0)
	++n;
  }
  else
    for (dimension_type i = num_rows(); i-- > 0 ; ) {
      const Generator& g = (*this)[i];
      if (g.is_ray_or_point() && g[0] == 0)
	++n;
    }
  return n;
}

PPL::Poly_Con_Relation
PPL::GenSys::relation_with(const Constraint& c) const {
  // Note: this method is not public and it is the responsibility
  // of the caller to actually test for dimension compatibility.
  // We simply assert it.
  assert(space_dimension() >= c.space_dimension());
  // Number of generators: the case of an empty polyhedron
  // has already been filtered out by the caller.
  dimension_type n_rows = num_rows();
  assert(n_rows > 0);
  const GenSys& gen_sys = *this;

  // `res' will keep the relation holding between the generators
  // we have seen so far and the constraint `c'.
  Poly_Con_Relation res = Poly_Con_Relation::saturates();

  switch (c.type()) {

  case Constraint::EQUALITY:
    {
      // The hyperplane defined by the equality `c' is included
      // in the set of points satisfying `c' (it is the same set!).
      res = res && Poly_Con_Relation::is_included();
      // The following integer variable will hold the scalar product sign
      // of either the first point or the first non-saturating ray we find.
      // If it is equal to 2, then it means that we haven't found such
      // a generator yet.
      int first_point_or_nonsaturating_ray_sign = 2;

      for (dimension_type i = n_rows; i-- > 0; ) {
	const Generator& g = gen_sys[i];
	int sp_sign = sgn(c * g);
	// Checking whether the generator saturates the equality.
	// If that is the case, then we have to do something only if
	// the generator is a point.
	if (sp_sign == 0) {
	  if (g.is_point())
	    if (first_point_or_nonsaturating_ray_sign == 2)
	      // It is the first time that we find a point and
	      // we have not found a non-saturating ray yet.
	      first_point_or_nonsaturating_ray_sign = 0;
	    else
	      // We already found a point or a non-saturating ray.
	      if (first_point_or_nonsaturating_ray_sign != 0)
		return Poly_Con_Relation::strictly_intersects();
	}
	else
	  // Here we know that sp_sign != 0.
	  switch (g.type()) {

	  case Generator::LINE:
	    // If a line does not saturate `c', then there is a strict
	    // intersection between the points satisfying `c'
	    // and the points generated by `gen_sys'.
	    return Poly_Con_Relation::strictly_intersects();

	  case Generator::RAY:
	    if (first_point_or_nonsaturating_ray_sign == 2) {
	      // It is the first time that we have a non-saturating ray
	      // and we have not found any point yet.
	      first_point_or_nonsaturating_ray_sign = sp_sign;
	      res = Poly_Con_Relation::is_disjoint();
	    }
	    else
	      // We already found a point or a non-saturating ray.
	      if (sp_sign != first_point_or_nonsaturating_ray_sign)
		return Poly_Con_Relation::strictly_intersects();
	    break;

	  case Generator::POINT:
	  case Generator::CLOSURE_POINT:
	    // NOTE: a non-saturating closure point is treated as
	    // a normal point.
	    if (first_point_or_nonsaturating_ray_sign == 2) {
	      // It is the first time that we find a point and
	      // we have not found a non-saturating ray yet.
	      first_point_or_nonsaturating_ray_sign = sp_sign;
	      res = Poly_Con_Relation::is_disjoint();
	    }
	    else
	      // We already found a point or a non-saturating ray.
	      if (sp_sign != first_point_or_nonsaturating_ray_sign)
		return Poly_Con_Relation::strictly_intersects();
	    break;
	  }
      }
    }
    break;

  case Constraint::NONSTRICT_INEQUALITY:
    {
      // The hyperplane implicitly defined by the non-strict inequality `c'
      // is included in the set of points satisfying `c'.
      res = res && Poly_Con_Relation::is_included();
      // The following boolean variable will be set to `false'
      // as soon as either we find (any) point or we find a
      // non-saturating ray.
      bool first_point_or_nonsaturating_ray = true;

      for (dimension_type i = n_rows; i-- > 0; ) {
	const Generator& g = gen_sys[i];
	int sp_sign = sgn(c * g);
	// Checking whether the generator saturates the non-strict
	// inequality. If that is the case, then we have to do something
	// only if the generator is a point.
	if (sp_sign == 0) {
	  if (g.is_point())
	    if (first_point_or_nonsaturating_ray)
	      // It is the first time that we have a point and
	      // we have not found a non-saturating ray yet.
	      first_point_or_nonsaturating_ray = false;
	    else
	      // We already found a point or a non-saturating ray before.
	      if (res == Poly_Con_Relation::is_disjoint())
		// Since g saturates c, we have a strict intersection if
		// none of the generators seen so far are included in `c'.
		return Poly_Con_Relation::strictly_intersects();
	}
	else
	  // Here we know that sp_sign != 0.
	  switch (g.type()) {

	  case Generator::LINE:
	    // If a line does not saturate `c', then there is a strict
	    // intersection between the points satisfying `c' and
	    // the points generated by `gen_sys'.
	    return Poly_Con_Relation::strictly_intersects();

	  case Generator::RAY:
	    if (first_point_or_nonsaturating_ray) {
	      // It is the first time that we have a non-saturating ray
	      // and we have not found any point yet.
	      first_point_or_nonsaturating_ray = false;
	      res = (sp_sign > 0)
		? Poly_Con_Relation::is_included()
		: Poly_Con_Relation::is_disjoint();
	    }
	    else {
	      // We already found a point or a non-saturating ray.
	      if ((sp_sign > 0
		   && res == Poly_Con_Relation::is_disjoint())
		  || (sp_sign < 0
		      && res.implies(Poly_Con_Relation::is_included())))
		// We have a strict intersection if either:
		// - `g' satisfies `c' but none of the generators seen
		//    so far are included in `c'; or
		// - `g' does not satisfy `c' and all the generators
		//    seen so far are included in `c'.
		return Poly_Con_Relation::strictly_intersects();
	      if (sp_sign > 0)
		// Here all the generators seen so far either saturate
		// or are included in `c'.
		// Since `g' does not saturate `c' ...
		res = Poly_Con_Relation::is_included();
	    }
	    break;

	  case Generator::POINT:
	  case Generator::CLOSURE_POINT:
	    // NOTE: a non-saturating closure point is treated as
	    // a normal point.
	    if (first_point_or_nonsaturating_ray) {
	      // It is the first time that we have a point and
	      // we have not found a non-saturating ray yet.
	      // - If point `g' saturates `c', then all the generators
	      //   seen so far saturate `c'.
	      // - If point `g' is included (but does not saturate) `c',
	      //   then all the generators seen so far are included in `c'.
	      // - If point `g' does not satisfy `c', then all the
	      //   generators seen so far are disjoint from `c'.
	      first_point_or_nonsaturating_ray = false;
	      if (sp_sign > 0)
		res = Poly_Con_Relation::is_included();
	      else if (sp_sign < 0)
		res = Poly_Con_Relation::is_disjoint();
	    }
	    else {
	      // We already found a point or a non-saturating ray before.
	      if ((sp_sign > 0
		   && res == Poly_Con_Relation::is_disjoint())
		  || (sp_sign < 0
		      && res.implies(Poly_Con_Relation::is_included())))
		// We have a strict intersection if either:
		// - `g' satisfies or saturates `c' but none of the
		//    generators seen so far are included in `c'; or
		// - `g' does not satisfy `c' and all the generators
		//    seen so far are included in `c'.
		return Poly_Con_Relation::strictly_intersects();
	      if (sp_sign > 0)
		// Here all the generators seen so far either saturate
		// or are included in `c'.
		// Since `g' does not saturate `c' ...
		res = Poly_Con_Relation::is_included();
	    }
	    break;
	  }
      }
    }
    break;

  case Constraint::STRICT_INEQUALITY:
    {
      // The hyperplane implicitly defined by the strict inequality `c'
      // is disjoint from the set of points satisfying `c'.
      res = res && Poly_Con_Relation::is_disjoint();
      // The following boolean variable will be set to `false'
      // as soon as either we find (any) point or we find a
      // non-saturating ray.
      bool first_point_or_nonsaturating_ray = true;
      for (dimension_type i = n_rows; i-- > 0; ) {
	const Generator& g = gen_sys[i];
	// Using the reduced scalar product operator to avoid
	// both topology and num_columns mismatches.
	int sp_sign = sgn(reduced_scalar_product(c, g));
	// Checking whether the generator saturates the strict inequality.
	// If that is the case, then we have to do something
	// only if the generator is a point.
	if (sp_sign == 0) {
	  if (g.is_point())
	    if (first_point_or_nonsaturating_ray)
	      // It is the first time that we have a point and
	      // we have not found a non-saturating ray yet.
	      first_point_or_nonsaturating_ray = false;
	    else
	      // We already found a point or a non-saturating ray before.
	      if (res == Poly_Con_Relation::is_included())
		return Poly_Con_Relation::strictly_intersects();
	}
	else
	  // Here we know that sp_sign != 0.
	  switch (g.type()) {

	  case Generator::LINE:
	    // If a line does not saturate `c', then there is a strict
	    // intersection between the points satisfying `c' and the points
	    // generated by `gen_sys'.
	    return Poly_Con_Relation::strictly_intersects();

	  case Generator::RAY:
	    if (first_point_or_nonsaturating_ray) {
	      // It is the first time that we have a non-saturating ray
	      // and we have not found any point yet.
	      first_point_or_nonsaturating_ray = false;
	      res = (sp_sign > 0)
		? Poly_Con_Relation::is_included()
		: Poly_Con_Relation::is_disjoint();
	    }
	    else {
	      // We already found a point or a non-saturating ray before.
	      if ((sp_sign > 0
		   && res.implies(Poly_Con_Relation::is_disjoint()))
		  ||
		  (sp_sign <= 0
		   && res == Poly_Con_Relation::is_included()))
		return Poly_Con_Relation::strictly_intersects();
	      if (sp_sign < 0)
		// Here all the generators seen so far either saturate
		// or are disjoint from `c'.
		// Since `g' does not saturate `c' ...
		res = Poly_Con_Relation::is_disjoint();
	    }
	    break;

	  case Generator::POINT:
	  case Generator::CLOSURE_POINT:
	    if (first_point_or_nonsaturating_ray) {
	      // It is the first time that we have a point and
	      // we have not found a non-saturating ray yet.
	      // - If point `g' saturates `c', then all the generators
	      //   seen so far saturate `c'.
	      // - If point `g' is included in (but does not saturate) `c',
	      //   then all the generators seen so far are included in `c'.
	      // - If point `g' strictly violates `c', then all the
	      //   generators seen so far are disjoint from `c'.
	      first_point_or_nonsaturating_ray = false;
	      if (sp_sign > 0)
		res = Poly_Con_Relation::is_included();
	      else if (sp_sign < 0)
		res = Poly_Con_Relation::is_disjoint();
	    }
	    else {
	      // We already found a point or a non-saturating ray before.
	      if ((sp_sign > 0
		   && res.implies(Poly_Con_Relation::is_disjoint()))
		  ||
		  (sp_sign <= 0
		   && res == Poly_Con_Relation::is_included()))
		return Poly_Con_Relation::strictly_intersects();
	      if (sp_sign < 0)
		// Here all the generators seen so far either saturate
		// or are disjoint from `c'.
		// Since `g' does not saturate `c' ...
		res = Poly_Con_Relation::is_disjoint();
	    }
	    break;
	  }
      }
    }
    break;
  }
  // We have seen all generators.
  return res;
}


bool
PPL::GenSys::satisfied_by_all_generators(const Constraint& c) const {
  assert(c.space_dimension() <= space_dimension());

  // Setting `sp_fp' to the appropriate scalar product operator.
  // This also avoids problems when having _legal_ topology mismatches
  // (which could also cause a mismatch in the number of columns).
  const Integer& (*sp_fp)(const Row&, const Row&);
  if (c.is_necessarily_closed())
    sp_fp = PPL::operator*;
  else
    sp_fp = PPL::reduced_scalar_product;

  const GenSys& gs = *this;
  switch (c.type()) {
  case Constraint::EQUALITY:
    // Equalities must be saturated by all generators.
    for (dimension_type i = gs.num_rows(); i-- > 0; )
      if (sp_fp(c, gs[i]) != 0)
	return false;
    break;
  case Constraint::NONSTRICT_INEQUALITY:
    // Non-strict inequalities must be satisfied by all generators.
    for (dimension_type i = gs.num_rows(); i-- > 0; )
      if (sp_fp(c, gs[i]) < 0)
	return false;
    break;
  case Constraint::STRICT_INEQUALITY:
    // Strict inequalities must be satisfied by all generators
    // and must not be saturated by points.
    for (dimension_type i = gs.num_rows(); i-- > 0; ) {
      const Generator& g = gs[i];
      if (g.is_point()) {
	if (sp_fp(c, g) <= 0)
	  return false;
      }
      else
	// `g' is a line, ray or closure point.
	if (sp_fp(c, g) < 0)
	  return false;
    }
    break;
  }
  // If we reach this point, `c' is satisfied by all generators.
  return true;
}


void
PPL::GenSys::affine_image(dimension_type v,
			  const LinExpression& expr,
			  const Integer& denominator) {
  // `v' is the index of a column corresponding to
  // a "user" variable (i.e., it cannot be the inhomogeneous term,
  // nor the epsilon dimension of NNC polyhedra).
  assert(v > 0 && v <= space_dimension());
  assert(expr.space_dimension() <= space_dimension());
  assert(denominator > 0);

  dimension_type n_columns = num_columns();
  dimension_type n_rows = num_rows();
  GenSys& x = *this;

  // Compute the numerator of the affine transformation and assign it
  // to the column of `*this' indexed by `v'.
  for (dimension_type i = n_rows; i-- > 0; ) {
    Generator& row = x[i];
    tmp_Integer[1] = 0;
    for (dimension_type j = expr.size(); j-- > 0; )
      tmp_Integer[1] += row[j] * expr[j];
    std::swap(tmp_Integer[1], row[v]); 
  }

  if (denominator != 1) {
    // Since we want integer elements in the matrix,
    // we multiply by the value of `denominator'
    // all the columns of `*this' having an index different from `v'.
    for (dimension_type i = n_rows; i-- > 0; )
      for (dimension_type j = n_columns; j-- > 0; )
	if (j != v)
	  x[i][j] *= denominator;
  }

  // If the mapping is not invertible we may have transformed
  // valid lines and rays into the origin of the space.
  bool not_invertible = (v > expr.space_dimension() || expr[v] == 0);
  if (not_invertible)
    x.remove_invalid_lines_and_rays();
  // This also resets the sortedness flag.
#if EXTRA_NORMALIZATION
  x.strong_normalize();
#else
  x.normalize();
#endif
}

void
PPL::GenSys::ascii_dump(std::ostream& s) const {
  Matrix::ascii_dump(s);
  const char separator = ' ';
  const GenSys& x = *this;
  for (dimension_type i = 0; i < x.num_rows(); ++i) {
    for (dimension_type j = 0; j < x.num_columns(); ++j)
      s << x[i][j] << separator;
    s << separator << separator;
    switch (static_cast<Generator>(x[i]).type()) {
    case Generator::LINE:
      s << "L";
      break;
    case Generator::RAY:
      s << "R";
      break;
    case Generator::POINT:
      s << "P";
      break;
    case Generator::CLOSURE_POINT:
      s << "C";
      break;
    }
    s << std::endl;
  }
}

bool
PPL::GenSys::ascii_load(std::istream& s) {
  if (!Matrix::ascii_load(s))
    return false;

  GenSys& x = *this;
  for (dimension_type i = 0; i < x.num_rows(); ++i) {
    for (dimension_type j = 0; j < x.num_columns(); ++j)
      if (!(s >> x[i][j]))
	return false;

    std::string str;
    if (!(s >> str))
      return false;
    if (str == "L")
      x[i].set_is_line();
    else
      x[i].set_is_ray_or_point();

    // Checking for equality of actual and declared types.
    switch (static_cast<Generator>(x[i]).type()) {
    case Generator::LINE:
      if (str == "L")
	continue;
      break;
    case Generator::RAY:
      if (str == "R")
	continue;
      break;
    case Generator::POINT:
      if (str == "P")
	continue;
      break;
    case Generator::CLOSURE_POINT:
      if (str == "C")
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

void
PPL::GenSys::remove_invalid_lines_and_rays() {
  // The origin of the vector space cannot be a valid line/ray.
  // NOTE: the following swaps will mix generators without even trying
  // to preserve sortedness: as a matter of fact, it will almost always
  // be the case that the input generator system is NOT sorted.
  GenSys& gs = *this;
  dimension_type n_rows = gs.num_rows();
  if (num_pending_rows() == 0) {
    for (dimension_type i = n_rows; i-- > 0; ) {
      Generator& g = gs[i];
      if (g[0] == 0 && g.all_homogeneous_terms_are_zero()) {
	// An invalid line/ray has been found.
	--n_rows;
	std::swap(g, gs[n_rows]);
	gs.set_sorted(false);
      }
    }
    set_index_first_pending_row(n_rows);
  }
  else {
    // If the matrix has some pending rows, we can not
    // swap the "normal" rows with the pending rows. So
    // we must put at the end of the "normal" rows
    // the invalid "normal" rows, put them at the end
    // of the matrix, find the invalid rows in the pending
    // part and then erase the invalid rows that now
    // are in the bottom part of the matrix.
    assert(num_pending_rows() > 0);
    dimension_type first_pending = first_pending_row();
    for (dimension_type i = first_pending; i-- > 0; ) {
      Generator& g = gs[i];
      if (g[0] == 0 && g.all_homogeneous_terms_are_zero()) {
	// An invalid line/ray has been found.
	--first_pending;
	std::swap(g, gs[first_pending]);
	gs.set_sorted(false);
      }
    }
    dimension_type num_invalid_rows = first_pending_row() - first_pending;
    set_index_first_pending_row(first_pending);
    for (dimension_type i = 0; i < num_invalid_rows; ++i)
      std::swap(gs[n_rows - i], gs[first_pending + i]);
    n_rows -= num_invalid_rows;
    for (dimension_type i = n_rows; i-- > first_pending; ) {
      Generator& g = gs[i];
      if (g[0] == 0 && g.all_homogeneous_terms_are_zero()) {
	// An invalid line/ray has been found.
	--n_rows;
	std::swap(g, gs[n_rows]);
	gs.set_sorted(false);
      }
    }
  }
  gs.erase_to_end(n_rows);
}
  
bool
PPL::GenSys::OK() const {
  // A GenSys must be a valid Matrix.
  if (!Matrix::OK())
    return false;

  // Checking each generator in the system.
  for (dimension_type i = num_rows(); i-- > 0; ) {
    const Generator& g = (*this)[i];
    if (!g.OK())
      return false;
  }

  // All checks passed.
  return true;
}

/*! \relates Parma_Polyhedra_Library::GenSys */
std::ostream&
PPL::IO_Operators::operator<<(std::ostream& s, const GenSys& gs) {
  GenSys::const_iterator i = gs.begin();
  GenSys::const_iterator gs_end = gs.end();
  if (i == gs_end)
    s << "false";
  else {
    while (i != gs_end) {
      s << *i++;
      if (i != gs_end)
	s << ", ";
    }
  }
  return s;
}
