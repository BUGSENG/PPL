/* Test Polyhedron::poly_hull_assign().
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

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(x));
  gs1.insert(ray(y));

  C_Polyhedron ph1(gs1);

  print_generators(ph1, "*** ph1 ***");

  Generator_System gs2;
  gs2.insert(point(-x + y));
  gs2.insert(point(x + y));
  gs2.insert(point(3*x));

  C_Polyhedron ph2(gs2);

  print_generators(ph2, "*** ph2 ***");

  C_Polyhedron computed_result = ph1;

  computed_result.poly_hull_assign(ph2);

  Generator_System gs_known_result;
  gs_known_result.insert(point());
  gs_known_result.insert(point(-x + y));
  gs_known_result.insert(ray(x));
  gs_known_result.insert(ray(y));

  C_Polyhedron known_result(gs_known_result);

  print_generators(computed_result, "*** ph1.poly_hull_assign(ph2) ***");

  return (computed_result == known_result) ? 0 : 1;
}
CATCH
