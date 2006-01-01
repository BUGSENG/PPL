/* Remove some variables from the space.
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

int
main() TRY {
  set_handlers();

  Generator_System gs;

  // Creating 10 points.
  for (int i = 0; i < 10; i++) {
    Linear_Expression e;
    for (int j = 0; j < 10; j++)
      e += (10*i + j) * Variable(j);
    gs.insert(point(e));
  }

  C_Polyhedron ph(gs);

  print_generators(ph, "*** before ***");

  // This is the set of the variables that we want to remove.
  Variables_Set to_be_removed;
  to_be_removed.insert(Variable(0));
  to_be_removed.insert(Variable(5));
  to_be_removed.insert(Variable(3));
  to_be_removed.insert(Variable(4));
  to_be_removed.insert(Variable(8));

  ph.remove_space_dimensions(to_be_removed);

  // Useless, but much clearer.
  gs.clear();

  Variable a(0);
  Variable b(1);
  Variable c(2);
  Variable d(3);
  Variable e(4);

  Linear_Expression expr01 = (1*a + 2*b + 6*c + 7*d + 9*e);
  Linear_Expression expr10 = 10 * (a + b + c + d + e);

  for (int i = 0; i < 10; i++) {
    Linear_Expression expr = i * expr10 + expr01;
    gs.insert(point(expr));
  }

  C_Polyhedron known_result(gs);

  int retval = (ph == known_result ? 0 : 1);

  print_generators(ph, "*** after ***");
  print_generators(known_result, "*** known_result ***");

  return retval;
}
CATCH
