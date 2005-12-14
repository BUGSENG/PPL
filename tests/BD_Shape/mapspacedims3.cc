/* Test BD_Shape::map_space_dimensions().
   Copyright (C) 2001-2005 Roberto Bagnara <bagnara@cs.unipr.it>

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
#include <string>
#include <ostream>

namespace {

void
print_function(const PFunction& function,
	       const std::string& intro = "",
	       std::ostream& s = nout) {
  if (!intro.empty())
    s << intro << endl;
  function.print(s);
}

} // namespace

int
main() TRY {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);

  Constraint_System cs;
  cs.insert(A >= 0);
  cs.insert(B >= 0);
  cs.insert(C >= 0);
  cs.insert(D == 0);
  cs.insert(B - A == 0);
  TBD_Shape bd(cs);

  PFunction function;
  function.insert(0, 2);
  function.insert(1, 1);
  function.insert(3, 0);

  print_function(function, "*** function ***");
  print_constraints(bd, "*** bd ***");

  bd.map_space_dimensions(function);

  TBD_Shape known_result(3);
  known_result.add_constraint(A == 0);
  known_result.add_constraint(B >= 0);
  known_result.add_constraint(C >= 0);
  known_result.add_constraint(B - C == 0);

  print_constraints(bd, "*** bd.map_space_dimensions(function) ***");

  return bd == known_result ? 0 : 1;
}
CATCH
