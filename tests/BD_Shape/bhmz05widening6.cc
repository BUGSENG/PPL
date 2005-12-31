/* Test BD_Shape::BHMZ05_widening_assign().
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

using namespace Parma_Polyhedra_Library::IO_Operators;

namespace {

Variable a(0);
Variable b(1);
Variable c(2);

TBD_Shape
n(int i) {
  TBD_Shape bd(3);
  if (i == 0) {
    bd.add_constraint(0 <= a-b);
    bd.add_constraint(     a-b <= 0);
    bd.add_constraint(-1 <= b-c);
    bd.add_constraint(      b-c <= 1);
  }
  else {
    bd.add_constraint(-i <= a-b);
    bd.add_constraint(      a-b <= i);
    bd.add_constraint(-1 <= b-c);
    bd.add_constraint(      b-c <= 1);
    bd.add_constraint(-i <= a-c);
    bd.add_constraint(      a-c <= i);
  }

  nout << "*** n_" << i << " ***" << endl
       << bd << endl;

  // Force closure.
  (void) (bd == bd);
  return bd;
}

} // namespace

int
main() TRY {
  unsigned i = 0;
  TBD_Shape m_i = n(i);
  TBD_Shape m_i_next;
  while (i < 100) {

    nout << "*** m_" << i << " ***" << endl
	 << m_i << endl;

    m_i_next = n(++i);
    m_i_next.bds_hull_assign(m_i);
    m_i_next.BHMZ05_widening_assign(m_i);
    // Force closure.
    (void) (m_i_next == m_i_next);
    if (m_i == m_i_next) {
      TBD_Shape known_result(3);
      known_result.add_constraint(-1 <= b-c);
      known_result.add_constraint(      b-c <= 1);
      int retval = (m_i == known_result) ? 0 : 1;

      nout << "*** m_" << i << " (fixpoint) ***" << endl
	   << m_i << endl;
      print_constraints(known_result, "*** known_result ***");

      return retval;
    }
    m_i = m_i_next;
  }

  return 1;
}
CATCH
