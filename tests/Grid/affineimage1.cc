/* Test Grid::affine_image().
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

Variable A(0);
Variable B(1);
Variable C(2);
Variable D(3);

void
test1() {
  nout << "test1:" << endl;

  Grid gr(3);
  gr.add_congruence((C == -2) / 0);
  gr.add_congruence((A ==  0) / 0);

  gr.affine_image(B, A + 2, 1);

  if (find_variation(gr))
    exit(1);

  Grid known_gr(3, EMPTY);
  known_gr.add_generator(grid_point(2*B - 2*C));

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

void
test2() {
  nout << "test2:" << endl;

  Grid gr(3);
  gr.add_congruence((A - B %= 0) / 0);
  gr.add_congruence((A %= 0) / 3);

  gr.affine_image(A, A + B + 1);

  if (find_variation(gr))
    exit(1);

  Grid known_gr(3, EMPTY);
  known_gr.add_generator(grid_point(A));
  known_gr.add_generator(grid_point(7*A + 3*B));
  known_gr.add_generator(grid_line(C));

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Denominator.

void
test3() {
  nout << "test3:" << endl;

  Grid gr(3);
  gr.add_congruence((A %= 3) / 0);
  gr.add_congruence((B %= 2) / 0);

  gr.affine_image(A, A + 1, 2);

  if (find_variation(gr))
    exit(1);

  Grid known_gr(3, EMPTY);
  known_gr.add_generator(grid_point(2*A + 2*B));
  known_gr.add_generator(grid_line(C));

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Invertible transformation with denominator, modulus and up-to-date
// congruences.

void
test4() {
  nout << "test4:" << endl;

  Grid gr(3);
  gr.add_congruence((A %= 3) / 5);
  gr.add_congruence((B %= 2) / 0);

  gr.affine_image(A, A + 1, 3);

  if (find_variation(gr))
    exit(1);

  Grid known_gr(3, EMPTY);
  known_gr.add_generator(grid_point(4*A + 6*B, 3));
  known_gr.add_generator(grid_point(9*A + 6*B, 3));
  known_gr.add_generator(grid_line(C));

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Simple invertible transformation with denominator and modulus.

void
test5() {
  nout << "test5:" << endl;

  Grid gr(3, EMPTY);
  gr.add_generator(grid_point(3*A + 2*B));
  gr.add_generator(grid_point(8*A + 2*B));
  gr.add_generator(grid_line(C));

  gr.affine_image(A, A + 1, 3);

  Grid known_gr(3, EMPTY);
  known_gr.add_generator(grid_point(4*A + 6*B, 3));
  known_gr.add_generator(grid_point(9*A + 6*B, 3));
  known_gr.add_generator(grid_line(C));

  if (gr == known_gr) {

    // Up-to-date, minimized congruences.

    gr = Grid(3);
    gr.add_congruence((A %= 3) / 5);
    gr.add_congruence((B %= 2) / 0);

    if (find_variation(gr))
      exit(1);

    gr.affine_image(A, A + 1, 3);

    if (gr == known_gr)
      return;

    nout << "Congruence";
  }
  else
    nout << "Generator";

  nout << " grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Invertible transformation which changes the modulus.

void
test6() {
  nout << "test6:" << endl;

  Grid gr(2, EMPTY);
  gr.add_generator(grid_point());
  gr.add_generator(grid_point(A));
  gr.add_generator(grid_point(B));

  // Out-of-date congruences.

  gr.affine_image(A, 3*A + 2*B + 4);

  Grid known_gr(2, EMPTY);
  known_gr.add_generator(grid_point(4*A));
  known_gr.add_generator(grid_point(7*A));
  known_gr.add_generator(grid_point(6*A + B));

  if (gr == known_gr) {

    // Up-to-date congruences.

    gr = Grid(2);
    gr.add_congruence(A %= 0);
    gr.add_congruence(B %= 0);

    gr.affine_image(A, 3*A + 2*B + 4);

    if (gr == known_gr)
      return;
  }

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// One dimension.

void
test7() {
  nout << "test7:" << endl;

  Grid gr(1, EMPTY);
  gr.add_generator(grid_point());
  gr.add_generator(grid_point(A));

  // Out-of-date congruences.

  gr.affine_image(A, 2*A);

  Grid known_gr(1);
  known_gr.add_congruence((A %= 0) / 2);

  if (gr == known_gr) {

    // Up-to-date congruences.

    gr = Grid(1);
    gr.add_congruence(A %= 0);

    gr.affine_image(A, 2*A);

    if (gr == known_gr)
      return;
  }

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// The first example described at anchor grid_affine_transformation in
// definitions.dox.

void
test8() {
  nout << "test8:" << endl;

  Grid gr(2, EMPTY);
  gr.add_generator(grid_point());
  gr.add_generator(grid_point(3*A));
  gr.add_generator(grid_point(3*B));

  // Out-of-date congruences.

  gr.affine_image(A, 3*A + 2*B + 1);

  Grid known_gr(2, EMPTY);
  known_gr.add_generator(grid_point(A));
  known_gr.add_generator(grid_point(7*A + 3*B));
  known_gr.add_generator(grid_point(10*A));

  if (gr == known_gr) {

    // Up-to-date, minimized congruences.

    gr = Grid(2);
    gr.add_congruence((A %= 0) / 3);
    gr.add_congruence((B %= 0) / 3);

    // find_variation will minimize the congruences.
    if (find_variation(gr))
      exit(1);

    gr.affine_image(A, 3*A + 2*B + 1);

    if (gr == known_gr)
      return;
  }

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// The second example described at anchor grid_affine_transformation
// in definitions.dox.

void
test9() {
  nout << "test9:" << endl;

  Grid gr(2, EMPTY);
  gr.add_generator(grid_point());
  gr.add_generator(grid_point(3*A));
  gr.add_generator(grid_point(3*B));

  // Out-of-date congruences.

  gr.affine_image(A, B);

  Grid known_gr(2, EMPTY);
  known_gr.add_generator(grid_point());
  known_gr.add_generator(grid_point(3*A + 3*B));

  if (gr == known_gr) {

    // Up-to-date congruences.

    gr = Grid(2);
    gr.add_congruence((A %= 0) / 3);
    gr.add_congruence((B %= 0) / 3);

    gr.affine_image(A, B);

    if (gr == known_gr)
      return;
  }

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Denominator, with generators having a variety of divisors.

void
test10() {
  nout << "test10:" << endl;

  Grid_Generator_System gs;
  gs.insert(grid_point());
  gs.insert(grid_point(A, 3));
  gs.insert(grid_point(B, 2));

  // The divisors are normalized on construction.
  Grid gr(gs);

  // All divisors should change, even when the coefficient of A is 0.
  gr.affine_image(A, 2*A, 5);

  Grid known_gr(2, EMPTY);
  known_gr.add_generator(grid_point());
  known_gr.add_generator(grid_point(2*A, 15));
  known_gr.add_generator(grid_point(5*B, 10));

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Negative denominator.

void
test11() {
  nout << "test11:" << endl;

  Grid gr(2, EMPTY);
  gr.add_generator(grid_point(A));

  gr.affine_image(A, B + 2, -3);

  Grid known_gr(2, EMPTY);
  known_gr.add_generator(grid_point(-2*A, 3));

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Empty grid.

void
test12() {
  nout << "test12:" << endl;

  Grid gr(2, EMPTY);

  gr.affine_image(A, 2*A + B + 1);

  Grid known_gr(2, EMPTY);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Shift a rectilinear pointed grid along A.

void
test13() {
  nout << "test13:" << endl;

  Grid gr(3);
  gr.add_congruence((A %= 0) / 4);
  gr.add_congruence((B %= 0) / 2);

  gr.affine_image(A, A + 3);

  if (find_variation(gr))
    exit(1);

  Grid known_gr(3);
  known_gr.add_congruence((A %= 3) / 4);
  known_gr.add_congruence((B %= 0) / 2);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Slant a rectilinear pointed grid along A == B.

void
test14() {
  nout << "test14:" << endl;

  Grid gr(2);
  gr.add_congruence((A %= 0) / 4);
  gr.add_congruence((B %= 0) / 2);

  gr.affine_image(A, A + B);

  if (find_variation(gr))
    exit(1);

  Grid known_gr(2);
  known_gr.add_congruence((A - B %= 0) / 4);
  known_gr.add_congruence((A %= 0) / 2);
  known_gr.add_congruence((B %= 0) / 2);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Compress a rectilinear pointed grid to a line of points.

void
test15() {
  nout << "test15:" << endl;

  Grid gr(2);
  gr.add_congruence((A %= 0) / 4);
  gr.add_congruence((B %= 0) / 2);

  gr.affine_image(A, B);

  if (find_variation(gr))
    exit(1);

  Grid known_gr(2);
  known_gr.add_congruence(A - B == 0);
  known_gr.add_congruence((A %= 0) / 2);

  if (gr == known_gr)
    return;

  nout << "Grid should equal known grid." << endl
       << " grid:" << endl << gr << endl
       << "known:" << endl << known_gr << endl;

  exit(1);
}

// Zero denominator.

void
test16() {
  nout << "test16:" << endl;

  Grid gr(3);
  gr.add_congruence((C == -2) / 0);
  gr.add_congruence((A ==  0) / 0);

  try {
    gr.affine_image(B, A + 2, 0);
    exit(1);
  }
  catch (const std::invalid_argument& e) {}
}

// Expression of a greater space dimension than the grid.

void
test17() {
  nout << "test17:" << endl;

  Grid gr(3);
  gr.add_congruence((C == -2) / 0);
  gr.add_congruence((A ==  0) / 0);

  try {
    gr.affine_image(B, D + 2);
    exit(1);
  }
  catch (const std::invalid_argument& e) {}
}

// Variable of a greater space dimension than the grid.

void
test18() {
  nout << "test18:" << endl;

  Grid gr(3);
  gr.add_congruence((C == -2) / 0);
  gr.add_congruence((A ==  0) / 0);

  try {
    gr.affine_image(D, A + 2);
    exit(1);
  }
  catch (const std::invalid_argument& e) {}
}

} // namespace

int
main() TRY {
  set_handlers();

  nout << "affineimage1:" << endl;

  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();
  test10();
  test11();
  test12();
  test13();
  test14();
  test15();
  test16();
  test17();
  test18();

  return 0;
}
CATCH
