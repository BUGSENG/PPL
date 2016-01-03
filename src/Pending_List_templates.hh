/* Pending_List class implementation.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2016 BUGSENG srl (http://bugseng.com)

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

#ifndef PPL_Pending_List_templates_hh
#define PPL_Pending_List_templates_hh 1

#include <iostream>

namespace Parma_Polyhedra_Library {

namespace Implementation {

namespace Watchdog {

template <typename Traits>
typename Pending_List<Traits>::iterator
Pending_List<Traits>::insert(const typename Traits::Threshold& deadline,
                             const Handler& handler,
                             bool& expired_flag) {
  iterator position = active_list.begin();
  for (iterator active_list_end = active_list.end();
       position != active_list_end
         && Traits::less_than(position->deadline(), deadline);
       ++position) {
  }
  iterator pending_element_p;
  // Only allocate a new element if the free list is empty.
  if (free_list.empty()) {
    pending_element_p
      = new Pending_Element<typename Traits::Threshold>(deadline,
                                                        handler,
                                                        expired_flag);
  }
  else {
    pending_element_p = free_list.begin();
    free_list.erase(pending_element_p);
    pending_element_p->assign(deadline, handler, expired_flag);
  }
  iterator r = active_list.insert(position, *pending_element_p);
  assert(OK());
  return r;
}

template <typename Traits>
bool
Pending_List<Traits>::OK() const {
  if (!active_list.OK()) {
    return false;
  }
  if (!free_list.OK()) {
    return false;
  }

  const typename Traits::Threshold* old;
  const_iterator i = active_list.begin();
  old = &i->deadline();
  ++i;
  for (const_iterator active_list_end = active_list.end(); i != active_list_end; ++i) {
    const typename Traits::Threshold& t = i->deadline();
    if (Traits::less_than(t, *old)) {
#ifndef NDEBUG
      std::cerr << "The active list is not sorted!"
                << std::endl;
#endif
      return false;
    }
    old = &t;
  }
  return true;
}

} // namespace Watchdog

} // namespace Implementation

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Pending_List_templates_hh)
