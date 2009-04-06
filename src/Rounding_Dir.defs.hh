/* Declaration of Rounding_Dir and related functions.
   Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

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

#ifndef PPL_Rounding_defs_hh
#define PPL_Rounding_defs_hh 1

#include "Result.defs.hh"
#include "fpu.defs.hh"

namespace Parma_Polyhedra_Library {

//! Rounding directions for arithmetic computations.
/*! \ingroup PPL_CXX_interface */
enum Rounding_Dir {
  /*! \hideinitializer
    Round toward \f$-\infty\f$.
  */
  ROUND_DOWN = 0,

  /*! \hideinitializer
    Round toward \f$+\infty\f$.
  */
  ROUND_UP = 1,

  /*! \hideinitializer
    Rounding is delegated to lower level. Result info is evaluated lazily.
  */
  ROUND_IGNORE = 6,
  ROUND_NATIVE = ROUND_IGNORE,

  /*! \hideinitializer
    Rounding is not needed: client code must ensure the operation is exact.
  */
  ROUND_NOT_NEEDED = 7,

  ROUND_DIRECT = ROUND_UP,
  ROUND_INVERSE = ROUND_DOWN,

  ROUND_DIR_MASK = 7,

  ROUND_FPU_CHECK_INEXACT = 8,

  ROUND_CHECK = ROUND_DIRECT | ROUND_FPU_CHECK_INEXACT
};

/*! \brief
  Returns the inverse rounding mode of \p dir,
  <CODE>ROUND_IGNORE</CODE> being the inverse of itself.
*/
Rounding_Dir inverse(Rounding_Dir dir);

Rounding_Dir round_dir(Rounding_Dir dir);
bool round_down(Rounding_Dir dir);
bool round_up(Rounding_Dir dir);
bool round_ignore(Rounding_Dir dir);
bool round_direct(Rounding_Dir dir);
bool round_inverse(Rounding_Dir dir);

bool round_fpu_check_inexact(Rounding_Dir dir);

fpu_rounding_direction_type round_fpu_dir(Rounding_Dir dir);

} // namespace Parma_Polyhedra_Library

#include "Rounding_Dir.inlines.hh"

#endif // !defined(PPL_Float_defs_hh)

