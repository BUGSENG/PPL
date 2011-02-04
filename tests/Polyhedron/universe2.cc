/* Test the function is_universe() for a NNC_polyhedron.
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
  NNC_Polyhedron ph1(4);
  NNC_Polyhedron ph2(3);
  NNC_Polyhedron ph3(3);

  ph2.add_constraint(Linear_Expression(1) > 0);
  ph3.add_constraint(Linear_Expression(1) < 0);

  Constraint_System cs;
  NNC_Polyhedron ph4(cs);

  print_constraints(ph1, "*** ph1 ***");
  print_constraints(ph2, "*** ph2 ***");
  print_constraints(ph3, "*** ph3 ***");
  print_constraints(ph4, "*** ph4 ***");

  bool universe1 = ph1.is_universe();

  nout << "*** ph1.is_universe() ***"
       << endl
       << (universe1 ? "true" : "false")
       << endl;

  bool universe2 = ph2.is_universe();

  nout << "*** ph2.is_universe() ***"
       << endl
       << (universe2 ? "true" : "false")
       << endl;

  bool universe3 = ph3.is_universe();

  nout << "*** ph3.is_universe() ***"
       << endl
       << (universe3 ? "true" : "false")
       << endl;

  bool universe4 = ph4.is_universe();

  nout << "*** ph4.is_universe() ***"
       << endl
       << (universe4 ? "true" : "false")
       << endl;

  return universe1 && universe2 && !universe3 && universe4;
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
END_MAIN
