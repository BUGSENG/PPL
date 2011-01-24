/* Test BD_Shape::max_space_dimension().
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2011 BUGSENG srl (http://bugseng.com)

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
site: http://www.cs.unipr.it/ppl/ . */

#include "ppl_test.hh"

namespace {

bool
test01() {
  nout << BD_Shape<signed char>::max_space_dimension() << " (signed char)"
       << endl
       << BD_Shape<short>::max_space_dimension() << " (short)"
       << endl
       << BD_Shape<int>::max_space_dimension() << " (int)"
       << endl
       << BD_Shape<long>::max_space_dimension() << " (long)"
       << endl
       << BD_Shape<long long>::max_space_dimension() << " (long long)"
       << endl
#if PPL_SUPPORTED_FLOAT
       << BD_Shape<float>::max_space_dimension() << " (float)"
       << endl
#endif
#if PPL_SUPPORTED_DOUBLE
       << BD_Shape<double>::max_space_dimension() << " (double)"
       << endl
#endif
#if PPL_SUPPORTED_LONG_DOUBLE
       << BD_Shape<long double>::max_space_dimension() << " (long double)"
       << endl
#endif
       << BD_Shape<mpz_class>::max_space_dimension() << " (mpz_class)"
       << endl
       << BD_Shape<mpq_class>::max_space_dimension() << " (mpq_class)"
       << endl;

  if (BD_Shape<signed char>::max_space_dimension()
      < BD_Shape<short>::max_space_dimension())
    return false;

  if (BD_Shape<short>::max_space_dimension()
      < BD_Shape<int>::max_space_dimension())
    return false;

  if (BD_Shape<int>::max_space_dimension()
      < BD_Shape<long>::max_space_dimension())
    return false;

  if (BD_Shape<long>::max_space_dimension()
      < BD_Shape<long long>::max_space_dimension())
    return false;

#if PPL_SUPPORTED_FLOAT && PPL_SUPPORTED_DOUBLE
  if (BD_Shape<float>::max_space_dimension()
      < BD_Shape<double>::max_space_dimension())
    return false;
#endif

#if PPL_SUPPORTED_DOUBLE && PPL_SUPPORTED_LONG_DOUBLE
  if (BD_Shape<double>::max_space_dimension()
      < BD_Shape<long double>::max_space_dimension())
    return false;
#endif

  if (2*BD_Shape<mpz_class>::max_space_dimension()
      < BD_Shape<mpq_class>::max_space_dimension())
    return false;

  return true;
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
END_MAIN
