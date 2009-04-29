/* Variable class implementation (non-inline functions).
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
#include "Variable.defs.hh"
#include <iostream>

namespace PPL = Parma_Polyhedra_Library;

PPL::Variable::output_function_type*
PPL::Variable::current_output_function = 0;

bool
PPL::Variable::OK() const {
  return id() < max_space_dimension();
}

void
PPL::Variable::default_output_function(std::ostream& s, const Variable& v) {
  dimension_type varid = v.id();
  s << static_cast<char>('A' + varid % 26);
  if (dimension_type i = varid / 26)
    s << i;
}

/*! \relates Parma_Polyhedra_Library::Variable */
std::ostream&
PPL::IO_Operators::operator<<(std::ostream& s, const Variable& v) {
  (*Variable::current_output_function)(s, v);
  return s;
}
