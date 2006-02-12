/* Test Polyhedron::BHRZ03_widening_assign(): the third technique
   of this function is tested.
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

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(A));
  gs1.insert(ray(A + B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(A));
  gs2.insert(ray(A + 2*B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(B >= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test2() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(-A));
  gs1.insert(ray(-A + B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(-A));
  gs2.insert(ray(-A + 2*B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A <= 0);
  known_result.add_constraint(B >= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test3() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(-A));
  gs1.insert(ray(-A - B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(-A));
  gs2.insert(ray(-A - 2*B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A <= 0);
  known_result.add_constraint(B <= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test4() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(A));
  gs1.insert(ray(A - B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(A));
  gs2.insert(ray(A - 2*B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(B <= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test5() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(B));
  gs1.insert(ray(A + 2*B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(B));
  gs2.insert(ray(A + B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(B >= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test6() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(B));
  gs1.insert(ray(-A + 2*B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(B));
  gs2.insert(ray(-A + B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A <= 0);
  known_result.add_constraint(B >= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test7() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(-B));
  gs1.insert(ray(-A - 2*B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(-B));
  gs2.insert(ray(-A - B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A <= 0);
  known_result.add_constraint(B <= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test8() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(-B));
  gs1.insert(ray(A - 2*B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(-B));
  gs2.insert(ray(A - B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(B <= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test9() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(A));
  gs1.insert(ray(A + B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(A));
  gs2.insert(ray(-A + B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(B >= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test10() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(B));
  gs1.insert(ray(-A + B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(B));
  gs2.insert(ray(-A - B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A <= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test11() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(-A));
  gs1.insert(ray(-A - B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(-A));
  gs2.insert(ray(A - B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(B <= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test12() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(-B));
  gs1.insert(ray(A - B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(-B));
  gs2.insert(ray(A + B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A >= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test13() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(-A));
  gs1.insert(ray(-A + B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(-A));
  gs2.insert(ray(A + B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(B >= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test14() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(-B));
  gs1.insert(ray(-A - B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(-B));
  gs2.insert(ray(-A + B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A <= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test15() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(A));
  gs1.insert(ray(A - B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(A));
  gs2.insert(ray(-A - B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(B <= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

void
test16() {
  Variable A(0);
  Variable B(1);

  Generator_System gs1;
  gs1.insert(point());
  gs1.insert(ray(B));
  gs1.insert(ray(A + B));
  C_Polyhedron ph1(gs1);

  Generator_System gs2;
  gs2.insert(point());
  gs2.insert(ray(B));
  gs2.insert(ray(A - B));
  C_Polyhedron ph2(gs2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  ph2.BHRZ03_widening_assign(ph1);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A >= 0);

  bool equal = (ph2 == known_result);

  print_generators(ph2, "*** After ph2.BHRZ03_widening_assign(ph1) ***");

  if (!equal)
    exit(1);
}

} // namespace

int
main() TRY {
  set_handlers();

  DO_TEST(test1);
  DO_TEST(test2);
  DO_TEST(test3);
  DO_TEST(test4);
  DO_TEST(test5);
  DO_TEST(test6);
  DO_TEST(test7);
  DO_TEST(test8);
  DO_TEST(test9);
  DO_TEST(test10);
  DO_TEST(test11);
  DO_TEST(test12);
  DO_TEST(test13);
  DO_TEST(test14);
  DO_TEST(test15);
  DO_TEST(test16);

  return 0;
}
CATCH
