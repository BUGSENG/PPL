/* Partially_Reduced_Product class implementation: inline functions.
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

#ifndef PPL_Partially_Reduced_Product_inlines_hh
#define PPL_Partially_Reduced_Product_inlines_hh 1

#include "Constraint_System.defs.hh"
#include "Congruence_System.defs.hh"
#include "C_Polyhedron.defs.hh"
#include "NNC_Polyhedron.defs.hh"
#include "Grid.defs.hh"

namespace Parma_Polyhedra_Library {

template <typename D1, typename D2, typename R>
inline dimension_type
Partially_Reduced_Product<D1, D2, R>::max_space_dimension() {
  return std::min(D1::max_space_dimension(), D2::max_space_dimension());
}

template <typename D1, typename D2, typename R>
inline
Partially_Reduced_Product<D1, D2, R>
::Partially_Reduced_Product(dimension_type num_dimensions,
			    const Degenerate_Element kind)
  : d1(num_dimensions <= max_space_dimension()
       ?  num_dimensions
       : (throw_space_dimension_overflow
               ("Partially_Reduced_Product<D1, D2, R>(n, k)",
                "n exceeds the maximum "
                "allowed space dimension"),
	       0), kind),
    d2(num_dimensions, kind) {
  set_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline
Partially_Reduced_Product<D1, D2, R>
::Partially_Reduced_Product(const Congruence_System& ccgs)
  : d1((ccgs.space_dimension() <= max_space_dimension())
       ?  ccgs
       : (throw_space_dimension_overflow
          ("Partially_Reduced_Product<D1, D2, R>(n, k)",
           "the space dimension of ccgs "
           "exceeds the maximum allowed "
           "space dimension"), ccgs)),
    d2(ccgs) {
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline
Partially_Reduced_Product<D1, D2, R>
::Partially_Reduced_Product(Congruence_System& cgs)
  : d1((cgs.space_dimension() <= max_space_dimension())
       ? const_cast<const Congruence_System&>(cgs)
       : (throw_space_dimension_overflow
          ("Partially_Reduced_Product<D1, D2, R>(n, k)",
           "the space dimension of cgs "
           "exceeds the maximum allowed "
           "space dimension"), cgs)),
    d2(cgs) {
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline
Partially_Reduced_Product<D1, D2, R>
::Partially_Reduced_Product(const Constraint_System& ccs)
  : d1((ccs.space_dimension() <= max_space_dimension())
       ?  ccs
       : (throw_space_dimension_overflow
          ("Partially_Reduced_Product<D1, D2, R>(n, k)",
           "the space dimension of ccs "
           "exceeds the maximum allowed "
           "space dimension"), ccs)),
    d2(ccs) {
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline
Partially_Reduced_Product<D1, D2, R>
::Partially_Reduced_Product(Constraint_System& cs)
  : d1((cs.space_dimension() <= max_space_dimension())
       ? const_cast<const Constraint_System&>(cs)
       : (throw_space_dimension_overflow
          ("Partially_Reduced_Product<D1, D2, R>(n, k)",
           "the space dimension of cs "
           "exceeds the maximum allowed "
           "space dimension"), cs)),
    d2(cs) {
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline
Partially_Reduced_Product<D1, D2, R>
::Partially_Reduced_Product(const C_Polyhedron& ph,
                            Complexity_Class complexity)
  : d1((ph.space_dimension() <= max_space_dimension())
       ? ph
       : (throw_space_dimension_overflow
          ("Partially_Reduced_Product<D1, D2, R>(n, k)",
           "the space dimension of ph "
           "exceeds the maximum allowed "
           "space dimension"), ph), complexity),
   d2(ph, complexity) {
  set_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline
Partially_Reduced_Product<D1, D2, R>
::Partially_Reduced_Product(const NNC_Polyhedron& ph,
                            Complexity_Class complexity)
 : d1((ph.space_dimension() <= max_space_dimension())
       ? ph
       : (throw_space_dimension_overflow
          ("Partially_Reduced_Product<D1, D2, R>(n, k)",
           "the space dimension of ph "
           "exceeds the maximum allowed "
           "space dimension"), ph), complexity),
   d2(ph, complexity) {
  set_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline
Partially_Reduced_Product<D1, D2, R>
::Partially_Reduced_Product(const Grid& gr, Complexity_Class)
 : d1((gr.space_dimension() <= max_space_dimension())
       ? gr
       : (throw_space_dimension_overflow
          ("Partially_Reduced_Product<D1, D2, R>(n, k)",
           "the space dimension of gr "
           "exceeds the maximum allowed "
           "space dimension"), gr)),
   d2(gr) {
  set_reduced_flag();
}

template <typename D1, typename D2, typename R>
template <typename Interval>
inline
Partially_Reduced_Product<D1, D2, R>
::Partially_Reduced_Product(const Box<Interval>& box, Complexity_Class)
 : d1((box.space_dimension() <= max_space_dimension())
       ? box
       : (throw_space_dimension_overflow
          ("Partially_Reduced_Product<D1, D2, R>(n, k)",
           "the space dimension of box "
           "exceeds the maximum allowed "
           "space dimension"), box)),
   d2(box) {
  set_reduced_flag();
}

template <typename D1, typename D2, typename R>
template <typename U>
inline
Partially_Reduced_Product<D1, D2, R>
::Partially_Reduced_Product(const BD_Shape<U>& bd, Complexity_Class)
 : d1((bd.space_dimension() <= max_space_dimension())
       ? bd
       : (throw_space_dimension_overflow
          ("Partially_Reduced_Product<D1, D2, R>(n, k)",
           "the space dimension of bd "
           "exceeds the maximum allowed "
           "space dimension"), bd)),
   d2(bd) {
  set_reduced_flag();
}

template <typename D1, typename D2, typename R>
template <typename U>
inline
Partially_Reduced_Product<D1, D2, R>
::Partially_Reduced_Product(const Octagonal_Shape<U>& os, Complexity_Class)
 : d1((os.space_dimension() <= max_space_dimension())
       ? os
       : (throw_space_dimension_overflow
          ("Partially_Reduced_Product<D1, D2, R>(n, k)",
           "the space dimension of os "
           "exceeds the maximum allowed "
           "space dimension"), os)),
   d2(os) {
  set_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline
Partially_Reduced_Product<D1, D2, R>
::Partially_Reduced_Product(const Partially_Reduced_Product& y,
                            Complexity_Class)
  : d1(y.d1), d2(y.d2) {
  reduced = y.reduced;
}

template <typename D1, typename D2, typename R>
inline
Partially_Reduced_Product<D1, D2, R>::~Partially_Reduced_Product() {
}

template <typename D1, typename D2, typename R>
inline memory_size_type
Partially_Reduced_Product<D1, D2, R>::external_memory_in_bytes() const {
  return d1.external_memory_in_bytes() + d2.external_memory_in_bytes();
}

template <typename D1, typename D2, typename R>
inline memory_size_type
Partially_Reduced_Product<D1, D2, R>::total_memory_in_bytes() const {
  return sizeof(*this) + external_memory_in_bytes();
}

template <typename D1, typename D2, typename R>
inline dimension_type
Partially_Reduced_Product<D1, D2, R>::space_dimension() const {
  assert(d1.space_dimension() == d2.space_dimension());
  return d1.space_dimension();
}

template <typename D1, typename D2, typename R>
inline dimension_type
Partially_Reduced_Product<D1, D2, R>::affine_dimension() const {
  reduce();
  const dimension_type d1_dim = d1.affine_dimension();
  const dimension_type d2_dim = d2.affine_dimension();
  return std::min(d1_dim, d2_dim);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::unconstrain(const Variable var) {
  reduce();
  d1.unconstrain(var);
  d2.unconstrain(var);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::unconstrain(const Variables_Set& to_be_unconstrained) {
  reduce();
  d1.unconstrain(to_be_unconstrained);
  d2.unconstrain(to_be_unconstrained);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::intersection_assign(const Partially_Reduced_Product& y) {
  d1.intersection_assign(y.d1);
  d2.intersection_assign(y.d2);
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::difference_assign(const Partially_Reduced_Product& y) {
  reduce();
  y.reduce();
  d1.difference_assign(y.d1);
  d2.difference_assign(y.d2);
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::upper_bound_assign(const Partially_Reduced_Product& y) {
  reduce();
  y.reduce();
  d1.upper_bound_assign(y.d1);
  d2.upper_bound_assign(y.d2);
  // CHECKME: if upper_bound_assign is not a least_upper_bound
  //          it may not be reduced.
  // clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>
::upper_bound_assign_if_exact(const Partially_Reduced_Product& y) {
  reduce();
  y.reduce();
  D1 d1_copy = d1;
  bool ub_exact = d1_copy.upper_bound_assign_if_exact(y.d1);
  if (!ub_exact)
    return false;
  ub_exact = d2.upper_bound_assign_if_exact(y.d2);
  if (!ub_exact)
    return false;
  std::swap(d1,d1_copy);
  return true;
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::affine_image(Variable var,
	       const Linear_Expression& expr,
	       Coefficient_traits::const_reference denominator) {
  d1.affine_image(var, expr, denominator);
  d2.affine_image(var, expr, denominator);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::affine_preimage(Variable var,
		  const Linear_Expression& expr,
		  Coefficient_traits::const_reference denominator) {
  d1.affine_preimage(var, expr, denominator);
  d2.affine_preimage(var, expr, denominator);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::generalized_affine_image(Variable var,
			   const Relation_Symbol relsym,
			   const Linear_Expression& expr,
			   Coefficient_traits::const_reference denominator) {
  d1.generalized_affine_image(var, relsym, expr, denominator);
  d2.generalized_affine_image(var, relsym, expr, denominator);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::generalized_affine_preimage(Variable var,
			      const Relation_Symbol relsym,
			      const Linear_Expression& expr,
			      Coefficient_traits::const_reference denominator) {
  d1.generalized_affine_preimage(var, relsym, expr, denominator);
  d2.generalized_affine_preimage(var, relsym, expr, denominator);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::generalized_affine_image(const Linear_Expression& lhs,
			   const Relation_Symbol relsym,
			   const Linear_Expression& rhs) {
  d1.generalized_affine_image(lhs, relsym, rhs);
  d2.generalized_affine_image(lhs, relsym, rhs);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::generalized_affine_preimage(const Linear_Expression& lhs,
                              const Relation_Symbol relsym,
                              const Linear_Expression& rhs) {
  d1.generalized_affine_preimage(lhs, relsym, rhs);
  d2.generalized_affine_preimage(lhs, relsym, rhs);
}


template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::bounded_affine_image(Variable var,
                       const Linear_Expression& lb_expr,
                       const Linear_Expression& ub_expr,
                       Coefficient_traits::const_reference denominator) {
  d1.bounded_affine_image(var, lb_expr, ub_expr, denominator);
  d2.bounded_affine_image(var, lb_expr, ub_expr, denominator);
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::bounded_affine_preimage(Variable var,
                          const Linear_Expression& lb_expr,
                          const Linear_Expression& ub_expr,
                          Coefficient_traits::const_reference denominator) {
  d1.bounded_affine_preimage(var, lb_expr, ub_expr, denominator);
  d2.bounded_affine_preimage(var, lb_expr, ub_expr, denominator);
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::time_elapse_assign(const Partially_Reduced_Product& y) {
  reduce();
  y.reduce();
  d1.time_elapse_assign(y.d1);
  d2.time_elapse_assign(y.d2);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>::topological_closure_assign() {
  d1.topological_closure_assign();
  d2.topological_closure_assign();
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>::swap(Partially_Reduced_Product& y) {
  std::swap(d1, y.d1);
  std::swap(d2, y.d2);
  std::swap(reduced, y.reduced);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>::add_constraint(const Constraint& c) {
  d1.add_constraint(c);
  d2.add_constraint(c);
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>::refine_with_constraint(const Constraint& c) {
  d1.refine_with_constraint(c);
  d2.refine_with_constraint(c);
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>::add_congruence(const Congruence& cg) {
  d1.add_congruence(cg);
  d2.add_congruence(cg);
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>::refine_with_congruence(const Congruence& cg) {
  d1.refine_with_congruence(cg);
  d2.refine_with_congruence(cg);
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::add_constraints(const Constraint_System& cs) {
  d1.add_constraints(cs);
  d2.add_constraints(cs);
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::refine_with_constraints(const Constraint_System& cs) {
  d1.refine_with_constraints(cs);
  d2.refine_with_constraints(cs);
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::add_congruences(const Congruence_System& cgs) {
  d1.add_congruences(cgs);
  d2.add_congruences(cgs);
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::refine_with_congruences(const Congruence_System& cgs) {
  d1.refine_with_congruences(cgs);
  d2.refine_with_congruences(cgs);
  clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline Partially_Reduced_Product<D1, D2, R>&
Partially_Reduced_Product<D1, D2, R>
::operator=(const Partially_Reduced_Product& y) {
  d1 = y.d1;
  d2 = y.d2;
  reduced = y.reduced;
  return *this;
}

template <typename D1, typename D2, typename R>
inline const D1&
Partially_Reduced_Product<D1, D2, R>::domain1() const {
  reduce();
  return d1;
}

template <typename D1, typename D2, typename R>
inline const D2&
Partially_Reduced_Product<D1, D2, R>::domain2() const {
  reduce();
  return d2;
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>::is_empty() const {
  reduce();
  return d1.is_empty() || d2.is_empty();
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>::is_universe() const {
  return d1.is_universe() && d2.is_universe();
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>::is_topologically_closed() const {
  reduce();
  return d1.is_topologically_closed() && d2.is_topologically_closed();
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>
::is_disjoint_from(const Partially_Reduced_Product& y) const {
  reduce();
  y.reduce();
  return d1.is_disjoint_from(y.d1) || d2.is_disjoint_from(y.d2);
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>::is_discrete() const {
  reduce();
  return d1.is_discrete() || d2.is_discrete();
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>::is_bounded() const {
  reduce();
  return d1.is_bounded() || d2.is_bounded();
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>
::bounds_from_above(const Linear_Expression& expr) const {
  reduce();
  return d1.bounds_from_above(expr) || d2.bounds_from_above(expr);
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>
::bounds_from_below(const Linear_Expression& expr) const {
  reduce();
  return d1.bounds_from_below(expr) || d2.bounds_from_below(expr);
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>::constrains(Variable var) const {
  reduce();
  return d1.constrains(var) || d2.constrains(var);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::widening_assign(const Partially_Reduced_Product& y,
                  unsigned* tp) {
  reduce();
  y.reduce();
  d1.widening_assign(y.d1, tp);
  d2.widening_assign(y.d2, tp);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::add_space_dimensions_and_embed(dimension_type m) {
  d1.add_space_dimensions_and_embed(m);
  d2.add_space_dimensions_and_embed(m);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::add_space_dimensions_and_project(dimension_type m) {
  d1.add_space_dimensions_and_project(m);
  d2.add_space_dimensions_and_project(m);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::concatenate_assign(const Partially_Reduced_Product& y) {
  d1.concatenate_assign(y.d1);
  d2.concatenate_assign(y.d2);
  if (!is_reduced() || !y.is_reduced())
    clear_reduced_flag();
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::remove_space_dimensions(const Variables_Set& to_be_removed) {
  d1.remove_space_dimensions(to_be_removed);
  d2.remove_space_dimensions(to_be_removed);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::remove_higher_space_dimensions(dimension_type new_dimension) {
  d1.remove_higher_space_dimensions(new_dimension);
  d2.remove_higher_space_dimensions(new_dimension);
}

template <typename D1, typename D2, typename R>
template <typename Partial_Function>
inline void
Partially_Reduced_Product<D1, D2, R>
::map_space_dimensions(const Partial_Function& pfunc) {
  d1.map_space_dimensions(pfunc);
  d2.map_space_dimensions(pfunc);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::expand_space_dimension(Variable var, dimension_type m) {
  d1.expand_space_dimension(var, m);
  d2.expand_space_dimension(var, m);
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>
::fold_space_dimensions(const Variables_Set& to_be_folded,
			Variable var) {
  d1.fold_space_dimensions(to_be_folded, var);
  d2.fold_space_dimensions(to_be_folded, var);
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>
::contains(const Partially_Reduced_Product& y) const {
  reduce();
  y.reduce();
  return d1.contains(y.d1) && d2.contains(y.d2);
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>
::strictly_contains(const Partially_Reduced_Product& y) const {
  reduce();
  y.reduce();
  return (d1.contains(y.d1) && d2.strictly_contains(y.d2))
    || (d2.contains(y.d2) && d1.strictly_contains(y.d1));
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>::reduce() const {
  Partially_Reduced_Product& dp
    = const_cast<Partially_Reduced_Product&>(*this);
  if (dp.is_reduced())
    return false;
  R r;
  r.product_reduce(dp.d1, dp.d2);
  set_reduced_flag();
  return true;
}

template <typename D1, typename D2, typename R>
inline bool
Partially_Reduced_Product<D1, D2, R>::is_reduced() const {
  return reduced;
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>::clear_reduced_flag() const {
  const_cast<Partially_Reduced_Product&>(*this).reduced = false;
}

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>::set_reduced_flag() const {
  const_cast<Partially_Reduced_Product&>(*this).reduced = true;
}

PPL_OUTPUT_3_PARAM_TEMPLATE_DEFINITIONS(D1, D2, R, Partially_Reduced_Product)

template <typename D1, typename D2, typename R>
inline void
Partially_Reduced_Product<D1, D2, R>::ascii_dump(std::ostream& s) const {
  const char yes = '+';
  const char no = '-';
  s << "Partially_Reduced_Product\n";
  s << (reduced ? yes : no) << "reduced\n";
  s << "Domain 1:\n";
  d1.ascii_dump(s);
  s << "Domain 2:\n";
  d2.ascii_dump(s);
}

template <typename D1, typename D2, typename R>
inline int32_t
Partially_Reduced_Product<D1, D2, R>::hash_code() const {
  return space_dimension() & 0x7fffffff;
}

/*! \relates Parma_Polyhedra_Library::Partially_Reduced_Product */
template <typename D1, typename D2, typename R>
inline bool
operator==(const Partially_Reduced_Product<D1, D2, R>& x,
	   const Partially_Reduced_Product<D1, D2, R>& y) {
  x.reduce();
  y.reduce();
  return x.d1 == y.d1 && x.d2 == y.d2;
}

/*! \relates Parma_Polyhedra_Library::Partially_Reduced_Product */
template <typename D1, typename D2, typename R>
inline bool
operator!=(const Partially_Reduced_Product<D1, D2, R>& x,
	   const Partially_Reduced_Product<D1, D2, R>& y) {
  return !(x == y);
}

/*! \relates Parma_Polyhedra_Library::Partially_Reduced_Product */
template <typename D1, typename D2, typename R>
inline std::ostream&
IO_Operators::operator<<(std::ostream& s, const Partially_Reduced_Product<D1, D2, R>& pd) {
  return s << "Domain 1:\n"
	   << pd.d1
	   << "Domain 2:\n"
	   << pd.d2;
}

template <typename D1, typename D2, typename R>
void
Partially_Reduced_Product<D1, D2, R>::throw_space_dimension_overflow(const char* method,
                                                                     const char* reason) {
  std::ostringstream s;
  s << "Partially_Reduced_Product<D1, D2, R>::" << method << ":" << std::endl
    << reason << ".";
  throw std::length_error(s.str());
}

} // namespace Parma_Polyhedra_Library

namespace Parma_Polyhedra_Library {

template <typename D1, typename D2>
inline
No_Reduction<D1, D2>::No_Reduction() {
}

template <typename D1, typename D2>
void No_Reduction<D1, D2>::product_reduce(D1&, D2&) {
}

template <typename D1, typename D2>
inline
No_Reduction<D1, D2>::~No_Reduction() {
}

template <typename D1, typename D2>
inline
Smash_Reduction<D1, D2>::Smash_Reduction() {
}

template <typename D1, typename D2>
inline
Smash_Reduction<D1, D2>::~Smash_Reduction() {
}

template <typename D1, typename D2>
inline
Constraints_Reduction<D1, D2>::Constraints_Reduction() {
}

template <typename D1, typename D2>
inline
Constraints_Reduction<D1, D2>::~Constraints_Reduction() {
}

} // namespace Parma_Polyhedra_Library

/*! \relates Parma_Polyhedra_Library::Partially_Reduced_Product */
template <typename D1, typename D2, typename R>
inline void
std::swap(Parma_Polyhedra_Library::Partially_Reduced_Product<D1, D2, R>& x,
	  Parma_Polyhedra_Library::Partially_Reduced_Product<D1, D2, R>& y) {
  x.swap(y);
}

#endif // !defined(PPL_Partially_Reduced_Product_inlines_hh)
