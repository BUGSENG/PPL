/* Different ways of creating an empty BD_Shape.
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
  Variable x1(0);
  Variable x2(1);
  Variable x3(2);
  Variable x4(3);
  Variable x5(4);

  TBD_Shape bd(5);

  Coefficient a;
  if (std::numeric_limits<Coefficient>::is_bounded)
    a = -(std::numeric_limits<Coefficient>::min()/2) + 1;
  else
    a = 1300000000;

  bd.add_constraint(x1 - x2 <= -a);
  bd.add_constraint(x2 - x3 <= -a);
  bd.add_constraint(x3 - x4 <= a);
  bd.add_constraint(x4 - x5 <= a);
  bd.add_constraint(x5 - x1 <= a);

  print_constraints(bd, "*** bd ***");

  bool empty = bd.is_empty();

  nout << "*** bd.is_empty() ***" << endl;
  nout << (empty ? "true" : "false") << endl;

  return !empty ? 0 : 1;
}
CATCH
