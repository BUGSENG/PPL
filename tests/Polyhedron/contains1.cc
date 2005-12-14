/* Test Polyhedron::contains(const Polyhedron&).
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
  set_handlers();

  Variable x(0);
  Variable y(1);

  C_Polyhedron segment(2);
  segment.add_constraint(x >= 0);
  segment.add_constraint(x <= 1);
  segment.add_constraint(y == 0);

  print_constraints(segment, "*** segment constraints ***");
  print_generators(segment, "*** segment generators ***");

  C_Polyhedron halfline(2);
  halfline.add_constraint(x >= 0);
  halfline.add_constraint(y == 0);

  print_constraints(halfline, "*** halfline constraints ***");
  print_generators(halfline, "*** halfline generators ***");

  bool segment_includes_halfline = segment.contains(halfline);

  nout << "segment ";
  if (segment_includes_halfline)
    nout << "includes ";
  else
    nout << "does not include ";
  nout << "or is equal to halfline" << endl;

  bool halfline_includes_segment = halfline.contains(segment);

  nout << "halfline ";
  if (halfline_includes_segment)
    nout << "includes ";
  else
    nout << "does not include ";
  nout << "or is equal to segment" << endl;

  return (halfline_includes_segment && !segment_includes_halfline) ? 0 : 1;
}
CATCH
