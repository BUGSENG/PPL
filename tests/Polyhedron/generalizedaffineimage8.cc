/* Test Polyhedron::generalized_affine_image(): we apply this
   function to a non necessarily closed polyhedron with the
   relation `>'.
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
  ph.add_constraint(A == 1);
  ph.add_constraint(B > 1);
  ph.add_constraint(B < 2);

  print_constraints(ph, "*** ph ***");
  print_generators(ph, "*** ph ***");

  ph.generalized_affine_image(B, GREATER_THAN, A + B + 1);

  NNC_Polyhedron known_result(2);
  known_result.add_constraint(A == 1);
  known_result.add_constraint(B > 3);

  int retval = (ph == known_result) ? 0 : 1;

  print_constraints(ph, "*** After ph.generalized_affine_image"
		    "(B, GREATER_THAN, A + B + 1) ***");
  print_generators(ph, "*** After ph.generalized_affine_image"
		   "(B, PPLGT, A + B + 1) ***");

  return retval;
}
CATCH
