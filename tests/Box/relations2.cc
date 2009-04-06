/* Test Box::relation_with().
   Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

This file is part of the Parma Polyhedra Library (PPL).

The PPL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
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

bool
test01() {
  // The zero-dim universe BDS.
  TBox box(0);
  Poly_Con_Relation rel = box.relation_with(Linear_Expression(0) > 0);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(0 > 0) == " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::saturates()
    && Poly_Con_Relation::is_disjoint();

  return rel == known_result;
}

bool
test02() {
  // The zero-dim universe box.
  TBox box(0);
  Poly_Con_Relation rel = box.relation_with(Linear_Expression(0) > 1);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(0 > 1) == " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::is_disjoint();

  return rel == known_result;
}

bool
test03() {
  // The zero-dim universe box.
  TBox box(0);
  Poly_Con_Relation rel = box.relation_with(Linear_Expression(1) > 0);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(1 > 0) == " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::is_included();

  return rel == known_result;
}

bool
test04() {
  // An empty box.
  TBox box(1);
  box.add_constraint(Linear_Expression(0) >= 1);

  Variable A(0);

  Poly_Con_Relation rel = box.relation_with(A > 0);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(A > 0) = " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::saturates()
    && Poly_Con_Relation::is_included()
    && Poly_Con_Relation::is_disjoint();

  return rel == known_result;
}

bool
test05() {
  Variable A(0);
  Variable B(1);
  Constraint_System cs(A == 3);
  TBox box(cs);

  Poly_Con_Relation rel = box.relation_with(A > 3);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(A > 3) == " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::saturates()
    && Poly_Con_Relation::is_disjoint();

  return rel == known_result;
}

bool
test06() {
  Variable A(0);
  Variable B(1);
  Constraint_System cs(A <= 3);
  TBox box(cs);

  Poly_Con_Relation rel = box.relation_with(A > 3);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(A > 3) == " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::is_disjoint();

  return rel == known_result;
}

bool
test07() {
  Variable A(0);

  Constraint_System cs;
  cs.insert(A <= 1);

  TBox box(cs);

  Poly_Con_Relation rel = box.relation_with(A > 0);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(A > 0) == " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::strictly_intersects();

  return rel == known_result;
}

bool
test08() {
  Variable A(0);
  Variable B(1);

  Constraint_System cs;
  cs.insert(A >= 1);
  cs.insert(B >= 0);

  TBox box(cs);

  Poly_Con_Relation rel = box.relation_with(A > 1);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(A - B > 1) == " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::strictly_intersects();

  return rel == known_result;
}

bool
test09() {
  Variable A(0);
  Variable B(1);

  Constraint_System cs;
  cs.insert(A >= 1);
  cs.insert(B >= 0);

  TBox box(cs);

  Poly_Con_Relation rel = box.relation_with(A > 0);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(A > 0) == " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::is_included();

  return rel == known_result;
}

bool
test10() {
  Variable A(0);
  Variable B(1);

  Constraint_System cs;
  cs.insert(A == 0);
  cs.insert(B <= -1);

  TBox box(cs);

  Poly_Con_Relation rel = box.relation_with(B > 1);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(B - A > 1) == " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::is_disjoint();

  return rel == known_result;
}

bool
test11() {
  Variable A(0);

  TBox box(1);
  box.add_constraint(A >= 0);

  Poly_Con_Relation rel = box.relation_with(Linear_Expression(1) >= 1);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(1 >= 1) == " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::saturates()
    && Poly_Con_Relation::is_included();

  return rel == known_result;
}

bool
test12() {
  Variable A(0);
  Variable B(1);

  TBox box(2);
  box.add_constraint(A == 1);
  box.add_constraint(B >= 2);

  Poly_Con_Relation rel = box.relation_with(Linear_Expression(1) > 1);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(1 > 1) == " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::saturates()
    && Poly_Con_Relation::is_disjoint();

  return rel == known_result;
}

bool
test13() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  TBox box(3);
  box.add_constraint(A == 1);
  box.add_constraint(B >= 2);
  box.add_constraint(C <= 1);

  Poly_Con_Relation rel = box.relation_with(Linear_Expression(1) == 1);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(1 == 1) == " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::saturates()
    && Poly_Con_Relation::is_included();

  return rel == known_result;
}

bool
test14() {
  Variable A(0);
  Variable B(1);

  TBox box(2);
  box.add_constraint(B == 0);

  Poly_Gen_Relation rel1 = box.relation_with(point(B));
  Poly_Gen_Relation rel2 = box.relation_with(point(-B));

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(point(B)) == " << rel1 << endl;
  nout << "box.relation_with(point(-B)) == " << rel2 << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::nothing();

  return rel1 == known_result && rel2 == known_result;
}

bool
test15() {
  Variable A(0);

  TBox box(2);
  box.add_constraint(A >= 0);

  Poly_Gen_Relation rel = box.relation_with(ray(-A));

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(ray(-A)) == " << rel << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::nothing();

  return rel == known_result;
}

bool
test16() {
  Variable A(0);

  TBox box(2);
  box.add_constraint(A >= 0);

  Poly_Gen_Relation rel = box.relation_with(line(A));

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(line(A)) == " << rel << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::nothing();

  return rel == known_result;
}

bool
test17() {
  Variable A(0);
  Variable B(1);

  TBox box(2);
  box.add_constraint(A == 0);
  box.add_constraint(B == 0);

  Poly_Gen_Relation rel = box.relation_with(closure_point(A));

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(closure_point(A)) == " << rel << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::nothing();

  return rel == known_result;
}

bool
test18() {
  Variable A(0);
  Variable B(1);

  TBox box(2);
  box.add_constraint(A >= 2);
  box.add_constraint(B == 0);

  Poly_Gen_Relation rel = box.relation_with(ray(A + B));

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(ray(A + B)) == " << rel << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::nothing();

  return rel == known_result;
}

bool
test19() {
  // The system of constraints of the box contains only
  // an equality and the generator `g' is a point.
  Variable A(0);

  TBox box(2);
  box.add_constraint(A == 0);

  Poly_Gen_Relation rel = box.relation_with(point(2*A));

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(point(2*A)) == " << rel << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::nothing();

  return rel == known_result;
}

bool
test20() {
  // The relation is on a variable (B) other than the first.
  Variable A(0);
  Variable B(1);

  TBox box(2);
  box.add_constraint(A >= 0);
  box.add_constraint(B >= 0);

  Poly_Con_Relation rel = box.relation_with(B > 0);

  print_constraints(box, "*** box ***");
  using namespace IO_Operators;
  nout << "box.relation_with(B > 0) == " << rel << endl;

  Poly_Con_Relation known_result = Poly_Con_Relation::strictly_intersects();

  return rel == known_result;
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
  DO_TEST(test03);
  DO_TEST(test04);
  DO_TEST(test05);
  DO_TEST(test06);
  DO_TEST(test07);
  DO_TEST(test08);
  DO_TEST(test09);
  DO_TEST(test10);
  DO_TEST(test11);
  DO_TEST(test12);
  DO_TEST(test13);
  DO_TEST(test14);
  DO_TEST(test15);
  DO_TEST(test16);
  DO_TEST(test17);
  DO_TEST(test18);
  DO_TEST(test19);
  DO_TEST(test20);
END_MAIN
