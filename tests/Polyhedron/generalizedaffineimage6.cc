/* Test Polyhedron::generalized_affine_image() with a linear expression
   as the left hand side.
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
  Variable B(1);

  C_Polyhedron ph(2, EMPTY);
  ph.add_generator(point(A + B));
  ph.add_generator(point(3*A + B));
  ph.add_generator(point(A + 3*B));
  ph.add_generator(point(3*A + 3*B));

  print_constraints(ph, "*** ph ***");

  C_Polyhedron known_result(2);
  known_result.add_constraint(A + B >= 1);

  ph.generalized_affine_image(A + B, GREATER_THAN_OR_EQUAL, 2*A - B + 2);

  int retval = (ph == known_result) ? 0 : 1;

  print_generators(ph,
		   "*** After ph.generalized_affine_image"
		   "(A + B, GREATER_THAN_OR_EQUAL, 2*A - B + 2) ***");

  return retval;
}
CATCH
