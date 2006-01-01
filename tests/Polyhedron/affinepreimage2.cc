/* Test Polyhedron::affine_preimage(): we apply this function to
   a polyhedron defined by its system of constraints.
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
  Variable C(2);

  C_Polyhedron ph(3);
  ph.add_constraint(A + C == 0);
  ph.add_constraint(A + B >= 0);
  ph.add_constraint(A + B - 1 >= 0);

  print_constraints(ph, "--- ph ---");

  ph.affine_preimage(A, A + B);

  C_Polyhedron known_result(3);
  known_result.add_constraint(A + B + C == 0);
  known_result.add_constraint(A + 2*B >= 0);
  known_result.add_constraint(A + 2*B -1 >= 0);

  int retval = (ph == known_result) ? 0 : 1;

  print_constraints(ph, "--- ph after ph.affine_preimage(A, A+B) ---");

  return retval;
}
CATCH
