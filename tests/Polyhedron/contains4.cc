/* Test NNC_Polyhedron::contains(const Polyhedron&).
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

  NNC_Polyhedron ph1(2);
  ph1.add_constraint(A > 0);
  ph1.add_constraint(B > 0);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(line(A + B));
  NNC_Polyhedron ph2(gs2);

  print_constraints(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  bool ok = !ph1.contains(ph2);

  return ok ? 0 : 1;
}
CATCH
