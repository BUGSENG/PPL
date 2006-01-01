/* Declarations and inline functions for class PFunction.
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

#ifndef PPL_PFunction_hh
#define PPL_PFunction_hh 1

#include <map>
#include <iosfwd>

class PFunction {
private:
  typedef Parma_Polyhedra_Library::dimension_type dim_t;

public:
  PFunction();
  bool has_empty_codomain() const;
  dim_t max_in_codomain() const;
  bool maps(dim_t i, dim_t& j) const;

  void print(std::ostream& s) const;

  void insert(dim_t x, dim_t y);


private:
  typedef std::map<unsigned, unsigned, std::less<dim_t> > Map;
  Map map;
  dim_t max;
};

inline
PFunction::PFunction()
  : max(0) {
}

inline bool
PFunction::has_empty_codomain() const {
  return map.empty();
}

#endif // !defined(PPL_PFunction_hh)
