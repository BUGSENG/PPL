/* Test Polyhedron::is_topologically_closed(): a polyhedron
   that contains in its minimized system of generators
   non-redundant closure point that are not
   matched by a corresponding point is not topologically closed.
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

  Variable A(0);

  NNC_Polyhedron ph1(1, EMPTY);

  Generator_System gs1;
  gs1.insert(point(A));
  gs1.insert(closure_point());
  gs1.insert(closure_point(A));
  ph1.add_generators_and_minimize(gs1);

  bool ok = !ph1.is_topologically_closed();

  print_generators(ph1, "*** ph1 ***");

  return ok ? 0 : 1;
}
CATCH
