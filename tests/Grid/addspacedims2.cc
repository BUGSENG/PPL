/* Test Grid::add_space_dimensions_and_project().
   Copyright (C) 2005 Roberto Bagnara <bagnara@cs.unipr.it>

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

// From congruences.

void
test1() {
  nout << "test1:" << endl;

  Variable A(0);
  Variable C(2);
  Variable D(3);
  Variable E(4);

  Congruence_System cgs;
  cgs.insert((A + 0*C %= 0) / 2);

  Grid gr(cgs);

  if (find_variation(gr))
    exit(1);

  gr.add_space_dimensions_and_project(2);

  if (find_variation(gr))
    exit(1);

  Congruence_System known_cgs;
  known_cgs.insert((E %= 0) / 0);
  known_cgs.insert((D %= 0) / 0);
  known_cgs.insert((A %= 0) / 2);

  Grid known_gr(known_cgs);

  if (find_variation(known_gr))
    exit(1);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Empty grid.

void
test2() {
  nout << "test2:" << endl;

  Grid gr(2, EMPTY);

  if (find_variation(gr))
    exit(1);

  gr.add_space_dimensions_and_project(3);

  if (find_variation(gr))
    exit(1);

  Grid known_gr(5, EMPTY);

  if (find_variation(known_gr))
    exit(1);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Universe grid, compared to congruences.

void
test3() {
  nout << "test3:" << endl;

  Grid gr(1);

  if (find_variation(gr))
    exit(1);

  gr.add_space_dimensions_and_project(4);

  if (find_variation(gr))
    exit(1);

  Variable B(1);
  Variable C(2);
  Variable D(3);
  Variable E(4);

  Congruence_System known_cgs;
  known_cgs.insert((B %= 0) / 0);
  known_cgs.insert((C %= 0) / 0);
  known_cgs.insert((D %= 0) / 0);
  known_cgs.insert((E %= 0) / 0);

  Grid known_gr(known_cgs);

  if (find_variation(known_gr))
    exit(1);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Universe grid, compared to generators.

void
test4() {
  nout << "test4:" << endl;

  Grid gr(1);

  if (find_variation(gr))
    exit(1);

  gr.add_space_dimensions_and_project(3);

  Variable A(0);
  Variable D(3);

  Grid_Generator_System known_gs;
  known_gs.insert(grid_point());
  known_gs.insert(grid_line(A + 0*D));

  Grid known_gr(known_gs);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  nout << "known_gr.ascii_dump(nout):" << std::endl;
  known_gr.ascii_dump(nout);
  nout << "gr.ascii_dump(nout):" << std::endl;
  gr.ascii_dump(nout);

  exit(1);
}

// From generators.

void
test5() {
  nout << "test5:" << endl;

  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);
  Variable E(4);

  Grid_Generator_System gs;
  gs.insert(grid_point(A));
  gs.insert(grid_point(A + B + C));

  Grid gr(gs);

  if (find_variation(gr))
    exit(1);

  gr.add_space_dimensions_and_project(2);

  if (find_variation(gr))
    exit(1);

  Congruence_System known_cgs;
  known_cgs.insert(A == 1);
  known_cgs.insert(B - C == 0);
  known_cgs.insert(B %= 0);
  known_cgs.insert(D == 0);
  known_cgs.insert(E == 0);

  Grid known_gr(known_cgs);

  if (find_variation(known_gr))
    exit(1);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// From generators, where dimensions are only added to the grid's
// generator system.

void
test6() {
  nout << "test6:" << endl;

  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);
  Variable E(4);

  Grid_Generator_System gs;
  gs.insert(grid_point(A));
  gs.insert(grid_point(A + B + C));

  Grid gr(gs);

  // Add space dimensions directly after creating the grid, to ensure
  // that only the generators are up to date.

  gr.add_space_dimensions_and_project(2);

  if (find_variation(gr))
    exit(1);

  Congruence_System known_cgs;
  known_cgs.insert((A == 1) / 0);
  known_cgs.insert((B - C %= 0) / 0);
  known_cgs.insert((B %= 0) / 1);
  known_cgs.insert((D %= 0) / 0);
  known_cgs.insert((E %= 0) / 0);

  Grid known_gr(known_cgs);

  if (find_variation(known_gr))
    exit(1);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

int
main() TRY {
  set_handlers();

  nout << "addspacedims2:" << endl;

  test1();
  test2();
  test3();
  test4();
  test5();
  test6();

  return 0;
}
CATCH
