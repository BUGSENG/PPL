/* Test Pointset_Powerset<PH>::is_discrete().
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

// Powerset of C polyhedra: is_discrete().
bool
test01() {
  Pointset_Powerset<C_Polyhedron> ps(0, EMPTY);
  bool b = ps.is_discrete();

  ps.add_disjunct(C_Polyhedron(0));
  // A zero-dimension universe is discrete.
  bool b1 = ps.is_discrete();
  return b && b1;
}

// Powerset of NNC polyhedra: is_discrete().
bool
test02() {
  Variable x(0);
  Constraint_System cs;
  Pointset_Powerset<NNC_Polyhedron> ps(1, EMPTY);

  cs.clear();
  cs.insert(x == 2);
  ps.add_disjunct(NNC_Polyhedron(cs));

  bool b = ps.is_discrete();

  cs.clear();
  cs.insert(x > 0);
  cs.insert(x <= 1);
  ps.add_disjunct(NNC_Polyhedron(cs));

  bool b1 = !ps.is_discrete();
  return b && b1;
}

// Powerset of C polyhedra: is_discrete().
bool
test03() {
  Pointset_Powerset<C_Polyhedron> ps(1, EMPTY);
  bool b = ps.is_discrete();

  ps.add_disjunct(C_Polyhedron(1));

  bool b1 = !ps.is_discrete();
  return b && b1;
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
  DO_TEST(test03);
END_MAIN
