/* Test Ask_Tell<D>.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2019 BUGSENG srl (http://bugseng.com)

This file is part of the Parma Polyhedra Library (PPL).

The PPL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The PPL is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://bugseng.com/products/ppl/ . */

#include "ppl_test.hh"

using namespace IO_Operators;

typedef Ask_Tell<FCAIBVP> DEF_Formula;

namespace {

void
shift_rename_add(const DEF_Formula& p,
                 dimension_type offset,
                 DEF_Formula& q) {
  assert(p.OK());
  for (DEF_Formula::const_iterator i = p.begin(),
         p_end = p.end(); i != p_end; ++i)
    q.add_pair(FCAIBVP(i->ask(), offset), FCAIBVP(i->tell(), offset));
  assert(q.OK());
}

void
remove_dimensions(DEF_Formula& f, const Variables_Set& to_remove) {
  f.normalize();
  DEF_Formula g;
  for (DEF_Formula::const_iterator i = f.begin(),
         f_end = f.end(); i != f_end; ++i) {
    DEF_Formula h = f;
    const FCAIBVP& ask = i->ask();
    FCAIBVP projected_ask = ask;
    projected_ask.difference_assign(FCAIBVP(to_remove));
    h.add_pair(FCAIBVP(), projected_ask);
    if (h.definitely_entails(DEF_Formula(FCAIBVP(), ask))) {
      FCAIBVP projected_tell = i->tell();
      projected_tell.difference_assign(FCAIBVP(to_remove));
      g.add_pair(projected_ask, projected_tell);
    }
  }
  using std::swap;
  swap(f, g);
}

} // namespace

namespace {

bool test01() {
  Variable X(0);
  Variable Y(1);
  Variable Z(2);

  FCAIBVP XY(X);
  XY.meet_assign(FCAIBVP(Y));
  FCAIBVP XZ(X);
  XZ.meet_assign(FCAIBVP(Z));
  FCAIBVP XYZ(XY);
  XYZ.meet_assign(FCAIBVP(Z));

  DEF_Formula a1;
  a1.add_pair(FCAIBVP(), FCAIBVP(Z));
  a1.add_pair(XZ, XYZ);

  DEF_Formula a2;
  a2.add_pair(FCAIBVP(X), XY);

  DEF_Formula a = a1;
  a.upper_bound_assign(a2);

#if 1
  nout << "a1 = " << a1 << endl
       << "a2 = " << a2 << endl
       << "a = " << a << endl;
#endif

  // FIXME
  return true;
}

bool
test02() {
  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);
  Variable E(4);
  Variable F(5);
  Variable G(6);

  // This is the base case:
  // append(A,B,C) :- A = [], B = C.
  DEF_Formula base;
  base.add_pair(FCAIBVP(), FCAIBVP(A));
  base.add_pair(FCAIBVP(B), FCAIBVP(C));
  base.add_pair(FCAIBVP(C), FCAIBVP(B));

  //nout << "*** base ***" << endl << base << endl;

  // This is the inductive case:
  // append(A,B,C) :- A = [D|E], B = F, C = [D|G], append(E,F,G).
  DEF_Formula inductive;
  FCAIBVP D_E(D);
  D_E.meet_assign(FCAIBVP(E));
  inductive.add_pair(FCAIBVP(A), D_E);
  inductive.add_pair(D_E, FCAIBVP(A));
  inductive.add_pair(FCAIBVP(B), FCAIBVP(F));
  inductive.add_pair(FCAIBVP(F), FCAIBVP(B));
  FCAIBVP D_G(D);
  D_G.meet_assign(FCAIBVP(G));
  inductive.add_pair(FCAIBVP(C), D_G);
  inductive.add_pair(D_G, FCAIBVP(C));

  //nout << "*** inductive ***" << endl << inductive << endl;

  // Initialize the fixpoint iteration.
  DEF_Formula current = base;

  //nout << "*** start ***" << endl << current << endl;

  // Contains the polyhedron computed at the previous iteration.
  DEF_Formula previous;
  do {
    previous = current;
    current = inductive;
    shift_rename_add(previous, 4, current);

    //nout << "*** after shift_rename_add ***" << endl << current << endl;

    Variables_Set dimensions_to_remove;
    // Deliberately inserted out of order (!).
    dimensions_to_remove.insert(D);
    dimensions_to_remove.insert(F);
    dimensions_to_remove.insert(E);
    dimensions_to_remove.insert(G);
    remove_dimensions(current, dimensions_to_remove);

    //nout << "*** after remove_dimensions ***" << endl << current << endl;

    current.upper_bound_assign(previous);

    //nout << "*** after upper_bound_assign ***" << endl << current << endl;

  } while (current != previous);

  DEF_Formula expected;
  FCAIBVP A_B(A);
  A_B.meet_assign(FCAIBVP(B));
  expected.add_pair(FCAIBVP(C), A_B);
  expected.add_pair(A_B, FCAIBVP(C));

  //nout << "*** expected ***" << endl << expected << endl;

  return current == expected;
}

bool
test03() {
  typedef Pointset_Ask_Tell<C_Polyhedron> AT;

  Variable A(0);
  Variable B(1);
  Variable C(2);
  Variable D(3);
  Variable E(4);
  Variable F(5);

  // This is the base case:
  // append(A,B,C) :- A = [], B = C.
  C_Polyhedron base_ask(3);
  base_ask.add_constraint(A == 0);
  C_Polyhedron base_tell(3);
  base_tell.add_constraint(B >= 0);
  base_tell.add_constraint(C == B);
  AT base(3);
  base.add_pair(base_ask, base_tell);

  print_constraints(base, "*** base ***");

  // This is the inductive case:
  // append(A,B,C) :- A = [X|D], B = E, C = [X|F], append(D,E,F).
  C_Polyhedron inductive_ask(6);
  inductive_ask.add_constraint(A >= 1);
  C_Polyhedron inductive_tell(6);
  inductive_tell.add_constraint(A + F == C + D);
  inductive_tell.add_constraint(B == E);
  inductive_tell.add_constraint(C + D >= A);
  inductive_tell.add_constraint(D >= 0);
  inductive_tell.add_constraint(B >= 0);
  inductive_tell.add_constraint(A >= D + 1);
  AT inductive(6);
  inductive.add_pair(inductive_ask, inductive_tell);

  print_constraints(inductive, "*** inductive ***");

  // Initialize the fixpoint iteration.
  AT current = base;

  print_constraints(current, "*** start ***");

  // Contains the polyhedron computed at the previous iteration.
  AT previous;
  do {
    previous = current;
    current = inductive;

    //shift_rename_add(previous, 3, current);
    {
      AT r(3, UNIVERSE);
      r.concatenate_assign(previous);
      current.meet_assign(r);
    }

    print_constraints(current, "*** after shift_rename_add ***");

    Variables_Set dimensions_to_remove(D, F);
    current.remove_space_dimensions(dimensions_to_remove);

    current.normalize();
    print_constraints(current, "*** after remove_space_dimensions ***");

    current.upper_bound_assign(previous);

    print_constraints(current, "*** after upper_bound_assign ***");

  } while (current != previous);

#if 0
  C_Polyhedron expected(3);
  expected.add_constraint(A + B == C);
  expected.add_constraint(B >= 0);
  expected.add_constraint(C >= B);

  print_constraints(expected, "*** expected ***");

  return current == expected ? 0 : 1;
#endif
  return true;
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
  DO_TEST(test03);
END_MAIN
