/* Generator class implementation (non-inline functions).
   Copyright (C) 2001, 2002 Roberto Bagnara <bagnara@cs.unipr.it>

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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#include <config.h>

#include "Generator.defs.hh"

#include "Variable.defs.hh"
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace PPL = Parma_Polyhedra_Library;

void
PPL::Generator::throw_dimension_incompatible(const char* method,
					     Variable v) const {
  std::ostringstream s;
  s << method << ":" << std::endl
    << "this->space_dimension() == " << this->space_dimension()
    << ", v.id() == " << v.id();
  throw std::invalid_argument(s.str());
}

void
PPL::Generator::throw_invalid_argument(const char* method,
				       const char* reason) const {
  std::ostringstream s;
  s << method << ":" << std::endl
    << reason;
  throw std::invalid_argument(s.str());
}

PPL::Generator
PPL::point(const LinExpression& e, const Integer& d) {
  if (d == 0)
    throw std::invalid_argument("Generator PPL::point(e, d): d == 0");
  LinExpression ec = e;
  Generator g(ec);
  g[0] = d;

  // If the divisor is negative, we negate it as well as
  // all the coefficients of the point, because we want to preserve
  // the invariant: the divisor of a point is strictly positive.
  if (d < 0)
    for (size_t i = g.size(); i-- > 0; )
      negate(g[i]);

  g.set_is_ray_or_point();
  return g;
}

PPL::Generator
PPL::closure_point(const LinExpression& e, const Integer& d) {
  if (d == 0)
    throw std::invalid_argument("Generator PPL::closure_point(e, d): d == 0");
  // Adding the \epsilon dimension with coefficient 0.
  LinExpression ec = 0 * Variable(e.space_dimension());
  ec += e;
  // A closure point is indeed a point in the higher dimension space.
  Generator g = point(ec, d);
  // Setting the topology kind.
  g.set_not_necessarily_closed();
  return g;
}

PPL::Generator
PPL::ray(const LinExpression& e) {
  // The origin of the space cannot be a ray.
  if (e.all_homogeneous_terms_are_zero())
    throw std::invalid_argument("PPL::ray(e): the origin cannot be a ray");

  LinExpression ec = e;
  Generator g(ec);
  g[0] = 0;
  g.set_is_ray_or_point();
  return g;
}

PPL::Generator
PPL::line(const LinExpression& e) {
  // The origin of the space cannot be a line.
  if (e.all_homogeneous_terms_are_zero())
    throw std::invalid_argument("PPL::line(e): the origin cannot be a line");

  LinExpression ec = e;
  Generator g(ec);
  g[0] = 0;
  g.set_is_line();
  return g;
}

/*! \relates Parma_Polyhedra_Library::Generator */
std::ostream&
PPL::operator<<(std::ostream& s, const Generator& g) {
  bool needed_divisor = false;
  bool extra_parentheses = false;
  int num_variables = g.space_dimension();
  Generator::Type t = g.type();
  switch (t) {
  case Generator::LINE:
    s << "l(";
    break;
  case Generator::RAY:
    s << "r(";
    break;
  case Generator::POINT:
  case Generator::CLOSURE_POINT:
    if (t == Generator::POINT)
      s << "p(";
    else
      s << "c(";
    if (g[0] != 1) {
      needed_divisor = true;
      int num_non_zero_coefficients = 0;
      for (int v = 0; v < num_variables; ++v)
	if (g[v+1] != 0)
	  if (++num_non_zero_coefficients > 1) {
	    extra_parentheses = true;
	    s << "(";
	    break;
	  }
    }
    break;
  }

  bool first = true;
  for (int v = 0; v < num_variables; ++v) {
    Integer gv = g[v+1];
    if (gv != 0) {
      if (!first) {
	if (gv > 0)
	  s << " + ";
	else {
	  s << " - ";
	  negate(gv);
	}
      }
      else
	first = false;
      if (gv == -1)
	s << "-";
      else if (gv != 1)
	s << gv << "*";
      s << PPL::Variable(v);
    }
  }
  if (first)
    // A point or closure point in the origin.
    s << 0;
  if (extra_parentheses)
    s << ")";
  if (needed_divisor)
    s << "/" << g[0];
  s << ")";
  return s;
}

bool
PPL::Generator::OK() const {
  using std::endl;
  using std::cerr;

  const Generator& g = *this;

  switch (g.type()) {

  case LINE:
  case RAY:
    if (g[0] != 0) {
      cerr << "Lines must have a zero inhomogeneous term!"
	   << endl;
      return false;
    }
    if (!g.is_necessarily_closed() && g[size() - 1] != 0) {
      cerr << "Lines and rays must have a zero coefficient "
	   << "for the epsilon dimension!"
	   << endl;
      return false;
    }
    // The following test is correct, since we already checked
    // that the \epsilon coordinate is zero.
    if (g.all_homogeneous_terms_are_zero()) {
      cerr << "The origin of the vector space cannot be "
	   << "a line or a ray!"
	   << endl;
      return false;
    }
    break;

  case POINT:
    if (g[0] <= 0) {
      cerr << "Points must have a positive divisor!"
	   << endl;
      return false;
    }
    if (!g.is_necessarily_closed())
      if (g[size() - 1] <= 0 || g[size() - 1] > g[0]) {
	cerr << "In the NNC topology, "
	     << "points must have 0 < epsilon <= 1"
	     << endl;
	return false;
      }
    break;

  case CLOSURE_POINT:
    if (g[0] <= 0) {
      cerr << "Closure points must have a positive divisor!"
	   << endl;
      return false;
    }
    break;
  }

  // All tests passed.
  return true;
}
