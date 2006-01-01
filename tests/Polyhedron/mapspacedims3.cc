/* Test Polyhedron::map_space_dimensions().
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
#include "PFunction.hh"

namespace {

void
print_function(const PFunction& function, const std::string& intro = "",
	       std::ostream& s = nout) {
  if (!intro.empty())
    s << intro << endl;
  function.print(s);
}

} // namespace

int
main() TRY {
  set_handlers();

  Variable A(0);
  Variable B(1);
  Variable C(2);

  C_Polyhedron ph(3);
  ph.add_constraint(A >= 2);
  ph.add_constraint(B >= 1);
  ph.add_constraint(C >= 0);

  PFunction rotate_right;
  rotate_right.insert(0, 1);
  rotate_right.insert(1, 2);
  rotate_right.insert(2, 0);

  print_constraints(ph, "*** ph ***");
  print_function(rotate_right, "*** rotate_right ***");

  ph.map_space_dimensions(rotate_right);

  C_Polyhedron known_result(3);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(B >= 2);
  known_result.add_constraint(C >= 1);

  bool ok = (ph == known_result);

  print_constraints(ph, "*** After ph.map_space_dimensions(rotate_right) ***");

  if (!ok)
    exit(1);

  return 0;
}
CATCH
