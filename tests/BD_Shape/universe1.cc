/* Different ways of creating an universe BD_Shape.
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
  Variable x(0);
  Variable y(1);
  // Variable z(2);

  TBD_Shape bd1(4);
  TBD_Shape bd2(4);

  bd1.add_constraint(-x <= 4);
  bd1.add_constraint(y - x <= 0);
  bd1.add_constraint(x - y <= -5);

  bool universe1 = bd1.is_universe();

  nout << "*** bd1.is_universe() ***" << endl;
  nout << (universe1 ? "true" : "false") << endl;

  bool universe2 = bd2.is_universe();

  nout << "*** bd2.is_universe() ***" << endl;
  nout << (universe2 ? "true" : "false") << endl;

  return (universe1 != universe2) ? 0 : 1;
}
CATCH
