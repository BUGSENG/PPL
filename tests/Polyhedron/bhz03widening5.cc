/* Test Polyhedra_Powerset<PH>::BHZ03_widening_assign().
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
#include <vector>

namespace {

Variable A(0);
Variable B(1);

typedef Polyhedra_Powerset<C_Polyhedron> PSet;

PSet
S(unsigned n) {
  PSet s(2, EMPTY);
  n += 3;
  if (n % 2 == 0) {
    C_Polyhedron p(2);
    p.add_constraint(A >= 1);
    p.add_constraint(A <= n);
    p.add_constraint(B == 1);
    s.add_disjunct(p);
  }
  else {
    C_Polyhedron p(2);
    C_Polyhedron q(2);
    C_Polyhedron r(2);
    p.add_constraint(A >= 1);
    p.add_constraint(B == 0);
    q.add_constraint(A >= 2);
    q.add_constraint(A <= n);
    q.add_constraint(B == 1);
    r.add_constraint(A >= 1);
    r.add_constraint(A <= n-1);
    r.add_constraint(B == 1);
    s.add_disjunct(p);
    s.add_disjunct(q);
    s.add_disjunct(r);
  }
  return s;
}

} // namespace

int
main() TRY {
  set_handlers();

  C_Polyhedron p(2);
  C_Polyhedron q(2);
  C_Polyhedron r(2);
  C_Polyhedron s(2);
  p.add_constraint(A >= 1);
  p.add_constraint(B == 0);
  q.add_constraint(A >= 2);
  q.add_constraint(A <= 7);
  q.add_constraint(B == 1);
  r.add_constraint(A >= 3);
  r.add_constraint(A <= 8);
  r.add_constraint(B == 1);
  s.add_constraint(A >= 1);
  s.add_constraint(A <= 6);
  s.add_constraint(B == 1);
  PSet P(2, EMPTY);
  P.add_disjunct(p);
  P.add_disjunct(q);
  P.add_disjunct(r);
  P.add_disjunct(s);
  PSet Q(2, EMPTY);
  Q.add_disjunct(p);
  Q.add_disjunct(q);
  Q.add_disjunct(s);

  using namespace Parma_Polyhedra_Library::IO_Operators;

  nout << "P = " << P << endl
       << "Q = " << Q << endl;

  PSet old_P = P;
  P.BHZ03_widening_assign<H79_Certificate>
    (Q, widen_fun_ref(&Polyhedron::H79_widening_assign));

  nout << "P.BHZ03(Q, H79)" << " = " << P << endl;

  return (P.geometrically_covers(old_P) && P.geometrically_covers(Q)) ? 0 : 1;
}
CATCH
