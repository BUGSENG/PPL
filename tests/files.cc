/* Definition of simple helper functions to open and close files.
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

#include "files.hh"
#include <fstream>
#include <iostream>
#include <cstdlib>

using namespace std;

void
open(fstream& s, const char* path, ios_base::openmode mode) {
  s.open(path, mode);
  if (!s) {
    cerr << "Cannot open `" << path << "'";
    if (mode == ios_base::in)
      cerr << " for reading";
    else if (mode == ios_base::out)
      cerr << " for writing";
    else if (mode == (ios_base::in | ios_base::out))
      cerr << " for reading/writing";
    cerr << endl;
    exit(1);
  }
}

void
close(fstream& s) {
  if (s)
    s.close();
}
