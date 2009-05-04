/* Pointset_Ask_Tell class implementation: inline functions.
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

#ifndef PPL_Pointset_Ask_Tell_inlines_hh
#define PPL_Pointset_Ask_Tell_inlines_hh 1

#include "Constraint.defs.hh"
#include "Constraint_System.defs.hh"
#include "Constraint_System.inlines.hh"
#include "Congruence.defs.hh"
#include "Congruence_System.defs.hh"
#include "Congruence_System.inlines.hh"
#include "C_Polyhedron.defs.hh"
#include "NNC_Polyhedron.defs.hh"
#include <algorithm>
#include <deque>

namespace Parma_Polyhedra_Library {

template <typename PSET>
inline dimension_type
Pointset_Ask_Tell<PSET>::space_dimension() const {
  return space_dim;
}

template <typename PSET>
inline dimension_type
Pointset_Ask_Tell<PSET>::max_space_dimension() {
  return PSET::max_space_dimension();
}

template <typename PSET>
inline
Pointset_Ask_Tell<PSET>::Pointset_Ask_Tell(dimension_type num_dimensions,
					 Degenerate_Element kind)
  : Base(), space_dim(num_dimensions) {
  if (kind == EMPTY)
    pair_insert(COW_Pointset<PSET>(PSET(num_dimensions, UNIVERSE)),
		COW_Pointset<PSET>(PSET(num_dimensions, EMPTY)));
  assert(OK());
}

template <typename PSET>
inline
Pointset_Ask_Tell<PSET>::Pointset_Ask_Tell(const Pointset_Ask_Tell& y)
  : Base(y), space_dim(y.space_dim) {
}

template <typename PSET>
inline
Pointset_Ask_Tell<PSET>::Pointset_Ask_Tell(const PSET& ph)
  : Base(ph), space_dim(ph.space_dimension()) {
}

template <typename PSET>
inline
Pointset_Ask_Tell<PSET>::Pointset_Ask_Tell(const Constraint_System& cs)
  : Base(COW_Pointset<PSET>(cs)), space_dim(cs.space_dimension()) {
  assert(OK());
}

template <typename PSET>
inline
Pointset_Ask_Tell<PSET>::Pointset_Ask_Tell(const Congruence_System& cgs)
  : Base(COW_Pointset<PSET>(cgs)), space_dim(cgs.space_dimension()) {
  assert(OK());
}

template <typename PSET>
inline Pointset_Ask_Tell<PSET>&
Pointset_Ask_Tell<PSET>::operator=(const Pointset_Ask_Tell& y) {
  Pointset_Ask_Tell& x = *this;
  x.Base::operator=(y);
  x.space_dim = y.space_dim;
  return x;
}

template <typename PSET>
inline void
Pointset_Ask_Tell<PSET>::swap(Pointset_Ask_Tell& y) {
  Pointset_Ask_Tell& x = *this;
  x.Base::swap(y);
  std::swap(x.space_dim, y.space_dim);
}

template <typename PSET>
template <typename QH>
inline Pointset_Ask_Tell<PSET>&
Pointset_Ask_Tell<PSET>::operator=(const Pointset_Ask_Tell<QH>& y) {
  Pointset_Ask_Tell& x = *this;
  Pointset_Ask_Tell<PSET> pps(y);
  x.swap(pps);
  return x;
}

template <typename PSET>
inline void
Pointset_Ask_Tell<PSET>::intersection_assign(const Pointset_Ask_Tell& y) {
  Pointset_Ask_Tell& x = *this;
  x.pairwise_apply_assign
    (y,
     COW_PSET::lift_op_assign(std::mem_fun_ref(&PSET::intersection_assign)));
}

template <typename PSET>
inline void
Pointset_Ask_Tell<PSET>::time_elapse_assign(const Pointset_Ask_Tell& y) {
  Pointset_Ask_Tell& x = *this;
  x.pairwise_apply_assign
    (y,
     COW_PSET::lift_op_assign(std::mem_fun_ref(&PSET::time_elapse_assign)));
}

template <typename PSET>
inline bool
Pointset_Ask_Tell<PSET>
::geometrically_covers(const Pointset_Ask_Tell& y) const {
  const Pointset_Ask_Tell<NNC_Polyhedron> xx(*this);
  const Pointset_Ask_Tell<NNC_Polyhedron> yy(y);
  return xx.geometrically_covers(yy);
}

template <typename PSET>
inline bool
Pointset_Ask_Tell<PSET>
::geometrically_equals(const Pointset_Ask_Tell& y) const {
  const Pointset_Ask_Tell<NNC_Polyhedron> xx(*this);
  const Pointset_Ask_Tell<NNC_Polyhedron> yy(y);
  return xx.geometrically_covers(yy) && yy.geometrically_covers(xx);
}

template <>
inline bool
Pointset_Ask_Tell<NNC_Polyhedron>
::geometrically_equals(const Pointset_Ask_Tell& y) const {
  const Pointset_Ask_Tell& x = *this;
  return x.geometrically_covers(y) && y.geometrically_covers(x);
}

template <typename PSET>
inline memory_size_type
Pointset_Ask_Tell<PSET>::external_memory_in_bytes() const {
  return Base::external_memory_in_bytes();
}

template <typename PSET>
inline memory_size_type
Pointset_Ask_Tell<PSET>::total_memory_in_bytes() const {
  return sizeof(*this) + external_memory_in_bytes();
}

template <typename PSET>
inline int32_t
Pointset_Ask_Tell<PSET>::hash_code() const {
  return space_dimension() & 0x7fffffff;
}

template <>
inline void
Pointset_Ask_Tell<C_Polyhedron>
::poly_difference_assign(const Pointset_Ask_Tell& y) {
  Pointset_Ask_Tell<NNC_Polyhedron> nnc_this(*this);
  Pointset_Ask_Tell<NNC_Polyhedron> nnc_y(y);
  nnc_this.poly_difference_assign(nnc_y);
  *this = nnc_this;
}

/*! \relates Pointset_Ask_Tell */
template <typename PSET>
inline bool
check_containment(const PSET& ph, const Pointset_Ask_Tell<PSET>& ps) {
  const NNC_Polyhedron pph = NNC_Polyhedron(ph.constraints());
  const Pointset_Ask_Tell<NNC_Polyhedron> pps(ps);
  return check_containment(pph, pps);
}

/*! \relates Pointset_Ask_Tell */
template <>
inline bool
check_containment(const C_Polyhedron& ph,
		  const Pointset_Ask_Tell<C_Polyhedron>& ps) {
  return check_containment(NNC_Polyhedron(ph),
			   Pointset_Ask_Tell<NNC_Polyhedron>(ps));
}

} // namespace Parma_Polyhedra_Library


namespace std {

/*! \relates Parma_Polyhedra_Library::Pointset_Ask_Tell */
template <typename PSET>
inline void
swap(Parma_Polyhedra_Library::Pointset_Ask_Tell<PSET>& x,
     Parma_Polyhedra_Library::Pointset_Ask_Tell<PSET>& y) {
  x.swap(y);
}

} // namespace std

#endif // !defined(PPL_Pointset_Ask_Tell_inlines_hh)
