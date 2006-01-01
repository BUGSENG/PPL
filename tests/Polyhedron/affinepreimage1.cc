/* Test Polyhedron::affine_preimage(): we apply this function to
   a polyhedron defined by its system of generators.
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

  Generator_System gs;
  gs.insert(point());
  gs.insert(ray(A + B));
  gs.insert(point(A));

  C_Polyhedron ph(gs);

  print_generators(ph, "--- ph ---");

  ph.affine_preimage(A, A+2);

  Generator_System gs_known_result;
  gs_known_result.insert(point(-2*A));
  gs_known_result.insert(ray(A + B));
  gs_known_result.insert(point(-A));

  C_Polyhedron known_result(gs_known_result);

  int retval = (ph == known_result) ? 0 : 1;

  print_generators(ph, "--- ph after ph.affine_preimage(A, A+2) ---");

  return retval;
}
CATCH
