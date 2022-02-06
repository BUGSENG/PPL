/* Affine_Space class implementation.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2022 BUGSENG srl (http://bugseng.com)

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
#include "Affine_Space_defs.hh"
#include <iostream>

namespace PPL = Parma_Polyhedra_Library;

PPL::Affine_Space::Affine_Space(const Generator_System& gs) {
  dimension_type space_dim = gs.space_dimension();
  // First find a point or closure point and convert it to a
  // grid point and add to the (initially empty) set of grid generators.
  Grid_Generator_System ggs(space_dim);
  Linear_Expression point_expr;
  PPL_DIRTY_TEMP_COEFFICIENT(point_divisor);
  for (Generator_System::const_iterator g = gs.begin(),
         gs_end = gs.end(); g != gs_end; ++g) {
    if (g->is_ray()) {
      throw std::invalid_argument("Affine_Space::Affine_Space(gs):\n"
                                  "gs contains rays.");
    }
    else if (g->is_point() || g->is_closure_point()) {
      point_expr = Linear_Expression(*g);
      ggs.insert(grid_point(point_expr, g->divisor()));
      goto non_empty;
    }
  }
  // No (closure) point was found.
  Grid(EMPTY).m_swap(gr);
  return;

 non_empty:
  // Add a grid line for each line.  If the generator is a (closure)
  // point, the grid line must have the direction given by a line
  // that joins the grid point already inserted and the new point.
  PPL_DIRTY_TEMP_COEFFICIENT(coeff);
  PPL_DIRTY_TEMP_COEFFICIENT(g_divisor);
  for (Generator_System::const_iterator g = gs.begin(),
         gs_end = gs.end(); g != gs_end; ++g) {
    if (g->is_point() || g->is_closure_point()) {
      Linear_Expression e = point_expr;
      e.linear_combine(g->expression(), g->divisor(), -point_divisor,
                       1, space_dim + 1);
      if (!e.all_homogeneous_terms_are_zero()) {
        ggs.insert(grid_line(e));
      }
    }
    else {
      ggs.insert(grid_line(Linear_Expression(*g)));
    }
  }
  Grid(ggs).m_swap(gr);
  PPL_ASSERT(OK());
}


PPL::Affine_Space&
PPL::Affine_Space::operator=(const Affine_Space& y) {
  gr = y.gr;
  return *this;
}

PPL::dimension_type
PPL::Affine_Space::affine_dimension() const {
  return gr.affine_dimension();
}

const PPL::Congruence_System&
PPL::Affine_Space::congruences() const {
  return gr.congruences();
}

const PPL::Congruence_System&
PPL::Affine_Space::minimized_congruences() const {
  return gr.minimized_congruences();
}

PPL::Generator_System
PPL::Affine_Space::generators() const {
  // FIXME: implement by filtering the grid generators.
  abort();
}

PPL::Generator_System
PPL::Affine_Space::minimized_generators() const {
  // FIXME: implement by filtering the grid generators.
  abort();
}

PPL::Poly_Con_Relation
PPL::Affine_Space::relation_with(const Congruence& cg) const {
  return gr.relation_with(cg);
}

PPL::Poly_Gen_Relation
PPL::Affine_Space::relation_with(const Generator& g) const {
  return gr.relation_with(g);
}

PPL::Poly_Con_Relation
PPL::Affine_Space::relation_with(const Constraint& c) const {
  return gr.relation_with(c);
}

bool
PPL::Affine_Space::is_empty() const {
  return gr.is_empty();
}

bool
PPL::Affine_Space::is_universe() const {
  return gr.is_universe();
}

bool
PPL::Affine_Space::is_bounded() const {
  return gr.is_bounded();
}

bool
PPL::Affine_Space::is_discrete() const {
  return gr.is_discrete();
}

bool
PPL::Affine_Space::is_topologically_closed() const {
  return true;
}

bool
PPL::Affine_Space::contains_integer_point() const {
  return gr.contains_integer_point();
}

bool
PPL::Affine_Space::constrains(const Variable var) const {
  return gr.constrains(var);
}

bool
PPL::Affine_Space::OK(bool check_not_empty) const {
  return gr.OK(check_not_empty);
}

void
PPL::Affine_Space::add_constraints(const Constraint_System& cs) {
  gr.add_constraints(cs);
}

void
PPL::Affine_Space::add_generator(const Generator& /*g*/) {
  // FIXME: do we want this method?
  abort();
}

void
PPL::Affine_Space::add_recycled_congruences(Congruence_System& /*cgs*/) {
  // FIXME: do we want this method?
  abort();
}

void
PPL::Affine_Space::add_recycled_generators(Generator_System& /*gs*/) {
  // FIXME: do we want this method?
  abort();
}

void
PPL::Affine_Space::add_generators(const Generator_System& /*gs*/) {
  // FIXME: do we want this method?
  abort();
}

void
PPL::Affine_Space::refine_with_constraint(const Constraint& c) {
  gr.refine_with_constraint(c);
}

void
PPL::Affine_Space::refine_with_constraints(const Constraint_System& cs) {
  gr.refine_with_constraints(cs);
}

void
PPL::Affine_Space::unconstrain(const Variable var) {
  gr.unconstrain(var);
}

void
PPL::Affine_Space::unconstrain(const Variables_Set& vars) {
  gr.unconstrain(vars);
}

void
PPL::Affine_Space::intersection_assign(const Affine_Space& y) {
  gr.intersection_assign(y.gr);
}

void
PPL::Affine_Space::upper_bound_assign(const Affine_Space& y) {
  // FIXME: horrible kludge to filter congruences away.
  gr.upper_bound_assign(y.gr);
  Affine_Space a(gr.constraints());
  m_swap(a);
}

bool
PPL::Affine_Space::upper_bound_assign_if_exact(const Affine_Space& y) {
  return gr.upper_bound_assign_if_exact(y.gr);
}

void
PPL::Affine_Space::difference_assign(const Affine_Space& y) {
  gr.difference_assign(y.gr);
}

bool
PPL::Affine_Space::simplify_using_context_assign(const Affine_Space& y) {
  return gr.simplify_using_context_assign(y.gr);
}

void
PPL::Affine_Space::affine_image(const Variable var,
                                const Linear_Expression& expr,
                                Coefficient_traits::const_reference
                                denominator) {
  gr.affine_image(var, expr, denominator);
}

void
PPL::Affine_Space::
affine_preimage(const Variable var,
                const Linear_Expression& expr,
                Coefficient_traits::const_reference denominator) {
  gr.affine_preimage(var, expr, denominator);
}

void
PPL::Affine_Space::
generalized_affine_image(const Variable var,
                         const Relation_Symbol relsym,
                         const Linear_Expression& expr,
                         Coefficient_traits::const_reference denominator) {
  gr.generalized_affine_image(var, relsym, expr, denominator);
}

void
PPL::Affine_Space::
generalized_affine_preimage(const Variable var,
                            const Relation_Symbol relsym,
                            const Linear_Expression& expr,
                            Coefficient_traits::const_reference denominator) {
  gr.generalized_affine_preimage(var, relsym, expr, denominator);
}

void
PPL::Affine_Space::
generalized_affine_image(const Linear_Expression& lhs,
                         const Relation_Symbol relsym,
                         const Linear_Expression& rhs) {
  gr.generalized_affine_image(lhs, relsym, rhs);
}

void
PPL::Affine_Space::
generalized_affine_preimage(const Linear_Expression& lhs,
                            const Relation_Symbol relsym,
                            const Linear_Expression& rhs) {
  gr.generalized_affine_preimage(lhs, relsym, rhs);
}

void
PPL::Affine_Space::
bounded_affine_image(const Variable var,
                     const Linear_Expression& lb_expr,
                     const Linear_Expression& ub_expr,
                     Coefficient_traits::const_reference denominator) {
  gr.bounded_affine_image(var, lb_expr, ub_expr, denominator);
}


void
PPL::Affine_Space::
bounded_affine_preimage(const Variable var,
                        const Linear_Expression& lb_expr,
                        const Linear_Expression& ub_expr,
                        Coefficient_traits::const_reference denominator) {
  gr.bounded_affine_preimage(var, lb_expr, ub_expr, denominator);
}

void
PPL::Affine_Space::time_elapse_assign(const Affine_Space& y) {
  gr.time_elapse_assign(y.gr);
}

/*! \relates Parma_Polyhedra_Library::Affine_Space */
bool
PPL::operator==(const Affine_Space& x, const Affine_Space& y) {
  return x.gr == y.gr;
}

bool
PPL::Affine_Space::contains(const Affine_Space& y) const {
  return gr.contains(y.gr);
}

bool
PPL::Affine_Space::is_disjoint_from(const Affine_Space& y) const {
  return gr.is_disjoint_from(y.gr);
}

void
PPL::Affine_Space::ascii_dump(std::ostream& s) const {
  gr.ascii_dump(s);
}

PPL_OUTPUT_DEFINITIONS(Affine_Space)

bool
PPL::Affine_Space::ascii_load(std::istream& s) {
  return gr.ascii_load(s);
}

PPL::memory_size_type
PPL::Affine_Space::external_memory_in_bytes() const {
  return gr.external_memory_in_bytes();
}

void
PPL::Affine_Space::add_space_dimensions_and_embed(dimension_type m) {
  gr.add_space_dimensions_and_embed(m);
}

void
PPL::Affine_Space::add_space_dimensions_and_project(dimension_type m) {
  gr.add_space_dimensions_and_project(m);
}

void
PPL::Affine_Space::concatenate_assign(const Affine_Space& y) {
  gr.concatenate_assign(y.gr);
}

void
PPL::Affine_Space::remove_space_dimensions(const Variables_Set& vars) {
  gr.remove_space_dimensions(vars);
}

void
PPL::Affine_Space::remove_higher_space_dimensions(const dimension_type
                                                  new_dimension) {
  gr.remove_higher_space_dimensions(new_dimension);
}

void
PPL::Affine_Space::expand_space_dimension(Variable var, dimension_type m) {
  gr.expand_space_dimension(var, m);
}

void
PPL::Affine_Space::fold_space_dimensions(const Variables_Set& to_be_folded,
                                         Variable var) {
  gr.fold_space_dimensions(to_be_folded, var);
}

void
PPL::Affine_Space::widening_assign(const Affine_Space& y, unsigned*) {
  Affine_Space& x = *this;

  // Dimension-compatibility check.
  if (x.space_dimension() != y.space_dimension())
    throw_dimension_incompatible("widening_assign(y)", "y", y);

  // Assume `y' is contained in or equal to `x'.
  PPL_EXPECT_HEAVY(copy_contains(x, y));
}

void
PPL::Affine_Space::limited_extrapolation_assign(const Affine_Space& y,
                                                const Constraint_System&,
                                                unsigned*) {
  Affine_Space& x = *this;

  // Dimension-compatibility check.
  if (x.space_dimension() != y.space_dimension()) {
    throw_dimension_incompatible("widening_assign(y)", "y", y);
  }

  // Assume `y' is contained in or equal to `x'.
  PPL_EXPECT_HEAVY(copy_contains(x, y));
}

/*! \relates Parma_Polyhedra_Library::Affine_Space */
std::ostream&
PPL::IO_Operators::operator<<(std::ostream& s, const Affine_Space& gr) {
  s << gr;
  return s;
}

void
PPL::Affine_Space
::throw_dimension_incompatible(const char* method,
                               const char* other_name,
                               dimension_type other_dim) const {
  std::ostringstream s;
  s << "PPL::Affine_Space::" << method << ":\n"
    << "this->space_dimension() == " << space_dimension() << ", "
    << other_name << ".space_dimension() == " << other_dim << ".";
  throw std::invalid_argument(s.str());
}

void
PPL::Affine_Space
::throw_dimension_incompatible(const char* method,
                               const char* as_name,
                               const Affine_Space& as) const {
  throw_dimension_incompatible(method, as_name, as.space_dimension());
}
