/* Test BD_Shape::constraints().
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

namespace {

void
test1() {

  TBD_Shape bd1(0, EMPTY);

  TBD_Shape known_result(bd1);

  Constraint_System cs = bd1.constraints();
  TBD_Shape bd2(cs);

  bool ok = (bd2 == known_result);

  print_constraints(bd1, "*** bd1 ***");
  print_constraints(bd2, "*** bd2 ***");
  print_constraints(cs, "*** cs ***");

  if (!ok)
    exit(1);
}

void
test2() {

  TBD_Shape bd1(0, UNIVERSE);

  TBD_Shape known_result(bd1);

  Constraint_System cs = bd1.constraints();
  TBD_Shape bd2(cs);

  bool ok = (bd2 == known_result);

  print_constraints(bd1, "*** bd1 ***");
  print_constraints(bd2, "*** bd2 ***");
  print_constraints(cs, "*** cs ***");

  if (!ok)
    exit(1);
}

void
test3() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBD_Shape bd1(3);

  bd1.add_constraint(A >= 0);
  bd1.add_constraint(B >= 0);
  bd1.add_constraint(B - C >= 1);
  bd1.add_constraint(C - A <= 9);

  TBD_Shape known_result(bd1);

  bd1.contains(bd1);

 Constraint_System cs = bd1.constraints();
 TBD_Shape bd2(cs);

  bool ok = (bd2 == known_result);

  print_constraints(bd1, "*** bd1 ***");
  print_constraints(bd2, "*** bd2 ***");
  print_constraints(cs, "*** cs ***");

  if (!ok)
    exit(1);
}

} // namespace

int
main() TRY {

  test1();
  test2();
  test3();

  return 0;
}
CATCH
