/* Test Polyhedron::is_disjoint_from(const Polyhedron& y).
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

NNC_Polyhedron
half_strip(const Generator& p,
	   const Linear_Expression& e,
	   bool closed = true) {
  assert((p.is_point() && closed) || (p.is_closure_point() && ! closed));

  Linear_Expression e1(p);
  e1 += 3*Variable(0);

  Generator_System gs;
  gs.insert(p);
  gs.insert(ray(e));
  if (closed)
    gs.insert(point(e1, p.divisor()));
  else {
    gs.insert(closure_point(e1, p.divisor()));
    e1 -= Variable(0);
    e1 += e.coefficient(Variable(1)) * p.divisor() * Variable(1);
    gs.insert(point(e1));
  }
  NNC_Polyhedron ph(gs);
  return ph;
}

void
test1() {
  Variable A(0);
  Variable B(1);

  NNC_Polyhedron ph1 = half_strip(point(A + B), B);

  NNC_Polyhedron ph2(2, EMPTY);
  ph2.add_generator(point(3*A + B));
  ph2.add_generator(closure_point(2*A + B));
  ph2.add_generator(closure_point(4*A + 3*B));
  ph2.add_generator(ray(A - B));

  bool disjoint = ph1.is_disjoint_from(ph2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  if (disjoint)
    exit(1);
}

void
test2() {
  Variable A(0);
  Variable B(1);

  NNC_Polyhedron ph1 = half_strip(point(A + B), B);
  NNC_Polyhedron ph2 = half_strip(closure_point(4*A + B), B, false);

  bool disjoint = ph1.is_disjoint_from(ph2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  if (!disjoint)
    exit(1);
}

void
test3() {
  Variable A(0);
  Variable B(1);

  NNC_Polyhedron ph1 = half_strip(point(A + B), B);
  NNC_Polyhedron ph2 = half_strip(closure_point(A + B), -B, false);

  bool disjoint = ph1.is_disjoint_from(ph2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  if (!disjoint)
    exit(1);
}

void
test4() {
  Variable A(0);
  Variable B(1);

  NNC_Polyhedron ph1 = half_strip(point(), B);

  NNC_Polyhedron ph2(2, EMPTY);
  ph2.add_generator(point(-2*A - 2*B));
  ph2.add_generator(closure_point(2*A - 2*B));
  ph2.add_generator(closure_point(-2*A + 2*B));
  ph2.add_generator(ray(-A - B));

  bool disjoint = ph1.is_disjoint_from(ph2);

  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");

  if (!disjoint)
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

  return 0;
}
CATCH
