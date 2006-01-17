/* Init class declaration.
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

#ifndef PPL_Init_defs_hh
#define PPL_Init_defs_hh 1

#include "Init.types.hh"

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Class for initialization and finalization.
/*! \ingroup PPL_CXX_interface
  <EM>Nifty Counter</EM> initialization class,
  ensuring that the library is initialized only once
  and before its first use.
  A count of the number of translation units using the library
  is maintained. A static object of Init type will be declared
  by each translation unit using the library.  As a result,
  only one of them will initialize and properly finalize
  the library.
*/
#endif // PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS

class Parma_Polyhedra_Library::Init {
private:
  //! Count the number of objects created.
  static unsigned int count;
  static int old_rounding_direction;

public:
  //! Initializes the PPL.
  Init();

  //! Finalizes the PPL.
  ~Init();
};

#endif // !defined(PPL_Init_defs_hh)
