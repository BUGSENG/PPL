/* Test BD_Shape<T>::minimized_constraints().
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

using namespace std;
using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::IO_Operators;

#ifndef NOISY
#define NOISY 0
#endif

int
main() TRY {
  set_handlers();

  Variable A(0);
  Variable B(1);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 1);
  bd.add_constraint(B >= 0);
  bd.add_constraint(A - B >= -2);
  bd.add_constraint(A >= -3);
  bd.add_constraint(A <= 1);

#if NOISY
  print_constraints(bd, "*** bd ***");
#endif

  const Constraint_System cs = bd.minimized_constraints();

#if NOISY
  std::cout << "*** bd.minimized_constraints() ***" << std::endl;
#endif
  dimension_type num_constraints = 0;
  for (Constraint_System::const_iterator i = cs.begin(),
	 iend = cs.end(); i != iend; ++i) {
#if NOISY
    std::cout << *i << std::endl;
#endif
    ++num_constraints;
  }
#if NOISY
  std::cout << "num_constraints == " << num_constraints << std::endl;
#endif

  C_Polyhedron ph_bd(cs);
  C_Polyhedron known_result(2);
  known_result.add_constraint(A == 1);
  known_result.add_constraint(B >= 0);
  known_result.add_constraint(B <= 3);

  int retval = (num_constraints == 3 && known_result == ph_bd) ? 0: 1;

  return retval;
}
CATCH
