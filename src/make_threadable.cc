/* make_threadable implementation (non-inline functions).
   Copyright (C) 2016-2016 Enea Zaffanella <enea.zaffanella@unipr.it>
   Copyright (C) 2016-2016 BUGSENG srl (http://bugseng.com)

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

#ifdef PPL_THREAD_SAFE

#    include "make_threadable.hh"
#    include "globals_defs.hh"
#    include <mutex>
#    include <map>

namespace Parma_Polyhedra_Library
{

namespace Implementation
{

// A map associating each thread id to the corresponding (thread local)
// abandon_type object. This is populated (by the Abandon_Init RAII helper)
// when an Interruptible object starts/ends execution.
// Note: this map is really meant to be a single global object:
// thus, we provide a mutex to guarantee thread-safe access.
typedef std::map<std::thread::id, abandon_type *> Abandon_Ptr_Map;
Abandon_Ptr_Map abandon_ptrs;
std::mutex      abandon_ptrs_mutex;

Abandon_Init::Abandon_Init ()
{
    // Register the address of the *current* thread (TLS) abandon object.
    std::lock_guard<std::mutex> guard(abandon_ptrs_mutex);

    abandon_ptrs[std::this_thread::get_id()] = &abandon_expensive_computations;
}

Abandon_Init::~Abandon_Init ()
{
    // Unregister the address of the *current* thread (TLS) abandon object.
    std::lock_guard<std::mutex> guard(abandon_ptrs_mutex);

    abandon_ptrs.erase(std::this_thread::get_id());
}

} // namespace Implementation

bool
interrupt_thread (const std::thread     & interruptible_thread,
                  const Throwable * const throwable)
{
    using namespace Implementation;

    std::lock_guard<std::mutex> guard(abandon_ptrs_mutex);
    const std::thread::id       tid  = interruptible_thread.get_id();
    Abandon_Ptr_Map::iterator   iter = abandon_ptrs.find(tid);

    if (iter == abandon_ptrs.end())
    {
        // Either the thread is not (yet) registered in the map,
        // or it has already unregistered.
        return false;
    }

    PPL_ASSERT(iter->second != nullptr);
    *(iter->second) = throwable;
    return true;
}

} // namespace Parma_Polyhedra_Library

#endif // defined(PPL_THREAD_SAFE)

