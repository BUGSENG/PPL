/* Test the *::max_space_dimension() methods.
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

template <typename T>
dimension_type foo() {
  return T::max_space_dimension();
}

template dimension_type foo<Variable>();
template dimension_type foo<Linear_Expression>();
template dimension_type foo<Constraint>();
template dimension_type foo<Generator>();
template dimension_type foo<Constraint_System>();
template dimension_type foo<Generator_System>();
template dimension_type foo<C_Polyhedron>();
template dimension_type foo<NNC_Polyhedron>();
template dimension_type foo<Polyhedra_Powerset<C_Polyhedron> >();
template dimension_type foo<Polyhedra_Powerset<NNC_Polyhedron> >();

#define PRINT(T) \
nout << #T "::max_space_dimension() = " << T::max_space_dimension() << endl

int
main() TRY {
  set_handlers();

  PRINT(Variable);
  PRINT(Linear_Expression);
  PRINT(Constraint);
  PRINT(Generator);
  PRINT(Constraint_System);
  PRINT(Generator_System);
  PRINT(C_Polyhedron);
  PRINT(NNC_Polyhedron);
  PRINT(Polyhedra_Powerset<C_Polyhedron>);
  PRINT(Polyhedra_Powerset<NNC_Polyhedron>);

  // FIXME.
  return 0;
}
CATCH
