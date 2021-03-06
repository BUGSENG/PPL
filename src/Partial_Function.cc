/* Implementation of class Partial_Function (non-inline functions).
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
#include "Partial_Function_defs.hh"
#include "Variable_defs.hh"
#include <iostream>

namespace Parma_Polyhedra_Library {

void
Partial_Function::print(std::ostream& s) const {
  using namespace Parma_Polyhedra_Library::IO_Operators;
  if (has_empty_codomain()) {
    s << "empty" << std::endl;
  }
  else {
    for (dimension_type i = 0, i_end = vec.size(); i < i_end; ++i) {
      if (vec[i] != not_a_dimension()) {
        s << Variable(i) << " --> " << Variable(vec[i]) << "\n";
      }
    }
  }
}

} // namespace Parma_Polyhedra_Library
