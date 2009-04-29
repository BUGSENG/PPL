/* EList class implementation: inline functions.
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

#ifndef PWL_EList_inlines_hh
#define PWL_EList_inlines_hh 1

namespace Parma_Watchdog_Library {

template <typename T>
inline
EList<T>::EList()
  : Doubly_Linked_Object(this, this) {
}

template <typename T>
inline void
EList<T>::push_front(T& obj) {
  next->insert_before(obj);
}

template <typename T>
inline void
EList<T>::push_back(T& obj) {
  prev->insert_after(obj);
}

template <typename T>
inline typename EList<T>::Iterator
EList<T>::insert(Iterator position, T& obj) {
  position->insert_before(obj);
  return Iterator(&obj);
}

template <typename T>
inline typename EList<T>::Iterator
EList<T>::begin() {
  return Iterator(next);
}

template <typename T>
inline typename EList<T>::Iterator
EList<T>::end() {
  return Iterator(this);
}

template <typename T>
inline typename EList<T>::Const_Iterator
EList<T>::begin() const {
  return Const_Iterator(next);
}

template <typename T>
inline typename EList<T>::Const_Iterator
EList<T>::end() const {
  return Const_Iterator(const_cast<EList<T>*>(this));
}

template <typename T>
inline bool
EList<T>::empty() const {
  return begin() == end();
}

template <typename T>
inline typename EList<T>::Iterator
EList<T>::erase(Iterator position) {
  assert(!empty());
  return Iterator(position->erase());
}

template <typename T>
inline
EList<T>::~EList() {
  // Erase and deallocate all the elements.
  for (Iterator i = begin(), lend = end(), next; i != lend; i = next) {
    next = erase(i);
    delete &*i;
  }
}

template <typename T>
inline bool
EList<T>::OK() const {
  for (Const_Iterator i = begin(), lend = end(); i != lend; ++i)
    if (!i->OK())
      return false;

  return true;
}

} // namespace Parma_Watchdog_Library

#endif // !defined(PWL_EList_inlines_hh)
