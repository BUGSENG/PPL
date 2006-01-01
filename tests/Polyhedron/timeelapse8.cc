/* Test Polyhedron::time_elapse_assign(): both polyhedra can have
   something pending.
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
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph1(2);
  ph1.generators();
  ph1.add_constraint(A == 0);
  ph1.add_constraint(B == 0);

  C_Polyhedron ph2(2);
  ph2.generators();
  ph2.add_constraint(A == 2);
  ph2.add_constraint(B == 2);

  print_constraints(ph1, "*** ph1 ***");
  print_constraints(ph2, "*** ph2 ***");

  ph1.time_elapse_assign(ph2);

  C_Polyhedron ph3(2, EMPTY);
  ph3.add_generator(point());
  ph3.constraints();

  C_Polyhedron ph4(2, EMPTY);
  ph4.add_generator(point(2*A + 2*B));

  print_generators(ph3, "*** ph3 ***");
  print_generators(ph4, "*** ph4 ***");

  ph3.time_elapse_assign(ph4);

  int retval = (ph1 ==  ph3) ? 0 : 1;

  print_generators(ph1, "*** After ph1.time_elapse_assign(ph2) ***");
  print_generators(ph3, "*** After ph3.time_elapse_assign(ph4) ***");

  return retval;
}
CATCH
