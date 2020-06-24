/* Pointset_Ask_Tell class implementation: non-inline functions.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2020 BUGSENG srl (http://bugseng.com)

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
#include "Pointset_Ask_Tell_defs.hh"

namespace PPL = Parma_Polyhedra_Library;

template <>
template <>
PPL::Pointset_Ask_Tell<PPL::NNC_Polyhedron>
::Pointset_Ask_Tell(const Pointset_Ask_Tell<C_Polyhedron>& y)
  : Base(), space_dim(y.space_dimension()) {
  Pointset_Ask_Tell& x = *this;
  for (Pointset_Ask_Tell<C_Polyhedron>::const_iterator i = y.begin(),
         y_end = y.end(); i != y_end; ++i) {
    Determinate<NNC_Polyhedron> nnc_ask(NNC_Polyhedron(i->ask().pointset()));
    Determinate<NNC_Polyhedron> nnc_tell(NNC_Polyhedron(i->tell().pointset()));
    x.sequence.push_back(Pair(nnc_ask, nnc_tell));
  }
  x.normalized = y.normalized;
  PPL_ASSERT_HEAVY(x.OK());
}

template <>
template <>
PPL::Pointset_Ask_Tell<PPL::C_Polyhedron>
::Pointset_Ask_Tell(const Pointset_Ask_Tell<NNC_Polyhedron>& y)
  : Base(), space_dim(y.space_dimension()) {
  Pointset_Ask_Tell& x = *this;
  for (Pointset_Ask_Tell<NNC_Polyhedron>::const_iterator i = y.begin(),
         y_end = y.end(); i != y_end; ++i) {
    Determinate<C_Polyhedron> c_ask(C_Polyhedron(i->ask().pointset()));
    Determinate<C_Polyhedron> c_tell(C_Polyhedron(i->tell().pointset()));
    x.sequence.push_back(Pair(c_ask, c_tell));
  }

  // Note: this might be non-normalized even when `y' is known to be
  // normalized, because the constructor of C_Polyhedron, by enforcing
  // topological closure, may have made different elements comparable.
  x.normalized = false;
  PPL_ASSERT_HEAVY(x.OK());
}
