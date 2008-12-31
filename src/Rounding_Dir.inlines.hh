/* Inline functions operating on enum Rounding_Dir values.
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

#ifndef PPL_Rounding_Dir_inlines_hh
#define PPL_Rounding_Dir_inlines_hh 1

#include <cassert>

namespace Parma_Polyhedra_Library {

inline Rounding_Dir
round_dir(Rounding_Dir dir) {
  return static_cast<Rounding_Dir>(dir & ROUND_DIR_MASK);
}

inline bool
round_down(Rounding_Dir dir) {
  return round_dir(dir) == ROUND_DOWN;
}

inline bool
round_up(Rounding_Dir dir) {
  return round_dir(dir) == ROUND_UP;
}

inline bool
round_ignore(Rounding_Dir dir) {
  return round_dir(dir) == ROUND_IGNORE;
}

inline bool
round_direct(Rounding_Dir dir) {
  return round_dir(dir) == ROUND_DIRECT;
}

inline bool
round_inverse(Rounding_Dir dir) {
  return round_dir(dir) == ROUND_INVERSE;
}

inline bool
round_fpu_check_inexact(Rounding_Dir dir) {
  return dir & ROUND_FPU_CHECK_INEXACT;
}

#if PPL_CAN_CONTROL_FPU

inline fpu_rounding_direction_type
round_fpu_dir(Rounding_Dir dir) {
  switch (round_dir(dir)) {
  case ROUND_UP:
    return static_cast<fpu_rounding_direction_type>(PPL_FPU_UPWARD);
  case ROUND_DOWN:
    return static_cast<fpu_rounding_direction_type>(PPL_FPU_DOWNWARD);
  default:
    assert(false);
    return static_cast<fpu_rounding_direction_type>(PPL_FPU_UPWARD);
  }
}

#undef PPL_FPU_DOWNWARD
#undef PPL_FPU_TONEAREST
#undef PPL_FPU_TOWARDZERO
#undef PPL_FPU_UPWARD

#endif

/*! \relates Parma_Polyhedra_Library::Rounding_Dir */
inline Rounding_Dir
inverse(Rounding_Dir dir) {
  Rounding_Dir d = round_dir(dir);
  switch (d) {
  case ROUND_UP:
    d = ROUND_DOWN;
    break;
  case ROUND_DOWN:
    d = ROUND_UP;
    break;
  default:
    assert(false);
    /* Fall through */
  case ROUND_IGNORE:
    return dir;
  }
  return static_cast<Rounding_Dir>((dir & ~ROUND_DIR_MASK) | d);
}

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Rounding_Dir_inlines_hh)
