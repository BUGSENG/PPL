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
  Variable A(0);
  Variable B(1);
  Linear_Expression e1(B - 3);
  Linear_Expression e2(B + 1);

  TBD_Shape bd(2);
  bd.add_constraint(A >= 0);
  bd.add_constraint(A <= 4);
  bd.add_constraint(B <= 5);
  
  print_constraints(bd, "*** bd ***");

  TBD_Shape known_result(2);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(A <= 4); 
  known_result.add_constraint(B <= 9);
  
  bd.generalized_affine_image(e1, EQUAL, e2);

  print_constraints(bd, "*** bd.generalized_affine_image(B-3,EQUAL, B+1) ***");

  int retval = (bd == known_result) ? 0 : 1;

  return retval;
}
CATCH
