/* Test Polyhedron::generalized_affine_image() with a linear expression
   as the left hand side: when the set of variables of left hand side
   and the set of the variable of the right hand side are not disjoint
   and the relation is LESS_THAN.
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

  NNC_Polyhedron ph(2);
  ph.add_constraint(A >= 0);
  ph.add_constraint(A <= 2);
  ph.add_constraint(B >= 0);
  ph.add_constraint(B <= 2);

  print_constraints(ph, "*** ph ***");

  ph.generalized_affine_image(A + B, LESS_THAN, 2*A - 3*B);

  NNC_Polyhedron known_result(2);
  known_result.add_constraint(A + B < 4);

  int retval = (ph == known_result) ? 0 : 1;

  print_constraints(ph, "*** After ph.generalized_affine_image"
		    "(A + B, LESS_THAN, 2*A - 3*B) ***");

  return retval;
}
CATCH
