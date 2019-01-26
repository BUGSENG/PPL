/* Nifty counter object for the initialization of the library.
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

#ifndef PPL_initializer_hh
#define PPL_initializer_hh 1

#include "Init_defs.hh"

#ifndef PPL_NO_AUTOMATIC_INITIALIZATION

static Parma_Polyhedra_Library::Init Parma_Polyhedra_Library_initializer;

#else

namespace Parma_Polyhedra_Library {

namespace Implementation {

void initialize_aux();
void finalize_aux();

void thread_initialize_aux();
void thread_finalize_aux();

} // namespace Implementation

} // namespace Parma_Polyhedra_Library

#endif

namespace Parma_Polyhedra_Library {

//! Initializes the library.
inline void
initialize() {
#ifdef PPL_NO_AUTOMATIC_INITIALIZATION
  Implementation::initialize_aux();
#endif
}

//! Finalizes the library.
inline void
finalize() {
#ifdef PPL_NO_AUTOMATIC_INITIALIZATION
  Implementation::finalize_aux();
#endif
}

//! Initializes a thread using the library.
inline void
thread_initialize() {
#ifdef PPL_NO_AUTOMATIC_INITIALIZATION
  Implementation::thread_initialize_aux();
#endif
}

//! Finalizes a thread using the library.
inline void
thread_finalize() {
#ifdef PPL_NO_AUTOMATIC_INITIALIZATION
  Implementation::thread_finalize_aux();
#endif
}

} //namespace Parma_Polyhedra_Library

#endif // !defined(PPL_initializer_hh)
