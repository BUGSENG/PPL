/* Test Polyhedron::affine_preimage(): we apply this function to a
   polyhedron that can have something pending.
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

namespace {

void
test1() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph(2);
  ph.generators();
  ph.add_constraint(A >= 0);
  ph.add_constraint(B >= 0);
  C_Polyhedron copy_ph(ph);

  print_constraints(ph, "*** ph ***");

  ph.affine_preimage(A, A + 1);
  copy_ph.affine_preimage(A, -A - 1, -1);

  bool ok = (ph == copy_ph);

  print_generators(ph, "*** After ph.affine_preimage(A, A + 1) ***");
  print_generators(copy_ph,
		   "*** After copy_ph.affine_preimage(A, -A - 1, -1) ***");

  if (!ok)
    exit(1);
}

void
test2() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph(2);
  ph.generators();
  ph.add_constraint(A >= 0);
  ph.add_constraint(B >= 0);
  C_Polyhedron copy_ph(ph);

  print_constraints(ph, "*** ph ***");

  ph.affine_preimage(B, A + 1);
  copy_ph.affine_preimage(B, -A - 1, -1);

  bool ok = (ph == copy_ph);

  print_generators(ph, "*** After ph.affine_preimage(B, A + 1) ***");
  print_generators(copy_ph,
		   "*** After copy_ph.affine_preimage(B, -A - 1, -1) ***");

  if (!ok)
    exit(1);
}

} // namespace

int
main() TRY {
  set_handlers();

  test1();
  test2();

  return 0;
}
CATCH
