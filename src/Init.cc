/* Init class implementation (non-inline functions and static variables).
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

#include <config.h>

#include "Init.defs.hh"
#include "Variable.defs.hh"
#include "fpu.defs.hh"
#include "checked.defs.hh"

namespace PPL = Parma_Polyhedra_Library;

unsigned int PPL::Init::count = 0;

PPL::fpu_rounding_direction_type PPL::Init::old_rounding_direction;

extern "C" void
set_GMP_memory_allocation_functions(void)
#if CXX_SUPPORTS_ATTRIBUTE_WEAK
  __attribute__((weak));

void
set_GMP_memory_allocation_functions(void) {
}
#else
  ;
#endif

PPL::Init::Init() {
  // Only when the first Init object is constructed...
  if (count++ == 0) {
    // ... the GMP memory allocation functions are set, ...
    set_GMP_memory_allocation_functions();
    // ... and the default output function for Variable objects is set.
    Variable::set_output_function(Variable::default_output_function);
#if PPL_CAN_CONTROL_FPU
    old_rounding_direction = fpu_get_rounding_direction();
    fpu_set_rounding_direction(ROUND_DIRECT);
#endif
    set_rational_sqrt_precision_parameter(0);
  }
}

PPL::Init::~Init() {
  // Only when the last Init object is destroyed...
  if (--count == 0) {
#if PPL_CAN_CONTROL_FPU
    fpu_set_rounding_direction(old_rounding_direction);
#endif
  }
}
