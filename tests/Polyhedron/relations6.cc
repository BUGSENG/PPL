/* Test Polyhedron::relation_with(c): we verify that all the points
   of an empty polyhedron saturate a constraints.
   Copyright (C) 2001-2006 Roberto Bagnara <bagnara@cs.unipr.it>

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
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#include "ppl_test.hh"

using namespace Parma_Polyhedra_Library::IO_Operators;

int
main() TRY {
  set_handlers();

  Variable y(1);

  C_Polyhedron ph(2, EMPTY);
  print_generators(ph, "--- ph ---");

  Constraint c(y >= 0);
  print_constraint(c, "--- c ---");

  Poly_Con_Relation rel = ph.relation_with(c);

  Poly_Con_Relation known_rel = Poly_Con_Relation::saturates()
    && Poly_Con_Relation::is_included()
    && Poly_Con_Relation::is_disjoint();
  int retval = (rel == known_rel) ? 0 : 1;

  nout << "ph.relation_with(c) == " << rel << endl;

  return retval;
}
CATCH
