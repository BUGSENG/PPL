/* Test Pointset_Powerset<PH>::omega_reduce().
        Pointset_Powerset<PH>::pairwise_reduce().
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

// Powerset of polyhedra: omega_reduce().
bool
test01() {
  Variable x(0);
  Pointset_Powerset<C_Polyhedron> c_ps(1, EMPTY);
  Constraint_System cs;

  cs.insert(x >= 0);
  cs.insert(x <= 2);
  c_ps.add_disjunct(C_Polyhedron(cs));

  cs.clear();
  cs.insert(x >= 0);
  cs.insert(x <= 3);
  c_ps.add_disjunct(C_Polyhedron(cs));
  c_ps.omega_reduce();

  bool ok = (c_ps.size() == 1);

  return ok;
}

// Powerset of boxes: omega_reduce().
bool
test02() {
  Variable x(0);
  Pointset_Powerset<TBox> pps_box(1, EMPTY);
  Constraint_System cs;

  cs.insert(x >= 0);
  cs.insert(x <= 2);
  pps_box.add_disjunct(TBox(cs));

  cs.clear();
  cs.insert(x >= 0);
  cs.insert(x <= 3);
  pps_box.add_disjunct(TBox(cs));
  pps_box.omega_reduce();

  bool ok = (pps_box.size() == 1);

  return ok;
}

/* test07() in difference1.cc includes a use of pairwise_reduce
   for C polyhedra. */

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
END_MAIN
