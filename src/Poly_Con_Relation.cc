/* Poly_Con_Relation class implementation (non-inline functions).
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

#include "Poly_Con_Relation.defs.hh"

#include <iostream>
#include <string>
#include <cassert>

namespace PPL = Parma_Polyhedra_Library;

void
PPL::Poly_Con_Relation::ascii_dump(std::ostream& s) const {
  flags_t f = flags;
  if (f == NOTHING) {
    s << "NOTHING";
    return;
  }

  while (true) {
    if (implies(f, IS_DISJOINT)) {
      s << "IS_DISJOINT";
      f &= ~IS_DISJOINT;
    }
    else if (implies(f, STRICTLY_INTERSECTS)) {
      s << "STRICTLY_INTERSECTS";
      f &= ~STRICTLY_INTERSECTS;
    }
    else if (implies(f, IS_INCLUDED)) {
      s << "IS_INCLUDED";
      f &= ~IS_INCLUDED;
    }
    else if (implies(f, SATURATES)) {
      s << "SATURATES";
      f &= ~SATURATES;
    }
    if (f != NOTHING)
      s << " & ";
    else
      break;
  }
}

PPL_OUTPUT_DEFINITIONS(Poly_Con_Relation)

/*! \relates Parma_Polyhedra_Library::Poly_Con_Relation */
std::ostream&
PPL::IO_Operators::operator<<(std::ostream& s, const Poly_Con_Relation& r) {
  r.ascii_dump(s);
  return s;
}

bool
PPL::Poly_Con_Relation::OK() const {
  return true;
}
