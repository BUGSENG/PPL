/* Test Status::ascii_dump() and Status::ascii_load().
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
#include "files.hh"
#include <fstream>

using std::string;
using std::fstream;
using std::ios_base;

namespace {

const char* my_file = "ascii_dump_load6.dat";

void
test1() {

  nout << "test1()" << endl;

  C_Polyhedron ph1;

  fstream f;
  open(f, my_file, ios_base::out);
  ph1.ascii_dump(f);
  close(f);

  open(f, my_file, ios_base::in);
  C_Polyhedron ph2;
  ph2.ascii_load(f);
  close(f);

  bool ok = (ph1 == ph2);

  if (!ok)
    exit(1);
}

void
test2() {

  nout << "test2()" << endl;

  C_Polyhedron ph1(2, EMPTY);

  fstream f;
  open(f, my_file, ios_base::out);
  ph1.ascii_dump(f);
  close(f);

  open(f, my_file, ios_base::in);
  C_Polyhedron ph2;
  ph2.ascii_load(f);
  close(f);

  bool ok = (ph1 == ph2);

  if (!ok)
    exit(1);
}

void
test3() {

  nout << "test3()" << endl;

  fstream f;
  open(f, my_file, ios_base::out);
  f << "space_dim 2\n"
    << "-ZE";
  close(f);

  open(f, my_file, ios_base::in);
  C_Polyhedron ph2;
  bool ok =! ph2.ascii_load(f);
  close(f);

  if (!ok)
    exit(1);
}

void
test4() {

  nout << "test4()" << endl;

  fstream f;
  open(f, my_file, ios_base::out);
  f << "space_dim 2\n"
    << "-ZE -EM";
  close(f);

  open(f, my_file, ios_base::in);
  C_Polyhedron ph2;
  bool ok =! ph2.ascii_load(f);
  close(f);

  if (!ok)
    exit(1);
}

void
test5() {

  nout << "test5()" << endl;

  fstream f;
  open(f, my_file, ios_base::out);
  f << "space_dim 2\n"
    << "-ZE -EM  +CM";
  close(f);

  open(f, my_file, ios_base::in);
  C_Polyhedron ph2;
  bool ok =! ph2.ascii_load(f);
  close(f);

  if (!ok)
    exit(1);
}

void
test6() {

  nout << "test6()" << endl;

  fstream f;
  open(f, my_file, ios_base::out);
  f << "space_dim 2\n"
    << "-ZE -EM  +CM +GM";
  close(f);

  open(f, my_file, ios_base::in);
  C_Polyhedron ph2;
  bool ok =! ph2.ascii_load(f);
  close(f);

  if (!ok)
    exit(1);
}

void
test7() {

  nout << "test7()" << endl;

  fstream f;
  open(f, my_file, ios_base::out);
  f << "space_dim 2\n"
    << "-ZE -EM  +CM +GM  +CS";
  close(f);

  open(f, my_file, ios_base::in);
  C_Polyhedron ph2;
  bool ok =! ph2.ascii_load(f);
  close(f);

  if (!ok)
    exit(1);
}

void
test8() {

  nout << "test8()" << endl;

  fstream f;
  open(f, my_file, ios_base::out);
  f << "space_dim 2\n"
    << "-ZE -EM  +CM +GM  +CS  +SC";
  close(f);

  open(f, my_file, ios_base::in);
  C_Polyhedron ph2;
  bool ok =! ph2.ascii_load(f);
  close(f);

  if (!ok)
    exit(1);
}

void
test9() {

  nout << "test9()" << endl;

  fstream f;
  open(f, my_file, ios_base::out);
  f << "space_dim 2\n"
    << "-ZE -EM  +CM +GM  +CS  +GS";
  close(f);

  open(f, my_file, ios_base::in);
  C_Polyhedron ph2;
  bool ok =! ph2.ascii_load(f);
  close(f);

  if (!ok)
    exit(1);
}

void
test10() {

  nout << "test10()" << endl;

  fstream f;
  open(f, my_file, ios_base::out);
  f << "space_dim 2\n"
    << "-ZE -EM  +CM +GM  +CS  +GS  -SC";
  close(f);

  open(f, my_file, ios_base::in);
  C_Polyhedron ph2;
  bool ok =! ph2.ascii_load(f);
  close(f);

  if (!ok)
    exit(1);
}

} // namespace

int
main() TRY {
  set_handlers();

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

  return 0;
}
CATCH
