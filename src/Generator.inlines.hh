/* Generator class implementation: inline functions.
   Copyright (C) 2001, 2002 Roberto Bagnara <bagnara@cs.unipr.it>

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

#ifndef _Generator_inlines_hh
#define _Generator_inlines_hh 1

namespace Parma_Polyhedra_Library {

inline
Generator::Generator(LinExpression& e) {
  swap(e);
}

inline
Generator::Generator(const Generator& g)
  : Row(g) {
}

inline
Generator::Generator(const Generator& g, size_t sz)
  : Row(g, sz, sz) {
}

inline
Generator::~Generator() {
}

inline Generator&
Generator::operator=(const Generator& g) {
  Row::operator=(g);
  return *this;
}

inline size_t
Generator::space_dimension() const {
  return Row::space_dimension();
}

inline bool
Generator::is_line() const {
  return is_line_or_equality();
}

inline bool
Generator::is_ray_or_point() const {
  return is_ray_or_point_or_inequality();
}

inline bool
Generator::is_ray() const {
  return is_ray_or_point() && ((*this)[0] == 0);
}

inline Generator::Type
Generator::type() const {
  if (is_line())
    return LINE;
  const Generator& g = *this;
  if (g[0] == 0)
    return RAY;
  if (is_necessarily_closed())
    return POINT;
  else
    // Checking the value of the \epsilon coefficient.
    return (g[size() - 1] == 0) ? CLOSURE_POINT : POINT;
}

inline bool
Generator::is_point() const {
  return type() == POINT;
}

inline bool
Generator::is_closure_point() const {
  return type() == CLOSURE_POINT;
}

inline void
Generator::set_is_line() {
  set_is_line_or_equality();
}

inline void
Generator::set_is_ray_or_point() {
  set_is_ray_or_point_or_inequality();
}

inline const Integer&
Generator::coefficient(Variable v) const {
  if (v.id() >= space_dimension())
    throw_dimension_incompatible("PPL::Generator::coefficient(v)", v);
  return Row::coefficient(v.id());
}

inline const Integer&
Generator::divisor() const {
  const Integer& d = Row::inhomogeneous_term();
  if (!is_ray_or_point() || d == 0)
    throw_invalid_argument("PPL::Generator::divisor()",
			   "*this is is neither a point "
			   "nor a closure point");
  return d;
}


inline const Generator&
Generator::zero_dim_point() {
  static Generator zdp = point();
  return zdp;
}

inline const Generator&
Generator::zero_dim_closure_point() {
  static Generator zdcp = closure_point();
  return zdcp;
}

} // namespace Parma_Polyhedra_Library

namespace std {

/*!
  Specializes <CODE>std::swap</CODE> to use the fast swap that is
  provided as a member function instead of using the default
  algorithm (which creates a temporary and uses assignment).
*/
inline void
swap(Parma_Polyhedra_Library::Generator& x,
     Parma_Polyhedra_Library::Generator& y) {
  x.swap(y);
}

} // namespace std

#endif // _Generator_inlines_hh
