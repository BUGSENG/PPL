/* NNC_Polyhedron class implementation: inline functions.
   Copyright (C) 2001-2004 Roberto Bagnara <bagnara@cs.unipr.it>

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

#ifndef PPL_NNC_Polyhedron_inlines_hh
#define PPL_NNC_Polyhedron_inlines_hh 1

#include "C_Polyhedron.defs.hh"

namespace Parma_Polyhedra_Library {

inline
NNC_Polyhedron::NNC_Polyhedron(dimension_type num_dimensions,
			       Degenerate_Kind kind)
  : Polyhedron(NOT_NECESSARILY_CLOSED, num_dimensions, kind) {
}

inline
NNC_Polyhedron::NNC_Polyhedron(const ConSys& cs)
  : Polyhedron(NOT_NECESSARILY_CLOSED, cs) {
}

inline
NNC_Polyhedron::NNC_Polyhedron(ConSys& cs)
  : Polyhedron(NOT_NECESSARILY_CLOSED, cs) {
}

inline
NNC_Polyhedron::NNC_Polyhedron(const GenSys& gs)
  : Polyhedron(NOT_NECESSARILY_CLOSED, gs) {
}

inline
NNC_Polyhedron::NNC_Polyhedron(GenSys& gs)
  : Polyhedron(NOT_NECESSARILY_CLOSED, gs) {
}

template <typename Box>
NNC_Polyhedron::NNC_Polyhedron(const Box& box, From_Bounding_Box)
  : Polyhedron(NOT_NECESSARILY_CLOSED, box) {
}

inline
NNC_Polyhedron::NNC_Polyhedron(const NNC_Polyhedron& y)
  : Polyhedron(y) {
}

inline NNC_Polyhedron&
NNC_Polyhedron::operator=(const NNC_Polyhedron& y) {
  Polyhedron::operator=(y);
  return *this;
}

inline
NNC_Polyhedron::~NNC_Polyhedron() {
}

inline dimension_type
NNC_Polyhedron::max_space_dimension() {
  // We reserve one dimension for the epsilon dimension.
  return C_Polyhedron::max_space_dimension() - 1;
}

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_NNC_Polyhedron_inlines_hh)
