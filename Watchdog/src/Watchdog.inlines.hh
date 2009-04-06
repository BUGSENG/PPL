/* Watchdog and associated classes' implementation: inline functions.
   Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

This file is part of the Parma Watchdog Library (PWL).

The PWL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The PWL is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#ifndef PWL_Watchdog_inlines_hh
#define PWL_Watchdog_inlines_hh 1

#include <stdexcept>

#include "Handler.defs.hh"

namespace Parma_Watchdog_Library {

inline void
Watchdog::reschedule() {
  set_timer(reschedule_time);
}

template <typename Flag_Base, typename Flag>
Watchdog::Watchdog(int units, const Flag_Base* volatile& holder, Flag& flag)
  : expired(false),
    handler(*new Handler_Flag<Flag_Base, Flag>(holder, flag)) {
  if (units <= 0)
    throw std::invalid_argument("Watchdog constructor called with a"
				" non-positive number of time units");
  in_critical_section = true;
  pending_position = new_watchdog_event(units, handler, expired);
  in_critical_section = false;
}

inline
Watchdog::Watchdog(int units, void (*function)())
  : expired(false), handler(*new Handler_Function(function)) {
  if (units <= 0)
    throw std::invalid_argument("Watchdog constructor called with a"
				" non-positive number of time units");
  in_critical_section = true;
  pending_position = new_watchdog_event(units, handler, expired);
  in_critical_section = false;
}

inline
Init::Init() {
  // Only when the first Init object is constructed...
  if (count++ == 0) {
    // ... the library is initialized.
    Watchdog::initialize();
  }
}

inline
Init::~Init() {
  // Only when the last Init object is destroyed...
  if (--count == 0) {
    // ... the library is finalized.
    Watchdog::finalize();
  }
}

} // namespace Parma_Watchdog_Library

#endif // !defined(PWL_Watchdog_inlines_hh)
