/* Test the MIP_Problem class.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2022 BUGSENG srl (http://bugseng.com)

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
site: http://bugseng.com/products/ppl/ . */

#include "ppl_test.hh"

namespace {

bool
test01() {
  MIP_Problem mip(0);
  mip.is_satisfiable();
  mip.set_optimization_mode(MINIMIZATION);
  mip.solve();
  return true;
}

bool
test02() {
  Variable A(0);
  Variable B(1);
  MIP_Problem mip(1);
  Generator p1 = mip.optimizing_point();
  bool ok1 = (p1 == point(0*A));
  mip.add_space_dimensions_and_embed(1);
  Generator p2 = mip.optimizing_point();
  bool ok2 = (p2 == point(0*A + 0*B));
  return ok1 && ok2;
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
END_MAIN
