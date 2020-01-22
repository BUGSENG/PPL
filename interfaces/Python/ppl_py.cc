// -*- compile-command: "g++ -c -fPIC -I/usr/include/python2.6 -W -Wall ppl_py.cc -lboost_python -lpython2.6 ; g++ -shared -o ppl.so ppl_py.o -lboost_python -lppl" -*-

/* Proof of concept for a Python interface.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2020 BUGSENG srl (http://bugseng.com)

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

#include <boost/python.hpp>
using namespace boost::python;

#include <ppl.hh>
using namespace Parma_Polyhedra_Library;

namespace Parma_Polyhedra_Library {
using IO_Operators::operator<<;

}

BOOST_PYTHON_MODULE(ppl)
{
  class_<Variable>("Variable", init<dimension_type>())
    .def("id", &Variable::id)
    .def(self_ns::str(self))
    ;
}


