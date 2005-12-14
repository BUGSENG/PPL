/* Test add_space_dimensions_and_project() for NNC_Polyhedron.
   Copyright (C) 2001-2005 Roberto Bagnara <bagnara@cs.unipr.it>

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

int
main() TRY {
  set_handlers();

  Variable x(0);
  Variable y(1);
  Variable z(2);
  Variable w(3);

  Constraint_System cs;
  cs.insert(x > 2);
  cs.insert(y > 2);
  cs.insert(x < 6);
  cs.insert(y < 6);

  NNC_Polyhedron ph(cs);

  ph.generators();

  print_constraints(ph, "*** ph ***");
  print_generators(ph, "*** ph ***");

  ph.add_space_dimensions_and_project(2);

  NNC_Polyhedron known_result(4);
  known_result.add_constraint(z == 0);
  known_result.add_constraint(w == 0);
  known_result.add_constraint(x > 2);
  known_result.add_constraint(y > 2);
  known_result.add_constraint(x < 6);
  known_result.add_constraint(y < 6);

  int retval = (ph == known_result) ? 0 : 1;

  print_constraints(ph, "*** After add_space_dimensions_and_project ***");
  print_generators(ph, "*** After add_space_dimensions_and_project ***");

  return retval;
}
CATCH
