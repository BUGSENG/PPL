/* Test Polyhedron::bounded_affine_image().
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

  Variable A(0);
  Variable B(1);

  C_Polyhedron ph(2);
  ph.add_constraint(A >= 0);
  ph.add_constraint(A <= 4);
  ph.add_constraint(B >= 0);
  ph.add_constraint(B <= 4);
  ph.add_constraint(A - B <= 2);
  ph.add_constraint(A - B >= -2);

  print_constraints(ph, "--- ph ---");

  C_Polyhedron kr1 = ph;
  C_Polyhedron kr2 = ph;

  ph.bounded_affine_image(A, 7-3*A+2*B, B+5*A-3);

  kr1.generalized_affine_image(A, GREATER_THAN_OR_EQUAL, 7-3*A+2*B);
  kr2.generalized_affine_image(A, LESS_THAN_OR_EQUAL, B+5*A-3);
  kr1.intersection_assign(kr2);

  int retval = (ph == kr1) ? 0 : 1;

  print_generators(ph, "--- ph after "
		   "ph.bounded_affine_image(A, 7-3*A+2*B, B+5*A-3)"
		   " ---");

  return retval;
}
CATCH
