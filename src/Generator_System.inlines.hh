/* Generator_System class implementation: inline functions.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2011 BUGSENG srl (http://bugseng.com)

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

#ifndef PPL_Generator_System_inlines_hh
#define PPL_Generator_System_inlines_hh 1

#include "Generator.defs.hh"

// TODO: Remove this.
// It was added to please KDevelop4.
#include "Generator_System.defs.hh"

namespace Parma_Polyhedra_Library {

inline
Generator_System::Generator_System()
  : sys(NECESSARILY_CLOSED) {
}

inline
Generator_System::Generator_System(const Generator& g)
  : sys(g.topology()) {
  sys.insert(g);
}

inline
Generator_System::Generator_System(const Generator_System& gs)
  : sys(gs.sys) {
}

inline
Generator_System::Generator_System(const Topology topol)
  : sys(topol) {
}

inline
Generator_System::Generator_System(const Topology topol,
				   const dimension_type n_rows,
				   const dimension_type n_columns)
  : sys(topol, n_rows, n_columns) {
}

inline
Generator_System::~Generator_System() {
}

inline Generator_System&
Generator_System::operator=(const Generator_System& y) {
  sys = y.sys;
  return *this;
}

inline dimension_type
Generator_System::max_space_dimension() {
  return Linear_System<Generator>::max_space_dimension();
}

inline dimension_type
Generator_System::space_dimension() const {
  return sys.space_dimension();
}

inline void
Generator_System::set_space_dimension(dimension_type space_dim) {
  const dimension_type old_space_dim = space_dimension();
  sys.set_space_dimension(space_dim);

  if (space_dim < old_space_dim)
    // We may have invalid lines and rays now.
    remove_invalid_lines_and_rays();
}

inline void
Generator_System::clear() {
  sys.clear();
}

inline const Generator&
Generator_System::operator[](const dimension_type k) const {
  return sys[k];
}

inline void
Generator_System
::remove_space_dimensions(const Variables_Set& vars) {
  sys.remove_space_dimensions(vars);

  // We may have invalid lines and rays now.
  remove_invalid_lines_and_rays();
}

inline void
Generator_System
::permute_space_dimensions(const std::vector<Variable>& cycle) {
  sys.permute_space_dimensions(cycle);
}

inline void
Generator_System
::swap_space_dimensions(Variable v1, Variable v2) {
  sys.swap_space_dimensions(v1, v2);
}

inline dimension_type
Generator_System::num_rows() const {
  return sys.num_rows();
}

inline void
Generator_System::add_universe_rows_and_columns(dimension_type n) {
  sys.add_universe_rows_and_columns(n);
}

inline Topology
Generator_System::topology() const {
  return sys.topology();
}

inline void
Generator_System::remove_trailing_columns(dimension_type n) {
  return sys.remove_trailing_columns(n);
}

inline dimension_type
Generator_System::first_pending_row() const {
  return sys.first_pending_row();
}

inline void
Generator_System::unset_pending_rows() {
  sys.unset_pending_rows();
}

inline void
Generator_System::release_row(Generator& row) {
  sys.release_row(row);
}

inline void
Generator_System::release_rows(Swapping_Vector<Generator>& v) {
  sys.release_rows(v);
}

inline void
Generator_System::take_ownership_of_rows(Swapping_Vector<Generator>& v) {
  sys.take_ownership_of_rows(v);
}

inline void
Generator_System::set_sorted(bool b) {
  sys.set_sorted(b);
}

inline bool
Generator_System::is_sorted() const {
  return sys.is_sorted();
}

inline void
Generator_System::set_index_first_pending_row(dimension_type i) {
  sys.set_index_first_pending_row(i);
}

inline bool
Generator_System::is_necessarily_closed() const {
  return sys.is_necessarily_closed();
}

inline void
Generator_System::assign_with_pending(const Generator_System& y) {
  sys.assign_with_pending(y.sys);
}

inline dimension_type
Generator_System::num_pending_rows() const {
  return sys.num_pending_rows();
}

inline void
Generator_System::sort_pending_and_remove_duplicates() {
  return sys.sort_pending_and_remove_duplicates();
}

inline void
Generator_System::sort_and_remove_with_sat(Bit_Matrix& sat) {
  sys.sort_and_remove_with_sat(sat);
}

inline void
Generator_System::sort_rows() {
  sys.sort_rows();
}

inline bool
Generator_System::check_sorted() const {
  return sys.check_sorted();
}

inline dimension_type
Generator_System::num_lines_or_equalities() const {
  return sys.num_lines_or_equalities();
}

inline void
Generator_System::resize_no_copy(dimension_type new_n_rows,
                                 dimension_type new_space_dim) {
  sys.resize_no_copy(new_n_rows, new_space_dim);
}

inline void
Generator_System::swap_rows(dimension_type i, dimension_type j) {
  sys.swap_rows(i, j);
}

inline void
Generator_System::remove_trailing_rows(dimension_type n) {
  sys.remove_trailing_rows(n);
}

inline dimension_type
Generator_System::gauss(dimension_type n_lines_or_equalities) {
  return sys.gauss(n_lines_or_equalities);
}

inline void
Generator_System::back_substitute(dimension_type n_lines_or_equalities) {
  sys.back_substitute(n_lines_or_equalities);
}

inline void
Generator_System::strong_normalize() {
  sys.strong_normalize();
}

inline void
Generator_System::merge_rows_assign(const Generator_System& y) {
  sys.merge_rows_assign(y.sys);
}

inline void
Generator_System::insert(const Generator_System& y) {
  sys.insert(y.sys);
}

inline void
Generator_System::insert_pending(const Generator_System& r) {
  sys.insert_pending(r.sys);
}

inline bool
operator==(const Generator_System& x, const Generator_System& y) {
  return x.sys == y.sys;
}

inline bool
operator!=(const Generator_System& x, const Generator_System& y) {
  return !(x == y);
}

inline
Generator_System_const_iterator::Generator_System_const_iterator()
  : i(), gsp(0) {
}

inline
Generator_System_const_iterator::Generator_System_const_iterator(const Generator_System_const_iterator& y)
  : i(y.i), gsp(y.gsp) {
}

inline
Generator_System_const_iterator::~Generator_System_const_iterator() {
}

inline
Generator_System_const_iterator&
Generator_System_const_iterator::operator=(const Generator_System_const_iterator& y) {
  i = y.i;
  gsp = y.gsp;
  return *this;
}

inline const Generator&
Generator_System_const_iterator::operator*() const {
  return *i;
}

inline const Generator*
Generator_System_const_iterator::operator->() const {
  return i.operator->();
}

inline Generator_System_const_iterator&
Generator_System_const_iterator::operator++() {
  ++i;
  if (!gsp->is_necessarily_closed())
    skip_forward();
  return *this;
}

inline Generator_System_const_iterator
Generator_System_const_iterator::operator++(int) {
  const Generator_System_const_iterator tmp = *this;
  operator++();
  return tmp;
}

inline bool
Generator_System_const_iterator::operator==(const Generator_System_const_iterator& y) const {
  return i == y.i;
}

inline bool
Generator_System_const_iterator::operator!=(const Generator_System_const_iterator& y) const {
  return i != y.i;
}

inline
Generator_System_const_iterator::
Generator_System_const_iterator(const Linear_System<Generator>::const_iterator& iter,
                                const Generator_System& gsys)
  : i(iter), gsp(&gsys.sys) {
}

inline bool
Generator_System::empty() const {
  return sys.has_no_rows();
}

inline bool
Generator_System::has_no_rows() const {
  return sys.has_no_rows();
}

inline Generator_System::const_iterator
Generator_System::begin() const {
  const_iterator i(sys.begin(), *this);
  if (!sys.is_necessarily_closed())
    i.skip_forward();
  return i;
}

inline Generator_System::const_iterator
Generator_System::end() const {
  const const_iterator i(sys.end(), *this);
  return i;
}

inline const Generator_System&
Generator_System::zero_dim_univ() {
  PPL_ASSERT(zero_dim_univ_p != 0);
  return *zero_dim_univ_p;
}

inline void
Generator_System::swap(Generator_System& y) {
  sys.swap(y.sys);
}

inline memory_size_type
Generator_System::external_memory_in_bytes() const {
  return sys.external_memory_in_bytes();
}

inline memory_size_type
Generator_System::total_memory_in_bytes() const {
  return external_memory_in_bytes() + sizeof(*this);
}

inline void
Generator_System::simplify() {
  sys.simplify();
  remove_invalid_lines_and_rays();
}

} // namespace Parma_Polyhedra_Library


namespace std {

/*! \relates Parma_Polyhedra_Library::Constraint_System */
inline void
swap(Parma_Polyhedra_Library::Generator_System& x,
     Parma_Polyhedra_Library::Generator_System& y) {
  x.swap(y);
}

} // namespace std

#endif // !defined(PPL_Generator_System_inlines_hh)
