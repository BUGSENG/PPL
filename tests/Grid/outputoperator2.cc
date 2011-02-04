/* Test Grid_Generator IO operators.
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
#include <sstream>

using namespace Parma_Polyhedra_Library::IO_Operators;

namespace {

// Point.
bool
test01() {
  std::stringstream ss1;
  ss1 << grid_point();

  std::stringstream ss2;
  ss2 << Grid_Generator::zero_dim_point();

  bool ok = (!ss2.str().compare(ss1.str()));

  return ok;
}

// Point type.
bool
test02() {
  std::stringstream ss1;
  ss1 << grid_point().type();

  std::stringstream ss2;
  ss2 << Grid_Generator::zero_dim_point().type();

  bool ok = (!ss2.str().compare(ss1.str()));

  return ok;
}

// Parameter type.
bool
test03() {
  Variable A(0);
  Variable B(1);

  std::stringstream ss1;
  ss1 << parameter(B).type();

  std::stringstream ss2;
  ss2 << parameter(A).type();

  bool ok = (!ss2.str().compare(ss1.str()));

  return ok;
}

// Line type.
bool
test04() {
  Variable A(0);
  Variable B(1);

  std::stringstream ss1;
  ss1 << grid_line(2*A).type();

  std::stringstream ss2;
  ss2 << grid_line(3*A).type();

  bool ok = (!ss2.str().compare(ss1.str()));

  return ok;
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
  DO_TEST(test03);
  DO_TEST(test04);
END_MAIN
