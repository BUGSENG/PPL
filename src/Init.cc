/* Init class implementation (non-inline functions and static variables).
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2022 BUGSENG srl (http://bugseng.com)

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

#include "ppl-config.h"
#include "Init_defs.hh"
#include "Variable_defs.hh"
#include "fpu_defs.hh"
#include "Rounding_Dir_defs.hh"
#include "checked_defs.hh"
#include "Coefficient_defs.hh"
#include "Linear_Expression_defs.hh"
#include "Constraint_defs.hh"
#include "Generator_defs.hh"
#include "Congruence_defs.hh"
#include "Grid_Generator_defs.hh"
#include "Constraint_System_defs.hh"
#include "Generator_System_defs.hh"
#include "Congruence_System_defs.hh"
#include "Grid_Generator_System_defs.hh"
#include "Polyhedron_defs.hh"
#include "Watchdog_defs.hh"
#include <stdexcept>

namespace PPL = Parma_Polyhedra_Library;

PPL::Init::counter_type PPL::Init::count;

PPL_TLS PPL::fpu_rounding_direction_type
PPL::Thread_Init::old_rounding_direction;

extern "C" void
ppl_set_GMP_memory_allocation_functions(void);

#if PPL_CAN_CONTROL_FPU \
  && defined(PPL_ARM_CAN_CONTROL_FPU) && PPL_ARM_CAN_CONTROL_FPU

namespace {

const float nf1 = -3.0F;
const float pf1 = 3.0F;
const float f2 = 5.0F;
const double nd1 = -7.0;
const double pd1 = 7.0;
const double d2 = 11.0;
const long double nl1 = -13.0L;
const long double pl1 = 13.0L;
const long double l2 = 17.0L;

PPL_TLS float nf[2];
PPL_TLS float pf[2];
PPL_TLS double nd[2];
PPL_TLS double pd[2];
PPL_TLS long double nl[2];
PPL_TLS long double pl[2];

int
ppl_check_function() {
  int r = 0;
  if (nf[0] == nf[1] || pf[0] == pf[1] || -nf[0] != pf[1] || -nf[1] != pf[0]) {
    r |= 1;
  }
  if (nd[0] == nd[1] || pd[0] == pd[1] || -nd[0] != pd[1] || -nd[1] != pd[0]) {
    r |= 2;
  }
  if (nl[0] == nl[1] || pl[0] == pl[1] || -nl[0] != pl[1] || -nl[1] != pl[0]) {
    r |= 4;
  }
  return r;
}

int
ppl_setround_function(int rounding_mode) {
  return fesetround(rounding_mode);
}

} // namespace

namespace Parma_Polyhedra_Library {

namespace Implementation {

PPL_TLS int (* volatile ppl_check_function_p)() = ppl_check_function;
PPL_TLS int (* volatile ppl_setround_function_p)(int) = ppl_setround_function;

} // namespace Implementation

} // namespace Parma_Polyhedra_Library

namespace {

int
ppl_test_rounding() {
  if ((*ppl_setround_function_p)(FE_DOWNWARD) != 0) {
    return 255;
  }

  nf[0] = nf1 / f2;
  nd[0] = nd1 / d2;
  nl[0] = nl1 / l2;
  pf[0] = pf1 / f2;
  pd[0] = pd1 / d2;
  pl[0] = pl1 / l2;

  if ((*ppl_setround_function_p)(FE_UPWARD) != 0) {
    return 255;
  }

  nf[1] = nf1 / f2;
  nd[1] = nd1 / d2;
  nl[1] = nl1 / l2;
  pf[1] = pf1 / f2;
  pd[1] = pd1 / d2;
  pl[1] = pl1 / l2;

  return (*ppl_check_function_p)();
}

} // namespace

#endif // PPL_CAN_CONTROL_FPU
       // && defined(PPL_ARM_CAN_CONTROL_FPU) && PPL_ARM_CAN_CONTROL_FPU


namespace Parma_Polyhedra_Library {

namespace Implementation {

namespace {

static Parma_Polyhedra_Library::Init*
Parma_Polyhedra_Library_initializer_p;

static PPL_TLS Parma_Polyhedra_Library::Thread_Init*
Parma_Polyhedra_Library_thread_initializer_p;

} // namespace

void
initialize_aux() {
  if (Parma_Polyhedra_Library_initializer_p == nullptr) {
    Parma_Polyhedra_Library_initializer_p = new Init();
  }
}

void
finalize_aux() {
  PPL_ASSERT(Parma_Polyhedra_Library_initializer_p != nullptr);
  delete Parma_Polyhedra_Library_initializer_p;
  Parma_Polyhedra_Library_initializer_p = nullptr;
}

void
thread_initialize_aux() {
  if (Parma_Polyhedra_Library_thread_initializer_p == nullptr) {
    Parma_Polyhedra_Library_thread_initializer_p = new Thread_Init();
  }
}

void
thread_finalize_aux() {
  PPL_ASSERT(Parma_Polyhedra_Library_thread_initializer_p != nullptr);
  delete Parma_Polyhedra_Library_thread_initializer_p;
  Parma_Polyhedra_Library_thread_initializer_p = nullptr;
}

} // namespace Implementation

} // namespace Parma_Polyhedra_Library

PPL::Init::Init() {
  // Only when the first Init object is constructed...
  if (count++ == 0) {
    // ... the GMP memory allocation functions are set, ...
    ppl_set_GMP_memory_allocation_functions();

#if PPL_HAVE_DECL_SETITIMER && PPL_HAVE_DECL_SIGACTION
    // FIXME: current implementation is not thread-safe.
    // ... the Watchdog subsystem is initialized, ...
    Watchdog::Watchdog::initialize();
#endif // PPL_HAVE_DECL_SETITIMER && PPL_HAVE_DECL_SIGACTION

    // Perform thread-specific initialization.
    Implementation::thread_initialize_aux();
  }
}

PPL::Init::~Init() {
  // Only when the last Init object is destroyed...
  if (--count == 0) {
    // Perform thread-specific finalization.
    Implementation::thread_finalize_aux();

#if PPL_HAVE_DECL_SETITIMER && PPL_HAVE_DECL_SIGACTION
    // FIXME: current implementation is not thread-safe.
    // ... the Watchdog subsystem is finalized, ...
    Watchdog::Watchdog::finalize();
#endif // PPL_HAVE_DECL_SETITIMER && PPL_HAVE_DECL_SIGACTION

  }
}

PPL::Thread_Init::Thread_Init() {
  // ... the default output function for Variable objects is set, ...
  Variable::set_output_function(&Variable::default_output_function);
  // ... the Coefficient constants are initialized, ...
  Coefficient_constants_initialize();
  // ... the Linear_Expression class is initialized, ...
  Linear_Expression::initialize();
  // ... the Constraint, Generator, Congruence, Grid_Generator,
  // Constraint_System, Generator_System, Congruence_System,
  // Grid_Generator_System and Polyhedron classes are initialized, ...
  Constraint::initialize();
  Generator::initialize();
  Congruence::initialize();
  Grid_Generator::initialize();
  Constraint_System::initialize();
  Generator_System::initialize();
  Congruence_System::initialize();
  Grid_Generator_System::initialize();
  Polyhedron::initialize();

#if PPL_CAN_CONTROL_FPU

  // ... and the FPU rounding direction is set.
  fpu_initialize_control_functions();
  old_rounding_direction = fpu_get_rounding_direction();
  fpu_set_rounding_direction(round_fpu_dir(ROUND_DIRECT));

#if defined(PPL_ARM_CAN_CONTROL_FPU) && PPL_ARM_CAN_CONTROL_FPU
  if (ppl_test_rounding() != 0) {
    throw std::logic_error("PPL configuration error:"
                           " PPL_ARM_CAN_CONTROL_FPU evaluates to true,"
                           " but rounding does not work.");
  }
#endif // defined(PPL_ARM_CAN_CONTROL_FPU) && PPL_ARM_CAN_CONTROL_FPU

#endif // PPL_CAN_CONTROL_FPU

  // The default is chosen to have a precision greater than most
  // precise IEC 559 floating point (112 bits of mantissa).
  set_irrational_precision(DEFAULT_IRRATIONAL_PRECISION);
}

PPL::Thread_Init::~Thread_Init() {
#if PPL_CAN_CONTROL_FPU
  // ... the FPU rounding direction is restored, ...
  fpu_set_rounding_direction(old_rounding_direction);
#endif

  // ... the Polyhedron, Grid_Generator_System, Congruence_System,
  // Generator_System, Constraint_System, Grid_Generator,
  // Congruence, Generator and Constraint classes are finalized
  // IN THAT ORDER, ...
  Polyhedron::finalize();
  Grid_Generator_System::finalize();
  Congruence_System::finalize();
  Generator_System::finalize();
  Constraint_System::finalize();
  Grid_Generator::finalize();
  Congruence::finalize();
  Generator::finalize();
  Constraint::finalize();
  // ... the Linear_Expression class is finalized, ...
  Linear_Expression::finalize();
  // ... and the Coefficient constants are finalized.
  Coefficient_constants_finalize();
}
