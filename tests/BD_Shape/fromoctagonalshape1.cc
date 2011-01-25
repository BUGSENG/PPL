/* Test BD_Shape::BD_Shape(const Octagonal_Shape&).
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2011 BUGSENG srl (http://bugseng.com)

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
  Variable A(0);
  Variable B(1);

  Constraint_System cs;
  cs.insert(A >= 0);
  cs.insert(B <= 7);
  cs.insert(A - B <= 18);
  TOctagonal_Shape os(cs);
  os.add_constraint(A + B <= 5);
  os.add_constraint(A + B >= -10);

  TBD_Shape bds(os);

  BD_Shape<mpq_class> known_result(cs);
  known_result.add_constraint(2*A <= 23);
  known_result.add_constraint(B >= -14);
  known_result.add_constraint(B <= 5);

  bool ok = (check_result(bds, known_result));

  print_constraints(bds, "*** bds ***");

  return ok;
}

bool
test02() {
  Variable A(0);
  Variable B(1);

  Constraint_System cs;
  cs.insert(A + B <= 5);
  cs.insert(A + B >= 10);
  TOctagonal_Shape os(cs);

  TBD_Shape bds(os);

  BD_Shape<mpq_class> known_result(os.space_dimension(), EMPTY);

  bool ok = (check_result(bds, known_result));

  print_constraints(bds, "*** bds ***");

  return ok;
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
END_MAIN
