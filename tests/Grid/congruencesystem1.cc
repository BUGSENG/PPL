/* Test the straightforward member functions of Congruence_System.
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

class Test_Congruence_System : public Congruence_System {
public:
  Test_Congruence_System() : Congruence_System() {}
  Test_Congruence_System(Congruence_System cgs) : Congruence_System(cgs) {}
  Test_Congruence_System(Congruence cg) : Congruence_System(cg) {}
};

// Public constructors.

void
test1() {
  Test_Congruence_System cgs0;
  if (find_variation(cgs0))
    exit(1);

  Test_Congruence_System cgs1(cgs0);
  if (find_variation(cgs1))
    exit(1);

  if (cgs1 == cgs0) {
    Variable A(0);
    Variable B(1);

    Test_Congruence_System cgs2((A - 3*B %= 5) / 2);
    if (find_variation(cgs2))
      exit(1);

    if (cgs2 == cgs0) {
      nout << "`cgs2 == cgs0' should fail." << endl;
      exit(1);
    }

    if (cgs2 == cgs1) {
      nout << "`cgs2 == cgs1' should fail." << endl;
      exit(1);
    }

    return;
  }

  nout << "`cgs1 == cgs0' failed." << endl;
  exit(1);
}

// operator=

void
test2() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Test_Congruence_System cgs0;
  if (find_variation(cgs0))
    exit(1);

  Test_Congruence_System cgs1((A - 3*B + 21*C %= 5) / 2);
  if (find_variation(cgs1))
    exit(1);

  cgs0 = cgs1;
  if (find_variation(cgs0))
    exit(1);

  if (cgs0 == cgs1)
    return;

  nout << "`cgs0 == cgs1' should fail." << endl;
  exit(1);
}

// space_dimension

void
test3() {
  Variable A(3);
  Variable B(7);
  Variable C(4);

  Test_Congruence_System cgs0((A - 3*B + 21*C %= 55) / 21);
  if (find_variation(cgs0))
    exit(1);

#define SPACE_DIM 8

  if (cgs0.space_dimension() == SPACE_DIM)
    return;

  nout << "Space dimension of cgs0 should have been " PPL_TEST_XSTR(SPACE_DIM) "."
       << endl;
  exit(1);
}

// clear

void
test4() {
  Variable A(0);
  Variable B(1);

  Test_Congruence_System cgs0((A - 3*B %= 5) / 7);
  if (find_variation(cgs0))
    exit(1);

  cgs0.clear();
  if (find_variation(cgs0))
    exit(1);

  if (cgs0.space_dimension() == 0)
    return;

  nout << "Space dimension of cgs0 ("
       << cgs0.space_dimension()
       << ") should have been 0."
       << endl;

  exit(1);
}

// insert

void
test5() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);

  Test_Congruence_System cgs0((A - 3*B + C - 18*D %= 5) / 7);
  if (find_variation(cgs0))
    exit(1);

  cgs0.insert((A %= 0) / 3);
  cgs0.insert((A + B %= 3) / 5);
  if (find_variation(cgs0))
    exit(1);

  return;
}

// num_[non_]equalities

void
test6() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Test_Congruence_System cgs0((-A - B + 13*C %= 6) / 7);

  if (find_variation(cgs0))
    exit(1);

  cgs0.insert((A %= 0) / 0);
  cgs0.insert((A + B %= 3) / 0);
  if (find_variation(cgs0))
    exit(1);

#define NUM_EQS 2

  dimension_type tem = cgs0.num_equalities();
  if (tem != NUM_EQS) {
    nout << "cgs0 should have had " PPL_TEST_XSTR(NUM_EQS)
	 << " equalities (instead of "
	 << tem << ")." << endl;
    exit(1);
  }

#define NUM_PRPR_CGS 1

  tem = cgs0.num_proper_congruences();
  if (tem == NUM_PRPR_CGS)
    return;

  nout << "cgs0 should have had " PPL_TEST_XSTR(NUM_PRPR_CGS)
       << " \"non-equalities\" (instead of "
       << tem << ")." << endl;

  exit(1);
}

// insert, including a row with all terms zero and an equality.

void
test7() {
  Variable A(0);
  Variable B(1);
  Variable C(2);

  Test_Congruence_System cgs0(0*A + 0*B %= -1);
  if (find_variation(cgs0))
    exit(1);

  cgs0.insert((A + 0*B %= 1) / 0);
  cgs0.insert(0*A + 1*B %= 1);

  if (find_variation(cgs0))
    exit(1);

  return;
}

int
main() TRY {
  set_handlers();

  DO_TEST(test1);
  DO_TEST(test2);
  DO_TEST(test3);
  DO_TEST(test4);
  DO_TEST(test5);
  DO_TEST(test6);
  DO_TEST(test7);

  return 0;
}
CATCH
