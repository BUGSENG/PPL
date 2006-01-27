/* Test class Congruence.
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

class Test_Congruence : public Congruence {
public:
  Test_Congruence(Congruence cg) : Congruence(cg) {}
  Test_Congruence(Constraint c) : Congruence(c) {}
  void strong_normalize() { Congruence::strong_normalize(); }
  void normalize() { Congruence::normalize(); }
};

// Negative inhomogeneous term.

static void
test1() {
  nout << "test1:" << endl;

  Test_Congruence a((A + 2*B + 3*C %= 5) / 7);
  if (find_variation(a))
    exit(1);
  a.strong_normalize();

  Test_Congruence b((A %= 5 - 3*C - 2*B) / 7);
  if (find_variation(b))
    exit(1);
  b.strong_normalize();

  if (a == b)
    return;

  nout << "Test_Congruences a and b should be equal." << endl
       << "a:" << endl << a << endl
       << "b:" << endl << b << endl;
  exit(1);
}

// Positive inhomogeneous term.

static void
test2() {
  nout << "test2:" << endl;

  Test_Congruence a((A + 2*B + 3*C %= -5) / 7);
  if (find_variation(a))
    exit(1);
  a.strong_normalize();

  Test_Congruence b((A %= -5 - 3*C - 2*B) / 7);
  if (find_variation(b))
    exit(1);
  b.strong_normalize();

  if (a == b)
    return;

  nout << "Test_Congruences a and b should be equal." << endl
       << "a:" << endl << a << endl
       << "b:" << endl << b << endl;
  exit(1);
}

// Common factors and reducible positive inhomogeneous term.

static void
test3() {
  nout << "test3:" << endl;

  Test_Congruence a((16*A + 2*B + 8*C + 64 %= 0) / 4);
  if (find_variation(a))
    exit(1);
  a.strong_normalize();

  Test_Congruence b((16*A + 2*B %= - 64 - 8*C) / 4);
  b.strong_normalize();
  if (find_variation(b))
    exit(1);

  if (a == b)
    return;

  nout << "Test_Congruences a and b should be equal." << endl
       << "a:" << endl << a << endl
       << "b:" << endl << b << endl;
  exit(1);
}

// Negative first coefficient.

static void
test4() {
  nout << "test4:" << endl;

  Test_Congruence a((- A + 2*B + 3*C %= 5) / 7);
  if (find_variation(a))
    exit(1);
  a.strong_normalize();

  Test_Congruence b((- A %= - 2*B + 5 - 3*C) / 7);
  if (find_variation(b))
    exit(1);
  b.strong_normalize();

  if (a == b)
    return;

  nout << "Test_Congruences a and b should be equal." << endl
       << "a:" << endl << a << endl
       << "b:" << endl << b << endl;
  exit(1);
}

// Constructed with only the %= operator.

static void
test5() {
  nout << "test5:" << endl;

  Test_Congruence c(A + 4*B + 3*C %= 5);
  Test_Congruence a(c);
  //Test_Congruence a = (A + 4*B + 3*C %= 5);
  //Test_Congruence a(A + 4*B + 3*C %= 5);
  if (find_variation(a))
    exit(1);
  a.strong_normalize();

  Test_Congruence b(A + 4*B %= 5 - 3*C);
  if (find_variation(b))
    exit(1);
  b.strong_normalize();

  if (a == b)
    return;

  nout << "Test_Congruences a and b should be equal." << endl
       << "a:" << endl << a << endl
       << "b:" << endl << b << endl;
  exit(1);
}

// Equality congruence (a modulus of 0).

static void
test6() {
  nout << "test6:" << endl;

  Test_Congruence a((3*A + 24*B + 3*C %= -19) / 0);
  if (find_variation(a))
    exit(1);
  a.strong_normalize();

  Test_Congruence b((3*A + 24*B %= -19 - 3*C) / 0);
  if (find_variation(b))
    exit(1);
  b.strong_normalize();

  if (a == b)
    return;

  nout << "Test_Congruences a and b should be equal." << endl
       << "a:" << endl << a << endl
       << "b:" << endl << b << endl;
  exit(1);
}

// Constructed from a Constraint with the `/' operator.

static void
test7() {
  nout << "test7:" << endl;

  Test_Congruence a((A + 4*B + 3*C == 17) / 3);
  if (find_variation(a))
    exit(1);
  a.strong_normalize();

  Test_Congruence b((A + 4*B == 17 - 3*C) / 3);
  if (find_variation(b))
    exit(1);
  b.strong_normalize();

  if (a == b)
    return;

  nout << "Test_Congruences a and b should be equal." << endl
       << "a:" << endl << a << endl
       << "b:" << endl << b << endl;
  exit(1);
}

// Constructed from a Constraint.

static void
test8() {
  nout << "test8:" << endl;

  Test_Congruence a(A + 4*B + 3*C == 17);
  if (find_variation(a))
    exit(1);
  a.strong_normalize();

  Test_Congruence b(A + 4*B == 17 - 3*C);
  if (find_variation(b))
    exit(1);
  b.strong_normalize();

  if (a == b)
    return;

  nout << "Test_Congruences a and b should be equal." << endl
       << "a:" << endl << a << endl
       << "b:" << endl << b << endl;
  exit(1);
}

// Set modulus with `/='.

static void
test9() {
  nout << "test9:" << endl;

  Test_Congruence a(A + 4*B + 3*C == 17);
  a /= 3;
  if (find_variation(a))
    exit(1);
  a.strong_normalize();

  Test_Congruence b(A + 4*B == 17 - 3*C);
  b /= 3;
  if (find_variation(b))
    exit(1);
  b.strong_normalize();

  if (a == b)
    return;

  nout << "Test_Congruences a and b should be equal." << endl
       << "a:" << endl << a << endl
       << "b:" << endl << b << endl;
  exit(1);
}

// is_trivial_true and is_trivial_false.

static void
test10() {
  nout << "test10:" << endl;

  Test_Congruence a(0*A + 0*B + 0*C %= 17);
  if (find_variation(a))
    exit(1);
  if (!a.is_trivial_true()) {
    nout << "is_trivial_true(" << a << ") should have returned true." << endl;
    exit(1);
  }
  if (a.is_trivial_false()) {
    nout << "is_trivial_false(" << a << ") should have returned false." << endl;
    exit(1);
  }

  a = Test_Congruence((0*A + 0*B + 0*C %= 0) / 3);
  if (find_variation(a))
    exit(1);
  if (!a.is_trivial_true()) {
    nout << "is_trivial_true(" << a << ") should have returned true." << endl;
    exit(1);
  }
  if (a.is_trivial_false()) {
    nout << "is_trivial_false(" << a << ") should have returned false." << endl;
    exit(1);
  }

  a = Test_Congruence((0*A + 0*B + 8 %= 0) / 4);
  if (find_variation(a))
    exit(1);
  if (!a.is_trivial_true()) {
    nout << "is_trivial_true(" << a << ") should have returned true." << endl;
    exit(1);
  }
  if (a.is_trivial_false()) {
    nout << "is_trivial_false(" << a << ") should have returned false." << endl;
    exit(1);
  }

  a = Test_Congruence(0*A + 0*B %= 17);
  a /= 0;
  if (find_variation(a))
    exit(1);
  if (a.is_trivial_true()) {
    nout << "is_trivial_true(" << a << ") should have returned false." << endl;
    exit(1);
  }
  if (!a.is_trivial_false()) {
    nout << "is_trivial_false(" << a << ") should have returned true." << endl;
    exit(1);
  }

  a = Test_Congruence((0*A + 0*B + 3 %= 0) / 0);
  a.strong_normalize();
  if (find_variation(a))
    exit(1);
  if (a.is_trivial_true()) {
    nout << "is_trivial_true(" << a << ") should have returned false." << endl;
    exit(1);
  }
  if (!a.is_trivial_false()) {
    nout << "is_trivial_false(" << a << ") should have returned true." << endl;
    exit(1);
  }

  a = Test_Congruence((0*A + 0*B + 4 %= 0) / 3);
  a.strong_normalize();
  if (find_variation(a))
    exit(1);
  if (a.is_trivial_true()) {
    nout << "is_trivial_true(" << a << ") should have returned false." << endl;
    exit(1);
  }
  if (!a.is_trivial_false()) {
    nout << "is_trivial_false(" << a << ") should have returned true." << endl;
    exit(1);
  }
}

// Negative moduli.

static void
test11() {
  nout << "test11:" << endl;

  Test_Congruence a((A + 4*B + 3*C %= -4) / -3);
  a.strong_normalize();
  if (find_variation(a))
    exit(1);

  Test_Congruence b((A + 4*B %= -1 - 3*C) / -3);
  if (find_variation(b))
    exit(1);
  b.strong_normalize();

  if (a == b)
    return;

  nout << "Test_Congruences a and b should be equal." << endl
       << "a:" << endl << a << endl
       << "b:" << endl << b << endl;
  exit(1);
}

// Negative modulus and negative first coefficient.

static void
test12() {
  Variable x0(0);
  Variable x1(1);
  Variable x2(2);
  Variable x3(3);
  Variable x4(4);

  nout << "test12:" << endl;

  Test_Congruence a((-x0 + 4*x1 + 3*x2 + 17*x3 + 2*x4 %= -4) / -3);
  if (find_variation(a))
    exit(1);
  a.strong_normalize();

  Test_Congruence b((-x0 + 4*x1 %= - 3*x2 - 17*x3 - 2*x4 - 4) / -3);
  b.strong_normalize();
  if (find_variation(b))
    exit(1);

  if (a == b)
    return;

  nout << "Test_Congruences a and b should be equal." << endl
       << "a:" << endl << a << endl
       << "b:" << endl << b << endl;
  exit(1);
}

// Create from empty linear expression.

static void
test13() {
  nout << "test13:" << endl;

  Linear_Expression le;
  Test_Congruence a(le %= le);
  a.strong_normalize();
  if (find_variation(a))
    exit(1);

  Test_Congruence b(le %= 0);
  b.strong_normalize();
  if (find_variation(b))
    exit(1);

  if (a == b)
    return;

  nout << "Test_Congruences a and b should be equal." << endl
       << "a:" << endl << a << endl
       << "b:" << endl << b << endl;
  exit(1);
}

// Space dimension exception.

static void
test14() {
  nout << "test14:" << endl;

  Grid gr(2);

  try {
    gr.add_congruence(A + C %= 0);
    exit(1);
  }
  catch (std::invalid_argument) {}
}

} // namespace

int
main() TRY {
  set_handlers();

  nout << "congruence1:" << endl;

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

  return 0;
}
CATCH
