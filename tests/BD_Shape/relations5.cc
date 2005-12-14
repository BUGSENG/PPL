/* Test BD_Shape::relation_with(g).
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

using namespace Parma_Polyhedra_Library::IO_Operators;

namespace {

void test1() {
  // We verify that a generator is not subsumed by an empty BDS.
  Variable x(0);

  TBD_Shape bd(2, EMPTY);

  Generator g = point(x);
  Poly_Gen_Relation rel = bd.relation_with(g);

  print_constraints(bd, "--- bd ---");
  print_generator(g, "--- g ---");
  nout << "bd.relation_with(v(A)) == " << rel << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::nothing();

  if (rel != known_result)
    exit(1);
}

void test2() {
  // We verify that a zero-dimensional generator is subsumed 
  // by a zero-dimensional, universal BDS.
  TBD_Shape bd;

  Generator g = point();
  Poly_Gen_Relation rel = bd.relation_with(g);

  print_constraints(bd, "--- bd ---");
  print_generator(g, "--- g ---");
  nout << "bd.relation_with(v()) == " << rel << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::subsumes();
  
  if (rel != known_result)
    exit(1);
}

void test3() {
  // The system of constraints of the BDS contains only 
  // an equality and the generator `g' is a point.
  Variable A(0);

  TBD_Shape bd(2);
  bd.add_constraint(A == 0);

  Poly_Gen_Relation rel = bd.relation_with(point(2*A));

  print_constraints(bd, "--- bd ---");
  nout << "bd.relation_with(point(2*A)) == " << rel << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::nothing();

  if (rel != known_result) 
    exit(1);
}

void test4() {
  Variable A(0);
  Variable B(1);

  TBD_Shape bd(2);
  bd.add_constraint(A - B >= 0);
  bd.add_constraint(B >= 0);

  Poly_Gen_Relation rel1 = bd.relation_with(point(B));
  Poly_Gen_Relation rel2 = bd.relation_with(point(-B));

  print_constraints(bd, "*** bd ***");
  nout << "bd.relation_with(point(B)) == " << rel1 << endl;
  nout << "bd.relation_with(point(-B)) == " << rel2 << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::nothing();
 
  if (rel1 != known_result && rel2 != known_result) 
    exit(1);
}

void test5() {
  Variable A(0);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 0);

  Poly_Gen_Relation rel = bd.relation_with(ray(-A));

  print_constraints(bd, "*** bd ***");
  nout << "bd.relation_with(ray(-A)) == " << rel << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::nothing();
  
  if (rel != known_result) 
    exit(1);
}

void test6() {
  Variable A(0);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 0);

  Poly_Gen_Relation rel = bd.relation_with(line(A));

  print_constraints(bd, "*** bd ***");
  nout << "bd.relation_with(line(A)) == " << rel << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::nothing();
  
  if (rel != known_result) 
    exit(1);
}

void test7() {
  Variable A(0);
  Variable B(1);

  TBD_Shape bd(2);
  bd.add_constraint(A == 0);
  bd.add_constraint(B == 0);

  Poly_Gen_Relation rel = bd.relation_with(closure_point(A));

  print_constraints(bd, "*** bd ***");
  nout << "bd.relation_with(closure_point(A)) == " << rel << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::nothing();
  
  if (rel != known_result) 
    exit(1);
}

void test8() {
  Variable A(0);
  Variable B(1);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 2);
  bd.add_constraint(B == 0);

  Poly_Gen_Relation rel = bd.relation_with(ray(A + B));
  
  print_constraints(bd, "*** bd ***");
  nout << "bd.relation_with(ray(A + B)) == " << rel << endl;

  Poly_Gen_Relation known_result = Poly_Gen_Relation::nothing();
  
  if (rel != known_result) 
    exit(1);
}

} // namespace

int main() TRY {

  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();

  return 0;
}
CATCH
