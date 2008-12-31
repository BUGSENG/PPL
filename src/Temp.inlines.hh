/* Temp_* classes implementation: inline functions.
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

#ifndef PPL_Temp_inlines_hh
#define PPL_Temp_inlines_hh 1

#include "meta_programming.hh"

namespace Parma_Polyhedra_Library {

template <typename T>
inline
Temp_Item<T>::Temp_Item()
  : item_() {
}

template <typename T>
inline T&
Temp_Item<T>::item() {
    return item_;
}

template <typename T>
inline Temp_Item<T>&
Temp_Item<T>::obtain() {
  if (free_list_head != 0) {
    Temp_Item* p = free_list_head;
    free_list_head = free_list_head->next;
    return *p;
  }
  else
    return *new Temp_Item();
}

template <typename T>
inline void
Temp_Item<T>::release(Temp_Item& p) {
  p.next = free_list_head;
  free_list_head = &p;
}

template <typename T>
inline
Temp_Reference_Holder<T>::Temp_Reference_Holder(Temp_Item<T>& p)
  : held(p) {
}

template <typename T>
inline
Temp_Reference_Holder<T>::~Temp_Reference_Holder() {
  Temp_Item<T>::release(held);
}

template <typename T>
inline T&
Temp_Reference_Holder<T>::item() {
  return held.item();
}

template <typename T>
inline
Temp_Value_Holder<T>::Temp_Value_Holder() {
}

template <typename T>
inline T
Temp_Value_Holder<T>::item() {
  return item_;
}

template <typename T>
inline typename Temp_From_Free_List<T>::holder_type
Temp_From_Free_List<T>::obtain_holder() {
  return Temp_Reference_Holder<T>(Temp_Item<T>::obtain());
}

template <typename T>
inline typename Temp_From_Local_Variable<T>::holder_type
Temp_From_Local_Variable<T>::obtain_holder() {
  return Temp_Value_Holder<T>();
}

} // namespace Parma_Polyhedra_Library

#define PPL_DIRTY_TEMP(T, id)						\
  typename Parma_Polyhedra_Library::Dirty_Temp<T>::holder_type holder ## id = \
    Parma_Polyhedra_Library::Dirty_Temp<T>::obtain_holder();		\
  typename Parma_Polyhedra_Library::Dirty_Temp<T>::type id =		\
    holder ## id.item()

#define PPL_DIRTY_TEMP0(T, id)						\
  Parma_Polyhedra_Library::Dirty_Temp<T>::holder_type holder ## id =	\
    Parma_Polyhedra_Library::Dirty_Temp<T>::obtain_holder();		\
  Parma_Polyhedra_Library::Dirty_Temp<T>::type id = holder ## id.item()

#endif // !defined(PPL_Temp_inlines_hh)
