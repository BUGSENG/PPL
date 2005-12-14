/* Test BD_Shape::generalized_affine_image().
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

int
main() TRY {
  Variable x(0);
  Variable y(1);

  TBD_Shape bd(3);

  bd.add_constraint(x >= 2);
  bd.add_constraint(x - y <= 3);
  bd.add_constraint(y <= 2);

  print_constraints(bd, "*** bd ***");

  TBD_Shape known_result(3);
  known_result.add_constraint(x >= 1);
  known_result.add_constraint(y <= 2);
  known_result.add_constraint(- y <= 1);
  known_result.add_constraint(x - y >= -1);

  bd.generalized_affine_image(x,GREATER_THAN_OR_EQUAL, 2*x - 2, 2);

  print_constraints(bd, "*** bd.generalized_affine_image(x, "
                        "GREATER_THAN_OR_EQUAL, 2*x - 2, 2) ***");

  int retval = (bd == known_result) ? 0 : 1;

  return retval;
}
CATCH
